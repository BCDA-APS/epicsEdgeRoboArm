#include <map>

#include <iostream>

#include <epicsExit.h>

#include "devUSB.h"
#include "Allocation.h"
#include "StringUtils.h"
#include "StorageType.h"

using namespace std;


static void remove_driver(void* data);
static hidDriver* get_driver(const char* name);
static bool port_used(const char* port_name);

static void parse(const char* filename, DataLayout& spec);

static map<string, hidDriver*> device_drivers;

/**
 * Parse a given specification file, replacing all instances of the given
 * macros.
 *
 * @param[in]  spec_file     The specification file to read parameter listings from
 * @param[out] spec          The final listing of all parameters and their data
 */
static void parse_layout(ifstream* spec_file, DataLayout* spec)
{
	string line;
	string type;
		
	pair<string,string> index_range;
	pair<string,string> optional_shift;
	
	while (getline(*spec_file, line))
	{
		trim(&line);

		if(! line.empty() && line[0] != '#')
		{
			Allocation toadd;

			unsigned end = 0;
			
			/* NAME [START |, END|] |>> SHIFT| -> TYPE |/MASK| */
			toadd.name = split_on(&line, "[");
			
			index_range = split_optional(&line, ",", "]");
				to_int(index_range.first, &toadd.start);
				to_int(index_range.second, &end);
				toadd.length = (end == 0) ? 1 : (end - toadd.start) + 1;
				
			optional_shift = split_optional(&line, ">>", "->");
			
			if (not optional_shift.second.empty())
			{
				to_int(optional_shift.second, &toadd.shift);
				
				toadd.start += (int) (toadd.shift / 8);
				toadd.shift = toadd.shift % 8;
			}
			
			type = split_on(&line, "/");
				type_from_string(type, &toadd.type);
				hex_to_int(line, &toadd.mask);
				
			if (toadd.type == TYPE_UNKNOWN)
			{
				printf("Unknown parameter type for param: %s\n", toadd.name.c_str());
			}

			spec->add(toadd);
		}
	}
}


static hidDriver* get_driver(const char* name)
{
	map<string, hidDriver*>::iterator dev = device_drivers.find(string(name));
	
	return (dev != device_drivers.end()) ? dev->second : NULL;
}


static void remove_driver(void* data)
{
	hidDriver* driver = (hidDriver*) data;
	
	device_drivers.erase(string(driver->portName));
}

static bool port_used(const char* port_name)    { return (get_driver(port_name) != NULL); }


bool checkConnectionArgs(const iocshArgBuf* args)
{
	if (args[0].sval == NULL)
	{
		printf("Error: no input given.\n");
		return false;
	}
	else if (not port_used(args[0].sval))
	{ 
		printf("Error: couldn't find port specified.\n");
		return false;
	}
	else if (args[1].ival < 0)
	{
		printf("Error: interface cannot be negative.\n");
		return false;
	}
	
	return true;
}


bool checkDriverArgs(const iocshArgBuf* args)
{
	if (args[0].sval == NULL)
	{
		printf("Error: no input given.\n");
		return false;
	}
	else if (args[1].sval == NULL)
	{
		printf("Error: no input filename specified.\n");
		return false;
	}
	else if (port_used(args[0].sval))
	{
		printf("Error: port(%s) already registered.\n", args[0].sval);
		return false;
	}
	
	return true;
}


bool checkTimeoutArgs(const iocshArgBuf* args)
{
	if (args[0].sval == NULL)
	{
		printf("Error: no input given.\n");
		return false;
	}
	else if (not port_used(args[0].sval))
	{ 
		printf("Error: couldn't find port specified.\n");
		return false;
	}
	else if (args[1].ival < 0)
	{
		printf("Error: input cannot be negative.\n");
		return false;
	}
	
	return true;
}


bool checkFrequencyArgs(const iocshArgBuf* args)
{
	if (args[0].sval == NULL)
	{
		printf("Error: no input given.\n");
		return false;
	}
	else if (not port_used(args[0].sval))
	{ 
		printf("Error: couldn't find port specified.\n");
		return false;
	}
	else if (args[1].dval < 0.0)
	{
		printf("Error: input cannot be negative.\n");
		return false;
	}
	
	return true;
}


bool checkDelayArgs(const iocshArgBuf* args)
{
	if (args[0].sval == NULL)
	{
		printf("Error: no input given.\n");
		return false;
	}
	else if (not port_used(args[0].sval))
	{ 
		printf("Error: couldn't find port specified.\n");
		return false;
	}
	else if (args[1].dval < 0.0)
	{
		printf("Error: input cannot be negative.\n");
		return false;
	}
	
	return true;
}


