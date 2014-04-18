#ifndef DYPC_CUBES_MIPMAP_STRUCTURE_LOADER_H_
#define DYPC_CUBES_MIPMAP_STRUCTURE_LOADER_H_

#include "cubes_mipmap_structure.h"
#include "../structure_loader.h"
#include "../../geometry/cuboid.h"


namespace dypc {

class cubes_mipmap_structure_loader : public structure_loader {
protected:
	float downsampling_start_distance_ = 70;
	float downsampling_step_distance_ = 30;
	bool frustum_culling_ = true;
		
	static cuboid cube_from_index_(cubes_mipmap_structure::cube_index_t idx, float side_length) {
		return cuboid(
			glm::vec3(
				std::get<0>(idx) * side_length,
				std::get<1>(idx) * side_length,
				std::get<2>(idx) * side_length
			),
			side_length
		);
	}

public:
	void set_downsampling_start_distance(float d) { downsampling_start_distance_ = d; }
	void set_downsampling_step_distance(float d) { downsampling_step_distance_ = d; }
	void set_frustum_culling(bool f) { frustum_culling_ = f; }
	
	double get_setting(const std::string&) const override;
	void set_setting(const std::string&, double) override;
	
	loader_type get_loader_type() const override { return loader_type::cubes_mipmap; }
};

}

#endif
