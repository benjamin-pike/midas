#ifndef MARKET_SERVICE_HPP
#define MARKET_SERVICE_HPP

#include "services/TraderService.hpp"

#include <deque>

class OrderBook;

class MarketService
{
public:
    MarketService(std::shared_ptr<TraderService> traderService): traderService(traderService) {}
    ~MarketService() = default;

    double getCurrentPrice() const { return currentPrice; }
    double getVolatility() const { return volatility; }

    void updatePrice(double newPrice);

private:
    std::shared_ptr<TraderService> traderService;

    double currentPrice = 100.0;
    double volatility = 0.0;
    std::deque<double> priceHistory;
    static constexpr size_t MAX_HISTORY_SIZE = 50;

    double calculateVolatility();
};

#endif
