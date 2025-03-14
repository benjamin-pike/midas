#ifndef ORDER_REPOSITORY_HPP
#define ORDER_REPOSITORY_HPP

#include "database/BaseRepository.hpp"
#include "database/OrderMapping.hpp"

class OrderRepository : public BaseRepository<OrderRecord>
{
public:
    OrderRepository(sqlite3 *connection) : BaseRepository<OrderRecord>(connection) {}

    virtual int create(const Order &order);
    virtual void update(const Order &order);
    virtual std::vector<std::shared_ptr<Order>> getAllActive();
};

#endif