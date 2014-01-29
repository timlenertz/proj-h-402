#include "concentric_spheres_model_create.h"
#include "../../loader/direct_model_loader.h"

namespace dypc {

concentric_spheres_model* concentric_spheres_model_create::create_model() const {
	unsigned long count, outer, inner;
	
	wxString count_str = count_text->GetLineText(0);
	wxString outer_str = outer_text->GetLineText(0);
	wxString inner_str = inner_text->GetLineText(0);
	
	count_str.ToULong(&count);
	outer_str.ToULong(&outer);
	inner_str.ToULong(&inner);
	
	unsigned steps = steps_spin->GetValue();
	
	return new concentric_spheres_model(count, inner, outer, steps);
}

loader* concentric_spheres_model_create::create_direct_model_loader() const {
	return new direct_model_loader<concentric_spheres_model>(* create_model());
}

}
