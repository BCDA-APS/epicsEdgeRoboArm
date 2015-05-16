#include <cstring>
#include <sstream>

#include "hidDriver.h"



/*
 * USB defines endpoint addresses so that if this bit is set it is an
 * input endpoint.
 */
const int DIRECTION_INPUT = 0x80;

/*
 * Many devices don't provide unique serial numbers. So to keep different
 * drivers from connecting to the same device we will keep a record of all
 * devices currently claimed by HID_Drivers
 */
std::list<libusb_device*> claimed;


/*
 * Each of the HID_Drivers will be running their own connect/disconnect
 * and update threads, so we need a lock to keep the integrity of the
 * list of claimed devices.
 */
epicsMutexId mylock = epicsMutexCreate();


/**
 * See if a given device has been claimed by a driver or not
 *
 * @param[in]  check    The usb device that may already be claimed
 *
 * @return  true if there already exists a driver for that device, 
 * false otherwise
 */
bool contains(libusb_device* check)
{	
	for(std::list<libusb_device*>::iterator any_device = claimed.begin();
	    any_device != claimed.end(); ++any_device)
	{
		if(*any_device == check) return true;
	}
	
	return false;
}

void connect_thread_callback(void* arg)
{
	hidDriver* driver = (hidDriver*) arg;
	
	driver->connect_thread();
}



void hidDriver::connect(uint16_t vendor_id, uint16_t product_id, std::string serial, int interface_num)
{
	this->disconnect();
	
	this->VENDOR_ID = vendor_id;
	this->PRODUCT_ID = product_id;
	this->SERIAL_NUM = serial;
	this->INTERFACE = interface_num;
	
	this->connect(); // Spawn connection thread
}


void hidDriver::connect()
{
	std::stringstream temp_stream;
	std::string threadname;
	
	temp_stream << "Driver_Connect (" << VENDOR_ID << ":" << PRODUCT_ID << ")";
	temp_stream >> threadname;
	
	epicsThreadCreate(threadname.c_str(), 
	                  epicsThreadPriorityLow, 
	                  epicsThreadGetStackSize(epicsThreadStackMedium), 
	                  (EPICSTHREADFUNC)::connect_thread_callback, this);
}


void hidDriver::disconnect()
{
	epicsMutexLock(this->device_state);
	if (this->connected)
	{	
		this->printDebug(20, "Disconnecting device\n");
		
		this->setStatuses(asynError);
		
		asynStatus temp;
		
		this->getParamStatus(15, &temp);
		
		printf("STATUS: %d\n", temp);
		
		/* 
		* We don't want to delete our data while our update thread is still reading 
		* through it 
		*/
		epicsMutexLock(this->input_state);
			delete [] this->state;;
			delete [] this->last_state;
			
			this->state = NULL;
			this->last_state = NULL;
		epicsMutexUnlock(this->input_state);
			
		epicsMutexLock(mylock);
			/* We no longer claim exclusive rights to the usb device */
			claimed.remove(libusb_get_device(DEVICE));
			
			this->releaseInterface();
				
			libusb_close(DEVICE);
		epicsMutexUnlock(mylock);
	
		this->connected = false;
		this->DEVICE = NULL;
	}
	epicsMutexUnlock(this->device_state);
}


void hidDriver::loadDeviceInfo()
{
	/*
	 * While most devices should have an endpoint for reading at 0x81, one 
	 * should never assume that. So we'll dig through the libusb nested structs 
	 * to get at the default input endpoint.
	 */
	struct libusb_config_descriptor* config_description;
	
	libusb_get_active_config_descriptor(libusb_get_device(DEVICE), &config_description);
	
	if (this->INTERFACE >= config_description->bNumInterfaces)
	{
		this->printDebug(1, "Interface %d not found.\n", INTERFACE);
		libusb_free_config_descriptor(config_description);
		return;
	}
	
	libusb_interface_descriptor interface = config_description->interface[INTERFACE].altsetting[0];
	
	bool found_input = false;
	bool found_output = false;
	
	for (int index = 0; index < interface.bNumEndpoints; index += 1)
	{
		uint8_t endpoint_info = interface.endpoint[index].bEndpointAddress;
		
		/* Input Endpoint */
		if (not found_input and (endpoint_info & (LIBUSB_ENDPOINT_IN | LIBUSB_TRANSFER_TYPE_INTERRUPT)))
		{			
			loadInputData(interface.endpoint[index]);
			
			need_init = true;
			found_input = true;
		}
		
		/* Output Endpoint */
		else if (not found_output and (endpoint_info & LIBUSB_TRANSFER_TYPE_INTERRUPT))
		{
			this->loadOutputData(interface.endpoint[index]);
			found_output = true;
		}
	}
	
	libusb_free_config_descriptor(config_description);
}


