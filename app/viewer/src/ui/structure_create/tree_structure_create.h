#ifndef DYPC_UI_TREE_STRUCTURE_CREATE_H_
#define DYPC_UI_TREE_STRUCTURE_CREATE_H_

#include "../../../forms/tree_structure_create.h"
#include "structure_create_panel.h"

namespace dypc {
	
class tree_structure_create : public tree_structure_create_ui, public structure_create_panel {
private:
	const dypc_structure_type structure_type_;
	
	std::size_t selected_levels_() const;
	
public:
	tree_structure_create(dypc_structure_type t, wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) :
		structure_type_(t), tree_structure_create_ui(parent, id) { }
	
	dypc_loader create_memory_loader(dypc_model) const override;
	void write_structure_file(dypc_model mod, const std::string& filename) const override;
};


}

#endif
