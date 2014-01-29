#ifndef DYPC_CUBES_STRUCTURE_SQLITE_LOADER_H_
#define DYPC_CUBES_STRUCTURE_SQLITE_LOADER_H_

#include <string>
#include <vector>
#include <utility>
#include "cubes_structure_loader.h"
#include "../../sqlite/sqlite_database.h"
#include "../../point.h"
#include "../../cuboid.h"
#include "../../loader/loader.h"

namespace dypc {
	
class cubes_structure;

class cubes_structure_sqlite_loader : public cubes_structure_loader {	
private:
	class cube_entry {
	public:
		sqlite_database::id_t id;
		std::size_t number_of_points;
		cuboid cube;
		
		cube_entry(id_t nid, std::size_t npts, const cuboid& c) :
			id(nid), number_of_points(npts), cube(c) { }
	};

	sqlite_database database_;
	std::vector<cube_entry> cube_entries_;

	static void create_tables_(sqlite_database&);
	
public:
	static void write(const std::string& file, const cubes_structure&);
	
	explicit cubes_structure_sqlite_loader(const std::string& file);
	
	std::string loader_name() const override { return "Cubes Structure SQLite Loader"; }

protected:
	std::size_t extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t&);
	std::size_t memory_size_() const override;
	std::size_t file_size_() const override;
	std::size_t total_points_() const override;
};

}

#endif
