#include "loader.h"
#include <iostream>

namespace dypc {

loader::loader() :
	points_(nullptr), points_count_(0), points_capacity_(0),
	finished_(true), stop_(true) { }

loader::~loader() { }

void loader::stop() {
	if(! running()) return;
	stop_ = true;
	thread_.join();
}

void loader::start(std::chrono::milliseconds check_interval, bool check_condition) {
	if(running()) return;
	stop_ = false;
	configuration_t config;
	config.check_interval = check_interval;
	config.check_condition = check_condition;
	thread_ = std::thread(&loader::thread_main_, this, config);
}

void loader::update_now() {
	if(running()) return;
	this->compute_points_(next_request_, points_, points_count_, points_capacity_);
	finished_ = true;
}

void loader::thread_main_(configuration_t config) {
	auto previous_time = std::chrono::high_resolution_clock::now();
	request_t previous_request;
	bool first_request = true;
	while(! stop_) {		
		next_request_mutex_.lock();
		request_t request = next_request_;
		next_request_mutex_.unlock();
		
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::milliseconds dtime = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_time);
		
		bool go = !config.check_condition || first_request || this->should_compute_points_(request, previous_request, dtime);
		if(go) {
			using namespace std::chrono;
			high_resolution_clock::time_point start_time = high_resolution_clock::now();
			this->compute_points_(request, points_, points_count_, points_capacity_);
			high_resolution_clock::time_point end_time = high_resolution_clock::now();
			last_compute_duration_ = duration_cast<std::chrono::milliseconds>(end_time - start_time);
			
			previous_request = request;
			previous_time = now;
			finished_ = true;
		}
		
		first_request = false;
		
		std::this_thread::sleep_for(config.check_interval);
	}
}

void loader::update_request(const glm::vec3& position, const glm::vec3& velocity, const glm::quat& orientation, const frustum& view_frustum) {
	next_request_mutex_.lock();
	next_request_.position = position;
	next_request_.velocity = velocity;
	next_request_.orientation = orientation;
	next_request_.view_frustum = view_frustum;
	next_request_mutex_.unlock();
}
	
std::size_t loader::new_points_available() const {
	if(finished_ && points_count_) return points_count_;
	else return 0;
}

std::chrono::milliseconds loader::last_compute_duration() {
	if(! finished_) throw std::logic_error("Cannot get last compute duration when it is not idle.");
	return last_compute_duration_;
}

void loader::reset(point_buffer_t buffer, std::size_t output_buffer_capacity_) {
	if(! finished_) throw std::logic_error("Cannot reset loader when it is not idle.");
	
	points_ = buffer;
	points_count_ = 0;
	points_capacity_ = output_buffer_capacity_;
	finished_ = false;
}

std::string loader::loader_name() const {
	return "Unnamed Loader";
}

::wxWindow* loader::create_panel(::wxWindow* parent) {
	return nullptr;
}


}
