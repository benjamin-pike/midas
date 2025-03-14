#ifndef MATCHING_STRATEGY_HPP
#define MATCHING_STRATEGY_HPP

#include "OrderQueueManager.hpp"
#include "services/TradeService.hpp"
#include "events/EventLogger.hpp"
#include "models/Order.hpp"

#include <vector>
#include <memory>

class MatchingStrategy {
public:
    virtual void match(Order &incomingOrder,
                       OrderQueueManager &orderQueueManager,
                       std::vector<std::shared_ptr<Order>> &updatedOrders,
                       TradeService &tradeService,
                       EventLogger &eventLogger) const = 0;
    virtual ~MatchingStrategy() = default;
};

#endif
