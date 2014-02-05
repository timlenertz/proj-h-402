#include "hpr_panel.h"
#include "../hpr/hpr_loader.h"

namespace dypc {

void hpr_panel::update_() {
	if(! loader_) return;
	loader_->set_sphere_radius( radius_spin->GetValue() );
}

void hpr_panel::trigger_() {
	if(loader_) loader_->trigger();
}

void hpr_panel::underlying_loader_settings_() {

}

}
