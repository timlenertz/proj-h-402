#ifndef DYPC_UPDATER_H_
#define DYPC_UPDATER_H_

#include <dypc/dypc.h>

#include <stdexcept>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace dypc {

class updater {
private:
	dypc_loader loader_;
	std::chrono::milliseconds check_interval_ = std::chrono::milliseconds(100);
	bool check_condition_ = true;
	std::mutex configuration_mutex_;

	dypc_points_buffer points_;
	std::size_t points_count_;
	std::size_t points_capacity_;

	dypc_loader_request next_request_;
	std::mutex next_request_mutex_;
	
	std::atomic_bool finished_;
	std::atomic_bool stop_;
	std::atomic_bool force_update_;
	std::thread thread_;

	std::chrono::milliseconds last_compute_duration_ = std::chrono::milliseconds(0);

	void thread_main_();

public:
	updater();
	~updater();
	
	void switch_loader(dypc_loader);
	void delete_loader();
	void access_loader(std::function<void(dypc_loader)>);
	double get_loader_setting(const std::string&);
	void set_loader_settings(const std::map<std::string, double>&);
	void set_loader_setting(const std::string& setting, double value) { set_loader_settings({{setting, value}}); }
	
	void set_check_interval(std::chrono::milliseconds);
	void set_check_condition(bool);
	std::chrono::milliseconds get_check_interval() const { return check_interval_; }
	bool get_check_condition() const { return check_condition_; }

	void set_request(const glm::vec3& position, const glm::vec3& velocity, const glm::quat& orientation, const glm::mat4& view_projection_matrix);
	const dypc_loader_request& get_request() const { return next_request_; }
	
	bool new_points_available(std::size_t&) const;
	bool is_finished() const { return finished_; }
	std::chrono::milliseconds get_last_compute_duration();
	void reset(dypc_points_buffer buffer, std::size_t output_buffer_capacity_);
	
	bool is_running() const { return thread_.joinable(); }
	void stop();
	void start();
	void update_now();
};
	
}

#endif
