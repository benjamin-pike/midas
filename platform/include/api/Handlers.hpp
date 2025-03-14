#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "api/Server.hpp"

#include <crow.h>
#include <string>

bool handleWebsocketAccept(Server &server, const crow::request &req, void **data);
void handleWebsocketOpen(Server &server, crow::websocket::connection &connection);
void handleWebsocketClose(Server &server, crow::websocket::connection &connection);

crow::response handlePostOrders(Server &server, const crow::request &req);
crow::response handleGetOrders(Server &server, const crow::request &req);
crow::response handleDeleteOrder(Server &server, int orderId);
crow::response handleGetTrades(Server &server, const crow::request &req);
crow::response handleGetTraderById(Server &server, const std::string &traderId);
crow::response handleGetMarket(Server &server);
crow::response handlePutRisk(Server &server, const crow::request &req);
crow::response handleGetRisk(Server &server, const crow::request &req);

#endif
