#ifndef DYPC_UI_APPLICATION_H_
#define DYPC_UI_APPLICATION_H_

#include <wx/wx.h>

namespace dypc {

class application : public wxApp {
private:
	static void error_message_(const char* title, const char* msg);
	static int user_choice_(const char* title, const char** choices);

public:
	bool OnInit() override;
	int OnRun() override;
};

}

#endif
