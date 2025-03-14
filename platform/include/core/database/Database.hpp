#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "database/OrderRepository.hpp"
#include "database/TradeRepository.hpp"

#include <sqlite3.h>
#include <memory>

class Database
{
public:
    Database(const std::string &dbFile);
    virtual ~Database()
    {
        if (db)
        {
            sqlite3_close(db);
        }
    }

    virtual std::shared_ptr<OrderRepository> orders() const
    {
        return ordersRepo;
    }

    virtual std::shared_ptr<TradeRepository> trades() const
    {
        return tradesRepo;
    }

private:
    sqlite3 *db = nullptr;
    std::shared_ptr<OrderRepository> ordersRepo;
    std::shared_ptr<TradeRepository> tradesRepo;
};

#endif
