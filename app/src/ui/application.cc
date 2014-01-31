#include "application.h"
#include "main_frame.h"
#include "../renderer/renderer.h"

#include <iostream>
#include <stdexcept>
#include <wx/msgdlg.h>

namespace dypc {

bool application::OnInit() {	
	main_frame* frame = new main_frame();
	frame->Show();
	SetTopWindow(frame);
	
	return true;
}

int application::OnRun() {
	try {
		return wxApp::OnRun();
	} catch(const std::exception& ex) {
		wxMessageDialog dialog(
			nullptr,
			wxString(ex.what(), wxConvUTF8),
			wxT("An unhandled exception occured"),
			wxOK | wxICON_ERROR
		);
		dialog.ShowModal();
		return 0;
	}
}

}


IMPLEMENT_APP(dypc::application);
