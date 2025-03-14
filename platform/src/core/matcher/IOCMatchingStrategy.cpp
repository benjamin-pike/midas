#include "matcher/IOCMatchingStrategy.hpp"
#include "matcher/DefaultMatchingStrategy.hpp"

#include <memory>

void IOCMatchingStrategy::match(Order &incomingOrder,
                                OrderQueueManager &orderQueueManager,
                                std::vector<std::shared_ptr<Order>> &updatedOrders,
                                TradeService &tradeService,
                                EventLogger &eventLogger) const
{
    DefaultMatchingStrategy defaultStrategy;
    defaultStrategy.match(incomingOrder, orderQueueManager, updatedOrders, tradeService, eventLogger);

    if (incomingOrder.getRemainingQuantity() > 0)
    {
        incomingOrder.setStatus(OrderStatus::CANCELLED);
        eventLogger.logEvent(std::make_shared<OrderEvent>(EventType::ORDER_CANCELLED, incomingOrder, generateIOCOrderCancelledMessage(incomingOrder)));
    }

    updatedOrders.push_back(std::make_shared<Order>(incomingOrder));
}
