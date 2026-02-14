#ifndef GREEKCORE_YIELDCURVE_H
#define GREEKCORE_YIELDCURVE_H

#include <vector>
#include <span>
#include <cmath>
#include <concepts>
#include "GreekCore/Time/Date.h"
#include "GreekCore/Rates/InterpolatorStrategy.h"
#include "GreekCore/Time/DayCountStrategy.h"
#include "GreekCore/Numerics/BrentSolver.h"

namespace GreekCore {

    using Time::Date;

    enum class InstrumentType {
        Deposit,
        FRA,
        Swap
    };

    /**
     * @brief Curve Input Definition.
     * Defined by a concrete date, rate, and frequency.
     */
    struct CurveInput {
        InstrumentType type;
        double rate;          // Annualized Par Rate (e.g., 0.05)
        Date maturity_date; 
        Date start_date;      // Required. For Spot instruments, set to reference date.
        int frequency;        // 0 = Simple/Zero, 1,2,4 = Swap payments per year
    };

    /**
     * @brief High-Performance Yield Curve using SoA layout.
     * Templated on strategies for maximum compiler optimization (inlining).
     */
    template<DayCountStrategy DC = DefaultDayCounter, InterpolatorStrategy Interp = LinearInterpolator>
    class YieldCurve {
    public:
        /**
         * @brief Construct a new Yield Curve object (RAII).
         */
        YieldCurve(Date reference_date, std::span<const CurveInput> instruments, 
                   DC dc = DC(), Interp interp = Interp());

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
        
        DC day_count_convention_;
        Interp interpolator_;

        void bootstrapPoint(const CurveInput& instr, double T);
    };

    // Deduction Guide: Allows YieldCurve(date, instruments) to deduce default template args
    YieldCurve(Date, std::span<const CurveInput>) -> YieldCurve<DefaultDayCounter, LinearInterpolator>;
}

#endif // GREEKCORE_YIELDCURVE_H
