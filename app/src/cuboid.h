#ifndef DYPC_CUBOID_H_
#define DYPC_CUBOID_H_

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <array>

namespace dypc {

class frustum;

class cuboid {
private:
	glm::vec3 origin_;
	glm::vec3 side_lengths_;
	
public:
	cuboid() : origin_(0), side_lengths_(0) { }
	cuboid(glm::vec3 origin, glm::vec3 side_lengths) : origin_(origin), side_lengths_(side_lengths) { }
	cuboid(glm::vec3 origin, float side_length) : origin_(origin), side_lengths_(side_length, side_length, side_length) { }

	bool in_range(glm::vec3 pt) const {
		const float ep = 0.01;
		glm::vec3 extr = extremity();
		return (pt[0] >= origin_[0]-ep) && (pt[0] <= extr[0]+ep)
		    && (pt[1] >= origin_[1]-ep) && (pt[1] <= extr[1]+ep)
		    && (pt[2] >= origin_[2]-ep) && (pt[2] <= extr[2]+ep);
	}
	
	std::array<glm::vec3, 8> corners() const;
	glm::vec3 center() const { return origin_ + 0.5f*side_lengths_; }
	
	float minimal_distance(glm::vec3 pt) const;
	float maximal_distance(glm::vec3 pt) const;
	
	const glm::vec3& origin() const { return origin_; }
	glm::vec3 extremity() const { return origin_ + side_lengths_; }
	
	float side_length_x() const { return side_lengths_[0]; }
	float side_length_y() const { return side_lengths_[1]; }
	float side_length_z() const { return side_lengths_[2]; }
	const glm::vec3& side_lengths() const { return side_lengths_; }
	
	bool is_cube() const { return side_length_x() == side_length_y() && side_length_y() == side_length_z(); }
	float area() const { return side_length_x() * side_length_y() * side_length_z(); }
};

}

#endif
