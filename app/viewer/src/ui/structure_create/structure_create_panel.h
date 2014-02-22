#ifndef DYPC_UI_STRUCTURE_CREATE_PANEL_H_
#define DYPC_UI_STRUCTURE_CREATE_PANEL_H_

#include <string>
#include <map>
#include "../../util.h"
#include <dypc/model.h>
#include <dypc/loader.h>

namespace dypc {

class structure_create_panel {
public:
	virtual dypc_loader create_memory_loader(dypc_model) const = 0;
	virtual void write_structure_file(dypc_model mod, const std::string& filename, const std::string& format) const = 0;
};

}

#endif
