import axios from "axios";
import { Trade } from "../lib/types";
import { toast } from "sonner";

export const API_BASE = "http://localhost:8080/trades";

const apiClient = axios.create({
	baseURL: API_BASE,
	headers: {
		"Content-Type": "application/json"
	}
});

export async function getTrades(
	start: number = 0,
	limit: number = 100
): Promise<Trade[]> {
	try {
		const response = await apiClient.get("", {
			params: { start, limit }
		});
		return response.data.trades;
	} catch (err: any) {
		toast.error(
			err.response.data || err.response?.statusText || err.message
		);
		throw new Error(
			err.response.data || err.response?.statusText || err.message
		);
	}
}
