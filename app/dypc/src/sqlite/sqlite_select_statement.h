#ifndef DYPC_SQLITE_SELECT_STATEMENT_H_
#define DYPC_SQLITE_SELECT_STATEMENT_H_

#include <string>
#include <functional>
#include <stdexcept>
#include <iterator>
#include <vector>
#include <type_traits>

#include <sqlite3.h>

#include "sqlite_error.h"
#include "sqlite_statement.h"

namespace dypc {

class sqlite_select_statement : public sqlite_statement {
public:
	class value {
	private:
		sqlite3_stmt* stmt_;
		int col_;
	public:
		value(sqlite3_stmt* stmt, int col) : stmt_(stmt), col_(col) { }
		
		int int_value() const { return sqlite3_column_int(stmt_, col_); }
		sqlite3_int64 id_value() const { return sqlite3_column_int64(stmt_, col_); }
		sqlite3_int64 int64_value() const { return sqlite3_column_int64(stmt_, col_); }
		double double_value() const { return sqlite3_column_double(stmt_, col_); }
		float float_value() const { return sqlite3_column_double(stmt_, col_); }
		std::string string_value() const {
			const auto* str = sqlite3_column_text(stmt_, col_);
			if(str) return std::string((const char*)str);
			else return "";
		}
		
		bool is_null() const { return (sqlite3_column_text(stmt_, col_) == nullptr); }
	};


	class row {
	private:
		sqlite3_stmt* stmt_;
		
	public:
		explicit row(sqlite3_stmt* stmt) : stmt_(stmt) { }
		value operator[](int col) { return value(stmt_, col); }
	};
	
	
	class iterator : public std::iterator<std::input_iterator_tag, row> {
	private:
		sqlite_select_statement* statement_;
	public:
		explicit iterator(sqlite_select_statement* statement = nullptr) : statement_(statement) { }
		row operator*() { return statement_->current_row(); }
		iterator& operator++() {
			bool ok = statement_->next();
			if(! ok) statement_ = nullptr;
			return *this;
		}
		bool operator==(const iterator& it) const { return statement_ == it.statement_; }
		bool operator!=(const iterator& it) const { return statement_ != it.statement_; }
	};


	sqlite_select_statement(sqlite_database& database, const std::string& query) : sqlite_statement(database, query) { }
	
	bool next();
	row current_row() { return row(stmt_); }

	iterator begin() {
		if(next()) return iterator(this);
		else return iterator();
	}
	
	iterator end() { return iterator(); }
	
	template<class... Params>
	sqlite_select_statement& operator()(Params&&... params) {
		set_parameters(std::forward<Params>(params)...);
		return *this;
	}
};

}

#endif
