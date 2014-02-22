#include "cubes_structure.h"
#include "../../model/model.h"
#include "../../progress.h"
#include "cubes_structure_memory_loader.h"
#include "cubes_structure_sqlite_loader.h"
#include "cubes_structure_hdf_loader.h"

#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <cmath>

namespace dypc {

void cubes_structure::add_point_(const point& pt) {
	auto idx = cube_index_for_coordinates(pt);
	cubes_[idx].add_point(pt);
}


cubes_structure::cubes_structure(float side, model& mod) : side_length_(side) {
	progress("Creating Cubes Structure...", mod.number_of_points(), 250000, [&]() {
		std::size_t points = 0;
		for(const auto& pt : mod) {
			add_point_(pt);
			set_progress(++points);
		}
	});

	progress("Finalizing Cubes Structure...", cubes_.size(), 10, [&]() {
		for(auto& p : cubes_) {
			p.second.assign_random_weights();
			increment_progress();
		}
	});
}

std::size_t cubes_structure::size() const {
	std::size_t sz = 0;
	for(const auto& p : cubes_) sz += sizeof(cube_index_t) + p.second.size();
	return sz;
}


std::size_t cubes_structure::total_number_of_points() const {
	std::size_t nb = 0;
	for(const auto& p : cubes_) nb += p.second.number_of_points();
	return nb;
}


std::size_t cubes_structure::cube::extract_points_with_minimal_weight(point_buffer_t points, std::size_t capacity, float min_weight) const {
	std::size_t count = 0;
	point_buffer_t buf = points;
	
	for(const auto& pt : points_) {
		if(count == capacity) break;

		if(pt.weight < min_weight) break;
		*(buf++) = pt;
		++count;
	}
		
	return count;
}

void cubes_structure::cube::add_point(const point& pt) {
	points_.emplace_back(pt);
}

void cubes_structure::cube::assign_random_weights() {
	for(auto& p : points_) p.weight = (float)std::rand() / RAND_MAX;

	std::sort(points_.begin(), points_.end(), [](const weighted_point& a, const weighted_point& b) -> bool {
		return a.weight > b.weight;
	});
}


}
