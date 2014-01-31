#include "cubes_structure_loader.h"
#include "../../ui/cubes_structure_panel.h"

namespace dypc {

::wxWindow* cubes_structure_loader::create_panel(::wxWindow* parent) {
	cubes_structure_panel* panel = new cubes_structure_panel(parent);
	panel->set_loader(*this);
	return panel;
}


}
