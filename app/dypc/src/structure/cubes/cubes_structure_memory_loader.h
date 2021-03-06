#ifndef DYPC_CUBES_STRUCTURE_MEMORY_LOADER_H_
#define DYPC_CUBES_STRUCTURE_MEMORY_LOADER_H_

#include "cubes_structure_loader.h"
#include "cubes_structure.h"


namespace dypc {
	
class model;

class cubes_structure_memory_loader : public cubes_structure_loader {
private:
	cubes_structure structure_;

public:
	cubes_structure_memory_loader(float side, model& mod) : structure_(side, mod) { }
	
	std::string loader_name() const override { return "Cubes Structure Memory Loader"; }

protected:
	std::size_t compute_downsampled_points_(point_buffer_t points, std::size_t capacity, const loader::request_t&) override;
	
public:
	std::size_t memory_size() const override;
	std::size_t rom_size() const override;
	std::size_t number_of_points() const override;
};

}

#endif
