#ifndef DYPC_UI_STRUCTURE_CREATE_PANEL_H_
#define DYPC_UI_STRUCTURE_CREATE_PANEL_H_

#include <string>
#include <map>
#include "../../structure/structure_loader.h"
#include "../../util.h"

namespace dypc {

class model;
class loader;

class structure_create_panel {
public:
	virtual loader* create_memory_loader(model&) const = 0;
	virtual void write_structure_file(model& mod, const std::string& filename, const std::string& format) const = 0;

	static user_choices_t available_file_formats() { throw std::logic_error("Not implemented"); };
	static loader* create_file_loader(const std::string& filename, const std::string& format) { throw std::logic_error("Not implemented"); }
	
	virtual user_choices_t instance_available_file_formats() const = 0;
	virtual loader* instance_create_file_loader(const std::string& filename, const std::string& format) const = 0;
};

}

#endif
