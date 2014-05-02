#ifndef DYPC_STRUCTURE_H_
#define DYPC_STRUCTURE_H_

#include "../model/model.h"

namespace dypc {

/**
 * Structure which stores points from model.
 * Stored reference to model
 */
class structure {
protected:
	model& model_; ///< The model whose points this structure contains.

	explicit structure(model& mod) : model_(mod) { } ///< Create structure for given model.
	
	structure(const structure&) = delete;

public:
	virtual ~structure() { }
	
	std::size_t total_number_of_points() const { return model_.number_of_points(); } ///< Get total number of points in model/structure.
};

}

#endif
