#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>
#include <vector>

#define NOW std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

enum class OrderType
{
    LIMIT,
    MARKET,
    IOC,
    FOK,
    ICEBERG,
    STOP,
    STOP_LIMIT,
    TRAILING_STOP,
};

enum class OrderSide
{
    ASK,
    BID
};

enum class OrderStatus
{
    UNFILLED,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED
};

struct OrderCounts
{
    int bids = 0;
    int asks = 0;
};

class Order
{
public:
    Order(
        OrderType type,
        OrderSide side,
        int quantity,
        const std::string &traderId,

        double price = -1,
        int displaySize = -1,
        int hiddenQuantity = -1,
        int limitPrice = -1,
        int bestPrice = -1)
        : type(type),
          side(side),
          initialQuantity(quantity),
          remainingQuantity(type == OrderType::ICEBERG ? displaySize : quantity),
          traderId(traderId),
          price(price),
          displaySize(displaySize),
          hiddenQuantity(hiddenQuantity),
          limitPrice(limitPrice),
          bestPrice(bestPrice),
          timestamp(NOW)
    {
    }

    int getId() const { return id; }
    OrderType getType() const { return type; }
    OrderSide getSide() const { return side; }
    OrderStatus getStatus() const { return status; }
    int getInitialQuantity() const { return initialQuantity; }
    int getRemainingQuantity() const { return remainingQuantity; }
    std::string getTraderId() const { return traderId; }
    long long getTimestamp() const { return timestamp; }

    double getPrice() const { return price; }
    double getLimitPrice() const { return limitPrice; }
    double getBestPrice() const { return bestPrice; }
    int getHiddenQuantity() const { return hiddenQuantity; }
    int getDisplaySize() const { return displaySize; }

    void setId(int newId) { id = newId; }
    void setStatus(OrderStatus newStatus) { status = newStatus; }
    void setPrice(double newPrice) { price = newPrice; }
    void setInitialQuantity(int newQuantity)
    {
        initialQuantity = newQuantity;
        remainingQuantity = newQuantity;
    }
    void setRemainingQuantity(int newQuantity) { remainingQuantity = newQuantity; }
    void setLimitPrice(double lp) { limitPrice = lp; }
    void setBestPrice(double bp) { bestPrice = bp; }
    void setHiddenQuantity(int qty) { hiddenQuantity = qty; }
    void setDisplaySize(int size) { displaySize = size; }
    void setTimestamp(long long ts) { timestamp = ts; }

private:
    int id = -1;
    OrderType type;
    OrderSide side;
    OrderStatus status = OrderStatus::UNFILLED;
    int initialQuantity;
    int remainingQuantity;
    std::string traderId;
    long long timestamp;

    double price = -1;
    int hiddenQuantity = -1;
    int displaySize = -1;

    double limitPrice = 0.0;
    double bestPrice = 0.0;
};

class MarketOrder : public Order
{
public:
    MarketOrder(OrderSide side, int quantity, const std::string &traderId)
        : Order(OrderType::MARKET, side, quantity, traderId) {}
};

class LimitOrder : public Order
{
public:
    LimitOrder(OrderSide side, int quantity, const std::string &traderId, double price)
        : Order(OrderType::LIMIT, side, quantity, traderId, price) {}
};

class IOCOrder : public Order
{
public:
    IOCOrder(OrderSide side, int quantity, const std::string &traderId, double price)
        : Order(OrderType::IOC, side, quantity, traderId, price) {}
};

class FOKOrder : public Order
{
public:
    FOKOrder(OrderSide side, int quantity, const std::string &traderId, double price)
        : Order(OrderType::FOK, side, quantity, traderId, price) {}
};

class IcebergOrder : public Order
{
public:
    IcebergOrder(OrderSide side, int quantity, const std::string &traderId, double price, int displaySize, int hiddenQuantity)
        : Order(OrderType::ICEBERG, side, quantity, traderId, price, displaySize, hiddenQuantity) {}
};

class StopOrder : public Order
{
public:
    StopOrder(OrderSide side, int quantity, const std::string &traderId, double price)
        : Order(OrderType::STOP, side, quantity, traderId, price) {}
};

class StopLimitOrder : public Order
{
public:
    StopLimitOrder(OrderSide side, int quantity, const std::string &traderId, double price, double limitPrice)
        : Order(OrderType::STOP_LIMIT, side, quantity, traderId, price, -1, -1, limitPrice) {}
};

class TrailingStopOrder : public Order
{
public:
    TrailingStopOrder(OrderSide side, int quantity, const std::string &traderId, double price, double bestPrice)
        : Order(OrderType::TRAILING_STOP, side, quantity, traderId, price, -1, -1, -1, bestPrice) {}
};

#endif
