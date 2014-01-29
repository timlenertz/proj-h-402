#ifndef DYPC_UTIL_H_
#define DYPC_UTIL_H_

#include <string>
#include <chrono>

namespace dypc {

std::string file_path_extension(const std::string&);
std::string file_size_to_string(std::size_t);
std::string time_to_string(std::chrono::milliseconds);
std::string float_to_string(double f, std::size_t decimal_digits = 2);

}

#endif
