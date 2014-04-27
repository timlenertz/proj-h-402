#ifndef DYPC_UI_APPLICATION_H_
#define DYPC_UI_APPLICATION_H_

#include <wx/wx.h>
#include <map>

namespace dypc {

class application : public wxApp {
public:
	bool OnInit() override;
	int OnRun() override;
};

}

#endif
