#ifndef DYPC_UI_CONCENTRIC_SPHERES_MODEL_CREATE_H_
#define DYPC_UI_CONCENTRIC_SPHERES_MODEL_CREATE_H_

#include "../../../res/concentric_spheres_model_create.h"
#include "model_create_panel.h"
#include "../../model/concentric_spheres_model.h"


namespace dypc {
	

class concentric_spheres_model_create : public concentric_spheres_model_create_ui, public model_create_panel {
public:
	concentric_spheres_model_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : concentric_spheres_model_create_ui(parent, id) { }
	concentric_spheres_model* create_model() const override;
	loader* create_direct_model_loader() const override;
};


}

#endif
