#include "octree_structure_create.h"
#include "../../structure/tree/octree/octree_structure.h"

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
	return {};
}

void octree_structure_create::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	/*unsigned cap = capacity_spin->GetValue();
	unsigned levels = levels_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	int mode = mode_choice->GetSelection();

	octree_structure s(
		cap,
		levels,
		factor,
		(mode == 0 ? random_downsampling_mode : uniform_downsampling_mode),
		mod
	);
	octree_structure_loader::write_to_file(s, format, filename);*/
}

}
