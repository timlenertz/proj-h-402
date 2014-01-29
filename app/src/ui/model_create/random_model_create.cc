#include "random_model_create.h"
#include "../../loader/direct_model_loader.h"

namespace dypc {

random_model* random_model_create::create_model() const {
	unsigned long count, side;
	
	wxString count_str = count_text->GetLineText(0);
	wxString side_str = side_text->GetLineText(0);
	
	count_str.ToULong(&count);
	side_str.ToULong(&side);
	
	return new random_model(count, side);
}

loader* random_model_create::create_direct_model_loader() const {
	return new direct_model_loader<random_model>(* create_model());
}

}
