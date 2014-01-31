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

user_choices_t cubes_structure_create::available_file_formats() {
	return {
		{ "db", "SQLite Database" },
		{ "hdf", "HDF5 File" }
	};
}

void cubes_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned side = side_spin->GetValue();

	cubes_structure s(side, mod);
	if(format == "hdf") cubes_structure_hdf_loader::write(filename, s);
	else if(format == "db") cubes_structure_sqlite_loader::write(filename, s);
}


loader* cubes_structure_create::create_file_loader(const std::string& filename, const std::string& format) {
	if(format == "hdf") return new cubes_structure_hdf_loader(filename);
	else if(format == "db") return new cubes_structure_sqlite_loader(filename);
	else return nullptr;
}

}
