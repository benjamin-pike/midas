#include "OrderBook.hpp"

#include <iostream>

OrderBook::OrderBook(
    std::shared_ptr<Database> database,
    std::shared_ptr<EventLogger> eventLogger,
    std::shared_ptr<MarketService> marketService,
    std::shared_ptr<RiskService> riskService,
    std::shared_ptr<TraderService> traderService)
    : database(database),
      eventLogger(eventLogger),
      marketService(marketService),
      traderService(traderService),
      riskService(riskService),
      tradeService(std::make_shared<TradeService>(database, eventLogger, marketService, traderService)),
      activeOrderService(std::make_unique<ActiveOrderService>(database, eventLogger, tradeService, traderService)),
      conditionalOrderService(std::make_unique<ConditionalOrderService>(eventLogger))
{
}

Order OrderBook::addOrder(Order &order)
{
    if (!riskService->checkOrder(order, marketService->getCurrentPrice()))
    {
        eventLogger->logEvent(std::make_shared<OrderEvent>(EventType::ORDER_REJECTED, order, generateOrderRejectedMessage(order)));
        throw std::runtime_error("Order rejected due to risk limits");
    }

    auto trader = traderService->getTrader(order.getTraderId());
    if (order.getSide() == OrderSide::ASK)
    {
        if (!trader->placeOrder(order))
            throw std::runtime_error("Trader " + order.getTraderId() + ": Insufficient inventory for sale");
    }

    auto orderId = database->orders()->create(order);

    order.setId(orderId);

    if (
        order.getType() == OrderType::STOP ||
        order.getType() == OrderType::STOP_LIMIT ||
        order.getType() == OrderType::TRAILING_STOP)
        conditionalOrderService->addOrder(order);
    else
    {
        activeOrderService->addOrder(order);
    }

    auto triggeredOrders = conditionalOrderService->triggerOrders(marketService->getCurrentPrice());
    for (const auto &triggeredOrder : triggeredOrders)
    {
        if (triggeredOrder->getType() == OrderType::STOP_LIMIT)
        {
            auto payload = LimitOrder(triggeredOrder->getSide(), triggeredOrder->getInitialQuantity(), triggeredOrder->getTraderId(), triggeredOrder->getLimitPrice());
            auto triggeredOrder = addOrder(payload);
        }
        else
        {
            auto payload = MarketOrder(triggeredOrder->getSide(), triggeredOrder->getInitialQuantity(), triggeredOrder->getTraderId());
            auto triggeredOrder = addOrder(payload);
        }
    }

    return order;
}

std::vector<Order> OrderBook::getActiveAsks(int start, int limit) const
{
    std::vector<Order> result;
    auto asksPtr = activeOrderService->getAsks(start, limit);
    for (const auto &orderPtr : asksPtr)
        result.push_back(*orderPtr);
    return result;
}

std::vector<Order> OrderBook::getActiveBids(int start, int limit) const
{
    std::vector<Order> result;
    auto bidsPtr = activeOrderService->getBids(start, limit);
    for (const auto &orderPtr : bidsPtr)
        result.push_back(*orderPtr);
    return result;
}

std::vector<Order> OrderBook::getConditionalAsks(int start, int limit) const
{
    std::vector<Order> result;
    auto asksPtr = conditionalOrderService->getAsks(start, limit);
    for (const auto &orderPtr : asksPtr)
        result.push_back(*orderPtr);
    return result;
}

std::vector<Order> OrderBook::getConditionalBids(int start, int limit) const
{
    std::vector<Order> result;
    auto bidsPtr = conditionalOrderService->getBids(start, limit);
    for (const auto &orderPtr : bidsPtr)
        result.push_back(*orderPtr);
    return result;
}

std::vector<Trade> OrderBook::getTrades(int start, int limit) const
{
    return tradeService->getTrades(start, limit);
}

