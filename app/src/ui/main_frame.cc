#include "main_frame.h"

#include "../renderer/renderer.h"
#include "renderer_canvas.h"
#include "../model/model.h"
#include "../structure/structure_loader.h"
#include "../loader/loader.h"

#include "../ui/model_create/model_create.h"
#include "../ui/structure_create/structure_create.h"
#include "../util.h"

#include <wx/msgdlg.h>
#include <wx/event.h>
#include <wx/app.h>
#include <wx/colour.h>

#include <string>
#include <chrono>
#include <utility>
#include <iostream>
#include <stdexcept>
#include <memory>

DEFINE_EVENT_TYPE(DYPC_STATISTICS_UPDATED_EVENT)

namespace dypc {

main_frame::main_frame(wxWindow* parent, wxWindowID id) :
main_frame_ui(parent, id) {
	using namespace std::placeholders;
	statistics::set_callbacks(
		std::bind(&main_frame::statistics_table_update_, this, _1),
		std::bind(&main_frame::statistics_item_update_, this, _1, _2)
	);
	
	Connect(DYPC_STATISTICS_UPDATED_EVENT, wxCommandEventHandler(main_frame::on_statistics_updated_), NULL, this);
}

main_frame::~main_frame() {
	Disconnect(DYPC_STATISTICS_UPDATED_EVENT, wxCommandEventHandler(main_frame::on_statistics_updated_), NULL, this);
}

void main_frame::statistics_table_update_(const statistics::table_t& table) {
	wxCommandEvent ev(DYPC_STATISTICS_UPDATED_EVENT, wxID_ANY);
	ev.SetClientData(nullptr);
	AddPendingEvent(ev);
}

void main_frame::statistics_item_update_(const statistics::table_t& table, const statistics::element_t& el) {
	wxCommandEvent ev(DYPC_STATISTICS_UPDATED_EVENT, wxID_ANY);
	ev.SetClientData((void*)&el);
	AddPendingEvent(ev);
}


void main_frame::special_statistic_updated_(const statistics::value& val) {	
	wxColour color_red(230, 0, 0);
	wxColour color_yellow(213, 166, 8);
	wxColour color_green(26, 175, 35);
		
	long nval = val;
	bool update_filter_points = false;
	
	switch(val.get_kind()) {
	case statistics::loader_time:
		loader_time->SetLabel(wxString(val.to_string().c_str(), wxConvUTF8));
		if(nval > 1000) loader_time->SetForegroundColour(color_red);
		else if(nval > 300) loader_time->SetForegroundColour(color_yellow);
		else loader_time->SetForegroundColour(color_green);
		break;
		
	case statistics::rom_size:
		memory_rom->SetLabel(wxString(val.to_string().c_str(), wxConvUTF8));
		break;

	case statistics::memory_size:
		memory_ram->SetLabel(wxString(val.to_string().c_str(), wxConvUTF8));
		break;
		
	case statistics::model_total_points:
		points_stat_total->SetLabel(wxString(val.to_string().c_str(), wxConvUTF8));
		break;

	case statistics::rendered_points:
		renderer_points_stat_ = nval;
		points_stat_rendered->SetLabel(wxString(val.to_string().c_str(), wxConvUTF8));
		update_filter_points = true;
		break;
		
	case statistics::renderer_capacity:
		renderer_capacity_stat_ = nval;
		points_stat_capacity->SetLabel(wxString(val.to_string().c_str(), wxConvUTF8));
		update_filter_points = true;
		break;
		
	default: break;
	}

	if(update_filter_points && renderer_points_stat_ && renderer_capacity_stat_) {
		auto percent = 100 * renderer_points_stat_ / renderer_capacity_stat_;
		std::string percent_str = std::to_string(percent) + " %";
		points_filter_percent->SetLabel(wxString(percent_str.c_str(), wxConvUTF8));
		if(percent >= 95) points_filter_percent->SetForegroundColour(color_red);
		else if(percent > 80) points_filter_percent->SetForegroundColour(color_yellow);
		else points_filter_percent->SetForegroundColour(color_green);
	}
}


void main_frame::on_statistics_updated_(wxCommandEvent& event) {	
	void* client_data = event.GetClientData();
	const auto& table = statistics::table();

	if(client_data == nullptr) {
		statistics_list_items_.clear();
	
		statistics_list->ClearAll();
		statistics_list->InsertColumn(0, wxT("Name"), wxLIST_FORMAT_LEFT, 130);
		statistics_list->InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 60);
		long i = 0;
		for(const statistics::element_t& el : table) {
			const std::string& name = el.first;
			const statistics::value val = el.second;
			
			wxString name_str(wxString(name.c_str(), wxConvUTF8));
			statistics_list->InsertItem(i, name_str);
			statistics_list->SetItem(i, 1, wxString(val.to_string().c_str(), wxConvUTF8));
			
			if(val.get_kind()) special_statistic_updated_(val);
			
			statistics_list_items_[&el] = i++;
		}
	} else {
		const statistics::element_t* el = (const statistics::element_t*)client_data;
		auto it = statistics_list_items_.find(el);
		if(it != statistics_list_items_.end()) {
			auto i = it->second;
			const auto& val = it->first->second;
			statistics_list->SetItem(i, 1, wxString(val.to_string().c_str(), wxConvUTF8));
			
			if(val.get_kind()) special_statistic_updated_(val);
		}
	}
	
