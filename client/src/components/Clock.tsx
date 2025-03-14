import { useState, useEffect } from "react";

const Clock = () => {
	const [time, setTime] = useState(() =>
		new Date().toLocaleTimeString("en-GB", { hour12: false })
	);

	useEffect(() => {
		const interval = setInterval(() => {
			setTime(new Date().toLocaleTimeString("en-GB", { hour12: false }));
		}, 1000);

		return () => clearInterval(interval);
	}, []);

	return time;
};

export default Clock;
