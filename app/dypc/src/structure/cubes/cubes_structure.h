#ifndef DYPC_CUBES_STRUCTURE_H_
#define DYPC_CUBES_STRUCTURE_H_

#include <map>
#include <cmath>
#include <vector>
#include "../../point.h"
#include "../../weighted_point.h"
#include "../../geometry/cuboid.h"
#include "../../loader/loader.h"
#include "../structure.h"

namespace dypc {

class model;

class cubes_structure : public structure {	
public:
	using cube_index_t = std::tuple<std::ptrdiff_t, std::ptrdiff_t, std::ptrdiff_t>;
	class cube;
	
	cube_index_t cube_index_for_coordinates(glm::vec3 pt) const {
		return cube_index_t(
			std::floor(pt[0] / side_length_),
			std::floor(pt[1] / side_length_),
			std::floor(pt[2] / side_length_)
		);
	}
	
	using cubes_t = std::map<cube_index_t, cube>;
		
private:
	const float side_length_;
	cubes_t cubes_;

	void add_point_(const point& pt);

public:
	cubes_structure(float side, model& mod);
	
	float get_side_length() const { return side_length_; }
	
	std::size_t total_number_of_points() const;
	const cubes_t& cubes() const { return cubes_; }
	std::size_t size() const;	
};


class cubes_structure::cube {
private:		
	std::vector<weighted_point> points_;
	
public:
	const std::vector<weighted_point>& weighted_points() const { return points_; }

	std::size_t size() const { return sizeof(cube) + (points_.size() * sizeof(weighted_point)); }
	
	std::size_t number_of_points() const { return points_.size(); }
	void add_point(const point& pt);
	
	void assign_random_weights();
	
	std::size_t extract_points_with_minimal_weight(point_buffer_t points, std::size_t capacity, float min_weight) const;
};

}

#endif
