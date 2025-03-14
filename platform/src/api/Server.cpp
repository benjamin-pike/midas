#include "api/Server.hpp"
#include "api/Handlers.hpp"
#include "server-utils.hpp"
#include "order-utils.hpp"

#include <crow.h>
#include <unordered_set>
#include <memory>
#include <functional>
#include <thread>

Server::Server(const std::string &dbFilePath)
    : database(std::make_shared<Database>(dbFilePath)),
      eventLogger(std::make_shared<EventLogger>()),
      traderService(std::make_shared<TraderService>()),
      marketService(std::make_shared<MarketService>(traderService)),
      riskService(std::make_shared<RiskService>(eventLogger, traderService)),
      book(std::make_shared<OrderBook>(database, eventLogger, marketService, riskService, traderService))
{
}

Server::~Server()
{
    for (auto &[connection, _] : wsConnections)
    {
        connection->close();
    }
    app.stop();
}

void Server::start()
{
    setupRoutes();
    setupWebsocket();
    std::thread eventThread(&Server::processEvents, this);
    app.port(PORT).multithreaded().run();
}

void Server::setupRoutes()
{
    CROW_ROUTE(app, "/orders").methods("POST"_method)([this](const crow::request &req)
                                                      { return handlePostOrders(*this, req); });
    CROW_ROUTE(app, "/orders").methods("GET"_method)([this](const crow::request &req)
                                                     { return handleGetOrders(*this, req); });
    CROW_ROUTE(app, "/orders/<int>").methods("DELETE"_method)([this](int orderId)
                                                              { return handleDeleteOrder(*this, orderId); });
    CROW_ROUTE(app, "/trades").methods("GET"_method)([this](const crow::request &req)
                                                     { return handleGetTrades(*this, req); });
    CROW_ROUTE(app, "/traders/<string>").methods("GET"_method)([this](const std::string &traderId)
                                                               { return handleGetTraderById(*this, traderId); });
    CROW_ROUTE(app, "/market").methods("GET"_method)([this]()
                                                     { return handleGetMarket(*this); });
    CROW_ROUTE(app, "/risk").methods("PUT"_method)([this](const crow::request &req)
                                                   { return handlePutRisk(*this, req); });
    CROW_ROUTE(app, "/risk").methods("GET"_method)([this](const crow::request &req)
                                                   { return handleGetRisk(*this, req); });
};

void Server::setupWebsocket()
{
    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onaccept([this](const crow::request &req, void **data)
                  { return handleWebsocketAccept(*this, req, data); })
        .onopen([this](crow::websocket::connection &conn)
                { return handleWebsocketOpen(*this, conn); })
        .onclose([this](crow::websocket::connection &conn, const std::string &reason)
                 { return handleWebsocketClose(*this, conn); });
}

void Server::sendBroadcastUpdate(const std::string &message)
{
    for (auto &[connection, _] : wsConnections)
        connection->send_text(message);
}

void Server::sendTargetedUpdate(const std::string &traderId, const std::string &message)
{
    for (auto &[connection, id] : wsConnections)
        if (id == traderId)
            connection->send_text(message);
}

void Server::processEvents()
{
    while (true)
    {
        std::shared_ptr<Event> event;
        if (!eventLogger->getNextEvent(event))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        crow::json::wvalue data;

        data["message"] = event->getMessage();

        switch (event->getType())
        {
        case EventType::ORDER_ADDED:
        case EventType::ORDER_MODIFIED:
        case EventType::ORDER_CANCELLED:
        case EventType::ORDER_REJECTED:
        {
            auto orderEvent = std::dynamic_pointer_cast<OrderEvent>(event);

            switch (orderEvent->getType())
            {
            case EventType::ORDER_ADDED:
                data["event"] = "ORDER_ADDED";
                break;
            case EventType::ORDER_MODIFIED:
                data["event"] = "ORDER_MODIFIED";
                break;
            case EventType::ORDER_CANCELLED:
                data["event"] = "ORDER_CANCELLED";
                break;
            case EventType::ORDER_REJECTED:
                data["event"] = "ORDER_REJECTED";
                break;
            default:
                break;
            }

            auto order = orderEvent->getOrder();
            data["data"]["order"]["id"] = order.getId();
            data["data"]["order"]["side"] = getOrderSideString(order.getSide());
            data["data"]["order"]["type"] = getOrderTypeString(order.getType());
            data["data"]["order"]["status"] = getOrderStatusString(order.getStatus());
            data["data"]["order"]["quantity"] = order.getRemainingQuantity();
            data["data"]["order"]["traderId"] = order.getTraderId();
            if (order.getPrice() != -1)
                data["data"]["order"]["price"] = order.getPrice();

            data["data"]["asks"]["active"] = buildJsonList(book->getActiveAsks(0, 20), orderToJson);
            data["data"]["bids"]["active"] = buildJsonList(book->getActiveBids(0, 20), orderToJson);
            data["data"]["asks"]["conditional"] = buildJsonList(book->getConditionalAsks(0, 20), orderToJson);
            data["data"]["bids"]["conditional"] = buildJsonList(book->getConditionalBids(0, 20), orderToJson);

            auto marketData = book->getMarketData();
            data["data"]["market"] = marketDataToJson(marketData);

            sendBroadcastUpdate(data.dump());
            break;
        }
        case EventType::TRADE_EXECUTED:
        {
            auto tradeEvent = std::dynamic_pointer_cast<TradeEvent>(event);
            data["event"] = "TRADE_EXECUTED";
            data["data"]["trades"] = buildJsonList(book->getTrades(0, 20), tradeToJson);
            data["data"]["buyTraderId"] = tradeEvent->getBuyTraderId();
            data["data"]["sellTraderId"] = tradeEvent->getSellTraderId();

            auto marketData = book->getMarketData();
            data["data"]["market"] = marketDataToJson(marketData);

            sendBroadcastUpdate(data.dump());
            break;
        }
        case EventType::RISK_UPDATED:
        {
            auto riskEvent = std::dynamic_pointer_cast<RiskEvent>(event);
            RiskEvent::Scope scope = riskEvent->getScope();

            if (scope == RiskEvent::Scope::TRADER)
            {
                std::string traderId = riskEvent->getTraderId();
                RiskLimits limits = riskService->getEffectiveLimits(traderId);
                data["event"] = "RISK_UPDATED";
                data["data"]["limits"] = riskLimitsToJson(limits);

                sendTargetedUpdate(traderId, data.dump());
                break;
            }

            if (!riskEvent->isOverride())
            {
                for (auto &[connection, traderId] : wsConnections)
                {
                    RiskLimits limits = riskService->getEffectiveLimits(traderId);
                    data["event"] = "RISK_UPDATED";
                    data["data"]["limits"] = riskLimitsToJson(limits);
                    connection->send_text(data.dump());
                }
                break;
            }

            RiskLimits limits = riskService->getEffectiveLimits("");
            data["event"] = "RISK_UPDATED";
            data["data"]["limits"] = riskLimitsToJson(limits);

            sendBroadcastUpdate(data.dump());
            break;
        }
        default:
            break;
        }
    }
}