#ifndef DYPC_STRUCTURE_H_
#define DYPC_STRUCTURE_H_

namespace dypc {

/**
 * Abstract base class for structure.
 * A structure hold's the model's points in some structured form, and can be written to a file in some format. Parts of the model can be extracted either from the in-memory structure object or the file by the corresponding structure loader.
 */
class structure {
public:
	virtual ~structure() { }
};

}

#endif
