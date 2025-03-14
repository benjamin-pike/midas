#include "services/ConditionalOrderService.hpp"

#include <algorithm>
#include <iostream>

std::shared_ptr<Order> ConditionalOrderService::getOrder(int orderId) const
{
    auto it = std::find_if(orders.begin(), orders.end(),
                           [orderId](const std::shared_ptr<Order> &o)
                           { return o->getId() == orderId; });
    if (it != orders.end())
        return *it;
    throw std::runtime_error("Conditional order not found");
}

std::shared_ptr<Order> ConditionalOrderService::addOrder(const Order &order)
{
    auto orderPtr = std::make_shared<Order>(order);
    orders.push_back(orderPtr);

    eventLogger->logEvent(std::make_shared<OrderEvent>(EventType::ORDER_ADDED, *orderPtr, generateOrderAddedMessage(*orderPtr)));

    return orderPtr;
}

bool ConditionalOrderService::cancelOrder(int orderId)
{
    auto it = std::find_if(orders.begin(), orders.end(),
                           [orderId](const std::shared_ptr<Order> &o)
                           { return o->getId() == orderId; });
    if (it != orders.end())
    {
        auto orderPtr = *it;
        eventLogger->logEvent(std::make_shared<OrderEvent>(EventType::ORDER_CANCELLED, *orderPtr, generateOrderCancelledMessage(*orderPtr)));
        orders.erase(it);
        return true;
    }
    return false;
}

std::vector<std::shared_ptr<Order>> ConditionalOrderService::getBids(int start, int limit) const
{
    std::vector<std::shared_ptr<Order>> bids;
    auto it = orders.begin();
    int index = 0;
    while (it != orders.end() && (limit == -1 || bids.size() < static_cast<size_t>(limit)))
    {
        auto orderPtr = *it;
        if (orderPtr->getSide() == OrderSide::BID)
        {
            if (index >= start)
                bids.push_back(orderPtr);
            ++index;
        }
        ++it;
    }
    return bids;
}

std::vector<std::shared_ptr<Order>> ConditionalOrderService::getAsks(int start, int limit) const
{
    std::vector<std::shared_ptr<Order>> asks;
    auto it = orders.begin();
    int index = 0;
    while (it != orders.end() && (limit == -1 || asks.size() < static_cast<size_t>(limit)))
    {
        auto orderPtr = *it;
        if (orderPtr->getSide() == OrderSide::ASK)
        {
            if (index >= start)
                asks.push_back(orderPtr);
            ++index;
        }
        ++it;
    }
    return asks;
}

std::vector<std::shared_ptr<Order>> ConditionalOrderService::triggerOrders(double currentMarketPrice)
{
    std::vector<std::shared_ptr<Order>> triggeredOrders;
    auto it = orders.begin();
    while (it != orders.end())
    {
        auto &orderPtr = *it;
        bool trigger = false;

        switch (orderPtr->getType())
        {
        case OrderType::STOP:
        {
            if ((orderPtr->getSide() == OrderSide::ASK && currentMarketPrice <= orderPtr->getPrice()) ||
                (orderPtr->getSide() == OrderSide::BID && currentMarketPrice >= orderPtr->getPrice()))
            {
                trigger = true;
            }
            break;
        }
        case OrderType::STOP_LIMIT:
        {
            if ((orderPtr->getSide() == OrderSide::ASK && currentMarketPrice <= orderPtr->getPrice()) ||
                (orderPtr->getSide() == OrderSide::BID && currentMarketPrice >= orderPtr->getPrice()))
            {
                trigger = true;
            }
            break;
        }
        case OrderType::TRAILING_STOP:
        {
            if (orderPtr->getSide() == OrderSide::ASK)
            {
                if (currentMarketPrice > orderPtr->getBestPrice())
                    orderPtr->setBestPrice(currentMarketPrice);
                if (currentMarketPrice <= (orderPtr->getBestPrice() - orderPtr->getPrice()))
                    trigger = true;
            }
            else
            {
                if (currentMarketPrice < orderPtr->getBestPrice())
                    orderPtr->setBestPrice(currentMarketPrice);
                if (currentMarketPrice >= (orderPtr->getBestPrice() + orderPtr->getPrice()))
                    trigger = true;
            }
            break;
        }
        default:
            break;
        }

        if (trigger)
        {
            triggeredOrders.push_back(orderPtr);
            eventLogger->logEvent(std::make_shared<OrderEvent>(EventType::ORDER_TRIGGERED, *orderPtr, generateOrderTriggeredMessage(*orderPtr)));
            it = orders.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return triggeredOrders;
}

OrderCounts ConditionalOrderService::countOrdersForTrader(const std::string &traderId) const
{
    OrderCounts counts;
    for (const auto &orderPtr : orders)
    {
        if (orderPtr->getTraderId() == traderId)
        {
            if (orderPtr->getSide() == OrderSide::BID)
                ++counts.bids;
            else
                ++counts.asks;
        }
    }
    return counts;
}
