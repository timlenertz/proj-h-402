#ifndef DYPC_CUBOID_H_
#define DYPC_CUBOID_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace dypc {

class frustum;

class cuboid {
private:	
	float x_range_[2];
	float y_range_[2];
	float z_range_[2];
	glm::vec3 corners_[8];
	glm::vec3 center_;
	
public:
	cuboid();
	cuboid(glm::vec3 origin, glm::vec3 side_lengths);
	cuboid(glm::vec3 origin, float side_length) : cuboid(origin, glm::vec3(side_length, side_length, side_length)) {}

	bool in_range(glm::vec3 pt) const {
		return (pt[0] >= x_range_[0]) && (pt[0] <= x_range_[1])
		    && (pt[1] >= y_range_[0]) && (pt[1] <= y_range_[1])
		    && (pt[2] >= z_range_[0]) && (pt[2] <= z_range_[1]);
	}
	
	const glm::vec3* corners() const { return corners_; }
	const glm::vec3* corners_end() const { return corners_ + 8; }
	const glm::vec3& center() const { return center_; }
	
	float minimal_distance(glm::vec3 pt) const;
	float maximal_distance(glm::vec3 pt) const;
	
	const glm::vec3 origin() const { return glm::vec3(x_range_[0], y_range_[0], z_range_[0]); }
	const glm::vec3 extremity() const { return glm::vec3(x_range_[1], y_range_[1], z_range_[1]); }
	
	float side_length_x() const { return x_range_[1] - x_range_[0]; }
	float side_length_y() const { return y_range_[1] - y_range_[0]; }
	float side_length_z() const { return z_range_[1] - z_range_[0]; }
};

}

#endif
