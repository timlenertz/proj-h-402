#include "dypc.h"
#include "../util.h"
#include <string>
#include <iostream>

int dypc_error = 0;

static std::string error_message_ = std::string("");


const char* dypc_error_message() {
	return error_message_.c_str();
}

void dypc_clear_error() {
	dypc_error = 0;
	error_message_.clear();
}

void dypc_set_error_message(const char* msg) {
	std::cerr << "Error: " << msg << std::endl;

	dypc_error = 1;
	error_message_ = msg;
}
