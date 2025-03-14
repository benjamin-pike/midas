#ifndef ORDERMAPPING_HPP
#define ORDERMAPPING_HPP

#include "database/BaseMapping.hpp"
#include "models/Order.hpp"

#include <vector>
#include <memory>
#include <stdexcept>

class OrderRecord
{
public:
    OrderRecord() {};

    static OrderRecord fromOrder(const Order &order)
    {
        OrderRecord record;

        record.setId(order.getId());
        record.setType(static_cast<int>(order.getType()));
        record.setSide(static_cast<int>(order.getSide()));
        record.setStatus(static_cast<int>(order.getStatus()));
        record.setInitialQuantity(order.getInitialQuantity());
        record.setRemainingQuantity(order.getRemainingQuantity());
        record.setTraderId(order.getTraderId());
        record.setTimestamp(order.getTimestamp());

        record.setPrice(order.getPrice());
        record.setLimitPrice(order.getLimitPrice());
        record.setBestPrice(order.getBestPrice());
        record.setHiddenQuantity(order.getHiddenQuantity());
        record.setDisplaySize(order.getDisplaySize());

        return record;
    }

    std::shared_ptr<Order> toOrder() const
    {
        std::shared_ptr<Order> order;
        switch (type)
        {
        case OrderType::MARKET:
            order = std::make_shared<MarketOrder>(side, initialQuantity, traderId);
            break;
        case OrderType::LIMIT:
            order = std::make_shared<LimitOrder>(side, initialQuantity, traderId, price);
            break;
        case OrderType::IOC:
            order = std::make_shared<IOCOrder>(side, initialQuantity, traderId, price);
            break;
        case OrderType::FOK:
            order = std::make_shared<FOKOrder>(side, initialQuantity, traderId, price);
            break;
        case OrderType::ICEBERG:
            order = std::make_shared<IcebergOrder>(side, initialQuantity, traderId, price, displaySize, hiddenQuantity);
            break;
        case OrderType::STOP:
            order = std::make_shared<StopOrder>(side, initialQuantity, traderId, price);
            break;
        case OrderType::STOP_LIMIT:
            order = std::make_shared<StopLimitOrder>(side, initialQuantity, traderId, price, limitPrice);
            break;
        case OrderType::TRAILING_STOP:
            order = std::make_shared<TrailingStopOrder>(side, initialQuantity, traderId, price, bestPrice);
            break;
        default:
            throw std::runtime_error("Unknown order type");
        }

        order->setId(id);
        order->setStatus(status);
        order->setRemainingQuantity(remainingQuantity);
        order->setTimestamp(timestamp);

        return order;
    }

    static constexpr const char *tableName = "orders";
    static const std::vector<FieldDescriptor<OrderRecord>> fields;

    static const std::vector<std::string> joins;
    static const std::vector<std::string> joinSelects;
    static const std::vector<FieldDescriptor<OrderRecord>> joinFields;

    int getId() const { return id; }
    int getType() const { return static_cast<int>(type); }
    int getSide() const { return static_cast<int>(side); }
    int getStatus() const { return static_cast<int>(status); }
    int getInitialQuantity() const { return initialQuantity; }
    int getRemainingQuantity() const { return remainingQuantity; }
    std::string getTraderId() const { return traderId; }
    long long getTimestamp() const { return timestamp; }

    void setId(int value) { id = value; }
    void setType(int value) { type = static_cast<OrderType>(value); }
    void setSide(int value) { side = static_cast<OrderSide>(value); }
    void setStatus(int value) { status = static_cast<OrderStatus>(value); }
    void setInitialQuantity(int value) { initialQuantity = value; }
    void setRemainingQuantity(int value) { remainingQuantity = value; }
    void setTraderId(const std::string &value) { traderId = value; }
    void setTimestamp(long long value) { timestamp = value; }

    std::optional<double> getPrice() const
    {
        if (price > 0)
            return price;
        else
            return std::nullopt;
    }
    std::optional<double> getLimitPrice() const
    {
        if (limitPrice > 0)
            return limitPrice;
        else
            return std::nullopt;
    }
    std::optional<double> getBestPrice() const
    {
        if (bestPrice > 0)
            return bestPrice;
        else
            return std::nullopt;
    }
    std::optional<int> getHiddenQuantity() const
    {
        if (hiddenQuantity > 0)
            return hiddenQuantity;
        else
            return std::nullopt;
    }
    std::optional<int> getDisplaySize() const
    {
        if (displaySize > 0)
            return displaySize;
        else
            return std::nullopt;
    }

    void setPrice(std::optional<double> value)
    {
        if (value.has_value())
            price = value.value();
        else
            price = -1;
    }
    void setDisplaySize(std::optional<int> value)
    {
        if (value.has_value())
            displaySize = value.value();
        else
            displaySize = -1;
    }
    void setHiddenQuantity(std::optional<int> value)
    {
        if (value.has_value())
            hiddenQuantity = value.value();
        else
            hiddenQuantity = -1;
    }
    void setLimitPrice(std::optional<double> value)
    {
        if (value.has_value())
            limitPrice = value.value();
        else
            limitPrice = -1;
    }
    void setBestPrice(std::optional<double> value)
    {
        if (value.has_value())
            bestPrice = value.value();
        else
            bestPrice = -1;
    }

private:
    int id;
    OrderType type;
    OrderSide side;
    OrderStatus status;
    int initialQuantity;
    int remainingQuantity;  
    std::string traderId;
    long long timestamp;

    double price;
    int displaySize;
    int hiddenQuantity;
    double limitPrice;
    double bestPrice;
};

#endif
