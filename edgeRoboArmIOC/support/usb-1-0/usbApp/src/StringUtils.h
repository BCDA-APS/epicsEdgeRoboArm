#ifndef INC_STRINGUTILS_H
#define INC_STRINGUTILS_H

#include <string>
#include <vector>

void ltrim(std::string* input);
void rtrim(std::string* input);
void trim(std::string* input);

void apply_macros(std::string* input, std::vector<std::string> macros);
void slice(std::string input, std::string split, std::vector<std::string>* output);

void to_int(std::string& input, unsigned* output_location);
void hex_to_int(std::string& input, unsigned* output_location);

std::string split_on(std::string* input, std::string search);
std::pair<std::string, std::string> split_optional( std::string* line, 
                                                    std::string optional_seq, 
                                                    std::string mandatory_seq);

#endif
