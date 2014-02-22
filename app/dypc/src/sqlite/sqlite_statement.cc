#include "sqlite_statement.h"
#include "sqlite_database.h"

namespace dypc {

sqlite_statement::sqlite_statement(sqlite_database& database, const std::string& query) : database_(database) {
	database_.call(sqlite3_prepare_v2, database_.handle(), query.c_str(), query.size(), &stmt_, nullptr);
}
	
void sqlite_statement::reset() {
	database_.call(sqlite3_reset, stmt_);
	done_ = false;
}

void sqlite_statement::execute() {
	if(done_) reset();
	int res = sqlite3_step(stmt_);
	if(res == SQLITE_ROW) throw sqlite_error("SELECT statement executed through sqlite_statement::execute");
	else if(res == SQLITE_DONE) done_ = true;
	else throw sqlite_error(database_.handle());
}

}
