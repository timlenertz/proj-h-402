#ifndef DYPC_UI_TREE_STRUCTURE_CREATE_H_
#define DYPC_UI_TREE_STRUCTURE_CREATE_H_

#include "../../../res/tree_structure_create.h"
#include "../../structure/structure_loader_interface.h"
#include "structure_create_panel.h"

namespace dypc {
	
class loader;
class model;

class tree_structure_create : public tree_structure_create_ui, public structure_create_panel {
private:
	const structure_type_t structure_type_;
	
	std::size_t selected_levels_() const;
	
public:
	tree_structure_create(structure_type_t t, wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) :
		structure_type_(t), tree_structure_create_ui(parent, id) { }
	
	loader* create_memory_loader(model&) const override;
	void write_structure_file(model& mod, const std::string& filename, const std::string& format) const override;
};


}

#endif
