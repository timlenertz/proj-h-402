#ifndef DYPC_CUBES_STRUCTURE_LOADER_H_
#define DYPC_CUBES_STRUCTURE_LOADER_H_

#include "cubes_structure.h"
#include "../structure_loader.h"
#include "../../geometry/cuboid.h"
#include "../../downsampling.h"


namespace dypc {

class cubes_structure_loader : public structure_loader {
protected:
	bool frustum_culling_ = true;
	std::size_t downsampling_levels_ = 16;
	float downsampling_amount_ = 2.0;
	float secondary_pass_distance_ = 100;
		
	static cuboid cube_from_index_(cubes_structure::cube_index_t idx, float side_length) {
		return cuboid(
			glm::vec3(
				std::get<0>(idx) * side_length,
				std::get<1>(idx) * side_length,
				std::get<2>(idx) * side_length
			),
			side_length
		);
	}
	
	float downsampling_ratio_(float distance, std::size_t capacity, std::size_t total_points) {
		return choose_downsampling_level_ratio_continuous(downsampling_levels_, distance, downsampling_setting_, total_points, capacity, downsampling_amount_);
	}
	
public:
	void set_frustum_culling(bool f) { frustum_culling_ = f; }
	void set_downsampling_levels(unsigned l) { downsampling_levels_ = l; }
	void set_downsampling_amount(float d) { downsampling_amount_ = d; }
	void set_secondary_pass_minimal_distance(float d) { secondary_pass_distance_ = d; }
	
	double get_setting(const std::string&) const override;
	void set_setting(const std::string&, double) override;
	
	loader_type get_loader_type() const override { return loader_type::cubes; }
};

}

#endif
