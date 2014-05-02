#include "util.h"
#include <cstdio>
#include <string>
#include <memory>

namespace dypc {

std::string file_path_extension(const std::string& path) {
	auto pos = path.rfind('.');
	if(pos == std::string::npos) return "";
	else return path.substr(pos + 1);
}

std::string file_size_to_string(std::size_t sz) {
	const double k = 1024;
	const double M = k*1024;
	const double G = M*1024;
	
	if(sz >= G) return float_to_string((double)sz/G) + " GiB";
	else if(sz >= M) return float_to_string((double)sz/M) + " MiB";
	else if(sz >= k) return float_to_string((double)sz/k) + " kiB";
	else return std::to_string(sz) + " B";
}

std::string time_to_string(std::chrono::milliseconds dur) {
	long ms = dur.count();
	if(ms >= 1000) return float_to_string((double)ms/1000) + " s";
	else return std::to_string(ms) + " ms";
}

std::string float_to_string(double f, std::size_t decimal_digits) {
	char decimal[256];
	std::string format = std::string("%.") + std::to_string(decimal_digits) + "f";
	std::snprintf(decimal, sizeof(decimal), format.c_str(), f);
	return std::string(decimal);
}

}

