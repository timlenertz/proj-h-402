#include "cubes_mipmap_structure_create.h"
#include "../../structure/structure_loader_interface.h"
#include "../../structure/cubes_mipmap/cubes_mipmap_structure.h"
#include "../../structure/cubes_mipmap/cubes_mipmap_structure_hdf_loader.h"
#include "../../structure/cubes_mipmap/cubes_mipmap_structure_memory_loader.h"
#include "../../util.h"
#include <stdexcept>


namespace dypc {

loader* cubes_mipmap_structure_create::create_memory_loader(model& mod) const {
	unsigned side = side_spin->GetValue();
	unsigned levels = levels_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	auto mode = (mode_choice->GetSelection() == 0 ? random_downsampling_mode : uniform_downsampling_mode);

	return new cubes_mipmap_structure_memory_loader(side, levels, factor, mode, mod);
}


void cubes_mipmap_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned side = side_spin->GetValue();
	unsigned levels = levels_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	auto mode = (mode_choice->GetSelection() == 0 ? random_downsampling_mode : uniform_downsampling_mode);

	cubes_mipmap_structure s(side, levels, factor, mode, mod);

	auto ext = file_path_extension(filename);
	if(ext == "hdf") {
		cubes_mipmap_structure_hdf_loader::write(filename, s);
		write_hdf_structure_file_type(filename, cubes_mipmap_structure_type);
	}
}


}
