import React, { useCallback, useEffect, useMemo, useState } from "react";
import MarketDataBanner from "../components/MarketDataBanner";
import OrderColumn from "../components/OrderColumn";
import TradeHistory from "../components/TradeHistory";
import NewOrderForm from "../components/NewOrderForm";
import {
	Order,
	OrderMechanism,
	OrderPayload,
	RiskLimits,
	RiskPayload,
	Trade
} from "../lib/types";
import {
	Tabs,
	TabsContent,
	TabsList,
	TabsTrigger
} from "../components/ui/tabs";
import { Card, CardContent, CardHeader } from "../components/ui/card";
import { createOrder, getOrders } from "../api/order";
import { getTrader } from "../api/traders";
import useWebSocket from "../hooks/useWebSocket";
import RiskManagerForm from "../components/RiskManagerForm";
import { getTrades } from "../api/trades";
import { useTheme } from "../components/providers/theme-provider";
import { ScrollArea } from "../components/ui/scroll-area";
import { getTraderIdFromUrl } from "../lib/utils";
import { getMarketData } from "../api/market";
import Loading from "../components/Loading";
import useMarketData from "../hooks/useMarketData";
import useTraderData from "../hooks/useTraderData";
import TraderProfile from "../components/TraderProfile";
import { getRiskLimits, updateRiskLimits } from "../api/risk";
import { toast } from "sonner";

