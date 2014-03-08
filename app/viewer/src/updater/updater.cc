#include "updater.h"

namespace dypc {

updater::updater() :
	loader_(nullptr), points_(nullptr), points_count_(0), points_capacity_(0),
	finished_(false), stop_(true), force_update_(false) { }


updater::~updater() {
	stop();
	if(loader_) dypc_delete_loader(loader_);
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


void updater::switch_loader(dypc_loader ld) {
	configuration_mutex_.lock();
	if(loader_) dypc_delete_loader(loader_);
	loader_ = ld;
	force_update_ = true;
	configuration_mutex_.unlock();
}


void updater::delete_loader() {
	switch_loader(nullptr);
}


void updater::access_loader(std::function<void(dypc_loader)> fct) {
	configuration_mutex_.lock();
	try {
		fct(loader_);
		configuration_mutex_.unlock();
	} catch(...) {
		configuration_mutex_.unlock();
		throw;
	}
}

double updater::get_loader_setting(const std::string& setting) {
	double value;
	access_loader([&](dypc_loader ld) {
		value = dypc_loader_get_setting(ld, setting.c_str());
	});
	return value;
}


void updater::set_loader_settings(const std::map<std::string, double>& settings) {
	access_loader([&](dypc_loader ld) {
		for(const auto& p : settings) dypc_loader_set_setting(ld, p.first.c_str(), p.second);
	});
	update_now();
}


void updater::stop() {
	if(! is_running()) return;
	stop_ = true;
	thread_.join();
}

void updater::start() {
	if(is_running()) return;
	force_update_ = true;
	stop_ = false;
	finished_ = false;
	thread_ = std::thread(&updater::thread_main_, this);
}

void updater::update_now() {
	if(is_running()) {
		force_update_ = true;
	} else if(loader_) {
		dypc_size count = points_capacity_;
		dypc_loader_compute_points(loader_, &next_request_, points_, &count);
		points_count_ = count;
		finished_ = true;
	}
}


void updater::thread_main_() {
	auto previous_time = std::chrono::high_resolution_clock::now();
	dypc_loader_request previous_request;
	while(! stop_) {		
		configuration_mutex_.lock();
		
		next_request_mutex_.lock();
		dypc_loader_request request = next_request_;
		next_request_mutex_.unlock();
			
		auto now = std::chrono::high_resolution_clock::now();	
		std::chrono::milliseconds dtime = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous_time);
				
		if(loader_ && (force_update_ || !check_condition_ || dypc_loader_should_compute_points(loader_, &request, &previous_request, dtime.count()))) {
			using namespace std::chrono;

			high_resolution_clock::time_point start_time = high_resolution_clock::now();
			
			dypc_size count = points_capacity_;
			dypc_loader_compute_points(loader_, &request, points_, &count);
			points_count_ = count;

			high_resolution_clock::time_point end_time = high_resolution_clock::now();
			last_compute_duration_ = duration_cast<std::chrono::milliseconds>(end_time - start_time);
			
			previous_request = request;
			previous_time = now;
			
			finished_ = true;
			force_update_ = false;
		}
			
		configuration_mutex_.unlock();
		
		if(last_compute_duration_ < check_interval_) std::this_thread::sleep_for(check_interval_ - last_compute_duration_);
		while(finished_ && !stop_) std::this_thread::sleep_for(check_interval_);
	}
}


void updater::set_request(const glm::vec3& position, const glm::vec3& velocity, const glm::quat& orientation, const glm::mat4& view_projection_matrix) {
	next_request_mutex_.lock();
	for(std::ptrdiff_t i = 0; i < 3; ++i) next_request_.position[i] = position[i];
	for(std::ptrdiff_t i = 0; i < 3; ++i) next_request_.velocity[i] = velocity[i];
	for(std::ptrdiff_t i = 0; i < 4; ++i) next_request_.orientation[i] = orientation[i];
	for(std::ptrdiff_t i = 0; i < 16; ++i) next_request_.view_projection_matrix[i] = view_projection_matrix[i/4][i%4];
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

void updater::reset(dypc_points_buffer buffer, std::size_t output_buffer_capacity_) {
	if(! finished_) throw std::logic_error("Cannot reset loader when it is not idle.");
	
	points_ = buffer;
	points_count_ = 0;
	points_capacity_ = output_buffer_capacity_;
	finished_ = false;
}


}
