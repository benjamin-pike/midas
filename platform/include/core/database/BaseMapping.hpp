#ifndef BASE_MAPPING_HPP
#define BASE_MAPPING_HPP

#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <sqlite3.h>

template <typename T>
struct FieldDescriptor
{
    std::string columnName;
    std::function<void(sqlite3_stmt *, int, const T &)> bindFunc;
    std::function<void(sqlite3_stmt *, int, T &)> extractFunc;
};

template <typename T>
class BaseMapping
{
public:
    static std::string generateInsertSQL(const std::string &tableName,
                                         const std::vector<FieldDescriptor<T>> &fields)
    {
        std::stringstream ss;
        ss << "INSERT INTO " << tableName << " (";
        bool first = true;
        for (const auto &field : fields)
        {
            if (!first)
                ss << ", ";
            ss << field.columnName;
            first = false;
        }
        ss << ") VALUES (";
        first = true;
        for (size_t i = 0; i < fields.size(); i++)
        {
            if (!first)
                ss << ", ";
            ss << "?";
            first = false;
        }
        ss << ") RETURNING id;";
        return ss.str();
    }

    static std::string generateUpdateSQL(const std::string &tableName,
                                         const std::vector<FieldDescriptor<T>> &fields)
    {
        std::stringstream ss;
        ss << "UPDATE " << tableName << " SET ";
        bool first = true;
        for (const auto &field : fields)
        {
            if (!first)
                ss << ", ";
            ss << field.columnName << " = ?";
            first = false;
        }
        ss << " WHERE id = ?;";
        return ss.str();
    }

    static std::string generateSelectSQL(const std::string &tableName,
                                         const std::vector<FieldDescriptor<T>> &fields,
                                         const std::vector<std::string> &joins = {},
                                         const std::vector<std::string> &joinSelects = {},
                                         const std::string &whereClause = "")
    {
        std::stringstream ss;
        ss << "SELECT " << tableName << ".id";
        for (const auto &field : fields)
        {
            ss << ", " << tableName << "." << field.columnName;
        }
        for (const auto &col : joinSelects)
        {
            ss << ", " << col;
        }
        ss << " FROM " << tableName;
        for (const auto &join : joins)
        {
            ss << " " << join;
        }
        if (!whereClause.empty())
        {
            ss << " WHERE " << whereClause;
        }
        ss << ";";
        return ss.str();
    }
};

#endif