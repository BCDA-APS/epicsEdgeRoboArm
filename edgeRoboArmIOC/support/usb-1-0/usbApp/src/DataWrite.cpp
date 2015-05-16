#include <cstring>

#include <epicsTypes.h>

#include "Data.h"

static void write_INT8(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void write_INT16(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void write_INT32(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void write_UINT8(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void write_UINT16(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void write_UINT32(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void write_UINT32DIGITAL(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void write_BOOLEAN(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void write_FLOAT32(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void write_FLOAT64(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void write_INT8ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void write_INT16ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void write_INT32ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void write_FLOAT32ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void write_FLOAT64ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void write_STRING(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void write_UNKNOWN(asynPortDriver* callback, uint8_t* data, Allocation* layout);



static void write_int(asynPortDriver* callback, uint8_t* data, Allocation* layout, int max_bytes)
{
	epicsInt32 temp;
	epicsUInt32 value;
	epicsInt32 current;
	
	callback->getIntegerParam(layout->index, &temp);
	
	memcpy(&current, data, std::min(max_bytes, (int) layout->length));
	
	value = (epicsUInt32) temp;
	
	value &= layout->mask;
	value <<= layout->shift;
	current |= value;
	
	memcpy(data, &current, std::min(max_bytes, (int) layout->length));
}


/**
 * Which function to use to push data out to the usb device based
 * upon what output type the data is being processed as.
 *
 * @param[in]  type    The output type of the parameter
 *
 * @return  The function used to process data as the given type.
 */
WRITE_FUNCTION getWriteFunction(STORAGE_TYPE type)
{
	switch(type)
	{
		case TYPE_INT8:             return write_INT8;
		case TYPE_INT16:            return write_INT16;
		case TYPE_INT32:            return write_INT32;
		case TYPE_UINT8:            return write_UINT8;
		case TYPE_UINT16:           return write_UINT16;
		case TYPE_UINT32:           return write_UINT32;
		case TYPE_UINT32DIGITAL:    return write_UINT32DIGITAL;
		case TYPE_BOOLEAN:          return write_BOOLEAN;
		case TYPE_FLOAT32:          return write_FLOAT32;
		case TYPE_FLOAT64:          return write_FLOAT64;
		case TYPE_INT8ARRAY:        return write_INT8ARRAY;
		case TYPE_INT16ARRAY:       return write_INT16ARRAY;
		case TYPE_INT32ARRAY:       return write_INT32ARRAY;
		case TYPE_FLOAT32ARRAY:     return write_FLOAT32ARRAY;
		case TYPE_FLOAT64ARRAY:     return write_FLOAT64ARRAY;
		case TYPE_STRING:           return write_STRING;
		default:                    return write_UNKNOWN;
	}
}


static void write_INT8(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	write_int(callback, data, layout, 1);
}

static void write_INT16(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	write_int(callback, data, layout, 2);
}

static void write_INT32(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	write_int(callback, data, layout, 4);
}

static void write_UINT8(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	write_int(callback, data, layout, 1);
}

static void write_UINT16(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	write_int(callback, data, layout, 2);
}

static void write_UINT32(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	write_int(callback, data, layout, 4);
}

static void write_UINT32DIGITAL(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	write_int(callback, data, layout, 4);
}

static void write_BOOLEAN(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	epicsInt32 temp = 0;
	epicsUInt32 value = 0;
	epicsUInt32 current = 0;
	
	callback->getIntegerParam(layout->index, &temp);
	memcpy(&current, data, std::min(4, (int) layout->length));
	
	value = (epicsUInt32) temp;
	
	uint8_t test[1];
	memcpy(test, &value, 1);
	
	value *= layout->mask;
	value <<= layout->shift;
	current |= value;
	
	memcpy(data, &current, std::min(4, (int) layout->length));
}

static void write_FLOAT32(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	epicsFloat64 temp;
	epicsFloat32 value;
	
	callback->getDoubleParam(layout->index, &temp);
	
	value = (epicsFloat32) temp;
	
	memcpy(data, &value, 4);
}

static void write_FLOAT64(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	epicsFloat64 value;
	
	callback->getDoubleParam(layout->index, &value);
	
	memcpy(data, &value, 8);
}

static void write_STRING(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	callback->getStringParam(layout->index, std::min(40, (int) layout->length), (char*) data);
}

static void write_UNKNOWN(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
}

static void write_INT8ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout){}
static void write_INT16ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout){}
static void write_INT32ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout){}

static void write_FLOAT32ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout){}
static void write_FLOAT64ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout){}
