import { useCallback, useState } from "react";
import { Trader, CalculatedTraderData } from "../lib/types";
import { getTrend } from "../lib/utils";

const useTraderData = () => {
	const [traderData, setTraderData] = useState<Trader>();
	const [prevTraderData, setPrevTraderData] = useState<Trader>();
	const [calculatedTraderData, setCalculatedTraderData] =
		useState<CalculatedTraderData>();

	const setStateWithPrevious = useCallback(
		(newTraderData: Trader) => {
			const [fname, lname] = newTraderData.name.split(" ");
			const initials = `${fname[0]}${lname[0]}`;

			const newCalculatedTraderData: CalculatedTraderData = {
				id: newTraderData.id,
				fname,
				lname,
				initials,
				inventory: {
					value: newTraderData.inventory,
					trend: getTrend(
						newTraderData.inventory,
						prevTraderData?.inventory,
						calculatedTraderData?.inventory.trend
					)
				},
				openOrders: {
					total: {
						value:
							newTraderData.openOrders.bids +
							newTraderData.openOrders.asks,
						trend: getTrend(
							newTraderData.openOrders.bids +
								newTraderData.openOrders.asks,
							prevTraderData
								? prevTraderData.openOrders.bids +
										prevTraderData.openOrders.asks
								: undefined,
							calculatedTraderData?.openOrders.total.trend
						)
					},
					bids: {
						value: newTraderData.openOrders.bids,
						trend: getTrend(
							newTraderData.openOrders.bids,
							prevTraderData?.openOrders.bids,
							calculatedTraderData?.openOrders.bids.trend
						)
					},
					asks: {
						value: newTraderData.openOrders.asks,
						trend: getTrend(
							newTraderData.openOrders.asks,
							prevTraderData?.openOrders.asks,
							calculatedTraderData?.openOrders.asks.trend
						)
					}
				},
				closedTrades: newTraderData.closedTrades,
				winRate: {
					value: newTraderData.wins / newTraderData.closedTrades,
					trend: getTrend(
						newTraderData.wins / newTraderData.closedTrades,
						prevTraderData
							? prevTraderData.wins / prevTraderData.closedTrades
							: undefined,
						calculatedTraderData?.winRate.trend
					)
				},
				avgEntryPrice: {
					value: newTraderData.avgEntryPrice,
					trend: getTrend(
						newTraderData.avgEntryPrice,
						prevTraderData?.avgEntryPrice,
						calculatedTraderData?.avgEntryPrice.trend
					)
				},
				avgExitPrice: {
					value: newTraderData.avgExitPrice,
					trend: getTrend(
						newTraderData.avgExitPrice,
						prevTraderData?.avgExitPrice,
						calculatedTraderData?.avgExitPrice.trend
					)
				},
				realizedPnL: {
					value: newTraderData.realizedPnL,
					trend: getTrend(
						newTraderData.realizedPnL,
						prevTraderData?.realizedPnL,
						calculatedTraderData?.realizedPnL.trend
					)
				},
				unrealizedPnL: {
					value: newTraderData.unrealizedPnL,
					trend: getTrend(
						newTraderData.unrealizedPnL,
						prevTraderData?.unrealizedPnL,
						calculatedTraderData?.unrealizedPnL.trend
					)
				},
				maxDrawdown: newTraderData.maxDrawdown
			};

			setPrevTraderData(traderData);
			setTraderData(newTraderData);
			setCalculatedTraderData(newCalculatedTraderData);
		},
		[calculatedTraderData, prevTraderData, traderData]
	);

	return [calculatedTraderData, setStateWithPrevious] as const;
};

export default useTraderData;
