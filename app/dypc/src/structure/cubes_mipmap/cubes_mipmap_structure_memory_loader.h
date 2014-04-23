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
	cubes_mipmap_structure_memory_loader(float side, std::size_t dlevels, std::size_t dmin, float damount, downsampling_mode dmode, model& mod) :
	structure_(side, dlevels, dmin, damount, dmode, mod) { }
	
	std::string loader_name() const override { return "Cubes Mipmap Structure Memory Loader"; }

protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t&) override;

public:
	std::size_t memory_size() const override;
	std::size_t rom_size() const override;
	std::size_t number_of_points() const override;
};

}

#endif
