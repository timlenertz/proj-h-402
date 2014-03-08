/*#ifndef DYPC_UI_HPR_PANEL_H_
#define DYPC_UI_HPR_PANEL_H_

#include "../../res/hpr_panel.h"

namespace dypc {

class hpr_loader;

class hpr_panel : public hpr_panel_ui {
private:
	hpr_loader* loader_ = nullptr;
	wxWindow* underlying_ = nullptr;

	void update_();
	void trigger_();
	void underlying_loader_settings_();

protected:
	void on_change_(wxCommandEvent&) { update_(); }
	void on_trigger_(wxCommandEvent&) { trigger_(); }
	void on_underlying_(wxCommandEvent&) { underlying_loader_settings_(); }
	void on_change_(wxSpinEvent&) { update_(); }

public:
	hpr_panel(wxWindow* par) : hpr_panel_ui(par) { }

	void set_loader(hpr_loader& ld, wxWindow* underlying) {
		loader_ = &ld;
		underlying_ = underlying;
	}
};

}

#endif
*/
