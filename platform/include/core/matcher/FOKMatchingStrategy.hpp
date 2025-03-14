#ifndef FOK_MATCHING_STRATEGY_HPP
#define FOK_MATCHING_STRATEGY_HPP

#include "matcher/MatchingStrategy.hpp"

class FOKMatchingStrategy : public MatchingStrategy
{
public:
    void match(Order &incomingOrder,
               OrderQueueManager &orderQueueManager,
               std::vector<std::shared_ptr<Order>> &updatedOrders,
               TradeService &tradeService,
               EventLogger &eventLogger) const override;

private:
    bool isOrderFillable(const Order &order, OrderQueueManager &orderQueueManager) const;
};

#endif
