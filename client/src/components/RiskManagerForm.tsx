import React, { useCallback, useState } from "react";
import { Card, CardContent, CardHeader, CardTitle } from "./ui/card";
import { Button } from "./ui/button";
import { Input } from "./ui/input";
import { isRiskScope, RiskLimits, RiskPayload, RiskScope } from "../lib/types";
import { Tabs, TabsList, TabsTrigger } from "./ui/tabs";
import { Checkbox } from "./ui/checkbox";
import { Separator } from "./ui/separator";
import useNumericChangeHandler from "../hooks/useNumericChangeHandler";
import { formatMoneyElement, formatNumberElement } from "../lib/utils";

const HALF_SPACE = " ";

interface RiskManagerFormProps {
	currentTraderId: string;
	limits: RiskLimits;
	onSubmit: (order: RiskPayload) => Promise<boolean>;
}

const RiskManagerForm: React.FC<RiskManagerFormProps> = ({
	currentTraderId,
	limits,
	onSubmit
}) => {
	const [scope, setScope] = useState<RiskScope>(RiskScope.GLOBAL);
	const [traderId, setTraderId] = useState(currentTraderId);
	const [override, setOverride] = useState(false);
	const [maxOpenPosition, setMaxOpenPosition] = useState<number | null>(null);
	const [maxOrderSize, setMaxOrderSize] = useState<number | null>(null);
	const [maxOrdersPerMin, setMaxOrdersPerMin] = useState<number | null>(null);
	const [maxDailyLoss, setMaxDailyLoss] = useState<number | null>(null);
	const [maxDrawdown, setMaxDrawdown] = useState<number | null>(null);
	const [maxRiskPerOrder, setMaxRiskPerOrder] = useState<number | null>(null);

	const handleScopeValueChange = useCallback((value: string) => {
		if (!isRiskScope(value)) return;
		setScope(value);
	}, []);

	const handleTraderIdChange = useCallback(
		(e: React.ChangeEvent<HTMLInputElement>) => {
			setTraderId(e.target.value);
		},
		[]
	);

	const handleOverrideChange = useCallback((checked: boolean) => {
		setOverride(checked);
	}, []);

	const handleMaxOpenPositionChange =
		useNumericChangeHandler(setMaxOpenPosition);
	const handleMaxOrderSizeChange = useNumericChangeHandler(setMaxOrderSize);
	const handleMaxOrdersPerMinChange =
		useNumericChangeHandler(setMaxOrdersPerMin);
	const handleMaxDailyLossChange = useNumericChangeHandler(setMaxDailyLoss);
	const handleMaxDrawdownChange = useNumericChangeHandler(setMaxDrawdown);
	const handleMaxRiskPerOrderChange =
		useNumericChangeHandler(setMaxRiskPerOrder);

	const handleSubmit = useCallback(
		async (e: React.FormEvent) => {
			e.preventDefault();

			const newRiskLimits = {
				scope,
				traderId,
				override,
				limits: {
					maxOpenPosition: maxOpenPosition ?? undefined,
					maxOrderSize: maxOrderSize ?? undefined,
					maxOrdersPerMin: maxOrdersPerMin ?? undefined,
					maxDailyLoss: maxDailyLoss ?? undefined,
					maxDrawdown: maxDrawdown ?? undefined,
					maxRiskPerOrder: maxRiskPerOrder ?? undefined
				}
			};

			const success = await onSubmit(newRiskLimits);

			if (success) {
				setTraderId("");
				setOverride(false);
				setMaxOpenPosition(null);
				setMaxOrderSize(null);
				setMaxOrdersPerMin(null);
				setMaxDailyLoss(null);
				setMaxDrawdown(null);
				setMaxRiskPerOrder(null);
			}
		},
		[
			maxDailyLoss,
			maxDrawdown,
			maxOpenPosition,
			maxOrderSize,
			maxOrdersPerMin,
			maxRiskPerOrder,
			onSubmit,
			override,
			scope,
			traderId
		]
	);

	return (
		<Card className="p-2 w-115">
			<CardHeader>
				<CardTitle>Risk Management</CardTitle>
			</CardHeader>
			<CardContent>
				<form
					onSubmit={handleSubmit}
					className="grid grid-cols-1 gap-4"
				>
					<div className="flex flex-col gap-4">
						<p className="text-sm font-semibold	">Current Limits</p>
						<div className="mb-1 flex flex-grow justify-between">
							<div className="flex flex-col gap-1">
								<label className="border-b border-zinc-700 pb-2 text-xs">
									Daily Loss
								</label>
								<p>{formatMoneyElement(limits.maxDailyLoss)}</p>
							</div>
							<div className="flex flex-col gap-1">
								<label className="border-b border-zinc-700 pb-2 text-xs">
									Drawdown
								</label>
								<p>
									{formatNumberElement(limits.maxDrawdown, 1)}
									<span className="text-xs">
										{HALF_SPACE}%
									</span>
								</p>
							</div>
							<div className="flex flex-col gap-1">
								<label className="border-b border-zinc-700 pb-2 text-xs">
									Position
								</label>
								<p>{limits.maxOpenPosition.toLocaleString()}</p>
							</div>
							<div className="flex flex-col gap-1">
								<label className="border-b border-zinc-700 pb-2 text-xs">
									Size
								</label>
								<p>{limits.maxOrderSize.toLocaleString()}</p>
							</div>
							<div className="flex flex-col gap-1">
								<label className="border-b border-zinc-700 pb-2 text-xs">
									Rate
								</label>
								<p>
									{limits.maxOrdersPerMin.toLocaleString()}
									<span className="text-xs">
										{HALF_SPACE}/{HALF_SPACE}min
									</span>
								</p>
							</div>
							<div className="flex flex-col gap-1">
								<label className="border-b border-zinc-700 pb-2 text-xs">
									Risk
								</label>
								<p>
									{formatNumberElement(
										limits.maxRiskPerOrder,
										1
									)}
									<span className="text-xs">
										{HALF_SPACE}%
									</span>
								</p>
							</div>
						</div>
						<Separator />
						<div className="flex gap-3">
							<div className="flex flex-col gap-1">
								<label className="text-sm">Scope</label>
								<Tabs
									defaultValue={RiskScope.GLOBAL}
									className="flex-grow"
									onValueChange={handleScopeValueChange}
								>
									<TabsList>
										<TabsTrigger value={RiskScope.GLOBAL}>
											Global
										</TabsTrigger>
										<TabsTrigger value={RiskScope.TRADER}>
											Trader
										</TabsTrigger>
									</TabsList>
								</Tabs>
							</div>
							<div className="flex flex-col gap-1 flex-grow">
								<label className="text-sm">Trader ID</label>
								<Input
									type="text"
									value={traderId}
									disabled={scope !== RiskScope.TRADER}
									placeholder="Trader ID"
									onChange={handleTraderIdChange}
								/>
							</div>
						</div>
					</div>
					{scope !== RiskScope.TRADER && (
						<div className="ml-2 flex gap-2 items-center">
							<Checkbox
								id="override-existing"
								checked={override}
								onCheckedChange={handleOverrideChange}
							/>
							<label className="text-sm">
								Override all existing trader-specific risk limits
							</label>
						</div>
					)}
					<Separator />
					<div className="flex gap-3 flex-grow">
						<div className="flex flex-col gap-1 flex-grow">
							<label className="text-sm">Max Daily Loss</label>
							<Input
								type="number"
								value={maxDailyLoss?.toString() ?? ""}
								placeholder="Daily loss"
								onChange={handleMaxDailyLossChange}
							/>
						</div>
						<div className="flex flex-col gap-1 flex-grow">
							<label className="text-sm">Max Drawdown (%)</label>
							<Input
								type="number"
								value={maxDrawdown?.toString() ?? ""}
								placeholder="Drawdown"
								onChange={handleMaxDrawdownChange}
							/>
						</div>
					</div>
					<div className="flex gap-3 flex-grow">
						<div className="flex flex-col gap-1 flex-grow">
							<label className="text-sm">Max Open Position</label>
							<Input
								type="number"
								value={maxOpenPosition?.toString() ?? ""}
								placeholder="Open position"
								onChange={handleMaxOpenPositionChange}
							/>
						</div>
						<div className="flex flex-col gap-1 flex-grow">
							<label className="text-sm">Max Order Size</label>
							<Input
								type="number"
								value={maxOrderSize?.toString() ?? ""}
								placeholder="Order size"
								onChange={handleMaxOrderSizeChange}
							/>
						</div>
					</div>
					<div className="flex gap-3 flex-grow">
						<div className="flex flex-col gap-1 flex-grow">
							<label className="text-sm">
								Max Orders Per Minute
							</label>
							<Input
								type="number"
								value={maxOrdersPerMin?.toString() ?? ""}
								placeholder="Orders per minute"
								onChange={handleMaxOrdersPerMinChange}
							/>
						</div>
						<div className="flex flex-col gap-1 flex-grow">
							<label className="text-sm">
								Max Risk Per Order (%)
							</label>
							<Input
								type="number"
								value={maxRiskPerOrder?.toString() ?? ""}
								placeholder="Risk per order"
								onChange={handleMaxRiskPerOrderChange}
							/>
						</div>
					</div>
					<Separator />
					<Button type="submit">Update Risk Limits</Button>
				</form>
			</CardContent>
		</Card>
	);
};

export default RiskManagerForm;
