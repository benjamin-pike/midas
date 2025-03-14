#ifndef ACTIVE_ORDER_SERVICE_HPP
#define ACTIVE_ORDER_SERVICE_HPP

#include "services/TradeService.hpp"
#include "services/TraderService.hpp"
#include "database/Database.hpp"
#include "matcher/MatchingEngine.hpp"
#include "events/EventLogger.hpp"
#include "models/Order.hpp"

#include <memory>

class ActiveOrderService {
public:
    ActiveOrderService(std::shared_ptr<Database> database,
                       std::shared_ptr<EventLogger> eventLogger,
                       std::shared_ptr<TradeService> tradeService,
                       std::shared_ptr<TraderService> traderService);
    ~ActiveOrderService() = default;
    
    std::shared_ptr<Order> getOrder(int orderId) const;
    std::shared_ptr<Order> addOrder(const Order &order);
    bool cancelOrder(int orderId);
    bool modifyOrder(int orderId, double newPrice, int newQuantity);

    std::vector<std::shared_ptr<Order>> getBids(int start, int limit) const;
    std::vector<std::shared_ptr<Order>> getAsks(int start, int limit) const;
    OrderCounts countOrdersForTrader(const std::string &traderId) const;
    std::shared_ptr<Order> getBestBid() const;
    std::shared_ptr<Order> getBestAsk() const;
private:
    std::shared_ptr<Database> database;
    std::shared_ptr<EventLogger> eventLogger;
    std::shared_ptr<TradeService> tradeService;
    std::shared_ptr<TraderService> traderService;
    
    OrderQueueManager orderQueueManager;
    MatchingEngine matchingEngine;
};

#endif
