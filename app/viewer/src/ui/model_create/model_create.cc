#include "model_create.h"
#include "ply_model_create.h"
#include "concentric_spheres_model_create.h"
#include "torus_model_create.h"

namespace dypc {

void model_create::on_model_choice_(wxCommandEvent& event) {		
	int choice = model_choice->GetSelection(); 
	switch(choice) {
		case 0: set_panel_(new ply_model_create(model_panel)); break;
		case 1: set_panel_(new concentric_spheres_model_create(model_panel)); break;
		case 2: set_panel_(new torus_model_create(model_panel)); break;
	}
}

void model_create::on_ok_(wxCommandEvent& event) {
	EndModal(1);
}

void model_create::on_cancel_(wxCommandEvent& event) {
	EndModal(0);
}

dypc_model model_create::create_model() const {
	if(panel_) return panel_->create_model();
	else return nullptr;
}

model_create::model_create(wxWindow* parent, wxWindowID id) : model_create_ui(parent, id) {
	wxCommandEvent ev;
	on_model_choice_(ev);
}


}
