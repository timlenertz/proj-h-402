#include "point.h"

#include "gl.h"

namespace dypc {

void point::gl_setup_position_vertex_attribute(GLuint index) {
	constexpr GLsizei stride = sizeof(point);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride, 0);
}

void point::gl_setup_color_vertex_attribute(GLuint index) {
	constexpr GLsizei stride = sizeof(point);
	constexpr GLsizei offset = 3 * sizeof(float);
	glVertexAttribPointer(index, 3, GL_UNSIGNED_BYTE, GL_TRUE, stride, reinterpret_cast<const GLvoid*>(offset));
}


}
