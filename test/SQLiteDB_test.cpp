/**
 * Tests for SQLiteDB - A C++ Wrapper around SQLite3
 *
 * Author: Bartek Musiol (bmusiol@proton.me)
 *
 * See LICENSE
 */

#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

#include "../incl/SQLiteDB.hpp"

TEST(Row, General_Row_test)
{
    // Main
    auto row = SQLiteDB::Row();
    EXPECT_EQ(row.get_size(), 0);

    // Int Test
    const std::int64_t int_testval = 3;
    row.push_integer(int_testval);
    EXPECT_EQ(row.get_size(), 1);

    EXPECT_TRUE(row.is_integer(0));
    EXPECT_FALSE(row.is_real(0));
    EXPECT_FALSE(row.is_text(0));
    EXPECT_FALSE(row.is_blob(0));
    EXPECT_FALSE(row.is_null(0));

    EXPECT_EQ(row.get_integer(0), int_testval);

    try
    {
        row.get_real(0);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a REAL from a SQLiteDB Row at "
                              "index 0 where the type did not match!"));
    }

    try
    {
        row.get_text(0);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a TEXT from a SQLiteDB Row at "
                              "index 0 where the type did not match!"));
    }

    try
    {
        row.get_blob(0);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a BLOB from a SQLiteDB Row at "
                              "index 0 where the type did not match!"));
    }

    // Real Test
    const double double_testval = 5.7;
    row.push_real(double_testval);
    EXPECT_EQ(row.get_size(), 2);

    EXPECT_FALSE(row.is_integer(1));
    EXPECT_TRUE(row.is_real(1));
    EXPECT_FALSE(row.is_text(1));
    EXPECT_FALSE(row.is_blob(1));
    EXPECT_FALSE(row.is_null(1));

    EXPECT_FLOAT_EQ(row.get_real(1), double_testval);

    try
    {
        row.get_integer(1);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get an INTEGER from a SQLiteDB Row "
                              "at index 1 where the type did not match!"));
    }

    try
    {
        row.get_text(1);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a TEXT from a SQLiteDB Row at "
                              "index 1 where the type did not match!"));
    }

    try
    {
        row.get_blob(1);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a BLOB from a SQLiteDB Row at "
                              "index 1 where the type did not match!"));
    }

    // Text Test
    auto text_testval = std::string("hello");
    row.push_text(std::string(
        "hello")); // This should compile with an in place assignment
    row.pop();
    row.push_text(text_testval);
    EXPECT_EQ(row.get_size(), 3);

    EXPECT_FALSE(row.is_integer(2));
    EXPECT_FALSE(row.is_real(2));
    EXPECT_TRUE(row.is_text(2));
    EXPECT_FALSE(row.is_blob(2));
    EXPECT_FALSE(row.is_null(2));

    EXPECT_EQ(row.get_text(2), text_testval);

    try
    {
        row.get_integer(2);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get an INTEGER from a SQLiteDB Row "
                              "at index 2 where the type did not match!"));
    }

    try
    {
        row.get_real(2);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a REAL from a SQLiteDB Row at "
                              "index 2 where the type did not match!"));
    }

    try
    {
        row.get_blob(2);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a BLOB from a SQLiteDB Row at "
                              "index 2 where the type did not match!"));
    }

    // Blob Test
    auto blob_testval = std::vector<std::uint8_t>();
    blob_testval.push_back(5);
    blob_testval.push_back(7);
    blob_testval.push_back(3);
    blob_testval.push_back(0);
    row.push_blob(blob_testval);
    EXPECT_EQ(row.get_size(), 4);

    EXPECT_FALSE(row.is_integer(3));
    EXPECT_FALSE(row.is_real(3));
    EXPECT_FALSE(row.is_text(3));
    EXPECT_TRUE(row.is_blob(3));
    EXPECT_FALSE(row.is_null(3));

    EXPECT_EQ(row.get_blob(3), blob_testval);

    try
    {
        row.get_integer(3);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get an INTEGER from a SQLiteDB Row "
                              "at index 3 where the type did not match!"));
    }

    try
    {
        row.get_real(3);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a REAL from a SQLiteDB Row at "
                              "index 3 where the type did not match!"));
    }

    try
    {
        row.get_text(3);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a TEXT from a SQLiteDB Row at "
                              "index 3 where the type did not match!"));
    }

    // Pop Test
    row.pop();
    EXPECT_EQ(row.get_size(), 3);
    row.pop();
    EXPECT_EQ(row.get_size(), 2);
    row.pop();
    EXPECT_EQ(row.get_size(), 1);
    row.pop();
    EXPECT_EQ(row.get_size(), 0);

    // Null Test
    row.push_null();
    EXPECT_EQ(row.get_size(), 1);
    EXPECT_FALSE(row.is_integer(0));
    EXPECT_FALSE(row.is_real(0));
    EXPECT_FALSE(row.is_text(0));
    EXPECT_FALSE(row.is_blob(0));
    EXPECT_TRUE(row.is_null(0));

    try
    {
        row.get_integer(0);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get an INTEGER from a SQLiteDB Row "
                              "at index 0 where the type did not match!"));
    }

    try
    {
        row.get_real(0);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a REAL from a SQLiteDB Row at "
                              "index 0 where the type did not match!"));
    }

    try
    {
        row.get_text(0);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a TEXT from a SQLiteDB Row at "
                              "index 0 where the type did not match!"));
    }

    try
    {
        row.get_blob(0);
    }
    catch (std::runtime_error e)
    {
        EXPECT_EQ(e.what(),
                  std::string("Attempted to get a BLOB from a SQLiteDB Row at "
                              "index 0 where the type did not match!"));
    }

    row.pop();
    EXPECT_EQ(row.get_size(), 0);
}
TEST(Row, set_test)
{
    // Main
    auto row = SQLiteDB::Row();
    EXPECT_EQ(row.get_size(), 0);

    std::vector<std::uint8_t> blob1 = {0xfe, 0xff, 0x45, 0xa4};
    std::vector<std::uint8_t> blob2 = {0x79, 0x06, 0x45, 0x00, 0xdd};

    row.push_blob(blob1);
    row.push_integer(34);
    row.push_null();
    row.push_real(99.43);
    row.push_text("some_text");
    EXPECT_EQ(row.get_size(), 5);
    EXPECT_EQ(blob1, row.get_blob(0));
    EXPECT_EQ(34, row.get_integer(1));
    ;
    EXPECT_DOUBLE_EQ(99.43, row.get_real(3));
    EXPECT_EQ("some_text", row.get_text(4));

    row.set_blob(0, blob2);
    row.set_integer(1, 1020);
    row.set_real(3, -485.32);
    row.set_text(4, "some_other_text");
    EXPECT_EQ(row.get_size(), 5);
    EXPECT_EQ(blob2, row.get_blob(0));
    EXPECT_EQ(1020, row.get_integer(1));
    ;
    EXPECT_DOUBLE_EQ(-485.32, row.get_real(3));
    EXPECT_EQ("some_other_text", row.get_text(4));
}

