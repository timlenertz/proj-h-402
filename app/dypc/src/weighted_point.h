#ifndef DYPC_WEIGHTED_POINT_H_
#define DYPC_WEIGHTED_POINT_H_

#include "point.h"

namespace dypc {

/**
 * Point with added weight information.
 * Contains XYZRGB information like point. Is not a subclass so that it remains a POD type.
 * @see point
 */
class weighted_point {
public:
	float x, y, z; ///< X, Y, Z coordinates.
	std::uint8_t r, g, b; ///< R, G, B color.
	float weight; ///< Weight.
	
	weighted_point(const weighted_point&) = default;
		
	weighted_point(const point& p = point(), float w = 0) :
		x(p.x), y(p.y), z(p.z), r(p.r), g(p.g), b(p.b), weight(w) { }

	operator point () const { return point(x, y, z, r, g, b); } ///< Convert into point.
	operator glm::vec3 () const { return glm::vec3(x, y, z); } ///< Convert into position vector.
};

}

#endif
