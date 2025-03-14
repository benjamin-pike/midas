#ifndef TRADE_MAPPING_HPP
#define TRADE_MAPPING_HPP

#include "database/BaseMapping.hpp"
#include "models/Trade.hpp"

#include <vector>
#include <memory>
#include <stdexcept>

class TradeRecord
{
public:
    TradeRecord() {}

    static TradeRecord fromTrade(const Trade &trade)
    {
        TradeRecord record;

        record.setId(trade.getId());
        record.setBuyOrderId(trade.getBuyOrderId());
        record.setSellOrderId(trade.getSellOrderId());
        record.setQuantity(trade.getQuantity());
        record.setPrice(trade.getPrice());
        record.setTimestamp(trade.getTimestamp());
        record.setBuyOrderType(static_cast<int>(trade.getBuyOrderType()));
        record.setSellOrderType(static_cast<int>(trade.getSellOrderType()));

        return record;
    }

    std::shared_ptr<Trade> toTrade() const
    {
        auto trade = std::make_shared<Trade>(buyOrderId,
                                             sellOrderId,
                                             static_cast<OrderType>(buyOrderType),
                                             static_cast<OrderType>(sellOrderType),
                                             quantity,
                                             price);
        trade->setId(id);
        trade->setTimestamp(timestamp);

        return trade;
    }

    static constexpr const char *tableName = "trades";
    static const std::vector<FieldDescriptor<TradeRecord>> fields;

    static const std::vector<std::string> joins;
    static const std::vector<std::string> joinSelects;
    static const std::vector<FieldDescriptor<TradeRecord>> joinFields;

    int getId() const { return id; }
    int getBuyOrderId() const { return buyOrderId; }
    int getSellOrderId() const { return sellOrderId; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    long long getTimestamp() const { return timestamp; }
    int getBuyOrderType() const { return static_cast<int>(buyOrderType); }
    int getSellOrderType() const { return static_cast<int>(sellOrderType); }

    void setId(int value) { id = value; }
    void setBuyOrderId(int value) { buyOrderId = value; }
    void setSellOrderId(int value) { sellOrderId = value; }
    void setQuantity(int value) { quantity = value; }
    void setPrice(double value) { price = value; }
    void setTimestamp(long long value) { timestamp = value; }
    void setBuyOrderType(int value) { buyOrderType = static_cast<OrderType>(value); }
    void setSellOrderType(int value) { sellOrderType = static_cast<OrderType>(value); }

private:
    int id;
    int buyOrderId;
    int sellOrderId;
    int quantity;
    double price;
    long long timestamp;
    OrderType buyOrderType;
    OrderType sellOrderType;
};

#endif
