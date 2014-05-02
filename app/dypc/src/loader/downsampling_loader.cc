#include "downsampling_loader.h"
#include "downsampling_controller/pid_downsampling_controller.h"
#include "../progress.h"

namespace dypc {

downsampling_loader::downsampling_loader() {
	downsampling_controller_.reset(new pid_downsampling_controller());
}

double downsampling_loader::get_setting(const std::string& setting) const {
	if(setting == "adaptive") return adaptive_ ? 1.0 : 0.0;
	else if(setting == "downsampling_setting") return downsampling_setting_;
	else return loader::get_setting(setting);
}

void downsampling_loader::set_setting(const std::string& setting, double value) {
	if(setting == "adaptive") adaptive_ = (value != 0.0);
	else if(setting == "downsampling_setting") downsampling_setting_ = value;
	else loader::set_setting(setting, value);
}

void downsampling_loader::compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) {
	if(!adaptive_ || !downsampling_controller_) {
		count = this->compute_downsampled_points_(points, capacity, req);
		return;
	}
	
	std::size_t expected_output = expected_output_ratio_ * capacity;
	std::size_t attempts_remaining = maximal_attempts_;
	float error;
	do {
		count = this->compute_downsampled_points_(points, capacity, req);
		error = ((float)count - expected_output)/capacity;
		downsampling_setting_ = downsampling_controller_->adapt_setting(downsampling_setting_, error);
	} while(--attempts_remaining && count > capacity);
	
	std::size_t output_change_threshold = output_change_threshold_ratio_ * capacity;
	bool reached_maximum = (count < capacity) && (count >= previous_output_) && ((count - previous_output_) < output_change_threshold);
	should_recompute_ = std::abs(error) > error_tolerance_ratio && !reached_maximum;
	previous_output_ = count;
}

bool downsampling_loader::should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) {
	if(loader::should_compute_points(request, previous, dtime)) return true;
	else return (adaptive_ && downsampling_controller_ && should_recompute_);
}


void downsampling_loader::generate_setting_output_statistics(std::ostream& output, size_t capacity, const loader::request_t& req, float min_setting, float max_setting, float step) {
	float old_setting = downsampling_setting_;
	std::unique_ptr<point[]> buffer(new point[capacity]);
	progress((max_setting - min_setting)/step, "Generating setting-output points statistics...", [&](progress_handle& pr) {
		for(float setting = min_setting; setting <= max_setting; setting += step) {
			downsampling_setting_ = setting;
			std::size_t count = this->compute_downsampled_points_(buffer.get(), capacity, req);
			output << setting << " " << count << std::endl;
			pr.increment();
		}
	});
	downsampling_setting_ = old_setting;
}

}
