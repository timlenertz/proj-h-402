#include "progress.h"
#include <wx/progdlg.h>
#include <stdexcept>

static wxProgressDialog* dialog = nullptr;
static int update_step = 0;
static int current_value = 0;

namespace dypc {

void set_progress(int value) {
	if(! dialog) return;
	current_value = value;
	if(value % update_step) return;
	dialog->Update(value);
}

void increment_progress(int add) {
	if(! dialog) return;
	current_value += add;
	if(current_value % update_step) return;
	dialog->Update(current_value);

}

void progress(const std::string& label, int maximum, int s, const std::function<void()>& callback) {
	if(dialog) throw std::logic_error("Cannot nest progress");
	
	dialog = new wxProgressDialog(
		wxT("Progress"),
		wxString(label.c_str(), wxConvUTF8),
		maximum,
		nullptr,
		wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME
	);
	
	update_step = s;
	dialog->Update(0);

	callback();

	dialog->Destroy();
	dialog = nullptr;
}

}
