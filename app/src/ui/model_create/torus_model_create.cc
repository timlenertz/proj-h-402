#include "torus_model_create.h"
#include "../../loader/direct_model_loader.h"

namespace dypc {

torus_model* torus_model_create::create_model() const {
	unsigned long count, r0, r1;
	
	wxString count_str = count_text->GetLineText(0);
	wxString r0_str = r0_text->GetLineText(0);
	wxString r1_str = r1_text->GetLineText(0);
	
	count_str.ToULong(&count);
	r0_str.ToULong(&r0);
	r1_str.ToULong(&r1);
	
	return new torus_model(count, r0, r1);
}

loader* torus_model_create::create_direct_model_loader() const {
	return new direct_model_loader<torus_model>(* create_model());
}

}
