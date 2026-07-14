/**
 * Main Class for SQLiteDB - A C++ Wrapper around SQLite3
 * Execute a statement with rows being returned (like SELECT)
 *
 * Author: Bartek Musiol (bmusiol@proton.me)
 *
 * See LICENSE
 */

#include "../incl/SQLiteDB.hpp"
#include "../incl_private/SQLiteConnection.hpp"
#include <sqlite3.h>

using namespace SQLiteDB;

Table Database::execute_statement_returns(const char *sql, Row &params)
{
    Table ret_table;

    conn_->prepare_statement(sql);

    for (auto i = 0; i < params.get_size(); ++i)
    {
        conn_->bind_parameter(params, i);
    }

    ret_table.header = get_headers_for_last_statement();
    // ret_table.table = get_table_name_for_last_statement();
    // ret_table.database = get_database_name_for_last_statement();

    ret_table.data = step_and_get_rows();

    finalize_statement();

    return ret_table;
}

Table Database::execute_statement_returns(const char *sql)
{
    Table ret_table;

    conn_->prepare_statement(sql);

    ret_table.header = get_headers_for_last_statement();
    // ret_table.table = get_table_name_for_last_statement();
    // ret_table.database = get_database_name_for_last_statement();

    ret_table.data = step_and_get_rows();

    finalize_statement();

    return ret_table;
}

///////////////////////
/// Convenience Methods

//

Table Database::execute_statement_returns(std::string sql, Row &params)
{
    return execute_statement_returns(sql.c_str(), params);
}

Table Database::execute_statement_returns(std::string sql)
{
    return execute_statement_returns(sql.c_str());
}