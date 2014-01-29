/*
#include "../loader/model_loader_list.h"
#include "../loader/structure_loader.h"
#include "../model/model.h"
#include "../structure/cubes/cubes_structure.h"
#include "../structure/cubes/cubes_structure_sqlite.h"
#include "../structure/cubes/cubes_structure_hdf.h"
#include "../../res/cubes_structure_create.h"
#include "../../res/cubes_structure_panel.h"
#include "../progress.h"
#include <utility>


namespace dypc {
	
class cubes_structure_panel : public cubes_structure_panel_ui {
private:
	cubes_structure_base* structure_ = nullptr;

	void update_() {
		if(! structure_) return;
		
		structure_->set_downsampling_minimal_distance( downsampling_spin->GetValue() );
		structure_->set_frustum_culling( frustum_check->GetValue() );
		structure_->set_downsampling_level( level_spin->GetValue() );
		structure_->set_secondary_pass_minimal_distance( secondary_spin->GetValue() );
	}

protected:
	void on_change_(wxCommandEvent&) { update_(); }
	void on_change_(wxSpinEvent&) { update_(); }

public:
	cubes_structure_panel(wxWindow* par) : cubes_structure_panel_ui(par) { }

	void set_structure(cubes_structure_base& str) {
		structure_ = &str;
	}
};

static std::pair<loader*, wxWindow*> create_(model& mod, ::wxWindow* parent) {
	cubes_structure_create_ui dialog(nullptr);
	if(dialog.ShowModal() == wxID_OK) {
		unsigned side = dialog.side_spin->GetValue();
		bool colorize = dialog.colorize_check->GetValue();
		int proxy = dialog.proxy_choice->GetSelection();

		cubes_structure str(side, colorize);
		str.import_model(mod);

		cubes_structure_panel* panel = new cubes_structure_panel(parent);
		
		if(proxy == 1) {
			cubes_structure_sqlite sqlstr(side, colorize);
			sqlstr.import_structure(str);
			auto ld = new structure_loader<cubes_structure_sqlite>(std::move(sqlstr));
			panel->set_structure(ld->structure());
			return std::make_pair(ld, panel);
		} else if(proxy == 2) {
			cubes_structure_hdf hdfstr(side, colorize);
			hdfstr.import_structure(str);
			auto ld = new structure_loader<cubes_structure_hdf>(std::move(hdfstr));
			panel->set_structure(ld->structure());
			return std::make_pair(ld, panel);
		} else {
			auto ld = new structure_loader<cubes_structure>(std::move(str));
			panel->set_structure(ld->structure());
			return std::make_pair(ld, panel);
		}
	}
	
	return std::make_pair(nullptr, nullptr);
}

DYPC_REGISTER_LOADER("Cubes Structure", create_);

}
*/
