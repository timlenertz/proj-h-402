#include "cubes_mipmap_structure_panel.h"
#include "../structure/cubes_mipmap/cubes_mipmap_structure_loader.h"

namespace dypc {

void cubes_mipmap_structure_panel::update_() {
	if(! loader_) return;
	
	loader_->set_downsampling_start_distance( start_spin->GetValue() );
	loader_->set_downsampling_step_distance( step_spin->GetValue() );
	loader_->set_frustum_culling( frustum_check->GetValue() );
}

}
