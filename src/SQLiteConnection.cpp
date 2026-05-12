#include "../incl/SQLiteConnection.hpp"
#include <format>
#include <sqlite3.h>
#include <stdexcept>

using namespace SQLiteDB;

Database::SQLiteConnection::SQLiteConnection(const std::string db_path,
                                             int flags,
                                             std::uint64_t reserve_for_select)
    : db_path(db_path), is_write_enabled((flags & SQLITE_OPEN_READWRITE) != 0),
      reserve_for_select(reserve_for_select)
{
    rc = sqlite3_open_v2(db_path.c_str(), &db, flags, nullptr);
    check_maybe_throw(std::format("Failed to open database at {}", db_path));
}
Database::SQLiteConnection::~SQLiteConnection()
{
    if (db)
    {
        sqlite3_close(db);
    }
}

void Database::SQLiteConnection::check_maybe_throw(std::string msg)
{
    if (rc != SQLITE_OK)
    {
        sqlite3_close(db);
        throw std::runtime_error(msg);
    }
}

void Database::SQLiteConnection::prepare_statement(const char *sql)
{
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    check_maybe_throw(std::format("Failed to prepare statement in database "
                                  "at {}\n\nBad Statement:\n\n{}",
                                  db_path,
                                  sql));
}

void Database::SQLiteConnection::prepare_statement(std::string sql)
{
    prepare_statement(sql.c_str());
}

Row::ValueType Database::SQLiteConnection::bind_parameter(Row &row,
                                                          std::uint64_t ix)
{
    auto val_type        = row.get_type(ix);
    std::uint64_t sql_ix = ix + 1;
    switch (val_type)
    {
    case Row::INTEGER:
        rc = sqlite3_bind_int64(stmt, sql_ix, row.get_integer(ix));
        check_maybe_throw(
            std::format("Failed to bind INTEGER value to statement in database "
                        "at {}\n\nBad value:\n\n{}",
                        db_path,
                        row.get_integer(ix)));
        break;
    case Row::REAL:
        rc = sqlite3_bind_double(stmt, sql_ix, row.get_real(ix));
        check_maybe_throw(
            std::format("Failed to bind REAL value to statement in database "
                        "at {}\n\nBad value:\n\n{}",
                        db_path,
                        row.get_real(ix)));
        break;
    case Row::TEXT:
        rc = sqlite3_bind_text(stmt,
                               sql_ix,
                               row.get_text(ix).c_str(),
                               -1,
                               SQLITE_TRANSIENT);
        check_maybe_throw(
            std::format("Failed to bind TEXT value to statement in database "
                        "at {}\n\nBad value:\n\n{}",
                        db_path,
                        row.get_text(ix)));
        break;
    case Row::BLOB:
        blob_buf = row.get_blob(ix);
        rc       = sqlite3_bind_blob64(stmt,
                                       sql_ix,
                                       blob_buf.data(),
                                       blob_buf.size(),
                                       SQLITE_TRANSIENT);
        check_maybe_throw(
            std::format("Failed to bind BLOB value to statement in database "
                        "at {}\n\nBad value size:\n\n{}",
                        db_path,
                        blob_buf.size()));
        break;
    case Row::ISNULL:
        rc = sqlite3_bind_null(stmt, sql_ix);
        check_maybe_throw(
            std::format("Failed to bind NULL value to statement in database "
                        "at {}",
                        db_path));
        break;
    }

    return val_type;
}

void Database::SQLiteConnection::execute_plain(std::string sql)
{
    execute_plain(sql.c_str());
}

void Database::SQLiteConnection::execute_plain(const char *sql)
{
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errmsg);
    check_maybe_throw(std::format("Failed to execute plain sql in database "
                                  "at {}\n\nSQLite error: {}\n\nBad SQL:\n\n{}",
                                  db_path,
                                  errmsg ? errmsg : "unknown error",
                                  sql));
}
