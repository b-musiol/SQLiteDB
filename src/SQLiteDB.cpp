/**
 * Main Class for SQLiteDB - A C++ Wrapper around SQLite3
 *
 * Author: Bartek Musiol (bmusiol@proton.me)
 *
 * See LICENSE
 */

#include "../incl/SQLiteDB.hpp"
#include "../incl/SQLiteConnection.hpp"
#include <memory>
#include <sqlite3.h>

using namespace SQLiteDB;

Database::Database(const std::string db_path,
                   bool write,
                   bool multithread_enable,
                   bool wal_in_journal,
                   bool fast_mode,
                   bool journal_off)
{
    // Make sure SQLite is not running in some form in this instance
    sqlite3_shutdown();

    // Handle multithreading, if it is required
    if (multithread_enable)
    {
        sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    }
    else
    {
        sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
    }

    // Finally Initialize SQLite
    sqlite3_initialize();

    // Deal with Read-Only Mode if required
    int flags = write ? SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                      : SQLITE_OPEN_READONLY;

    // Actually open the Database
    conn_ = std::make_unique<SQLiteConnection>(db_path, flags);

    // Set up the connection
    if (conn_->rc == SQLITE_OK)
    {
        // Fast mode for in-memory journaling or turning it off etc.
        if (fast_mode)
        {
            execute_plain("PRAGMA auto_vacuum = OFF;");
            execute_plain("PRAGMA synchronous = OFF;");

            if (journal_off == true)
            {
                execute_plain("PRAGMA journal_mode = OFF;");
            }
            else
            {
                execute_plain("PRAGMA journal_mode = MEMORY;");
            }
            execute_plain("PRAGMA cache_spill = OFF;");
        }
        // WAL mode or Journal Mode
        else
        {
            if (wal_in_journal)
            {
                execute_plain("PRAGMA journal_mode = WAL;");
            }
        }
    }
}

Database::~Database()
{
    // Connection auto-terminates because SQLiteConnection has the appropriate
    // Destructor.
}

void Database::begin_transaction()
{
    conn_->execute_plain("BEGIN TRANSACTION;");
}

void Database::commit()
{
    conn_->execute_plain("COMMIT;");
}

void Database::rollback()
{
    conn_->execute_plain("ROLLBACK;");
}

void Database::execute_plain(std::string sql)
{
    conn_->execute_plain(sql);
}

void Database::execute_plain(const char *sql)
{
    conn_->execute_plain(sql);
}

void Database::step_and_reset()
{
    conn_->step();
    sqlite3_reset(conn_->stmt);
}
void Database::finalize_statement()
{
    sqlite3_finalize(conn_->stmt);
}

std::vector<Row> Database::step_and_get_rows()
{
    std::vector<Row> rows;
    result_t buf;
    size_t blob_bytes;
    const std::uint8_t *blob_buf;
    std::vector<std::uint8_t> blob_out_buf;

    while (conn_->step())
    {
        Row row;

        int column_count = sqlite3_column_count(conn_->stmt);

        for (int i = 0; i < column_count; i++)
        {

            int type = sqlite3_column_type(conn_->stmt, i);

            switch (type)
            {

            case SQLITE_INTEGER:
                buf = sqlite3_column_int64(conn_->stmt, i);
                row.push_integer(std::get<std::int64_t>(buf));
                break;

            case SQLITE_FLOAT:
                buf = sqlite3_column_double(conn_->stmt, i);
                row.push_real(std::get<double>(buf));
                break;

            case SQLITE_TEXT:
                buf = std::string(reinterpret_cast<const char *>(
                    sqlite3_column_text(conn_->stmt, i)));
                row.push_text(std::get<std::string>(buf));
                break;

            case SQLITE_BLOB:
                blob_bytes = sqlite3_column_bytes(conn_->stmt, i);
                blob_buf   = static_cast<const std::uint8_t *>(
                    sqlite3_column_blob(conn_->stmt, i));
                for (size_t b = 0; b < blob_bytes; ++b)
                {
                    blob_out_buf.push_back(*(blob_buf + b));
                }
                row.push_blob(blob_out_buf);
                break;

            case SQLITE_NULL:
                row.push_null();
                break;
            }
        }
        rows.push_back(row);
    }

    return rows;
}

std::vector<std::string> Database::get_headers_for_last_statement()
{
    std::vector<std::string> headers;
    size_t col_count = sqlite3_column_count(conn_->stmt);
    for (size_t i = 0; i < col_count; i++)
    {
        const char *col_name = sqlite3_column_name(conn_->stmt, i);
        headers.push_back(std::string(col_name));
    }
    return headers;
}

// std::string Database::get_table_name_for_last_statement()
// {
//         const char * table_name = sqlite3_column_table_name(conn_->stmt, 1);
//         return std::string(table_name);
// }

// std::string Database::get_database_name_for_last_statement()
// {

//     const char * database_name = sqlite3_column_database_name(conn_->stmt,
//     1); return std::string(database_name);
// }