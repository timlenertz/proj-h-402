#ifndef DYPC_UTIL_H_
#define DYPC_UTIL_H_

#include <string>
#include <vector>
#include <chrono>
#include <map>

namespace dypc {

std::string file_path_extension(const std::string&);
std::string file_formats_to_wildcard(const std::map<std::string, std::string>& file_formats);

std::string file_size_to_string(std::size_t);
std::string time_to_string(std::chrono::milliseconds);
std::string float_to_string(double f, std::size_t decimal_digits = 2);

std::ptrdiff_t user_choice(const std::vector<std::string>& choices, const std::string& caption);
void error_message(const std::string& msg, const std::string& title = "Error");

}

#endif
