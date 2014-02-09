#ifndef DYPC_CUBES_STRUCTURE_LOADER_H_
#define DYPC_CUBES_STRUCTURE_LOADER_H_

#include "cubes_structure.h"
#include "../structure_loader.h"
#include "../../statistics.h"
#include "../../cuboid.h"


namespace dypc {

class cubes_structure_loader : public structure_loader {
protected:
	float downsampling_distance_ = 20;
	bool frustum_culling_ = true;
	unsigned downsampling_level_ = 3;
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

	statistics::item stat_total_cubes_;
	statistics::item stat_frustum_cubes_;
	statistics::item stat_downsampled_cubes_;
	statistics::item stat_secondary_cubes_;
	
	adapt_result_t adapt_settings_(std::size_t last_extracted, std::size_t capacity) override;

public:
	cubes_structure_loader() :
		stat_total_cubes_(statistics::add("Total Cubes", 0)),
		stat_frustum_cubes_(statistics::add("Cubes in Frustum", 0)),
		stat_downsampled_cubes_(statistics::add("Downsampled Cubes", 0)),
		stat_secondary_cubes_(statistics::add("2nd Pass Cubes", 0)) { }

	void set_downsampling_minimal_distance(float d) { downsampling_distance_ = d; }
	void set_frustum_culling(bool f) { frustum_culling_ = f; }
	void set_downsampling_level(unsigned l) { downsampling_level_ = l; }
	void set_secondary_pass_minimal_distance(float d) { secondary_pass_distance_ = d; }
	
	::wxWindow* create_panel(::wxWindow* parent) override;
};

}

#endif