bool checkDebugArgs(const iocshArgBuf* args)
{
	if (args[0].sval == NULL)
	{
		printf("Error: no input given.\n");
		return false;
	}
	else if (not port_used(args[0].sval))
	{ 
		printf("Error: couldn't find port specified.\n");
		return false;
	}
	else if (args[1].ival < 0)
	{
		printf("Error: input cannot be negative.\n");
		return false;
	}
	
	return true;
}


bool checkInterfaceArgs(const iocshArgBuf* args)
{
	if (args[0].sval == NULL)
	{
		printf("Error: no input given.\n");
		return false;
	}
	else if (not port_used(args[0].sval))
	{ 
		printf("Error: couldn't find port specified.\n");
		return false;
	}
	else if (args[1].ival < 0)
	{
		printf("Error: input cannot be negative.\n");
		return false;
	}
	
	return true;
}

bool checkTransArgs(const iocshArgBuf* args)
{
	if (args[0].sval == NULL)
	{
		printf("Error: no input given.\n");
		return false;
	}
	else if (not port_used(args[0].sval))
	{ 
		printf("Error: couldn't find port specified.\n");
		return false;
	}
	else if (args[1].ival < 0)
	{
		printf("Error: input cannot be negative.\n");
		return false;
	}
	
	return true;
}


static void parse(const char* filename, DataLayout& spec)
{
	ifstream spec_file;

	if(filename == NULL) { return; }
	
	spec_file.open(filename);
	
	if (not spec_file.is_open())
	{
		printf("Error: couldn't open specified file.\n");
		return;
	}
	
	parse_layout(&spec_file, &spec);
	spec_file.close();
}


void usbCreateDriver(const char* port_name, const char* input_filename, const char* output_filename)
{
	DataLayout      input_spec;
	DataLayout      output_spec;
	
	parse(input_filename, input_spec);
	parse(output_filename, output_spec);
	
	device_drivers[string(port_name)] = new hidDriver(port_name, input_spec, output_spec);
}


void usbConnectDevice( const char* port_name, 
                             int   interface_num, 
                             int   vendor_id, 
                             int   product_id, 
                       const char* serial_num)
{
	string serial_out = (serial_num == NULL) ? "" : string(serial_num);
	
	get_driver(port_name)->connect( (uint16_t) vendor_id, 
	                                (uint16_t) product_id, 
	                                           serial_out, 
	                                           interface_num);
	
	epicsAtExit(remove_driver, get_driver(port_name));
}


void usbSetTimeout(const char* port_name, int timeout)
{
	get_driver(port_name)->setTimeout(timeout);
}


void usbSetDelay(const char* port_name, double delay)
{
	get_driver(port_name)->setConnectDelay(delay);
}


void usbSetInterface(const char* port_name, int interface_num)
{
	get_driver(port_name)->setInterface(interface_num);
}


void usbSetDebugLevel(const char* port_name, int amt)
{
	get_driver(port_name)->setDebugLevel(amt);
}


void usbSetFrequency( const char* port_name, double frequency)
{
	double timing = 1.0 / frequency;
	
	/* 
	 * If frequency is 0.0 or similar, timing will not be a correct number, however
	 * a frequency of 0 is a valid amount. So, we'll take advantage of the IEEE
	 * standard that states that comparisons with NaN will always return false.
	 */
	timing = (timing != timing) ? 0.0 : timing;
	
	get_driver(port_name)->setFrequency(timing);
}

void usbShowIO(const char* port_name, int tf)
{
	get_driver(port_name)->setIOPrinting(tf);
}


