#include "updater.h"

namespace dypc {

updater::updater() :
	points_(nullptr), points_count_(0), points_capacity_(0),
	finished_(true), stop_(true), force_update_(false) { }


updater::~updater() {
	stop();
}


void updater::set_check_interval(std::chrono::milliseconds interval) {
	configuration_mutex_.lock();
	check_interval_ = interval;
	configuration_mutex_.unlock();
}


void updater::set_check_condition(bool check) {
	configuration_mutex_.lock();
	check_condition_ = check;
	configuration_mutex_.unlock();
}


void updater::switch_loader(loader* ld) {
	configuration_mutex_.lock();
	loader_.reset(ld);
	force_update_ = true;
	configuration_mutex_.unlock();
}


void updater::delete_loader() {
	switch_loader(nullptr);
}


void updater::stop() {
	if(! is_running()) return;
	stop_ = true;
	thread_.join();
}

void updater::start() {
	if(is_running() || !loader_) return;
	force_update_ = true;
	stop_ = false;
	finished_ = false;
	thread_ = std::thread(&updater::thread_main_, this);
}

void updater::update_now() {
	if(is_running()) {
		force_update_ = true;
	} else {
		loader_->compute_points(next_request_, points_, points_count_, points_capacity_);
		finished_ = true;
	}
}

void updater::thread_main_() {
	auto previous_time = std::chrono::high_resolution_clock::now();
	loader::request_t previous_request;
	while(! stop_) {		
		configuration_mutex_.lock();
		
		next_request_mutex_.lock();
		loader::request_t request = next_request_;
		next_request_mutex_.unlock();
			
		auto now = std::chrono::high_resolution_clock::now();	
		std::chrono::milliseconds dtime = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_time);
				
		if(loader_ && (force_update_ || !check_condition_ || loader_->should_compute_points(request, previous_request, dtime))) {
			using namespace std::chrono;

			high_resolution_clock::time_point start_time = high_resolution_clock::now();
			loader_->compute_points(request, points_, points_count_, points_capacity_);
			high_resolution_clock::time_point end_time = high_resolution_clock::now();
			last_compute_duration_ = duration_cast<std::chrono::milliseconds>(end_time - start_time);
			
			previous_request = request;
			previous_time = now;
			
			finished_ = true;
			force_update_ = false;
		}
			
		configuration_mutex_.unlock();
		
		while(finished_) std::this_thread::sleep_for(check_interval_);
	}
}


void updater::set_request(const glm::vec3& position, const glm::vec3& velocity, const glm::quat& orientation, const glm::mat4& view_projection_matrix) {
	next_request_mutex_.lock();
	next_request_.position = position;
	next_request_.velocity = velocity;
	next_request_.orientation = orientation;
	next_request_.view_projection_matrix = view_projection_matrix;
	next_request_.view_frustum = frustum(view_projection_matrix);
	next_request_mutex_.unlock();
}
	
	
bool updater::new_points_available(std::size_t& count) const {
	count = points_count_;
	return finished_;
}

std::chrono::milliseconds updater::get_last_compute_duration() {
	if(! finished_) throw std::logic_error("Cannot get last compute duration when it is not idle.");
	return last_compute_duration_;
}

void updater::reset(point_buffer_t buffer, std::size_t output_buffer_capacity_) {
	if(! finished_) throw std::logic_error("Cannot reset loader when it is not idle.");
	
	points_ = buffer;
	points_count_ = 0;
	points_capacity_ = output_buffer_capacity_;
	finished_ = false;
}


}