const Dashboard: React.FC = () => {
	const { theme } = useTheme();

	const traderId = useMemo(() => getTraderIdFromUrl(), []);

	const [activeAsks, setActiveAsks] = useState<Order[]>();
	const [activeBids, setActiveBids] = useState<Order[]>();
	const [conditionalAsks, setConditionalAsks] = useState<Order[]>();
	const [conditionalBids, setConditionalBids] = useState<Order[]>();

	const [trades, setTrades] = useState<Trade[]>();
	const [traderData, setTraderData] = useTraderData();
	const [marketData, setMarketData] = useMarketData();
	const [riskLimits, setRiskLimits] = useState<RiskLimits>();

	const [isLoadingState, setIsLoadingState] = useState(true);

	const [mechanism, setMechanism] = useState<OrderMechanism>(
		OrderMechanism.ACTIVE
	);

	const handleMechanismValueChange = useCallback((value: string) => {
		setMechanism(value as OrderMechanism);
	}, []);

	const handleNewOrder = useCallback(
		async (payload: Omit<OrderPayload, "traderId">) => {
			const order = {
				traderId: traderId,
				orderType: payload.orderType,
				side: payload.side,
				price: payload.price,
				quantity: payload.quantity,
				displaySize: payload.displaySize,
				limitPrice: payload.limitPrice,
				bestPrice: payload.bestPrice
			};

			return await createOrder(order);
		},
		[traderId]
	);

	const handleNewRiskLimits = useCallback(async (payload: RiskPayload) => {
		return await updateRiskLimits(payload);
	}, []);

	useEffect(() => {
		if (
			activeAsks ||
			activeBids ||
			conditionalAsks ||
			conditionalBids ||
			trades ||
			traderData ||
			marketData ||
			riskLimits
		) {
			return;
		}

		setIsLoadingState(true);

		(async () => {
			const { asks, bids } = await getOrders();
			const trades = await getTrades();
			const trader = await getTrader(traderId);
			const marketData = await getMarketData();
			const riskLimits = await getRiskLimits(traderId);

			setActiveAsks(asks.active);
			setActiveBids(bids.active);
			setConditionalAsks(asks.conditional);
			setConditionalBids(bids.conditional);
			setTrades(trades);
			setTraderData(trader);
			setMarketData(marketData);
			setRiskLimits(riskLimits);

			setIsLoadingState(false);
		})();
	}, [
		activeAsks,
		activeBids,
		conditionalAsks,
		conditionalBids,
		marketData,
		riskLimits,
		setMarketData,
		setTraderData,
		traderData,
		traderId,
		trades
	]);

	const updateOpenOrders = useCallback((data: any) => {
		const { asks, bids } = data;

		setActiveAsks(asks.active);
		setActiveBids(bids.active);
		setConditionalAsks(asks.conditional);
		setConditionalBids(bids.conditional);
	}, []);

	const webSocketCallback = useCallback(
		(res: any) => {
			const { event, message, data } = res;

			if (data.market) {
				setMarketData(data.market);
			}

            console.log(res);

			switch (event) {
				case "ORDER_ADDED": {
					updateOpenOrders(data);
					break;
				}
				case "ORDER_UPDATED":
					updateOpenOrders(data);
					break;
				case "TRADE_EXECUTED":
					setTrades(data.trades);

					if (
						data.buyTraderId === traderId ||
						data.sellTraderId === traderId
					) {
						(async () => {
							setTraderData(await getTrader(traderId));
						})();
					}
					break;
				case "RISK_UPDATED":
					setRiskLimits(data.limits);
					break;
				default:
					break;
			}

            toast.info(message);
		},
		[setMarketData, setTraderData, traderId, updateOpenOrders]
	);

	useWebSocket(traderId, webSocketCallback);

	const isLoading =
		isLoadingState ||
		!activeAsks ||
		!activeBids ||
		!conditionalAsks ||
		!conditionalBids ||
		!trades ||
		!traderData ||
		!marketData ||
		!riskLimits;

	return isLoading ? (
		<Loading />
	) : (
		<div
			className={`w-screen ${
				theme === "light" ? "bg-zinc-100" : "bg-zinc-950"
			}`}
		>
			<div className="flex flex-col gap-4 mx-auto p-4 min-h-screen min-w-[1280px]">
				<MarketDataBanner data={marketData} />

				<div className="flex gap-4 items-start h-full items-stretch">
					<div className="flex-grow stretch">
						<TradeHistory trades={trades} />
					</div>
					<div className="flex flex-col flex-none gap-4">
						<TraderProfile data={traderData} />
						<NewOrderForm
							marketPrice={marketData.currentPrice.value}
							onSubmit={handleNewOrder}
						/>
					</div>
				</div>

				<div className="flex gap-4 items-start h-full items-stretch">
					<div className="flex-grow stretch">
						<Tabs defaultValue="asks" className="h-full">
							<Card className="p-2 mb-4 h-full flex flex-col">
								<CardHeader className="flex-row content-center pb-2 pt-5">
									<TabsList className="bg-transparent justify-start p-0">
										<TabsTrigger
											value="asks"
											className={`py-3 pl-5 pr-5 rounded-none shadow-none data-[state=active]:shadow-none border-b-2 ${
												theme === "dark"
													? "data-[state=active]:border-zinc-300 border-zinc-700 text-zinc-300"
													: "data-[state=active]:border-black border-zinc-200 text-zinc-900"
											}`}
										>
											Asks Orders
										</TabsTrigger>
										<TabsTrigger
											value="bids"
											className={`py-3 pl-5 pr-5 rounded-none shadow-none data-[state=active]:shadow-none border-b-2 ${
												theme === "dark"
													? "data-[state=active]:border-zinc-300 border-zinc-700 text-zinc-300"
													: "data-[state=active]:border-black border-zinc-200 text-zinc-900"
											}`}
										>
											Bid Orders
										</TabsTrigger>
									</TabsList>
									<Tabs
										defaultValue={mechanism}
										className="ml-auto"
										onValueChange={
											handleMechanismValueChange
										}
									>
										<TabsList>
											<TabsTrigger
												value={OrderMechanism.ACTIVE}
											>
												Active
											</TabsTrigger>
											<TabsTrigger
												value={
													OrderMechanism.CONDITIONAL
												}
											>
												Conditional
											</TabsTrigger>
										</TabsList>
									</Tabs>
								</CardHeader>
								<div
									className={`relative m-6 mt-0 flex-grow overflow-auto border-b ${
										theme === "light"
											? "border-zinc-200"
											: "border-zinc-800"
									}`}
								>
									<CardContent className="p-0 absolute inset-0">
										<ScrollArea className="w-full h-full">
											<TabsContent
												className="mt-0"
												value="asks"
											>
												<OrderColumn
													orders={
														mechanism ===
														OrderMechanism.ACTIVE
															? activeAsks
															: conditionalAsks
													}
													mechanism={mechanism}
												/>
											</TabsContent>
											<TabsContent
												className="mt-0"
												value="bids"
											>
												<OrderColumn
													orders={
														mechanism ===
														OrderMechanism.ACTIVE
															? activeBids
															: conditionalBids
													}
													mechanism={mechanism}
												/>
											</TabsContent>
										</ScrollArea>
									</CardContent>
								</div>
							</Card>
						</Tabs>
					</div>
					<div className="flex flex-col flex-none gap-4">
						<RiskManagerForm
							currentTraderId={traderId}
							limits={riskLimits}
							onSubmit={handleNewRiskLimits}
						/>
					</div>
				</div>
			</div>
		</div>
	);
};

export default Dashboard;
