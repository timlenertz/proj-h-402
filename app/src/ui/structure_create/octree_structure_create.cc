#include "octree_structure_create.h"
#include "../../structure/tree/octree/octree_structure.h"

#include <stdexcept>

namespace dypc {

loader* octree_structure_create::create_memory_loader(model& mod) const {
	unsigned cap = capacity_spin->GetValue();
	
	return new octree_structure_memory_loader(
		cap,
		mod
	);
}

structure_loader::file_formats_t octree_structure_create::available_file_formats() const {
	return {
		{"hdf", "HDF5"}
	};
}

void octree_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned cap = capacity_spin->GetValue();

	octree_structure s(
		cap,
		mod
	);
	if(format == "hdf") octree_structure_hdf_loader::write(filename, s);
}

}
