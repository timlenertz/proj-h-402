#include "frustum.h"
#include "cuboid.h"

// see http://www.crownandcutlass.com/features/technicaldetails/frustum.html
//     http://voxelengine.googlecode.com/svn/branches/0_base/Frustum.cpp (wrong)

namespace dypc {

frustum::frustum(const glm::mat4& mvp) :
planes {
	plane(mvp[0][3] + mvp[0][2], mvp[1][3] + mvp[1][2], mvp[2][3] + mvp[2][2], mvp[3][3] + mvp[3][2]), // near_plane
	plane(mvp[0][3] - mvp[0][2], mvp[1][3] - mvp[1][2], mvp[2][3] - mvp[2][2], mvp[3][3] - mvp[3][2]), // far_plane
	plane(mvp[0][3] + mvp[0][0], mvp[1][3] + mvp[1][0], mvp[2][3] + mvp[2][0], mvp[3][3] + mvp[3][0]), // left_plane
	plane(mvp[0][3] - mvp[0][0], mvp[1][3] - mvp[1][0], mvp[2][3] - mvp[2][0], mvp[3][3] - mvp[3][0]), // right_plane
	plane(mvp[0][3] + mvp[0][1], mvp[1][3] + mvp[1][1], mvp[2][3] + mvp[2][1], mvp[3][3] + mvp[3][1]), // bottom_plane
	plane(mvp[0][3] - mvp[0][1], mvp[1][3] - mvp[1][1], mvp[2][3] - mvp[2][1], mvp[3][3] - mvp[3][1])  // top_plane
} { }


bool frustum::contains_point(const glm::vec3& pt) const {
	for(const plane& p : planes) if(p.distance(pt) < 0) return false;
	return true;
}

frustum::intersection_t frustum::contains_cuboid(const cuboid& cub) const {
	const auto& a = cub.origin;
	const auto& b = cub.extremity;
	std::size_t c, c2 = 0;
	for(const plane& p : planes) {
		c = 0;
		if(p.normal[0]*a[0] + p.normal[1]*a[1] + p.normal[2]*a[2] + p.d > 0) ++c;
		if(p.normal[0]*b[0] + p.normal[1]*a[1] + p.normal[2]*a[2] + p.d > 0) ++c;
		if(p.normal[0]*a[0] + p.normal[1]*b[1] + p.normal[2]*a[2] + p.d > 0) ++c;
		if(p.normal[0]*b[0] + p.normal[1]*b[1] + p.normal[2]*a[2] + p.d > 0) ++c;
		if(p.normal[0]*a[0] + p.normal[1]*a[1] + p.normal[2]*b[2] + p.d > 0) ++c;
		if(p.normal[0]*b[0] + p.normal[1]*a[1] + p.normal[2]*b[2] + p.d > 0) ++c;
		if(p.normal[0]*a[0] + p.normal[1]*b[1] + p.normal[2]*b[2] + p.d > 0) ++c;
		if(p.normal[0]*b[0] + p.normal[1]*b[1] + p.normal[2]*b[2] + p.d > 0) ++c;
		if(c == 0) return outside_frustum;
		if(c == 8) ++c2;
	}
	return (c2 == 6) ? inside_frustum : partially_inside_frustum;
}

}
