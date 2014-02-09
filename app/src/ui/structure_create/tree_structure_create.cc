#include "tree_structure_create.h"
#include "../../structure/tree/tree_structure_loader.h"
#include <stdexcept>

namespace dypc {

std::size_t tree_structure_create::selected_levels_() const {
	switch(levels_choice->GetSelection()) {
		case 0: return 1;
		case 1: return 4;
		case 2: return 8;
		case 3: return 16;
	}
	throw std::runtime_error("Invalid levels choice selection");
}

loader* tree_structure_create::create_memory_loader(model& mod) const {
	unsigned leaf_cap = capacity_spin->GetValue();
	unsigned dmax = dmax_spin->GetValue();
	double mmfac; factor_text->GetLineText(0).ToDouble(&mmfac);
	auto dmode = (mode_choice->GetSelection() == 0 ? random_downsampling_mode : uniform_downsampling_mode);

	return create_tree_structure_memory_loader(structure_type_, selected_levels_(), leaf_cap, mmfac, dmode, dmax, mod);
}

void tree_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned leaf_cap = capacity_spin->GetValue();
	unsigned dmax = dmax_spin->GetValue();
	double mmfac; factor_text->GetLineText(0).ToDouble(&mmfac);
	auto dmode = (mode_choice->GetSelection() == 0 ? random_downsampling_mode : uniform_downsampling_mode);

	write_tree_structure_file(filename, structure_type_, selected_levels_(), leaf_cap, mmfac, dmode, dmax, mod);
}


}


