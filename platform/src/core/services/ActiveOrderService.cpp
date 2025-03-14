#include "services/ActiveOrderService.hpp"
#include "order-utils.hpp"

#include <stdexcept>

ActiveOrderService::ActiveOrderService(std::shared_ptr<Database> database,
                                       std::shared_ptr<EventLogger> eventLogger,
                                       std::shared_ptr<TradeService> tradeService,
                                       std::shared_ptr<TraderService> traderService)
    : database(database),
      eventLogger(eventLogger),
      tradeService(tradeService),
      traderService(traderService)
{
    auto orders = database->orders()->getAllActive();
    for (const auto &order : orders)
        orderQueueManager.addOrder(order);
}

std::shared_ptr<Order> ActiveOrderService::getOrder(int orderId) const
{
    return orderQueueManager.getOrder(orderId);
}

std::shared_ptr<Order> ActiveOrderService::addOrder(const Order &order)
{
    auto orderPtr = std::make_shared<Order>(order);
    orderQueueManager.storeOrder(orderPtr);
    eventLogger->logEvent(std::make_shared<OrderEvent>(EventType::ORDER_ADDED, *orderPtr, generateOrderAddedMessage(*orderPtr)));

    std::vector<std::shared_ptr<Order>> updatedOrders;
    matchingEngine.matchOrder(*orderPtr, orderQueueManager, *tradeService, *eventLogger, updatedOrders);

    for (const auto &order : updatedOrders)
        database->orders()->update(*order);

    if (isOpenOrder(*orderPtr))
        orderQueueManager.enqueueOrder(orderPtr);

    database->orders()->update(*orderPtr);
    return orderPtr;
}

bool ActiveOrderService::cancelOrder(int orderId)
{
    auto orderPtr = getOrder(orderId);
    if (!orderPtr || orderPtr->getStatus() != OrderStatus::UNFILLED)
        return false;

    orderPtr->setStatus(OrderStatus::CANCELLED);
    orderQueueManager.removeOrder(orderId, orderPtr->getSide());
    eventLogger->logEvent(std::make_shared<OrderEvent>(EventType::ORDER_CANCELLED, *orderPtr, generateOrderCancelledMessage(*orderPtr)));
    database->orders()->update(*orderPtr);

    return true;
}

bool ActiveOrderService::modifyOrder(int orderId, double newPrice, int newQuantity)
{
    auto orderPtr = getOrder(orderId);
    if (!orderPtr || orderPtr->getStatus() != OrderStatus::UNFILLED)
        return false;
    int oldQuantity = orderPtr->getInitialQuantity();
    int quantityDiff = newQuantity - oldQuantity;

    orderPtr->setPrice(newPrice);
    orderPtr->setInitialQuantity(newQuantity);
    orderPtr->setRemainingQuantity(newQuantity);

    orderQueueManager.removeOrder(orderId, orderPtr->getSide());
    orderQueueManager.addOrder(orderPtr);

    eventLogger->logEvent(std::make_shared<OrderEvent>(EventType::ORDER_MODIFIED, *orderPtr, generateOrderModifiedMessage(*orderPtr)));
    database->orders()->update(*orderPtr);
    return true;
}

OrderCounts ActiveOrderService::countOrdersForTrader(const std::string &traderId) const
{
    OrderCounts counts;
    for (const auto &pair : orderQueueManager.getOrderMap())
    {
        const auto &orderPtr = pair.second;
        if (orderPtr->getTraderId() == traderId && orderPtr->getStatus() == OrderStatus::UNFILLED)
        {
            if (orderPtr->getSide() == OrderSide::BID)
                ++counts.bids;
            else
                ++counts.asks;
        }
    }
    return counts;
}

std::vector<std::shared_ptr<Order>> ActiveOrderService::getBids(int start, int limit) const
{
    return orderQueueManager.getOrders(OrderSide::BID, start, limit);
}

std::vector<std::shared_ptr<Order>> ActiveOrderService::getAsks(int start, int limit) const
{
    return orderQueueManager.getOrders(OrderSide::ASK, start, limit);
}

std::shared_ptr<Order> ActiveOrderService::getBestBid() const
{
    return orderQueueManager.getBestOrder(OrderSide::BID);
}

std::shared_ptr<Order> ActiveOrderService::getBestAsk() const
{
    return orderQueueManager.getBestOrder(OrderSide::ASK);
}