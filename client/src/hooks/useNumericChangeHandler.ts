import { useCallback } from "react";

const useNumericChangeHandler = (
	setter: React.Dispatch<React.SetStateAction<number | null>>
) => {
	return useCallback(
		(e: React.ChangeEvent<HTMLInputElement>) => {
			const { value } = e.target;
			setter(value === "" ? null : Number(value));
		},
		[setter]
	);
};

export default useNumericChangeHandler;
