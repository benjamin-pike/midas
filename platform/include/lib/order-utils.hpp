#ifndef ORDER_UTILS_HPP
#define ORDER_UTILS_HPP

#include "models/Order.hpp"

bool isActiveOrder(const Order& order);
bool isConditionalOrder(const Order& order);
bool isOpenOrder(const Order& order);

std::string getOrderTypeString(OrderType orderType, bool titleCase = false);
std::string getOrderSideString(OrderSide orderSide);
std::string getOrderStatusString(OrderStatus orderStatus);

std::string formatPrice(double price);

#endif