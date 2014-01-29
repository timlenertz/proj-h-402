#ifndef DYPC_UI_CUBES_MIPMAP_STRUCTURE_PANEL_H_
#define DYPC_UI_CUBES_MIPMAP_STRUCTURE_PANEL_H_

#include "../../res/cubes_mipmap_structure_panel.h"

namespace dypc {

class cubes_mipmap_structure_loader;

class cubes_mipmap_structure_panel : public cubes_mipmap_structure_panel_ui {
private:
	cubes_mipmap_structure_loader* loader_ = nullptr;

	void update_();

protected:
	void on_change_(wxCommandEvent&) { update_(); }
	void on_change_(wxSpinEvent&) { update_(); }

public:
	cubes_mipmap_structure_panel(wxWindow* par) : cubes_mipmap_structure_panel_ui(par) { }

	void set_loader(cubes_mipmap_structure_loader& ld) {
		loader_ = &ld;
	}
};

}

#endif
