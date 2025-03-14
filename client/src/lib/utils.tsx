import { clsx, type ClassValue } from "clsx";
import { twMerge } from "tailwind-merge";
import { isNullish, OrderStatus, OrderType, Trend } from "./types";

export function cn(...inputs: ClassValue[]) {
	return twMerge(clsx(inputs));
}

export function toTitleCase(str: string) {
	return str
		.toLowerCase()
		.split(" ")
		.map((word) => {
			return word.charAt(0).toUpperCase() + word.slice(1);
		})
		.join(" ");
}

export const formatPrice = (price: number) => {
	return new Intl.NumberFormat("en-US", {
		style: "currency",
		currency: "USD",
		minimumFractionDigits: 2,
		maximumFractionDigits: 2
	}).format(price);
};

export const formatTimestamp = (timestamp: number) => {
	const date = new Date(timestamp);
	const today = new Date();
	const yesterday = new Date(today);

	yesterday.setDate(today.getDate() - 1);

	if (date.toDateString() === today.toDateString()) {
		return date.toLocaleTimeString();
	} else if (date.toDateString() === yesterday.toDateString()) {
		return "Yesterday";
	} else {
		return date.toLocaleDateString();
	}
};

export const formatNumberElement = (number: number, decimalPlaces: number) => {
	const numberString = number.toFixed(decimalPlaces);
	const [integerPart, decimalPart] = numberString.split(".");

	return (
		<span>
			{parseInt(integerPart, 10).toLocaleString()}
			{decimalPart !== undefined && (
				<>
					.<span className="text-xs">{decimalPart}</span>
				</>
			)}
		</span>
	);
};

export const formatMoneyElement = (amount: number) => {
	return <span>${formatNumberElement(amount, 2)}</span>;
};

export const getTypeColor = (type: OrderType) => {
	switch (type) {
		case OrderType.LIMIT:
			return "bg-red-500";
		case OrderType.MARKET:
			return "bg-green-500";
		case OrderType.IOC:
			return "bg-yellow-500";
		case OrderType.FOK:
			return "bg-purple-500";
		case OrderType.ICEBERG:
			return "bg-blue-500";
		case OrderType.STOP:
			return "bg-green-500";
		case OrderType.STOP_LIMIT:
			return "bg-red-500";
		case OrderType.TRAILING_STOP:
			return "bg-orange-500";
	}
};

export const getStatusColor = (status: OrderStatus) => {
	switch (status) {
		case OrderStatus.UNFILLED:
			return "bg-blue-500";
		case OrderStatus.PARTIALLY_FILLED:
			return "bg-orange-500";
		default:
			return "bg-gray-500";
	}
};

export const getTraderIdFromUrl = () => {
	const pathSegments = window.location.pathname.split("/");

	if (pathSegments.length == 2 && pathSegments[1] !== "") {
		return pathSegments[1];
	}

	const traderId =
		100 +
		Math.floor(Math.random() * 900) +
		"-" +
		(100 + Math.floor(Math.random() * 900)) +
		"-" +
		(100 + Math.floor(Math.random() * 900));

	window.history.pushState({}, "", `/${traderId}`);

	return traderId;
};

export const getTrend = (
	newVal: number | undefined,
	oldVal: number | undefined,
	currTrend: Trend | undefined
) => {
	if (isNullish(newVal) && isNullish(oldVal)) {
		return 0;
	}

	if (isNullish(newVal)) {
		return -1;
	}

	if (isNullish(oldVal)) {
		return 1;
	}

	if (newVal > oldVal) {
		return 1;
	}

	if (newVal < oldVal) {
		return -1;
	}

	return currTrend ?? 0;
};
