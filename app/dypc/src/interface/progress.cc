#include "progress.h"
#include "dypc.h"
#include "../util.h"
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
		print_ident();
		std::cout << label_ << std::endl;
	}
	
	void print_ident() const {
		for(std::ptrdiff_t i = 0; i < depth_; ++i) std::cout << "   ";
	}
	
	void set(unsigned val) {
		value_ = val;
		move_before_last_bar_();
		print_ident();
		print_bar_();
	}
	
	static dypc_progress open(const char* label, unsigned maximum, dypc_progress par) {
		progress_view* parent = (progress_view*)par;
		progress_view* progress = new progress_view(label, maximum, parent);
		return (dypc_progress)progress;
	}
	
	static void set(dypc_progress pr, unsigned value) {
		progress_view* progress = (progress_view*)pr;
		progress->set(value);
	}
	
	static void message(dypc_progress pr, const char* msg) {
		progress_view* progress = (progress_view*)pr;
		progress->print_ident();
		std::cout << msg << std::endl;
	}
	
	static void close(dypc_progress pr) {
		progress_view* progress = (progress_view*)pr;
		delete progress;
	}
};


static const dypc_progress_callbacks default_callbacks_ = { &progress_view::open, &progress_view::close, &progress_view::set, &progress_view::message };


dypc_progress_callbacks dypc_current_progress_callbacks = default_callbacks_;

void dypc_set_progress_callbacks(const dypc_progress_callbacks* callbacks) {
	DYPC_INTERFACE_BEGIN;
	dypc_current_progress_callbacks = *callbacks;
	DYPC_INTERFACE_END;
}

void dypc_get_progress_callbacks(dypc_progress_callbacks* callbacks) {
	DYPC_INTERFACE_BEGIN;
	*callbacks = dypc_current_progress_callbacks;
	DYPC_INTERFACE_END;
}

void dypc_set_default_progress_callbacks() {
	DYPC_INTERFACE_BEGIN;
	dypc_current_progress_callbacks = default_callbacks_;
	DYPC_INTERFACE_END;
}
