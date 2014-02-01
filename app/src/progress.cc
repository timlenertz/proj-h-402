#include "progress.h"
#include <wx/progdlg.h>
#include <stdexcept>

static wxProgressDialog* dialog = nullptr;
static int update_step = 0;
static int maximal_value = 0;
static int current_value = 0;

namespace dypc {

void set_progress(int value) {
	if(! dialog) return;
	
	if(maximal_value) {
		int old_steps = current_value / update_step;
		current_value = value;
		if(current_value > maximal_value) current_value = maximal_value;
		if(current_value / update_step == old_steps) return;
		dialog->Update(current_value);
	} else {
		dialog->Pulse();
	}
}

void increment_progress(int add) {
	set_progress(current_value + add);
}

void progress(const std::string& label, int maximum, int step, const std::function<void()>& callback) {
	if(dialog) throw std::logic_error("Cannot nest progress");
	
	if(maximum > 0) {
		dialog = new wxProgressDialog(
			wxT("Progress"),
			wxString(label.c_str(), wxConvUTF8),
			maximum,
			nullptr,
			wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME
		);
	
		current_value = 0;
		update_step = (step >= 1 ? step : 1);
		maximal_value = maximum;
		dialog->Update(0);

	} else {
		dialog = new wxProgressDialog(
			wxT("Progress"),
			wxString(label.c_str(), wxConvUTF8),
			1,
			nullptr,
			wxPD_APP_MODAL | wxPD_SMOOTH
		);
	
		current_value = 0;
		update_step = 0;
		maximal_value = 0;
		dialog->Pulse();

	}

	callback();

	dialog->Destroy();
	dialog = nullptr;
}

}
