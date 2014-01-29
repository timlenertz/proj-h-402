#ifndef DYPC_UI_TREE_STRUCTURE_PANEL_H_
#define DYPC_UI_TREE_STRUCTURE_PANEL_H_

#include "../../res/tree_structure_panel.h"

namespace dypc {

class tree_structure_loader;

class tree_structure_panel : public tree_structure_panel_ui {
private:
	tree_structure_loader* loader_ = nullptr;

	void update_();

protected:
	void on_change_(wxCommandEvent&) { update_(); }
	void on_change_(wxSpinEvent&) { update_(); }

public:
	tree_structure_panel(wxWindow* par) : tree_structure_panel_ui(par) { }

	void set_loader(tree_structure_loader& ld) {
		loader_ = &ld;
	}
};

}

#endif
