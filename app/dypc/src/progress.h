#ifndef DYPC_PROGRESS_H_
#define DYPC_PROGRESS_H_

#include <functional>
#include <string>
#include <type_traits>
#include "interface/progress.h"

namespace dypc {

/**
 * Handle to progress indicator view.
 * Uses callbacks set by library user
 */
class progress_handle {
private:
	dypc_progress progress_; ///< Progress handle from interface.
	unsigned value_; ///< Current progress value.
public:
	progress_handle() = default; ///< Create stub progress handle.
	explicit progress_handle(dypc_progress p) : progress_(p), value_(0) { } ///< Create progress handle bound to a dypc_progress.
	
	/**
	 * Set progress.
	 * Calls interface callback.
	 * @param v New value, must be between 0 and the maximum.
	 */
	void set(unsigned v) {
		value_ = v;
		dypc_current_progress_callbacks.set(progress_, value_);
	}
	
	/**
	 * Increment the progress.
	 * Calls interface callback. @see set
	 * @param i Increment step.
	 */
	void increment(unsigned i = 1) {
		set(value_ + i);
	}
	
	/**
	 * Update indeterminate progress indicator.
	 * Calls interface callback. Makes the progress indicator move to show there is some progress.
	 */
	void pulse() {
		dypc_current_progress_callbacks.pulse(progress_);
	}
	
	/**
	 * Display a message.
	 * Calls interface callback.
	 * @param msg Message to display.
	 */
	void message(const std::string& msg) {
		dypc_current_progress_callbacks.message(progress_, msg.c_str());
	}
};


/**
 * Execute given function, and show progress bar.
 * Progress bar is handled by process_callbacks set by library user.
 * @see dypc_set_progress_callbacks
 * @param total Maximal value for progress bar.
 * @param label Label for progress bar.
 * @param func Function that is called. Takes one progress_handle attribute, by non-const reference
 */
template<class Function>
void progress(std::size_t total, const std::string& label, Function func) {
	static thread_local dypc_progress current = nullptr; ///< Current progress, for this thread

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


/**
 * Execute given function, and show indeterminate progress indicator.
 * Progress bar is handled by process_callbacks set by library user.
 * @see dypc_set_progress_callbacks
 * @param label Label for progress bar.
 * @param func Function that is called. Takes one progress_handle attribute, by non-const reference.
 */
template<class Function>
inline void progress(const std::string& label, Function func) {
	progress(0, label, func);
}


/**
 * Iterate through list, and and show progress bar.
 * @see progress
 * @param begin Iterator to start of list.
 * @param end Iterator to end of list.
 * @param total Number of elements in list.
 * @param label Label for progress bar.
 * @param func Function that is called. Takes argument with dereference type of iterator, by reference.
 */
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


/**
 * Iterate through list, and and show progress bar.
 * Version that breaks when function returns false.
 * @see progress
 * @param begin Iterator to start of list.
 * @param end Iterator to end of list.
 * @param total Number of elements in list.
 * @param label Label for progress bar.
 * @param func Function that is called. Takes argument with dereference type of iterator, by reference. Break when return false.
 */
template<class Iterator, class Function>
void progress_foreach_break(Iterator begin, Iterator end, std::size_t total, const std::string& label, Function func) {
	progress(100, label, [&](progress_handle& pr) {
		std::size_t update_step = total/100;
		std::size_t counter = 0;
		for(Iterator it = begin; it != end; ++it, ++counter) {
			if(! func(*it)) break;
			if(counter == update_step) {
				pr.increment();
				counter = 0;
			}
		}
	});
}


/**
 * Iterate through list, and and show progress bar.
 * Must be possible to subtract iterators. If not, use version with total number argument.
 * @see progress
 * @param begin Iterator to start of list.
 * @param end Iterator to end of list.
 * @param label Label for progress bar.
 * @param func Function that is called. Takes attribute with dereference type of iterator, by reference.
 */
template<class Iterator, class Function>
inline void progress_foreach(Iterator begin, Iterator end, const std::string& label, Function func) {
	progress_foreach(begin, end, end - begin, label, func);
}


/**
 * Iterate through list, and and show progress bar.
 * @see progress
 * @param container Container to iterate through.
 * @param label Label for progress bar.
 * @param func Function that is called. Takes attribute with dereference type of iterator, by reference.
 */
template<class Container, class Function>
inline void progress_foreach(Container& container, const std::string& label, Function func) {
	progress_foreach(container.begin(), container.end(), container.size(), label, func);
}


/**
 * Count, and and show progress bar.
 * @see progress
 * @param begin Start value of counter.
 * @param end End value of counter.
 * @param label Label for progress bar.
 * @param func Function that is called. Takes attribute of type ptrdiff_t, by reference.
 */
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
