#ifndef DYPC_UI_RENDERER_CANVAS_H_
#define DYPC_UI_RENDERER_CANVAS_H_

#include "../gl.h" // GLEW must be included first (before glcanvas)
#include <wx/wx.h>
#include <wx/glcanvas.h>

#include <chrono>

#include "../renderer/renderer.h"


namespace dypc {

class renderer_canvas : public wxGLCanvas {
private:
	class render_timer : public wxTimer {
	private:
		renderer_canvas& canvas_;
	public:
		render_timer(renderer_canvas& can) : wxTimer(), canvas_(can) { }
		void Notify() { canvas_.draw_now_(); }
	};

	static constexpr int* attrib_list_ = nullptr;
	
	wxStatusBar* status_bar_;
	
	wxGLContext context_;
	renderer* renderer_;
	
	render_timer render_timer_;
	typedef std::chrono::high_resolution_clock clock_t_;
	clock_t_::time_point last_time_;

	float speed_ = 30.0;

	struct movement_t {
		bool up:1;
		bool down:1;
		bool left:1;
		bool right:1;
		bool forward:1;
		bool backward:1;
		movement_t() :
		up(false), down(false), left(false), right(false), forward(false), backward(false) { }
	} movement_;
	
	wxPoint drag_position_;
	
	void on_paint_(wxPaintEvent&);
	void on_mouse_motion_(wxMouseEvent&);
	void on_mouse_wheel_(wxMouseEvent&);
	void on_mouse_down_(wxMouseEvent&);
	void on_mouse_up_(wxMouseEvent&);
	void on_key_down_(wxKeyEvent&);
	void on_key_up_(wxKeyEvent&);
	
	void handle_key_(int code, bool down);
	
	void initialize_renderer_();
	void draw_now_();
	
public:
	renderer_canvas(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("renderer_canvas"));
	~renderer_canvas();
	
	renderer& get_renderer();
	
	void set_status_bar(wxStatusBar* bar) { status_bar_ = bar; }
	void set_movement_speed(float s) { speed_ = s; }
};

}

#endif
