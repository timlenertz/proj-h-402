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
 * @param filename Path of HDF file.
 * @return Pair of structure type, and number of mipmap levels.
 */
std::pair<structure_type, std::size_t> read_hdf_structure_file_type(const std::string& filename);

/**
 * Write structure type information into HDF file.
 * @param filename Path of HDF file.
 * @param type Structure type.
 * @param levels Mipmap levels.
 */
void write_hdf_structure_file_type(const std::string& filename, structure_type type, std::size_t levels = 0);

/**
 * Read structure type information from SQLite file.
 * @param filename Path of HDF file.
 * @return Pair of structure type, and number of mipmap levels.
 */
std::pair<structure_type, std::size_t> read_sqlite_structure_file_type(const std::string& filename);

/**
 * Write structure type information into SQLite file.
 * @param filename Path of HDF file.
 * @param type Structure type.
 * @param levels Mipmap levels.
 */
void write_sqlite_structure_file_type(const std::string& filename, structure_type type, std::size_t levels = 0);

/**
 * Create tree structure, and memory loader for it.
 * This function allows for the structure type, downsampling levels, and loader type to be specified as runtime variables. One of the (template) classes defined in structure_loader_factory.cc is chosen based on the parameters.
 * @param type The structure type, must be a tree structure type.
 * @param levels Downsampling levels, must be 1, 4, 8 or 16.
 * @param leaf_cap Leaf capacity of the tree structure.
 * @param dmin Minimal downsampling output.
 * @param damount Downsampling amount.
 * @param mod The model. Must exist for lifetime of returned loader.
 * @param ltype Tree structure loader type.
 * @return Pointer to a new memory loader for the tree structure. Must be deleted by the caller.
 */
tree_structure_loader* create_tree_structure_memory_loader(structure_type type, unsigned levels, std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod, tree_structure_loader_type ltype);

/**
 * Create loader from a structure file.
 * The file type is determined from the file name extension, and the structure type information is read from the file.
 * @param filename Full path to file. Needs to have correct extension.
 * @param ltype Tree structure loader type. Ignored for non-tree structures.
 * @return Pointer to a new file loader for the file. Must be deleted by the caller.
 */
loader* create_structure_file_loader(const std::string& filename, tree_structure_loader_type ltype = tree_structure_loader_type::ordered);


/**
 * Create tree structure, and store it in file.
 * This function allows for the structure type, downsampling levels, and loader type to be specified as runtime variables. One of the (template) classes defined in structure_loader_factory.cc is chosen based on the parameters. The file type is determined from the file name extension, and the structure type information is written to the file.
 * @param filename Output file path, with extension.
 * @param type The structure type, must be a tree structure type.
 * @param levels Downsampling levels, must be 1, 4, 8 or 16.
 * @param leaf_cap Leaf capacity of the tree structure.
 * @param dmin Minimal downsampling output.
 * @param damount Downsampling amount.
 * @param mod The model. Must exist for lifetime of returned loader.
 */
void write_tree_structure_file(const std::string& filename, structure_type type, unsigned levels, std::size_t leaf_cap, std::size_t dmin, float damount, downsampling_mode dmode, model& mod);

}




#endif