void hidDriver::claimInterface()
{
	this->printDebug(20, "Claiming interface from kernel: %d\n", this->INTERFACE);
	
	/*
	 * Linux has a general purpose HID driver that blocks access to the
	 * port if it is attached. So we must detach it before being able to 
	 * read any data from the device. libusb just ignores the call if it
	 * isn't attached.
	 */
	libusb_detach_kernel_driver(DEVICE, INTERFACE);
	libusb_claim_interface(DEVICE, INTERFACE);  
}


void hidDriver::releaseInterface()
{
	this->printDebug(20, "Releasing interface to kernel: %d\n", this->INTERFACE);
	
	libusb_release_interface(DEVICE, INTERFACE);
	libusb_attach_kernel_driver(DEVICE, INTERFACE);
	
	this->ENDPOINT_ADDRESS_IN = 0;
	this->ENDPOINT_ADDRESS_OUT = 0;
	
	this->TRANSFER_LENGTH_IN = 0;
	this->TRANSFER_LENGTH_OUT = 0;
}


void hidDriver::connect_thread()
{
	this->printDebug(20, "Attempting to connect to device:\n");
	this->printDebug(20, "\tVendor_id:  0x%04x\n", this->VENDOR_ID);
	this->printDebug(20, "\tProduct_id: 0x%04x\n", this->PRODUCT_ID);
	this->printDebug(20, "\tInterface:  %d\n", this->INTERFACE);
	
	if (not this->SERIAL_NUM.empty())
	{
		this->printDebug(20, "\tSerial Num: %s\n", this->SERIAL_NUM.c_str()); 
	}
	
	int following = 0;;
	
	this->disconnect();
	
	epicsMutexLock(this->device_state);
	while (not this->connected)
	{
		this->findDevice();
		
		if(this->DEVICE == NULL)
		{
			this->printDebug( following, 
			                  "error connecting to device with vendor: 0x%04X and product: 0x%04X, waiting for reconnect\n", 
			                  this->VENDOR_ID, 
			                  this->PRODUCT_ID);
			following = 1;
			
			/* 
			 * We don't want to constantly poll for open connections, so we can 
			 * sleep a bit before checking again.
			 */
			epicsThreadSleep(TIME_BETWEEN_CHECKS);
		}
		else
		{
			printf("connection (0x%04X:0x%04X) succeeded\n", this->VENDOR_ID, this->PRODUCT_ID);
			
			this->connected = true;
			
			this->loadDeviceInfo();
			this->claimInterface();
			this->setStatuses(asynSuccess);
			this->startUpdating();
		}
	}
	epicsMutexUnlock(this->device_state);
}


void  hidDriver::findDevice()
{	
	libusb_device** connected_devices;
	size_t amt_connected = libusb_get_device_list(context, &connected_devices);
		
	/*
	 * We don't want multiple drivers to have a race condition to grab 
	 * an open device, so we'll lock the entire connection. This also 
	 * keeps disconnects from pulling a device out of claimed while we 
	 * are iterating.
	 */
	epicsMutexLock(mylock);
	
	/* 
	 * Check every available device for one that matches our specifications
	 * and hasn't already been claimed by another driver
	 */
	for(unsigned index = 0; index < amt_connected; index += 1)
	{
		libusb_device* dev = connected_devices[index];
		
		if (this->isMatch(dev) and not contains(dev))
		{
			claimed.push_back(dev);
			libusb_open(dev, &DEVICE);
			break;
		}
	}
	
	epicsMutexUnlock(mylock);
	
	libusb_free_device_list(connected_devices, 0);
}


bool hidDriver::isMatch(libusb_device* dev)
{
	struct libusb_device_descriptor info;
	libusb_device_handle* handle;
	
	
	libusb_get_device_descriptor(dev, &info);

	if (info.idVendor == this->VENDOR_ID and 
	    info.idProduct == this->PRODUCT_ID)
	{
		if (not this->SERIAL_NUM.empty())
		{
		    libusb_open(dev, &handle);
		
			/* 
			 * 126 is the maximum amount of characters we'll have to deal with according 
			 * to the USB spec which mandates maximum sizes for the device_descriptor struct
			 * and character encodings. There are no guarantees if you use a device that
			 * doesn't follow the USB standard.
			 */
			unsigned char buffer[126];
			
			libusb_get_string_descriptor_ascii(handle, info.iSerialNumber, buffer, 126);
			
			libusb_close(handle);
			
			return (strcmp((char*) buffer, this->SERIAL_NUM.c_str()) == 0);
		}
		else
		{
			return true;
		}
	}
	
	return false;
}
