#ifndef MOCK_DATABASE_HPP
#define MOCK_DATABASE_HPP

#include "database/Database.hpp"
#include "models/Order.hpp"
#include "models/Trade.hpp"
#include <memory>
#include <vector>

class MockOrderRepository : public OrderRepository
{
public:
    MockOrderRepository() : OrderRepository(nullptr), nextOrderId(1) {}

    int create(const Order &order) override { return nextOrderId++; }
    void update(const Order &order) override {}
    std::vector<std::shared_ptr<Order>> getAllActive() override { return {}; }

private:
    int nextOrderId;
};

class MockTradeRepository : public TradeRepository
{
public:
    MockTradeRepository() : TradeRepository(nullptr), nextTradeId(1) {}

    int create(const Trade &trade) override { return nextTradeId++; }
    std::vector<Trade> getAll() override { return {}; }

private:
    int nextTradeId;
};

class MockDatabase : public Database
{
public:
    MockDatabase() : Database("") {}

    std::shared_ptr<OrderRepository> orders() const override { return mockOrdersRepo; }
    std::shared_ptr<TradeRepository> trades() const override { return mockTradesRepo; }

private:
    std::shared_ptr<OrderRepository> mockOrdersRepo = std::make_shared<MockOrderRepository>();
    std::shared_ptr<TradeRepository> mockTradesRepo = std::make_shared<MockTradeRepository>();
};

#endif
