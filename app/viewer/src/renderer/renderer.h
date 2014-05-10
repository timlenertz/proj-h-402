#ifndef DYPC_RENDERER_H_
#define DYPC_RENDERER_H_

#include "../gl.h"
#include "program.h"
#include "../updater/updater.h"

#include <dypc/dypc.h>

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
#include <functional>

namespace dypc {

class point;
class loader;

/**
 * OpenGL renderer for point cloud.
 * Renders point sets outputted by loader. Operates using buffer swapping: There are two point buffers allocated (OpenGL buffer object), where one is used for rendering while the other one is periodically being filled by the loader with a new point set on a different thread. Upon completion, the buffers' roles are swapped.
 * Contains the updater, which periodically takes input from a loader. @see updater
 * Also handles motion (using velocity vector, and simulated damping), and shaders.
 */
class renderer {
private:
	updater updater_; ///< The updater.
	
	std::size_t point_buffer_capacity_ = 1000000; ///< Capacity for point buffers.
	GLuint renderer_point_buffer_ = 0; ///< OpenGL buffer object used for rendering.
	GLuint loader_point_buffer_ = 0; ///< OpenGL buffer object used for filling by loader.
	std::size_t renderer_point_buffer_size_ = 0; ///< Number of points in rendered buffer.
		
	const float viewport_width_; ///< Width of viewport.
	const float viewport_height_; ///< Height of viewport.
	float fov_ = 60.0; ///< Field of view.
	float background_color_[3] = { 0.0, 0.0, 0.0 }; ///< Background color.
	
	bool shadow_ = false;

	glm::mat4 projection_matrix_; ///< Current projection matrix. Depends on FOV, viewport size.
	glm::mat4 view_matrix_; ///< Current view matrix. Depends on camera position and orientation.
	
	program* shaders_; ///< Shaders program used.
	GLint projection_matrix_uniform_; ///< Shader uniform variable for projection matrix.
	GLint view_matrix_uniform_; ///< Shader uniform variable for view matrix.
	GLint fog_uniform_;
	GLint background_color_uniform_;
	GLint fog_distance_uniform_;
	GLint shadow_uniform_;
	GLint shadow_point_size_uniform_;
	GLint maximal_shadow_distance_uniform_;
	
	float scale_ = 1.0;
	glm::vec3 position_; ///< Current camera position.
	glm::quat orientation_; ///< Current camera orientation as quaternion.
	glm::vec3 velocity_; ///< Current camera velocity.
	glm::vec3 view_target_velocity_;
	
	std::function<void()> callback_;

	void compute_projection_matrix_();
	void compute_view_matrix_();
	void compute_motion_(float dtime);
	void compute_fps_();
	
	void initialize_point_buffers_();
	void update_request_();
	void check_updater_();
	
	void initialize_gl_();

public:
	renderer(float viewport_width, float viewport_height); ///< Create renderer.
	~renderer();
	
	void draw(float dtime = 0.0);
	
	const glm::vec3& get_position() const { return position_; } ///< Get current camera position vector.
	const glm::vec3& get_velocity() const { return velocity_; } ///< Get current camera velocity vector.
	
	void set_view_target_velocity(float x, float y, float z) { view_target_velocity_ = glm::vec3(x, y, z); }
	void rotate_camera(float horizontalAngle, float verticalAngle, float rollAngle = 0.0);
	
	void set_configuration(float fov, float scale, unsigned char bg_r, unsigned char bg_g, unsigned char bg_b, bool fog, float fog_distance, bool depth_test, bool shadow, unsigned shadow_size, float shadow_max_distance);
	void set_point_capacity(std::size_t capacity);
	
	const updater& get_updater() const { return updater_; } ///< Get the updater.
	updater& get_updater() { return updater_; } ///< Get the updater.
	bool get_updater_paused() const { return ! updater_.is_running(); }
	void set_updater_paused(bool p);
	std::chrono::milliseconds get_updater_check_interval() const { return updater_.get_check_interval(); }
	void set_updater_check_interval(std::chrono::milliseconds i) { updater_.set_check_interval(i); }
	bool get_updater_check_condition() const { return updater_.get_check_condition(); }
	void set_updater_check_condition(bool c) { updater_.set_check_condition(c); }
	
	void set_loader_downsampling_setting(float setting);
	float get_loader_downsampling_setting();
	void set_loader_adaptive(bool adaptive);
	bool get_loader_adaptive();
	
	void set_callback(const std::function<void()>& f) { callback_ = f; }
	void clear_callback() { callback_ = nullptr; }
	std::function<void()> get_callback() const { return callback_; }
	
	std::size_t get_capacity() const { return point_buffer_capacity_; }
	std::size_t get_rendered_points() const { return renderer_point_buffer_size_; } 
	
	void update_now();
	void switch_loader(dypc_loader);
	void delete_loader() { switch_loader(nullptr); }
};

}

#endif
