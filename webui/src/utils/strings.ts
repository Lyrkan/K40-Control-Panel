export const formatNumber = (
    value: number | null,
    {
        unit = "",
        fallback = "",
        fractionDigits = 0,
    }: {
        unit: string;
        fallback: string;
        fractionDigits: number;
    }
) => {
    if (value === null) {
        return fallback ?? "";
    }

    return `${value.toFixed(fractionDigits)}${unit}`;
};
