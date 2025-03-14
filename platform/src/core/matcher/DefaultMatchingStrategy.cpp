#include "matcher/DefaultMatchingStrategy.hpp"
#include "models/Event.hpp"

#include <algorithm>
#include <stdexcept>

bool DefaultMatchingStrategy::isPriceAcceptable(const Order &incomingOrder,
                                                const Order &opposingOrder) const
{
    if (incomingOrder.getPrice() > 0 && opposingOrder.getPrice() > 0)
    {
        if (incomingOrder.getSide() == OrderSide::BID && incomingOrder.getPrice() < opposingOrder.getPrice())
            return false;
        if (incomingOrder.getSide() == OrderSide::ASK && incomingOrder.getPrice() > opposingOrder.getPrice())
            return false;
    }
    return true;
}

void DefaultMatchingStrategy::processNormalOrderMatch(std::shared_ptr<Order> opposingOrderPtr,
                                                      int matchQty,
                                                      int availableQty,
                                                      OrderQueueManager &orderQueueManager,
                                                      std::vector<std::shared_ptr<Order>> &updatedOrders) const
{
    auto &queue = orderQueueManager.getQueue(opposingOrderPtr->getSide());
    queue.pop();
    if (matchQty < availableQty)
    {
        opposingOrderPtr->setRemainingQuantity(availableQty - matchQty);
        opposingOrderPtr->setStatus(OrderStatus::PARTIALLY_FILLED);
        queue.push(opposingOrderPtr);
    }
    else
    {
        opposingOrderPtr->setStatus(OrderStatus::FILLED);
    }
    updatedOrders.push_back(opposingOrderPtr);
}

void DefaultMatchingStrategy::processIcebergOrderMatch(std::shared_ptr<Order> opposingOrderPtr,
                                                       int matchQty, int availableQty,
                                                       OrderQueueManager &orderQueueManager,
                                                       std::vector<std::shared_ptr<Order>> &updatedOrders) const
{
    auto &queue = orderQueueManager.getQueue(opposingOrderPtr->getSide());
    queue.pop();
    if (matchQty == availableQty)
    {
        int hiddenQty = opposingOrderPtr->getHiddenQuantity();
        if (hiddenQty > 0)
        {
            int replenish = std::min(opposingOrderPtr->getDisplaySize(), hiddenQty);

            opposingOrderPtr->setHiddenQuantity(hiddenQty - replenish);
            opposingOrderPtr->setRemainingQuantity(replenish);
            opposingOrderPtr->setStatus(OrderStatus::PARTIALLY_FILLED);

            queue.push(opposingOrderPtr);
        }
        else
        {
            opposingOrderPtr->setStatus(OrderStatus::FILLED);
        }
    }
    else
    {
        opposingOrderPtr->setRemainingQuantity(availableQty - matchQty);
        opposingOrderPtr->setStatus(OrderStatus::PARTIALLY_FILLED);

        queue.push(opposingOrderPtr);
    }
    updatedOrders.push_back(opposingOrderPtr);
}

void DefaultMatchingStrategy::match(Order &incomingOrder,
                                    OrderQueueManager &orderQueueManager,
                                    std::vector<std::shared_ptr<Order>> &updatedOrders,
                                    TradeService &tradeService,
                                    EventLogger &eventLogger) const
{
    int unmatchedQuantity = incomingOrder.getRemainingQuantity();
    std::vector<std::shared_ptr<Order>> skippedOrders;

    auto &oppositeQueue = (incomingOrder.getSide() == OrderSide::BID)
                              ? orderQueueManager.getQueue(OrderSide::ASK)
                              : orderQueueManager.getQueue(OrderSide::BID);

    while (unmatchedQuantity > 0 && !oppositeQueue.empty())
    {
        auto opposingOrderPtr = oppositeQueue.top();

        if (incomingOrder.getTraderId() == opposingOrderPtr->getTraderId())
        {
            oppositeQueue.pop();
            skippedOrders.push_back(opposingOrderPtr);
            continue;
        }

        if (!isPriceAcceptable(incomingOrder, *opposingOrderPtr))
            break;

        int availableQty = opposingOrderPtr->getRemainingQuantity();
        int matchQty = std::min(unmatchedQuantity, availableQty);

        auto &bidOrder = (incomingOrder.getSide() == OrderSide::BID) ? incomingOrder : *opposingOrderPtr;
        auto &askOrder = (incomingOrder.getSide() == OrderSide::BID) ? *opposingOrderPtr : incomingOrder;
        Trade trade = tradeService.addTrade(bidOrder, askOrder, matchQty);

        eventLogger.logEvent(std::make_shared<TradeEvent>(EventType::TRADE_EXECUTED, trade, bidOrder.getTraderId(), askOrder.getTraderId()));
        unmatchedQuantity -= matchQty;

        if (opposingOrderPtr->getType() == OrderType::ICEBERG)
            processIcebergOrderMatch(opposingOrderPtr, matchQty, availableQty, orderQueueManager, updatedOrders);
        else
            processNormalOrderMatch(opposingOrderPtr, matchQty, availableQty, orderQueueManager, updatedOrders);
    }
    for (auto &order : skippedOrders)
    {
        oppositeQueue.push(order);
    }
    incomingOrder.setRemainingQuantity(unmatchedQuantity);
    if (unmatchedQuantity == 0)
        incomingOrder.setStatus(OrderStatus::FILLED);
    else if (unmatchedQuantity < incomingOrder.getInitialQuantity())
        incomingOrder.setStatus(OrderStatus::PARTIALLY_FILLED);

    updatedOrders.push_back(std::make_shared<Order>(incomingOrder));
}
