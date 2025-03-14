#ifndef API_UTILS_HPP
#define API_UTILS_HPP

#include <OrderBook.hpp>

#include <crow.h>

using OrderCreator = std::function<std::unique_ptr<Order>(const crow::json::rvalue &json,
                                                          OrderSide side,
                                                          int quantity,
                                                          const std::string &traderId)>;

int getQueryParam(const crow::query_string &qs, const char *param, int defaultValue);
crow::json::rvalue loadJsonOrError(const crow::request &req, crow::response &res);

OrderType parseOrderType(const std::string &orderTypeStr);

crow::json::wvalue orderToJson(const Order &order);
crow::json::wvalue tradeToJson(const Trade &trade);
crow::json::wvalue traderToJson(std::shared_ptr<Trader> trader, std::shared_ptr<MarketService> market, const OrderBook &orderBook);
crow::json::wvalue marketDataToJson(MarketData &marketData);
crow::json::wvalue riskLimitsToJson(const RiskLimits &limits);

std::unique_ptr<Order> createOrderFromJson(OrderType type, const crow::json::rvalue &json,
                                           OrderSide side, int quantity,
                                           const std::string &traderId);

template <typename Container, typename Converter>
crow::json::wvalue::list buildJsonList(const Container &items, Converter converter)
{
    crow::json::wvalue::list jsonList;
    for (const auto &item : items)
        jsonList.push_back(converter(item));
    return jsonList;
}

#endif