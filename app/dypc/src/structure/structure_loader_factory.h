#ifndef DYPC_STRUCTURE_LOADER_FACTORY_H_
#define DYPC_STRUCTURE_LOADER_FACTORY_H_

#include <string>
#include <utility>
#include "../util.h"
#include "../downsampling.h"


namespace dypc {

class loader;
class tree_structure_loader;
class model;

enum structure_type_t {
	cubes_structure_type = 0,
	cubes_mipmap_structure_type,
	octree_structure_type,
	kdtree_structure_type,
	kdtree_half_structure_type
};

enum tree_structure_loader_type_t {
	simple_tree_structure_loader_type = 0,
	ordered_tree_structure_loader_type,
	occluding_tree_structure_loader_type
};


std::pair<structure_type_t, std::size_t> read_hdf_structure_file_type(const std::string& filename);
void write_hdf_structure_file_type(const std::string& filename, structure_type_t type, std::size_t levels = 0);
std::pair<structure_type_t, std::size_t> read_sqlite_structure_file_type(const std::string& filename);
void write_sqlite_structure_file_type(const std::string& filename, structure_type_t type, std::size_t levels = 0);

tree_structure_loader* create_tree_structure_memory_loader(structure_type_t type, unsigned levels, std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod, tree_structure_loader_type_t ltype);

loader* create_structure_file_loader(const std::string& filename, tree_structure_loader_type_t ltype = ordered_tree_structure_loader_type);

void write_tree_structure_file(const std::string& filename, structure_type_t type, unsigned levels, std::size_t leaf_cap, float mmfac, downsampling_mode_t dmode, std::size_t dmax, model& mod);


}




#endif
