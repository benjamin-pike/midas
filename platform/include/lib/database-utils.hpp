#ifndef DATABASE_UTILS_HPP
#define DATABASE_UTILS_HPP

#include <optional>
#include <sqlite3.h>

#define INT_FIELD(fieldName, getter, setter) { \
    #fieldName, \
    [](sqlite3_stmt* stmt, int idx, const auto &record) { \
        sqlite3_bind_int(stmt, idx, static_cast<int>(record.getter())); \
    }, \
    [](sqlite3_stmt* stmt, int idx, auto &record) { \
        record.setter(sqlite3_column_int(stmt, idx)); \
    } \
}

#define INT64_FIELD(fieldName, getter, setter) { \
    #fieldName, \
    [](sqlite3_stmt* stmt, int idx, const auto &record) { \
        sqlite3_bind_int64(stmt, idx, record.getter()); \
    }, \
    [](sqlite3_stmt* stmt, int idx, auto &record) { \
        record.setter(sqlite3_column_int64(stmt, idx)); \
    } \
}

#define DOUBLE_FIELD(fieldName, getter, setter) { \
    #fieldName, \
    [](sqlite3_stmt* stmt, int idx, const auto &record) { \
        sqlite3_bind_double(stmt, idx, record.getter()); \
    }, \
    [](sqlite3_stmt* stmt, int idx, auto &record) { \
        record.setter(sqlite3_column_double(stmt, idx)); \
    } \
}

#define TEXT_FIELD(fieldName, getter, setter) { \
    #fieldName, \
    [](sqlite3_stmt* stmt, int idx, const auto &record) { \
        sqlite3_bind_text(stmt, idx, record.getter().c_str(), -1, SQLITE_TRANSIENT); \
    }, \
    [](sqlite3_stmt* stmt, int idx, auto &record) { \
        const unsigned char *text = sqlite3_column_text(stmt, idx); \
        record.setter(text ? reinterpret_cast<const char*>(text) : ""); \
    } \
}

#define OPTIONAL_INT_FIELD(fieldName, getter, setter) { \
    #fieldName, \
    [](sqlite3_stmt* stmt, int idx, const auto &record) { \
        if (record.getter().has_value()) \
            sqlite3_bind_int(stmt, idx, record.getter().value()); \
        else \
            sqlite3_bind_null(stmt, idx); \
    }, \
    [](sqlite3_stmt* stmt, int idx, auto &record) { \
        record.setter(sqlite3_column_int(stmt, idx)); \
    } \
}

#define OPTIONAL_DOUBLE_FIELD(fieldName, getter, setter) { \
    #fieldName, \
    [](sqlite3_stmt* stmt, int idx, const auto &record) { \
        if (record.getter().has_value()) \
            sqlite3_bind_double(stmt, idx, record.getter().value()); \
        else \
            sqlite3_bind_null(stmt, idx); \
    }, \
    [](sqlite3_stmt* stmt, int idx, auto &record) { \
        record.setter(sqlite3_column_double(stmt, idx)); \
    } \
}

#endif
