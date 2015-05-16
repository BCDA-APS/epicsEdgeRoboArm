#include "Allocation.h"


Allocation::Allocation()
{
	name   = "";
	length = 0;
	mask   = 0xFFFFFFFF;
	type   = TYPE_UNKNOWN;
	shift = 0;
	start  = 0;
	index  = 0;
}
