#ifndef DYPC_SQLITE_ERROR_H_
#define DYPC_SQLITE_ERROR_H_

#include <string>
#include <stdexcept>
#include <sqlite3.h>

namespace dypc {
	
class sqlite_error : public std::runtime_error {
private:
	static std::string error_string_(sqlite3* sql) {
		int code = sqlite3_errcode(sql);
		return std::string("SQLite error ") + std::to_string(code) + ": " + sqlite3_errmsg(sql);
	}
	
public:
	explicit sqlite_error(const std::string& msg) : std::runtime_error(msg) { }
	explicit sqlite_error(sqlite3* sql) : std::runtime_error(error_string_(sql)) { }
};
	
}

#endif
