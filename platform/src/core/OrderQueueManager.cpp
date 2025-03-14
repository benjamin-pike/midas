#include "OrderQueueManager.hpp"

void OrderQueueManager::addOrder(const std::shared_ptr<Order> &order)
{
    orderMap.emplace(order->getId(), order);
    if (order->getSide() == OrderSide::BID)
        bidQueue.push(order);
    else
        askQueue.push(order);
}

void OrderQueueManager::storeOrder(const std::shared_ptr<Order> &order)
{
    orderMap.emplace(order->getId(), order);
}

void OrderQueueManager::enqueueOrder(const std::shared_ptr<Order> &order)
{
    if (order->getSide() == OrderSide::BID)
        bidQueue.push(order);
    else
        askQueue.push(order);
}

void OrderQueueManager::removeOrder(int orderId, OrderSide side)
{
    OrderQueue newQueue;
    OrderQueue &queue = (side == OrderSide::BID) ? bidQueue : askQueue;
    while (!queue.empty())
    {
        auto ord = queue.top();
        queue.pop();
        if (ord->getId() != orderId)
            newQueue.push(ord);
    }
    queue = std::move(newQueue);
    orderMap.erase(orderId);
}

OrderQueue &OrderQueueManager::getQueue(OrderSide side)
{
    return (side == OrderSide::BID) ? bidQueue : askQueue;
}

std::shared_ptr<Order> OrderQueueManager::getOrder(int orderId) const
{
    auto it = orderMap.find(orderId);
    if (it != orderMap.end())
        return it->second;
    throw std::runtime_error("Order not found");
}

std::vector<std::shared_ptr<Order>> OrderQueueManager::getOrders(OrderSide side, int start, int limit) const
{
    std::vector<std::shared_ptr<Order>> orders;
    OrderQueue queueCopy = (side == OrderSide::BID) ? bidQueue : askQueue;
    int index = 0;
    while (!queueCopy.empty() && (limit == -1 || orders.size() < static_cast<size_t>(limit)))
    {
        auto order = queueCopy.top();
        queueCopy.pop();
        if (index >= start)
            orders.push_back(order);
        ++index;
    }
    return orders;
}

std::shared_ptr<Order> OrderQueueManager::getBestOrder(OrderSide side) const
{
    OrderQueue queueCopy = (side == OrderSide::BID) ? bidQueue : askQueue;
    while (!queueCopy.empty())
    {
        auto topOrder = queueCopy.top();
        if (orderMap.find(topOrder->getId()) != orderMap.end() && topOrder->getPrice() > 0)
            return topOrder;
        queueCopy.pop();
    }
    throw std::runtime_error("No valid order available.");
}

const std::unordered_map<int, std::shared_ptr<Order>> &OrderQueueManager::getOrderMap() const
{
    return orderMap;
}

void OrderQueueManager::updateOrderInBook(const std::shared_ptr<Order> &order)
{
    removeOrder(order->getId(), order->getSide());
    addOrder(order);
}
