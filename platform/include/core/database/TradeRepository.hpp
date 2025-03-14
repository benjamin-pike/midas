#ifndef TRADE_REPOSITORY_HPP
#define TRADE_REPOSITORY_HPP

#include "database/BaseRepository.hpp"
#include "database/TradeMapping.hpp"

class TradeRepository : public BaseRepository<TradeRecord>
{
public:
    TradeRepository(sqlite3 *connection) : BaseRepository<TradeRecord>(connection) {}

    virtual int create(const Trade &trade);
    virtual std::vector<Trade> getAll();
};

#endif