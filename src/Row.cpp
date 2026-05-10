/**
 * Row Class for SQLiteDB - A C++ Wrapper around SQLite3
 *
 * Author: Bartek Musiol (bmusiol@proton.me)
 *
 * See LICENSE
 */

#include "../incl/SQLiteDB.hpp"
#include <format>
#include <stdexcept>
#include <variant>

using namespace SQLiteDB;

// Type Checkers
bool Row::is_integer(std::uint64_t ix)
{
    return std::holds_alternative<std::int64_t>(data_.at(ix));
}

bool Row::is_real(std::uint64_t ix)
{
    return std::holds_alternative<double>(data_.at(ix));
}

bool Row::is_text(std::uint64_t ix)
{
    return std::holds_alternative<std::string>(data_.at(ix));
}

bool Row::is_blob(std::uint64_t ix)
{
    return std::holds_alternative<std::vector<std::uint8_t>>(data_.at(ix));
}

bool Row::is_null(std::uint64_t ix)
{
    return std::holds_alternative<std::monostate>(data_.at(ix));
}

// Getters
std::int64_t Row::get_integer(std::uint64_t ix)
{
    try
    {
        auto value = std::get<std::int64_t>(data_.at(ix));
        return value;
    }
    catch (std::bad_variant_access)
    {
        throw std::runtime_error(std::format(
            "Attempted to get an INTEGER from a SQLiteDB Row at index "
            "{} where the type did not match!",
            ix));
    }
}

double Row::get_real(std::uint64_t ix)
{
    try
    {
        auto value = std::get<double>(data_.at(ix));
        return value;
    }
    catch (std::bad_variant_access)
    {
        throw std::runtime_error(std::format(
            "Attempted to get a REAL from a SQLiteDB Row at index {} "
            "where the type did not match!",
            ix));
    }
}

std::string Row::get_text(std::uint64_t ix)
{
    try
    {
        auto value = std::get<std::string>(data_.at(ix));
        return value;
    }
    catch (std::bad_variant_access)
    {
        throw std::runtime_error(std::format(
            "Attempted to get a TEXT from a SQLiteDB Row at index {} "
            "where the type did not match!",
            ix));
    }
}

std::vector<std::uint8_t> Row::get_blob(std::uint64_t ix)
{
    try
    {
        auto value = std::get<std::vector<std::uint8_t>>(data_.at(ix));
        return value;
    }
    catch (std::bad_variant_access)
    {
        throw std::runtime_error(std::format(
            "Attempted to get a BLOB from a SQLiteDB Row at index {} "
            "where the type did not match!",
            ix));
    }
}

// Setters

void Row::push_integer(std::int64_t integer_value)
{
    data_.push_back(integer_value);
    data_type_.push_back(ValueType::INTEGER);
}

void Row::push_real(double real_value)
{
    data_.push_back(real_value);
    data_type_.push_back(ValueType::REAL);
}

void Row::push_text(std::string text_value)
{
    data_.push_back(text_value);
    data_type_.push_back(ValueType::TEXT);
}

void Row::push_blob(std::vector<std::uint8_t> &blob_value)
{
    data_.push_back(blob_value);
    data_type_.push_back(ValueType::BLOB);
}

void Row::push_null()
{
    data_.push_back((std::monostate{}));
    data_type_.push_back(ValueType::ISNULL);
}

void Row::pop()
{
    data_.pop_back();
    data_type_.pop_back();
}

// Utils
size_t Row::get_size()
{
    return data_.size();
}