#ifndef INC_DATALAYOUT_H
#define INC_DATALAYOUT_H

#include <vector>
#include <string>

#include "Allocation.h"

class DataLayout
{
	public:
		DataLayout();
		void               add(Allocation& input);
			
		unsigned    const  size();              //Number of Params
		int         const  interface_mask();    //What types are supported
		int         const  interrupt_mask();    //What interrupt types are supported
		Allocation* const  get(const unsigned index);
		Allocation* const  get(std::string param_name);
		Allocation* const  withIndex(int find_index);
		
	private:
		unsigned bytes;
		int face_mask;
		int rupt_mask;
		std::vector<Allocation> storage;
};

#endif
