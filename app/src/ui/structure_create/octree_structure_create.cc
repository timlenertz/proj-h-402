#include "octree_structure_create.h"
#include "../../structure/tree/octree/octree_structure.h"

#include <stdexcept>

namespace dypc {


static const user_choices_t hdf_variants = {
	{ "striped", "Striped Point Sets" },
	{ "separate", "Separate Point Sets" }
};

loader* octree_structure_create::create_memory_loader(model& mod) const {
	unsigned cap = capacity_spin->GetValue();
	
	return new octree_structure_memory_loader(cap, mod);
}

user_choices_t octree_structure_create::available_file_formats() {
	return {
		{"hdf", "HDF5"}
	};
}

void octree_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned cap = capacity_spin->GetValue();

	octree_structure s(cap, mod);
	if(format == "hdf") {
		auto variant = user_choice(hdf_variants, "HDF format variant");
		if(variant == "striped") octree_structure_hdf_loader::write(filename, s);
		else if(variant == "separate") octree_structure_hdf_separate_loader::write(filename, s);
	}
}

loader* octree_structure_create::create_file_loader(const std::string& filename, const std::string& format) {
	if(format == "hdf") {
		auto variant = user_choice(hdf_variants, "HDF format variant");
		if(variant == "striped") return new octree_structure_hdf_loader(filename);
		else if(variant == "separate") return new octree_structure_hdf_separate_loader(filename);
	}
	return nullptr;
}

}