extern "C"
{
	static const iocshArg cx_arg0     = {"portName",       iocshArgString};
	static const iocshArg cx_arg1     = {"interfaceNum",   iocshArgInt};
	static const iocshArg cx_arg2     = {"vendorID",       iocshArgInt};
	static const iocshArg cx_arg3     = {"productID",      iocshArgInt};
	static const iocshArg cx_arg4     = {"serialNum",      iocshArgString};
	
	static const iocshArg driver_arg0 = {"portName",       iocshArgString};
	static const iocshArg driver_arg1 = {"inputSpecFile",  iocshArgString};
	static const iocshArg driver_arg2 = {"outputSpecFile", iocshArgString};

	static const iocshArg tout_arg0   = {"portName",       iocshArgString};
	static const iocshArg tout_arg1   = {"timeout",        iocshArgInt};

	static const iocshArg freq_arg0   = {"portName",       iocshArgString};
	static const iocshArg freq_arg1   = {"frequency",      iocshArgDouble};

	static const iocshArg delay_arg0  = {"portName",       iocshArgString};
	static const iocshArg delay_arg1  = {"delay",          iocshArgDouble};

	static const iocshArg debug_arg0  = {"portName",       iocshArgString};
	static const iocshArg debug_arg1  = {"debugLevel",     iocshArgInt};

	static const iocshArg inter_arg0  = {"portName",       iocshArgString};
	static const iocshArg inter_arg1  = {"interfaceNum",   iocshArgInt};
	
	static const iocshArg trans_arg0  = {"portName",       iocshArgString};
	static const iocshArg trans_arg1  = {"print_io_data",  iocshArgInt};
	
	
	
	static const iocshArg* cx_args[]     = {&cx_arg0, &cx_arg1, &cx_arg2, &cx_arg3, &cx_arg4};
	static const iocshArg* driver_args[] = {&driver_arg0, &driver_arg1, &driver_arg2};
	static const iocshArg* tout_args[]   = {&tout_arg0, &tout_arg1};
    static const iocshArg* freq_args[]   = {&freq_arg0, &freq_arg1};
	static const iocshArg* delay_args[]  = {&delay_arg0, &delay_arg1};
	static const iocshArg* debug_args[]  = {&debug_arg0, &debug_arg1};
	static const iocshArg* inter_args[]  = {&inter_arg0, &inter_arg1};
	static const iocshArg* trans_args[]  = {&trans_arg0, &trans_arg1};
	


	static const iocshFuncDef cx_func     = {"usbConnectDevice", 5, cx_args};
	static const iocshFuncDef driver_func = {"usbCreateDriver", 3, driver_args};
	static const iocshFuncDef tout_func   = {"usbSetTimeout", 2, tout_args};
	static const iocshFuncDef freq_func   = {"usbSetFrequency", 2, freq_args};
	static const iocshFuncDef delay_func  = {"usbSetDelay", 2, delay_args};
	static const iocshFuncDef debug_func  = {"usbSetDebugLevel", 2, debug_args};
	static const iocshFuncDef inter_func  = {"usbSetInterface", 2, inter_args};
	static const iocshFuncDef trans_func  = {"usbShowIO", 2, trans_args};
	
	

	static void call_cx_func(const iocshArgBuf* args)
	{
		if (checkConnectionArgs(args))
		{
			usbConnectDevice( args[0].sval, args[1].ival, args[2].ival, 
			                  args[3].ival, args[4].sval);
		}
	}

	static void call_driver_func(const iocshArgBuf* args)
	{
		if (checkDriverArgs(args))
		{
			usbCreateDriver(args[0].sval, args[1].sval, args[2].sval);
		}
	}

	static void call_tout_func(const iocshArgBuf* args)
	{
		if (checkTimeoutArgs(args))
		{
			usbSetTimeout(args[0].sval, args[1].ival);
		}
	}

	static void call_freq_func(const iocshArgBuf* args)
	{
		if (checkFrequencyArgs(args))
		{
			usbSetFrequency(args[0].sval, args[1].dval);
		}
	}
	
	static void call_delay_func(const iocshArgBuf* args)
	{
		if (checkDelayArgs(args))
		{
			usbSetDelay(args[0].sval, args[1].dval);
		}
	}
	
	static void call_debug_func(const iocshArgBuf* args)
	{
		if (checkDebugArgs(args))
		{
			usbSetDebugLevel(args[0].sval, args[1].ival);
		}
	}
	
	static void call_inter_func(const iocshArgBuf* args)
	{
		if (checkInterfaceArgs(args))
		{
			usbSetInterface(args[0].sval, args[0].ival);
		}
	}
	
	static void call_trans_func(const iocshArgBuf* args)
	{
		if (checkTransArgs(args))
		{
			usbShowIO(args[0].sval, args[0].ival);
		}
	}
	

	static void usbConnectRegistrar(void)       { iocshRegister(&cx_func, call_cx_func); }
	static void usbDriverRegistrar(void)        { iocshRegister(&driver_func, call_driver_func); }
	static void usbTimeoutRegistrar(void)       { iocshRegister(&tout_func, call_tout_func); }
	static void usbFrequencyRegistrar(void)     { iocshRegister(&freq_func, call_freq_func); }
	static void usbDelayRegistrar(void)         { iocshRegister(&delay_func, call_delay_func); }
	static void usbDebugRegistrar(void)         { iocshRegister(&debug_func, call_debug_func); }
	static void usbInterRegistrar(void)         { iocshRegister(&inter_func, call_inter_func); }
	static void usbTransRegistrar(void)         { iocshRegister(&trans_func, call_trans_func); }
	
	

	epicsExportRegistrar(usbConnectRegistrar);
	epicsExportRegistrar(usbDriverRegistrar);
	epicsExportRegistrar(usbTimeoutRegistrar);
	epicsExportRegistrar(usbFrequencyRegistrar);
	epicsExportRegistrar(usbDelayRegistrar);
	epicsExportRegistrar(usbDebugRegistrar);
	epicsExportRegistrar(usbInterRegistrar);
	epicsExportRegistrar(usbTransRegistrar);
}
