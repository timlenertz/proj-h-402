#include "concentric_spheres_model_create.h"

namespace dypc {

dypc_model concentric_spheres_model_create::create_model() const {
	unsigned long count, outer, inner;
	
	wxString count_str = count_text->GetLineText(0);
	wxString outer_str = outer_text->GetLineText(0);
	wxString inner_str = inner_text->GetLineText(0);
	
	count_str.ToULong(&count);
	outer_str.ToULong(&outer);
	inner_str.ToULong(&inner);
	
	unsigned steps = steps_spin->GetValue();
	
	return dypc_create_concentric_spheres_model(count, inner, outer, steps);
}

}
