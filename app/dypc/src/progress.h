#ifndef DYPC_PROGRESS_H_
#define DYPC_PROGRESS_H_

#include <functional>
#include <string>
#include "interface/progress.h"

namespace dypc {

class progress_handle {
private:
	dypc_progress progress_;
	unsigned value_;
public:
	progress_handle() = default;
	explicit progress_handle(dypc_progress p) : progress_(p), value_(0) { }
	void set(unsigned v) {
		value_ = v;
		dypc_current_progress_callbacks.set(progress_, value_);
	}
	void increment(unsigned i = 1) {
		set(value_ + i);
	}
	void pulse() {
		set(0);
	}
	void message(const std::string& msg) {
		dypc_current_progress_callbacks.message(progress_, msg.c_str());
	}
};


template<class Function>
void progress(std::size_t total, const std::string& label, Function func) {
	static thread_local dypc_progress current = nullptr;

	dypc_progress previous = current;
	dypc_progress progress = dypc_current_progress_callbacks.open(label.c_str(), total, current);
	current = progress;
	
	try {
		progress_handle handle(progress);
		func(handle);
	} catch(...) {
		dypc_current_progress_callbacks.close(progress);
		current = previous;
		throw;
	}
	
	dypc_current_progress_callbacks.close(progress);
	current = previous;
}


template<class Function>
inline void progress(const std::string& label, Function func) {
	progress(0, label, func);
}


template<class Iterator, class Function>
void progress_foreach(Iterator begin, Iterator end, std::size_t total, const std::string& label, Function func) {
	progress(100, label, [&](progress_handle& pr) {
		std::size_t update_step = total/100;
		std::size_t counter = 0;
		for(Iterator it = begin; it != end; ++it, ++counter) {
			func(*it);
			if(counter == update_step) {
				pr.increment();
				counter = 0;
			}
		}
	});
}


template<class Iterator, class Function>
inline void progress_foreach(Iterator begin, Iterator end, const std::string& label, Function func) {
	progress_foreach(begin, end, end - begin, label, func);
}


template<class Container, class Function>
inline void progress_foreach(Container& container, const std::string& label, Function func) {
	progress_foreach(container.begin(), container.end(), container.size(), label, func);
}


template<class Function>
void progress_for(std::ptrdiff_t begin, std::ptrdiff_t end, const std::string& label, Function func) {
	class counter_iterator {
	private:
		std::ptrdiff_t counter_;
	public:
		explicit counter_iterator(std::ptrdiff_t c) : counter_(c) { }
		std::ptrdiff_t operator*() const { return counter_; }
		bool operator==(counter_iterator it) { return (counter_ == it.counter_); }
		bool operator!=(counter_iterator it) { return (counter_ != it.counter_); }
		counter_iterator& operator++() { ++counter_; return *this; }
		counter_iterator operator++(int) { return counter_iterator(counter_++); }
	};
	progress_foreach(counter_iterator(begin), counter_iterator(end), end - begin, label, func);
}

}

#endif
