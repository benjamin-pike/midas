#ifndef MATCHING_ENGINE_HPP
#define MATCHING_ENGINE_HPP

#include "services/TradeService.hpp"
#include "events/EventLogger.hpp"
#include "matcher/MatchingStrategy.hpp"
#include "models/Order.hpp"

#include <memory>

class MatchingEngine
{
public:
    MatchingEngine() = default;
    void matchOrder(Order &incomingOrder,
                    OrderQueueManager &orderQueueManager,
                    TradeService &tradeService,
                    EventLogger &eventLogger,
                    std::vector<std::shared_ptr<Order>> &updatedOrders) const;

private:
    std::shared_ptr<MatchingStrategy> getStrategyForOrder(const Order &order) const;
};

#endif
