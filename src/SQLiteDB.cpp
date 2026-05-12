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
    sqlite3_step(conn_->stmt);
    sqlite3_reset(conn_->stmt);
}
void Database::finalize_statement()
{
    sqlite3_finalize(conn_->stmt);
}