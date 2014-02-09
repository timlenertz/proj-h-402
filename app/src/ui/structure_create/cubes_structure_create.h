#ifndef DYPC_UI_CUBES_STRUCTURE_CREATE_H_
#define DYPC_UI_CUBES_STRUCTURE_CREATE_H_

#include "../../../res/cubes_structure_create.h"
#include "structure_create_panel.h"

namespace dypc {
	
class loader;
class model;

class cubes_structure_create : public cubes_structure_create_ui, public structure_create_panel {	
public:
	cubes_structure_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : cubes_structure_create_ui(parent, id) { }
	
	loader* create_memory_loader(model&) const override;
	void write_structure_file(model& mod, const std::string& filename, const std::string& format) const override;
};


}

#endif
