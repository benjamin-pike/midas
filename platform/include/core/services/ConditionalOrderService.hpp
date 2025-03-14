#ifndef CONDITIONAL_ORDER_SERVICE_HPP
#define CONDITIONAL_ORDER_SERVICE_HPP

#include "events/EventLogger.hpp"
#include "models/Order.hpp"

#include <vector>
#include <memory>

class ConditionalOrderService
{
public:
    ConditionalOrderService(std::shared_ptr<EventLogger> eventLogger)
        : eventLogger(eventLogger) {}
    ~ConditionalOrderService() = default;

    std::shared_ptr<Order> getOrder(int orderId) const;
    std::shared_ptr<Order> addOrder(const Order &order);
    bool cancelOrder(int orderId);

    std::vector<std::shared_ptr<Order>> getBids(int start, int limit) const;
    std::vector<std::shared_ptr<Order>> getAsks(int start, int limit) const;

    std::vector<std::shared_ptr<Order>> triggerOrders(double currentMarketPrice);

    OrderCounts countOrdersForTrader(const std::string &traderId) const;

private:
    std::shared_ptr<EventLogger> eventLogger;
    std::vector<std::shared_ptr<Order>> orders;
};

#endif
