#include "loader.h"

namespace dypc {

loader::~loader() { }

std::string loader::loader_name() const {
	return "Unnamed loader";
}
/*
::wxWindow* loader::create_panel(::wxWindow* parent) {
	return nullptr;
}*/

}
