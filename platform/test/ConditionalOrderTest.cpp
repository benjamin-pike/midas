#include "OrderBook.hpp"
#include "mocks/MockDatabase.hpp"
#include "mocks/MockRiskService.hpp"

#include <gtest/gtest.h>

class ConditionalOrderTest : public ::testing::Test
{
protected:
    std::shared_ptr<MockDatabase> database;
    std::shared_ptr<EventLogger> eventLogger;
    std::shared_ptr<MockTraderService> traderService;
    std::shared_ptr<MockRiskService> riskService;
    std::shared_ptr<MarketService> marketService;
    OrderBook orderBook;

    ConditionalOrderTest()
        : database(std::make_shared<MockDatabase>()),
          eventLogger(std::make_shared<EventLogger>()),
          traderService(std::make_shared<MockTraderService>()),
          riskService(std::make_shared<MockRiskService>(eventLogger, traderService)),
          marketService(std::make_shared<MarketService>(traderService)),
          orderBook(database, eventLogger, marketService, riskService, traderService)
    {
    }
};

TEST_F(ConditionalOrderTest, StopOrderNotTriggeredPrematurely)
{
    auto askPayload = StopOrder(OrderSide::ASK, 10, "TraderStop", 95.0);
    auto askOrder = orderBook.addOrder(askPayload);

    orderBook.updateMarketPrice(100.0, 0.0);

    auto events = orderBook.getEventLogger().getEventLog();
    auto it = std::find_if(events.begin(), events.end(), [](const std::shared_ptr<Event> &event)
                           { return event->getType() == EventType::ORDER_TRIGGERED; });

    EXPECT_EQ(it, events.end());
}

TEST_F(ConditionalOrderTest, StopOrderTriggered)
{
    auto askPayload = StopOrder(OrderSide::ASK, 10, "TraderStop", 95.0);
    auto askOrder = orderBook.addOrder(askPayload);

    orderBook.updateMarketPrice(94.0, 0.0);

    auto events = orderBook.getEventLogger().getEventLog();
    auto it = std::find_if(events.begin(), events.end(), [](const std::shared_ptr<Event> &event)
                           { return event->getType() == EventType::ORDER_TRIGGERED; });

    EXPECT_NE(it, events.end());
}

TEST_F(ConditionalOrderTest, StopLimitOrderNotTriggeredPrematurely)
{
    auto askPayload = StopLimitOrder(OrderSide::ASK, 10, "TraderStopLimit", 95.0, 94.0);
    auto askOrder = orderBook.addOrder(askPayload);

    orderBook.updateMarketPrice(96.0, 0.0);

    auto events = orderBook.getEventLogger().getEventLog();
    auto it = std::find_if(events.begin(), events.end(), [](const std::shared_ptr<Event> &event)
                           { return event->getType() == EventType::ORDER_TRIGGERED; });

    EXPECT_EQ(it, events.end());
}

TEST_F(ConditionalOrderTest, StopLimitOrderTriggered)
{
    auto askPayload = StopLimitOrder(OrderSide::ASK, 10, "TraderStopLimit", 95.0, 94.0);
    auto askOrder = orderBook.addOrder(askPayload);

    orderBook.updateMarketPrice(93.0, 0.0);

    auto events = orderBook.getEventLogger().getEventLog();
    auto it = std::find_if(events.begin(), events.end(), [](const std::shared_ptr<Event> &event)
                           { return event->getType() == EventType::ORDER_TRIGGERED; });

    EXPECT_NE(it, events.end());

    Order bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.getType(), OrderType::LIMIT);
}

TEST_F(ConditionalOrderTest, TrailingStopOrderNotTriggeredPrematurely)
{
    auto askPayload = TrailingStopOrder(OrderSide::ASK, 10, "TraderTrailing", 2.0, 100.0);
    auto askOrder = orderBook.addOrder(askPayload);

    orderBook.updateMarketPrice(99.0, 0.0);

    auto events = orderBook.getEventLogger().getEventLog();
    auto it = std::find_if(events.begin(), events.end(), [](const std::shared_ptr<Event> &event)
                           { return event->getType() == EventType::ORDER_TRIGGERED; });
    EXPECT_EQ(it, events.end());
}

TEST_F(ConditionalOrderTest, TrailingStopOrderTriggered)
{
    auto askPayload = TrailingStopOrder(OrderSide::ASK, 10, "TraderTrailing", 2.0, 100.0);
    auto askOrder = orderBook.addOrder(askPayload);

    orderBook.updateMarketPrice(97.0, 0.0);

    auto events = orderBook.getEventLogger().getEventLog();
    auto it = std::find_if(events.begin(), events.end(), [](const std::shared_ptr<Event> &event)
                           { return event->getType() == EventType::ORDER_TRIGGERED; });
    EXPECT_NE(it, events.end());
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
