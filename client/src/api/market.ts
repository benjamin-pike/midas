import axios from "axios";
import { MarketData } from "../lib/types";
import { toast } from "sonner";

export const API_BASE = "http://localhost:8080/market";

const apiClient = axios.create({
	baseURL: API_BASE,
	headers: {
		"Content-Type": "application/json"
	}
});

export async function getMarketData(): Promise<MarketData> {
	try {
		const response = await apiClient.get("");
		return response.data.marketData;
	} catch (err: any) {
		toast.error(
			err.response.data || err.response?.statusText || err.message
		);
		throw new Error(
			err.response.data || err.response?.statusText || err.message
		);
	}
}
