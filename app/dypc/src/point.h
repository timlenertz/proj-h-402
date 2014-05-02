#ifndef DYPC_POINT_H_
#define DYPC_POINT_H_

#include <glm/glm.hpp>
#include <cstdint>
#include "util.h"

namespace dypc {

/**
 * Point in XYZRGB point cloud.
 * Is POD type, and meant to be copied directly into GPU buffers and/or into files. Distinguishes \e null point.
 */
class point {
private:
	static constexpr std::uint8_t default_color[] = { 255, 255, 255 };

public:
	float x = 0.0, y = 0.0, z = 0.0; ///< X, Y, Z coordinates.
	std::uint8_t r = 0, g = 0, b = 0; ///< R, G, B color.
	
	point() = default; ///< Create null point.
	point(const point&) = default; ///< Copy point.
	point(glm::vec3 pt) : x(pt[0]), y(pt[1]), z(pt[2]), r(default_color[0]), g(default_color[1]), b(default_color[2]) { } ///< Generate non colored point from position vector.
		
	/**
	 * Create non-null point from position, and color information.
	 */
	point(float nx, float ny, float nz, std::uint8_t nr = default_color[0], std::uint8_t ng = default_color[1], std::uint8_t nb = default_color[2]) :
	x(nx), y(ny), z(nz), r(nr), g(ng), b(nb) {
		if(! *this) x = float_comparison_epsilon; // Differenciate between null point and black point at center.
	}

	operator glm::vec3 () const { return glm::vec3(x, y, z); } ///< Convert info position vector.

	explicit operator bool () const { return x != 0.0 || y != 0.0 || z != 0.0 || r != 0 || b != 0 || g != 0; } ///< Check if point is \e null.
};

/**
 * Buffer of points.
 * I.e. pointer to at least one point
 */
using point_buffer_t = point*;

}

#endif
