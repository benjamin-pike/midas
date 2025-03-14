#include "matcher/FOKMatchingStrategy.hpp"
#include "matcher/DefaultMatchingStrategy.hpp"

#include <algorithm>

bool FOKMatchingStrategy::isOrderFillable(const Order &order,
                                          OrderQueueManager &orderQueueManager) const
{
    int requiredQty = order.getInitialQuantity();
    int accumulatedQty = 0;

    const auto &oppositeQueue = (order.getSide() == OrderSide::BID)
                                    ? orderQueueManager.getQueue(OrderSide::ASK)
                                    : orderQueueManager.getQueue(OrderSide::BID);
    OrderQueue tempQueue = oppositeQueue;

    while (!tempQueue.empty() && accumulatedQty < requiredQty)
    {
        auto opposingOrder = tempQueue.top();
        tempQueue.pop();
        if (order.getTraderId() == opposingOrder->getTraderId())
            continue;
        if (order.getSide() == OrderSide::BID)
        {
            if (opposingOrder->getPrice() > order.getPrice())
                break;
        }
        else
        {
            if (opposingOrder->getPrice() < order.getPrice())
                break;
        }
        accumulatedQty += opposingOrder->getRemainingQuantity();
    }
    return accumulatedQty >= requiredQty;
}

void FOKMatchingStrategy::match(Order &incomingOrder,
                                OrderQueueManager &orderQueueManager,
                                std::vector<std::shared_ptr<Order>> &updatedOrders,
                                TradeService &tradeService,
                                EventLogger &eventLogger) const
{
    if (!isOrderFillable(incomingOrder, orderQueueManager))
    {
        incomingOrder.setStatus(OrderStatus::CANCELLED);
        updatedOrders.push_back(std::make_shared<Order>(incomingOrder));
        eventLogger.logEvent(std::make_shared<OrderEvent>(EventType::ORDER_CANCELLED, incomingOrder, generateFOKOrderRejectedMessage(incomingOrder)));
        return;
    }

    DefaultMatchingStrategy defaultStrategy;
    defaultStrategy.match(incomingOrder, orderQueueManager, updatedOrders, tradeService, eventLogger);
}
