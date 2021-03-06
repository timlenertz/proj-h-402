#ifndef DYPC_UI_MAIN_FRAME_H_
#define DYPC_UI_MAIN_FRAME_H_

#include <map>
#include "../../forms/main_frame.h"

namespace dypc {
	
class renderer;
class model;

class main_frame : public main_frame_ui {
private:
	std::size_t renderer_points_stat_ = 0;
	std::size_t renderer_capacity_stat_ = 0;	

	renderer& get_renderer_();

	void on_renderer_config_();
	void on_loader_config_();
	
	void updated_callback_(bool changed_loader);
		
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
	void on_setting_points_stat_(wxCommandEvent& event) override;
	void render_to_png_(wxCommandEvent& event) override;

public:
	main_frame(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY);
	~main_frame();
};


}

#endif
