#ifndef EVENT_HPP
#define EVENT_HPP

#include "models/Order.hpp"
#include "models/Trade.hpp"
#include "models/Risk.hpp"

#include <string>
#include <variant>

std::string generateOrderAddedMessage(const Order &order);
std::string generateOrderModifiedMessage(const Order &order);
std::string generateOrderCancelledMessage(const Order &order);
std::string generateOrderRejectedMessage(const Order &order);
std::string generateOrderTriggeredMessage(const Order &order);
std::string generateIOCOrderCancelledMessage(const Order &order);
std::string generateFOKOrderRejectedMessage(const Order &order);
std::string generateTradeExecutedMessage(const Trade &trade);

enum class EventType
{
    ORDER_ADDED,
    ORDER_MODIFIED,
    ORDER_CANCELLED,
    ORDER_REJECTED,
    ORDER_TRIGGERED,
    TRADE_EXECUTED,
    RISK_UPDATED
};

class Event
{
public:
    explicit Event(EventType type, const std::string message) : eventType(type), message(message) {}
    virtual ~Event() = default;

    EventType getType() const { return eventType; }
    const std::string &getMessage() const { return message; }

private:
    EventType eventType;
    std::string message;
};

class OrderEvent : public Event
{
public:
    OrderEvent(EventType type, Order order, std::string message)
        : Event(type, message), order(order) {}

    Order getOrder() const { return order; }

private:
    Order order;
};

class TradeEvent : public Event
{
public:
    TradeEvent(EventType type, Trade trade, const std::string &buyTraderId, const std::string &sellTraderId)
        : Event(type, generateTradeExecutedMessage(trade)), trade(trade), buyTraderId(buyTraderId), sellTraderId(sellTraderId) {}

    Trade getTrade() const { return trade; }
    std::string getBuyTraderId() const { return buyTraderId; }
    std::string getSellTraderId() const { return sellTraderId; }

private:
    Trade trade;
    std::string buyTraderId;
    std::string sellTraderId;
};

class RiskEvent : public Event
{
public:
    static constexpr const char *message = "Risk limits updated";

    enum class Scope
    {
        GLOBAL,
        TRADER
    };

    RiskEvent(EventType type,
              Scope scope,
              const RiskLimits &update,
              const std::string &traderId)
        : Event(type, message),
          scope(scope),
          update(update),
          traderId(traderId),
          override(false) {}

    RiskEvent(EventType type,
              Scope scope,
              const RiskLimits &update,
              bool override)
        : Event(type, message),
          scope(scope),
          update(update),
          traderId(""),
          override(override) {}

    Scope getScope() const { return scope; }
    const std::string &getTraderId() const { return traderId; }
    bool isOverride() const { return override; }
    const RiskLimits &getUpdate() const { return update; }

private:
    Scope scope;
    std::string traderId;
    bool override;
    RiskLimits update;
};

#endif