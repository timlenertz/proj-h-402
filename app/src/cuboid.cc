#include "cuboid.h"
#include "frustum.h"

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

}
