/**
 * Public header for SQLiteDB - A C++ Wrapper around SQLite3
 *
 * Author: Bartek Musiol (bmusiol@proton.me)
 *
 * See LICENSE
 */


#ifndef _SQLITEDB_HPP_
#define _SQLITEDB_HPP_

#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace SQLiteDB
{

/**
 * SQLite provides the builtins `REAL`, `NUMERIC`, `INTEGER`, `TEXT` and `BLOB`.
 *
 * `NUMERIC` is sort of ignored here, because it is rarely used anyway.
 *
 * `REAL` maps to `double`
 *
 * `INTEGER` maps to `std::int64_t`
 *
 * `TEXT` maps to `std::string`
 *
 * `BLOB` maps to `std::vector<std::uint8_t>`, a vector of bytes
 *
 * If `NULL` is encountered, `std::monostate` is set.
 *
 */
typedef std::variant<std::monostate,
                     std::int64_t,
                     double,
                     std::string,
                     std::vector<std::uint8_t>>
    result_t;

/**
 * One row of data
 *
 */
class Row
{
  public:
    enum ValueType
    {
        INTEGER,
        REAL,
        TEXT,
        BLOB,
        ISNULL
    };

  public:
    /**
     * Get an `INTEGER (`int64_t`) at column of index `ix`. Throws if it is no
     * integer.
     */
    std::int64_t get_integer(std::uint64_t ix);
    /**
     * Get a `REAL (`double`) at column of index `ix`. Throws if it is no
     * double.
     */
    double get_real(std::uint64_t ix);
    /**
     * Get a `TEXT (`string`) at column of index `ix`. Throws if it is no
     * string.
     */
    std::string get_text(std::uint64_t ix);
    /**
     * Get a BLOB (`vector<uint8_t>`) at column of index `ix`. Throws if it is
     * no blob.
     */
    std::vector<std::uint8_t> get_blob(std::uint64_t ix);
    // There is no reason to have a get_null() method. What else but NULL do you
    // expect?

  public:
    /**
     * Push a `INTEGER (`int64_t`) value at the end of the row.
     */
    void push_integer(std::int64_t integer_value);
    /**
     * Push a `REAL (`double`) value at the end of the row.
     */
    void push_real(double real_value);
    /**
     * Push a `TEXT (`string`) value at the end of the row.
     */
    void push_text(std::string text_value);
    /**
     * Push a BLOB (`vector<uint8_t>`) value at the end of the row.
     */
    void push_blob(std::vector<std::uint8_t> &blob_value);
    /**
     * Push a NULL value at the end of the row.
     */
    void push_null();
    /**
     * Remove the last element
     */
    void pop();

  public:
    /**
     * Check if value at column number `ix` is a `INTEGER (`int64_t`).
     */
    bool is_integer(std::uint64_t ix);
    /**
     * Check if value at column number `ix` is a `REAL (`double`).
     */
    bool is_real(std::uint64_t ix);
    /**
     * Check if value at column number `ix` is a `TEXT (`string`).
     */
    bool is_text(std::uint64_t ix);
    /**
     * Check if value at column number `ix` is a BLOB (`vector<uint8_t>`).
     */
    bool is_blob(std::uint64_t ix);
    /**
     * Check if value at column number `ix` is NULL.
     */
    bool is_null(std::uint64_t ix);

  public:
    /**
     * Return how many columns this row has.
     */
    size_t get_size();

  private:
    std::vector<result_t> data_;
    std::vector<ValueType> data_type_;
};

class Database
{

  public:
    /**
     * Creates a new database at `dbPath`, unless it already exists. In that
     * case it only opens it. If `write` is set, the database is opened in
     * Read-Write mode and potentially created. If `write` is not set, the
     * database is opened in Read-Only mode and will crash if the database does
     * not exist.
     * It is possible to enable multithreaded mode with `multithread_enable`.
     * If WAL (Write-Ahead-Log) is supposed to be used as the journal mode,
     * `wal_in_journal` must be set. Otherwise the regular Journal is used. This
     * is overridden by `fast_mode`, which if set, enables in-memory journaling
     * or no journaling at all, if `journal_off` is set. Additionally other
     * potentially speed improving (and integrity reducing) pragmas are
     * executed.
     *
     */
    Database(const std::string db_path,
             bool write              = true,
             bool multithread_enable = false,
             bool wal_in_journal     = true,
             bool fast_mode          = false,
             bool journal_off        = false);
    /**
     * Automatically closes the connection properly.
     */
    ~Database();

    public:


  private:
    struct SQLiteConnection;

  private:
    std::unique_ptr<SQLiteConnection> conn_;
    bool is_write_enabled_;
    std::string db_path_;
    std::uint64_t reserve_for_select_;
};

} // namespace SQLiteDB

#endif