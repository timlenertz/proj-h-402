#ifndef DYPC_UI_CUBES_MIPMAP_STRUCTURE_PANEL_H_
#define DYPC_UI_CUBES_MIPMAP_STRUCTURE_PANEL_H_

#include "../../../forms/cubes_mipmap_structure_panel.h"
#include "../../updater/updater.h"

namespace dypc {

class cubes_mipmap_structure_panel : public cubes_mipmap_structure_panel_ui {
private:
	updater& updater_;

	void update_() {
		updater_.set_loader_settings({
			{ "frustum_culling", (frustum_check->GetValue() ? 1.0 : 0.0) }
		});
	}

protected:
	void on_change_(wxCommandEvent&) { update_(); }
	void on_change_(wxSpinEvent&) { update_(); }

public:
	cubes_mipmap_structure_panel(updater& ud, wxWindow* par) : cubes_mipmap_structure_panel_ui(par), updater_(ud) { }
};

}

#endif
