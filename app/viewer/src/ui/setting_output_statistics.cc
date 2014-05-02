#include "setting_output_statistics.h"

namespace dypc {

void setting_output_statistics::on_ok_(wxCommandEvent& event) {
	wxString path = file_picker->GetPath();
	float min_s = 0.1 * min_setting->GetValue();
	float max_s = 0.1 * max_setting->GetValue();
	float step = 0.1 * step_setting->GetValue();
	double cap; n_min->GetLineText(0).ToDouble(&cap);

	dypc_generate_setting_output_statistics(loader_, path.utf8_str(), cap, &request_, min_s, max_s, step);
	
	Close();
	
}

void setting_output_statistics::on_cancel_(wxCommandEvent& event) {
	Close();
}

}
