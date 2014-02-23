#include "dypc.h"
#include <iostream>
#include <vector>
#include <stack>
#include <string>

struct progress {
	progress(const char* lbl, unsigned max, const progress& parent) :
		label(lbl), maximum(max), value(0), step_size(max / 100), depth(parent.depth + 1) { }
	progress(const char* lbl, unsigned max) :
		label(lbl), maximum(max), value(0), step_size(max / 100), depth(0) { }
	
	std::string label;
	unsigned maximum;
	unsigned value;
	unsigned step_size;
	unsigned depth;
};

static std::vector<std::stack<progress>> progresses_ = std::vector<std::stack<progress>>();


static dypc_progress open_progress_(const char* label, int maximum, dypc_progress parent) {
	dypc_progress p = nullptr;
	if(parent) {
		for(auto& stack : progresses_) if(&stack.top() == (progress*)&parent) {
			stack.emplace(label, maximum, stack.top());
			p = &stack.top();
			for(std::ptrdiff_t i = stack.size(); i > 1; --i) std::cout << "   ";
			break;
		}
	} else {
		progresses_.emplace_back();
		progresses_.back().emplace(label, maximum);
		p = &progresses_.back().top();
	}
	std::cout << label << std::endl;
	return p;
}

static void set_progress_(dypc_progress id, unsigned value) {
	progress& p = *(progress*)id;
	
	unsigned old_value = p.value;
	if(value > p.maximum) value = p.maximum;
	if(value/p.step_size != p.value/p.step_size) {
		p.value = value;
		
		constexpr std::ptrdiff_t width = 70;
		std::ptrdiff_t split = p.value * width / p.maximum;
		for(std::ptrdiff_t i = p.depth; i > 1; --i) std::cout << "   ";
		std::cout << '[';
		for(std::ptrdiff_t i = 0; i < width; ++i) std::cout << ((i < split) ? '#' : ' ');
		std::cout << "]\n";
	}
}

static unsigned get_progress_(dypc_progress id) {
	progress& p = *(progress*)id;
	return p.value;
}

static void close_progress_(dypc_progress id) {
	for(auto it = progresses_.begin(); it != progresses_.end(); ++it) {
		auto& stack = *it;
		if(&stack.top() == (progress*)&id) {
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
	&get_progress_,
	&close_progress_,
	&error_message_
};


dypc_callbacks* dypc_get_callbacks() {
	return &callbacks_;
}
