import axios from "axios";
import { Trader } from "../lib/types";
import { toast } from "sonner";

export const API_BASE = "http://localhost:8080/traders";

const apiClient = axios.create({
	baseURL: API_BASE,
	headers: {
		"Content-Type": "application/json"
	}
});

export async function getTrader(id: string): Promise<Trader> {
	try {
		const response = await apiClient.get(`/${id}`);
		return response.data.trader;
	} catch (err: any) {
		toast.error(
			err.response.data || err.response?.statusText || err.message
		);
		throw new Error(
			err.response.data || err.response?.statusText || err.message
		);
	}
}
