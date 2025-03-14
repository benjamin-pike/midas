import axios from "axios";
import { Order, OrderPayload } from "../lib/types";
import { toast } from "sonner";

export const API_BASE = "http://localhost:8080/orders";

export interface GetOrdersResponse {
    asks: {
        active: Order[];
        conditional: Order[];
    },
    bids: {
        active: Order[];
        conditional: Order[];
    }
}

const apiClient = axios.create({
	baseURL: API_BASE,
	headers: {
		"Content-Type": "application/json"
	}
});

export async function createOrder(payload: OrderPayload): Promise<boolean> {
	try {
		await apiClient.post("", payload);
        return true;
	} catch (_err: any) {
        return false
	}
}

export async function getOrders(
    start: number = 0,
    limit: number = 100
): Promise<GetOrdersResponse> {
    try {
        const response = await apiClient.get("", {
            params: { start, limit }
        });
        return response.data;
    } catch (err: any) {
        toast.error(
            err.response.data || err.response?.statusText || err.message
        );
        throw new Error(
            err.response.data || err.response?.statusText || err.message
        );
    }
}