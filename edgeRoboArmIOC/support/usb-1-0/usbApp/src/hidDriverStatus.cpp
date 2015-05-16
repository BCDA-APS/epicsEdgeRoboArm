#include "hidDriver.h"


void hidDriver::setDebugLevel(int amt)
{
	this->DEBUG_LEVEL = amt;
}


void hidDriver::printDebug(int level, std::string format, ...)
{
	va_list args;
	
	va_start(args, format);
	
	if(DEBUG_LEVEL >= level)
	{
		printf("%s: ", this->portName);
		vprintf(format.c_str(), args);
	}
	
	va_end(args);
}


void hidDriver::setStatuses(asynStatus status)
{
	this->setStatuses(this->input_specification, status);
	this->setStatuses(this->output_specification, status);
}


void hidDriver::setStatuses(DataLayout& spec, asynStatus status)
{	
	for(unsigned index = 0; index < spec.size(); index += 1)
	{	
		Allocation* layout = spec.get(index);
		
		this->setParamStatus(layout->index, status);
	}

	this->callParamCallbacks();
}


void hidDriver::setFrequency(double freq)
{
	this->printDebug(10, "Setting Frequency: %fhz -> %fhz\n", this->FREQUENCY, freq);
	/* 
	 * Normally, comparing floating point numbers against constants is discouraged,
	 * but the only time frequency will be zero is when the constant 0.0 is passed
	 * is by hidSetFrequency, so things should be alright.
	 */
	if (this->FREQUENCY == 0.0)
	{
		this->FREQUENCY = freq;
		
		/*
		 * To avoid spinlocking, our update thread stops when frequency is zero,
		 * so we need to start it up again when we change back.
		 */
		startUpdating();
	}
	else
	{	
		this->FREQUENCY = freq;
	}
}


void hidDriver::setTimeout(int timing)
{
	this->printDebug(10, "Setting Timeout: %dms -> %dms\n", TIMEOUT, timing);
	
	this->TIMEOUT = timing;
}


void hidDriver::setConnectDelay(double delay)
{
	this->printDebug( 10, 
	                  "Setting Connection Delay: %fs -> %fs\n", 
	                  this->TIME_BETWEEN_CHECKS, 
	                  delay);
	
	this->TIME_BETWEEN_CHECKS = delay;
}


void hidDriver::setInterface(int interface_in)
{
	this->printDebug( 10, 
	                  "Setting USB Interface: %d -> %d\n", 
	                  this->INTERFACE, 
	                  interface_in);
	
	epicsMutexLock(this->device_state);
		/* We don't need the current interface anymore */
		this->releaseInterface();
		
		this->INTERFACE = interface_in;
		
		/* 
		 * New interface will quite likely have different endpoints and probably
		 * a different transfer size. Load new information and claim the interface.
		 */
		this->loadDeviceInfo();
		this->claimInterface();
	epicsMutexUnlock(this->device_state);
}
