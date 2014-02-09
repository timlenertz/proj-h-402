#include "structure_loader_interface.h"
#include "../sqlite/sqlite_database.h"
#include <H5Cpp.h>
#include <cstdint>
#include <utility>

#include "cubes/cubes_structure.h"
#include "cubes/cubes_structure_hdf_loader.h"
#include "cubes/cubes_structure_memory_loader.h"
#include "cubes/cubes_structure_sqlite_loader.h"

#include "cubes_mipmap/cubes_mipmap_structure.h"
#include "cubes_mipmap/cubes_mipmap_structure_hdf_loader.h"
#include "cubes_mipmap/cubes_mipmap_structure_memory_loader.h"

#include "tree/tree_structure_memory_loader.h"
#include "tree/tree_structure_memory_simple_loader.h"
#include "tree/tree_structure_hdf_loader.h"
#include "tree/tree_structure_hdf_simple_loader.h"
#include "tree/tree_structure_hdf_loader_base.h"

#include "tree/octree/octree_structure.h"
#include "tree/kdtree/kdtree_structure.h"
#include "tree/kdtree_half/kdtree_half_structure.h"

namespace dypc {

static const user_choices_t tree_loader_choice_ = {
	{ "simple", "Simple" },
	{ "ordered", "Ordered Nodes" }
};

const user_choices_t structure_file_formats = {
	{ "hdf", "HDF5" },
	{ "db", "SQLite Database" }
};


template<template<class> class Loader, template<std::size_t> class Structure, class... Args>
static tree_structure_loader* create_tree_structure_loader_(std::size_t levels, Args&&... args) {
	switch(levels) {
		case 1: return new Loader<Structure<1>>(std::forward<Args>(args)...);	
		case 4: return new Loader<Structure<4>>(std::forward<Args>(args)...);	
		case 8: return new Loader<Structure<8>>(std::forward<Args>(args)...);	
		case 16: return new Loader<Structure<16>>(std::forward<Args>(args)...);
		default: return nullptr;	
	}
}

template<template<class> class Loader, class... Args>
static tree_structure_loader* create_tree_structure_loader_for_structure_type_(structure_type_t type, unsigned levels, Args&&... args) {
	switch(type) {
		case octree_structure_type: return create_tree_structure_loader_<Loader, octree_structure>(levels, std::forward<Args>(args)...);
		case kdtree_structure_type: return create_tree_structure_loader_<Loader, kdtree_structure>(levels, std::forward<Args>(args)...);
		case kdtree_half_structure_type: return create_tree_structure_loader_<Loader, kdtree_half_structure>(levels, std::forward<Args>(args)...);
		default: return nullptr;
	}
}




std::pair<structure_type_t, std::size_t> read_hdf_structure_file_type(const std::string& filename) {
	H5::H5File file;
	file.openFile(filename.c_str(), H5F_ACC_RDONLY);
	
	std::uint32_t type_int, levels_int;
	
	H5::Group config_group = file.openGroup("structure_type");
	H5::Attribute attr = config_group.openAttribute("type");
	attr.read(H5::PredType::NATIVE_UINT32, (void*)&type_int);
	attr = config_group.openAttribute("levels");
	attr.read(H5::PredType::NATIVE_UINT32, (void*)&levels_int);
	
	return std::make_pair((structure_type_t)type_int, levels_int);
}


void write_hdf_structure_file_type(const std::string& filename, structure_type_t type, std::size_t levels) {
	H5::H5File file;
	file.openFile(filename.c_str(), H5F_ACC_RDWR);

	H5::Group config_group = file.createGroup("structure_type");

	std::uint32_t type_int = (std::uint32_t)type;
	std::uint32_t levels_int = levels;

	H5::Attribute attr = config_group.createAttribute("type", H5::PredType::NATIVE_UINT32, H5::DataSpace(H5S_SCALAR));
	attr.write(H5::PredType::NATIVE_UINT32, (const void*)&type_int);
	attr = config_group.createAttribute("levels", H5::PredType::NATIVE_UINT32, H5::DataSpace(H5S_SCALAR));
	attr.write(H5::PredType::NATIVE_UINT32, (const void*)&levels_int);
}


std::pair<structure_type_t, std::size_t> read_sqlite_structure_file_type(const std::string& filename) {
	sqlite_database database(filename);
	auto result = database.select("SELECT type, levels FROM structure_type");
	result.next();
	auto r = result.current_row();
	auto type_int = r[0].int_value();
	auto levels_int = (r[1].is_null() ? 0 : r[1].int_value());
	return std::make_pair((structure_type_t)type_int, levels_int);
}


void write_sqlite_structure_file_type(const std::string& filename, structure_type_t type, std::size_t levels) {
	sqlite_database database(filename);
	database.execute("CREATE TABLE structure_type ( type INTEGER NOT NULL, levels INTEGER )");
	int type_int = (int)type;
	auto ins = database.prepare("INSERT INTO structure_type (type, levels) VALUES (?, ?)");
	if(levels) ins(type_int, levels);
	else ins(type_int, nullptr);
}




tree_structure_loader* create_tree_structure_memory_loader(structure_type_t type, unsigned levels, std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod) {
	auto ltype = user_choice(tree_loader_choice_, "Type of memory tree loader");
	
	tree_structure_loader* ld = nullptr;
	
	if(ltype == "simple") ld = create_tree_structure_loader_for_structure_type_<tree_structure_memory_simple_loader>(type, levels, leaf_cap, mmfac, dmode, dmax, mod);
	else if(ltype == "ordered") ld = create_tree_structure_loader_for_structure_type_<tree_structure_memory_loader>(type, levels, leaf_cap, mmfac, dmode, dmax, mod);
	
	if(ld) return ld;
	else throw std::invalid_argument("Invalid memory tree structure loader");
}



loader* create_structure_file_loader(const std::string& filename) {
	loader* ld = nullptr;
	
	auto ext = file_path_extension(filename);
	if(ext == "hdf") {
		auto type = read_hdf_structure_file_type(filename);
		if(type.first == cubes_structure_type) {
			ld = new cubes_structure_hdf_loader(filename);
		} else if(type.first == cubes_mipmap_structure_type) {
			ld = new cubes_mipmap_structure_hdf_loader(filename);
		} else {
			auto ltype = user_choice(tree_loader_choice_, "Type of HDF tree loader");
			if(ltype == "simple") ld = create_tree_structure_loader_for_structure_type_<tree_structure_hdf_simple_loader>(type.first, type.second, filename);
			else if(ltype == "ordered") ld = create_tree_structure_loader_for_structure_type_<tree_structure_hdf_simple_loader>(type.first, type.second, filename);
		}
	} else if(ext == "db") {
		auto type = read_sqlite_structure_file_type(filename);
		if(type.first == cubes_structure_type) {
			ld = new cubes_structure_sqlite_loader(filename);
		}
	}
	
	if(ld) return ld;
	else throw std::invalid_argument("Invalid file tree structure loader");
}



template<template<class> class Loader, template<std::size_t> class Structure, class... Args>
static void write_tree_structure_file_(const std::string& filename, std::size_t levels, Args&&... args) {
	switch(levels) {
		case 1: Loader<Structure<1>>::write(filename, Structure<1>(std::forward<Args>(args)...)); break;
		case 4: Loader<Structure<4>>::write(filename, Structure<4>(std::forward<Args>(args)...)); break;
		case 8: Loader<Structure<8>>::write(filename, Structure<8>(std::forward<Args>(args)...)); break;
		case 16: Loader<Structure<16>>::write(filename, Structure<16>(std::forward<Args>(args)...)); break;
		default: throw std::invalid_argument("Invalid type of tree structure");
	}
}



void write_tree_structure_file(const std::string& filename, structure_type_t type, unsigned levels, std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod) {
	auto ext = file_path_extension(filename);
	if(ext == "hdf") {
		switch(type) {
			case octree_structure_type: write_tree_structure_file_<tree_structure_hdf_loader_base, octree_structure>(filename, levels, leaf_cap, mmfac, dmode, dmax, mod); break;
			case kdtree_structure_type: write_tree_structure_file_<tree_structure_hdf_loader_base, kdtree_structure>(filename, levels, leaf_cap, mmfac, dmode, dmax, mod); break;
			case kdtree_half_structure_type: write_tree_structure_file_<tree_structure_hdf_loader_base, kdtree_half_structure>(filename, levels, leaf_cap, mmfac, dmode, dmax, mod); break;
			default: std::invalid_argument("Invalid type of tree structure");
		}
		write_hdf_structure_file_type(filename, type, levels);
	} else {
		throw std::invalid_argument("Invalid file format");
	}
}

}
