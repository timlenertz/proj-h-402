#include "cubes_structure_create.h"
#include "../../structure/cubes/cubes_structure.h"
#include "../../structure/cubes/cubes_structure_loader.h"
#include "../../structure/cubes/cubes_structure_memory_loader.h"
#include "../../structure/cubes/cubes_structure_hdf_loader.h"
#include "../../structure/cubes/cubes_structure_sqlite_loader.h"
#include <stdexcept>

namespace dypc {

loader* cubes_structure_create::create_memory_loader(model& mod) const {
	unsigned side = side_spin->GetValue();
	return new cubes_structure_memory_loader(side, mod);
}

structure_loader::file_formats_t cubes_structure_create::available_file_formats() const {
	return cubes_structure_loader::available_file_formats();
}

void cubes_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned side = side_spin->GetValue();

	cubes_structure s(side, mod);
	cubes_structure_loader::write_to_file(s, format, filename);
}

}
