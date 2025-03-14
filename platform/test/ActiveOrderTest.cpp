#include "OrderBook.hpp"
#include "mocks/MockDatabase.hpp"
#include "mocks/MockRiskService.hpp"

#include <gtest/gtest.h>
#include <memory>

class ActiveOrderTest : public ::testing::Test
{
protected:
    std::shared_ptr<MockDatabase> database;
    std::shared_ptr<EventLogger> eventLogger;
    std::shared_ptr<MockTraderService> traderService;
    std::shared_ptr<MockRiskService> riskService;
    std::shared_ptr<MarketService> marketService;
    OrderBook orderBook;

    ActiveOrderTest()
        : database(std::make_shared<MockDatabase>()),
          eventLogger(std::make_shared<EventLogger>()),
          traderService(std::make_shared<MockTraderService>()),
          riskService(std::make_shared<MockRiskService>(eventLogger, traderService)),
          marketService(std::make_shared<MarketService>(traderService)),
          orderBook(database, eventLogger, marketService, riskService, traderService)
    {
    }
};

TEST_F(ActiveOrderTest, AddOrderTest)
{
    auto payload = LimitOrder(OrderSide::BID, 10, "Trader1", 100.0);
    auto order = orderBook.addOrder(payload);

    auto bestBid = orderBook.getBestBid();

    EXPECT_EQ(bestBid.getId(), order.getId());
}

TEST_F(ActiveOrderTest, ModifyOrderTest)
{
    auto payload = LimitOrder(OrderSide::BID, 10, "Trader2", 100.0);
    auto order = orderBook.addOrder(payload);
    bool modified = orderBook.modifyOrder(order.getId(), 101.0, 20);

    auto bestBid = orderBook.getBestBid();

    EXPECT_TRUE(modified);
    EXPECT_EQ(bestBid.getId(), order.getId());
}

TEST_F(ActiveOrderTest, CancelOrderTest)
{
    auto payload = LimitOrder(OrderSide::ASK, 5, "Trader3", 101.0);
    auto order = orderBook.addOrder(payload);

    bool cancelled = orderBook.cancelOrder(order.getId());
    EXPECT_TRUE(cancelled);

    EXPECT_THROW(orderBook.getBestAsk(), std::runtime_error);
}

TEST_F(ActiveOrderTest, ProcessMarketOrderTest)
{
    auto askPayload = LimitOrder(OrderSide::ASK, 5, "Trader4", 102.0);
    auto bidPayload = MarketOrder(OrderSide::BID, 15, "Trader5");

    auto askOrder = orderBook.addOrder(askPayload);
    auto bidOrder = orderBook.addOrder(bidPayload);

    EXPECT_THROW(orderBook.getBestAsk(), std::runtime_error);
}

TEST_F(ActiveOrderTest, ProcessValidLimitOrderTest)
{
    auto askPayload = LimitOrder(OrderSide::ASK, 5, "Trader6", 103.0);
    auto bidPayload = LimitOrder(OrderSide::BID, 5, "Trader7", 104.0);

    auto askOrder = orderBook.addOrder(askPayload);
    auto bidOrder = orderBook.addOrder(bidPayload);

    EXPECT_THROW(orderBook.getBestAsk(), std::runtime_error);
    EXPECT_THROW(orderBook.getBestBid(), std::runtime_error);
}

TEST_F(ActiveOrderTest, ProcessInvalidLimitOrderTest)
{
    auto askPayload = LimitOrder(OrderSide::ASK, 5, "Trader6", 103.0);
    auto bidPayload = LimitOrder(OrderSide::BID, 5, "Trader7", 102.0);

    auto askOrder = orderBook.addOrder(askPayload);
    auto bidOrder = orderBook.addOrder(bidPayload);

    auto bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.getId(), bidOrder.getId());
}

TEST_F(ActiveOrderTest, MarketOrderPriorityTest)
{
    auto limitAskPayload = LimitOrder(OrderSide::ASK, 10, "TraderA", 100.0);
    auto marketAskPayload = MarketOrder(OrderSide::ASK, 10, "TraderB");
    auto limitAskOrder = orderBook.addOrder(limitAskPayload);
    auto marketAskOrder = orderBook.addOrder(marketAskPayload);

    auto limitBidPayload = LimitOrder(OrderSide::BID, 10, "TraderC", 100.0);
    auto limitBidOrder = orderBook.addOrder(limitBidPayload);

    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.getId(), limitAskOrder.getId());
}

