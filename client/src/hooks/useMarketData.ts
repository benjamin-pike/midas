import { useCallback, useState } from "react";
import { CalculatedMarketData, MarketData } from "../lib/types";
import { getTrend } from "../lib/utils";

const useMarketData = () => {
	const [marketData, setMarketData] = useState<MarketData>();
	const [prevMarketData, setPrevMarketData] = useState<MarketData>();
	const [calculatedMarketData, setCalculatedMarketData] =
		useState<CalculatedMarketData>();

	const setStateWithPrevious = useCallback((newMarketData: MarketData) => {
		const newCalculatedMarketData = {
			currentPrice: {
				value: newMarketData.currentPrice,
				trend: getTrend(
					newMarketData.currentPrice,
					prevMarketData?.currentPrice,
					calculatedMarketData?.currentPrice.trend
				)
			},
			volatility: {
				value: newMarketData.volatility,
				trend: getTrend(
					newMarketData.volatility,
					prevMarketData?.volatility,
					calculatedMarketData?.volatility.trend
				)
			},
			trades: {
				count: newMarketData.trades.count,
				volume: newMarketData.trades.volume,
				avgPrice: {
					value: newMarketData.trades.avgPrice ?? 0,
					trend: getTrend(
						newMarketData.trades.avgPrice ?? undefined,
						prevMarketData?.trades.avgPrice ?? undefined,
						calculatedMarketData?.trades.avgPrice.trend
					)
				}
			},
			bids: {
				bestPrice: {
					value: newMarketData.bids.best?.price ?? 0,
					trend: getTrend(
						newMarketData.bids.best?.price ?? undefined,
						prevMarketData?.bids.best?.price ?? undefined,
						calculatedMarketData?.bids.bestPrice.trend
					)
				},
				bestQuantity: {
					value: newMarketData.bids.best?.remainingQuantity ?? 0,
					trend: getTrend(
						newMarketData.bids.best?.remainingQuantity ?? undefined,
						prevMarketData?.bids.best?.remainingQuantity ??
							undefined,
						calculatedMarketData?.bids.bestQuantity.trend
					)
				},
				volume: {
					value: newMarketData.bids.volume,
					trend: getTrend(
						newMarketData.bids.volume,
						prevMarketData?.bids.volume,
						calculatedMarketData?.bids.volume.trend
					)
				},
				count: {
					value: newMarketData.bids.count,
					trend: getTrend(
						newMarketData.bids.count,
						prevMarketData?.bids.count,
						calculatedMarketData?.bids.count.trend
					)
				}
			},
			asks: {
				bestPrice: {
					value: newMarketData.asks.best?.price ?? 0,
					trend: getTrend(
						newMarketData.asks.best?.price ?? undefined,
						prevMarketData?.asks.best?.price ?? undefined,
						calculatedMarketData?.asks.bestPrice.trend
					)
				},
				bestQuantity: {
					value: newMarketData.asks.best?.remainingQuantity ?? 0,
					trend: getTrend(
						newMarketData.asks.best?.remainingQuantity ?? undefined,
						prevMarketData?.asks.best?.remainingQuantity ??
							undefined,
						calculatedMarketData?.asks.bestQuantity.trend
					)
				},
				volume: {
					value: newMarketData.asks.volume,
					trend: getTrend(
						newMarketData.asks.volume,
						prevMarketData?.asks.volume,
						calculatedMarketData?.asks.volume.trend
					)
				},
				count: {
					value: newMarketData.asks.count,
					trend: getTrend(
						newMarketData.asks.count,
						prevMarketData?.asks.count,
						calculatedMarketData?.asks.count.trend
					)
				}
			},
			spread: {
				value:
					(newMarketData.asks.best?.price ?? 0) -
					(newMarketData.bids.best?.price ?? 0),
				trend: getTrend(
					(newMarketData.asks.best?.price ?? 0) -
						(newMarketData.bids.best?.price ?? 0),
					(prevMarketData?.asks.best?.price ?? 0) -
						(prevMarketData?.bids.best?.price ?? 0),
					calculatedMarketData?.spread.trend
				)
			},
			imbalance: {
				value: calculateImbalance(
					newMarketData.asks.volume,
					newMarketData.bids.volume
				),
				trend: getTrend(
					calculateImbalance(
						newMarketData.asks.volume,
						newMarketData.bids.volume
					),
					calculateImbalance(
						prevMarketData?.asks.volume ?? 0,
						prevMarketData?.bids.volume ?? 0
					),
					calculatedMarketData?.imbalance.trend
				)
			}
		};

		setPrevMarketData(marketData);
		setMarketData(newMarketData);
		setCalculatedMarketData(newCalculatedMarketData);
    }, [calculatedMarketData, marketData, prevMarketData]);

	return [calculatedMarketData, setStateWithPrevious] as const;
};

export default useMarketData;

const calculateImbalance = (asks: number, bids: number) => {
    if (asks === 0 && bids === 0) {
        return 0;
    }

	return (bids - asks) / (bids + asks);
};
