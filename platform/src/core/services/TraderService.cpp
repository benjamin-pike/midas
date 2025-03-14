#include <services/TraderService.hpp>

#include <memory>

std::shared_ptr<Trader> TraderService::getTrader(const std::string &traderId)
{
    if (traders.find(traderId) == traders.end())
    {
        traders[traderId] = std::make_shared<Trader>(traderId);
    }
    return traders[traderId];
}

void TraderService::updateTradersDrawdown(double currentPrice)
{
    for (const auto &trader : traders)
    {
        trader.second->updateMaxDrawdown(currentPrice);
    }
}
