#include "StorageType.h"

/**
 * Parses the name field from specification files into a type to be used
 * during parameter updates.
 *
 * While most do, these do not have to match up against an asynParamType
 * one to one as these are just used by the hidDriver to determine how
 * to read the data sent by the usb device.
 *
 * @param[in]  type_input        The string read in from the spec file
 * @param[out] ouput_location    The output type
 */
void type_from_string(std::string type_input, STORAGE_TYPE* output_location)
{
	if      (type_input == "Int8" || 
	         type_input == "int8")
		{ *output_location = TYPE_INT8; }
		
	else if (type_input == "Int16" || 
	         type_input == "int16")
		{ *output_location = TYPE_INT16; }
		
	else if (type_input == "Int32" || 
	         type_input == "int32")
		{ *output_location = TYPE_INT32; }
		
	else if (type_input == "UInt8" || 
	         type_input == "uint8")
		{ *output_location = TYPE_UINT8; }
		
	else if (type_input == "UInt16" || 
	         type_input == "uint16")
		{ *output_location = TYPE_UINT16; }
		
	else if (type_input == "UInt32" || 
	         type_input == "uint32")
		{ *output_location = TYPE_UINT32; }
		
	else if (type_input == "UInt32Digital" || 
	         type_input == "uint32digital" ||
	         type_input == "Bitfield" || 
	         type_input == "bitfield")
		{ *output_location = TYPE_UINT32DIGITAL; }
		
	else if (type_input == "Bool" || 
	         type_input == "bool" ||
	         type_input == "Boolean" || 
	         type_input == "boolean")
		{ *output_location = TYPE_BOOLEAN; }
		
	else if (type_input == "String" || 
	         type_input == "string")
		{ *output_location = TYPE_STRING; }
		
	else if (type_input == "Float32" || 
	         type_input == "float32")
		{ *output_location = TYPE_FLOAT32; }
		
	else if (type_input == "Float64" || 
	         type_input == "float64")
		{ *output_location = TYPE_FLOAT64; }
		
	else if (type_input == "Int8Array" || 
	         type_input == "int8array")
		{ *output_location = TYPE_INT8ARRAY; }
		
	else if (type_input == "Int16Array" || 
	         type_input == "int16array")
		{ *output_location = TYPE_INT16ARRAY; }
		
	else if (type_input == "Int32Array" || 
	         type_input == "int32array")
		{ *output_location = TYPE_INT32ARRAY; }
		
	else if (type_input == "Float32Array" || 
	         type_input == "float32array")
		{ *output_location = TYPE_FLOAT32ARRAY; }
		
	else if (type_input == "Float64Array" || 
	         type_input == "float64array")
		{ *output_location = TYPE_FLOAT64ARRAY; }
		
	else
		{ *output_location = TYPE_UNKNOWN; }
}

/**
 * Which asynParamType is used to store the given output type
 *
 * @param[in]  type_input    The output type
 *
 * @return    The asynParamType to store the param as.
 */
asynParamType asyn_from_type(STORAGE_TYPE type_input)
{
	switch(type_input)
	{
		case TYPE_INT8:
		case TYPE_INT16:
		case TYPE_INT32:
		case TYPE_UINT8:
		case TYPE_UINT16:
		case TYPE_UINT32:
		case TYPE_BOOLEAN:
			return asynParamInt32;
			
		case TYPE_UINT32DIGITAL:
			return asynParamUInt32Digital;
			
		case TYPE_STRING:
			return asynParamOctet;
			
		case TYPE_FLOAT32:
		case TYPE_FLOAT64:
			return asynParamFloat64;
			
		case TYPE_FLOAT32ARRAY:
			return asynParamFloat32Array;
			
		case TYPE_FLOAT64ARRAY:
			return asynParamFloat64Array;
			
		case TYPE_INT8ARRAY:
			return asynParamInt8Array;
			
		case TYPE_INT16ARRAY:
			return asynParamInt16Array;
			
		case TYPE_INT32ARRAY:
			return asynParamInt32Array;
			
		default:
			return asynParamInt32;
	}
}

epicsUInt32 get_type_mask(STORAGE_TYPE type_input)
{
	switch(type_input)
	{
		case TYPE_INT8:
		case TYPE_INT16:
		case TYPE_INT32:
		case TYPE_UINT8:
		case TYPE_UINT16:
		case TYPE_UINT32:
		case TYPE_BOOLEAN:
			return asynInt32Mask;
		
		case TYPE_UINT32DIGITAL:
			return asynUInt32DigitalMask;
				
		case TYPE_STRING:
			return asynOctetMask;
			
		case TYPE_FLOAT32:
		case TYPE_FLOAT64:
			return asynFloat64Mask;
			
		case TYPE_INT8ARRAY:
			return asynInt8ArrayMask;
			
		case TYPE_INT16ARRAY:
			return asynInt16ArrayMask;
			
		case TYPE_INT32ARRAY:
			return asynInt32ArrayMask;
			
		case TYPE_FLOAT32ARRAY:
			return asynFloat32ArrayMask;
			
		case TYPE_FLOAT64ARRAY:
			return asynFloat64ArrayMask;
			
		default:
			return 0;
	}
}
