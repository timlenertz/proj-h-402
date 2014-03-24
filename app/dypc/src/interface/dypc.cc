#include "dypc.h"
#include <iostream>
#include <vector>
#include <stack>
#include <string>


class progress_view {
private:
	progress_view* parent_;
	std::ptrdiff_t depth_;
	unsigned maximum_;
	unsigned value_;
	std::string label_;

	void print_ident_() const {
		for(std::ptrdiff_t i = 0; i < depth_; ++i) std::cout << "   ";
	}
	
	void print_bar_() const {
		const std::ptrdiff_t len = 70;
		if(maximum_ != 0) {
			const std::ptrdiff_t split = len * value_ / maximum_;
			std::cout << '[';
			for(std::ptrdiff_t i = 0; i < len; ++i) std::cout << (i < split ? '=' : ' ');
			std::cout << ']';			
		}
	}

	void move_before_last_bar_() const {
		std::ptrdiff_t len = 72 + 3 * depth_;
		for(std::ptrdiff_t i = 0; i < len; ++i) std::cout << '\b';
	}

public:
	progress_view(const std::string& lab, unsigned mx, progress_view* par) :
	parent_(par), maximum_(mx), value_(0), label_(lab) {
		depth_ = 0;
		while(par) {
			++depth_;
			par = par->parent_;
		}
		
		std::cout << std::endl;
		print_ident_();
		std::cout << label_ << std::endl;
	}
	
	void set(unsigned val) {
		value_ = val;
		move_before_last_bar_();
		print_ident_();
		print_bar_();
	}
};


static dypc_progress open_progress_(const char* label, unsigned maximum, dypc_progress par) {
	progress_view* parent = (progress_view*)par;
	progress_view* progress = new progress_view(label, maximum, parent);
	return (dypc_progress)progress;
}

static void set_progress_(dypc_progress pr, unsigned value) {
	progress_view* progress = (progress_view*)pr;
	progress->set(value);
}

static void close_progress_(dypc_progress pr) {
	progress_view* progress = (progress_view*)pr;
	delete progress;
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
