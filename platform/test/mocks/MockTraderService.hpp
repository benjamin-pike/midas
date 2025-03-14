#include "services/TraderService.hpp"
#include "MockTrader.hpp"

#include <limits.h>

class MockTraderService : public TraderService
{
public:
    std::shared_ptr<Trader> getTrader(const std::string &traderId) override
    {
        if (traders.find(traderId) == traders.end())
        {
            traders[traderId] = std::make_shared<MockTrader>(traderId, INT_MAX, 0.0, 0.0, 0, 100.0);
        }
        return traders[traderId];
    }

    void setTrader(const std::string &traderId, std::shared_ptr<MockTrader> trader)
    {
        traders[traderId] = trader;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Trader>> traders;
};
