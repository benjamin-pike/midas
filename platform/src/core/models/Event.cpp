#include "models/Event.hpp"
#include "order-utils.hpp"

std::string generateOrderAddedMessage(const Order &order)
{
    std::string message = getOrderTypeString(order.getType(), true) + " " + getOrderSideString(order.getSide()) + " order added (";
    message += std::to_string(order.getInitialQuantity()) + " units";
    if (order.getPrice() > 0)
        message += " @ $" + formatPrice(order.getPrice());
    message += ")";
    return message;
}

std::string generateOrderModifiedMessage(const Order &order)
{
    std::string message = "Order " + std::to_string(order.getId()) + " modified (";
    message += std::to_string(order.getInitialQuantity()) + " units";
    if (order.getPrice() > 0)
        message += " @ $" + formatPrice(order.getPrice());
    message += ")";
    return message;
}

std::string generateOrderCancelledMessage(const Order &order)
{
    return "Order " + std::to_string(order.getId()) + " cancelled";
}

std::string generateOrderRejectedMessage(const Order &order)
{
    return "Order exceeds risk limits and has been rejected";
}

std::string generateOrderTriggeredMessage(const Order &order)
{
    return "Order " + std::to_string(order.getId()) + " triggered";
}

std::string generateIOCOrderCancelledMessage(const Order &order)
{
    std::string message = "IOC " + getOrderSideString(order.getSide()) + " order (ID " + std::to_string(order.getId()) + ") cancelled (";
    message += std::to_string(order.getInitialQuantity() - order.getRemainingQuantity()) + "/" + std::to_string(order.getInitialQuantity()) + " units filled";
    message += ")";
    return message;
}

std::string generateFOKOrderRejectedMessage(const Order &order)
{
    return "FOK order cannot be filled and has been cancelled";
}

std::string generateTradeExecutedMessage(const Trade &trade)
{
    return "Trade executed (" + std::to_string(trade.getQuantity()) + " units @ $" + formatPrice(trade.getPrice()) + " per unit)";
}