#include "torus_model_create.h"

namespace dypc {

dypc_model torus_model_create::create_model() const {
	unsigned long count, r0, r1;
	
	wxString count_str = count_text->GetLineText(0);
	wxString r0_str = r0_text->GetLineText(0);
	wxString r1_str = r1_text->GetLineText(0);
	
	count_str.ToULong(&count);
	r0_str.ToULong(&r0);
	r1_str.ToULong(&r1);
	
	return dypc_create_torus_model(count, r0, r1);
}

}
