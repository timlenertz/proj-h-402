#include "gl.h"
#include <dypc/dypc.h>

namespace dypc {

void initialize_glew() {
	static bool done = false;
	if(done) return;
	glewExperimental = GL_TRUE;
	auto err = glewInit();
	if(err != GLEW_OK) throw gl_error(std::string("Failed to initialize GLEW: ") + (const char*)glewGetErrorString(err));
	done = true;
}

void gl_setup_position_vertex_attribute(GLuint index) {
	constexpr GLsizei stride = sizeof(dypc_point);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride, 0);
}

void gl_setup_color_vertex_attribute(GLuint index) {
	constexpr GLsizei stride = sizeof(dypc_point);
	constexpr GLsizei offset = 3 * sizeof(float);
	glVertexAttribPointer(index, 3, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const GLvoid*>(offset));
}

}
