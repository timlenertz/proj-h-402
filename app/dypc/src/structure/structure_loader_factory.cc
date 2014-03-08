#include "structure_loader_factory.h"
#include "../sqlite/sqlite_database.h"
#include <H5Cpp.h>
#include <cstdint>
#include <utility>
#include <memory>

#include "cubes/cubes_structure.h"
#include "cubes/cubes_structure_hdf_loader.h"
#include "cubes/cubes_structure_memory_loader.h"
#include "cubes/cubes_structure_sqlite_loader.h"

#include "cubes_mipmap/cubes_mipmap_structure.h"
#include "cubes_mipmap/cubes_mipmap_structure_hdf_loader.h"
#include "cubes_mipmap/cubes_mipmap_structure_memory_loader.h"

#include "tree/tree_structure_memory_ordered_loader.h"
#include "tree/tree_structure_memory_occluding_loader.h"
#include "tree/tree_structure_memory_simple_loader.h"
#include "tree/tree_structure_hdf_loader.h"
#include "tree/tree_structure_hdf_simple_loader.h"
#include "tree/tree_structure_hdf_write.h"
#include "tree/tree_structure_piecewise_hdf_write.h"

#include "tree/octree/octree_structure.h"
#include "tree/kdtree/kdtree_structure.h"
#include "tree/kdtree_half/kdtree_half_structure.h"

namespace dypc {


template<class Functor, template<std::size_t> class Structure, class... Args>
static typename Functor::result_t call_levels_(const Functor& func, std::size_t levels, Args&&... args) {
	switch(levels) {
		case 1: return func.template call<Structure<1>>(std::forward<Args>(args)...);
		case 4: return func.template call<Structure<4>>(std::forward<Args>(args)...);
		case 8: return func.template call<Structure<8>>(std::forward<Args>(args)...);
		case 16: return func.template call<Structure<16>>(std::forward<Args>(args)...);
		default: return typename Functor::result_t();	
	}
}

template<class Functor, class... Args>
static typename Functor::result_t call_(const Functor& func, structure_type type, std::size_t levels, Args&&... args) {
	switch(type) {
		case structure_type::octree: return call_levels_<Functor, octree_structure>(func, levels, std::forward<Args>(args)...);
		case structure_type::kdtree: return call_levels_<Functor, kdtree_structure>(func, levels, std::forward<Args>(args)...);
		case structure_type::kdtree_half: return call_levels_<Functor, kdtree_half_structure>(func, levels, std::forward<Args>(args)...);
		default: return typename Functor::result_t();
	}
}



struct create_tree_structure_ {
	using result_t = structure*;
	
	template<class Structure>
	result_t call(std::size_t leaf_cap, float mmfac, downsampling_mode dmode, std::size_t dmax, model& mod) const {
		return new Structure(leaf_cap, mmfac, dmode, dmax, mod);
	}

	template<class Structure>
	result_t call(std::size_t leaf_cap, float mmfac, downsampling_mode dmode, std::size_t dmax, model& mod, std::size_t piece_size) const {
		return new Structure(leaf_cap, mmfac, dmode, dmax, mod, piece_size);
	}
};

struct write_tree_structure_to_hdf_ {
	std::string filename;
	
	using result_t = void;

	template<class Structure>
	result_t call(structure& s) const {
		write_to_hdf_piecewise(filename, dynamic_cast<Structure&>(s));
	}
};


template<template<class> class Loader>
struct create_tree_structure_memory_loader_ {
	using result_t = tree_structure_loader*;
	
	template<class Structure>
	result_t call(const structure* s) const {
		return new Loader<Structure>(dynamic_cast<const Structure*>(s));
	}
};

template<template<class> class Loader>
struct create_tree_structure_file_loader_ {
	std::string filename;
	
	using result_t = tree_structure_loader*;
	
