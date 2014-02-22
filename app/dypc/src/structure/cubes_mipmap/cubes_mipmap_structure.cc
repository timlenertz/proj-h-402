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


cubes_mipmap_structure::cubes_mipmap_structure(float side, std::size_t mmlvl, float mmfac, downsampling_mode_t dmode, model& mod) :
side_length_(side), mipmap_levels_(mmlvl), mipmap_factor_(mmfac), downsampling_mode_(dmode) {
	progress("Creating Cubes Structure...", mod.number_of_points(), 250000, [&]() {
		std::size_t points = 0;
		for(const auto& pt : mod) {
			add_point_(pt);
			set_progress(++points);
		}
	});

	progress("Downsampling...", cubes_.size(), 10, [&]() {
		for(auto& p : cubes_) {
			p.second.generate_downsampling();
			increment_progress();
		}
	});
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
	point_sets_ = new point_set_t [s.mipmap_levels_];
}


cubes_mipmap_structure::cube::~cube() {
	if(point_sets_) delete[] point_sets_;
}

std::size_t cubes_mipmap_structure::cube::extract_points_at_level(point_buffer_t output, std::size_t capacity, std::ptrdiff_t lvl) const {
	if(lvl < 0) lvl = 0;
	else if(lvl >= structure_.mipmap_levels_) lvl = structure_.mipmap_levels_ - 1;
	const point_set_t& points = point_sets_[lvl]; 
	
	std::size_t count = (points.size() > capacity ? capacity : points.size());
	
	std::memcpy(
		(void*) output,
		(const void*) points.data(),
		count * sizeof(point)
	);
		
	return count;
}

void cubes_mipmap_structure::cube::add_point(const point& pt) {
	point_sets_[0].emplace_back(pt);
}

void cubes_mipmap_structure::cube::generate_downsampling() {
	const point_set_t& all_points = point_sets_[0]; 
	float factor = structure_.mipmap_factor_;
	float ratio = 1.0 / factor;
	float area = std::pow(structure_.get_side_length(), 3.0);
	
	if(structure_.downsampling_mode_ == random_downsampling_mode) {
		for(std::ptrdiff_t lvl = 1; lvl < structure_.mipmap_levels_; ++lvl) {
			random_downsampling(all_points.data(), all_points.data() + all_points.size(), ratio, point_sets_[lvl]);
			ratio /= factor;
		}
		
	} else if(structure_.downsampling_mode_ == uniform_downsampling_mode) {
		for(std::ptrdiff_t lvl = 1; lvl < structure_.mipmap_levels_; ++lvl) {
			uniform_downsampling(all_points.data(), all_points.data() + all_points.size(), ratio, area, point_sets_[lvl]);
			ratio /= factor;
		}
		
	}
}


std::size_t cubes_mipmap_structure::cube::size() const {
	std::size_t sz = sizeof(cube);
	for(std::ptrdiff_t lvl = 0; lvl < structure_.mipmap_levels_; ++lvl) {
		sz += point_sets_[lvl].size() * sizeof(point);
	}
	return sz;
}

}
