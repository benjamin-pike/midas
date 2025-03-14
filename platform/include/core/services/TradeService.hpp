#ifndef TRADE_SERVICE_HPP
#define TRADE_SERVICE_HPP

#include "services/MarketService.hpp"
#include "services/TraderService.hpp"
#include "database/Database.hpp"
#include "events/EventLogger.hpp"
#include "models/Trade.hpp"

#include <vector>

class TradeService
{
public:
    TradeService(
        std::shared_ptr<Database> database,
        std::shared_ptr<EventLogger> eventLogger,
        std::shared_ptr<MarketService> marketService,
        std::shared_ptr<TraderService> traderService);
    ~TradeService() = default;

    Trade addTrade(Order &bidOrder, Order &askOrder, int quantity);
    Trade getTrade(int tradeId) const;
    std::vector<Trade> getTrades(int start = 0, int limit = -1) const;

private:
    std::shared_ptr<Database> database;
    std::shared_ptr<EventLogger> eventLogger;
    std::shared_ptr<MarketService> marketService;
    std::shared_ptr<TraderService> traderService;
    std::vector<Trade> trades;
};

#endif