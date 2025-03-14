#ifndef IOC_MATCHING_STRATEGY_HPP
#define IOC_MATCHING_STRATEGY_HPP

#include "matcher/MatchingStrategy.hpp"

class IOCMatchingStrategy : public MatchingStrategy
{
public:
    void match(Order &incomingOrder,
               OrderQueueManager &orderQueueManager,
               std::vector<std::shared_ptr<Order>> &updatedOrders,
               TradeService &tradeService,
               EventLogger &eventLogger) const override;
};

#endif
