#include "cubes_structure_panel.h"
#include "../structure/cubes/cubes_structure_loader.h"

namespace dypc {

void cubes_structure_panel::update_() {
	if(! loader_) return;
	
	loader_->set_downsampling_minimal_distance( downsampling_spin->GetValue() );
	loader_->set_frustum_culling( frustum_check->GetValue() );
	loader_->set_downsampling_level( level_spin->GetValue() );
	loader_->set_secondary_pass_minimal_distance( secondary_spin->GetValue() );
}

}
