#include "tree_structure_panel.h"
#include "../structure/tree/tree_structure_loader.h"

namespace dypc {

void tree_structure_panel::update_() {
	if(! loader_) return;
	
	tree_structure_loader::downsampling_node_distance_t dd;
	switch(dist_choice->GetSelection()) {
		case 0: dd = tree_structure_loader::minimal_node_distance; break;
		case 1: dd = tree_structure_loader::maximal_node_distance; break;
		case 2: dd = tree_structure_loader::mean_node_distance; break;
		case 3: dd = tree_structure_loader::center_node_distance; break;
		default: dd = tree_structure_loader::minimal_node_distance; break;
	}
	
	loader_->set_downsampling_start_distance( start_spin->GetValue() );
	loader_->set_downsampling_step_distance( step_spin->GetValue() );
	loader_->set_minimal_number_of_points_for_split( min_split_spin->GetValue() );
	loader_->set_downsampling_node_distance( dd );
	loader_->set_additional_split_distance_difference( add_split_diff_spin->GetValue() );
}

}
