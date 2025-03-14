#include "server-utils.hpp"
#include "order-utils.hpp"

int getQueryParam(const crow::query_string &qs, const char *param, int defaultValue)
{
    return qs.get(param) ? std::atoi(qs.get(param)) : defaultValue;
}

crow::json::rvalue loadJsonOrError(const crow::request &req, crow::response &res)
{
    auto json = crow::json::load(req.body);
    if (!json)
    {
        res.code = 400;
        res.write("Invalid JSON");
    }
    return json;
}

OrderType parseOrderType(const std::string &orderTypeStr)
{
    if (orderTypeStr == "LIMIT")
        return OrderType::LIMIT;
    else if (orderTypeStr == "MARKET")
        return OrderType::MARKET;
    else if (orderTypeStr == "IOC")
        return OrderType::IOC;
    else if (orderTypeStr == "FOK")
        return OrderType::FOK;
    else if (orderTypeStr == "STOP")
        return OrderType::STOP;
    else if (orderTypeStr == "STOP_LIMIT")
        return OrderType::STOP_LIMIT;
    else if (orderTypeStr == "TRAILING_STOP")
        return OrderType::TRAILING_STOP;
    else if (orderTypeStr == "ICEBERG")
        return OrderType::ICEBERG;
    else
        throw std::invalid_argument("Invalid order type");
}

crow::json::wvalue orderToJson(const Order &order)
{
    crow::json::wvalue obj;

    obj["id"] = order.getId();
    obj["orderType"] = getOrderTypeString(order.getType());
    obj["side"] = (order.getSide() == OrderSide::BID) ? "BID" : "ASK";
    obj["status"] = getOrderStatusString(order.getStatus());
    obj["traderId"] = order.getTraderId();
    obj["timestamp"] = order.getTimestamp();

    if (order.getPrice() != -1)
    {
        obj["price"] = order.getPrice();
    }
    else
    {
        obj["price"] = nullptr;
    }

    if (order.getType() == OrderType::ICEBERG)
    {
        obj["initialQuantity"] = order.getDisplaySize();
        obj["remainingQuantity"] = order.getRemainingQuantity();
    }
    else
    {
        obj["initialQuantity"] = order.getInitialQuantity();
        obj["remainingQuantity"] = order.getRemainingQuantity();
    }

    if (order.getType() == OrderType::STOP_LIMIT)
    {
        obj["limitPrice"] = order.getLimitPrice();
    }

    if (order.getType() == OrderType::TRAILING_STOP)
    {
        obj["bestPrice"] = order.getBestPrice();
    }

    return obj;
}

crow::json::wvalue tradeToJson(const Trade &trade)
{
    crow::json::wvalue obj;

    obj["tradeId"] = trade.getId();
    obj["buyOrderId"] = trade.getBuyOrderId();
    obj["sellOrderId"] = trade.getSellOrderId();
    obj["buyOrderType"] = getOrderTypeString(trade.getBuyOrderType());
    obj["sellOrderType"] = getOrderTypeString(trade.getSellOrderType());
    obj["price"] = trade.getPrice();
    obj["quantity"] = trade.getQuantity();
    obj["timestamp"] = trade.getTimestamp();

    return obj;
}

crow::json::wvalue traderToJson(std::shared_ptr<Trader> trader, std::shared_ptr<MarketService> market, const OrderBook &orderBook)
{
    crow::json::wvalue obj;

    auto traderOrderCounts = orderBook.countOrdersForTrader(trader->getId());

    obj["id"] = trader->getId();
    obj["name"] = trader->getName();
    obj["inventory"] = trader->getInventory();
    obj["openOrders"]["bids"] = traderOrderCounts.bids;
    obj["openOrders"]["asks"] = traderOrderCounts.asks;
    obj["closedTrades"] = trader->getTotalClosedTrades();
    obj["wins"] = trader->getWins();
    obj["avgEntryPrice"] = trader->getAvgEntryPrice();
    obj["avgExitPrice"] = trader->getAvgExitPrice();
    obj["realizedPnL"] = trader->getRealizedPnL();
    obj["unrealizedPnL"] = trader->getUnrealizedPnL(market->getCurrentPrice());
    obj["maxDrawdown"] = trader->getMaxDrawdown(market->getCurrentPrice());

    return obj;
}

crow::json::wvalue marketDataToJson(MarketData &marketData)
{
    crow::json::wvalue obj;

    obj["currentPrice"] = marketData.marketPrice;
    obj["volatility"] = marketData.volatility;
    obj["bids"]["count"] = marketData.bids.count;
    obj["bids"]["volume"] = marketData.bids.volume;
    obj["asks"]["count"] = marketData.asks.count;
    obj["asks"]["volume"] = marketData.asks.volume;
    obj["trades"]["count"] = marketData.trades.count;
    obj["trades"]["volume"] = marketData.trades.volume;
    obj["bids"]["best"] = marketData.bids.best.has_value() ? orderToJson(marketData.bids.best.value()) : nullptr;
    obj["asks"]["best"] = marketData.asks.best.has_value() ? orderToJson(marketData.asks.best.value()) : nullptr;

    if (marketData.trades.avgPrice > 1)
    {
        obj["trades"]["avgPrice"] = marketData.trades.avgPrice;
    }
    else
    {
        obj["trades"]["avgPrice"] = nullptr;
    }

    return obj;
}

crow::json::wvalue riskLimitsToJson(const RiskLimits &limits)
{
    crow::json::wvalue obj;
    obj["maxOpenPosition"] = limits.maxOpenPosition;
    obj["maxOrderSize"] = limits.maxOrderSize;
    obj["maxOrdersPerMin"] = limits.maxOrdersPerMin;
    obj["maxDailyLoss"] = limits.maxDailyLoss;
    obj["maxDrawdown"] = limits.maxDrawdown;
    obj["maxRiskPerOrder"] = limits.maxRiskPerOrder;
    return obj;
}

std::unique_ptr<Order> createOrderFromJson(OrderType type, const crow::json::rvalue &json,
                                           OrderSide side, int quantity,
                                           const std::string &traderId)
{
    switch (type)
    {
    case OrderType::MARKET:
        return std::make_unique<MarketOrder>(side, quantity, traderId);
    case OrderType::LIMIT:
        return std::make_unique<LimitOrder>(side, quantity, traderId, json["price"].d());
    case OrderType::IOC:
        return std::make_unique<IOCOrder>(side, quantity, traderId, json["price"].d());
    case OrderType::FOK:
        return std::make_unique<FOKOrder>(side, quantity, traderId, json["price"].d());
    case OrderType::STOP:
        return std::make_unique<StopOrder>(side, quantity, traderId, json["price"].d());
    case OrderType::STOP_LIMIT:
        return std::make_unique<StopLimitOrder>(side, quantity, traderId, json["price"].d(), json["limitPrice"].d());
    case OrderType::TRAILING_STOP:
        return std::make_unique<TrailingStopOrder>(side, quantity, traderId, json["price"].d(), json["bestPrice"].d());
    case OrderType::ICEBERG:
    {
        int displaySize = json["displaySize"].i();
        int hiddenQuantity = quantity - displaySize;
        return std::make_unique<IcebergOrder>(side, quantity, traderId, json["price"].d(), displaySize, hiddenQuantity);
    }
    default:
        throw std::invalid_argument("Invalid order type");
    }
}
