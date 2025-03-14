#include "order-utils.hpp"

#include <iomanip>
#include <sstream>

bool isActiveOrder(const Order &order)
{
    return order.getType() == OrderType::LIMIT ||
           order.getType() == OrderType::MARKET ||
           order.getType() == OrderType::IOC ||
           order.getType() == OrderType::FOK ||
           order.getType() == OrderType::ICEBERG;
}

bool isConditionalOrder(const Order &order)
{
    return order.getType() == OrderType::STOP_LIMIT ||
           order.getType() == OrderType::STOP ||
           order.getType() == OrderType::TRAILING_STOP;
}

bool isOpenOrder(const Order &order)
{
    return order.getStatus() == OrderStatus::UNFILLED ||
           order.getStatus() == OrderStatus::PARTIALLY_FILLED;
}

std::string getOrderTypeString(OrderType orderType, bool titleCase)
{
    switch (orderType)
    {
    case OrderType::LIMIT:
        return titleCase ? "Limit" : "LIMIT";
    case OrderType::MARKET:
        return titleCase ? "Market" : "MARKET";
    case OrderType::IOC:
        return titleCase ? "IOC" : "IOC";
    case OrderType::FOK:
        return titleCase ? "FOK" : "FOK";
    case OrderType::STOP:
        return titleCase ? "Stop" : "STOP";
    case OrderType::STOP_LIMIT:
        return titleCase ? "Stop Limit" : "STOP_LIMIT";
    case OrderType::TRAILING_STOP:
        return titleCase ? "Trailing Stop" : "TRAILING_STOP";
    case OrderType::ICEBERG:
        return titleCase ? "Iceberg" : "ICEBERG";
    default:
        return "UNKNOWN";
    }
}

std::string getOrderSideString(OrderSide orderSide)
{
    return (orderSide == OrderSide::BID) ? "BID" : "ASK";
}

std::string getOrderStatusString(OrderStatus orderStatus)
{
    switch (orderStatus)
    {
    case OrderStatus::UNFILLED:
        return "UNFILLED";
    case OrderStatus::PARTIALLY_FILLED:
        return "PARTIALLY_FILLED";
    case OrderStatus::FILLED:
        return "FILLED";
    case OrderStatus::CANCELLED:
        return "CANCELLED";
    default:
        return "UNKNOWN";
    }
}

std::string formatPrice(double price)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << price;
    return stream.str();
}