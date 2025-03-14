#include "database/OrderMapping.hpp"
#include <database-utils.hpp>

const std::vector<FieldDescriptor<OrderRecord>> OrderRecord::fields = {
    INT_FIELD(type, getType, setType),
    INT_FIELD(side, getSide, setSide),
    INT_FIELD(status, getStatus, setStatus),
    INT_FIELD(initialQuantity, getInitialQuantity, setInitialQuantity),
    INT_FIELD(remainingQuantity, getRemainingQuantity, setRemainingQuantity),
    TEXT_FIELD(traderId, getTraderId, setTraderId),
    OPTIONAL_DOUBLE_FIELD(price, getPrice, setPrice),
    OPTIONAL_DOUBLE_FIELD(limitPrice, getLimitPrice, setLimitPrice),
    OPTIONAL_DOUBLE_FIELD(bestPrice, getBestPrice, setBestPrice),
    OPTIONAL_INT_FIELD(displaySize, getDisplaySize, setDisplaySize),
    OPTIONAL_INT_FIELD(hiddenQuantity, getHiddenQuantity, setHiddenQuantity),
    INT64_FIELD(timestamp, getTimestamp, setTimestamp),
};


const std::vector<std::string> OrderRecord::joins = {};
const std::vector<std::string> OrderRecord::joinSelects = {};
const std::vector<FieldDescriptor<OrderRecord>> OrderRecord::joinFields = {};
