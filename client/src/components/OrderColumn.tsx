import React, { useCallback } from "react";
import {
	Table,
	TableHeader,
	TableRow,
	TableHead,
	TableBody,
	TableCell
} from "./ui/table";
import { Badge } from "./ui/badge";
import { Order, OrderMechanism, orderStatusLabels, orderTypeLabels } from "../lib/types";
import {
	cn,
	formatMoneyElement,
	formatTimestamp,
	getStatusColor,
	getTypeColor
} from "../lib/utils";
import { useTheme } from "./providers/theme-provider";

interface OrderColumnProps {
	orders: Order[];
	mechanism: OrderMechanism;
}

const OrderColumn: React.FC<OrderColumnProps> = ({ orders, mechanism }) => {
	const { theme } = useTheme();
	const isDark = theme === "dark";

	const isConditional = OrderMechanism.CONDITIONAL === mechanism;

	const getLimitBestPriceValue = useCallback((order: Order) => {
		if (order.orderType === "STOP_LIMIT") {
			return order.limitPrice;
		}
		if (order.orderType === "TRAILING_STOP") {
			return order.bestPrice;
		}
	}, []);

	return (
		<Table>
			<TableHeader
				className={`sticky top-0 ${
					isDark ? "bg-card text-zinc-200" : "bg-white text-zinc-900"
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
					<TableHead>Type</TableHead>
					<TableHead>Quantity</TableHead>
					<TableHead>{isConditional && "Trigger "}Price</TableHead>
					{isConditional ? (
						<TableHead>
							Threshold Price
						</TableHead>
					) : (
						<TableHead>Status</TableHead>
					)}
					<TableHead>Trader ID</TableHead>
					<TableHead className="pr-6">Added</TableHead>
				</TableRow>
			</TableHeader>
			<TableBody>
				{orders.map((order, i) => (
					<TableRow
						key={order.id}
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
						<TableCell className="pl-6">{order.id}</TableCell>
						<TableCell>
							<Badge
								variant={"outline"}
								className={`${
									isDark
										? "bg-card border-zinc-700 text-zinc-300"
										: "bg-white border-zinc-200 text-zinc-900"
								}`}
							>
								<span
									className={`inline-block w-2 h-2 rounded-full ${getTypeColor(
										order.orderType
									)} mr-2`}
								/>
								{orderTypeLabels[order.orderType]}
							</Badge>
						</TableCell>
						<TableCell>
							{order.remainingQuantity}
							<span
								className={
									isDark ? "text-zinc-400" : "text-zinc-500"
								}
							>
								{order.initialQuantity !==
									order.remainingQuantity &&
									` (${order.initialQuantity})`}
							</span>
						</TableCell>
						<TableCell>
							{order.price
								? formatMoneyElement(order.price)
								: "—"}
						</TableCell>
						{isConditional ? (
							<TableCell>
								{order.orderType === "STOP_LIMIT" ||
								order.orderType === "TRAILING_STOP" ? (
									<p className="whitespace-pre">
										{formatMoneyElement(
											getLimitBestPriceValue(order) ?? 0
										)}
										{"  "}
										<span
											className={cn(
												"text-xs",
												isDark
													? "text-zinc-400"
													: "text-zinc-500"
											)}
										>
											{order.orderType === "STOP_LIMIT"
												? "LIMIT"
												: "BEST"}
										</span>
									</p>
								) : (
									"—"
								)}
							</TableCell>
						) : (
							<TableCell>
								<Badge
									variant={"outline"}
									className={`${
										isDark
											? "bg-card border-zinc-700 text-zinc-300"
											: "bg-white border-zinc-200 text-zinc-900"
									}`}
								>
									<span
										className={`inline-block w-2 h-2 rounded-full ${getStatusColor(
											order.status
										)} mr-2`}
									/>
									{orderStatusLabels[order.status]}
								</Badge>
							</TableCell>
						)}
						<TableCell>{order.traderId}</TableCell>
						<TableCell className="pr-6 whitespace-pre">
							{formatTimestamp(order.timestamp)}
						</TableCell>
					</TableRow>
				))}
			</TableBody>
		</Table>
	);
};

export default OrderColumn;
