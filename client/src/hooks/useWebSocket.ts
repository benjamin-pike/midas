import { useEffect, useRef } from "react";
import { connectWebSocket, closeWebSocket } from "../api/websocket";

const useWebSocket = (traderId: string, onUpdate: (data: any) => void) => {
	const onUpdateRef = useRef(onUpdate);

	useEffect(() => {
		onUpdateRef.current = onUpdate;
	}, [onUpdate]);

	useEffect(() => {
		const messageHandler = (data: any) => {
			onUpdateRef.current(data);
		};

		connectWebSocket(traderId, messageHandler);

		return () => {
			closeWebSocket();
		};
	}, [traderId]);
};

export default useWebSocket;