bool OrderBook::cancelOrder(int orderId)
{
    try
    {
        auto order = activeOrderService->getOrder(orderId);
        if (activeOrderService->cancelOrder(orderId))
        {
            order->setStatus(OrderStatus::CANCELLED);
            database->orders()->update(*order);
            return true;
        }
    }
    catch (std::runtime_error &)
    {
        // Order not found in active orders.
    }
    try
    {
        auto order = conditionalOrderService->getOrder(orderId);
        if (conditionalOrderService->cancelOrder(orderId))
        {
            order->setStatus(OrderStatus::CANCELLED);
            database->orders()->update(*order);
            return true;
        }
    }
    catch (std::runtime_error &)
    {
        // Order not found.
    }

    return false;
}

bool OrderBook::modifyOrder(int orderId, double newPrice, int newQuantity)
{
    auto oldOrder = activeOrderService->getOrder(orderId);

    Order modifiedAttempt(oldOrder->getType(), oldOrder->getSide(), newQuantity, oldOrder->getTraderId(), newPrice);
    if (!riskService->checkOrder(modifiedAttempt, marketService->getCurrentPrice()))
        return false;

    bool result = activeOrderService->modifyOrder(orderId, newPrice, newQuantity);
    if (!result)
        return false;

    auto modifiedOrder = activeOrderService->getOrder(orderId);
    database->orders()->update(*modifiedOrder);

    return true;
}

void OrderBook::updateMarketPrice(double currentMarketPrice, double volatility)
{
    auto triggered = conditionalOrderService->triggerOrders(currentMarketPrice);
    for (const auto &order : triggered)
    {
        if (order->getType() == OrderType::STOP_LIMIT)
        {
            auto payload = LimitOrder(order->getSide(), order->getInitialQuantity(), order->getTraderId(), order->getLimitPrice());
            auto triggeredOrder = addOrder(payload);
        }
        else
        {
            auto payload = MarketOrder(order->getSide(), order->getInitialQuantity(), order->getTraderId());
            auto triggeredOrder = addOrder(payload);
        }
    }
}

Order OrderBook::getBestBid() const
{
    return *activeOrderService->getBestBid();
}

Order OrderBook::getBestAsk() const
{
    return *activeOrderService->getBestAsk();
}

MarketData OrderBook::getMarketData() const
{
    auto asks = getActiveAsks();
    auto bids = getActiveBids();

    MarketData data;
    data.marketPrice = marketService->getCurrentPrice();
    data.volatility = marketService->getVolatility();

    try
    {
        data.asks.best = getBestAsk();
    }
    catch (std::runtime_error &)
    {
        data.asks.best = std::nullopt;
    }

    try
    {
        data.bids.best = getBestBid();
    }
    catch (std::runtime_error &)
    {
        data.bids.best = std::nullopt;
    }

    data.asks.count = asks.size();
    data.bids.count = bids.size();

    data.asks.volume = 0;
    for (const auto &order : asks)
        data.asks.volume += order.getRemainingQuantity();

    data.bids.volume = 0;
    for (const auto &order : bids)
        data.bids.volume += order.getRemainingQuantity();

    data.trades.count = tradeService->getTrades().size();
    data.trades.volume = 0;
    for (const auto &trade : tradeService->getTrades())
        data.trades.volume += trade.getQuantity();
    double totalTradeValue = 0;
    for (const auto &trade : tradeService->getTrades())
        totalTradeValue += trade.getQuantity() * trade.getPrice();
    data.trades.avgPrice = data.trades.volume > 0 ? totalTradeValue / data.trades.volume : -1;

    return data;
}

OrderCounts OrderBook::countOrdersForTrader(const std::string &traderId) const
{
    auto activeCounts = activeOrderService->countOrdersForTrader(traderId);
    auto conditionalCounts = conditionalOrderService->countOrdersForTrader(traderId);

    OrderCounts counts;
    counts.bids = activeCounts.bids + conditionalCounts.bids;
    counts.asks = activeCounts.asks + conditionalCounts.asks;

    return counts;
}
