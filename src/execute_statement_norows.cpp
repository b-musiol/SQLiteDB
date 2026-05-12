/**
 * Main Class for SQLiteDB - A C++ Wrapper around SQLite3
 * Execute a statement with no rows being returned (like INSERT/UPDATE/DELETE)
 *
 * Author: Bartek Musiol (bmusiol@proton.me)
 *
 * See LICENSE
 */

#include "../incl/SQLiteConnection.hpp"
#include "../incl/SQLiteDB.hpp"
#include <sqlite3.h>

using namespace SQLiteDB;


void Database::execute_statement_norows(const char *sql,
                                        std::vector<Row> &params_vec)
{
    conn_->prepare_statement(sql);

    begin_transaction();
    for (auto &params : params_vec)
    {
        for (auto i = 0; i < params.get_size(); ++i)
        {
            conn_->bind_parameter(params, i);
        }
        step_and_reset();
    }
    commit();

    finalize_statement();
}

void Database::execute_statement_norows(const char *sql, Row &params)
{
    conn_->prepare_statement(sql);

    for (auto i = 0; i < params.get_size(); ++i)
    {
        conn_->bind_parameter(params, i);
    }

    step_and_reset();

    finalize_statement();
}

void Database::execute_statement_norows(const char *sql)
{
    conn_->prepare_statement(sql);

    step_and_reset();

    finalize_statement();
}

////////////////////////////////////////////////
/// Convenience methods just call the actual ones
////////////////////////////////////////////////

//

void Database::execute_statement_norows(std::string sql,
                                        std::vector<Row> &params)
{
    execute_statement_norows(sql.c_str(), params);
}
void Database::execute_statement_norows(std::string sql, Row &params)
{
    execute_statement_norows(sql.c_str(), params);
}
void Database::execute_statement_norows(std::string sql)
{
    execute_statement_norows(sql.c_str());
}
