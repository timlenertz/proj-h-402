#include "application.h"
#include "main_frame.h"
#include "../renderer/renderer.h"
#include "../util.h"
#include <stdexcept>

#include <dypc/dypc.h>

namespace dypc {
	
	
void application::error_message_(const char* title, const char* msg) {
	error_message(title, msg);
}


bool application::OnInit() {
	dypc_callbacks* callbacks = dypc_get_callbacks();
	callbacks->error_message = &error_message_;
	
	main_frame* frame = new main_frame();
	frame->Show();
	SetTopWindow(frame);
	
	return true;
}

int application::OnRun() {
	try {
		return wxApp::OnRun();
	} catch(const std::exception& ex) {
		error_message_(ex.what(), "An unhandled exception occured");
		return 0;
	}
}

}


IMPLEMENT_APP(dypc::application);
