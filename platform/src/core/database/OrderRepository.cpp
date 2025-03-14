#include "database/OrderRepository.hpp"

int OrderRepository::create(const Order &order)
{
    OrderRecord record = OrderRecord::fromOrder(order);
    int newId = this->createRecord(record);
    record.setId(newId);
    return newId;
}

void OrderRepository::update(const Order &order)
{
    OrderRecord record = OrderRecord::fromOrder(order);
    this->updateRecord(record);
}

std::vector<std::shared_ptr<Order>> OrderRepository::getAllActive()
{
    std::string whereClause = "status IN (" +
                              std::to_string(static_cast<int>(OrderStatus::UNFILLED)) + ", " +
                              std::to_string(static_cast<int>(OrderStatus::PARTIALLY_FILLED)) + ")"
                                                                                                " AND type IN (" +
                              std::to_string(static_cast<int>(OrderType::LIMIT)) + ", " +
                              std::to_string(static_cast<int>(OrderType::MARKET)) + ", " +
                              std::to_string(static_cast<int>(OrderType::IOC)) + ", " +
                              std::to_string(static_cast<int>(OrderType::FOK)) + ", " +
                              std::to_string(static_cast<int>(OrderType::ICEBERG)) + ")";

    auto records = this->getAllRecords(whereClause);
    std::vector<std::shared_ptr<Order>> orders;
    for (auto &record : records)
        orders.push_back(record.toOrder());

    return orders;
}