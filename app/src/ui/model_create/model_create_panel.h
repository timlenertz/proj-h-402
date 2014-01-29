#ifndef DYPC_UI_MODEL_CREATE_PANEL_H_
#define DYPC_UI_MODEL_CREATE_PANEL_H_

namespace dypc {

class model;
class loader;

class model_create_panel {
public:
	virtual model* create_model() const = 0;
	virtual loader* create_direct_model_loader() const = 0;
};

}

#endif
