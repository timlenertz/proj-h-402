#ifndef DYPC_UI_TORUS_MODEL_CREATE_H_
#define DYPC_UI_TORUS_MODEL_CREATE_H_

#include "../../../forms/torus_model_create.h"
#include "model_create_panel.h"

namespace dypc {
	
class torus_model;

class torus_model_create : public torus_model_create_ui, public model_create_panel {
public:
	torus_model_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : torus_model_create_ui(parent, id) { }
	dypc_model create_model() const override;
};


}

#endif
