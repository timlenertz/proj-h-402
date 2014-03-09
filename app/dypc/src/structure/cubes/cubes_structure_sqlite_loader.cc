#include "cubes_structure_sqlite_loader.h"
#include "cubes_structure.h"
#include "../../progress.h"
#include <utility>

namespace dypc {
	
void cubes_structure_sqlite_loader::write(const std::string& filename, const cubes_structure& s) {
	sqlite_database database(filename);
	create_tables_(database);
		
	auto insert_cube = database.prepare("INSERT INTO cubes (index_x, index_y, index_z, number_of_points) VALUES (?, ?, ?, ?)");
	auto insert_point = database.prepare("INSERT INTO points (cube_id, x, y, z, r, g, b, weight) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	auto insert_config = database.prepare("INSERT INTO config (side_length) VALUES (?)");
	
	insert_config(s.get_side_length());
	
	database.transaction([&]() -> bool {
		progress_foreach(s.cubes(), "Writing Cubes Structure to SQLite...", [&](const cubes_structure::cubes_t::value_type& p) {
			const auto& idx = p.first;
			const cubes_structure::cube& cube = p.second;
			insert_cube(std::get<0>(idx), std::get<1>(idx), std::get<2>(idx), cube.number_of_points());
			auto cube_id = database.last_insert_id();
			
			for(const auto& pt : cube.weighted_points()) {					
				insert_point(cube_id, pt.x, pt.y, pt.z, pt.r, pt.g, pt.b, pt.weight);
			}
		});
		return true;
	});
}
	
cubes_structure_sqlite_loader::cubes_structure_sqlite_loader(const std::string& filename) : database_(filename) {
	auto select_config = database_.select("SELECT side_length FROM config LIMIT 1");
	select_config.next();
	float side_length = select_config.current_row()[0].float_value();
	
	auto select_cubes = database_.select("SELECT id, index_x, index_y, index_z, number_of_points FROM cubes");
	for(auto r : select_cubes) {
		sqlite_database::id_t id = r[0].id_value();
		std::ptrdiff_t index_x = r[1].int_value();
		std::ptrdiff_t index_y = r[2].int_value();
		std::ptrdiff_t index_z = r[3].int_value();
		std::size_t number_of_points = r[4].int_value();
		cube_entries_.emplace_back(
			id,
			number_of_points,
			cube_from_index_(std::make_tuple(index_x, index_y, index_z), side_length)
		);
	}
}

	
void cubes_structure_sqlite_loader::create_tables_(sqlite_database& database) {
	database.execute("DROP TABLE IF EXISTS cubes");
	database.execute("DROP TABLE IF EXISTS points");
	database.execute("DROP TABLE IF EXISTS config");
	
	database.execute(
		"CREATE TABLE cubes ("
			"id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"index_x INTEGER NOT NULL, "
			"index_y INTEGER NOT NULL, "
			"index_z INTEGER NOT NULL, "
			"number_of_points INTEGER NOT NULL"
		")"
	);
	
	database.execute(
		"CREATE TABLE points ("
			"cube_id INTEGER NOT NULL REFERENCES cubes (id), "
			"x REAL NOT NULL, "
			"y REAL NOT NULL, "
			"z REAL NOT NULL, "
			"r INTEGER NOT NULL, "
			"g INTEGER NOT NULL, "
			"b INTEGER NOT NULL, "
			"weight REAL NOT NULL"
		")"
	);
	
	database.execute(
		"CREATE TABLE config ("
			"side_length REAL NOT NULL"
		")"
	);
	
	database.execute(
		"CREATE INDEX points_cube_index ON points (cube_id)"
	);
}

std::size_t cubes_structure_sqlite_loader::extract_points_(point_buffer_t points, std::size_t capacity, const loader::request_t& req) {
	std::size_t frustum_cubes = 0, downsampled_cubes = 0;	
	std::size_t remaining = capacity;
	std::size_t total = 0;
	point_buffer_t buf = points;

	auto select_cube_points = database_.select("SELECT x, y, z, r, g, b FROM points WHERE cube_id=? AND weight>=?");
	
	for(const auto& entry : cube_entries_) {
		const cuboid& cube = entry.cube;
		if(entry.number_of_points > remaining) break;
		
		if(frustum_culling_ && !req.view_frustum.contains_cuboid(cube)) continue;
		++frustum_cubes;
		
		float min_weight = 0;
		float distance = glm::distance(req.position, cube.center());
		if(distance >= downsampling_distance_) {
			min_weight = 1.0 - downsampling_distance_/(distance*2);
			++downsampled_cubes;
		}

		select_cube_points.reset();
		select_cube_points[0] = entry.id;
		select_cube_points[1] = min_weight;
		for(auto r : select_cube_points) {
			buf->x = r[0].float_value();
			buf->y = r[1].float_value();
			buf->z = r[2].float_value();
			buf->r = r[3].int_value();
			buf->g = r[4].int_value();
			buf->b = r[5].int_value();
			
			++buf;
			--remaining;
			++total;
		}
	}

	return total;
}


std::size_t cubes_structure_sqlite_loader::memory_size_() const {
	return cube_entries_.size() * sizeof(cube_entry);
}

std::size_t cubes_structure_sqlite_loader::file_size_() const {
	return const_cast<sqlite_database&>(database_).database_size();
}

std::size_t cubes_structure_sqlite_loader::total_points_() const {
	auto select_number = const_cast<sqlite_database&>(database_).select("SELECT COUNT(*) FROM points");
	select_number.next();
	return select_number.current_row()[0].int_value();
}

}
