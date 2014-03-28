#ifndef DYPC_CUBOID_H_
#define DYPC_CUBOID_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <array>
#include "triangle.h"

namespace dypc {

class frustum;

/**
 * Represents axis-aligned cuboid in three-dimensional space.
 */
class cuboid {
private:
	glm::vec3 origin_;
	glm::vec3 side_lengths_;
	
public:
	/**
	 * Default constructor; generates stub cuboid.
	 */
	cuboid() : origin_(0), side_lengths_(0) { }
	
	/**
	 * Generate axis-aligned cuboid with given origin point and side lengths.
	 */
	cuboid(glm::vec3 origin, glm::vec3 side_lengths) : origin_(origin), side_lengths_(side_lengths) { }
	
	/**
	 * Generate axis-aligned cube with given origin point and side length.
	 */
	cuboid(glm::vec3 origin, float side_length) : origin_(origin), side_lengths_(side_length, side_length, side_length) { }

	/**
	 * Check whether point is inside the cuboid.
	 * Coordinates must be greater of equal to origin, and strictly lower than extremity.
	 * Bounds can be extended by small value epsilon to avoid missing points near border due to floating point imprecision.
	 * @param pt Point to check.
	 * @param ep Epsilon value.
	 */
	bool in_range(glm::vec3 pt, float ep = 0.0) const {
		glm::vec3 extr = extremity();
		return (pt[0] >= origin_[0]-ep) && (pt[0] < extr[0]+ep)
		    && (pt[1] >= origin_[1]-ep) && (pt[1] < extr[1]+ep)
		    && (pt[2] >= origin_[2]-ep) && (pt[2] < extr[2]+ep);
	}
	
	/**
	 * Get 8 corner points of the cuboid.
	 */
	std::array<glm::vec3, 8> corners() const;
	
	/**
	 * Get center point of the cuboid.
	 */
	glm::vec3 center() const { return origin_ + 0.5f*side_lengths_; }
	
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
	 * Get origin point of cuboid.
	 */
	const glm::vec3& origin() const { return origin_; }
	
	/**
	 * Get extremity point, opposide of origin point.
	 */
	glm::vec3 extremity() const { return origin_ + side_lengths_; }
	
	/**
	 * Get side length in X direction.
	 */
	float side_length_x() const { return side_lengths_[0]; }
	
	/**
	 * Get side length in Y direction.
	 */
	float side_length_y() const { return side_lengths_[1]; }
	
	/**
	 * Get side length in Z direction.
	 */
	float side_length_z() const { return side_lengths_[2]; }
	
	/**
	 * Get 3 side lengths of cuboid.
	 */
	const glm::vec3& side_lengths() const { return side_lengths_; }
	
	/**
	 * Check whether the cuboid is cubic (i.e. all side lengths are the same).
	 */
	bool is_cube() const { return side_length_x() == side_length_y() && side_length_y() == side_length_z(); }
	
	/**
	 * Get the area of the cuboid.
	 */
	float area() const { return side_length_x() * side_length_y() * side_length_z(); }
	
	/**
	 * Get triangluation of cuboid.
	 * Can be used to render cuboid.
	 * @return 12 triangles that compose the cuboid.
	 */
	std::array<triangle, 12> hull_triangles() const;
};

}

#endif
