#ifndef DYPC_UI_MAIN_FRAME_H_
#define DYPC_UI_MAIN_FRAME_H_

#include <map>
#include "../../res/main_frame.h"
#include "../statistics.h"

DECLARE_EVENT_TYPE(DYPC_STATISTICS_UPDATED_EVENT, -1)

namespace dypc {
	
class renderer;
class model;

class main_frame : public main_frame_ui {
private:
	std::map<const statistics::element_t*, long> statistics_list_items_;
	std::size_t renderer_points_stat_ = 0;
	std::size_t renderer_capacity_stat_ = 0;	

	renderer& get_renderer_();

	void on_renderer_config_();
	void on_loader_config_();
	
	void statistics_table_update_(const statistics::table_t&);
	void statistics_item_update_(const statistics::table_t&, const statistics::element_t&);
	
	void on_statistics_updated_(wxCommandEvent& event);
	void special_statistic_updated_(const statistics::value&);

protected:
	void on_create_structure_file_(wxCommandEvent& event) override;
	void on_loader_choice_(wxCommandEvent& event) override;
	void on_renderer_config_(wxCommandEvent& event) override { on_renderer_config_(); }
	void on_renderer_config_(wxSpinEvent& event) override { on_renderer_config_(); }
	void on_renderer_config_(wxColourPickerEvent& event) override { on_renderer_config_(); }
	void on_renderer_capacity_config_(wxCommandEvent& event) override;
	void on_loader_config_(wxSpinEvent& event) override { on_loader_config_(); }
	void on_loader_config_(wxCommandEvent& event) override { on_loader_config_(); }
	void on_loader_update_now_(wxCommandEvent& event) override;

public:
	main_frame(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY);
	~main_frame();
};


}

#endif
