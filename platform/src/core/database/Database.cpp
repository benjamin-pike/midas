#include "database/Database.hpp"

#include <string>
#include <stdexcept>

#define CREATE_ORDERS_TABLE_SQL \
    "CREATE TABLE IF NOT EXISTS orders (" \
    "id INTEGER PRIMARY KEY, " \
    "type INTEGER, " \
    "side INTEGER, " \
    "status INTEGER, " \
    "initialQuantity INTEGER, " \
    "remainingQuantity INTEGER, " \
    "traderId TEXT, " \
    "price REAL, " \
    "limitPrice REAL, " \
    "bestPrice REAL, " \
    "displaySize INTEGER, " \
    "hiddenQuantity INTEGER, " \
    "timestamp INTEGER" \
    ");"

#define CREATE_TRADES_TABLE_SQL \
    "CREATE TABLE IF NOT EXISTS trades (" \
    "id INTEGER PRIMARY KEY AUTOINCREMENT, " \
    "buyOrderId INTEGER, " \
    "sellOrderId INTEGER, " \
    "quantity INTEGER, " \
    "price REAL, " \
    "timestamp INTEGER" \
    ");"


Database::Database(const std::string &dbFile)
{
    if (sqlite3_open(dbFile.c_str(), &db))
    {
        throw std::runtime_error("Failed to open database");
    }

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, CREATE_ORDERS_TABLE_SQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to create orders table: " + error);
    }

    rc = sqlite3_exec(db, CREATE_TRADES_TABLE_SQL, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to create trades table: " + error);
    }

    ordersRepo = std::make_shared<OrderRepository>(db);
    tradesRepo = std::make_shared<TradeRepository>(db);
}