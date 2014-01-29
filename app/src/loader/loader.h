#ifndef DYPC_LOADER_H_
#define DYPC_LOADER_H_

#include "../point.h"
#include "../statistics.h"
#include "../frustum.h"

#include <stdexcept>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class wxWindow;

namespace dypc {

class loader {
public:
	struct request_t {
		glm::vec3 position;
		glm::vec3 velocity;
		glm::quat orientation;
		frustum view_frustum;
	};
	
private:
	struct configuration_t {
		std::chrono::milliseconds check_interval;
		bool check_condition;
	};

	point_buffer_t points_;
	std::size_t points_count_;
	std::size_t points_capacity_;

	request_t next_request_;
	std::mutex next_request_mutex_;
	
	std::atomic_bool finished_;
	std::atomic_bool stop_;
	std::thread thread_;

	std::chrono::milliseconds last_compute_duration_;

	void thread_main_(configuration_t);

protected:
	virtual void compute_points_(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) = 0;
	virtual bool should_compute_points_(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) = 0;

public:
	loader();
	virtual ~loader();

	void update_request(const glm::vec3& position, const glm::vec3& velocity, const glm::quat& orientation, const frustum& view_frustum);
	
	std::size_t new_points_available() const;
	bool finished() const { return finished_; }
	std::chrono::milliseconds last_compute_duration();
	void reset(point_buffer_t buffer, std::size_t output_buffer_capacity_);
	
	bool running() const { return thread_.joinable(); }
	void stop();
	void start(std::chrono::milliseconds check_interval, bool check_condition);
	void update_now();
	
	virtual ::wxWindow* create_panel(::wxWindow* parent);
};
	
}

#endif
