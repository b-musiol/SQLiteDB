/**
 * Private header for SQLiteDB - A C++ Wrapper around SQLite3
 * SQLiteConnection
 *
 * Author: Bartek Musiol (bmusiol@proton.me)
 *
 * See LICENSE
 */

#ifndef _SQLITECONNECTION_HPP_
#define _SQLITECONNECTION_HPP_

#include "../incl/SQLiteDB.hpp"
#include <sqlite3.h>

using namespace SQLiteDB;

struct Database::SQLiteConnection
{
    private:
    std::vector<std::uint8_t> blob_buf;

    public:
    sqlite3 *db = nullptr;
    sqlite3_stmt *stmt;
    int rc = SQLITE_OK;
    bool is_write_enabled;
    std::string db_path;
    std::uint64_t reserve_for_select;
    char *errmsg = nullptr;


    SQLiteConnection(const std::string db_path,
                     int flags,
                     std::uint64_t reserve_for_select = 16);

    ~SQLiteConnection();

    void check_maybe_throw(std::string msg, int check_against = SQLITE_OK, bool negative = true);

    /**
     * Executes a plain statement from `sql`.
     */
    void execute_plain(std::string sql);
    /**
     * Executes a plain statement from `sql`.
     */
    void execute_plain(const char *sql);
    /**
     * Prepares the statement from the string `sql`.
     */
    void prepare_statement(std::string sql);
    /**
     * Prepares the statement from the string `sql`.
     */
    void prepare_statement(const char *sql);
    /**
     * Binds a parameter from `row` at `ix` and returns its `ValueType`
     */
    Row::ValueType bind_parameter(Row &params, std::uint64_t ix);
    /**
     * Steps the active statement. Returns `true` if `SQLITE_ROW` was returned
     * and `false` if `SQLITE_DONE` was returned. Throws on error. Loops on
     * `SQLITE_BUSY`
     */
    bool step();
};

#endif // _SQLITECONNECTION_HPP_