#include "dypc.h"
#include <iostream>
#include <vector>
#include <stack>
#include <string>

static dypc_progress_id last_progress_id_ = 0;

struct progress {
	progress(const char* lbl, unsigned max) :
		label(lbl), id(++last_progress_id_), maximum(max), value(0), step_size(max / 100) { }
	
	std::string label;
	dypc_progress_id id;
	unsigned maximum;
	unsigned value;
	unsigned step_size;
};

static std::vector<std::stack<progress>> progresses_ = std::vector<std::stack<progress>>();


static dypc_progress_id open_progress_(const char* label, int maximum, dypc_progress_id parent) {
	if(parent) {
		for(auto& stack : progresses_) if(stack.top().id == parent) {
			stack.emplace(label, maximum);
			for(std::ptrdiff_t i = stack.size(); i > 1; --i) std::cout << "   ";
			break;
		}
	} else {
		progresses_.emplace_back();
		progresses_.back().emplace(label, maximum);
	}
	std::cout << label << std::endl;
	return last_progress_id_;
}

static void set_progress_(dypc_progress_id id, unsigned value) {
	for(auto& stack : progresses_) if(stack.top().id == id) {
		auto& p = stack.top();
		unsigned old_value = p.value;
		if(value > p.maximum) value = p.maximum;
		if(value % p.step_size != p.value != p.step_size) {
			p.value = value;
			
			constexpr std::ptrdiff_t width = 70;
			std::ptrdiff_t split = p.value * width / p.maximum;
			for(std::ptrdiff_t i = stack.size(); i > 1; --i) std::cout << "   ";
			std::cout << '[';
			for(std::ptrdiff_t i = 0; i < width; ++i) std::cout << ((i < split) ? ' ' : '#');
			std::cout << "]\n";
		}
		break;
	}
}

static void close_progress_(dypc_progress_id id) {
	for(auto it = progresses_.begin(); it != progresses_.end(); ++it) {
		auto& stack = *it;
		if(stack.top().id == id) {
			stack.pop();
			if(stack.empty()) progresses_.erase(it);
			break;
		}
	}
}

static void error_message_(const char* title, const char* msg) {
	std::cerr << title << ": \n" << msg << std::endl;
}

static int user_choice_(const char* title, const char** choices) {
	unsigned n = 0;
	while(choices++) {
		std::cout << '(' << ++n << ") " << *choices << std::endl;
	}
	unsigned input = 0;
	do {
		std::string line;
		std::cout << "--> " << std::flush;
		std::getline(std::cin, line);
		input = std::stoul(line);
	} while(input == 0 || input > n);
	return input - 1;
}



static dypc_callbacks callbacks_ = {
	&open_progress_,
	&set_progress_,
	&close_progress_,
	&error_message_
};


dypc_callbacks* dypc_get_callbacks() {
	return &callbacks_;
}
