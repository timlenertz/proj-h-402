#include "renderer_canvas.h"

#include <stdexcept>
#include <iostream>

namespace dypc {

renderer_canvas::renderer_canvas(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
wxGLCanvas(parent, id, attrib_list_, pos, size, style, name), status_bar_(nullptr),
context_(this), renderer_(nullptr), render_timer_(*this)
{
	Connect(wxEVT_PAINT, wxPaintEventHandler(renderer_canvas::on_paint_));
	
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(renderer_canvas::on_mouse_down_));
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(renderer_canvas::on_mouse_up_));
	Connect(wxEVT_MOTION, wxMouseEventHandler(renderer_canvas::on_mouse_motion_));
	Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(renderer_canvas::on_mouse_wheel_));
	
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(renderer_canvas::on_key_down_));
	Connect(wxEVT_KEY_UP, wxKeyEventHandler(renderer_canvas::on_key_up_));	
}

renderer_canvas::~renderer_canvas() {
	render_timer_.Stop();
	if(renderer_) delete renderer_;
}

void renderer_canvas::initialize_renderer_() {
	if(renderer_) return;

	renderer_ = new renderer(GetSize().x, GetSize().y);
	render_timer_.Start(10, wxTIMER_CONTINUOUS);
}

renderer& renderer_canvas::get_renderer() {
	if(! renderer_) throw std::runtime_error("Renderer not initialized.");
	else return *renderer_;
}

void renderer_canvas::draw_now_() {
	Refresh(); Update();
	
	auto pos = renderer_->get_position();
	auto vel = renderer_->get_velocity();
	
	if(status_bar_) status_bar_->SetStatusText(wxString::Format(wxT("Position: (%.2f, %.2f, %.2f)"), (double)pos[0], (double)pos[1], (double)pos[2]), 0);
	if(status_bar_) status_bar_->SetStatusText(wxString::Format(wxT("Velocity: (%.2f, %.2f, %.2f)"), (double)vel[0], (double)vel[1], (double)vel[2]), 1);
}

void renderer_canvas::handle_key_(int code, bool pressed) {	
	if(! renderer_) return;
	
	switch(code) {
		case 'Z': movement_.forward = pressed; break;
		case 'S': movement_.backward = pressed; break;
		case 'Q': movement_.left = pressed; break;
		case 'D': movement_.right = pressed; break;
		case ' ': movement_.up = pressed; break;
		case '<': movement_.down = pressed; break;
		default: return;
	}
		
	float x = 0.0, y = 0.0, z = 0.0;
	if(movement_.forward) z += speed_;
	if(movement_.backward) z -= speed_;
	if(movement_.left) x += speed_;
	if(movement_.right) x -= speed_;
	if(movement_.up) y += speed_;
	if(movement_.down) y -= speed_;
	
	renderer_->set_view_target_velocity(x, -y, z);
}

void renderer_canvas::on_paint_(wxPaintEvent&) {
	wxPaintDC(this);
	
	float dtime = 0.0;
	auto now = clock_t_::now();
	if(last_time_ > clock_t_::time_point()) dtime = (float)std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time_).count() / 1000.0;
	last_time_ = now;
	
	SetCurrent(context_);
	try {
		if(! renderer_) initialize_renderer_();
		renderer_->draw(dtime);
	} catch(const std::exception& ex) {
		std::cerr << "Exception in renderer: " << ex.what() << std::endl;
	}
    SwapBuffers();
}

void renderer_canvas::on_mouse_down_(wxMouseEvent& event) {
	drag_position_ = event.GetPosition();
	event.Skip();
}

void renderer_canvas::on_mouse_up_(wxMouseEvent& event) {
	event.Skip();
}

void renderer_canvas::on_mouse_motion_(wxMouseEvent& event) {
	if(! event.Dragging()) { return; event.Skip(); }
		
	wxPoint pos = event.GetPosition();
	
	float diff_x = pos.x - drag_position_.x;
	float diff_y = pos.y - drag_position_.y;
	
	float factor = M_PI / 500.0;
	
	renderer_->rotate_camera(diff_x * factor, diff_y * factor);
	
	drag_position_ = event.GetPosition();
}

void renderer_canvas::on_mouse_wheel_(wxMouseEvent& event) {
	float step_angle = M_PI / 20.0;
	float roll = step_angle * (float)event.GetWheelRotation() / event.GetWheelDelta();
	renderer_->rotate_camera(0, 0, roll);
}

void renderer_canvas::on_key_down_(wxKeyEvent& event) {
	handle_key_(event.GetKeyCode(), true);
}

void renderer_canvas::on_key_up_(wxKeyEvent& event) {
	handle_key_(event.GetKeyCode(), false);
}

}
