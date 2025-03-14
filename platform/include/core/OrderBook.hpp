#ifndef ORDER_BOOK_HPP
#define ORDER_BOOK_HPP

#include "services/ActiveOrderService.hpp"
#include "services/ConditionalOrderService.hpp"
#include "services/RiskService.hpp"
#include "services/TradeService.hpp"
#include "services/TraderService.hpp"
#include "services/MarketService.hpp"
#include "database/Database.hpp"
#include "events/EventLogger.hpp"

#include <vector>
#include <memory>
#include <optional>

struct OrdersData
{
    std::optional<Order> best;
    int count;
    double volume;
};

struct TradesData
{
    int count;
    double volume;
    double avgPrice;
};

struct MarketData
{
    double marketPrice;
    double volatility;
    OrdersData bids;
    OrdersData asks;
    TradesData trades;
};

class OrderBook
{
public:
    OrderBook(
        std::shared_ptr<Database> database,
        std::shared_ptr<EventLogger> eventLogger,
        std::shared_ptr<MarketService> marketService,
        std::shared_ptr<RiskService> riskService,
        std::shared_ptr<TraderService> traderService);
    ~OrderBook() = default;

    Order addOrder(Order &order);
    bool cancelOrder(int orderId);
    bool modifyOrder(int orderId, double newPrice, int newQuantity);

    void updateMarketPrice(double currentMarketPrice, double volatility);

    std::vector<Order> getActiveAsks(int start = 0, int limit = -1) const;
    std::vector<Order> getActiveBids(int start = 0, int limit = -1) const;
    std::vector<Order> getConditionalAsks(int start = 0, int limit = -1) const;
    std::vector<Order> getConditionalBids(int start = 0, int limit = -1) const;
    std::vector<Trade> getTrades(int start, int limit) const;

    OrderCounts countOrdersForTrader(const std::string &traderId) const;

    Order getBestBid() const;
    Order getBestAsk() const;
    MarketData getMarketData() const;

    EventLogger &getEventLogger() const { return *eventLogger; }

private:
    std::shared_ptr<Database> database;
    std::shared_ptr<EventLogger> eventLogger;
    std::shared_ptr<MarketService> marketService;
    std::shared_ptr<RiskService> riskService;
    std::shared_ptr<TraderService> traderService;
    std::shared_ptr<TradeService> tradeService;

    std::unique_ptr<ActiveOrderService> activeOrderService;
    std::unique_ptr<ConditionalOrderService> conditionalOrderService;
};

#endif
