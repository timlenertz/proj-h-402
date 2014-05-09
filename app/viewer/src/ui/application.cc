#include "application.h"
#include "main_frame.h"
#include "../renderer/renderer.h"
#include "../util.h"
#include <stdexcept>
#include <wx/progdlg.h>
#include <dypc/progress.h>

namespace dypc {
	
	
static dypc_progress progress_open_(const char* label, unsigned maximum, dypc_progress par) {
	wxProgressDialog* dialog = new wxProgressDialog(
		wxT("Progress"),
		wxString(label, wxConvUTF8),
		maximum,
		nullptr,
		wxPD_APP_MODAL | wxPD_AUTO_HIDE
	);
	return (dypc_progress)dialog;
}

static void progress_set_(dypc_progress pr, unsigned value) {
	wxProgressDialog* dialog = (wxProgressDialog*)pr;
	dialog->Update(value);
}

static void progress_pulse_(dypc_progress pr) {
	wxProgressDialog* dialog = (wxProgressDialog*)pr;
	dialog->Pulse();
}

static void progress_message_(dypc_progress pr, const char* msg) {
	wxProgressDialog* dialog = (wxProgressDialog*)pr;
}

static void progress_close_(dypc_progress pr) {
	wxProgressDialog* dialog = (wxProgressDialog*)pr;
	delete dialog;
}



bool application::OnInit() {
	dypc_progress_callbacks progress_callbacks = {
		&progress_open_,
		&progress_close_,
		&progress_set_,
		&progress_pulse_,
		&progress_message_
	};
	dypc_set_progress_callbacks(&progress_callbacks);
	
	main_frame* frame = new main_frame();
	frame->Show();
	SetTopWindow(frame);
	
	return true;
}

int application::OnRun() {
	try {
		return wxApp::OnRun();
	} catch(const std::exception& ex) {
		error_message(ex.what(), "An unhandled exception occured");
		return 0;
	}
}

}


IMPLEMENT_APP(dypc::application);
