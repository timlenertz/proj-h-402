#include "structure_loader.h"
#include "cubes/cubes_structure_loader.h"
#include "cubes_mipmap/cubes_mipmap_structure_loader.h"
#include "tree/octree/octree_structure.h"
#include "tree/octree/octree_mipmap_structure.h"

namespace dypc {

std::string structure_loader::file_formats_to_wildcard(const file_formats_t& file_formats) {
	std::string wildcard;
	for(const auto& p : file_formats) {
		if(wildcard != "") wildcard += "|";
		wildcard += p.second + " (*." + p.first + ")|*." + p.first;
	}
	return wildcard;
}


std::map<structure_loader::structure_type_t, std::string> structure_loader::structure_type_names = {
	{ cubes, "Cubes (Weights)" },
	{ cubes_mipmap, "Cubes (Mipmap)" },
	{ octree, "Octree" },
	{ octree_mipmap, "Octree (Mipmap)" }
};

structure_loader::file_formats_t structure_loader::structure_available_file_formats(structure_type_t tp) {
	switch(tp) {
		case cubes: return cubes_structure_loader::available_file_formats();
		case cubes_mipmap: return cubes_mipmap_structure_loader::available_file_formats();
		case octree: return {{ "hdf", "HDF5" }};
		case octree_mipmap: return {{ "hdf", "HDF5" }};
	}
	throw std::invalid_argument("Invalid argument type");
}

structure_loader* structure_loader::structure_create_file_loader(structure_type_t tp, const std::string& format, const std::string& filepath) {
	switch(tp) {
		case cubes: return cubes_structure_loader::create_file_loader(format, filepath);
		case cubes_mipmap: return cubes_mipmap_structure_loader::create_file_loader(format, filepath);
		case octree: return new octree_structure_hdf_loader(filepath);
		case octree_mipmap: return new octree_mipmap_structure_hdf_loader(filepath);
	}
	throw std::invalid_argument("Invalid argument type");
}

}
