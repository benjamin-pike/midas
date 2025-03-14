#include "services/RiskService.hpp"

#include <algorithm>
#include <stdexcept>

bool RiskService::checkOrder(const Order &order, double currentMarketPrice)
{
    RiskLimits limits = getEffectiveLimits(order.getTraderId());
    auto trader = traderService->getTrader(order.getTraderId());

    if (order.getInitialQuantity() > limits.maxOrderSize)
        return false;

    if (trader->getRealizedPnL() < -limits.maxDailyLoss)
        return false;

    if (trader->getMaxDrawdown(currentMarketPrice) > limits.maxDrawdown)
        return false;

    if (trader->getRecentOrdersCount() >= limits.maxOrdersPerMin)
        return false;

    if (order.getSide() == OrderSide::BID)
    {
        if (trader->getInventory() + order.getInitialQuantity() > limits.maxOpenPosition)
            return false;
    }

    if (order.getSide() == OrderSide::ASK)
    {
        double orderPrice = (order.getPrice() != -1) ? order.getPrice() : currentMarketPrice;
        double orderNotional = orderPrice * order.getInitialQuantity();
        double portfolioValue = trader->getInventory() * trader->getAvgEntryPrice();
        if (portfolioValue > 0)
        {
            double riskPct = (orderNotional / portfolioValue) * 100.0;
            if (riskPct > limits.maxRiskPerOrder)
                return false;
        }
    }

    return true;
}

const RiskLimits RiskService::getEffectiveLimits(const std::string &traderId) const
{
    auto it = traderLimits.find(traderId);
    if (it == traderLimits.end())
        return globalLimits;

    RiskLimits effective = it->second;
    if (effective.maxOpenPosition == -1)
        effective.maxOpenPosition = globalLimits.maxOpenPosition;
    if (effective.maxOrderSize == -1)
        effective.maxOrderSize = globalLimits.maxOrderSize;
    if (effective.maxOrdersPerMin == -1)
        effective.maxOrdersPerMin = globalLimits.maxOrdersPerMin;
    if (effective.maxDailyLoss == -1)
        effective.maxDailyLoss = globalLimits.maxDailyLoss;
    if (effective.maxDrawdown == -1)
        effective.maxDrawdown = globalLimits.maxDrawdown;
    if (effective.maxRiskPerOrder == -1)
        effective.maxRiskPerOrder = globalLimits.maxRiskPerOrder;
    return effective;
}

void RiskService::setGlobalRiskLimits(const RiskLimits &limits, bool override)
{
    globalLimits = limits;
    if (override)
    {
        traderLimits.clear();
    }

    eventLogger->logEvent(std::make_shared<RiskEvent>(EventType::RISK_UPDATED, RiskEvent::Scope::GLOBAL, limits, override));
}

void RiskService::setTraderRiskLimits(const std::string &traderId, const RiskLimits &newLimits)
{
    auto it = traderLimits.find(traderId);
    if (it == traderLimits.end())
    {
        traderLimits[traderId] = newLimits;
    }
    else
    {
        if (newLimits.maxOpenPosition != -1)
            it->second.maxOpenPosition = newLimits.maxOpenPosition;
        if (newLimits.maxOrderSize != -1)
            it->second.maxOrderSize = newLimits.maxOrderSize;
        if (newLimits.maxOrdersPerMin != -1)
            it->second.maxOrdersPerMin = newLimits.maxOrdersPerMin;
        if (newLimits.maxDailyLoss != -1)
            it->second.maxDailyLoss = newLimits.maxDailyLoss;
        if (newLimits.maxDrawdown != -1)
            it->second.maxDrawdown = newLimits.maxDrawdown;
        if (newLimits.maxRiskPerOrder != -1)
            it->second.maxRiskPerOrder = newLimits.maxRiskPerOrder;
    }

    eventLogger->logEvent(std::make_shared<RiskEvent>(EventType::RISK_UPDATED, RiskEvent::Scope::TRADER, newLimits, traderId));
};
