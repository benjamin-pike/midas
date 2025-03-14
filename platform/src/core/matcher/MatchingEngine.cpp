#include "matcher/MatchingEngine.hpp"
#include "matcher/DefaultMatchingStrategy.hpp"
#include "matcher/FOKMatchingStrategy.hpp"
#include "matcher/IOCMatchingStrategy.hpp"

std::shared_ptr<MatchingStrategy> MatchingEngine::getStrategyForOrder(const Order &order) const
{
    switch (order.getType())
    {
    case OrderType::IOC:
        return std::make_shared<IOCMatchingStrategy>();
    case OrderType::FOK:
        return std::make_shared<FOKMatchingStrategy>();
    default:
        return std::make_shared<DefaultMatchingStrategy>();
    }
}

void MatchingEngine::matchOrder(Order &incomingOrder,
                                OrderQueueManager &orderQueueManager,
                                TradeService &tradeService,
                                EventLogger &eventLogger,
                                std::vector<std::shared_ptr<Order>> &updatedOrders) const
{
    auto strategy = getStrategyForOrder(incomingOrder);
    strategy->match(incomingOrder, orderQueueManager, updatedOrders, tradeService, eventLogger);
}
