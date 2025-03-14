import React from "react";
import { Card, CardContent, CardHeader, CardTitle } from "./ui/card";
import {
	Table,
	TableHeader,
	TableRow,
	TableHead,
	TableBody,
	TableCell
} from "./ui/table";
import {
	formatMoneyElement,
	formatTimestamp,
	getTypeColor
} from "../lib/utils";
import { ScrollArea } from "./ui/scroll-area";
import { Badge } from "./ui/badge";
import { orderTypeLabels, Trade } from "../lib/types";
import { useTheme } from "./providers/theme-provider";

interface TradeHistoryProps {
	trades: Trade[];
}

const TradeHistory: React.FC<TradeHistoryProps> = ({ trades }) => {
	const { theme } = useTheme();
	const isDark = theme === "dark";

	return (
		<Card className="p-2 flex flex-col h-full">
			<CardHeader className="pb-3">
				<CardTitle>Trade History</CardTitle>
			</CardHeader>
			<div
				className={`relative m-6 mt-0 flex-grow overflow-auto border-b ${
					isDark ? "border-zinc-800" : "border-zinc-200"
				}`}
			>
				<CardContent className="p-0 absolute inset-0">
					<ScrollArea className="w-full h-full">
						<Table>
							<TableHeader
								className={`sticky top-0 ${
									isDark
										? "bg-card text-zinc-200"
										: "bg-white text-zinc-900"
								}`}
								style={{ bottom: "-1px" }}
							>
								<TableRow
									className={`${
										isDark
											? "hover:bg-card shadow-[inset_0_-1px_0_theme(colors.zinc.800)]"
											: "hover:bg-card shadow-[inset_0_-1px_0_theme(colors.zinc.200)]"
									}`}
								>
									<TableHead className="pl-6">ID</TableHead>
									<TableHead>Buy Order</TableHead>
									<TableHead>Sell Order</TableHead>
									<TableHead>Quantity</TableHead>
									<TableHead>Price</TableHead>
									<TableHead className="pr-6">Time</TableHead>
								</TableRow>
							</TableHeader>
							<TableBody>
								{trades.map((trade, i) => (
									<TableRow
										key={trade.tradeId}
										className={`${
											isDark
												? i % 2 === 1
													? "hover:bg-zinc-700"
													: "bg-zinc-900 hover:bg-zinc-700"
												: i % 2 === 1
												? "bg-zinc-50"
												: "bg-white hover:bg-zinc-100"
										}`}
									>
										<TableCell className="pl-6">
											{trade.tradeId}
										</TableCell>
										<TableCell>
											{trade.buyOrderId}
											<Badge
												variant={"outline"}
												className={`ml-3 ${
													isDark
														? "bg-card border-zinc-700 text-zinc-300"
														: "bg-white border-zinc-200 text-zinc-900"
												}`}
											>
												<span
													className={`inline-block w-2 h-2 rounded-full ${getTypeColor(
														trade.buyOrderType
													)} mr-2`}
												/>

												{
													orderTypeLabels[
														trade.buyOrderType
													]
												}
											</Badge>
										</TableCell>
										<TableCell>
											{trade.sellOrderId}
											<Badge
												variant={"outline"}
												className={`ml-3 ${
													isDark
														? "bg-card border-zinc-700 text-zinc-300"
														: "bg-white border-zinc-200 text-zinc-900"
												}`}
											>
												<span
													className={`inline-block w-2 h-2 rounded-full ${getTypeColor(
														trade.sellOrderType
													)} mr-2`}
												/>

												{
													orderTypeLabels[
														trade.sellOrderType
													]
												}
											</Badge>
										</TableCell>
										<TableCell>{trade.quantity}</TableCell>
										<TableCell>
											{formatMoneyElement(trade.price)}
										</TableCell>
										<TableCell className="pr-6 whitespace-pre">
											{formatTimestamp(trade.timestamp)}
										</TableCell>
									</TableRow>
								))}
							</TableBody>
						</Table>
					</ScrollArea>
				</CardContent>
			</div>
		</Card>
	);
};

export default TradeHistory;
