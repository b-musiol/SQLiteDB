cmake_minimum_required(VERSION 3.14)

project(SQLiteDB)

add_library(SQLiteDB
  src/SQLiteDB.cpp
  src/execute_statement_norows.cpp
  src/execute_statement_returns.cpp
  src/SQLiteConnection.cpp
  src/Row.cpp
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

target_compile_features(SQLiteDB PUBLIC cxx_std_20)


include(FetchContent)
FetchContent_Declare(
  sqlite
  URL https://sqlite.org/2026/sqlite-autoconf-3530100.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(sqlite)


add_library(sqlite3 STATIC
    ${sqlite_SOURCE_DIR}/sqlite3.c
)

target_include_directories(sqlite3 PRIVATE
    ${sqlite_SOURCE_DIR}
)

target_link_libraries(SQLiteDB PRIVATE sqlite3)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0  # Or use a specific commit hash
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(RunTests_SQLiteDB test/SQLiteDB_test.cpp)
target_link_libraries(RunTests_SQLiteDB SQLiteDB GTest::gtest_main)
target_compile_features(RunTests_SQLiteDB PUBLIC cxx_std_20)


include(GoogleTest)
gtest_discover_tests(RunTests_SQLiteDB)

enable_testing()
