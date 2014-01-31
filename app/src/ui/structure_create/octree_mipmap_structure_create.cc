#include "octree_mipmap_structure_create.h"
#include "../../structure/tree/octree/octree_mipmap_structure.h"
#include "../../util.h"

#include <stdexcept>

namespace dypc {

static const user_choices_t hdf_variants = {
	{ "striped", "Striped Point Sets" },
	{ "separate", "Separate Point Sets" }
};

loader* octree_mipmap_structure_create::create_memory_loader(model& mod) const {
	unsigned cap = capacity_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	auto mode = (mode_choice->GetSelection() == 0 ? random_downsampling_mode : uniform_downsampling_mode);
	
	return new octree_mipmap_structure_memory_loader(cap, factor, mode, mod);
}

user_choices_t octree_mipmap_structure_create::available_file_formats() {
	return {
		{"hdf", "HDF5"}
	};
}

void octree_mipmap_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned cap = capacity_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	auto mode = (mode_choice->GetSelection() == 0 ? random_downsampling_mode : uniform_downsampling_mode);

	octree_mipmap_structure s(cap, factor, mode, mod);
	if(format == "hdf") {
		auto variant = user_choice(hdf_variants, "HDF format variant");
		if(variant == "striped") octree_mipmap_structure_hdf_loader::write(filename, s);
		else if(variant == "separate") octree_mipmap_structure_hdf_separate_loader::write(filename, s);
	}
}

loader* octree_mipmap_structure_create::create_file_loader(const std::string& filename, const std::string& format) {
	if(format == "hdf") {
		auto variant = user_choice(hdf_variants, "HDF format variant");
		if(variant == "striped") return new octree_mipmap_structure_hdf_loader(filename);
		else if(variant == "separate") return new octree_mipmap_structure_hdf_separate_loader(filename);
	}
	return nullptr;
}

}
