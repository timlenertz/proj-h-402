#ifndef DYPC_UI_STRUCTURE_CREATE_H_
#define DYPC_UI_STRUCTURE_CREATE_H_

#include "../../../forms/structure_create.h"
#include "structure_create_panel.h"
#include <string>
#include <utility>
#include <dypc/loader.h>
#include <dypc/model.h>
#include <dypc/types.h>

namespace dypc {
	
class structure_create : public structure_create_ui {
private:
	structure_create_panel* panel_ = nullptr;

	template<class Panel>
	void set_panel_(Panel* panel) {
		structure_panel_sizer->Clear(true);
		panel_ = panel;
		structure_panel_sizer->Add(panel, wxSizerFlags().Expand());
	}

protected:
	void on_structure_choice_(wxCommandEvent& event) override;
	void on_ok_(wxCommandEvent& event) override;
	void on_cancel_(wxCommandEvent& event) override;

public:
	structure_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY);
	
	dypc_loader create_memory_loader(dypc_model) const;
	std::string write_structure_file(dypc_model) const;
};

}

#endif
