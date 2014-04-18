#include "cuboid.h"
#include "frustum.h"
#include "../util.h"
#include <cmath>

namespace dypc {


std::array<glm::vec3, 8> cuboid::corners() const {
	float x_range[2] = { origin_[0], origin_[0] + side_lengths_[0] };
	float y_range[2] = { origin_[1], origin_[1] + side_lengths_[1] };
	float z_range[2] = { origin_[2], origin_[2] + side_lengths_[2] };
	
	return {
		glm::vec3(x_range[0], y_range[0], z_range[0]),
		glm::vec3(x_range[1], y_range[0], z_range[0]),
		glm::vec3(x_range[0], y_range[1], z_range[0]),
		glm::vec3(x_range[1], y_range[1], z_range[0]),
		glm::vec3(x_range[0], y_range[0], z_range[1]),
		glm::vec3(x_range[1], y_range[0], z_range[1]),
		glm::vec3(x_range[0], y_range[1], z_range[1]),
		glm::vec3(x_range[1], y_range[1], z_range[1])
	};
}



float cuboid::minimal_distance(glm::vec3 pt) const {
	float x_range[2] = { origin_[0], origin_[0] + side_lengths_[0] };
	float y_range[2] = { origin_[1], origin_[1] + side_lengths_[1] };
	float z_range[2] = { origin_[2], origin_[2] + side_lengths_[2] };

	float terms = 0;

	if(pt.x < x_range[0]) terms += sq(x_range[0] - pt.x);  
	else if(pt.x > x_range[1]) terms += sq(pt.x - x_range[1]);  

	if(pt.y < y_range[0]) terms += sq(y_range[0] - pt.y);  
	else if(pt.y > y_range[1]) terms += sq(pt.y - y_range[1]);  

	if(pt.z < z_range[0]) terms += sq(z_range[0] - pt.z);  
	else if(pt.z > z_range[1]) terms += sq(pt.z - z_range[1]);  

	return std::sqrt(terms);
}


float cuboid::maximal_distance(glm::vec3 pt) const {
	float max_d = 0.0;
	for(const glm::vec3& corner : corners()) {
		float d = std::abs(glm::distance(corner, pt));
		if(d > max_d) max_d = d;
	}
	return max_d;	
}


std::array<triangle, 12> cuboid::hull_triangles() const {
	const auto& o = origin_;
	glm::vec3 dx, dy, dz;
	dx.x = side_lengths_.x;
	dy.y = side_lengths_.y;
	dz.z = side_lengths_.z;
	
	triangle top[2] = {
		{ o + dx, o + dz, o },
		{ o + dx, o + dz, o + dx + dz }
	};
	triangle left[2] = {
		{ o + dy, o + dz, o },
		{ o + dy, o + dz, o + dy + dz }
	};
	triangle front[2] = {
		{ o + dy, o + dx, o },
		{ o + dy, o + dx, o + dy + dx }
	};
	
	return {
		top[0], top[1],
		left[0], left[1],
		front[0], front[1],
		top[0] + dy, top[1] + dy,
		left[0] + dx, left[1] + dx,
		front[0] + dz, front[1] + dz	
	};
}



}
