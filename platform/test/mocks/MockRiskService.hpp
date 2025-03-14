#ifndef MOCK_RISK_SERVICE_HPP
#define MOCK_RISK_SERVICE_HPP

#include "services/RiskService.hpp"
#include "MockTraderService.hpp"

class MockRiskService : public RiskService
{
public:
    MockRiskService(std::shared_ptr<EventLogger> eventLogger, std::shared_ptr<MockTraderService> traderService)
        : RiskService(eventLogger, traderService) {}

    bool checkOrder(const Order &order, double currentMarketPrice) override
    {
        return true;
    }

    const RiskLimits getEffectiveLimits(const std::string &traderId) const override
    {
        return RiskLimits{1000, 100, 5, 1000.0, 25.0, 3.0};
    }

    void setGlobalRiskLimits(const RiskLimits &limits, bool override) override
    {
        // Not implemented
    }

    void setTraderRiskLimits(const std::string &traderId, const RiskLimits &limits) override
    {
        // Not implemented
    }
};

#endif
