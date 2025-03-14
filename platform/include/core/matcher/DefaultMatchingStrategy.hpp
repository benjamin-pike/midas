#ifndef DEFAULT_MATCHING_STRATEGY_HPP
#define DEFAULT_MATCHING_STRATEGY_HPP

#include "matcher/MatchingStrategy.hpp"

class DefaultMatchingStrategy : public MatchingStrategy
{
public:
    void match(Order &incomingOrder,
               OrderQueueManager &orderQueueManager,
               std::vector<std::shared_ptr<Order>> &updatedOrders,
               TradeService &tradeService,
               EventLogger &eventLogger) const override;

private:
    bool isPriceAcceptable(const Order &incomingOrder, const Order &opposingOrder) const;
    void processNormalOrderMatch(std::shared_ptr<Order> opposingOrder,
                                 int matchQty, int availableQty,
                                 OrderQueueManager &orderQueueManager,
                                 std::vector<std::shared_ptr<Order>> &updatedOrders) const;
    void processIcebergOrderMatch(std::shared_ptr<Order> opposingOrder,
                                  int matchQty, int availableQty,
                                  OrderQueueManager &orderQueueManager,
                                  std::vector<std::shared_ptr<Order>> &updatedOrders) const;
};

#endif
