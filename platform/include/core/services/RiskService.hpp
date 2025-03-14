#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include <events/EventLogger.hpp>
#include "services/TraderService.hpp"
#include "models/Risk.hpp"
#include "models/Order.hpp"

#include <string>
#include <unordered_map>
#include <functional>

class RiskService
{
public:
    RiskService(std::shared_ptr<EventLogger> eventLogger, std::shared_ptr<TraderService> traderService)
        : eventLogger(eventLogger), traderService(traderService) {}
    ~RiskService() = default;

    virtual bool checkOrder(const Order &order, double currentMarketPrice);
    virtual const RiskLimits getEffectiveLimits(const std::string &traderId) const;

    virtual void setGlobalRiskLimits(const RiskLimits &limits, bool override);
    virtual void setTraderRiskLimits(const std::string &traderId, const RiskLimits &limits);

private:
    std::shared_ptr<EventLogger> eventLogger;
    std::shared_ptr<TraderService> traderService;
    std::unordered_map<std::string, RiskLimits> traderLimits;
    RiskLimits globalLimits = {5000, 100, 5, 1000.0, 25.0, 3.0};
};

#endif
