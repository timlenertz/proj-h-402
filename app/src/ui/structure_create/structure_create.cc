#include "structure_create.h"
#include "tree_structure_create.h"
#include "cubes_structure_create.h"
#include "cubes_mipmap_structure_create.h"
#include "../../structure/structure_loader_interface.h"
#include "../../structure/tree/octree/octree_structure.h"
#include "../../structure/tree/kdtree/kdtree_structure.h"
#include "../../structure/tree/kdtree_half/kdtree_half_structure.h"
#include "../../structure/structure_loader.h"
#include "../../util.h"
#include <stdexcept>
#include <wx/filedlg.h>

namespace dypc {

void structure_create::on_structure_choice_(wxCommandEvent& event) {	
	int choice = structure_choice->GetSelection(); 
	switch(choice) {
		case 0: set_panel_(new cubes_structure_create(structure_panel)); break;
		case 1: set_panel_(new cubes_mipmap_structure_create(structure_panel)); break;
		case 2: set_panel_(new tree_structure_create(octree_structure_type, structure_panel)); break;
		case 3: set_panel_(new tree_structure_create(kdtree_structure_type, structure_panel)); break;
		case 4: set_panel_(new tree_structure_create(kdtree_half_structure_type, structure_panel)); break;
	}
}

void structure_create::on_ok_(wxCommandEvent& event) {
	EndModal(1);
}

void structure_create::on_cancel_(wxCommandEvent& event) {
	EndModal(0);
}

loader* structure_create::create_memory_loader(model& mod) const {
	if(panel_) return panel_->create_memory_loader(mod);
	else return nullptr;
}

std::string structure_create::write_structure_file(model& mod) const {
	if(! panel_) return "";

	std::string wildcard = file_formats_to_wildcard(structure_file_formats);
	
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
	std::string format = file_path_extension(filename);
	
	try {
		panel_->write_structure_file(mod, filename, format);
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
