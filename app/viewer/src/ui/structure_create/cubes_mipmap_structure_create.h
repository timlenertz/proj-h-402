#ifndef DYPC_UI_CUBES_MIPMAP_STRUCTURE_CREATE_H_
#define DYPC_UI_CUBES_MIPMAP_STRUCTURE_CREATE_H_

#include "../../../forms/cubes_mipmap_structure_create.h"
#include "structure_create_panel.h"

namespace dypc {
	
class cubes_mipmap_structure_create : public cubes_mipmap_structure_create_ui, public structure_create_panel {	
public:
	cubes_mipmap_structure_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : cubes_mipmap_structure_create_ui(parent, id) { }
	
	dypc_loader create_memory_loader(dypc_model) const override;
	void write_structure_file(dypc_model, const std::string& filename) const override;
};


}

#endif
