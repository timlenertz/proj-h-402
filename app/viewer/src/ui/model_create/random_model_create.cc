#include "random_model_create.h"

namespace dypc {

dypc_model random_model_create::create_model() const {
	unsigned long count, side;
	
	wxString count_str = count_text->GetLineText(0);
	wxString side_str = side_text->GetLineText(0);
	
	count_str.ToULong(&count);
	side_str.ToULong(&side);
	
	return dypc_create_random_model(count, side);
}

}
