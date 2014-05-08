#ifndef DYPC_UTIL_H_
#define DYPC_UTIL_H_

#include <string>
#include <chrono>
#include <map>
#include <ostream>
#include <random>

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



template<class Number> inline Number sq(Number n) { return n * n; } ///< Get square of number.

inline float min(float a, float b) { return (a < b ? a : b); } ///< Get minimum of two values.
inline float max(float a, float b) { return (a < b ? b : a); } ///< Get maximum of two values.

inline std::ptrdiff_t min(std::ptrdiff_t a, std::ptrdiff_t b) { return (a < b ? a : b); } ///< Get minimum of two values.
inline std::ptrdiff_t max(std::ptrdiff_t a, std::ptrdiff_t b) { return (a < b ? b : a); } ///< Get maximum of two values.

/**
 * Epsilon value used to compare floats to approximate equality.
 * Two floating point values will be considered equal after several operations, if their difference is less than this value.
 */
constexpr float float_comparison_epsilon = 0.0001;

/**
 * Random number generator type used in the library.
 */
using random_generator_t = std::mt19937;

/**
 * Check if two floating point values are approximately equal.
 * @param a First value.
 * @param b Second value.
 * @param ep Epsilon value.
 */
inline bool approximately_equal(float a, float b, float ep = float_comparison_epsilon) {
	if(a == b) return true;
	else if(a > b) return (a - b) <= ep;
	else return (b - a) <= ep;
}


/**
 * Fill container up with duplicates.
 * Takes random values from the container any appends duplicates to it, until the container reaches
 * a given size. If \a target_size is smaller or equal to the current number of elements in the container,
 * does nothing.
 * @param values Container to fill up.
 * @param target_size Number of elements in container after completion.
 */
template<class Container>
void fill_with_duplicates(Container& values, std::size_t target_size) {
	std::size_t current_size = values.size();
	if(current_size <= target_size) return;
	std::size_t remaining = target_size - current_size;
	random_generator_t random_generator;
	std::uniform_int_distribution<std::ptrdiff_t> distribution(0, current_size - 1);
	while(remaining--) {
		const auto& el = values[distribution(random_generator)];
		values.push_back(el);
	}
}

}

#endif
