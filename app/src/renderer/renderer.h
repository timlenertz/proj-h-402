#ifndef DYPC_RENDERER_H_
#define DYPC_RENDERER_H_

#include "../gl.h"
#include "program.h"
#include "../updater/updater.h"

#include "../statistics.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>

#include <cstdlib>
#include <vector>
#include <string>
#include <utility>
#include <chrono>

namespace dypc {

class point;
class loader;

class renderer {
private:
	updater updater_;
	
	std::size_t point_buffer_capacity_ = 1000000;
	GLuint renderer_point_buffer_ = 0;
	GLuint loader_point_buffer_ = 0;
	std::size_t renderer_point_buffer_size_ = 0;
		
	const float viewport_width_;
	const float viewport_height_;
	float fov_ = 60.0;
	float background_color_[3] = { 0.0, 0.0, 0.0 };
	
	bool shadow_ = false;

	glm::mat4 projection_matrix_;
	glm::mat4 view_matrix_;
	
	program* shaders_;
	GLint projection_matrix_uniform_;
	GLint view_matrix_uniform_;
	GLint fog_uniform_;
	GLint background_color_uniform_;
	GLint fog_distance_uniform_;
	GLint shadow_uniform_;
	GLint shadow_point_size_uniform_;
	GLint maximal_shadow_distance_uniform_;
	
	float scale_ = 1.0;
	glm::vec3 position_;
	glm::quat orientation_;
	glm::vec3 velocity_;
	glm::vec3 view_target_velocity_;

	statistics::item stat_capacity_;
	statistics::item stat_count_;
	statistics::item stat_loader_duration_;

	void compute_projection_matrix_();
	void compute_view_matrix_();
	void compute_motion_(float dtime);
	void compute_fps_();
	
	void initialize_point_buffers_();
	void update_request_();
	void check_updater_();
	
	void initialize_gl_();

public:
	renderer(float viewport_width, float viewport_height);
	~renderer();
	
	void draw(float dtime = 0.0);
	
	const glm::vec3& get_position() const { return position_; }
	const glm::vec3& get_velocity() const { return velocity_; }
	
	void set_view_target_velocity(float x, float y, float z) { view_target_velocity_ = glm::vec3(x, y, z); }
	void rotate_camera(float horizontalAngle, float verticalAngle, float rollAngle = 0.0);
	
	void set_configuration(float fov, float scale, unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, bool fog, float fog_distance, bool depth_test, bool shadow, unsigned shadow_size, float shadow_max_distance);
	void set_point_capacity(std::size_t capacity);
	
	bool get_updater_paused() const { return ! updater_.is_running(); }
	void set_updater_paused(bool p);
	std::chrono::milliseconds get_updater_check_interval() const { return updater_.get_check_interval(); }
	void set_updater_check_interval(std::chrono::milliseconds i) { updater_.set_check_interval(i); }
	bool get_updater_check_condition() const { return updater_.get_check_condition(); }
	void set_updater_check_condition(bool c) { updater_.set_check_condition(c); }
	
	void update_now();
	void switch_loader(loader* ld);
	void delete_loader() { switch_loader(nullptr); }
};

}

#endif
