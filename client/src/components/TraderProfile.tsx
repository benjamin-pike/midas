import React from "react";
import { Card, CardContent, CardHeader } from "./ui/card";
import { Avatar, AvatarFallback, AvatarImage } from "./ui/avatar";
import TrendArrow from "./TrendArrow";
import { formatMoneyElement, formatNumberElement } from "../lib/utils";
import { useTheme } from "./providers/theme-provider";
import { CalculatedTraderData } from "../lib/types";

interface TraderProfileProps {
	data: CalculatedTraderData;
}

const TraderProfile: React.FC<TraderProfileProps> = ({ data }) => {
	const { theme } = useTheme();

	const drawdownIndicatorColor = (() => {
		if (!data.maxDrawdown || data.maxDrawdown < 10) {
			return "text-green-500";
		}

		if (data.maxDrawdown < 20) {
			return "text-yellow-500";
		}

		if (data.maxDrawdown < 30) {
			return "text-orange-500";
		}

		return "text-red-500";
	})();

	return (
		<Card className="p-2">
			<CardHeader>
				<div className="flex gap-3 items-center">
					<Avatar
						className={`border-1 h-10 w-10 ${
							theme === "light"
								? "border-gray-300"
								: "border-zinc-700"
						}`}
					>
						<AvatarImage src="" />
						<AvatarFallback>{data.initials}</AvatarFallback>
					</Avatar>
					<p
						className={`border-b leading-10 w-full ${
							theme === "light"
								? "border-gray-300"
								: "border-zinc-800"
						}`}
					>
						<span className="font-semibold">
							{data.fname} {data.lname}
						</span>
						<span
							className={
								theme === "light"
									? "text-gray-300"
									: "text-zinc-600"
							}
						>
							{" "}
							｜{" "}
						</span>
						<span
							className={
								theme === "light"
									? "text-gray-600"
									: "text-zinc-200"
							}
						>
							{data.id}
						</span>
					</p>
				</div>
			</CardHeader>
			<CardContent>
				<div className="grid grid-cols-3 gap-3">
					<div className="flex flex-col gap-1">
						<label className="text-sm">Closed Trades</label>
						<p className="whitespace-pre">{data.closedTrades}</p>
					</div>
					<div className="flex flex-col gap-1">
						<label className="text-sm">Win Rate</label>
						<p className="whitespace-pre">
							{data.closedTrades ? <>{formatNumberElement(100 * data.winRate.value, 1)}<span className="text-xs"> %</span></> : "—"}{" "}
							<TrendArrow
								trend={data.winRate.trend}
								isFavorableMetric={true}
							/>
						</p>
					</div>
					<div className="flex flex-col gap-1">
						<label className="text-sm">Open Orders</label>
						<p className="whitespace-pre">
							{data.openOrders.total.value}{" "}
							<span className="text-sm">
								({data.openOrders.asks.value} |{" "}
								{data.openOrders.bids.value})
							</span>{" "}
							<TrendArrow trend={data.openOrders.total.trend} />
						</p>
					</div>
					<div className="flex flex-col gap-1">
						<label className="text-sm">Holdings</label>
						<p className="whitespace-pre">
							{data.inventory.value}{" "}
							<TrendArrow trend={data.inventory.trend} />
						</p>
					</div>
					<div className="flex flex-col gap-1">
						<label className="text-sm">Avg. Entry Price</label>
						<p className="whitespace-pre">
							{formatMoneyElement(data.avgEntryPrice.value)}{" "}
							<TrendArrow
								trend={data.avgEntryPrice.trend}
								isFavorableMetric={false}
							/>
						</p>
					</div>
					<div className="flex flex-col gap-1">
						<label className="text-sm">Avg. Exit Price</label>
						<p className="whitespace-pre">
							{data.closedTrades
								? formatMoneyElement(data.avgExitPrice.value)
								: "—"}{" "}
							<TrendArrow
								trend={data.avgExitPrice.trend}
								isFavorableMetric={true}
							/>
						</p>
					</div>
					<div className="flex flex-col gap-1">
						<label className="text-sm">Realised P/L</label>
						<p className="whitespace-pre">
							{data.closedTrades
								? formatMoneyElement(data.realizedPnL.value)
								: "—"}{" "}
							<TrendArrow
								trend={data.realizedPnL.trend}
								isFavorableMetric={true}
							/>
						</p>
					</div>
					<div className="flex flex-col gap-1">
						<label className="text-sm">Unrealised P/L</label>
						<p className="whitespace-pre">
							{formatMoneyElement(data.unrealizedPnL.value)}
							<TrendArrow trend={data.unrealizedPnL.trend} />
						</p>
					</div>
					<div className="flex flex-col gap-1">
						<label className="text-sm">Max Drawdown</label>
						<p className="whitespace-pre">
							{data.maxDrawdown
								? `${data.maxDrawdown.toFixed(2)}%`
								: "—"}
							{"  "}
							<span
								className={`text-sm ${drawdownIndicatorColor}`}
							>
								◆
							</span>
						</p>
					</div>
				</div>
			</CardContent>
		</Card>
	);
};

export default TraderProfile;
