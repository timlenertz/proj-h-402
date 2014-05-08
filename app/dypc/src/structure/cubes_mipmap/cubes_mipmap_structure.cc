#include "cubes_mipmap_structure.h"
#include "../../model/model.h"
#include "../../progress.h"
#include "../../downsampling.h"

#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace dypc {

void cubes_mipmap_structure::add_point_(const point& pt) {
	auto idx = cube_index_for_coordinates(pt);
	auto it = cubes_.find(idx);
	if(it == cubes_.end()) {
		auto p = cubes_.emplace(std::piecewise_construct, std::forward_as_tuple(idx), std::forward_as_tuple(*this));
		if(p.second) it = p.first;
		else throw std::runtime_error("Point insertion failed");
	}
	it->second.add_point(pt);
}


cubes_mipmap_structure::cubes_mipmap_structure(float side, std::size_t dlevels, std::size_t dmin, float damount, downsampling_mode dmode, model& mod) :
mipmap_structure(dlevels, dmin, damount, dmode, false, mod), side_length_(side) {
	using namespace std::placeholders;
	
	progress_foreach(
		mod.begin(), mod.end(), mod.number_of_points(), "Creating Cubes Structure...",
		std::bind(&cubes_mipmap_structure::add_point_, this, _1)
	);
	
	progress_foreach(
		cubes_, "Downsampling...",
		[&](cubes_t::value_type& c) { c.second.generate_downsampling(c.first); }
	);
}


std::size_t cubes_mipmap_structure::size() const {
	std::size_t sz = 0;
	for(const auto& p : cubes_) sz += sizeof(cube_index_t) + p.second.size();
	return sz;
}


std::size_t cubes_mipmap_structure::total_number_of_points() const {
	std::size_t nb = 0;
	for(const auto& p : cubes_) nb += p.second.number_of_points();
	return nb;
}

cubes_mipmap_structure::cube::cube(const cubes_mipmap_structure& s) : structure_(s) {
	point_sets_ = new point_set_t [s.get_downsampling_levels()];
}


cubes_mipmap_structure::cube::~cube() {
	if(point_sets_) delete[] point_sets_;
}

std::size_t cubes_mipmap_structure::cube::extract_points_at_level(point_buffer_t output, std::size_t capacity, std::ptrdiff_t lvl) const {
	if(lvl < 0) lvl = 0;
	else if(lvl >= structure_.get_downsampling_levels()) lvl = structure_.get_downsampling_levels() - 1;
	const point_set_t& points = point_sets_[lvl]; 
	
	std::size_t count = (points.size() > capacity ? capacity : points.size());
	
	std::memcpy(
		(void*) output,
		(const void*) points.data(),
		count * sizeof(point)
	);
		
	return count;
}

void cubes_mipmap_structure::cube::generate_downsampling(cube_index_t idx) {
	cuboid cub = get_cuboid(idx);
	uniform_downsampling_previous_results_t previous_results;
	for(std::ptrdiff_t lvl = 1; lvl < structure_.get_downsampling_levels(); ++lvl)
		structure_.downsample_points_(point_sets_[0].begin(), point_sets_[0].end(), lvl, cub, point_sets_[lvl], previous_results);
}

std::size_t cubes_mipmap_structure::cube::size() const {
	std::size_t sz = sizeof(cube);
	for(std::ptrdiff_t lvl = 0; lvl < structure_.get_downsampling_levels(); ++lvl) {
		sz += point_sets_[lvl].size() * sizeof(point);
	}
	return sz;
}

}
