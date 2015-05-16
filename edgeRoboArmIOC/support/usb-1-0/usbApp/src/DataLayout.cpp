#include "DataLayout.h"

#include <asynPortDriver.h>
#include "StorageType.h"

using namespace std;

DataLayout::DataLayout()
:   bytes(0), 
    face_mask(asynDrvUserMask),
    rupt_mask(0)
{
}

unsigned const DataLayout::size()              { return storage.size(); }
int      const DataLayout::interface_mask()    { return face_mask; }
int      const DataLayout::interrupt_mask()    { return rupt_mask; }

Allocation* const DataLayout::get(const unsigned index)
{
	return &storage[index];
}

Allocation* const DataLayout::withIndex(int find_index)
{
	for(unsigned index = 0; index < storage.size(); index += 1)
	{
		if (storage[index].index == find_index)    { return &storage[index]; }
	}
	
	return NULL;
}

Allocation* const DataLayout::get(string param_name)
{
	for(unsigned index = 0; index < storage.size(); index += 1)
	{
		if (storage[index].name == param_name)    { return &storage[index]; }
	}
	
	return NULL;
}

void DataLayout::add(Allocation& input)
{
	storage.push_back(input);

	
	/* Keep a note of the last index referenced by a parameter */
	unsigned endpoint = input.start + input.length;
	bytes = (endpoint > bytes) ? endpoint : bytes;
	
	
	/* Build the masks used by asynPortDriver to properly set parameters */
	epicsUInt32 type_mask = get_type_mask(input.type);
	
	face_mask |= type_mask;
	rupt_mask |= type_mask;	
}
