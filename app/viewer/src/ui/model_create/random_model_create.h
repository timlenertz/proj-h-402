#ifndef DYPC_UI_RANDOM_MODEL_CREATE_H_
#define DYPC_UI_RANDOM_MODEL_CREATE_H_

#include "../../../forms/random_model_create.h"
#include "model_create_panel.h"

namespace dypc {
	
class random_model_create : public random_model_create_ui, public model_create_panel {
public:
	random_model_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : random_model_create_ui(parent, id) { }
	dypc_model create_model() const override;
};


}

#endif
