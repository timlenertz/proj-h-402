#ifndef DYPC_FRUSTUM_H_
#define DYPC_FRUSTUM_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "plane.h"

namespace dypc {

class cuboid;

class frustum {
public:
	enum intersection_t { outside_frustum = 0, inside_frustum, partially_inside_frustum };

	enum {
		near_plane = 0,
		far_plane,
		left_plane,
		right_plane,
		bottom_plane,
		top_plane
	};
	
	plane planes[6];
	
	frustum() { }
	explicit frustum(const glm::mat4& mvp_matrix);
	
	bool contains_point(const glm::vec3& pt) const;
	intersection_t contains_cuboid(const cuboid&) const;
};

}

#endif
