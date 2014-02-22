#ifndef DYPC_UI_PLY_MODEL_CREATE_H_
#define DYPC_UI_PLY_MODEL_CREATE_H_

#include "../../../forms/ply_model_create.h"
#include "model_create_panel.h"
#include <dypc/model.h>


namespace dypc {
	
class ply_model_create : public ply_model_create_ui, public model_create_panel {
public:
	ply_model_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : ply_model_create_ui(parent, id) { }
	dypc_model create_model() const override;
};


}

#endif
