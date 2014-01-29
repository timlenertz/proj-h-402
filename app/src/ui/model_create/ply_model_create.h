#ifndef DYPC_UI_PLY_MODEL_CREATE_H_
#define DYPC_UI_PLY_MODEL_CREATE_H_

#include "../../../res/ply_model_create.h"
#include "model_create_panel.h"
#include "../../model/ply_model.h"


namespace dypc {
	
class ply_model;

class ply_model_create : public ply_model_create_ui, public model_create_panel {
public:
	ply_model_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : ply_model_create_ui(parent, id) { }
	ply_model* create_model() const override;
	loader* create_direct_model_loader() const override;
};


}

#endif
