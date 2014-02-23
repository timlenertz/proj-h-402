#include "structure_create.h"
#include "cubes_structure_create.h"
#include "cubes_mipmap_structure_create.h"
#include "tree_structure_create.h"
#include "../../util.h"
#include <stdexcept>
#include <wx/filedlg.h>

namespace dypc {

void structure_create::on_structure_choice_(wxCommandEvent& event) {	
	int choice = structure_choice->GetSelection(); 
	switch(choice) {
		case 0: set_panel_(new cubes_structure_create(structure_panel)); break;
		case 1: set_panel_(new cubes_mipmap_structure_create(structure_panel)); break;
		case 2: set_panel_(new tree_structure_create(dypc_octree_tree_structure_type, structure_panel)); break;
		case 3: set_panel_(new tree_structure_create(dypc_kdtree_tree_structure_type, structure_panel)); break;
		case 4: set_panel_(new tree_structure_create(dypc_kdtree_half_tree_structure_type, structure_panel)); break;
	}
}

void structure_create::on_ok_(wxCommandEvent& event) {
	EndModal(1);
}

void structure_create::on_cancel_(wxCommandEvent& event) {
	EndModal(0);
}

dypc_loader structure_create::create_memory_loader(dypc_model mod) const {
	if(panel_) return panel_->create_memory_loader(mod);
	else return nullptr;
}

std::string structure_create::write_structure_file(dypc_model mod) const {
	if(! panel_) return "";

	std::string wildcard = file_formats_to_wildcard({
		{ "hdf", "HDF5" },
		{ "db", "SQLite Database" }
	});
	
	wxFileDialog save_dialog(
		nullptr,
		wxT("Save structure file"),
		wxEmptyString,
		wxEmptyString,
		wxString(wildcard.c_str(), wxConvUTF8),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT
	);
	
	auto res = save_dialog.ShowModal();
	if(res == wxID_CANCEL) return "";
	
	std::string filename(save_dialog.GetPath().utf8_str());
	
	try {
		panel_->write_structure_file(mod, filename);
	} catch(const std::exception& ex) {
		error_message(ex.what(), "Could not write structure file");
	}

	return filename;
}

structure_create::structure_create(wxWindow* parent, wxWindowID id) : structure_create_ui(parent, id) {
	wxCommandEvent ev;
	on_structure_choice_(ev);
}


}
