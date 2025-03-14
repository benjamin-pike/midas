#ifndef TRADER_SERVICE_HPP
#define TRADER_SERVICE_HPP

#include "models/Trader.hpp"
#include "models/Trade.hpp"

#include <string>
#include <unordered_map>
#include <memory>

class TraderService
{
public:
    virtual std::shared_ptr<Trader> getTrader(const std::string &traderId);
    virtual void updateTradersDrawdown(double currentPrice);

private:
    std::unordered_map<std::string, std::shared_ptr<Trader>> traders;
};

#endif
