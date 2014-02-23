#include "progress.h"
#include "interface/dypc.h"
#include <iostream>

static thread_local dypc_progress current_ = nullptr;

namespace dypc {

void set_progress(int value) {return;
	auto callbacks = dypc_get_callbacks();
	callbacks->set_progress(current_, value);
}

void increment_progress(int add) {return;
	auto callbacks = dypc_get_callbacks();
	auto current_value = callbacks->get_progress(current_);
	callbacks->set_progress(current_, current_value + add);
}

void progress(const std::string& label, int maximum, int step, const std::function<void()>& function) {
	std::cout << label << std::endl;
	function();
	return;
	
	dypc_progress old = current_;
	
	auto callbacks = dypc_get_callbacks();
	current_ = callbacks->open_progress(label.c_str(), maximum, current_);
	try {
		function();
		callbacks->close_progress(current_);
		current_ = old;
	} catch(...) {
		callbacks->close_progress(current_);
		current_ = old;
		throw;
	}
}

}
