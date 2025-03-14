#ifndef ORDER_QUEUE_MANAGER_HPP
#define ORDER_QUEUE_MANAGER_HPP

#include "models/Order.hpp"

#include <queue>
#include <unordered_map>
#include <memory>
#include <vector>

struct OrderComparator
{
    bool operator()(const std::shared_ptr<Order> &lhs,
                    const std::shared_ptr<Order> &rhs) const
    {
        if (lhs->getSide() != rhs->getSide())
            throw std::runtime_error("Comparing orders with different sides");
        if (lhs->getSide() == OrderSide::ASK)
        {
            if (lhs->getPrice() == rhs->getPrice())
                return lhs->getId() > rhs->getId();
            return lhs->getPrice() > rhs->getPrice();
        }
        if (lhs->getPrice() == rhs->getPrice())
            return lhs->getId() > rhs->getId();
        return lhs->getPrice() < rhs->getPrice();
    }
};

using OrderQueue = std::priority_queue<std::shared_ptr<Order>,
                                       std::vector<std::shared_ptr<Order>>,
                                       OrderComparator>;

class OrderQueueManager
{
public:
    OrderQueueManager() = default;

    void addOrder(const std::shared_ptr<Order> &order);
    void storeOrder(const std::shared_ptr<Order> &order);
    void enqueueOrder(const std::shared_ptr<Order> &order);
    void removeOrder(int orderId, OrderSide side);
    OrderQueue &getQueue(OrderSide side);
    std::shared_ptr<Order> getOrder(int orderId) const;
    std::vector<std::shared_ptr<Order>> getOrders(OrderSide side, int start, int limit) const;
    std::shared_ptr<Order> getBestOrder(OrderSide side) const;
    const std::unordered_map<int, std::shared_ptr<Order>> &getOrderMap() const;
    void updateOrderInBook(const std::shared_ptr<Order> &order);

private:
    OrderQueue bidQueue;
    OrderQueue askQueue;
    std::unordered_map<int, std::shared_ptr<Order>> orderMap;
};

#endif
