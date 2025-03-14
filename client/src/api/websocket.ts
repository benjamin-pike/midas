import { toast } from "sonner";

const WS_URL = "ws://localhost:8080/ws";

let socket: WebSocket | null = null;
let messageHandler: ((data: any) => void) | null = null;
let shouldReconnect = true;
const reconnectInterval = 5000;

export const connectWebSocket = (
	traderId: string,
	onMessage: (data: any) => void
) => {
	if (socket) return;

	shouldReconnect = true;

	socket = new WebSocket(`${WS_URL}?traderId=${traderId}`);

	socket.onmessage = (event) => {
		if (messageHandler) {
			try {
				const data = JSON.parse(event.data);
				messageHandler(data);
			} catch (err) {
				console.error("Error parsing WebSocket message:", err);
			}
		}
	};

	socket.onerror = (error: any) => {
		if (error.message) {
			toast.error("WebSocket error: " + error.message);
		} else {
			toast.error("WebSocket error");
		}
		console.error("WebSocket error:", error);
	};

	socket.onclose = () => {
		socket = null;
		if (shouldReconnect) {
			setTimeout(() => connectWebSocket(traderId, onMessage), reconnectInterval);
		}
	};

	messageHandler = onMessage;
};

export const closeWebSocket = () => {
	shouldReconnect = false;
	if (socket && socket.readyState === WebSocket.OPEN) {
		socket.close();
		socket = null;
	}
};
