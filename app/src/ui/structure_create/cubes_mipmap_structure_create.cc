#include "cubes_mipmap_structure_create.h"
#include "../../structure/cubes_mipmap/cubes_mipmap_structure.h"
#include "../../structure/cubes_mipmap/cubes_mipmap_structure_loader.h"
#include "../../structure/cubes_mipmap/cubes_mipmap_structure_memory_loader.h"
#include <stdexcept>

namespace dypc {

loader* cubes_mipmap_structure_create::create_memory_loader(model& mod) const {
	unsigned side = side_spin->GetValue();
	unsigned levels = levels_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	int mode = mode_choice->GetSelection();

	
	return new cubes_mipmap_structure_memory_loader(
		side,
		levels,
		factor,
		(mode == 0 ? random_downsampling_mode : uniform_downsampling_mode),
		mod
	);
}

structure_loader::file_formats_t cubes_mipmap_structure_create::available_file_formats() const {
	return cubes_mipmap_structure_loader::available_file_formats();
}

void cubes_mipmap_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned side = side_spin->GetValue();
	unsigned levels = levels_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	int mode = mode_choice->GetSelection();

	cubes_mipmap_structure s(
		side,
		levels,
		factor,
		(mode == 0 ? random_downsampling_mode : uniform_downsampling_mode),
		mod
	);
	cubes_mipmap_structure_loader::write_to_file(s, format, filename);
}

}
