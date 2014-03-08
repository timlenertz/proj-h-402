#ifndef DYPC_CUBES_MIPMAP_STRUCTURE_H_
#define DYPC_CUBES_MIPMAP_STRUCTURE_H_

#include <map>
#include <cmath>
#include <vector>
#include <cassert>
#include "../../enums.h"
#include "../../point.h"
#include "../../cuboid.h"
#include "../../loader/loader.h"
#include "../structure.h"
#include "../../downsampling.h"


namespace dypc {

class model;

class cubes_mipmap_structure : public structure {	
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
		
private:
	const float side_length_;
	const std::size_t mipmap_levels_;
	const float mipmap_factor_;
	const downsampling_mode downsampling_mode_;
	
	std::map<cube_index_t, cube> cubes_;

	void add_point_(const point& pt);

public:
	cubes_mipmap_structure(float side, std::size_t mmlvl, float mmfac, downsampling_mode mod, model&);	
	float get_side_length() const { return side_length_; }
	std::size_t get_mipmap_levels() const { return mipmap_levels_; }
	float get_mipmap_factor() const { return mipmap_factor_; }
	downsampling_mode get_downsampling_mode() const { return downsampling_mode_; }
	
	std::size_t total_number_of_points() const;
	const std::map<cube_index_t, cube>& cubes() const { return cubes_; }
	std::size_t size() const;
};


class cubes_mipmap_structure::cube {
private:
	using point_set_t = std::vector<point>;

	const cubes_mipmap_structure& structure_;
	point_set_t* point_sets_ = nullptr;

public:
	explicit cube(const cubes_mipmap_structure&);
	~cube();

	const point_set_t& points_at_level(std::ptrdiff_t lvl) const
		{ assert(lvl >= 0 && lvl < structure_.mipmap_levels_); return point_sets_[lvl]; }

	std::size_t size() const;
	std::size_t number_of_points() const { return point_sets_[0].size(); }
	
	void add_point(const point& pt);
	void generate_downsampling();
		
	std::size_t extract_points_at_level(point_buffer_t points, std::size_t capacity, std::ptrdiff_t lvl) const;
};

}

#endif
