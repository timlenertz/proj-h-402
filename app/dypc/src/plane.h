#ifndef DYPC_PLANE_H_
#define DYPC_PLANE_H_

#include <glm/glm.hpp>

// based on http://voxelengine.googlecode.com/svn/branches/0_base/Plane.h (, .cpp)

namespace dypc {

class plane {	
public:
	glm::vec3 normal = glm::vec3(0, 0, 0); 
	float d = 0;
	
	plane() { }

	plane(float a, float b, float c, float D) :
		normal(a, b, c), d(D) { normalize(); }

	plane(const glm::vec3& pt, const glm::vec3& nrm) :
		normal(nrm), d(-glm::dot(normal, pt)) { normalize(); }
		
	plane(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3) : 
		normal(glm::cross(pt2 - pt1, pt3 - pt1)), d(-glm::dot(normal, pt1)) { normalize(); }
		
	void normalize() {
		float len = normal.length();
		normal /= len;
		d /= len;
	}
	
	float distance(const glm::vec3& pt) const {
		return glm::dot(normal, pt) + d;
	}
};

}

#endif
