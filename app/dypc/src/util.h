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

std::string file_formats_to_wildcard(const std::map<std::string, std::string>& file_formats);

template<class Number> inline Number sq(Number n) { return n * n; } 

inline float min(float a, float b) { return (a < b ? a : b); }
inline float max(float a, float b) { return (a < b ? b : a); }

inline std::ptrdiff_t min(std::ptrdiff_t a, std::ptrdiff_t b) { return (a < b ? a : b); }
inline std::ptrdiff_t max(std::ptrdiff_t a, std::ptrdiff_t b) { return (a < b ? b : a); }

}

#endif