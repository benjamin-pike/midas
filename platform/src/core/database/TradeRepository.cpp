#include "database/TradeRepository.hpp"

int TradeRepository::create(const Trade &trade)
{
    TradeRecord record = TradeRecord::fromTrade(trade);
    int newId = this->createRecord(record);
    record.setId(newId);
    return newId;
}

std::vector<Trade> TradeRepository::getAll()
{
    auto records = this->getAllRecords();
    std::vector<Trade> trades;
    for (auto &record : records)
        trades.push_back(*record.toTrade());

    return trades;
}