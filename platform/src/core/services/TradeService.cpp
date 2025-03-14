#include "services/TradeService.hpp"

#include <iostream>

TradeService::TradeService(
    std::shared_ptr<Database> database,
    std::shared_ptr<EventLogger> eventLogger,
    std::shared_ptr<MarketService> marketService,
    std::shared_ptr<TraderService> traderService)
    : database(database),
      eventLogger(eventLogger),
      marketService(marketService),
      traderService(traderService)
{
    trades = database->trades()->getAll();
}

Trade TradeService::addTrade(Order &bidOrder, Order &askOrder, int quantity)
{
    double bidPrice = bidOrder.getPrice();
    double askPrice = askOrder.getPrice();

    double tradePrice = marketService->getCurrentPrice();
    if (bidPrice == -1 && askPrice > 0) {
        tradePrice = askPrice;
    } else if (askPrice == -1 && bidPrice > 0) {
        tradePrice = bidPrice;
    } else if (bidPrice > 0 && askPrice > 0) {
        tradePrice = askPrice;
    } 

    Trade trade(bidOrder.getId(), askOrder.getId(), bidOrder.getType(), askOrder.getType(), quantity, tradePrice);

    int tradeId = database->trades()->create(trade);
    trade.setId(tradeId);
    trades.push_back(trade);

    auto buyTrader = traderService->getTrader(bidOrder.getTraderId());
    auto sellTrader = traderService->getTrader(askOrder.getTraderId());

    buyTrader->buy(trade.getQuantity(), trade.getPrice());
    sellTrader->sell(trade.getQuantity(), trade.getPrice());

    marketService->updatePrice(trade.getPrice());

    return trade;
}

Trade TradeService::getTrade(int tradeId) const
{
    for (const auto &trade : trades)
    {
        if (trade.getId() == tradeId)
            return trade;
    }
    throw std::runtime_error("Trade not found");
}

std::vector<Trade> TradeService::getTrades(int start, int limit) const
{
    int sizeLimit = limit > 0 ? limit : trades.size();
    int startIndex = static_cast<int>(trades.size()) - 1 - start;

    std::vector<Trade> result;
    result.reserve(std::min<size_t>(sizeLimit, trades.size()));

    if (startIndex < 0)
    {
        return result;
    }

    int endIndex = std::max(0, startIndex - (sizeLimit - 1));

    for (int i = startIndex; i >= endIndex; --i)
    {
        result.push_back(trades[i]);
    }

    return result;
}

