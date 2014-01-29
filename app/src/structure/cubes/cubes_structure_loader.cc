#include "cubes_structure_loader.h"
#include "cubes_structure_sqlite_loader.h"
#include "cubes_structure_hdf_loader.h"
#include "../../ui/cubes_structure_panel.h"

namespace dypc {

structure_loader::file_formats_t cubes_structure_loader::available_file_formats() {
	return {
		{"hdf", "HDF5"},
		{"db", "SQLite Database"}
	};
}

void cubes_structure_loader::write_to_file(const cubes_structure& s, const std::string& format, const std::string& filepath) {
	if(format == "hdf") cubes_structure_hdf_loader::write(filepath, s);
	else if(format == "db") cubes_structure_sqlite_loader::write(filepath, s);	
	else throw std::invalid_argument("Invalid file format for cubes structure");
}


cubes_structure_loader* cubes_structure_loader::create_file_loader(const std::string& format, const std::string& filepath) {
	if(format == "hdf") return new cubes_structure_hdf_loader(filepath);
	else if(format == "db") return new cubes_structure_sqlite_loader(filepath);	
	else throw std::invalid_argument("Invalid file format for cubes structure");
}

::wxWindow* cubes_structure_loader::create_panel(::wxWindow* parent) {
	cubes_structure_panel* panel = new cubes_structure_panel(parent);
	panel->set_loader(*this);
	return panel;
}


}
