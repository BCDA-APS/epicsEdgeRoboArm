#include <cstring>
#include <sstream>

#include "hidDriver.h"
#include "Data.h"

void update_thread_callback(void* arg)
{	
	hidDriver* driver = (hidDriver*) arg;
	
	driver->update_thread();
}


void hidDriver::update_thread()
{
	/*
	 * We want to keep the driver running somewhat smoothly, so we will take the 
	 * time at the beginning and the end of this function and use a thread wait 
	 * to slow things if they get too fast for the USB connection.
	 */
	epicsTimeStamp start;
	epicsTimeStamp end;
	int amt_transferred;
	
	this->printDebug(20, "Starting update\n");
	
	epicsMutexLock(this->device_state);
	while (this->connected && this->FREQUENCY > 0.0)
	{
		epicsTimeGetCurrent(&start);
		
		epicsMutexLock(this->input_state);
			int errno = libusb_interrupt_transfer( this->DEVICE, 
		                                           this->ENDPOINT_ADDRESS_IN, 
		                                           this->state, 
		                                           this->TRANSFER_LENGTH_IN, 
		                                           &amt_transferred, 
		                                           this->TIMEOUT);
		
			if (errno == LIBUSB_SUCCESS)    { this->updateParams(); }
		epicsMutexUnlock(this->device_state);
		epicsMutexUnlock(this->input_state);
		/*
		 * If the device is not there anymore, change state to disconnected 
		 * and then try to connect again. Reconnecting will spawn its own
		 * update thread, so we'll close out of this one.
		 */
		if (errno == LIBUSB_ERROR_NO_DEVICE or 
		    errno == LIBUSB_ERROR_NOT_FOUND or
			errno == LIBUSB_ERROR_PIPE)
		{
			this->printDebug(1, "Problem communicating with device, attempting reconnection.\n");
			
			this->disconnect();
			this->connect();
			return;
		}
		
		/*
		 * If the device sends us too much information, then something in our
		 * configuration is wrong. So we'll try to reload it.
		 */
		else if (errno == LIBUSB_ERROR_OVERFLOW)
		{
			this->printDebug(1, "Too much information sent by device, reloading connection parameters.\n");
		
			epicsMutexLock(this->input_state);
				this->setStatuses(this->input_specification, asynOverflow);
			epicsMutexUnlock(this->input_state);
			
			this->loadDeviceInfo();
		}
		
		else if (errno == LIBUSB_ERROR_TIMEOUT)
		{
			this->printDebug(1, "Connection timedout listening for input device report.\n");
			
			epicsMutexLock(this->input_state);
				this->setStatuses(this->input_specification, asynTimeout);
			epicsMutexUnlock(this->input_state);
		}	
			
		epicsTimeGetCurrent(&end);
			
		epicsThreadSleep(this->FREQUENCY - epicsTimeDiffInSeconds(&end, &start));
		epicsMutexLock(this->device_state);
	}
	
	epicsMutexUnlock(this->device_state);
	this->printDebug(20, "Updating stopped\n");
}


void hidDriver::updateParams()
{	
	/*
	 * Iterate through the asyn params and assign them to their 
	 * registers.
	 */
	for (unsigned index = 0; index < this->input_specification.size(); index += 1)
	{
		Allocation* layout = this->input_specification.get(index);
		
		unsigned offset = layout->start;
		
		/* We don't need to update if nothing has changed */
		bool changed = (memcmp(&state[offset], &last_state[offset], layout->length) != 0);
		
		if (this->need_init || changed)
		{					
			READ_FUNCTION tocall = getReadFunction(layout->type);
			
			tocall(this, &state[offset], layout);
		}
		
		this->setParamStatus(layout->index, asynSuccess);
	}
	
	this->need_init = false;
	
	memcpy(this->last_state, this->state, this->TRANSFER_LENGTH_IN);
	
	/*
	 * Make sure that non-array DB values that use 'I/O Intr' 
	 * will properly update themselves. asyn Documentation
	 * reccomends this happens after all values are updated
	 * rather than immediately after each one.
	 */			
	this->callParamCallbacks();
}


void hidDriver::loadInputData(const struct libusb_endpoint_descriptor endpoint)
{
	this->printDebug(10, "Input endpoint found at: 0x%02X\n", endpoint.bEndpointAddress);
	this->printDebug(10, "Report protocol length: %d bytes\n", endpoint.wMaxPacketSize);
	
	epicsMutexLock(this->input_state);
		this->ENDPOINT_ADDRESS_IN = endpoint.bEndpointAddress;
		this->TRANSFER_LENGTH_IN  = endpoint.wMaxPacketSize;
		
		delete [] this->state;
		delete [] this->last_state;
		
		this->state = new uint8_t[TRANSFER_LENGTH_IN];
		this->last_state = new uint8_t[TRANSFER_LENGTH_IN];
	
		memset(this->state, 0, TRANSFER_LENGTH_IN);
		memset(this->last_state, 0, TRANSFER_LENGTH_IN);
	epicsMutexUnlock(this->input_state);
}



void hidDriver::startUpdating()
{
	/* Spawn update thread */
	
	std::stringstream temp_stream;
	std::string threadname;
	
	temp_stream << "hidDriver (" << VENDOR_ID << ":" << PRODUCT_ID << ")";
	temp_stream >> threadname;
	
	epicsThreadCreate(threadname.c_str(), 
					epicsThreadPriorityMedium, 
					epicsThreadGetStackSize(epicsThreadStackMedium), 
					(EPICSTHREADFUNC)::update_thread_callback, this);
}
