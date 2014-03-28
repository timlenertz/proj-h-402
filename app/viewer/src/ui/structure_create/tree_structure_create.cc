#include "tree_structure_create.h"
#include "../../util.h"
#include <stdexcept>
#include <vector>
#include <string>

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

dypc_loader tree_structure_create::create_memory_loader(dypc_model mod) const {
	unsigned leaf_cap = capacity_spin->GetValue();
	std::size_t dmin = dmin_spin->GetValue();
	double damount; amount_text->GetLineText(0).ToDouble(&damount);
	dypc_downsampling_mode dmode = (mode_choice->GetSelection() == 0 ? dypc_random_downsampling_mode : dypc_uniform_downsampling_mode);

	dypc_tree_structure_loader_type ltypes[] = {
		dypc_simple_tree_structure_loader_type,
		dypc_ordered_tree_structure_loader_type,
		dypc_occluding_tree_structure_loader_type
	};
	std::vector<std::string> choices = { "Simple", "Ordered", "Occluding" };
	std::ptrdiff_t i = user_choice(choices, "Type of tree loader");
	if(i >= 0 && i < 3) {
		return dypc_create_tree_structure_loader(mod, structure_type_, selected_levels_(), leaf_cap, dmin, damount, dmode, ltypes[i]);
	} else {
		return nullptr;
	}
}

void tree_structure_create::write_structure_file(dypc_model mod, const std::string& filename) const {
	unsigned leaf_cap = capacity_spin->GetValue();
	std::size_t dmin = dmin_spin->GetValue();
	double damount; amount_text->GetLineText(0).ToDouble(&damount);
	dypc_downsampling_mode dmode = (mode_choice->GetSelection() == 0 ? dypc_random_downsampling_mode : dypc_uniform_downsampling_mode);

	dypc_write_tree_structure_to_file(filename.c_str(), mod, structure_type_, selected_levels_(), leaf_cap, dmin, damount, dmode);
}


}
