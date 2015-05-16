#include <sstream>

#include "StringUtils.h"
#include <stdio.h>


/**
 * Removes all whitespace from the leading edge of the string
 */
void ltrim(std::string* input)
{
	unsigned nonws = input->find_first_not_of("\t\n ");
	
	if(nonws != std::string::npos) { input->erase(0, nonws); }
}


/**
 * Removes all whitespace from the trailing edge of the string
 */
void rtrim(std::string* input)
{
	unsigned nonws = input->find_last_not_of("\t\n ");
	
	if(nonws != std::string::npos) { input->erase(nonws + 1); }
}


/**
 * Removes all whitespace from both the front and the back of a string
 */
void trim(std::string* input)
{
	ltrim(input);
	rtrim(input);
}


/**
 * Applys a set of macros to a given string by replaces instances
 * of a target word surrounded by '$()' with the given replacement
 * string.
 *
 * @param[in]  input     The string to work on
 * @param[in]  macros    The set of macros each defined as 'target=replace'
 */
void apply_macros(std::string* input, std::vector<std::string> macros)
{
	for(unsigned index = 0; index < macros.size(); index += 1)
	{
		std::string macro = macros[index];
		
		int split_point = macro.find_first_of("=");
		
		std::string tofind = macro.substr(0, split_point);
		std::string toreplace = macro.substr(split_point + 1);
		
		trim(&tofind);
		trim(&toreplace);
		
		size_t found = input->find("$(" + tofind + ")");
		
		while(found != std::string::npos)
		{
			//The '+ 3' is to account for the dollar sign and parens
			input->replace(found, tofind.length() + 3, toreplace);
			
			found = input->find("$(" + tofind + ")");
		}
	}
}

/**
 * Splits a string on all instances of a given string
 *
 * @param[in]  input     The string to split
 * @param[in]  split     The character string to split on
 * @param[out] output    A list of the character ranges between the
 *                       instances of the split string.
 */
void slice(std::string input, std::string split, std::vector<std::string>* output)
{
	unsigned start = 0;
	size_t split_point = 0;
	
	do
	{	
		split_point = input.find_first_of(split.c_str(), start);
		
		output->push_back(input.substr(start, split_point));
		
		start = split_point + 1;
	} while(split_point != std::string::npos);
}

/**
 * atoi, doesn't touch value in output_location if input is bad
 *
 * @param[in]  input
 * 
 */
void to_int(std::string& input, unsigned* output_location)
{
	if (input.empty())	{ return; }

	unsigned output;
	std::stringstream convert(input);
	
	if( !(convert >> output))	{ return; }
	
	*output_location = output;
}

/**
 * hex string to int, doesn't touch value in output_location if input is bad
 */
void hex_to_int(std::string& input, unsigned* output_location)
{
	if (input.empty())	{ return; }

	unsigned output;
	std::stringstream convert;
	
	convert << std::hex << input;
	
	if( !(convert >> output))	{ return; }
	
	*output_location = output;
}

/**
 * Tries to return the input string up to the first instance of search. If 
 * search isn't found, it returns the whole string. The input string is erased 
 * up to and including the search string.
 */
std::string split_on(std::string* input, std::string search)
{
	unsigned split_point = input->find(search.c_str());
	
	if(split_point == std::string::npos)	{ split_point = input->length(); }
	
	std::string output = input->substr(0, split_point);
	
	input->erase(0, split_point + search.length());
	
	trim(&output);
	
	return output;
}

/**
 * Similar to split_on for use on sections like a : b where a might exist 
 * without b. Optional_seq is an optional separator ':' in the above section. 
 * Mandatory_seq is an end separator. Output is a pair of strings, the second of
 * which will be an empty string if the optional separator isn't found.
 */
std::pair<std::string, std::string> split_optional( std::string* line, 
                                                    std::string optional_seq, 
                                                    std::string mandatory_seq)
{
	std::pair<std::string, std::string> output;
	
	if(line->find(optional_seq) != line->npos)
	{
		std::string first = split_on(line, optional_seq);
		std::string second = split_on(line, mandatory_seq);
			
		return make_pair(first, second);
	}
	else
	{		
		return make_pair(split_on(line, mandatory_seq), "");
	}
}
