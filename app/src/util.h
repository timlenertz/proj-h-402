#ifndef DYPC_UTIL_H_
#define DYPC_UTIL_H_

#include <string>
#include <chrono>
#include <map>

namespace dypc {

std::string file_path_extension(const std::string&);
std::string file_size_to_string(std::size_t);
std::string time_to_string(std::chrono::milliseconds);
std::string float_to_string(double f, std::size_t decimal_digits = 2);

using user_choices_t = std::map<std::string, std::string>;
std::string user_choice(const user_choices_t&, const std::string& caption = "Choose");

std::string file_formats_to_wildcard(const user_choices_t& file_formats);

template<class Number> inline Number sq(Number n) { return n * n; } 

}

#endif
