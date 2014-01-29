#include "ply_model_create.h"
#include "../../loader/direct_model_loader.h"

namespace dypc {

ply_model* ply_model_create::create_model() const {
	double scale;
	scale_text->GetLineText(0).ToDouble(&scale);
	wxString path = file_filepicker->GetPath();

	return new ply_model(std::string(path.utf8_str()), scale);
}

loader* ply_model_create::create_direct_model_loader() const {
	return new direct_model_loader<ply_model>(* create_model());
}


}
