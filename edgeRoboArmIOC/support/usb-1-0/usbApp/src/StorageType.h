#ifndef INC_STORAGETYPE_H
#define INC_STORAGETYPE_H

#include <string>

#include <asynPortDriver.h>
#include <epicsTypes.h>

enum STORAGE_TYPE
{
	TYPE_UNKNOWN,
	TYPE_INT8,
	TYPE_INT16,
	TYPE_INT32,
	TYPE_UINT8,
	TYPE_UINT16,
	TYPE_UINT32,
	TYPE_UINT32DIGITAL,
	TYPE_BOOLEAN,
	TYPE_FLOAT32,
	TYPE_FLOAT64,
	TYPE_STRING,
	TYPE_INT8ARRAY,
	TYPE_INT16ARRAY,
	TYPE_INT32ARRAY,
	TYPE_FLOAT32ARRAY,
	TYPE_FLOAT64ARRAY
};

/* Read in type from specification file to OUTPUT_TYPE */
void type_from_string(std::string type_input, STORAGE_TYPE* output_location);
asynParamType asyn_from_type(STORAGE_TYPE type_input);

epicsUInt32 get_type_mask(STORAGE_TYPE type_input);

#endif
