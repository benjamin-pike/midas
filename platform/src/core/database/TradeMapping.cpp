#include "database/TradeMapping.hpp"
#include "database-utils.hpp"

const std::vector<FieldDescriptor<TradeRecord>> TradeRecord::fields = {
    INT_FIELD(buyOrderId, getBuyOrderId, setBuyOrderId),
    INT_FIELD(sellOrderId, getSellOrderId, setSellOrderId),
    INT_FIELD(quantity, getQuantity, setQuantity),
    DOUBLE_FIELD(price, getPrice, setPrice),
    INT64_FIELD(timestamp, getTimestamp, setTimestamp),
};

const std::vector<std::string> TradeRecord::joins = {
    "JOIN orders AS b ON trades.buyOrderId = b.id",
    "JOIN orders AS s ON trades.sellOrderId = s.id"
};

const std::vector<std::string> TradeRecord::joinSelects = {
    "b.type AS buyOrderType",
    "s.type AS sellOrderType"
};

const std::vector<FieldDescriptor<TradeRecord>> TradeRecord::joinFields = {
    INT_FIELD(buyOrderType, getBuyOrderType, setBuyOrderType),
    INT_FIELD(sellOrderType, getSellOrderType, setSellOrderType),
};
