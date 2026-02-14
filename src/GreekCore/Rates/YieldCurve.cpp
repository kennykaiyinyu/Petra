#include "GreekCore/Rates/YieldCurve.h"
#include "GreekCore/Numerics/BrentSolver.h"
#include <stdexcept>

namespace GreekCore {

    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    YieldCurve<DC, Interp>::YieldCurve(Date reference_date, std::span<const CurveInput> instruments, 
                                       DC dc, Interp interp)
        : ref_date_(reference_date), day_count_convention_(std::move(dc)), interpolator_(std::move(interp)) {
        
        // Initialize with today: t=0, DF=1.0 => log_df=0.0
        times_.reserve(instruments.size() + 1);
        log_dfs_.reserve(instruments.size() + 1);
        times_.push_back(0.0);
        log_dfs_.push_back(0.0);

        // Run Bootstrap immediately
        for (const auto& instr : instruments) {
            double T = day_count_convention_(ref_date_, instr.maturity_date);

            if (T <= times_.back()) {
                throw std::invalid_argument("Instruments must be sorted by maturity");
            }

            bootstrapPoint(instr, T);
        }
    }

    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    double YieldCurve<DC, Interp>::getDiscountFactor(Date d) const {
        if (d < ref_date_) throw std::invalid_argument("Date cannot be before reference date");
        double t = day_count_convention_(ref_date_, d);
        return getDiscountFactor(t);
    }

    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    double YieldCurve<DC, Interp>::getDiscountFactor(double t) const {
        // Log-Linear Interpolation: Interpolate on Log DFs, then Exp
        // Direct call to strategy (inlined)
        double log_df = interpolator_.interpolate(t, times_, log_dfs_);
        return std::exp(log_df);
    }

    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    double YieldCurve<DC, Interp>::getZeroRate(Date d) const {
        double t = day_count_convention_(ref_date_, d);
        return getZeroRate(t);
    }

    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    double YieldCurve<DC, Interp>::getZeroRate(double t) const {
        if (t < 1e-8) return -log_dfs_[0];
        double df = getDiscountFactor(t);
        return -std::log(df) / t;
    }

    template<DayCountStrategy DC, InterpolatorStrategy Interp>
    void YieldCurve<DC, Interp>::bootstrapPoint(const CurveInput& instr, double T) {
        double R = instr.rate;
        
        // Calculate time to start (relative to ref_date)
        double T_start = day_count_convention_(ref_date_, instr.start_date);
        
        // Pre-calculate accrual for simple instruments
        double accrual = 0.0;
        if (instr.type != InstrumentType::Swap) {
            accrual = day_count_convention_(instr.start_date, instr.maturity_date);
        }

        auto pricer = [&](double trial_log_df) -> double {
            double prev_time = times_.back();
            double prev_log_df = log_dfs_.back();

            // Helper to interpolate during solver trial
            auto calc_df = [&](double t) {
                if (t <= prev_time) {
                    return std::exp(interpolator_.interpolate(t, times_, log_dfs_));
                }
                // Linear interpolation on log_df for the current segment
                double slope = (trial_log_df - prev_log_df) / (T - prev_time);
                double val = prev_log_df + slope * (t - prev_time);
                return std::exp(val);
            };

            double df_start = calc_df(T_start);
            double df_end = std::exp(trial_log_df); 

            if (instr.type == InstrumentType::Swap) {
                int freq = instr.frequency;
                double dt = 1.0 / freq;
                double pv_legs = 0.0;
                
                for (double t = T_start + dt; t <= T + 1e-4; t += dt) {
                    pv_legs += calc_df(t);
                }
                // Par Swap Equation: R * Sum(DF * dt) = DF_start - DF_end
                return R * dt * pv_legs - (df_start - df_end);
            } else {
                // Deposit / FRA
                return df_end * (1.0 + R * accrual) - df_start;
            }
        };
        
        double min_log = -T * 2.0; 
        double max_log = T * 0.1;

        auto result = BrentSolver::solve(pricer, min_log, max_log);

        if (!result.converged) {
            throw std::runtime_error("Bootstrap failed to converge");
        }

        times_.push_back(T);
        log_dfs_.push_back(result.root);
    }

    // Explicit Instantiation for the default types used in tests/applications
    template class YieldCurve<DefaultDayCounter, LinearInterpolator>;
}
