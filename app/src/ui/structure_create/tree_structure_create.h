#ifndef DYPC_UI_TREE_STRUCTURE_CREATE_H_
#define DYPC_UI_TREE_STRUCTURE_CREATE_H_

#include "../../../res/tree_structure_create.h"
#include "structure_create_panel.h"
#include "../../structure/structure.h"
#include "../../structure/structure_loader.h"
#include "../../structure/tree/tree_structure.h"
#include "../../structure/tree/tree_structure_memory_loader.h"
#include "../../structure/tree/tree_structure_memory_simple_loader.h"
#include "../../structure/tree/tree_structure_hdf_loader.h"
#include "../../structure/tree/tree_structure_hdf_simple_loader.h"

namespace dypc {
	
class loader;
class model;

template<class Structure>
class tree_structure_create : public tree_structure_create_ui, public structure_create_panel {	
private:
	static const user_choices_t loader_variants_;


public:
	tree_structure_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : tree_structure_create_ui(parent, id) { }

	loader* create_memory_loader(model&) const override;
	void write_structure_file(model& mod, const std::string& filename, const std::string& format) const override;

	static user_choices_t available_file_formats();
	static loader* create_file_loader(const std::string& filename, const std::string& format); 
	
	user_choices_t instance_available_file_formats() const override { return available_file_formats(); }
	loader* instance_create_file_loader(const std::string& filename, const std::string& format) const override { return create_file_loader(filename, format); }
};



template<class Structure>
const user_choices_t tree_structure_create<Structure>::loader_variants_ = {
	{ "ordered", "Ordered Loader" },
	{ "separate", "Simple Loader" }
};


template<class Structure>
loader* tree_structure_create<Structure>::create_memory_loader(model& mod) const {
	unsigned cap = capacity_spin->GetValue();
	
	auto variant = user_choice(loader_variants_, "Loader variant");
	if(variant == "ordered") return new tree_structure_memory_simple_loader<Structure>(cap, mod);
	else if(variant == "separate") return new tree_structure_memory_loader<Structure>(cap, mod);
	else return nullptr;
}

template<class Structure>
user_choices_t tree_structure_create<Structure>::available_file_formats() {
	return {
		{"hdf", "HDF5"}
	};
}


template<class Structure>
void tree_structure_create<Structure>::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned cap = capacity_spin->GetValue();

	Structure s(cap, mod);
	if(format == "hdf") tree_structure_hdf_simple_loader<Structure>::write(filename, s);
}


template<class Structure>
loader* tree_structure_create<Structure>::create_file_loader(const std::string& filename, const std::string& format) {
	if(format == "hdf") {
		auto variant = user_choice(loader_variants_, "Loader variant");
		if(variant == "ordered") return new tree_structure_hdf_simple_loader<Structure>(filename);
		else if(variant == "separate") return new tree_structure_hdf_loader<Structure>(filename);
	}
	return nullptr;
}




}

#endif
