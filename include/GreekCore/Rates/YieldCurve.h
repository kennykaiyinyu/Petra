#ifndef GREEKCORE_YIELDCURVE_H
#define GREEKCORE_YIELDCURVE_H

#include <vector>
#include <functional>
#include <span>
#include "GreekCore/Time/Date.h"

namespace GreekCore {

    using Time::Date;

    /**
     * @brief Curve Input Definition.
     * Defined by a concrete date, rate, and frequency.
     */
    struct CurveInput {
        Date maturity_date; 
        double rate;          // Annualized Par Rate (e.g., 0.05)
        int frequency;        // 0 = Simple/Zero, 1,2,4 = Swap payments per year
    };

    /**
     * @brief High-Performance Yield Curve using SoA layout.
     * Implementation details hidden in source file.
     */
    class YieldCurve {
    public:
        // Function types for strategies
        using DayCounterFn = std::function<double(Date, Date)>;
        using InterpolatorFn = std::function<double(double, std::span<const double>, std::span<const double>)>;

        /**
         * @brief Construct a new Yield Curve object (RAII).
         * 
         * @param reference_date The anchor date (t=0)
         * @param instruments Sorted list of market instruments to bootstrap from.
         * @param dc Day counting strategy function (defaults to Act/365)
         * @param interp Interpolation strategy function (defaults to Linear)
         * @throws std::invalid_argument if instruments are not sorted or invalid.
         */
        YieldCurve(Date reference_date, std::span<const CurveInput> instruments, DayCounterFn dc = nullptr, InterpolatorFn interp = nullptr);

        /**
         * @brief Calculates discount factor for a specific date.
         */
        [[nodiscard]] double getDiscountFactor(Date d) const;
        [[nodiscard]] double getDiscountFactor(double t) const;

        /**
         * @brief Calculates zero rate for a specific date/time.
         */
        [[nodiscard]] double getZeroRate(Date d) const;
        [[nodiscard]] double getZeroRate(double t) const;

    private:
        Date ref_date_;
        std::vector<double> times_;    // Grid points (x) - Year Fractions
        std::vector<double> log_dfs_;  // Log Discount Factors (y)
        
        DayCounterFn dc_;
        InterpolatorFn interp_;

        void bootstrapPoint(const CurveInput& instr, double T);
    };
}

#endif // GREEKCORE_YIELDCURVE_H
