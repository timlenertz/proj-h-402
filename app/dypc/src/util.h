#ifndef DYPC_UTIL_H_
#define DYPC_UTIL_H_

#include <string>
#include <chrono>
#include <map>
#include <ostream>

#define DYPC_INTERFACE_BEGIN \
	try { (void)0

#define DYPC_INTERFACE_END \
	dypc_clear_error(); \
	} catch(const std::exception& ex) { \
	dypc_set_error_message(ex.what()); \
	} catch(...) { \
	dypc_set_error_message("Unknown error"); \
	} (void)0

#define DYPC_INTERFACE_END_RETURN(val, errreturn) \
	dypc_clear_error(); \
	return val; \
	} catch(const std::exception& ex) { \
	dypc_set_error_message(ex.what()); \
	return errreturn; \
	} catch(...) { \
	dypc_set_error_message("Unknown error"); \
	return errreturn; \
	} (void)0

namespace dypc {

/**
 * Gets extension from file path/name.
 * @param filepath File path or name.
 * @return File extension, or empty string if none.
 */
std::string file_path_extension(const std::string& filepath);

/**
 * Get string for file size.
 * @param sz File size in byte.
 * @return String such as "655 B", "1.54 MiB"
 */
std::string file_size_to_string(std::size_t sz);

/**
 * Get string for time duration.
 * @param t Duration in milliseconds.
 * @return String such as "54 ms", "4.3 s"
 */
std::string time_to_string(std::chrono::milliseconds t);

/**
 * Get string for decimal number.
 * @param f Number.
 * @param decimal_digits Maximal amount of decimal digits.
 * @return String such as "4.78"
 */
std::string float_to_string(double f, std::size_t decimal_digits = 2);

std::string file_formats_to_wildcard(const std::map<std::string, std::string>& file_formats);

/**
 * Get square of number.
 */
template<class Number> inline Number sq(Number n) { return n * n; } 

inline float min(float a, float b) { return (a < b ? a : b); }
inline float max(float a, float b) { return (a < b ? b : a); }

inline std::ptrdiff_t min(std::ptrdiff_t a, std::ptrdiff_t b) { return (a < b ? a : b); }
inline std::ptrdiff_t max(std::ptrdiff_t a, std::ptrdiff_t b) { return (a < b ? b : a); }

}

#endif
