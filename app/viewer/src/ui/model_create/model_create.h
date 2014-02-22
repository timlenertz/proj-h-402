#ifndef DYPC_UI_MODEL_CREATE_H_
#define DYPC_UI_MODEL_CREATE_H_

#include "../../../forms/model_create.h"
#include "model_create_panel.h"
#include <dypc/model.h>

namespace dypc {
	
class loader;

class model_create : public model_create_ui {
private:
	model_create_panel* panel_ = nullptr;

	template<class Panel>
	void set_panel_(Panel* panel) {
		model_panel_sizer->Clear(true);
		panel_ = panel;
		model_panel_sizer->Add(panel, wxSizerFlags().Expand());
	}

protected:
	void on_model_choice_(wxCommandEvent& event) override;
	void on_ok_(wxCommandEvent& event) override;
	void on_cancel_(wxCommandEvent& event) override;

public:
	model_create(wxWindow* parent = nullptr, wxWindowID id = wxID_ANY);
	
	dypc_model create_model() const;
};

}

#endif
