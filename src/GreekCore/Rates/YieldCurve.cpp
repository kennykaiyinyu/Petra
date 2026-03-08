#include "GreekCore/Rates/YieldCurve.h"
#include "GreekCore/Numerics/BrentSolver.h"
#include <stdexcept>
#include <cmath>

namespace GreekCore {

    /// @brief Construct a new Yield Curve object and bootstrap it immediately.
    /// @tparam DC The Day Count Strategy type (e.g., Actual365Fixed).
    /// @tparam Interp The Interpolation Strategy type (e.g., LinearInterpolator).
    /// @param reference_date The anchor date (t=0) for the curve.
    /// @param instruments A sorted span of market instruments (Deposits, FRAs, Swaps) used for bootstrapping.
    /// @param dc Instance of the day count strategy.
    /// @param interp Instance of the interpolation strategy.
    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    YieldCurve<DC, Interp>::YieldCurve(Date reference_date, std::span<const CurveInput> instruments, DC dc, Interp interp)
        : ref_date_(reference_date), day_count_convention_(std::move(dc)), interpolator_(std::move(interp)) {
        
        times_.reserve(instruments.size() + 1);
        log_dfs_.reserve(instruments.size() + 1);
        times_.push_back(0.0);
        log_dfs_.push_back(0.0);

        for (const auto& instr : instruments) {
            double t_i = day_count_convention_(ref_date_, instr.maturity_date);

            if (t_i <= times_.back()) { [[unlikely]]
                throw std::invalid_argument("Instruments must be sorted by maturity");
            }

            bootstrapPoint(instr, t_i);
        }
    }

    /// @brief Calculates the discount factor for a specific date.
    /// @param d The target date.
    /// @return The discount factor P(0, d).
    /// @throws std::invalid_argument if d < reference_date.
    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    double YieldCurve<DC, Interp>::getDiscountFactor(Date d) const {
        if (d < ref_date_) throw std::invalid_argument("Date cannot be before reference date");
        return getDiscountFactor(day_count_convention_(ref_date_, d));
    }

    /// @brief Calculates the discount factor for a specific time fraction.
    /// @param t Time in years from reference date.
    /// @return The discount factor e^(-r*t).
    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    double YieldCurve<DC, Interp>::getDiscountFactor(double t) const {
        double log_df = interpolator_.interpolate(t, times_, log_dfs_);
        return std::exp(log_df);
    }

    /// @brief Calculates the continuously compounded zero rate for a specific date.
    /// @param d The target date.
    /// @return The annualized zero rate.
    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    double YieldCurve<DC, Interp>::getZeroRate(Date d) const {
        return getZeroRate(day_count_convention_(ref_date_, d));
    }

    /// @brief Calculates the continuously compounded zero rate for a specific time fraction.
    /// @param t Time in years from reference date.
    /// @return The annualized zero rate.
    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    double YieldCurve<DC, Interp>::getZeroRate(double t) const {
        if (t < 1e-8) {  [[unlikely]]
            return -log_dfs_[0]; 
        }
        return -std::log(getDiscountFactor(t)) / t;
    }

    /// @brief Internal helper to bootstrap a single instrument onto the curve.
    /// Solves for the zero rate/discount factor that prices the instrument at par.
    /// @param instr The market instrument (Deposit, FRA, Swap).
    /// @param T The maturity time of the instrument in years.
    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    void YieldCurve<DC, Interp>::bootstrapPoint(const CurveInput& instr, double T) {
        double R = instr.rate;
        double T_start = day_count_convention_(ref_date_, instr.start_date);
        
        double accrual = 0.0;
        if (instr.type != InstrumentType::Swap) {
            accrual = day_count_convention_(instr.start_date, instr.maturity_date);
        }

        auto pricer = [&](double trial_log_df) -> double {
            double prev_time = times_.back();
            double prev_log_df = log_dfs_.back();

            auto calc_df = [&](double t) {
                if (t <= prev_time) {
                    return std::exp(interpolator_.interpolate(t, times_, log_dfs_));
                }
                double slope = (trial_log_df - prev_log_df) / (T - prev_time);
                double val = prev_log_df + slope * (t - prev_time);
                return std::exp(val);
            };

            double df_start = calc_df(T_start);
            double df_end = std::exp(trial_log_df); 

            if (instr.type == InstrumentType::Swap) {
                using namespace std::chrono;
                int freq = instr.frequency;
                months period_duration{12 / freq};
                
                double pv_legs = 0.0;
                Date payment_date = instr.maturity_date;
                Date start_of_period;

                int num_periods = static_cast<int>(std::round((T - T_start) * freq));
                
                for (int i = 0; i < num_periods; ++i) {
                    year_month_day ymd{payment_date};
                    start_of_period = Date{ymd - period_duration};
                    double t_payment = day_count_convention_(ref_date_, payment_date);
                    double leg_accrual = day_count_convention_(start_of_period, payment_date);
                    pv_legs += calc_df(t_payment) * leg_accrual;
                    payment_date = start_of_period;
                }

                return R * pv_legs - (df_start - df_end);
            } else {
                return df_end * (1.0 + R * accrual) - df_start;
            }
        };
        
        double min_log = -T * 2.0; 
        double max_log = T * 0.1;

        double root = BrentSolver::solve(pricer, min_log, max_log);

        if (std::isnan(root)) {
            throw std::runtime_error("Bootstrap failed to converge");
        }

        times_.push_back(T);
        log_dfs_.push_back(root);
    }
    
    // Explicit instantiations
    template class YieldCurve<Act365DayCounter, LinearInterpolator>;
    template class YieldCurve<Act360DayCounter, LinearInterpolator>;
    template class YieldCurve<ActActDayCounter, LinearInterpolator>;
    template class YieldCurve<Thirty360DayCounter, LinearInterpolator>;
}
