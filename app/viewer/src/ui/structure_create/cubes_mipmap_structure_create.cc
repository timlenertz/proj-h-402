#include "cubes_mipmap_structure_create.h"
#include <stdexcept>


namespace dypc {

dypc_loader cubes_mipmap_structure_create::create_memory_loader(dypc_model mod) const {
	unsigned side = side_spin->GetValue();
	unsigned levels = levels_spin->GetValue();
	double mmfac; factor_text->GetLineText(0).ToDouble(&mmfac);
	dypc_downsampling_mode dmode = (mode_choice->GetSelection() == 0 ? dypc_random_downsampling_mode : dypc_uniform_downsampling_mode);

	return dypc_create_mipmap_cubes_structure_loader(mod, side, levels, mmfac, dmode);
}


void cubes_mipmap_structure_create::write_structure_file(dypc_model mod, const std::string& filename) const {
	unsigned side = side_spin->GetValue();
	unsigned levels = levels_spin->GetValue();
	double mmfac; factor_text->GetLineText(0).ToDouble(&mmfac);
	dypc_downsampling_mode dmode = (mode_choice->GetSelection() == 0 ? dypc_random_downsampling_mode : dypc_uniform_downsampling_mode);

	dypc_write_mipmap_cubes_structure_to_file(filename.c_str(), mod, side, levels, mmfac, dmode);
}


}
