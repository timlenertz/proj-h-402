#ifndef DYPC_DEBUG_H_
#define DYPC_DEBUG_H_

#ifndef NDEBUG

#include <glm/glm.hpp>

namespace dypc {

inline std::ostream& operator<<(std::ostream& out, glm::vec3 pt) {
	out << '(' << pt.x << "; " << pt.y << "; " << pt.z << ")";
	return out;
}

}

#endif

#endif
