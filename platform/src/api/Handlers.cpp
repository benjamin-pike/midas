#include "api/Handlers.hpp"
#include "server-utils.hpp"
#include <crow.h>
#include <iostream>

bool handleWebsocketAccept(Server &server, const crow::request &req, void **data)
{
    crow::query_string qs(req.url_params);
    std::string traderId = qs.get("traderId");
    if (traderId.empty())
        return false;

    *data = new std::string(traderId);

    return true;
}

void handleWebsocketOpen(Server &server, crow::websocket::connection &connection)
{
    server.wsConnections[&connection] = *static_cast<std::string *>(connection.userdata());
}

void handleWebsocketClose(Server &server, crow::websocket::connection &connection)
{
    auto it = server.wsConnections.find(&connection);
    if (it != server.wsConnections.end())
        server.wsConnections.erase(it);
}

crow::response handlePostOrders(Server &server, const crow::request &req)
{
    crow::response res;
    auto json = loadJsonOrError(req, res);
    if (!json)
        return res;

    try
    {
        OrderType type = parseOrderType(json["orderType"].s());
        OrderSide side = (std::string(json["side"].s()) == "BID") ? OrderSide::BID : OrderSide::ASK;
        int quantity = json["quantity"].i();
        std::string traderId = json["traderId"].s();

        auto order = createOrderFromJson(type, json, side, quantity, traderId);
        server.book->addOrder(*order);
        res.code = 201;
    }
    catch (const std::exception &ex)
    {
        res.code = 500;
        res.write(ex.what());
    }
    return res;
}

crow::response handleGetOrders(Server &server, const crow::request &req)
{
    auto qs = crow::query_string(req.url_params);
    int start = getQueryParam(qs, "start", 0);
    int limit = getQueryParam(qs, "limit", 20);

    auto activeAsks = server.book->getActiveAsks(start, limit);
    auto activeBids = server.book->getActiveBids(start, limit);
    auto conditionalAsks = server.book->getConditionalAsks(start, limit);
    auto conditionalBids = server.book->getConditionalBids(start, limit);

    crow::json::wvalue res;
    res["asks"]["active"] = buildJsonList(activeAsks, orderToJson);
    res["bids"]["active"] = buildJsonList(activeBids, orderToJson);
    res["asks"]["conditional"] = buildJsonList(conditionalAsks, orderToJson);
    res["bids"]["conditional"] = buildJsonList(conditionalBids, orderToJson);

    return crow::response(res);
}

crow::response handleDeleteOrder(Server &server, int orderId)
{
    bool result = server.book->cancelOrder(orderId);
    crow::json::wvalue res;
    res["status"] = result ? "Order cancelled" : "Order not found";
    int statusCode = result ? 204 : 404;
    return crow::response(statusCode, res);
}

crow::response handleGetTrades(Server &server, const crow::request &req)
{
    auto qs = crow::query_string(req.url_params);
    int start = getQueryParam(qs, "start", 0);
    int limit = getQueryParam(qs, "limit", -1);
    auto trades = server.book->getTrades(start, limit);

    crow::json::wvalue res;
    res["trades"] = buildJsonList(trades, tradeToJson);
    return crow::response(res);
}

crow::response handleGetTraderById(Server &server, const std::string &traderId)
{
    auto trader = server.traderService->getTrader(traderId);
    crow::json::wvalue res;
    res["trader"] = traderToJson(trader, server.marketService, *server.book);
    return crow::response(res);
}

crow::response handleGetMarket(Server &server)
{
    crow::json::wvalue res;
    auto marketData = server.book->getMarketData();
    res["marketData"] = marketDataToJson(marketData);
    return crow::response(res);
}

crow::response handlePutRisk(Server &server, const crow::request &req)
{
    crow::response res;
    auto json = loadJsonOrError(req, res);
    if (!json)
        return res;

    if (!json.has("limits"))
    {
        res.code = 400;
        res.write("Missing limits field");
        return res;
    }

    std::string scope = json["scope"].s();
    RiskLimits limits;
    if (scope == "GLOBAL")
    {
        RiskLimits currentGlobal = server.riskService->getEffectiveLimits("");
        limits.maxOpenPosition = json["limits"].has("maxOpenPosition") ? json["limits"]["maxOpenPosition"].i() : currentGlobal.maxOpenPosition;
        limits.maxOrderSize = json["limits"].has("maxOrderSize") ? json["limits"]["maxOrderSize"].i() : currentGlobal.maxOrderSize;
        limits.maxOrdersPerMin = json["limits"].has("maxOrdersPerMin") ? json["limits"]["maxOrdersPerMin"].i() : currentGlobal.maxOrdersPerMin;
        limits.maxDailyLoss = json["limits"].has("maxDailyLoss") ? json["limits"]["maxDailyLoss"].d() : currentGlobal.maxDailyLoss;
        limits.maxDrawdown = json["limits"].has("maxDrawdown") ? json["limits"]["maxDrawdown"].d() : currentGlobal.maxDrawdown;
        limits.maxRiskPerOrder = json["limits"].has("maxRiskPerOrder") ? json["limits"]["maxRiskPerOrder"].d() : currentGlobal.maxRiskPerOrder;
    }
    else if (scope == "TRADER")
    {
        limits.maxOpenPosition = json["limits"].has("maxOpenPosition") ? json["limits"]["maxOpenPosition"].i() : -1;
        limits.maxOrderSize = json["limits"].has("maxOrderSize") ? json["limits"]["maxOrderSize"].i() : -1;
        limits.maxOrdersPerMin = json["limits"].has("maxOrdersPerMin") ? json["limits"]["maxOrdersPerMin"].i() : -1;
        limits.maxDailyLoss = json["limits"].has("maxDailyLoss") ? json["limits"]["maxDailyLoss"].d() : -1;
        limits.maxDrawdown = json["limits"].has("maxDrawdown") ? json["limits"]["maxDrawdown"].d() : -1;
        limits.maxRiskPerOrder = json["limits"].has("maxRiskPerOrder") ? json["limits"]["maxRiskPerOrder"].d() : -1;
    }
    else
    {
        res.code = 400;
        res.write("Invalid scope; must be GLOBAL or TRADER");
        return res;
    }

    bool overrideFlag = json.has("override") ? json["override"].b() : false;

    if (scope == "GLOBAL")
        server.riskService->setGlobalRiskLimits(limits, overrideFlag);
    else if (scope == "TRADER")
    {
        if (!json.has("traderId"))
        {
            res.code = 400;
            res.write("Missing traderId for TRADER scope");
            return res;
        }
        server.riskService->setTraderRiskLimits(json["traderId"].s(), limits);
    }

    return crow::response(200);
}

crow::response handleGetRisk(Server &server, const crow::request &req)
{
    crow::json::wvalue res;
    auto qs = crow::query_string(req.url_params);
    std::string traderId = qs.get("traderId") ? qs.get("traderId") : "";
    RiskLimits limits = server.riskService->getEffectiveLimits(traderId);
    res["limits"] = riskLimitsToJson(limits);
    
    return crow::response(res);
}