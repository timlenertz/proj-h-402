#ifndef DYPC_TRIANGLE_H_
#define DYPC_TRIANGLE_H_

#include <glm/glm.hpp>

namespace dypc {

/**
 * Triangle in three-dimensional space.
 * Implements operators for translations.
 */
class triangle {
public:
	glm::vec3 a, b, c; ///< The points a, b, c that define the triangle.
	
	triangle() = default;
	triangle(const triangle&) = default;
	
	/**
	 * Generate triangle from 3 points.
	 */
	triangle(const glm::vec3& na, const glm::vec3& nb, const glm::vec3& nc) : a(na), b(nb), c(nc) { }
	
	triangle& operator=(const triangle&) = default;
	
	triangle& operator+=(glm::vec3 v) { a += v; b += v; c += v; return *this; }
	triangle& operator-=(glm::vec3 v) { return operator+=(-v); }
	triangle operator+(glm::vec3 v) { triangle copy(*this); copy += v; return copy; }
	triangle operator-(glm::vec3 v) { triangle copy(*this); copy -= v; return copy; }
};

}

#endif