TEST_F(ActiveOrderTest, PriceTimePriorityAsk)
{
    auto askPayload1 = LimitOrder(OrderSide::ASK, 10, "TraderA", 105.0);
    auto askPayload2 = LimitOrder(OrderSide::ASK, 15, "TraderB", 105.0);
    auto bidPayload = MarketOrder(OrderSide::BID, 10, "TraderC");

    auto askOrder1 = orderBook.addOrder(askPayload1);

    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.getId(), askOrder1.getId());

    auto askOrder2 = orderBook.addOrder(askPayload2);
    auto bidOrder = orderBook.addOrder(bidPayload);

    bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.getId(), askOrder2.getId());
}

TEST_F(ActiveOrderTest, PriceTimePriorityBid)
{
    auto bidPayload1 = LimitOrder(OrderSide::BID, 20, "TraderX", 110.0);
    Order bidOrder1 = orderBook.addOrder(bidPayload1);

    Order bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.getId(), bidOrder1.getId());

    auto bidPayload2 = LimitOrder(OrderSide::BID, 25, "TraderY", 110.0);
    Order bidOrder2 = orderBook.addOrder(bidPayload2);

    Order marketAsk(OrderType::MARKET, OrderSide::ASK, 20, "TraderZ");
    orderBook.addOrder(marketAsk);

    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.getId(), bidOrder2.getId());
}

TEST_F(ActiveOrderTest, IOCOrderPartialFill)
{
    auto askPayload = LimitOrder(OrderSide::ASK, 10, "TraderASK", 100.0);
    auto bidPayload = IOCOrder(OrderSide::BID, 6, "TraderIOC", 100.0);

    auto askOrder = orderBook.addOrder(askPayload);
    auto bidOrder = orderBook.addOrder(bidPayload);

    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.getRemainingQuantity(), 4);
    EXPECT_EQ(bestAsk.getId(), askOrder.getId());
}

TEST_F(ActiveOrderTest, IOCOrderFullFill)
{
    auto askPayload = LimitOrder(OrderSide::ASK, 5, "TraderASK2", 100.0);
    auto bidPayload = IOCOrder(OrderSide::BID, 5, "TraderIOC2", 100.0);

    orderBook.addOrder(askPayload);
    orderBook.addOrder(bidPayload);

    EXPECT_THROW(orderBook.getBestAsk(), std::runtime_error);
}

TEST_F(ActiveOrderTest, FOKOrderCannotFill)
{
    auto askPayload = LimitOrder(OrderSide::ASK, 5, "TraderASK3", 100.0);
    auto bidPayload = FOKOrder(OrderSide::BID, 10, "TraderFOK", 100.0);

    auto askOrder = orderBook.addOrder(askPayload);
    auto bidOrder = orderBook.addOrder(bidPayload);

    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.getRemainingQuantity(), 5);
    EXPECT_EQ(bestAsk.getId(), askOrder.getId());
}

TEST_F(ActiveOrderTest, FOKOrderFullFill)
{
    auto askPayload1 = LimitOrder(OrderSide::ASK, 5, "TraderASK4", 100.0);
    auto askPayload2 = LimitOrder(OrderSide::ASK, 10, "TraderASK5", 100.0);
    auto bidPayload = FOKOrder(OrderSide::BID, 10, "TraderFOK2", 100.0);

    auto askOrder1 = orderBook.addOrder(askPayload1);
    auto askOrder2 = orderBook.addOrder(askPayload2);
    auto bidOrder = orderBook.addOrder(bidPayload);

    auto bestAsk = orderBook.getBestAsk();
    EXPECT_EQ(bestAsk.getId(), askOrder2.getId());
    EXPECT_EQ(bestAsk.getRemainingQuantity(), 5);
}

TEST_F(ActiveOrderTest, IcebergOrderTest)
{
    auto askPayload1 = MarketOrder(OrderSide::ASK, 20, "TraderSeller1");
    auto askPayload2 = MarketOrder(OrderSide::ASK, 10, "TraderSeller2");
    auto icebergPayload = IcebergOrder(OrderSide::BID, 20, "TraderIceberg", 100, 20, 80);

    auto icebergOrder = orderBook.addOrder(icebergPayload);
    auto askOrder1 = orderBook.addOrder(askPayload1);

    Order bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.getType(), OrderType::ICEBERG);
    EXPECT_EQ(bestBid.getRemainingQuantity(), 20);
    EXPECT_EQ(bestBid.getHiddenQuantity(), 60);
    auto askOrder2 = orderBook.addOrder(askPayload2);

    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.getRemainingQuantity(), 10);
    EXPECT_EQ(bestBid.getHiddenQuantity(), 60);
    auto askPayload3 = MarketOrder(OrderSide::ASK, 10, "TraderSeller3");
    auto askOrder3 = orderBook.addOrder(askPayload3);

    bestBid = orderBook.getBestBid();
    EXPECT_EQ(bestBid.getRemainingQuantity(), 20);
    EXPECT_EQ(bestBid.getHiddenQuantity(), 40);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
