#ifndef DYPC_UI_MODEL_CREATE_PANEL_H_
#define DYPC_UI_MODEL_CREATE_PANEL_H_

#include <dypc/model.h>

namespace dypc {

class model_create_panel {
public:
	virtual dypc_model create_model() const = 0;
};

}

#endif
