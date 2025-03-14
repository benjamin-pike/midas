#ifndef RISK_HPP
#define RISK_HPP

struct RiskLimits
{
    int maxOpenPosition;
    int maxOrderSize;
    int maxOrdersPerMin;
    double maxDailyLoss;
    double maxDrawdown;
    double maxRiskPerOrder;
};

#endif