#include "cuboid.h"
#include "frustum.h"
#include "util.h"
#include <cmath>

namespace dypc {

cuboid::cuboid() :
	x_range_ { 0, 0 },
	y_range_ { 0, 0 },
	z_range_ { 0, 0 },
	corners_ {
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 0)
	},
	center_(0, 0, 0) { }


cuboid::cuboid(glm::vec3 origin, glm::vec3 side_lengths) : 
	x_range_ { origin[0], origin[0] + side_lengths[0] },
	y_range_ { origin[1], origin[1] + side_lengths[1] },
	z_range_ { origin[2], origin[2] + side_lengths[2] },
	corners_ {
		glm::vec3(x_range_[0], y_range_[0], z_range_[0]),
		glm::vec3(x_range_[1], y_range_[0], z_range_[0]),
		glm::vec3(x_range_[0], y_range_[1], z_range_[0]),
		glm::vec3(x_range_[1], y_range_[1], z_range_[0]),
		glm::vec3(x_range_[0], y_range_[0], z_range_[1]),
		glm::vec3(x_range_[1], y_range_[0], z_range_[1]),
		glm::vec3(x_range_[0], y_range_[1], z_range_[1]),
		glm::vec3(x_range_[1], y_range_[1], z_range_[1])
	},
	center_ {
		origin[0] + side_lengths[0]/2,
		origin[1] + side_lengths[1]/2,
		origin[2] + side_lengths[2]/2
	} { }



float cuboid::minimal_distance(glm::vec3 pt) const {
	float terms = 0;

	if(pt.x < x_range_[0]) terms += sq(x_range_[0] - pt.x);  
	else if(pt.x > x_range_[1]) terms += sq(pt.x - x_range_[1]);  

	if(pt.y < y_range_[0]) terms += sq(y_range_[0] - pt.y);  
	else if(pt.y > y_range_[1]) terms += sq(pt.y - y_range_[1]);  

	if(pt.z < z_range_[0]) terms += sq(z_range_[0] - pt.z);  
	else if(pt.z > z_range_[1]) terms += sq(pt.z - z_range_[1]);  

	return std::sqrt(terms);
}


float cuboid::maximal_distance(glm::vec3 pt) const {
	float max_d = 0.0;
	for(const glm::vec3& corner : corners_) {
		float d = std::abs(glm::distance(corner, pt));
		if(d > max_d) max_d = d;
	}
	return max_d;	
}



}
