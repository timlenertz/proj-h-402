#include "ply_model_create.h"

namespace dypc {

dypc_model ply_model_create::create_model() const {
	double scale;
	scale_text->GetLineText(0).ToDouble(&scale);
	wxString path = file_filepicker->GetPath();

	return dypc_create_ply_model(path.utf8_str(), scale);
}

}
