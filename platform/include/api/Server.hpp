#ifndef SERVER_HPP
#define SERVER_HPP

#include <crow.h>
#include <unordered_set>
#include <memory>

#include "OrderBook.hpp"

constexpr int PORT = 8080;

struct CORSHandler
{
    struct context
    {
    };

    void before_handle(crow::request &req, crow::response &res, context &)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");

        if (req.method == crow::HTTPMethod::OPTIONS)
        {
            res.end();
        }
    }

    void after_handle(crow::request &, crow::response &res, context &)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    }
};

class Server
{
public:
    Server(const std::string &dbFilePath);
    ~Server();
    void start();

    std::shared_ptr<Database> const database;
    std::shared_ptr<EventLogger> const eventLogger;
    std::shared_ptr<TraderService> const traderService;
    std::shared_ptr<MarketService> const marketService;
    std::shared_ptr<RiskService> const riskService;
    std::shared_ptr<OrderBook> const book;

    crow::App<CORSHandler> app;
    std::unordered_map<crow::websocket::connection *, std::string> wsConnections;
private:

    void setupRoutes();
    void setupWebsocket();
    void processEvents();
    void sendBroadcastUpdate(const std::string &message);
    void sendTargetedUpdate(const std::string &traderId, const std::string &message);
};

#endif
