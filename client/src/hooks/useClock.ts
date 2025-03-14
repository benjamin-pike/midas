import { useState, useEffect } from "react";

const useClock = () => {
	const [locale] = useState(navigator.language || "en-GB");
	const [time, setTime] = useState(() =>
		new Date().toLocaleTimeString(locale, { hour12: false })
	);

	useEffect(() => {
		const interval = setInterval(() => {
			setTime(new Date().toLocaleTimeString(locale, { hour12: false }));
		}, 1000);

		return () => clearInterval(interval);
	}, [locale]);

	return time;
};

export default useClock;
