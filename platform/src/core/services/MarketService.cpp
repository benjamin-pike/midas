#include "services/MarketService.hpp"
#include "services/TraderService.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

void MarketService::updatePrice(double newPrice)
{
    currentPrice = newPrice;

    if (priceHistory.size() >= MAX_HISTORY_SIZE)
        priceHistory.pop_front();

    priceHistory.push_back(newPrice);
    volatility = calculateVolatility();
    traderService->updateTradersDrawdown(currentPrice);
}

double MarketService::calculateVolatility()
{
    double mean = 0.0;
    for (double p : priceHistory)
        mean += p;

    mean /= priceHistory.size();
    double sumSq = 0.0;
    for (double p : priceHistory)
    {
        double diff = p - mean;
        sumSq += diff * diff;
    }

    return std::sqrt(sumSq / priceHistory.size());
}