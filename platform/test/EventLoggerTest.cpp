#include "OrderBook.hpp"
#include "mocks/MockDatabase.hpp"
#include "mocks/MockRiskService.hpp"

#include <gtest/gtest.h>
#include <algorithm>
#include <limits.h>

class EventLogTest : public ::testing::Test
{
protected:
    std::shared_ptr<MockDatabase> database;
    std::shared_ptr<EventLogger> eventLogger;
    std::shared_ptr<MockTraderService> traderService;
    std::shared_ptr<MockRiskService> riskService;
    std::shared_ptr<MarketService> marketService;
    OrderBook orderBook;

    EventLogTest()
        : database(std::make_shared<MockDatabase>()),
          eventLogger(std::make_shared<EventLogger>()),
          traderService(std::make_shared<MockTraderService>()),
          riskService(std::make_shared<MockRiskService>(eventLogger, traderService)),
          marketService(std::make_shared<MarketService>(traderService)),
          orderBook(database, eventLogger, marketService, riskService, traderService)
    {
    }
};

TEST_F(EventLogTest, AddLargeNumberOfOrders)
{
    const size_t orderCount = 100000;

    for (int i = 1; i <= orderCount; ++i)
    {
        auto traderId = "Trader" + std::to_string(i);
        auto payload = LimitOrder((i % 2 == 0 ? OrderSide::BID : OrderSide::ASK), 1, traderId, 10.0 + (i % 10));
        auto order = orderBook.addOrder(payload);
    }

    EXPECT_NO_THROW(orderBook.getBestBid());
    EXPECT_NO_THROW(orderBook.getBestAsk());

    auto audit = orderBook.getEventLogger().getEventLog();

    EXPECT_GT(audit.size(), orderCount);
    EXPECT_LT(audit.size(), orderCount * 2);
}

TEST_F(EventLogTest, EventLogContainsEvents)
{
    auto payload = LimitOrder(OrderSide::BID, 10, "Trader", 100.0);
    auto order = orderBook.addOrder(payload);

    orderBook.modifyOrder(order.getId(), 101.0, 12);
    orderBook.cancelOrder(order.getId());

    auto log = orderBook.getEventLogger().getEventLog();

    ASSERT_EQ(log.size(), 3);
    EXPECT_EQ(log[0]->getType(), EventType::ORDER_ADDED);
    EXPECT_EQ(log[1]->getType(), EventType::ORDER_MODIFIED);
    EXPECT_EQ(log[2]->getType(), EventType::ORDER_CANCELLED);
}

TEST_F(EventLogTest, EventLogMatchesMatchEvent)
{
    auto limitPayload1 = LimitOrder(OrderSide::BID, 5, "TraderA", 105.0);
    auto limitPayload2 = LimitOrder(OrderSide::ASK, 10, "TraderB", 106.0);
    auto marketPayload = MarketOrder(OrderSide::BID, 5, "TraderC");

    orderBook.addOrder(limitPayload1);
    orderBook.addOrder(limitPayload2);

    auto marketOrder = orderBook.addOrder(marketPayload);

    auto log = orderBook.getEventLogger().getEventLog();
    auto it = std::find_if(log.begin(), log.end(), [](const std::shared_ptr<Event> &ev)
                           { return ev->getType() == EventType::TRADE_EXECUTED; });

    EXPECT_NE(it, log.end());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
