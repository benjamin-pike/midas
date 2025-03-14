#ifndef BASE_REPOSITORY_HPP
#define BASE_REPOSITORY_HPP

#include "database/BaseMapping.hpp"
#include <sqlite3.h>
#include <stdexcept>
#include <string>
#include <vector>

template <typename T>
class BaseRepository
{
public:
    BaseRepository(sqlite3 *connection) : db(connection) {}
    
    int createRecord(const T &entity)
    {
        std::string sql = BaseMapping<T>::generateInsertSQL(T::tableName, T::fields);
        sqlite3_stmt *stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error("Prepare error in create: " + std::string(sqlite3_errmsg(db)));
        }
        int index = 1;
        for (const auto &field : T::fields)
        {
            field.bindFunc(stmt, index++, entity);
        }
        int newId = -1;
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            newId = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
        if (newId == -1)
        {
            throw std::runtime_error("Failed to retrieve new id");
        }
        return newId;
    }

    void updateRecord(const T &entity)
    {
        std::string sql = BaseMapping<T>::generateUpdateSQL(T::tableName, T::fields);
        sqlite3_stmt *stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error("Prepare error in update: " + std::string(sqlite3_errmsg(db)));
        }
        int index = 1;
        for (const auto &field : T::fields)
        {
            field.bindFunc(stmt, index++, entity);
        }

        sqlite3_bind_int(stmt, index, entity.getId());
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Update failed: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_finalize(stmt);
    }

    std::vector<T> getAllRecords(const std::string &whereClause = "")
    {
        std::string sql = BaseMapping<T>::generateSelectSQL(
            T::tableName,
            T::fields,
            T::joins,
            T::joinSelects,
            whereClause);
        sqlite3_stmt *stmt = nullptr;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error("Prepare error in getAll: " + std::string(sqlite3_errmsg(db)));
        }
        std::vector<T> results;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            T entity;
            entity.setId(sqlite3_column_int(stmt, 0));
            int index = 1;
            for (const auto &field : T::fields)
            {
                field.extractFunc(stmt, index++, entity);
            }
            for (const auto &joinField : T::joinFields)
            {
                joinField.extractFunc(stmt, index++, entity);
            }
            results.push_back(entity);
        }
        sqlite3_finalize(stmt);
        return results;
    }

private:
    sqlite3 *db;
};

#endif