import React from "react";
import { Card, CardContent, CardHeader, CardTitle } from "./ui/card";
import TrendArrow from "./TrendArrow";
import { CalculatedMarketData } from "../lib/types";
import { cn, formatMoneyElement } from "../lib/utils";
import useClock from "../hooks/useClock";
import { useTheme } from "./providers/theme-provider";

interface MarketDataBannerProps {
	data: CalculatedMarketData;
}

const EM_DASH = "—";

const MarketDataBanner: React.FC<MarketDataBannerProps> = ({ data }) => {
	const time = useClock();
	const { theme } = useTheme();

	const isDark = theme === "dark";

	const Separator = () => (
		<div
			className={cn(
				"border-r",
				isDark ? "border-zinc-700" : "border-zinc-200"
			)}
		/>
	);

	return (
		<Card className="px-1">
			<CardHeader className="pb-2">
				<CardTitle>
					<p className="whitespace-pre">
						<span className="text-lg font-bold">Market Data</span>
						<span className="text-base"> ｜ </span>
						<span className="text-sm">{time}</span>
					</p>
				</CardTitle>
			</CardHeader>
			<CardContent className="flex justify-between">
				<div>
					<p className="text-sm">Trades</p>
					<p className="text-lg">{data.trades.count} </p>
				</div>
				<div>
					<p className="text-sm">Volume</p>
					<p className="text-lg">
						{data.trades.count ? data.trades.volume : EM_DASH}{" "}
					</p>
				</div>
				<div>
					<p className="text-sm">Avg. Price</p>
					<p className="text-lg">
						{data.trades.avgPrice.value
							? formatMoneyElement(data.trades.avgPrice.value)
							: EM_DASH}{" "}
						<TrendArrow
							trend={data.trades.avgPrice.trend}
							isFavorableMetric={true}
						/>
					</p>
				</div>
				<Separator />
				<div>
					<p className="text-sm">Market Price</p>
					<p className="text-lg">
						{formatMoneyElement(data.currentPrice.value)}{" "}
						<TrendArrow
							trend={data.currentPrice.trend}
							isFavorableMetric={true}
						/>
					</p>
				</div>
				<div>
					<p className="text-sm">Volatility</p>
					<p className="text-lg">
						{data.volatility.value.toFixed(2)}{" "}
						<TrendArrow
							trend={data.volatility.trend}
							isFavorableMetric={false}
						/>
					</p>
				</div>
				<Separator />
				<div>
					<p className="text-sm">Best Ask</p>
					<p className="text-lg">
						{data.asks?.bestPrice.value
							? formatMoneyElement(data.asks?.bestPrice.value)
							: EM_DASH}{" "}
						<TrendArrow
							trend={data.asks?.bestPrice.trend}
							isFavorableMetric={true}
						/>{" "}
						{data.asks?.bestQuantity?.value !== 0 && (
							<span className="text-sm">
								({data.asks?.bestQuantity?.value})
							</span>
						)}
					</p>
				</div>
				<div>
					<p className="text-sm">Best Bid</p>
					<p className="text-lg">
						{data.bids?.bestPrice.value
							? formatMoneyElement(data.bids?.bestPrice.value)
							: EM_DASH}{" "}
						<TrendArrow
							trend={data.bids?.bestPrice.trend}
							isFavorableMetric={true}
						/>{" "}
						{data.bids?.bestQuantity?.value !== 0 && (
							<span className="text-sm">
								({data.bids?.bestQuantity?.value})
							</span>
						)}
					</p>
				</div>
				<div>
					<p className="text-sm">Spread</p>
					<p className="text-lg">
						{data.spread.value
							? formatMoneyElement(data.spread.value)
							: EM_DASH}{" "}
						<TrendArrow
							trend={data.spread.trend}
							isFavorableMetric={false}
						/>
					</p>
				</div>
				<Separator />
				<div>
					<p className="text-sm">Bid Volume</p>
					<p className="text-lg">
						{data.bids.volume.value}{" "}
						<TrendArrow
							trend={data.bids.volume.trend}
							isFavorableMetric={true}
						/>
					</p>
				</div>
				<div>
					<p className="text-sm">Ask Volume</p>
					<p className="text-lg">
						{data.asks.volume.value}{" "}
						<TrendArrow
							trend={data.asks.volume.trend}
							isFavorableMetric={true}
						/>
					</p>
				</div>
				<div>
					<p className="text-sm">Imbalance</p>
					<p className="text-lg">
						{(data.asks.volume.value > 0 &&
							data.bids.volume.value > 0 &&
							data.imbalance.value?.toFixed(2)) ||
							EM_DASH}{" "}
						<TrendArrow trend={data.imbalance.trend} />
					</p>
				</div>
			</CardContent>
		</Card>
	);
};

export default MarketDataBanner;
