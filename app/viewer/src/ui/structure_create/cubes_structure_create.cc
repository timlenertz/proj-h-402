#include "cubes_structure_create.h"
#include <stdexcept>
#include <iostream>

namespace dypc {

dypc_loader cubes_structure_create::create_memory_loader(dypc_model mod) const {
	unsigned side = side_spin->GetValue();

	return dypc_create_cubes_structure_loader(mod, side);
}


void cubes_structure_create::write_structure_file(dypc_model mod, const std::string& filename) const {
	unsigned side = side_spin->GetValue();
	dypc_write_cubes_structure_to_file(filename.c_str(), mod, side);
}


}
