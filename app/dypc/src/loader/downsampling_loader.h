#ifndef DYPC_DOWNSAMPLING_LOADER_H_
#define DYPC_DOWNSAMPLING_LOADER_H_

#include "loader.h"
#include "downsampling_controller/downsampling_controller.h"
#include <memory>
#include <utility>
#include <ostream>

namespace dypc {

/**
 * Loader that involves downsampling.
 * Adaptive downsampling functionality not implemented.
 */
class downsampling_loader : public loader {
private:
	static constexpr float initial_downsampling_setting_ = 20.0;
	static constexpr std::size_t maximal_attempts_ = 10;
	static constexpr float expected_output_ratio_ = 0.4;
	static constexpr float error_tolerance_ratio = 0.1;
	static constexpr float output_change_threshold_ratio_ = 0.1;
	
protected:
	bool adaptive_ = false; ///< Whether the loader is adaptive. Not implemented.
	float downsampling_setting_ = initial_downsampling_setting_; ///< Current downsampling setting.
	std::unique_ptr<downsampling_controller> downsampling_controller_; ///< Downsampling setting controller.
	bool should_recompute_ = false;
	std::size_t previous_output_ = 0;
	
	virtual std::size_t compute_downsampled_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) = 0;
	
public:
	downsampling_loader();

	void compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) override final;
	bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) override;

	bool get_adaptive() const { return adaptive_; }
	void set_adaptive(bool b) { adaptive_ = b; }
	
	float get_downsampling_setting() const { return downsampling_setting_; }
	void set_downsampling_setting(float s) { downsampling_setting_ = s; }
	
	const downsampling_controller& get_downsampling_controller() const { return *downsampling_controller_; }
	void set_downsampling_controller(std::unique_ptr<downsampling_controller> ctr) { downsampling_controller_ = std::move(ctr); }
	
	double get_setting(const std::string&) const override;
	void set_setting(const std::string&, double) override;
	
	void generate_setting_output_statistics(std::ostream& output, size_t capacity, const loader::request_t& req, float min_setting, float max_setting, float step);
};

}

#endif
