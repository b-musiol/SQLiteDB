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
    /**
     * Gets the `ValueType` of the entry at `ix`.
     */
    ValueType get_type(std::uint64_t ix);

  private:
    std::vector<result_t> data_;
    std::vector<ValueType> data_type_;
};

/**
 * A header row and data rows
 */
struct Table
{
    std::string database;
    std::string table;
    std::vector<std::string> header;
    std::vector<Row> data;
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
    /**
     * Executes `BEGIN_TRANSACTION;`. This does not call finalize.
     */
    void begin_transaction();
    /**
     * Executes `COMMIT;`. This does not call finalize.
     */
    void commit();
    /**
     * Executes `ROLLBACK;`. This does not call finalize.
     */
    void rollback();
    /**
     * Executes arbitrary `sql`. This does not call finalize.
     */
    void execute_plain(std::string sql);
    /**
     * Executes arbitrary `sql`. This does not call finalize.
     */
    void execute_plain(const char *sql);
    /**
     * Steps and resets the currently active statement. No returning values
     */
    void step_and_reset();
    /**
     * Gets the headers for the currently active statement.
     */
    std::vector<std::string> get_headers_for_last_statement();
    // /**
    //  * Gets the table name for the currently active statement.
    //  */
    // std::string get_table_name_for_last_statement();
    // /**
    //  * Gets the database name for the currently active statement.
    //  */
    // std::string get_database_name_for_last_statement();
    /**
     * Gets all rows through stepping
     */
    std::vector<Row> step_and_get_rows();
    /**
     * finalizes the currently active statement
     */
    void finalize_statement();

  public:
    /**
     * Executes some `sql` code as a prepared statement multiple times with
     * multiple `params`. No data is returned, so this is not useful for SELECT
     * statements, but might be a good choice for INSERT, UPDATE or DELETE
     * statements. The order of `params` in each `Row` must align with the order
     * of placeholders `?` within `sql`.
     */
    void execute_statement_norows(std::string sql, std::vector<Row> &params);
    /**
     * Executes some `sql` code as a prepared statement once with `params`. No
     * data is returned, so this is not useful for SELECT statements, but might
     * be a good choice for INSERT, UPDATE or DELETE statements. The order of
     * `params` in `Row` must align with the order of placeholders `?` within
     * `sql`.
     */
    void execute_statement_norows(std::string sql, Row &params);
    /**
     * Executes some `sql` code as a prepared statement once without any
     * parameters. No data is returned, so this is not useful for SELECT
     * statements, but might be a good choice for INSERT, UPDATE or DELETE
     * statements.
     */
    void execute_statement_norows(std::string sql);

    // Convenience methods

    /**
     * Executes some `sql` code as a prepared statement once without any
     * parameters. No data is returned, so this is not useful for SELECT
     * statements, but might be a good choice for INSERT, UPDATE or DELETE
     * statements.
     */
    void execute_statement_norows(const char *sql);
    /**
     * Executes some `sql` code as a prepared statement multiple times with
     * multiple `params`. No data is returned, so this is not useful for SELECT
     * statements, but might be a good choice for INSERT, UPDATE or DELETE
     * statements. The order of `params` in each `Row` must align with the order
     * of placeholders `?` within `sql`.
     */
    void execute_statement_norows(const char *sql, std::vector<Row> &params);
    /**
     * Executes some `sql` code as a prepared statement once with `params`. No
     * data is returned, so this is not useful for SELECT statements, but might
     * be a good choice for INSERT, UPDATE or DELETE statements. The order of
     * `params` in `Row` must align with the order of placeholders `?` within
     * `sql`.
     */
    void execute_statement_norows(const char *sql, Row &params);

  public:
    /**
     * Executes some `sql` code as a prepared statement with `params`. Data is
     * returned, so this is useful for SELECT statements, but might not be a
     * good choice for INSERT, UPDATE or DELETE statements. The order of
     * `params` in each `Row` must align with the order of placeholders `?`
     * within `sql`. Returns a `Table`.
     */
    Table execute_statement_returns(std::string sql, Row &params);
    /**
     * Executes some `sql` code as a prepared statement once without any
     * parameters. Data is returned, so this is useful for SELECT statements,
     * but might be not a good choice for INSERT, UPDATE or DELETE statements.
     */
    Table execute_statement_returns(std::string sql);

    // Convenience methods

    /**
     * Executes some `sql` code as a prepared statement with `params`. Data is
     * returned, so this is useful for SELECT statements, but might not be a
     * good choice for INSERT, UPDATE or DELETE statements. The order of
     * `params` in each `Row` must align with the order of placeholders `?`
     * within `sql`. Returns a `Table`.
     */
    Table execute_statement_returns(const char *sql, Row &params);
    /**
     * Executes some `sql` code as a prepared statement once without any
     * parameters. Data is returned, so this is useful for SELECT statements,
     * but might be not a good choice for INSERT, UPDATE or DELETE statements.
     */
    Table execute_statement_returns(const char *sql);

  private:
    struct SQLiteConnection;

  private:
    std::unique_ptr<SQLiteConnection> conn_;
};

} // namespace SQLiteDB

#endif