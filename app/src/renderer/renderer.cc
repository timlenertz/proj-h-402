#include "renderer.h"
#include "shader.h"
#include "../point.h"
#include "../frustum.h"
#include "../loader/loader.h"


#include <stdexcept>
#include <iostream>
#include <utility>

#include <string>

namespace dypc {

renderer::renderer(float w, float h) : viewport_width_(w), viewport_height_(h),
stat_capacity_(statistics::add("Renderer Capacity", point_buffer_capacity_, statistics::number, statistics::renderer_capacity)),
stat_count_(statistics::add("Renderer Points", 0, statistics::number, statistics::rendered_points)),
stat_loader_duration_(statistics::add("Loader Time", 0, statistics::milliseconds, statistics::loader_time))
{
	initialize_glew();
		
	shaders_ = new program({
		shader::read_file(GL_VERTEX_SHADER, "res/vertex_shader.glsl"),
		shader::read_file(GL_FRAGMENT_SHADER, "res/fragment_shader.glsl")
	});
	projection_matrix_uniform_ = shaders_->uniform_location("projection_matrix");
	view_matrix_uniform_ = shaders_->uniform_location("view_matrix");
	fog_color_uniform_ = shaders_->uniform_location("fog_color");
	
	compute_projection_matrix_();
	compute_view_matrix_();
	glUniform3fv(fog_color_uniform_, 1, background_color_);
	
	initialize_point_buffers_();
	
	apply_loader_configuration_();
	
	initialize_gl_();
}

renderer::~renderer() {
	if(loader_) delete loader_;
	if(shaders_) delete shaders_;
	if(renderer_point_buffer_) glDeleteBuffers(1, &renderer_point_buffer_);
	if(loader_point_buffer_) glDeleteBuffers(1, &loader_point_buffer_);
}

void renderer::initialize_point_buffers_() {
	if(loader_) loader_->stop();
	
	if(! renderer_point_buffer_) glGenBuffers(1, &renderer_point_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, renderer_point_buffer_);
	glBufferData(GL_ARRAY_BUFFER, point_buffer_capacity_*sizeof(point), nullptr, GL_STREAM_DRAW);

	if(! loader_point_buffer_) glGenBuffers(1, &loader_point_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, loader_point_buffer_);
	glBufferData(GL_ARRAY_BUFFER, point_buffer_capacity_*sizeof(point), nullptr, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	stat_count_ = 0;
	renderer_point_buffer_size_ = 0;
}

void renderer::update_loader_request_() {
	if(loader_) loader_->update_request(-position_, -velocity_, orientation_, frustum(projection_matrix_ * view_matrix_));
}

void renderer::check_loader_() {
	if(!loader_ || !loader_->finished()) return;

	glBindBuffer(GL_ARRAY_BUFFER, loader_point_buffer_);	
	if(std::size_t sz = loader_->new_points_available()) {
		stat_loader_duration_ = loader_->last_compute_duration().count();
		auto valid = glUnmapBuffer(GL_ARRAY_BUFFER);
		if(valid) {
			std::swap(loader_point_buffer_, renderer_point_buffer_);
			stat_count_ = sz;
			renderer_point_buffer_size_ = sz;
		}
	} else {
		glUnmapBuffer(GL_ARRAY_BUFFER); // does nothing if buffer was not mapped
		renderer_point_buffer_size_ = 0;
		stat_count_ = 0;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, loader_point_buffer_);	
	auto buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if(buf) loader_->reset(reinterpret_cast<point_buffer_t>(buf), point_buffer_capacity_);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderer::compute_projection_matrix_() {
	projection_matrix_ = glm::perspective(fov_, viewport_width_/viewport_height_, 0.1f, 1000.0f);
	glUniformMatrix4fv(projection_matrix_uniform_, 1, GL_FALSE, &projection_matrix_[0][0]);
}

void renderer::compute_view_matrix_() {
	compute_projection_matrix_();
	
	glm::mat4 translation_matrix = glm::translate(position_);
	glm::mat4 rotation_matrix = glm::mat4_cast(orientation_);
	
	view_matrix_ = rotation_matrix * translation_matrix;
	
	glUniformMatrix4fv(view_matrix_uniform_, 1, GL_FALSE, &view_matrix_[0][0]);
}

void renderer::compute_motion_(float dtime) {	
	if(dtime > 0.01) dtime = 0.01;
			
	glm::vec3 target_velocity = glm::rotate(glm::inverse(orientation_), view_target_velocity_);
	
	glm::vec3 velocity_difference = target_velocity - velocity_;
	velocity_difference /= 20.0;
	
	velocity_ += velocity_difference;
	
	position_ += velocity_ * dtime;
}

void renderer::initialize_gl_() {
	shaders_->use();
	glEnable(GL_DEPTH_TEST);
	glClearColor(background_color_[0], background_color_[1], background_color_[2], 0.0);
}

void renderer::draw(float dtime) {	
	check_loader_();

	compute_motion_(dtime);
	compute_view_matrix_();
	update_loader_request_();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, renderer_point_buffer_);
	point::gl_setup_position_vertex_attribute(0);
	point::gl_setup_color_vertex_attribute(1);
	glDrawArrays(GL_POINTS, 0, renderer_point_buffer_size_);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	++fps_counter_;
}

void renderer::rotate_camera(float horizontalAngle, float verticalAngle, float rollAngle) {
	glm::vec3 angles(verticalAngle, horizontalAngle, rollAngle);
	glm::quat rot(angles);
	orientation_ = rot * orientation_;
}

void renderer::delete_loader_() {
	if(! loader_) return;
	
	loader_->stop();
	delete loader_;
	loader_ = nullptr;
	glBindBuffer(GL_ARRAY_BUFFER, loader_point_buffer_);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void renderer::apply_loader_configuration_() {
	if(! loader_) return;
	
	loader_->stop();
	if(! pause_loader_) loader_->start(loader_check_interval_, loader_check_condition_);
}

void renderer::update_loader_now() {
	if(loader_) loader_->update_now();
}

void renderer::switch_loader(loader* ld) {
	delete_loader_();
	loader_ = ld;
	apply_loader_configuration_();
}

void renderer::set_loader_configuration(bool paused, std::chrono::milliseconds interval, bool check) {
	pause_loader_ = paused;
	loader_check_interval_ = interval;
	loader_check_condition_ = check;
	apply_loader_configuration_();
}

void renderer::set_configuration(float fov, unsigned char bg_r, unsigned char bg_g, unsigned char bg_b) {
	fov_ = fov;
	compute_projection_matrix_();
	
	background_color_[0] = (float)bg_r / 255.0;
	background_color_[1] = (float)bg_g / 255.0;
	background_color_[2] = (float)bg_b / 255.0;
	glUniform3fv(fog_color_uniform_, 1, background_color_);
	glClearColor(background_color_[0], background_color_[1], background_color_[2], 0.0);
}

void renderer::set_point_capacity(std::size_t capacity) {
	point_buffer_capacity_ = capacity;
	stat_capacity_ = capacity;
	initialize_point_buffers_();
}


}

