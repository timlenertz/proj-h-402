#ifndef DYPC_STRUCTURE_LOADER_FACTORY_H_
#define DYPC_STRUCTURE_LOADER_FACTORY_H_

#include <string>
#include <utility>
#include "../enums.h"
#include "../util.h"
#include "../downsampling.h"


namespace dypc {

class loader;
class tree_structure_loader;
class model;

/**
 * Read structure type information from HDF file.
 * @param filename Path of HDF file
 * @return Pair of structure type, and number of mipmap levels
 */
std::pair<structure_type, std::size_t> read_hdf_structure_file_type(const std::string& filename);

/**
 * Write structure type information into HDF file.
 * @param filename Path of HDF file
 * @param type Structure type
 * @param levels Mipmap levels
 */
void write_hdf_structure_file_type(const std::string& filename, structure_type type, std::size_t levels = 0);
std::pair<structure_type, std::size_t> read_sqlite_structure_file_type(const std::string& filename);
void write_sqlite_structure_file_type(const std::string& filename, structure_type type, std::size_t levels = 0);

tree_structure_loader* create_tree_structure_memory_loader(structure_type type, unsigned levels, std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, tree_structure_loader_type ltype);

loader* create_structure_file_loader(const std::string& filename, tree_structure_loader_type ltype = tree_structure_loader_type::ordered);

void write_tree_structure_file(const std::string& filename, structure_type type, unsigned levels, std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod);

}




#endif
