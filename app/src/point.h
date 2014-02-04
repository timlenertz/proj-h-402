#ifndef DYPC_POINT_H_
#define DYPC_POINT_H_

#include <glm/glm.hpp>
#include <cstdint>

#ifndef __gl_h_
typedef unsigned GLuint;
#endif

namespace dypc {

class point {
public:
	float x = 0.0, y = 0.0, z = 0.0;
	std::uint8_t r = 0, g = 0, b = 0;
	
	point() = default;
	point(const point&) = default;
	point(glm::vec3 pt) : x(pt[0]), y(pt[1]), z(pt[2]) { }
		
	point(float nx, float ny, float nz, std::uint8_t nr = 255, std::uint8_t ng = 255, std::uint8_t nb = 255) :
	x(nx), y(ny), z(nz), r(nr), g(ng), b(nb) {
		if(! *this) x = 0.00001; // Differenciate between null point and black point at center.
	}

	operator glm::vec3 () const { return glm::vec3(x, y, z); }

	explicit operator bool () const { return x != 0.0 || y != 0.0 || z != 0.0 || r != 0 || b != 0 || g != 0; }

	static void gl_setup_position_vertex_attribute(GLuint index);
	static void gl_setup_color_vertex_attribute(GLuint index);
};

using point_buffer_t = point*;

}

#endif
