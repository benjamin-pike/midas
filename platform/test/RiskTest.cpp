#include "OrderBook.hpp"
#include "mocks/MockTraderService.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <numeric>

class RiskServiceTest : public ::testing::Test
{
protected:
    std::shared_ptr<EventLogger> eventLogger;
    std::shared_ptr<MockTraderService> traderService;
    std::shared_ptr<RiskService> riskService;

    void SetUp() override
    {
        eventLogger = std::make_shared<EventLogger>();
        traderService = std::make_shared<MockTraderService>();
        riskService = std::make_shared<RiskService>(eventLogger, traderService);
    }
};

TEST_F(RiskServiceTest, AcceptValidOrder)
{
    auto trader = std::make_shared<MockTrader>("TraderValid", 500, 0.0, 10.0, 0, 100.0);
    traderService->setTrader("TraderValid", trader);
    Order order = LimitOrder(OrderSide::BID, 10, "TraderValid", 100.0);
    EXPECT_TRUE(riskService->checkOrder(order, 100.0));
}

TEST_F(RiskServiceTest, RejectExceedMaxOrderSize)
{
    auto trader = std::make_shared<MockTrader>("TraderOrderSize", 300, 0.0, 10.0, 0, 100.0);
    traderService->setTrader("TraderOrderSize", trader);
    Order order = LimitOrder(OrderSide::BID, 150, "TraderOrderSize", 100.0);
    EXPECT_FALSE(riskService->checkOrder(order, 100.0));
}

TEST_F(RiskServiceTest, RejectExceedMaxOpenPosition)
{
    auto trader = std::make_shared<MockTrader>("TraderOpenPos", 950, 0.0, 10.0, 0, 100.0);
    traderService->setTrader("TraderOpenPos", trader);
    Order order = LimitOrder(OrderSide::ASK, 100, "TraderOpenPos", 100.0);
    EXPECT_FALSE(riskService->checkOrder(order, 100.0));
}

TEST_F(RiskServiceTest, RejectDueToDailyLoss)
{
    auto trader = std::make_shared<MockTrader>("TraderDailyLoss", 500, -1200.0, 10.0, 0, 100.0);
    traderService->setTrader("TraderDailyLoss", trader);
    Order order = LimitOrder(OrderSide::BID, 10, "TraderDailyLoss", 100.0);
    EXPECT_FALSE(riskService->checkOrder(order, 100.0));
}

TEST_F(RiskServiceTest, RejectDueToDrawdown)
{
    auto trader = std::make_shared<MockTrader>("TraderDrawdown", 500, 0.0, 30.0, 0, 100.0);
    traderService->setTrader("TraderDrawdown", trader);
    Order order = LimitOrder(OrderSide::ASK, 10, "TraderDrawdown", 100.0);
    EXPECT_FALSE(riskService->checkOrder(order, 100.0));
}

TEST_F(RiskServiceTest, RejectDueToRecentOrders)
{
    auto trader = std::make_shared<MockTrader>("TraderRecent", 500, 0.0, 10.0, 5, 100.0);
    traderService->setTrader("TraderRecent", trader);
    Order order = LimitOrder(OrderSide::BID, 10, "TraderRecent", 100.0);
    EXPECT_FALSE(riskService->checkOrder(order, 100.0));
}

TEST_F(RiskServiceTest, RejectDueToRiskPerOrder)
{
    auto trader = std::make_shared<MockTrader>("TraderRiskTrade", 500, 0.0, 10.0, 0, 100.0);
    traderService->setTrader("TraderRiskTrade", trader);
    Order order = LimitOrder(OrderSide::ASK, 20, "TraderRiskTrade", 100.0);
    EXPECT_FALSE(riskService->checkOrder(order, 100.0));
}

TEST_F(RiskServiceTest, AcceptRiskPerOrderWhenPortfolioZero)
{
    auto trader = std::make_shared<MockTrader>("TraderZeroPortfolio", 0, 0.0, 10.0, 0, 100.0);
    traderService->setTrader("TraderZeroPortfolio", trader);
    Order order = LimitOrder(OrderSide::BID, 50, "TraderZeroPortfolio", 100.0);
    EXPECT_TRUE(riskService->checkOrder(order, 100.0));
}

TEST_F(RiskServiceTest, UseTraderSpecificLimits)
{
    RiskLimits strictLimits = {500, 50, 3, 500.0, 15.0, 2.0};
    riskService->setTraderRiskLimits("TraderSpecial", strictLimits);
    auto trader = std::make_shared<MockTrader>("TraderSpecial", 400, 0.0, 10.0, 0, 100.0);
    traderService->setTrader("TraderSpecial", trader);
    Order orderValid = LimitOrder(OrderSide::BID, 20, "TraderSpecial", 100.0);
    Order orderInvalid = LimitOrder(OrderSide::BID, 60, "TraderSpecial", 100.0);
    EXPECT_TRUE(riskService->checkOrder(orderValid, 100.0));
    EXPECT_FALSE(riskService->checkOrder(orderInvalid, 100.0));
}

TEST_F(RiskServiceTest, GlobalOverrideClearsTraderLimits)
{
    RiskLimits customLimits = {500, 50, 3, 500.0, 15.0, 2.0};
    riskService->setTraderRiskLimits("TraderOverride", customLimits);
    RiskLimits newGlobalLimits = {1200, 120, 10, 1500.0, 30.0, 5.0};
    riskService->setGlobalRiskLimits(newGlobalLimits, true);
    RiskLimits effective = riskService->getEffectiveLimits("TraderOverride");
    EXPECT_EQ(effective.maxOpenPosition, 1200);
    EXPECT_EQ(effective.maxOrderSize, 120);
    EXPECT_EQ(effective.maxOrdersPerMin, 10);
    EXPECT_DOUBLE_EQ(effective.maxDailyLoss, 1500.0);
    EXPECT_DOUBLE_EQ(effective.maxDrawdown, 30.0);
    EXPECT_DOUBLE_EQ(effective.maxRiskPerOrder, 5.0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}