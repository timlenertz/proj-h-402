#ifndef DYPC_PLANE_H_
#define DYPC_PLANE_H_

#include <glm/glm.hpp>

// based on http://voxelengine.googlecode.com/svn/branches/0_base/Plane.h (, .cpp)

namespace dypc {

/**
 * Oriented two-dimensional plane in three-dimensional space.
 */
class plane {	
public:
	glm::vec3 normal = glm::vec3(0, 0, 0); ///< Normal vector of plane.
	float d = 0; ///< Distance from plane to origin, when plane is normalized.
	
	/**
	 * Generate stub plane.
	 */
	plane() { }

	/**
	 * Generate normalized plane using numbers a, b, c, d.
	 * normal = (a, b, c), d = D
	 */
	plane(float a, float b, float c, float D) :
		normal(a, b, c), d(D) { normalize(); }

	/**
	 * Generate normalized plane using normal vector and point on the plane.
	 * @param pt A point on the plane.
	 * @param nrm Normal vector of the plane.
	 */
	plane(const glm::vec3& pt, const glm::vec3& nrm) :
		normal(nrm), d(-glm::dot(normal, pt)) { normalize(); }
		
	/**
	 * Generate normalized plane using 3 points on the plane.
	 */
	plane(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3) : 
		normal(glm::cross(pt2 - pt1, pt3 - pt1)), d(-glm::dot(normal, pt1)) { normalize(); }
	
	/**
	 * Normalize plane
	 * Defines the same plane, but length of normal vector becomes 1, and d gets adjusted.
	 */
	void normalize() {
		float len = normal.length();
		normal /= len;
		d /= len;
	}
	
	/**
	 * Get oriented distance from plane to a point.
	 * @param pt Point in three-dimensional space.
	 */
	float distance(const glm::vec3& pt) const {
		return glm::dot(normal, pt) + d;
	}
};

}

#endif