TEST(Database, create_and_write_many)
{
    {
        SQLiteDB::Database db("create_and_write_many.db");
        db.execute_plain(
            R"sql(
            DROP TABLE IF EXISTS "testtable";
            CREATE TABLE "testtable" (
            	"intval"	INTEGER,
            	"textval"	TEXT,
            	"realval"	REAL,
            	"blobval"	BLOB
            );
            )sql");
        std::vector<SQLiteDB::Row> rows;
        SQLiteDB::Row row;
        row.push_integer(4711);
        row.push_text("ein teststring");
        row.push_real(8.15);
        std::vector<std::uint8_t> blob_value{4, 6, 7, 3, 2};
        row.push_blob(blob_value);
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_integer(32453);
        row.push_text("mEhr, tests");
        row.push_real(-355.5468);
        row.push_blob(blob_value);
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_null();
        row.push_null();
        row.push_null();
        row.push_null();
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_integer(32453);
        row.push_null();
        row.push_real(-355.5468);
        row.push_blob(blob_value);
        rows.push_back(row);
        db.execute_statement_norows(
            R"sql(
            INSERT INTO "testtable"("intval","textval","realval","blobval")
            VALUES (?,?,?,?);
            )sql",
            rows);
    }
}

TEST(Database, simple_select)
{
    {
        SQLiteDB::Database db("simple_select.db");
        db.execute_plain(
            R"sql(
            DROP TABLE IF EXISTS "testtable";
            CREATE TABLE "testtable" (
            	"intval"	INTEGER,
            	"textval"	TEXT,
            	"realval"	REAL,
            	"blobval"	BLOB
            );
            )sql");
        std::vector<SQLiteDB::Row> rows;
        SQLiteDB::Row row;
        row.push_integer(4711);
        row.push_text("ein teststring");
        row.push_real(8.15);
        std::vector<std::uint8_t> blob_value{4, 6, 7, 3, 2};
        row.push_blob(blob_value);
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_integer(32453);
        row.push_text("mEhr, tests");
        row.push_real(-355.5468);
        row.push_blob(blob_value);
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_null();
        row.push_null();
        row.push_null();
        row.push_null();
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_integer(32453);
        row.push_null();
        row.push_real(-355.5468);
        row.push_blob(blob_value);
        rows.push_back(row);
        db.execute_statement_norows(
            R"sql(
            INSERT INTO "testtable"("intval","textval","realval","blobval")
            VALUES (?,?,?,?);
            )sql",
            rows);

        auto simple_select = db.execute_statement_returns(
            R"sql(
            SELECT * FROM "testtable";
            )sql");
        int a = 0;
    }
}

