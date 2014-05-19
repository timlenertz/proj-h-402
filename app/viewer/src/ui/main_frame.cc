#include "main_frame.h"

#include "../renderer/renderer.h"
#include "renderer_canvas.h"
#include "setting_output_statistics.h"

#include "../ui/model_create/model_create.h"
#include "../ui/structure_create/structure_create.h"
#include "../ui/loader_panel/cubes_structure_panel.h"
#include "../ui/loader_panel/cubes_mipmap_structure_panel.h"
#include "../ui/loader_panel/tree_structure_panel.h"
#include "../util.h"

#include <dypc/model.h>
#include <dypc/loader.h>

#include <wx/msgdlg.h>
#include <wx/event.h>
#include <wx/app.h>
#include <wx/colour.h>
#include <wx/filedlg.h>

#include <string>
#include <chrono>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <memory>

namespace dypc {

main_frame::main_frame(wxWindow* parent, wxWindowID id) : main_frame_ui(parent, id) { }

main_frame::~main_frame() { }


void main_frame::updated_callback_(bool changed_loader) {
	wxColour color_red(230, 0, 0);
	wxColour color_yellow(213, 166, 8);
	wxColour color_green(26, 175, 35);
	
	std::size_t rom, ram, pts;
	
	get_renderer_().get_updater().access_loader([&](dypc_loader ld) {
		if(changed_loader) {
			rom = dypc_loader_rom_size(ld);
			pts = dypc_loader_number_of_points(ld);
		}
		ram = dypc_loader_memory_size(ld);
	});
	
	if(changed_loader) {
		memory_rom->SetLabel(wxString(file_size_to_string(rom).c_str(), wxConvUTF8));
		points_stat_total->SetLabel(wxString(std::to_string(pts).c_str(), wxConvUTF8));		
	}
	memory_ram->SetLabel(wxString(file_size_to_string(ram).c_str(), wxConvUTF8));

		
	points_stat_rendered->SetLabel(wxString(std::to_string(get_renderer_().get_rendered_points()).c_str(), wxConvUTF8));
	points_stat_capacity->SetLabel(wxString(std::to_string(get_renderer_().get_capacity()).c_str(), wxConvUTF8));


	auto percent = 100 * get_renderer_().get_rendered_points() / get_renderer_().get_capacity();
	std::string percent_str = std::to_string(percent) + " %";
	points_filter_percent->SetLabel(wxString(percent_str.c_str(), wxConvUTF8));
	if(percent >= 95) points_filter_percent->SetForegroundColour(color_red);
	else if(percent > 80) points_filter_percent->SetForegroundColour(color_yellow);
	else points_filter_percent->SetForegroundColour(color_green);
		
	unsigned nval = get_renderer_().get_updater().get_last_compute_duration().count();
	loader_time->SetLabel(wxString(time_to_string(get_renderer_().get_updater().get_last_compute_duration()).c_str(), wxConvUTF8));
	if(nval > 1000) loader_time->SetForegroundColour(color_red);
	else if(nval > 300) loader_time->SetForegroundColour(color_yellow);
	else loader_time->SetForegroundColour(color_green);
}


renderer& main_frame::get_renderer_() {
	return renderer_canv->get_renderer();
}


void main_frame::on_create_structure_file_(wxCommandEvent& event) {
	model_create model_dialog(this);
	int ok = model_dialog.ShowModal();
	if(! ok) return;
	
	structure_create structure_dialog(this);
	ok = structure_dialog.ShowModal();
	if(! ok) return;
	
	dypc_model mod = model_dialog.create_model();
	if(! mod) {
		wxMessageDialog(nullptr, wxT("Could not create model")).ShowModal();
		return;
	}

	structure_dialog.write_structure_file(mod);
	
	wxMessageDialog dialog(
		nullptr,
		wxT("The structure file has been generated."),
		wxT("Success"),
		wxOK
	);
	dialog.ShowModal();
}


void main_frame::on_loader_choice_(wxCommandEvent& event) {
	get_renderer_().set_callback( std::bind(&main_frame::updated_callback_, this, std::placeholders::_1) );
	
	int index = loader_choice->GetSelection();
	if(index == wxNOT_FOUND) return;

	get_renderer_().delete_loader();
	loader_panel_sizer->Clear(true);
	
	dypc_loader ld = nullptr;
	wxWindow* panel = nullptr;

	if(index == 1) {
		// Direct Model Loader
		model_create model_dialog(this);
		int ok = model_dialog.ShowModal();
		if(! ok) return;
		dypc_model mod = model_dialog.create_model();
		if(! mod) return;

		ld = dypc_create_direct_model_loader(mod);


	} else if(index == 2) {
		// Memory Structure Loader
		model_create model_dialog(this);
		int ok = model_dialog.ShowModal();
		if(! ok) return;
	
		structure_create structure_dialog(this);
		ok = structure_dialog.ShowModal();
		if(! ok) return;

		dypc_model mod = model_dialog.create_model();
		if(! mod) {
			wxMessageDialog(nullptr, wxT("Could not create model")).ShowModal();
			return;
		}
		ld = structure_dialog.create_memory_loader(mod);
		
	} else if(index == 3) {
		// File Structure Loader
		wxFileDialog open_dialog(
			nullptr,
			wxT("Open structure file"),
			wxEmptyString,
			wxEmptyString,
			wxT("*|*"),
			wxFD_OPEN | wxFD_FILE_MUST_EXIST
		);
		auto result = open_dialog.ShowModal();
		if(result == wxID_CANCEL) return;
			
		std::string filename(open_dialog.GetPath().utf8_str());		
		
		ld = dypc_create_file_structure_loader(filename.c_str(), dypc_ordered_tree_structure_loader_type);
	}
	
	if(ld) {		
		get_renderer_().switch_loader(ld);
		loader_label->SetLabel(wxString(dypc_loader_name(ld), wxConvUTF8));
		
		wxWindow* panel;
		updater& upd = get_renderer_().get_updater();
		switch(dypc_loader_loader_type(ld)) {
			case dypc_cubes_loader_type: panel = new cubes_structure_panel(upd, loader_panel); break;
			case dypc_cubes_mipmap_loader_type: panel = new cubes_mipmap_structure_panel(upd, loader_panel); break;
			case dypc_tree_loader_type: panel = new tree_structure_panel(upd, loader_panel); break;
			default: panel = nullptr;
		}
		
		if(panel) loader_panel_sizer->Add(panel, wxSizerFlags().Expand());
	} else {
		loader_label->SetLabel(wxT(""));
	}
}


void main_frame::on_loader_config_() {
	renderer& rd = get_renderer_();
	
	bool paused = loader_paused->IsChecked();
	std::chrono::milliseconds interval( loader_interval->GetValue() );
	bool check_condition = loader_check_condition->IsChecked();
	
	rd.set_updater_paused(paused);
	rd.set_updater_check_condition(check_condition);
	rd.set_updater_check_interval(interval);
	
	float setting = (float)downsampling_setting->GetValue();
	
	rd.set_loader_downsampling_setting(setting);
	//rd.set_loader_adaptive(adap);
}

void main_frame::on_loader_update_now_(wxCommandEvent& event) {
	get_renderer_().update_now();
}

void main_frame::on_renderer_config_() {	
	auto speed = renderer_speed->GetValue();
	float scale = (float)renderer_scale->GetValue()/100.0;
	auto fov = renderer_fov->GetValue();
	wxColour bg_color = renderer_background->GetColour();
	auto bg_r = bg_color.Red(); auto bg_g = bg_color.Green(); auto bg_b = bg_color.Blue();
	bool fog = fog_checkbox->IsChecked();
	auto fog_distance = fog_distance_spin->GetValue();
	bool depth_test = depth_test_checkbox->IsChecked();
	bool shadow = point_shadow_checkbox->IsChecked();
	auto shadow_size = point_shadow_radius_spin->GetValue();
	auto shadow_max_distance = point_shadow_distance_spin->GetValue();
	
	renderer& rd = get_renderer_();
	rd.set_configuration(fov, scale, bg_r, bg_g, bg_b, fog, fog_distance, depth_test, shadow, shadow_size, shadow_max_distance);

	renderer_canv->set_movement_speed(speed);
}

void main_frame::on_renderer_capacity_config_(wxCommandEvent& event) {
	wxString capacity_str = renderer_capacity->GetLineText(0);
	unsigned long capacity;
	if(capacity_str.ToULong(&capacity)) {
		renderer& rd = get_renderer_();
		rd.set_point_capacity(capacity);
	}
}

void main_frame::on_setting_points_stat_(wxCommandEvent& event) {
	get_renderer_().get_updater().access_loader([&](dypc_loader ld) {
		setting_output_statistics dialog(get_renderer_().get_updater().get_request(), ld, this);
		dialog.ShowModal();		
	});
}

void main_frame::render_to_png_(wxCommandEvent& event) {
	wxFileDialog save_dialog(
		nullptr,
		wxT("Render to PNG file"),
		wxEmptyString,
		wxEmptyString,
		wxT("PNG files (*.png)|*.png"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT
	);
	auto result = save_dialog.ShowModal();
	if(result == wxID_CANCEL) return;
		
	std::string filename(save_dialog.GetPath().utf8_str());	
	
	renderer& rd = get_renderer_();
	rd.render_to_png(filename);
}

}
