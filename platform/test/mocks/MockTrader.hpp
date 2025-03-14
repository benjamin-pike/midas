#ifndef MOCK_TRADER_HPP
#define MOCK_TRADER_HPP

#include "models/Trader.hpp"

#include <string>
class MockTrader : public Trader
{
public:
    MockTrader(const std::string &id,
               int inventory,
               double realizedPnL,
               double maxDrawdown,
               int recentOrdersCount,
               double avgEntryPrice)
        : Trader(id),
          inventory_(inventory),
          realizedPnL_(realizedPnL),
          maxDrawdown_(maxDrawdown),
          recentOrdersCount_(recentOrdersCount),
          avgEntryPrice_(avgEntryPrice) {}

    int getInventory() const override
    {
        return inventory_;
    }
    double getRealizedPnL() const override
    {
        return realizedPnL_;
    }
    double getMaxDrawdown(double /*currentMarketPrice*/) const override
    {
        return maxDrawdown_;
    }
    int getRecentOrdersCount() const override
    {
        return recentOrdersCount_;
    }
    double getAvgEntryPrice() const override
    {
        return avgEntryPrice_;
    }

    void setInventory(int inv) { inventory_ = inv; }
    void setRealizedPnL(double pnl) { realizedPnL_ = pnl; }
    void setMaxDrawdown(double drawdown) { maxDrawdown_ = drawdown; }
    void setRecentOrdersCount(int count) { recentOrdersCount_ = count; }
    void setAvgEntryPrice(double price) { avgEntryPrice_ = price; }

private:
    int inventory_;
    double realizedPnL_;
    double maxDrawdown_;
    int recentOrdersCount_;
    double avgEntryPrice_;
};

#endif