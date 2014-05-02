#ifndef DYPC_CUBOID_H_
#define DYPC_CUBOID_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <array>
#include "../util.h"
#include "triangle.h"

namespace dypc {

class frustum;

/**
 * Axis-aligned cuboid in three-dimensional space.
 */
class cuboid {
public:
	glm::vec3 origin;
	glm::vec3 extremity;
	
	/**
	 * Generate stub cuboid.
	 */
	cuboid() : origin(0), extremity(0) { }
	
	/**
	 * Generate axis-aligned cuboid with given origin point and side lengths.
	 */
	cuboid(glm::vec3 o, glm::vec3 side_lengths) : origin(o), extremity(o + side_lengths) { }
	
	/**
	 * Generate axis-aligned cube with given origin point and side length.
	 */
	cuboid(glm::vec3 o, float side_length) : cuboid(o, glm::vec3(side_length, side_length, side_length)) { }

	/**
	 * Check whether point is inside the cuboid.
	 * Coordinates must be greater of equal to origin, and strictly lower than extremity.
	 * Bounds can be extended by small value epsilon to avoid missing points near border due to floating point imprecision.
	 * @param pt Point to check.
	 */
	bool in_range(glm::vec3 pt) const {
		return (pt[0] >= origin[0]) && (pt[0] < extremity[0])
		    && (pt[1] >= origin[1]) && (pt[1] < extremity[1])
		    && (pt[2] >= origin[2]) && (pt[2] < extremity[2]);
	}
	
	/**
	 * Check whether point is inside the cuboid.
	 * Coordinates must be greater of equal to origin, and strictly lower than extremity.
	 * Bounds can be extended by small value epsilon to avoid missing points near border due to floating point imprecision.
	 * @param pt Point to check.
	 * @param ep Epsilon value.
	 */
	bool in_range(glm::vec3 pt, float ep) const {
		return (pt[0] >= origin[0]-ep) && (pt[0] <= extremity[0]+ep)
		    && (pt[1] >= origin[1]-ep) && (pt[1] <= extremity[1]+ep)
		    && (pt[2] >= origin[2]-ep) && (pt[2] <= extremity[2]+ep);
	}
	
	/**
	 * Get 8 corner points of the cuboid.
	 */
	std::array<glm::vec3, 8> corners() const;
	
	/**
	 * Get center point of the cuboid.
	 */
	glm::vec3 center() const { return 0.5f*(origin + extremity); }
	
	/**
	 * Compute minimal distance from point to the cuboid.
	 * @param pt The point. May be inside or outside the cuboid.
	 * @return Minimal distance from pt to a point on the cuboid's sides.
	 */
	float minimal_distance(glm::vec3 pt) const;

	/**
	 * Compute maximal distance from point to the cuboid.
	 * @param pt The point. May be inside or outside the cuboid.
	 * @return Maximal distance from pt to a point on the cuboid's sides.
	 */
	float maximal_distance(glm::vec3 pt) const;
		
	/**
	 * Get side length in X direction.
	 */
	float side_length_x() const { return side_lengths()[0]; }
	
	/**
	 * Get side length in Y direction.
	 */
	float side_length_y() const { return side_lengths()[1]; }
	
	/**
	 * Get side length in Z direction.
	 */
	float side_length_z() const { return side_lengths()[2]; }
	
	/**
	 * Get 3 side lengths of cuboid.
	 */
	glm::vec3 side_lengths() const { return extremity - origin; }
	
	/**
	 * Check whether the cuboid is cubic.
	 * True if all side lengths are the same.
	 * @param ep Floating point comparation tolerance.
	 */
	bool is_cube(float ep = 0.001) const {
		return approximately_equal(side_length_x(), side_length_y()) && approximately_equal(side_length_y(), side_length_z());
	}
	
	/**
	 * Get the area of the cuboid.
	 */
	float area() const { return side_length_x() * side_length_y() * side_length_z(); }
	
	/**
	 * Get triangluation of cuboid surfaces.
	 * Can be used to render cuboid.
	 * @return 12 triangles that compose the cuboid.
	 */
	std::array<triangle, 12> hull_triangles() const;
};


inline cuboid make_cuboid(const glm::vec3 o, const glm::vec3 ex) {
	cuboid cub;
	cub.origin = o;
	cub.extremity = ex;
	return cub;
}

}

#endif
