#ifndef DYPC_RENDERER_H_
#define DYPC_RENDERER_H_

#include "../gl.h"
#include "program.h"

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
	std::size_t point_buffer_capacity_ = 300000;
	GLuint renderer_point_buffer_ = 0;
	GLuint loader_point_buffer_ = 0;
	std::size_t renderer_point_buffer_size_ = 0;
	
	loader* loader_ = nullptr;
	bool pause_loader_ = false;
	std::chrono::milliseconds loader_check_interval_ = std::chrono::milliseconds(200);
	bool loader_check_condition_ = true;
	
	const float viewport_width_;
	const float viewport_height_;
	float fov_ = 60.0;
	float background_color_[3] = { 0.0, 0.0, 0.0 };

	glm::mat4 projection_matrix_;
	glm::mat4 view_matrix_;
	
	program* shaders_;
	GLint projection_matrix_uniform_;
	GLint view_matrix_uniform_;
	GLint fog_color_uniform_;
	
	glm::vec3 position_;
	glm::quat orientation_;
	glm::vec3 velocity_;
	glm::vec3 view_target_velocity_;
	bool fix_z_orientation_;
	
	std::chrono::system_clock::time_point fps_time_;
	unsigned fps_counter_;
	
	statistics::item stat_capacity_;
	statistics::item stat_count_;
	statistics::item stat_loader_duration_;

	void compute_projection_matrix_();
	void compute_view_matrix_();
	void compute_motion_(float dtime);
	void compute_fps_();
	
	void initialize_point_buffers_();
	void update_loader_request_();
	void check_loader_();
	
	void apply_loader_configuration_();
	void delete_loader_();

	void initialize_gl_();

public:
	renderer(float viewport_width, float viewport_height);
	~renderer();
	
	void draw(float dtime = 0.0);
	
	const glm::vec3& get_position() const { return position_; }
	const glm::vec3& get_velocity() const { return velocity_; }
	
	void set_view_target_velocity(float x, float y, float z) { view_target_velocity_ = glm::vec3(x, y, z); }
	void rotate_camera(float horizontalAngle, float verticalAngle, float rollAngle = 0.0);
	
	void set_configuration(float fov, unsigned char bg_r, unsigned char bg_g, unsigned char bg_b);
	void set_point_capacity(std::size_t capacity);
	
	bool get_loader_paused() const { return pause_loader_; }
	void set_loader_paused(bool p) { pause_loader_ = p; apply_loader_configuration_(); }
	std::chrono::milliseconds get_loader_check_interval() const { return loader_check_interval_; }
	void set_loader_check_interval(std::chrono::milliseconds i) { loader_check_interval_ = i; apply_loader_configuration_(); }
	bool get_loader_check_condition() const { return loader_check_condition_; }
	void set_loader_check_condition(bool c) { loader_check_condition_ = c; apply_loader_configuration_(); }
	void set_loader_configuration(bool paused, std::chrono::milliseconds interval, bool check);
	
	void update_loader_now();
	void switch_loader(loader* ld);
	void delete_loader() { switch_loader(nullptr); }
};

}

#endif