TEST(Database, params_select)
{
    {
        SQLiteDB::Database db("params_select.db");
        db.execute_plain(
            R"sql(
            DROP TABLE IF EXISTS "testtable";
            CREATE TABLE "testtable" (
            	"intval"	INTEGER,
            	"textval"	TEXT,
            	"realval"	REAL,
            	"blobval"	BLOB
            );
            )sql");
        std::vector<SQLiteDB::Row> rows;
        SQLiteDB::Row row;
        row.push_integer(4711);
        row.push_text("ein teststring");
        row.push_real(8.15);
        std::vector<std::uint8_t> blob_value{4, 6, 7, 3, 2};
        row.push_blob(blob_value);
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_integer(32453);
        row.push_text("mEhr, tests");
        row.push_real(-355.5468);
        row.push_blob(blob_value);
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_null();
        row.push_null();
        row.push_null();
        row.push_null();
        rows.push_back(row);
        row = SQLiteDB::Row();
        row.push_integer(32453);
        row.push_null();
        row.push_real(-355.5468);
        row.push_blob(blob_value);
        rows.push_back(row);
        db.execute_statement_norows(
            R"sql(
            INSERT INTO "testtable"("intval","textval","realval","blobval")
            VALUES (?,?,?,?);
            )sql",
            rows);
        SQLiteDB::Row params;
        params.push_integer(5000);
        auto simple_select = db.execute_statement_returns(
            R"sql(
            SELECT * FROM "testtable" WHERE "intval" > ?;
            )sql",
            params);
        int a = 0;
    }
}

TEST(Database, explicit_syntax_error_bind_error)
{
    {
        SQLiteDB::Database db("explicit_syntax_error_bind_error.db");
        db.execute_plain(
            R"sql(
            DROP TABLE IF EXISTS "testtable";
            CREATE TABLE "testtable" (
            	"intval"	INTEGER
            );
            )sql");
        std::vector<SQLiteDB::Row> rows;
        SQLiteDB::Row row;
        row.push_null();
        bool has_thrown = false;
        try
        {
            db.execute_statement_norows(
                R"sql(
            INSERT INTO "testtable"("intval")
            VALUES (?,?,?,?);
            )sql",
                rows);
        }
        catch (std::runtime_error &e)
        {
            std::cerr << e.what() << "\nCorrectly triggered.";
            has_thrown = true;
        }
        EXPECT_TRUE(has_thrown);
    }
}