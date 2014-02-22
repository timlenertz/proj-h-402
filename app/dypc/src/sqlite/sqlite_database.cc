#include "sqlite_database.h"
#include <sys/stat.h>

namespace dypc {

void sqlite_database::transaction(const std::function<bool()>& fct) {
	execute("BEGIN TRANSACTION");
	try {
		bool ok = fct();
		if(ok) execute("COMMIT TRANSACTION");
		else execute("ROLLBACK TRANSACTION");
	} catch(...) {
		execute("ROLLBACK TRANSACTION");
		throw;
	}
}

std::size_t sqlite_database::database_size() {
	struct stat st;
	stat(filename_.c_str(), &st);
	return st.st_size;
}


}
