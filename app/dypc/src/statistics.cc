#include "statistics.h"

namespace dypc {

static statistics* instance = nullptr;

statistics& statistics::get_() {
	if(! instance) instance = new statistics;
	return *instance;
}

statistics::~statistics() {
	instance = nullptr;
}

statistics::item statistics::add(const std::string& name, long initial_value, type_t type, kind_t kind) {
	statistics& stat = get_();
	auto it = stat.table_.emplace(stat.table_.end(), name, value(initial_value, type, kind));
	if(stat.changed_table_callback_) stat.changed_table_callback_(stat.table_);
	return item(it);
}

void statistics::erase(item& itm) {
	statistics& stat = get_();
	stat.table_.erase(itm.iterator());
	if(stat.changed_table_callback_) stat.changed_table_callback_(stat.table_);
}

void statistics::item_changed(element_t& el) {
	statistics& stat = get_();
	if(stat.changed_item_callback_) stat.changed_item_callback_(stat.table_, el);
}

void statistics::set_callbacks(const std::function<changed_table_fct_t>& ct, const std::function<changed_item_fct_t>& ci) {
	statistics& stat = get_();
	stat.changed_table_callback_ = ct;
	stat.changed_item_callback_ = ci;
	stat.changed_table_callback_(stat.table_);
}

statistics::item::item(item&& itm) : iterator_(itm.iterator_) {
	itm.iterator_ = table_t::iterator();
}

statistics::item::~item() {
	if(iterator_ == table_t::iterator()) return;
	statistics::erase(*this);
}
	
const statistics::item& statistics::item::operator=(long val) const {
	mutex_.lock();
	iterator_->second = val;
	statistics::item_changed(*iterator_);
	mutex_.unlock();
	return *this;
}




}
