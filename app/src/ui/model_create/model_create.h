#ifndef DYPC_UI_MODEL_CREATE_H_
#define DYPC_UI_MODEL_CREATE_H_

#include "../../../res/model_create.h"
#include "model_create_panel.h"

namespace dypc {
	
class model;
class loader;

class model_create : public model_create_ui {
private:
	model* model_ = nullptr;
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
	
	model* create_model() const;
	loader* create_direct_model_loader() const;
};

}

#endif
