#ifndef DYPC_FRUSTUM_H_
#define DYPC_FRUSTUM_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "plane.h"

namespace dypc {

class cuboid;

/**
 * Viewing frustum formed by clipped pyramid in three-dimensional space.
 * Defined by its 6 planes.
 */
class frustum {
public:
	/**
	 * Possible intersection types of an object with the frustum.
	 */
	enum intersection_t { outside_frustum = 0, inside_frustum, partially_inside_frustum };

	/**
	 * Identifiers for the 6 planes.
	 */
	enum {
		near_plane = 0,
		far_plane,
		left_plane,
		right_plane,
		bottom_plane,
		top_plane
	};
	
	/**
	 * The 6 planes that define the frustum.
	 */
	plane planes[6];
	
	/**
	 * Generate stub frustum.
	 */
	frustum() = default;
	
	/**
	 * Generate frustum from 4x4 projection matrix.
	 */
	explicit frustum(const glm::mat4& matrix);
	
	/**
	 * Check if point inside frustum.
	 */
	bool contains_point(const glm::vec3& pt) const;
	
	/**
	 * Check if cuboid inside, partially inside of outside frustum.
	 * Also works correctly when the frustum is completely inside the frustum.
	 * However, can return incorrect results in a small region behind the frustum due to omission of an additional test.
	 */
	intersection_t contains_cuboid(const cuboid&) const;
};

}

#endif
