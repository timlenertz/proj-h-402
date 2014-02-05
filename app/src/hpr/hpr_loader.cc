#include "hpr_loader.h"
#include "../ui/hpr_panel.h"

#include <CGAL/convex_hull_3.h>

namespace dypc {

hpr_loader::hpr_loader(loader* ld, std::size_t cap) : 
	buffer_capacity_(cap),
	underlying_loader_(ld),
	buffer_(new point[cap]),
	dual_buffer_(new cgal_point[cap + 1])
{
	trigger_ = false;
}


void hpr_loader::compute_points(const request_t& req, point_buffer_t points, std::size_t& count, std::size_t capacity) {
	if(! trigger_) {
		underlying_loader_->compute_points(req, points, count, capacity);
		return;
	}
	
	trigger_ = false;
	
	std::size_t loader_count;
	underlying_loader_->compute_points(req, buffer_.get(), loader_count, capacity);
	
	if(loader_count <= 3) { count = 0; return; }

	cgal_point* dual_pt = dual_buffer_.get();
	*(dual_pt++) = cgal_k::Point_3(0.0, 0.0, 0.0);
	
	const point* pt_end = buffer_.get() + loader_count;
	for(const point* orig_pt = buffer_.get(); orig_pt != pt_end; ++orig_pt) {
		glm::vec3 pt = *orig_pt;
		pt -= req.position;
		float d = glm::length(pt);
		pt += (sphere_radius_ - d) * 2.0f * glm::normalize(pt);
		
		*(dual_pt++) = cgal_point(pt.x, pt.y, pt.z);
	}
	
	cgal_polyhedron hull;
	CGAL::convex_hull_3(dual_buffer_.get(), dual_buffer_.get() + loader_count + 1, hull);
	
	std::size_t c = 0;
	for(auto it = hull.points_begin(); it != hull.points_end(); it++) {
		for(std::ptrdiff_t i = 1; i <= buffer_capacity_; ++i) {
			if(dual_buffer_[i] == *it) {
				*(points++) = buffer_[i - 1];
				++c;
				break;
			}
		}
	}
	
	count = c;
}


bool hpr_loader::should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) {
	return trigger_ || underlying_loader_->should_compute_points(request, previous, dtime);
}


::wxWindow* hpr_loader::create_panel(::wxWindow* parent) {
	hpr_panel* panel = new hpr_panel(parent);
	panel->set_loader(*this, nullptr);
	return panel;
}



}
