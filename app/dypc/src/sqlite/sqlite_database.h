#ifndef DYPC_SQLITE_DATABASE_H_
#define DYPC_SQLITE_DATABASE_H_

#include <string>
#include <functional>
#include <stdexcept>
#include <utility>
#include <sqlite3.h>

#include "sqlite_error.h"
#include "sqlite_statement.h"
#include "sqlite_select_statement.h"

namespace dypc {


class sqlite_database {
public:
	class result;
	using id_t = sqlite3_int64;

	template<class Function, class... Args> void call(Function fct, Args... args);

private:
	const std::string& filename_;
	sqlite3* database_;
			
	
public:
	explicit sqlite_database(const std::string& filename) : filename_(filename) {
		call(sqlite3_config, SQLITE_CONFIG_SERIALIZED);
		call(sqlite3_open_v2, filename.c_str(), &database_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
	}
	
	sqlite_database(const sqlite_database&) = delete;
	sqlite_database(sqlite_database&& db) : filename_(db.filename_), database_(db.database_) { db.database_ = nullptr; }
	
	~sqlite_database() {
		if(database_) sqlite3_close_v2(database_);
	}
	
	std::size_t database_size();
	
	sqlite3* handle() { return database_; }
	
	void transaction(const std::function<bool()>&);
	
	template<class... Params>
	sqlite_statement prepare(const std::string& query, Params&&... params) {
		sqlite_statement statement(*this, query);
		statement.set_parameters(std::forward<Params>(params)...);
		return statement;
	}
	
	template<class... Params>
	sqlite_select_statement select(const std::string& query, Params&&... params) {
		sqlite_select_statement statement(*this, query);
		statement.set_parameters(std::forward<Params>(params)...);
		return statement;
	}
	
	template<class... Params> void execute(const std::string& query, Params&&... params)
		{ return prepare(query, std::forward<Params>(params)...).execute(); }


	void execute(const std::string& query) {
		call(sqlite3_exec, database_, query.c_str(), nullptr, nullptr, nullptr);
	}

	id_t last_insert_id() { return sqlite3_last_insert_rowid(database_); }
};



template<class Function, class... Args>
inline void sqlite_database::call(Function fct, Args... args) {
	int err = fct(args...);
	if(err != SQLITE_OK) throw sqlite_error(database_);
}


}

#endif
