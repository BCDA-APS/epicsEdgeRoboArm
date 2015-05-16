#include <asynDriver.h>
#include "hidDriver.h"

/*
 * USB operates at 125hz, which is 8 million nano seconds per transfer, 
 * so we don't need to work any faster than that. Though more advanced
 * devices do have higher frequency modes, so we allow this to be changed
 * per driver.
 */
static const double DEFAULT_FREQUENCY = .008; //seconds

static const int DEFAULT_TIMEOUT = 20; //milliseconds

/* 
 * If no device is found when attempting to connect, how long to wait 
 * before attempting to connect again.
 */
static const double DEFAULT_CHECK = 5.0; //seconds



hidDriver::hidDriver(const char* port_name, DataLayout& input, DataLayout& output)
	:asynPortDriver( port_name, 
	                 1,                                         //Max # of Addresses
	                 input.size() + output.size(),              //Number of Params 
	                 input.interface_mask() | output.interface_mask(),           //Interface Mask
	                 input.interrupt_mask() | output.interrupt_mask(),           //Interrupt Mask
	                 ASYN_MULTIDEVICE,                          //Interface Type
	                 1,                                         //Autoconnect
	                 0,                                         //Thread Priority
	                 0)                                         //Initial Stack Size
{	
	/* Class Initialization */
	this->input_specification = input;
	this->output_specification = output;
	
	this->device_state = epicsMutexCreate();
	this->input_state  = epicsMutexCreate();
	this->output_state = epicsMutexCreate();
	this->connected    = false;
	this->INTERFACE    = 0;
	
	this->state        = NULL;
	this->last_state   = NULL;
	this->DEVICE       = NULL;
	
	this->TIMEOUT             = DEFAULT_TIMEOUT;
	this->FREQUENCY           = DEFAULT_FREQUENCY;
	this->TIME_BETWEEN_CHECKS = DEFAULT_CHECK;

	this->DEBUG_LEVEL = 0;
	
	/* Asyn Initialization */
	this->createParams(this->input_specification);
	this->createParams(this->output_specification);	
		
	this->setStatuses(asynError);
	
	/* Libusb Initialization */
	libusb_init(&context);
}


hidDriver::~hidDriver()
{
	this->disconnect();
	
	libusb_exit(context);
	
	this->printDebug(20, "Closing driver\n");
}


void hidDriver::createParams(DataLayout& spec)
{
	asynStatus status;
	
	for (unsigned index = 0; index < spec.size(); index += 1)
	{	
		Allocation* layout = spec.get(index);
		
		/*
		 * asynPortDriver handles access behind the scene for reads, so all we 
		 * have to do is create the correct params to access.
		 */
		status = this->createParam(layout->name.c_str(), asyn_from_type(layout->type), &layout->index);
		
		if(status != asynSuccess)
		{
			printf("Error creating %s param: %d\n", layout->name.c_str(), status);
		}
	}
}
