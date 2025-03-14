#ifndef TRADER_HPP
#define TRADER_HPP

#include "models/Order.hpp"
#include "trader-utils.hpp"

#include <string>
#include <vector>
#include <deque>

struct Lot
{
    int quantity;
    double price;
};

std::vector<Lot> generateInitialLots();

class Trader
{
public:
    Trader(const std::string &id) : traderId(id) { openPosition = getInventory(); };
    ~Trader() = default;

    virtual std::string getId() const { return traderId; }
    virtual std::string getName() const { return traderName; }

    virtual bool placeOrder(const Order &order);
    virtual void buy(int quantity, double price);
    virtual void sell(int quantity, double price);

    virtual const std::string getTraderId() const;
    virtual int getInventory() const;
    virtual double getRealizedPnL() const;
    virtual int getWins() const { return wins; }
    virtual int getTotalClosedTrades() const { return totalClosedTrades; }
    virtual int getTotalOpenOrders() const;
    virtual double getAvgEntryPrice() const;
    virtual double getAvgExitPrice() const { return avgExitPrice; }
    virtual double getUnrealizedPnL(double currentPrice) const;
    virtual double getMaxDrawdown(double currentPrice) const { return maxDrawdown; }
    virtual int getRecentOrdersCount() const { return recentOrders.size(); }

    virtual void updateMaxDrawdown(double currentPrice);

private:
    std::string traderId;
    std::string traderName = generateTraderName();

    int totalClosedTrades = 0;
    double avgExitPrice = 0.0;
    int wins = 0;

    std::vector<Lot> lots = generateInitialLots();
    int reservedInventory = 0;

    int openPosition = 0;
    double realizedPnL = 0.0;
    double peakValue = 0.0;
    double troughValue = 0.0;
    double maxDrawdown = 0.0;
    std::deque<time_t> recentOrders;
};

#endif