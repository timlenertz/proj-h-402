#ifndef DYPC_DEBUG_H_
#define DYPC_DEBUG_H_

#ifndef NDEBUG

#include <glm/glm.hpp>
#include "geometry/cuboid.h"

namespace dypc {

inline std::ostream& operator<<(std::ostream& out, glm::vec3 pt) {
	return (out << '(' << pt.x << "; " << pt.y << "; " << pt.z << ")");
}

inline std::ostream& operator<<(std::ostream& out, const cuboid& cub) {
	return (out << "cuboid[" << cub.origin << " --> " << cub.extremity << "]");
}

}

#endif

#endif
