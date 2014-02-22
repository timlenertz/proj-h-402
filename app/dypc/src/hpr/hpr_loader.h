/*#ifndef DYPC_HPR_LOADER_H_
#define DYPC_HPR_LOADER_H_

#include "../loader/loader.h"
#include "../point.h"
#include <memory>
#include <atomic>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>

namespace dypc {

class hpr_loader : public loader {
private:
/*
	struct cgal_k : public CGAL::Exact_predicates_inexact_constructions_kernel {
		struct Point_3 : public CGAL::Exact_predicates_inexact_constructions_kernel::Point_3 {
			Point_3() = default;
			Point_3(double x, double y, double z, const point* p = nullptr) : 
				CGAL::Exact_predicates_inexact_constructions_kernel::Point_3(x, y, z), primal(p) { }
				
			const point* primal;
		};		
	};
*
	using cgal_k = CGAL::Exact_predicates_inexact_constructions_kernel;
		
	using cgal_point = cgal_k::Point_3;
	using cgal_polyhedron = CGAL::Polyhedron_3<cgal_k>;

	std::atomic_bool trigger_;
	const std::size_t buffer_capacity_;
	float sphere_radius_ = 5.0;
	
	std::unique_ptr<loader> underlying_loader_;
	std::unique_ptr<point[]> buffer_;
	std::unique_ptr<cgal_point[]> dual_buffer_;
	
public:
	hpr_loader(loader* ld, std::size_t cap);
	
	std::string loader_name() const override {
		return underlying_loader_->loader_name() + " (HPR)";
	}
	
	void trigger() { trigger_ = true; }
	void set_sphere_radius(float r) { sphere_radius_ = r; } 
	
	::wxWindow* create_panel(::wxWindow* parent) override;

protected:
	void compute_points(const request_t& request, point_buffer_t points, std::size_t& count, std::size_t capacity) override;
	bool should_compute_points(const request_t& request, const request_t& previous, std::chrono::milliseconds dtime) override;
};

}

#endif
*/
