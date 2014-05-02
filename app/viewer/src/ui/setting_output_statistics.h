#ifndef DYPC_SETTING_OUTPUT_STATISTICS_H_
#define DYPC_SETTING_OUTPUT_STATISTICS_H_

#include "../../forms/setting_output_statistics.h"
#include <dypc/dypc.h>

namespace dypc {
	
class setting_output_statistics : public setting_output_statistics_ui {
private:
	dypc_loader_request request_;
	dypc_loader loader_;

protected:
	void on_ok_(wxCommandEvent& event) override;
	void on_cancel_(wxCommandEvent& event) override;

public:
	setting_output_statistics(const dypc_loader_request& req, dypc_loader ld, wxWindow* parent = nullptr, wxWindowID id = wxID_ANY) : setting_output_statistics_ui(parent, id), loader_(ld), request_(req) { }
};

}

#endif
