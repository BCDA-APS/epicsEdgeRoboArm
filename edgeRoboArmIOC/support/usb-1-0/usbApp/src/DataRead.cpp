#include <cstring>
#include <cmath>

#include <epicsTypes.h>

#include "Data.h"

static int num_bits(unsigned mask);

static void read_signed(asynPortDriver* callback, uint8_t* data, Allocation* layout, int max_bytes);
static void read_unsigned(asynPortDriver* callback, uint8_t* data, Allocation* layout, int max_bytes);

static void read_INT8(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void read_INT16(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void read_INT32(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void read_UINT8(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void read_UINT16(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void read_UINT32(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void read_UINT32DIGITAL(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void read_BOOLEAN(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void read_FLOAT32(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void read_FLOAT64(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void read_INT8ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void read_INT16ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void read_INT32ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void read_FLOAT32ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);
static void read_FLOAT64ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void read_STRING(asynPortDriver* callback, uint8_t* data, Allocation* layout);

static void read_UNKNOWN(asynPortDriver* callback, uint8_t* data, Allocation* layout);


static int num_bits(unsigned mask)
{
	if (mask == 0)    { return 0; }
	
	/*
	 * To determine the most "leftward" bit, we can take the base 2 log of the
	 * mask. The ceil of the log gives us the ordinal position of the most
	 * significant bit.
	 */
	return (int) ceil(log(mask) / log(2));
}


static void read_signed(asynPortDriver* callback, uint8_t* data, Allocation* layout, int max_bytes)
{
	epicsInt32 itemp = 0;
	
	memcpy(&itemp, data, std::min(max_bytes, (int) layout->length));
	
	itemp = (itemp >> layout->shift) & layout->mask;
	
	int mask_bitsize = num_bits(layout->mask);
	
	/* 
	 * Now we'll compare that against the specified length. if the mask is larger 
	 * than the mask, we can ignore the mask.
	 */
	int bitsize = std::min(mask_bitsize, (int) (layout->length * 8 - layout->shift));
	
	int shift = 32 - bitsize;
	
	callback->setIntegerParam(layout->index, ((itemp << shift) >> shift));
}


static void read_unsigned(asynPortDriver* callback, uint8_t* data, Allocation* layout, int max_bytes)
{
	epicsUInt32 utemp = 0;

	memcpy(&utemp, data, std::min(max_bytes, (int) layout->length));
	
	utemp = (utemp >> layout->shift) & layout->mask;
	
	callback->setIntegerParam(layout->index, utemp);
}


/**
 * Which function to use to parse data coming in from the usb device based
 * upon what output type the data is being processed as.
 *
 * @param[in]  type    The output type of the parameter
 *
 * @return  The function used to process data as the given type.
 */
READ_FUNCTION getReadFunction(STORAGE_TYPE type)
{
	switch(type)
	{
		case TYPE_INT8:             return read_INT8;
		case TYPE_INT16:            return read_INT16;
		case TYPE_INT32:            return read_INT32;
		case TYPE_UINT8:            return read_UINT8;
		case TYPE_UINT16:           return read_UINT16;
		case TYPE_UINT32:           return read_UINT32;
		case TYPE_UINT32DIGITAL:    return read_UINT32DIGITAL;
		case TYPE_BOOLEAN:          return read_BOOLEAN;
		case TYPE_FLOAT32:          return read_FLOAT32;
		case TYPE_FLOAT64:          return read_FLOAT64;
		case TYPE_INT8ARRAY:        return read_INT8ARRAY;
		case TYPE_INT16ARRAY:       return read_INT16ARRAY;
		case TYPE_INT32ARRAY:       return read_INT32ARRAY;
		case TYPE_FLOAT32ARRAY:     return read_FLOAT32ARRAY;
		case TYPE_FLOAT64ARRAY:     return read_FLOAT64ARRAY;
		case TYPE_STRING:           return read_STRING;
		default:                    return read_UNKNOWN;
	}
}



static void read_INT8(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	read_signed(callback, data, layout, 1);
}


static void read_INT16(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	read_signed(callback, data, layout, 2);
}


/**
 * Update the given parameter index with the given data interpreted as an integer.
 * 
 * Ints will start with a copy, treating up to four bytes as a 32bit int, shift over
 * the data, apply the mask, then extend the final bit to the sign bit. Which bit is
 * considered the "final bit" is determined by the allocation length and the mask,
 * whatever is the most "leftward" (assuming big-endian) bit transferred and kept
 * will be extended.
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_INT32(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	read_signed(callback, data, layout, 4);
}


static void read_UINT8(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	read_unsigned(callback, data, layout, 1);
}


static void read_UINT16(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	read_unsigned(callback, data, layout, 2);
}


/**
 * Update the given parameter index with the given data interpreted as an unsigned
 * integer.
 * 
 * Unsigned ints will be a simple copy, treat up to four bytes as a 32bit int,
 * shift over, then apply mask. Due to asyn not implementing unsigned ints in its
 * parameter types, the range of the parameter is actually limited to 31bits.
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_UINT32(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	read_unsigned(callback, data, layout, 4);
}


/**
 * Digital ints are also simple copys, treat up to four bytes as a 32bit 
 * unsigned int, and apply mask. Due to how asynPortDriver treats digital 
 * params, there isn't a good way to handle shifts, so we don't.
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_UINT32DIGITAL(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	epicsUInt32 utemp = 0;

	memcpy(&utemp, data, std::min(4, (int) layout->length));

	callback->setUIntDigitalParam(layout->index, utemp, layout->mask);
}


/**
 * Apply a shift to the given data, then apply the mask. If any bits are still
 * set, the parameter will be 1, otherwise 0.
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_BOOLEAN(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	epicsUInt32 utemp = 0;
	
	memcpy(&utemp, data, std::min(4, (int) layout->length));
	
	utemp = (utemp >> layout->shift) & layout->mask;
	
	utemp = (utemp == 0) ? 0 : 1;
	
	callback->setIntegerParam(layout->index, utemp);
}


/**
 * Treat up to 4 bytes as a 32bit float
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_FLOAT32(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	epicsFloat32 ftemp = 0.0;

	memcpy(&ftemp, data, std::min(4, (int) layout->length));
	
	callback->setDoubleParam(layout->index, (epicsFloat64) ftemp);
}


/**
 * Treat up to 8 bytes as a 64bit float
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_FLOAT64(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{						
	epicsFloat64 ftemp = 0.0;

	memcpy(&ftemp, data, std::min(8, (int) layout->length));
	
	callback->setDoubleParam(layout->index, ftemp);
}


/**
 * Copies up to 40 bytes as an ascii string, no mask, no shift.
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_STRING(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	unsigned length = std::min(40, (int) (layout->length));
	
	epicsInt8 buffer[length];
	
	memcpy(&buffer, data, length);
	
	callback->setStringParam(layout->index, (const char*) buffer);
}


/**
 * This is literally a copy of the array, while bitshifting and masking make 
 * sense here, it is too complicated to implement initally.
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_INT8ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{	
	epicsInt8 atemp[layout->length];
	
	memcpy(atemp, data, layout->length);
	
	callback->doCallbacksInt8Array(atemp, layout->length, layout->index, 0);
}


/**
 * This is literally a copy of the array, while bitshifting and masking make 
 * sense here, it is too complicated to implement initally.
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_INT16ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	unsigned len = layout->length >> 1;
	
	epicsInt16 atemp[len];
	
	memcpy(atemp, data, layout->length);
	
	callback->doCallbacksInt16Array(atemp, len, layout->index, 0);
}


/**
 * This is literally a copy of the array, while bitshifting and masking make 
 * sense here, it is too complicated to implement initally.
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_INT32ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	unsigned len = layout->length >> 2;
						
	epicsInt32 atemp[len];
	
	memcpy(atemp, data, layout->length);
	
	callback->doCallbacksInt32Array(atemp, len, layout->index, 0);
}


/**
 * This is literally a copy of the array, no bitshift or mask
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_FLOAT32ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{	
	unsigned len = layout->length >> 2;
	
	epicsFloat32 atemp[len];
	
	memcpy(atemp, data, layout->length);
	
	callback->doCallbacksFloat32Array(atemp, len, layout->index, 0);
}


/**
 * This is literally a copy of the array, no bitshift or mask
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_FLOAT64ARRAY(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	unsigned len = layout->length >> 3;
						
	epicsFloat64 atemp[len];
	
	memcpy(atemp, data, layout->length);
	
	callback->doCallbacksFloat64Array(atemp, len, layout->index, 0);
}


/**
 * Purposefully does nothing
 *
 * @param[in]  index       Index of the parameter to update.
 * @param[out] callback    Which driver is calling.
 * @param[in]  data        A pointer to the start of the bytes to be interpreted.
 * @param[in]  layout      Other information about how to interpret the parameter.
 */
static void read_UNKNOWN(asynPortDriver* callback, uint8_t* data, Allocation* layout)
{
	
}