	short_statistics_sizer->RecalcSizes();
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
	
	std::unique_ptr<model> mod(model_dialog.create_model());
	if(! mod) {
		wxMessageDialog(nullptr, wxT("Could not create model")).ShowModal();
		return;
	}

	structure_dialog.write_structure_file(*mod);
}


void main_frame::on_loader_choice_(wxCommandEvent& event) {
	int index = loader_choice->GetSelection();
	if(index == wxNOT_FOUND) return;

	get_renderer_().delete_loader();
	loader_panel_sizer->Clear(true);
	
	loader* ld = nullptr;
	wxWindow* panel = nullptr;

	if(index == 1) {
		// Direct Model Loader
		model_create model_dialog(this);
		int ok = model_dialog.ShowModal();
		if(! ok) return;

		ld = model_dialog.create_direct_model_loader();

	} else if(index == 2) {
		// Memory Structure Loader
		model_create model_dialog(this);
		int ok = model_dialog.ShowModal();
		if(! ok) return;
	
		structure_create structure_dialog(this);
		ok = structure_dialog.ShowModal();
		if(! ok) return;

		std::unique_ptr<model> mod(model_dialog.create_model());
		if(! mod) {
			wxMessageDialog(nullptr, wxT("Could not create model")).ShowModal();
			return;
		}
		ld = structure_dialog.create_memory_loader(*mod);
		
	} else if(index == 3) {
		// File Structure Loader
		ld = structure_create::create_file_loader();
	}
	
	if(ld) {
		get_renderer_().switch_loader(ld);
		loader_label->SetLabel(wxString(ld->loader_name().c_str(), wxConvUTF8));
		wxWindow* panel = ld->create_panel(loader_panel);
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
	rd.set_loader_configuration(paused, interval, check_condition);
	
	loader_force_update->Enable(paused);
}

void main_frame::on_loader_update_now_(wxCommandEvent& event) {
	get_renderer_().update_loader_now();
}

void main_frame::on_renderer_config_() {	
	auto speed = renderer_speed->GetValue();
	float scale = (float)renderer_scale->GetValue()/100.0;
	auto fov = renderer_fov->GetValue();
	wxColour bg_color = renderer_background->GetColour();
	auto bg_r = bg_color.Red(); auto bg_g = bg_color.Green(); auto bg_b = bg_color.Blue();
	
	renderer& rd = get_renderer_();
	rd.set_configuration(fov, scale, bg_r, bg_g, bg_b);

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

}
