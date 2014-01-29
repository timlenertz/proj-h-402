#include "tree_structure_panel.h"
#include "../structure/tree/tree_structure_loader.h"

namespace dypc {

void tree_structure_panel::update_() {
	if(! loader_) return;
	
	loader_->set_downsampling_start_distance( start_spin->GetValue() );
	loader_->set_downsampling_step_distance( step_spin->GetValue() );
	loader_->set_minimal_number_of_points_for_split( min_split_spin->GetValue() );
}

}
