export enum OrderSide {
	ASK = "ASK",
	BID = "BID"
}

export enum OrderMechanism {
    ACTIVE = "ACTIVE",
    CONDITIONAL = "CONDITIONAL"
}

export enum OrderType {
	LIMIT = "LIMIT",
	MARKET = "MARKET",
	IOC = "IOC",
	FOK = "FOK",
	ICEBERG = "ICEBERG",
	STOP = "STOP",
	STOP_LIMIT = "STOP_LIMIT",
	TRAILING_STOP = "TRAILING_STOP"
}

export enum OrderStatus {
	UNFILLED = "UNFILLED",
	PARTIALLY_FILLED = "PARTIALLY_FILLED",
	FILLED = "FILLED",
	CANCELLED = "CANCELLED"
}

export interface Order {
	id: number;
	side: OrderSide;
	orderType: OrderType;
	price: number | null;
	initialQuantity: number;
	remainingQuantity: number;
	status: OrderStatus;
	traderId: string;
	timestamp: number;
	displaySize?: number;
    limitPrice?: number;
    bestPrice?: number;
}

export type OrderPayload = Omit<
	Order,
	"id" | "initialQuantity" | "remainingQuantity" | "status" | "timestamp"
> & {
	quantity: number;
};

export const orderTypeLabels = {
	[OrderType.LIMIT]: "LIMIT",
	[OrderType.MARKET]: "MARKET",
	[OrderType.IOC]: "IOC",
	[OrderType.FOK]: "FOK",
	[OrderType.ICEBERG]: "ICEBERG",
	[OrderType.STOP]: "STOP",
	[OrderType.STOP_LIMIT]: "STOP LIMIT",
	[OrderType.TRAILING_STOP]: "TRAILING STOP"
} as const;

export const orderStatusLabels = {
	[OrderStatus.UNFILLED]: "UNFILLED",
	[OrderStatus.PARTIALLY_FILLED]: "PART FILLED",
	[OrderStatus.FILLED]: "FILLED",
	[OrderStatus.CANCELLED]: "CANCELLED"
} as const;

export interface Trade {
	tradeId: number;
	buyOrderId: number;
	sellOrderId: number;
	buyOrderType: OrderType;
	sellOrderType: OrderType;
	quantity: number;
	price: number;
	timestamp: number;
}

export interface Trader {
	id: string;
	name: string;
	inventory: number;
	openOrders: {
		bids: number;
		asks: number;
	};
	closedTrades: number;
	wins: number;
	avgEntryPrice: number;
	avgExitPrice: number;
	realizedPnL: number;
	unrealizedPnL: number;
	maxDrawdown: number;
}

export interface MarketData {
	currentPrice: number;
	volatility: number;
	bids: {
		best: Order | null;
		volume: number;
		count: number;
	};
	asks: {
		best: Order | null;
		volume: number;
		count: number;
	};
	trades: {
		volume: number;
		count: number;
		avgPrice: number | null;
	};
}

export type Trend = 1 | -1 | 0;
type DataPoint = { value: number; trend: Trend };

export interface CalculatedTraderData {
	id: string;
	fname: string;
    lname: string
    initials: string;
	inventory: DataPoint;
	openOrders: {
        total: DataPoint;
		bids: DataPoint;
		asks: DataPoint;
	};
	closedTrades: number;
	winRate: DataPoint;
	avgEntryPrice: DataPoint;
	avgExitPrice: DataPoint;
	realizedPnL: DataPoint;
	unrealizedPnL: DataPoint;
	maxDrawdown: number;
}

export interface CalculatedMarketData {
	currentPrice: DataPoint;
	volatility: DataPoint;
	trades: {
		count: number;
		volume: number;
		avgPrice: DataPoint;
	};
	bids: {
		count: DataPoint;
		volume: DataPoint;
		bestPrice: DataPoint;
		bestQuantity: DataPoint;
	};
	asks: {
		count: DataPoint;
		volume: DataPoint;
		bestPrice: DataPoint;
		bestQuantity: DataPoint;
	};
	spread: DataPoint;
	imbalance: DataPoint;
}

export enum RiskScope {
	GLOBAL = "GLOBAL",
	TRADER = "TRADER"
}

export interface RiskLimits {
    maxOpenPosition: number;
    maxOrderSize: number;
    maxOrdersPerMin: number;
    maxDailyLoss: number;
    maxDrawdown: number;
    maxRiskPerOrder: number;
}

export interface RiskPayload {
    scope: RiskScope;
    traderId?: string;
    override: boolean;
    limits: Partial<RiskLimits>;
}

// --------------------------------------------------------------------------------
// Typeguards
// --------------------------------------------------------------------------------

export const isRiskScope = (value: string): value is RiskScope =>
	Object.values(RiskScope).includes(value as RiskScope);

export const isOrderSide = (value: string): value is OrderSide =>
	Object.values(OrderSide).includes(value as OrderSide);