	template<class Structure>
	result_t call() const {
		return new Loader<Structure>(filename);
	}
};


std::pair<structure_type, std::size_t> read_hdf_structure_file_type(const std::string& filename) {
	H5::H5File file;
	file.openFile(filename.c_str(), H5F_ACC_RDONLY);
	
	std::uint32_t type_int, levels_int;
	
	H5::Group config_group = file.openGroup("structure_type");
	H5::Attribute attr = config_group.openAttribute("type");
	attr.read(H5::PredType::NATIVE_UINT32, (void*)&type_int);
	attr = config_group.openAttribute("levels");
	attr.read(H5::PredType::NATIVE_UINT32, (void*)&levels_int);
	
	return std::make_pair((structure_type)type_int, levels_int);
}


void write_hdf_structure_file_type(const std::string& filename, structure_type type, std::size_t levels) {
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


std::pair<structure_type, std::size_t> read_sqlite_structure_file_type(const std::string& filename) {
	sqlite_database database(filename);
	auto result = database.select("SELECT type, levels FROM structure_type");
	result.next();
	auto r = result.current_row();
	auto type_int = r[0].int_value();
	auto levels_int = (r[1].is_null() ? 0 : r[1].int_value());
	return std::make_pair((structure_type)type_int, levels_int);
}


void write_sqlite_structure_file_type(const std::string& filename, structure_type type, std::size_t levels) {
	sqlite_database database(filename);
	database.execute("CREATE TABLE structure_type ( type INTEGER NOT NULL, levels INTEGER )");
	int type_int = (int)type;
	auto ins = database.prepare("INSERT INTO structure_type (type, levels) VALUES (?, ?)");
	if(levels) ins(type_int, levels);
	else ins(type_int, nullptr);
}




tree_structure_loader* create_tree_structure_memory_loader(structure_type type, unsigned levels, std::size_t leaf_cap, float mmfac, downsampling_mode dmode, std::size_t dmax, model& mod, tree_structure_loader_type ltype) {	
	structure* s = call_(create_tree_structure_(), type, levels, leaf_cap, mmfac, dmode, dmax, mod);
	
	tree_structure_loader* ld = nullptr;
	switch(ltype) {
		case tree_structure_loader_type::simple: ld = call_(create_tree_structure_memory_loader_<tree_structure_memory_simple_loader>(), type, levels, s); break;
		case tree_structure_loader_type::ordered: ld = call_(create_tree_structure_memory_loader_<tree_structure_memory_ordered_loader>(), type, levels, s); break;
 		case tree_structure_loader_type::occluding: ld = call_(create_tree_structure_memory_loader_<tree_structure_memory_occluding_loader>(), type, levels, s); break;
	}
	
	if(ld) return ld;
	else throw std::invalid_argument("Invalid memory tree structure loader");
}



loader* create_structure_file_loader(const std::string& filename, tree_structure_loader_type ltype) {
	loader* ld = nullptr;
		
	auto ext = file_path_extension(filename);
	if(ext == "hdf") {
		auto type = read_hdf_structure_file_type(filename);
		if(type.first == structure_type::cubes) {
			ld = new cubes_structure_hdf_loader(filename);
		} else if(type.first == structure_type::cubes_mipmap) {
			ld = new cubes_mipmap_structure_hdf_loader(filename);
		} else {
			ld = call_(create_tree_structure_file_loader_<tree_structure_hdf_loader> {filename}, type.first, type.second);
		}
	} else if(ext == "db") {
		auto type = read_sqlite_structure_file_type(filename);
		if(type.first == structure_type::cubes) {
			ld = new cubes_structure_sqlite_loader(filename);
		}
	}
	
	if(ld) return ld;
	else throw std::invalid_argument("Invalid file tree structure loader");
}


void write_tree_structure_file(const std::string& filename, structure_type type, unsigned levels, std::size_t leaf_cap, float mmfac, downsampling_mode dmode, std::size_t dmax, model& mod) {
	const std::size_t maximal_number_of_points_per_piece = 10000;//16 * 1024 * 1024;
	
	auto ext = file_path_extension(filename);
	std::unique_ptr<structure> s( call_(create_tree_structure_(), type, levels, leaf_cap, mmfac, dmode, dmax, mod, maximal_number_of_points_per_piece) );
	
	if(ext == "hdf") {
		write_tree_structure_to_hdf_ f {filename};
		call_(f, type, levels, *s);
		write_hdf_structure_file_type(filename, type, levels);
	} else {
		throw std::invalid_argument("Invalid file format");
	}
}

}
