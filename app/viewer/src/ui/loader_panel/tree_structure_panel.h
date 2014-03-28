#ifndef DYPC_UI_TREE_STRUCTURE_PANEL_H_
#define DYPC_UI_TREE_STRUCTURE_PANEL_H_

#include "../../../forms/tree_structure_panel.h"
#include "../../updater/updater.h"
#include <dypc/dypc.h>

namespace dypc {

class tree_structure_panel : public tree_structure_panel_ui {
private:
	updater& updater_;

	dypc_cuboid_distance_mode get_selected_distance_() {
		switch(dist_choice->GetSelection()) {
			case 0: return dypc_minimal_cuboid_distance_mode;
			case 1: return dypc_maximal_cuboid_distance_mode;
			case 2: return dypc_mean_cuboid_distance_mode;
			case 3: return dypc_center_cuboid_distance_mode;
			default: return dypc_minimal_cuboid_distance_mode;
		}
	}

	void update_() {
		updater_.set_loader_settings({
			{ "downsampling_setting", (double)setting_spin->GetValue() },
			{ "minimal_number_of_points_for_split", (double)min_split_spin->GetValue() },
			{ "downsampling_node_distance", (double)get_selected_distance_() },
			{ "additional_split_distance_difference", (double)add_split_diff_spin->GetValue() },
		});
	}
	
protected:
	void on_change_(wxCommandEvent&) { update_(); }
	void on_change_(wxSpinEvent&) { update_(); }

public:
	tree_structure_panel(updater& ud, wxWindow* par) : tree_structure_panel_ui(par), updater_(ud) { }
};

}

#endif
