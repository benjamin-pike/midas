import axios from "axios";
import { RiskLimits, RiskPayload } from "../lib/types";
import { toast } from "sonner";

export const API_BASE = "http://localhost:8080/risk";

const apiClient = axios.create({
	baseURL: API_BASE,
	headers: {
		"Content-Type": "application/json"
	}
});

export async function updateRiskLimits(payload: RiskPayload): Promise<boolean> {
	try {
		await apiClient.put("", payload);
        return true;
	} catch (_err: any) {
		return false;
	}
}

export async function getRiskLimits(traderId: string): Promise<RiskLimits> {
    try {
        const response = await apiClient.get("",{
            params: { traderId }
        });
        return response.data.limits;
    } catch (err: any) {
        toast.error(
            err.response.data || err.response?.statusText || err.message
        );
        throw new Error(
            err.response.data || err.response?.statusText || err.message
        );
    }
}