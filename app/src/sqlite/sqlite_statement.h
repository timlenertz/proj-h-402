#ifndef DYPC_SQLITE_STATEMENT_H_
#define DYPC_SQLITE_STATEMENT_H_

#include <string>
#include <utility>
#include <type_traits>
#include <sqlite3.h>

#include "sqlite_error.h"

namespace dypc {

class sqlite_database;

class sqlite_statement {
public:
	class parameter {
	private:
		sqlite3_stmt* stmt_;
		int index_;
		
		void check_(int result) {			
			if(result != SQLITE_OK) throw sqlite_error(std::string("Could not bind SQLite parameter #") + std::to_string(index_));
		}
	
	public:
		parameter(sqlite3_stmt* stmt, int idx) : stmt_(stmt), index_(idx) { }

		template<class Value, class = typename std::enable_if<std::is_integral<Value>::value>::type>
		parameter& operator=(Value v) { check_(sqlite3_bind_int(stmt_, index_, (int)v)); return *this; }

		template<class Value, class = typename std::enable_if<std::is_floating_point<Value>::value>::type, class=void> // workaround redeclare error (compiler bug?)
		parameter& operator=(Value v) { check_(sqlite3_bind_double(stmt_, index_, (double)v)); return *this; }
			
		parameter& operator=(std::nullptr_t) { check_(sqlite3_bind_null(stmt_, index_)); return *this; }
		
		parameter& operator=(const std::string& str) { check_(sqlite3_bind_text(stmt_, index_, str.c_str(), str.size(), SQLITE_TRANSIENT)); return *this; }
	};

protected:
	sqlite_database& database_;
	sqlite3_stmt* stmt_;
	bool done_ = false;

private:
	template<class CurrentParam, class... RemainingParams>
	void set_parameters_(int idx, CurrentParam&& current, RemainingParams&&... remaining) {
		parameter(stmt_, idx) = std::forward<CurrentParam>(current);
		set_parameters_(idx + 1, std::forward<RemainingParams>(remaining)...);
	}
	
	void set_parameters_(int) { }


public:
	sqlite_statement(sqlite_database& database, const std::string& query);
	~sqlite_statement() { sqlite3_finalize(stmt_); }
	
	parameter operator[](int idx) { return parameter(stmt_, idx + 1); }
	
	template<class... Params>
	void set_parameters(Params&&... params) { set_parameters_(1, std::forward<Params>(params)...); }
	
	void reset();
	void execute();
	
	template<class... Params>
	void operator()(Params&&... params) {
		if(done_) reset();
		set_parameters(std::forward<Params>(params)...);
		execute();
	}
};

}

#endif
