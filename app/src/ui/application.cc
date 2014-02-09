#include "application.h"
#include "main_frame.h"
#include "../renderer/renderer.h"
#include "../util.h"
#include <stdexcept>

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
		error_message(ex.what(), "An unhandled exception occured");
		return 0;
	}
}

}


IMPLEMENT_APP(dypc::application);
