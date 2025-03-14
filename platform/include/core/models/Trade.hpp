#ifndef TRADE_HPP
#define TRADE_HPP

#include "models/Order.hpp"

class Trade
{
public:
    Trade(
        int buyOrderId,
        int sellOrderId,
        OrderType buyOrderType,
        OrderType sellOrderType,
        int quantity,
        double price)
        : buyOrderId(buyOrderId),
          sellOrderId(sellOrderId),
          buyOrderType(buyOrderType),
          sellOrderType(sellOrderType),
          quantity(quantity),
          price(price),
          timestamp(NOW) {}

    int getId() const { return id; }
    int getBuyOrderId() const { return buyOrderId; }
    int getSellOrderId() const { return sellOrderId; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    long long getTimestamp() const { return timestamp; }

    OrderType getBuyOrderType() const { return buyOrderType; }
    OrderType getSellOrderType() const { return sellOrderType; }

    void setId(int tradeId) { id = tradeId; }
    void setTimestamp(long long ts) { timestamp = ts; }
    void setBuyOrderType(OrderType type) { buyOrderType = type; }
    void setSellOrderType(OrderType type) { sellOrderType = type; }

private:
    int id = -1;
    int buyOrderId;
    int sellOrderId;
    int quantity;
    double price;
    long long timestamp;

    OrderType buyOrderType;
    OrderType sellOrderType;
};

#endif
