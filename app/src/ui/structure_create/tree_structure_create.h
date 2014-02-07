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

template<template<std::size_t> class Structure>
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



template<template<std::size_t> class Structure>
const user_choices_t tree_structure_create<Structure>::loader_variants_ = {
	{ "ordered", "Ordered Loader" },
	{ "simple", "Simple Loader" }
};

template<template<std::size_t> class Structure>
loader* tree_structure_create<Structure>::create_memory_loader(model& mod) const {
	unsigned cap = capacity_spin->GetValue();
	unsigned dmax = dmax_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	auto mode = (mode_choice->GetSelection() == 0 ? random_downsampling_mode : uniform_downsampling_mode);
	auto levels = levels_choice->GetSelection();
	
	auto variant = user_choice(loader_variants_, "Loader variant");
	if(levels == 0) {
		if(variant == "simple") return new tree_structure_memory_simple_loader<Structure<1>>(cap, factor, mode, dmax, mod);
		else if(variant == "ordered") return new tree_structure_memory_loader<Structure<1>>(cap, factor, mode, dmax, mod);
	} else if(levels == 1) {
		if(variant == "simple") return new tree_structure_memory_simple_loader<Structure<4>>(cap, factor, mode, dmax, mod);
		else if(variant == "ordered") return new tree_structure_memory_loader<Structure<4>>(cap, factor, mode, dmax, mod);
	} else if(levels == 2) {
		if(variant == "simple") return new tree_structure_memory_simple_loader<Structure<8>>(cap, factor, mode, dmax, mod);
		else if(variant == "ordered") return new tree_structure_memory_loader<Structure<8>>(cap, factor, mode, dmax, mod);
	} else if(levels == 3) {
		if(variant == "simple") return new tree_structure_memory_simple_loader<Structure<16>>(cap, factor, mode, dmax, mod);
		else if(variant == "ordered") return new tree_structure_memory_loader<Structure<16>>(cap, factor, mode, dmax, mod);
	}
	return nullptr;
}

template<template<std::size_t> class Structure>
user_choices_t tree_structure_create<Structure>::available_file_formats() {
	return {
		{"hdf", "HDF5"}
	};
}


template<template<std::size_t> class Structure>
void tree_structure_create<Structure>::write_structure_file(model& mod, const std::string& filename, const std::string& format) const {
	unsigned cap = capacity_spin->GetValue();
	unsigned dmax = dmax_spin->GetValue();
	double factor; factor_text->GetLineText(0).ToDouble(&factor);
	auto mode = (mode_choice->GetSelection() == 0 ? random_downsampling_mode : uniform_downsampling_mode);
	auto levels = levels_choice->GetSelection();

	if(format == "hdf") {
		if(levels == 0) tree_structure_hdf_loader_base<Structure<1>>::write(filename, Structure<1>(cap, factor, mode, dmax, mod));
		else if(levels == 1) tree_structure_hdf_loader_base<Structure<4>>::write(filename, Structure<4>(cap, factor, mode, dmax, mod));
		else if(levels == 2) tree_structure_hdf_loader_base<Structure<8>>::write(filename, Structure<8>(cap, factor, mode, dmax, mod));
		else if(levels == 3) tree_structure_hdf_loader_base<Structure<16>>::write(filename, Structure<16>(cap, factor, mode, dmax, mod));
	}
}


template<template<std::size_t> class Structure>
loader* tree_structure_create<Structure>::create_file_loader(const std::string& filename, const std::string& format) {
	if(format == "hdf") {
		/*
		auto variant = user_choice(loader_variants_, "Loader variant");
		if(levels == 0) {
			if(variant == "ordered") return new tree_structure_hdf_simple_loader<Structure<1>>(cap, factor, mode, dmax, mod);
			else if(variant == "separate") return new tree_structure_hdf_loader<Structure<1>>(cap, factor, mode, dmax, mod);
		} else if(levels == 1) {
			if(variant == "ordered") return new tree_structure_hdf_simple_loader<Structure<4>>(cap, factor, mode, dmax, mod);
			else if(variant == "separate") return new tree_structure_hdf_loader<Structure<4>>(cap, factor, mode, dmax, mod);
		} else if(levels == 2) {
			if(variant == "ordered") return new tree_structure_hdf_simple_loader<Structure<8>>(cap, factor, mode, dmax, mod);
			else if(variant == "separate") return new tree_structure_hdf_loader<Structure<8>>(cap, factor, mode, dmax, mod);
		} else if(levels == 3) {
			if(variant == "ordered") return new tree_structure_hdf_simple_loader<Structure<16>>(cap, factor, mode, dmax, mod);
			else if(variant == "separate") return new tree_structure_hdf_loader<Structure<16>>(cap, factor, mode, dmax, mod);
		}
		*/
	}
	return nullptr;
}




}

#endif
