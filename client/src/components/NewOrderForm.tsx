import React, { useCallback, useEffect, useState } from "react";
import { Card, CardContent, CardHeader, CardTitle } from "./ui/card";
import { Button } from "./ui/button";
import { Input } from "./ui/input";
import {
	Select,
	SelectContent,
	SelectItem,
	SelectTrigger,
	SelectValue
} from "./ui/select";
import { isOrderSide, OrderPayload, OrderSide, OrderType } from "../lib/types";
import { Separator } from "./ui/separator";
import { Tabs, TabsList, TabsTrigger } from "./ui/tabs";
import { useTheme } from "./providers/theme-provider";
import { toast } from "sonner";

interface NewOrderFormProps {
	marketPrice: number;
	onSubmit: (order: Omit<OrderPayload, "traderId">) => Promise<boolean>;
}

const NewOrderForm: React.FC<NewOrderFormProps> = ({
	onSubmit,
	marketPrice
}) => {
	const { theme } = useTheme();

	const [side, setSide] = useState<OrderSide>(OrderSide.ASK);
	const [orderType, setOrderType] = useState<OrderType>(OrderType.LIMIT);
	const [price, setPrice] = useState<number | null>(null);
	const [quantity, setQuantity] = useState<number>(10);
	const [limitOrBestPrice, setLimitOrBestPrice] = useState<number>(100);
	const [icebergDisplay, setIcebergDisplay] = useState<number>(10);

	const isPriceVisible = orderType !== OrderType.MARKET;
	const isLimitOrBestPriceVisible =
		orderType === OrderType.STOP_LIMIT ||
		orderType === OrderType.TRAILING_STOP;
	const priceLabel =
		orderType === OrderType.STOP ||
		orderType === OrderType.STOP_LIMIT ||
		orderType === OrderType.TRAILING_STOP
			? "Trigger Price"
			: "Limit Price";
	const limitOrBestPriceLabel =
		orderType === OrderType.STOP_LIMIT ? "Limit Price" : "Best Price";

	const handleSideValueChange = useCallback((value: string) => {
		if (!isOrderSide(value)) {
			return;
		}

		setSide(value);
	}, []);

	const handleTypeChange = useCallback((value: OrderType) => {
		if (
			value === OrderType.MARKET
		) {
			setPrice(null);
		} else {
            setPrice(marketPrice);
        }

		setOrderType(value);
	}, [marketPrice]);

	const handleQuantityChange = useCallback((e: React.ChangeEvent<HTMLInputElement>) => {
		setQuantity(Number(e.target.value));
	}, []);

	const handlePriceChange = useCallback((e: React.ChangeEvent<HTMLInputElement>) => {
		const value = e.target.value;
		if (value === "") {
			setPrice(null);
		} else {
			setPrice(Number(value));
		}
	}, []);

	const handleLimitOrBestPriceChange = useCallback((
		e: React.ChangeEvent<HTMLInputElement>
	) => {
		const value = e.target.value;
		setLimitOrBestPrice(Number(value));
	}, []);

	const handleSubmit = useCallback(async (e: React.FormEvent) => {
		e.preventDefault();

		if (orderType === OrderType.LIMIT && !price) {
			toast.error("Price is required for LIMIT orders");
			return;
		}

		const newOrder = {
			side,
			orderType,
			price,
			quantity,
			...(orderType === OrderType.STOP_LIMIT && {
				limitPrice: limitOrBestPrice
			}),
			...(orderType === OrderType.TRAILING_STOP && {
				bestPrice: limitOrBestPrice
			}),
			...(orderType === OrderType.ICEBERG && {
				displaySize: icebergDisplay
			})
		};

		await onSubmit(newOrder);
	}, [icebergDisplay, limitOrBestPrice, onSubmit, orderType, price, quantity, side]);

	useEffect(() => {}, [price, marketPrice, orderType]);

	return (
		<Card className="p-2 w-110">
			<CardHeader>
				<CardTitle>Create New Order</CardTitle>
			</CardHeader>
			<CardContent>
				<form
					onSubmit={handleSubmit}
					className="grid grid-cols-1 gap-4"
				>
					<div className="flex gap-3">
						<div className="flex flex-col gap-1 flex">
							<label className="text-sm">Side</label>
							<Tabs
								defaultValue={OrderSide.ASK}
								className="flex-grow"
								onValueChange={handleSideValueChange}
							>
								<TabsList>
									<TabsTrigger value={OrderSide.ASK}>
										Ask
									</TabsTrigger>
									<TabsTrigger value={OrderSide.BID}>
										Bid
									</TabsTrigger>
								</TabsList>
							</Tabs>
						</div>
						<div className="flex grow flex-col gap-1">
							<label className="text-sm">Order Type</label>
							<Select
								value={orderType}
								onValueChange={handleTypeChange}
							>
								<SelectTrigger className="w-full">
									<SelectValue placeholder="Select Order Type" />
								</SelectTrigger>
								<SelectContent>
									<SelectItem value="LIMIT">Limit</SelectItem>
									<SelectItem value="MARKET">
										Market
									</SelectItem>
									<SelectItem value="IOC">
										Immediate or Cancel
									</SelectItem>
									<SelectItem value="FOK">
										Fill or Kill
									</SelectItem>
									<SelectItem value="ICEBERG">
										Iceberg
									</SelectItem>
									<Separator className="my-1" />
									<SelectItem value="STOP">
										Stop
									</SelectItem>
									<SelectItem value="STOP_LIMIT">
										Stop Limit
									</SelectItem>
									<SelectItem value="TRAILING_STOP">
										Trailing Stop
									</SelectItem>
								</SelectContent>
							</Select>
						</div>
					</div>
					<div className="flex gap-3">
						<div className="flex flex-col gap-1 flex-[40%]">
							<label className="text-sm">Quantity</label>
							<Input
								type="number"
								value={quantity ? quantity.toString() : ""}
								min={1}
								placeholder="Quantity"
								onChange={handleQuantityChange}
							/>
						</div>
						{isPriceVisible && (
							<div className="flex flex-col gap-1 flex-[60%]">
								<label className="text-sm">{priceLabel}</label>
								<div className="relative">
									<span
										className={`absolute left-3 text-sm top-1/2 -translate-y-1/2 ${
											theme === "light"
												? price
													? "text-zinc-700"
													: "text-zinc-500"
												: price
												? "text-zinc-100"
												: "text-zinc-400"
										}`}
									>
										$
									</span>
									<Input
										className="pl-7"
										type="number"
										value={price?.toString() ?? ""}
										step={0.01}
										min={0}
										prefix="$"
										placeholder="Price"
										onChange={handlePriceChange}
									/>
								</div>
							</div>
						)}
					</div>
					{isLimitOrBestPriceVisible && (
						<div className="flex flex-col gap-1">
							<label className="text-sm">
								{limitOrBestPriceLabel}
							</label>
							<div className="relative">
								<span
									className={`absolute left-3 text-sm top-1/2 -translate-y-1/2 ${
										theme === "light"
											? price
												? "text-zinc-700"
												: "text-zinc-500"
											: price
											? "text-zinc-100"
											: "text-zinc-400"
									}`}
								>
									$
								</span>
								<Input
									className="pl-7"
									type="number"
									prefix="$"
									value={
										limitOrBestPrice
											? limitOrBestPrice.toString()
											: ""
									}
									min={0}
									placeholder={limitOrBestPriceLabel}
									onChange={handleLimitOrBestPriceChange}
								/>
							</div>
						</div>
					)}
					{orderType === "ICEBERG" && (
						<div className="flex flex-col gap-1">
							<label className="text-sm">Display Quantity</label>
							<Input
								type="number"
								value={icebergDisplay}
								onChange={(e) =>
									setIcebergDisplay(Number(e.target.value))
								}
							/>
						</div>
					)}
					<Separator />
					<Button type="submit">Submit Order</Button>
				</form>
			</CardContent>
		</Card>
	);
};

export default NewOrderForm;
