#include "cubes_structure_create.h"
#include "../../structure/structure_loader_interface.h"
#include "../../structure/cubes/cubes_structure.h"
#include "../../structure/cubes/cubes_structure_hdf_loader.h"
#include "../../structure/cubes/cubes_structure_memory_loader.h"
#include "../../structure/cubes/cubes_structure_sqlite_loader.h"
#include "../../util.h"
#include <stdexcept>


namespace dypc {

loader* cubes_structure_create::create_memory_loader(model& mod) const {
	unsigned side = side_spin->GetValue();

	return new cubes_structure_memory_loader(side, mod);
}


void cubes_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned side = side_spin->GetValue();

	cubes_structure s(side, mod);

	auto ext = file_path_extension(filename);
	if(ext == "hdf") {
		cubes_structure_hdf_loader::write(filename, s);
		write_hdf_structure_file_type(filename, cubes_structure_type);
	} else if(ext == "db") {
		cubes_structure_sqlite_loader::write(filename, s);
		write_sqlite_structure_file_type(filename, cubes_structure_type);
	}
}


}
