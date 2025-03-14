#include "models/Trader.hpp"

#include <iostream>
#include <stdexcept>

void Trader::buy(int quantity, double price)
{
    Lot newLot{quantity, price};
    lots.push_back(newLot);
}

void Trader::sell(int quantity, double price)
{
    int available = 0;
    for (const auto &lot : lots)
        available += lot.quantity;
    if (quantity > available)
    {
        throw std::runtime_error("Trader " + traderId + ": Not enough inventory to sell");
    }

    int remaining = quantity;
    double tradePnL = 0.0;

    while (remaining > 0 && !lots.empty())
    {
        Lot &lot = lots.front();
        if (lot.quantity > remaining)
        {
            tradePnL += remaining * (price - lot.price);
            lot.quantity -= remaining;
            remaining = 0;
        }
        else
        {
            tradePnL += lot.quantity * (price - lot.price);
            remaining -= lot.quantity;
            lots.erase(lots.begin());
        }
    }
    realizedPnL += tradePnL;

    totalClosedTrades++;
    avgExitPrice = (avgExitPrice * (totalClosedTrades - 1) + price) / totalClosedTrades;
    if (tradePnL > 0)
        wins++;
}

int Trader::getInventory() const
{
    int total = 0;
    for (const auto &lot : lots)
        total += lot.quantity;
    return total;
}

double Trader::getRealizedPnL() const
{
    return realizedPnL;
}

const std::string Trader::getTraderId() const
{
    return traderId;
}

double Trader::getAvgEntryPrice() const
{
    double total = 0.0;
    int quantity = 0;
    for (const auto &lot : lots)
    {
        total += lot.price * lot.quantity;
        quantity += lot.quantity;
    }
    return total / quantity;
}

double Trader::getUnrealizedPnL(double currentPrice) const
{
    double total = 0.0;
    for (const auto &lot : lots)
        total += lot.quantity * (currentPrice - lot.price);
    return total;
}

int Trader::getTotalOpenOrders() const
{
    return lots.size();
}

void Trader::updateMaxDrawdown(double currentPrice)
{
    double currentValue = currentPrice * getInventory();

    if (currentValue > peakValue)
    {
        peakValue = currentValue;
        troughValue = currentValue;
    }

    if (currentValue < troughValue)
    {
        troughValue = currentValue;
        double drawdown = ((peakValue - troughValue) / peakValue) * 100.0;
        if (drawdown > maxDrawdown)
            maxDrawdown = drawdown;
    }
}

bool Trader::placeOrder(const Order &order)
{
    if (order.getSide() == OrderSide::ASK)
    {
        int inventory = getInventory();
        int quantity = order.getInitialQuantity();
        if (inventory - reservedInventory < quantity)
        {
            return false;
        }
        reservedInventory += quantity;
    }

    time_t now = time(nullptr);
    while (!recentOrders.empty() && now - recentOrders.front() > 60)
    {
        recentOrders.pop_front();
    }
    recentOrders.push_back(now);

    return true;
}

std::vector<Lot> generateInitialLots()
{
    std::vector<Lot> lots;
    int numLots = rand() % 5 + 1;
    for (int i = 0; i < numLots; ++i)
    {
        int quantity = rand() % 900 + 100;
        double price = (rand() % 2000 + 9000) / 100.0;
        lots.push_back({quantity, price});
    }
    return lots;
}
