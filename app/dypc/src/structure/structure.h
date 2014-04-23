#ifndef DYPC_STRUCTURE_H_
#define DYPC_STRUCTURE_H_

#include "../model/model.h"

namespace dypc {


class structure {
protected:
	model& model_;

	explicit structure(model& mod) : model_(mod) { }
	
	structure(const structure&) = delete;

public:
	virtual ~structure() { }
	
	std::size_t total_number_of_points() const { return model_.number_of_points(); }
};

}

#endif
