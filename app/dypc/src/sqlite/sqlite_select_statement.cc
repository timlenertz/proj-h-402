#include "sqlite_select_statement.h"
#include "sqlite_database.h"

namespace dypc {

bool sqlite_select_statement::next() {
	if(done_) reset();
	int res = sqlite3_step(stmt_);
	if(res == SQLITE_ROW) return true;
	else if(res == SQLITE_DONE) { done_ = true; return false; }
	else throw sqlite_error(database_.handle());
}

}
