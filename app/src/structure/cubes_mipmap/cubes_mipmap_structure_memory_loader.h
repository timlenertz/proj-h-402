#ifndef DYPC_CUBES_MIPMAP_STRUCTURE_MEMORY_LOADER_H_
#define DYPC_CUBES_MIPMAP_STRUCTURE_MEMORY_LOADER_H_

#include "cubes_mipmap_structure_loader.h"
#include "cubes_mipmap_structure.h"

namespace dypc {
	
class model;

class cubes_mipmap_structure_memory_loader : public cubes_mipmap_structure_loader {
private:
	cubes_mipmap_structure structure_;

public:
	cubes_mipmap_structure_memory_loader(float side, std::size_t mmlvl, float mmfac, downsampling_mode_t dmode, model& mod) :
	structure_(side, mmlvl, mmfac, dmode, mod) {
		stat_total_cubes_ = structure_.cubes().size();
	}
	
	std::string loader_name() const override { return "Cubes Mipmap Structure Memory Loader"; }

protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t&) override;
	
	std::size_t memory_size_() const override;
	std::size_t file_size_() const override;
	std::size_t total_points_() const override;
};

}

#endif
