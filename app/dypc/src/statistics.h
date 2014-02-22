#ifndef DYPC_STATISTICS_H_
#define DYPC_STATISTICS_H_

#include "util.h"
#include <list>
#include <utility>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>


namespace dypc {

class statistics {
public:
	enum type_t { number = 0, file_size, percent, milliseconds };

	enum kind_t {
		no_kind = 0,
		rendered_points,
		renderer_capacity,
		model_total_points,
		loader_time,
		rom_size,
		memory_size
	};

	class item;
	
	class value {	
	private:
		long value_;
		type_t type_;
		kind_t kind_;
		
	public:
		value(long v = 0, type_t t = number, kind_t k = no_kind) : value_(v), type_(t), kind_(k) { }
		
		value& operator=(long n) { value_ = n; return *this; }
		operator long () const { return value_; }
		
		kind_t get_kind() const { return kind_; }
		
		std::string to_string() const {
			switch(type_) {
				case file_size: return file_size_to_string(value_);
				case percent: return std::to_string(value_) + "%";
				case milliseconds : return time_to_string(std::chrono::milliseconds(value_));
				default: return std::to_string(value_);
			}
		}
	};
	
	using element_t = std::pair<std::string, value>;
	using table_t = std::list<element_t>;
	using changed_table_fct_t = void(const table_t&);
	using changed_item_fct_t = void(const table_t&, const element_t&);

private:
	table_t table_;
	std::function<changed_table_fct_t> changed_table_callback_ = nullptr;
	std::function<changed_item_fct_t> changed_item_callback_ = nullptr;
	
	static statistics& get_();
	
public:
	~statistics();
	
	static void set_callbacks(const std::function<changed_table_fct_t>& ct, const std::function<changed_item_fct_t>& ci);
	static const table_t& table() { return get_().table_; }
	
	static void item_changed(element_t&);
	static item add(const std::string& name, long initial_value = 0, type_t type = number, kind_t k = no_kind);
	static void erase(item&);
};


class statistics::item {
private:
	table_t::iterator iterator_;
	mutable std::mutex mutex_;
	
public:
	explicit item(table_t::iterator it) : iterator_(it) { }
	item(item&&);
	item(const item&) = delete;
	~item();
	
	item& operator=(const item&) = delete;
	item& operator=(item&&) = delete;
	
	table_t::iterator iterator() const { return iterator_; }
	
	operator long () const { return iterator_->second; }
	const item& operator=(long val) const;
};

}

#endif
