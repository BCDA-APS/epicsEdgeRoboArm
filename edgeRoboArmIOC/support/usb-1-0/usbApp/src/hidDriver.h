#ifndef INC_HIDDRIVER_H
#define INC_HIDDRIVER_H

#include <stdint.h>
#include <list>
#include <vector>
#include <string>

#include <libusb-1.0/libusb.h>

#include <epicsMutex.h>
#include <epicsExport.h>
#include <epicsThread.h>
#include <epicsTime.h>

#include "DataLayout.h"
#include "Allocation.h"


void setDebugLevel(int level);
bool contains(libusb_device* check);

class hidDriver : public asynPortDriver
{
	public:
		hidDriver(const char* portName, DataLayout& spec_input, DataLayout& spec_output);
					
		~hidDriver();
		
		void setTimeout(int new_timeout);
		void setFrequency(double new_frequency);
		void setConnectDelay(double new_delay);
		void setInterface(int new_interface);
		
		void connect(uint16_t vendor_id, uint16_t product_id, std::string serial, int interface_num);
		
		void connect_thread();
		void update_thread();
		void shutdown_thread();
		
		void printDebug(int level, std::string format, ...);
		void setDebugLevel(int amt);
		
		asynStatus writeInt32(asynUser* pasynuser, epicsInt32 value);
		asynStatus writeFloat64(asynUser* pasynuser, epicsFloat64 value);
		asynStatus writeOctet(asynUser* pasynuser, const char* value, size_t maxChars, size_t* nActual);
		
		
	private:
		void findDevice();
		bool isMatch(libusb_device* info);
		void loadDeviceInfo();
		void startUpdating();
		
		void releaseInterface();
		void claimInterface();
		
		void createParams(DataLayout& spec);
		
		void updateParams();

		void setStatuses(asynStatus status);
		void setStatuses(DataLayout& spec, asynStatus status);

		void loadInputData(const struct libusb_endpoint_descriptor endpoint);
		void loadOutputData(const struct libusb_endpoint_descriptor endpoint);
		
		asynStatus sendOutputReport();
		
		void connect();
		void disconnect();
	
		uint16_t     VENDOR_ID;
		uint16_t     PRODUCT_ID;
		std::string  SERIAL_NUM;
		unsigned     INTERFACE;
		
		int TRANSFER_LENGTH_IN;
		int ENDPOINT_ADDRESS_IN;
		
		int TRANSFER_LENGTH_OUT;
		int ENDPOINT_ADDRESS_OUT;
		
		int DEBUG_LEVEL;
		
		DataLayout input_specification;
		DataLayout output_specification;
		
		int    TIMEOUT;
        double FREQUENCY;
		double TIME_BETWEEN_CHECKS;
		
		uint8_t* state;
		uint8_t* last_state;
		
		libusb_context*         context;
		libusb_device_handle*   DEVICE;
		bool connected;
		epicsMutexId input_state;
		epicsMutexId output_state;
		epicsMutexId device_state;
		
		bool need_init;
};

#endif
