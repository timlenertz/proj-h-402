#include "structure_create.h"
#include "cubes_structure_create.h"
#include "cubes_mipmap_structure_create.h"
#include "octree_structure_create.h"
#include "octree_mipmap_structure_create.h"
#include "kdtree_structure_create.h"
#include "kdtree_mipmap_structure_create.h"
#include "../../structure/structure_loader.h"
#include "../../util.h"

#include <wx/filedlg.h>

namespace dypc {

void structure_create::on_structure_choice_(wxCommandEvent& event) {	
	int choice = structure_choice->GetSelection(); 
	switch(choice) {
		case 0: set_panel_(new cubes_structure_create(structure_panel)); break;
		case 1: set_panel_(new cubes_mipmap_structure_create(structure_panel)); break;
		case 2: set_panel_(new octree_structure_create(structure_panel)); break;
		case 3: set_panel_(new octree_mipmap_structure_create(structure_panel)); break;
		case 4: set_panel_(new kdtree_structure_create(structure_panel)); break;
		case 5: set_panel_(new kdtree_mipmap_structure_create(structure_panel)); break;
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

	std::string wildcard = file_formats_to_wildcard(panel_->instance_available_file_formats());
	
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
	
	panel_->write_structure_file(mod, filename, format);

	return filename;
}

loader* structure_create::create_file_loader() {
	user_choices_t structure_choices = {
		{ "cubes", "Cubes (Weights)" },
		{ "cubes_mipmap", "Cubes (Mipmap)" },
		{ "octree", "Octree" },
		{ "octree_mipmap", "Octree (Mipmap)" },
		{ "kdtree", "KdTree" },
		{ "kdtree_mipmap", "KdTree (Mipmap)" }
	};
	auto structure_type = user_choice(structure_choices, "Type of structure");
	
	user_choices_t formats;
	if(structure_type == "cubes") formats = cubes_structure_create::available_file_formats();
	else if(structure_type == "cubes_mipmap") formats = cubes_mipmap_structure_create::available_file_formats();
	else if(structure_type == "octree") formats = octree_structure_create::available_file_formats();
	else if(structure_type == "octree_mipmap") formats = octree_mipmap_structure_create::available_file_formats();
	else if(structure_type == "kdtree") formats = kdtree_structure_create::available_file_formats();
	else if(structure_type == "kdtree_mipmap") formats = kdtree_mipmap_structure_create::available_file_formats();
	else return nullptr;

	auto wildcard = file_formats_to_wildcard(formats);

	wxFileDialog open_dialog(
		nullptr,
		wxT("Open structure file"),
		wxEmptyString,
		wxEmptyString,
		wxString(wildcard.c_str(), wxConvUTF8),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST
	);
	auto result = open_dialog.ShowModal();
	if(result == wxID_CANCEL) return nullptr;
		
	std::string filename(open_dialog.GetPath().utf8_str());		
	std::string format = file_path_extension(filename);

	if(structure_type == "cubes") return cubes_structure_create::create_file_loader(filename, format);
	else if(structure_type == "cubes_mipmap") return cubes_mipmap_structure_create::create_file_loader(filename, format);
	else if(structure_type == "octree") return octree_structure_create::create_file_loader(filename, format);
	else if(structure_type == "octree_mipmap") return octree_mipmap_structure_create::create_file_loader(filename, format);
	else if(structure_type == "kdtree") return kdtree_structure_create::create_file_loader(filename, format);
	else if(structure_type == "kdtree_mipmap") return kdtree_mipmap_structure_create::create_file_loader(filename, format);
	else return nullptr;
}


structure_create::structure_create(wxWindow* parent, wxWindowID id) : structure_create_ui(parent, id) {
	wxCommandEvent ev;
	on_structure_choice_(ev);
}


}
