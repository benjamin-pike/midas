import React from "react";
import { Trend } from "../lib/types";

interface TrendArrowProps {
	trend: Trend;
	isFavorableMetric?: boolean;
}

const TrendArrow: React.FC<TrendArrowProps> = ({
	trend,
	isFavorableMetric
}) => {
	if (trend == 0) {
		return null;
	}

	const isIncreasing = trend === 1;
	const arrow = isIncreasing ? "▲" : "▼";
	let color = "text-blue-500";

	if (isFavorableMetric !== undefined) {
		if (isIncreasing) {
			color = isFavorableMetric ? "text-green-500" : "text-red-500";
		} else {
			color = isFavorableMetric ? "text-red-500" : "text-green-500";
		}
	}

	return <span className={`${color} ml-1`}>{arrow}</span>;
};

export default TrendArrow;
