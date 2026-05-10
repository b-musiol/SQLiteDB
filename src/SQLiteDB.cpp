/**
 * Main Class for SQLiteDB - A C++ Wrapper around SQLite3
 *
 * Author: Bartek Musiol (bmusiol@proton.me)
 *
 * See LICENSE
 */

#include "../incl/SQLiteDB.hpp"
#include <sqlite3.h>
#include <format>
#include <memory>

using namespace SQLiteDB;

struct Database::SQLiteConnection
{
    sqlite3 *db = nullptr;
    int rc;

    SQLiteConnection(const std::string db_path, int flags)
    {
        rc = sqlite3_open_v2(db_path.c_str(), &db, flags, nullptr);

        if (rc != SQLITE_OK)
        {
            sqlite3_close(db);
            throw std::runtime_error(
                std::format("Failed to open database at {}", db_path));
        }
    }

    ~SQLiteConnection()
    {
        if (db)
        {
            sqlite3_close(db);
        }
    }
};

Database::Database(const std::string db_path,
                   bool write,
                   bool multithread_enable,
                   bool wal_in_journal,
                   bool fast_mode,
                   bool journal_off)
    : is_write_enabled_(write)
    , db_path_(db_path)
    , reserve_for_select_(16)
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
    conn_ = std::make_unique<SQLiteConnection>(db_path_, flags);

    // Set up the connection
    if (conn_->rc == SQLITE_OK)
    {
        // Fast mode for in-memory journaling or turning it off etc.
        if (fast_mode)
        {
            // Turn off auto_vacuum
            sqlite3_exec(
                conn_->db, "PRAGMA auto_vacuum = OFF;", NULL, NULL, NULL);
            // Disable synchronous mode
            sqlite3_exec(
                conn_->db, "PRAGMA synchronous = OFF;", NULL, NULL, NULL);

            if (journal_off == true)
            {
                // Turn off the journal
                sqlite3_exec(
                    conn_->db, "PRAGMA journal_mode = OFF;", NULL, NULL, NULL);
            }
            else
            {
                // Journal in memory
                sqlite3_exec(conn_->db,
                             "PRAGMA journal_mode = MEMORY;",
                             NULL,
                             NULL,
                             NULL);
            }
            // Disable Cache spilling
            sqlite3_exec(
                conn_->db, "PRAGMA cache_spill = OFF;", NULL, NULL, NULL);
        }
        else
        {
            // Enable Write-Ahead-Log instead of Journal
            if (wal_in_journal)
            {
                sqlite3_exec(
                    conn_->db, "PRAGMA journal_mode = WAL;", NULL, NULL, NULL);
            }
        }
    }
}

Database::~Database()
{
    // Connection auto-terminates because SQLiteConnection has the appropriate
    // Destructor.
}