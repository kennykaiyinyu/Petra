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
     * @brief Represents a market instrument used for curve construction.
     * 
     * Defined by its type, rate, maturity, and payment frequency.
     */
    struct CurveInput {
        InstrumentType type;  ///< Instrument classification (Deposit, FRA, Swap).
        double rate;          ///< Annualized Par Rate (e.g., 0.05 for 5%).
        Date maturity_date;   ///< The date when the final payment occurs.
        Date start_date;      ///< The validation/start date (e.g., Spot Date for deposits).
        int frequency;        ///< Payment frequency per year (0 = Simple Interest/Zero, 1 = Annual, 2 = Semiannual).
    };

    /**
     * @brief High-Performance Yield Curve implementation using Structure of Arrays (SoA).
     * 
     * Uses template strategies for Day Count and Interpolation to maximize compile-time inlining.
     * Implements bootstrapping from market instruments (Deposits, FRAs, Swaps).
     * 
     * @tparam DC Day Count Strategy satisfying `DayCountStrategy` concept.
     * @tparam Interp Interpolation Strategy satisfying `InterpolatorStrategy` concept.
     */
    template<DayCountStrategy DC = Act365DayCounter, InterpolatorStrategy Interp = LinearInterpolator>
    class YieldCurve {
    public:
        /**
         * @brief Constructs a Yield Curve by bootstrapping from market instruments.
         * 
         * @param reference_date The curve's base date ($t=0$).
         * @param instruments A span of calibration instruments (must be sorted by maturity).
         * @param dc Day count convention instance.
         * @param interp Interpolation strategy instance.
         */
        YieldCurve(Date reference_date, std::span<const CurveInput> instruments, 
                   DC dc = DC(), Interp interp = Interp());

        /**
         * @brief Calculates the discount factor $P(0, T)$ for a specific date.
         */
        [[nodiscard]] double getDiscountFactor(Date d) const;
        
        /**
         * @brief Calculates the discount factor $P(0, T)$ for a specific time $T$.
         */
        [[nodiscard]] double getDiscountFactor(double t) const;

        /**
         * @brief Calculates zero rate $R(0, T)$ for a specific date.
         * $P(0, T) = e^{-R(0, T) \cdot T}$
         */
        [[nodiscard]] double getZeroRate(Date d) const;
        
        /**
         * @brief Calculates zero rate $R(0, T)$ for a specific time $T$.
         */
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
    YieldCurve(Date, std::span<const CurveInput>) -> YieldCurve<Act365DayCounter, LinearInterpolator>;
}

#endif // GREEKCORE_YIELDCURVE_H
