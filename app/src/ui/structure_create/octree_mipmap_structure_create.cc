#include "octree_mipmap_structure_create.h"
#include "../../structure/tree/octree/octree_mipmap_structure.h"

#include <stdexcept>

namespace dypc {

loader* octree_mipmap_structure_create::create_memory_loader(model& mod) const {
	unsigned cap = capacity_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	int mode = mode_choice->GetSelection();
	
	return new octree_mipmap_structure_memory_loader(
		cap,
		factor,
		(mode == 0 ? random_downsampling_mode : uniform_downsampling_mode),
		mod
	);
}

structure_loader::file_formats_t octree_mipmap_structure_create::available_file_formats() const {
	return {
		{"hdf", "HDF5"}
	};
}

void octree_mipmap_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned cap = capacity_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	int mode = mode_choice->GetSelection();

	octree_mipmap_structure s(
		cap,
		factor,
		(mode == 0 ? random_downsampling_mode : uniform_downsampling_mode),
		mod
	);
	if(format == "hdf") octree_mipmap_structure_hdf_loader::write(filename, s);
}

}
