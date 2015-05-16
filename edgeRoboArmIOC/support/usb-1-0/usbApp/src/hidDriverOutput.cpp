#include "hidDriver.h"
#include "Data.h"

void hidDriver::loadOutputData(const struct libusb_endpoint_descriptor endpoint)
{
	this->printDebug(10, "Ouput endpoint found at: 0x%02X\n", endpoint.bEndpointAddress);
	this->printDebug(10, "Report protocol length: %d bytes\n", endpoint.wMaxPacketSize);
	
	epicsMutexLock(this->output_state);
		this->ENDPOINT_ADDRESS_OUT = endpoint.bEndpointAddress;
		this->TRANSFER_LENGTH_OUT  = endpoint.wMaxPacketSize;
	epicsMutexUnlock(this->output_state);
}

asynStatus hidDriver::sendOutputReport()
{	
	int amt_transferred;
	
	epicsMutexLock(this->output_state);
		if (TRANSFER_LENGTH_OUT == 0)
		{
			epicsMutexUnlock(this->output_state);
			
			if (this->connected)    { return asynError; }
			else                    { return asynDisconnected; }
		}
	
		uint8_t data[this->TRANSFER_LENGTH_OUT];
		
		for(unsigned index = 0; index < this->output_specification.size(); index += 1)
		{
			Allocation* layout = this->output_specification.get(index);
			
			WRITE_FUNCTION tocall = getWriteFunction(layout->type);
			
			tocall(this, &data[layout->start], layout);
		}
	
		int errno = libusb_interrupt_transfer( this->DEVICE, 
		                                       this->ENDPOINT_ADDRESS_OUT, 
		                                       data, 
		                                       this->TRANSFER_LENGTH_OUT, 
		                                       &amt_transferred, 
		                                       this->TIMEOUT);
	epicsMutexUnlock(this->output_state);
		
	
	if (errno == LIBUSB_ERROR_TIMEOUT)
	{
		this->printDebug(1, "Connection timedout listening for output device report.\n");
		
		epicsMutexLock(this->output_state);
			this->setStatuses(this->output_specification, asynTimeout);
		epicsMutexUnlock(this->output_state);
	
		return asynTimeout;
	}
	
	else if (errno == LIBUSB_ERROR_PIPE or
	         errno == LIBUSB_ERROR_NOT_FOUND or
	         errno == LIBUSB_ERROR_NO_DEVICE)
	{
		this->printDebug(1, "Problem communicating with device, attempting reconnection.\n");
			
		this->disconnect();
		this->connect();
		
		return asynDisconnected;
	}
			
	else if (errno == LIBUSB_ERROR_OVERFLOW)
	{
		this->printDebug(1, "Too much information sent by device, reloading connection parameters.\n");
		
		epicsMutexLock(this->output_state);
			this->setStatuses(this->output_specification, asynOverflow);
		epicsMutexUnlock(this->output_state);
		
		this->loadDeviceInfo();
	
		return asynOverflow;
	}
	
	else
	{
		epicsMutexLock(this->output_state);
			this->setStatuses(this->output_specification, asynSuccess);
		epicsMutexUnlock(this->output_state);
		
		return asynSuccess;
	}
}

asynStatus hidDriver::writeInt32(asynUser* pasynuser, epicsInt32 value)
{
	asynPortDriver::writeInt32(pasynuser, value);	
	return this->sendOutputReport();
}

asynStatus hidDriver::writeFloat64(asynUser* pasynuser, epicsFloat64 value)
{
	asynPortDriver::writeFloat64(pasynuser, value);
	return this->sendOutputReport();
}

asynStatus hidDriver::writeOctet(asynUser* pasynuser, const char* value, size_t maxChars, size_t* nActual)
{
	asynPortDriver::writeOctet(pasynuser, value, maxChars, nActual);
	return this->sendOutputReport();
}
