#include "GreekCore/Rates/YieldCurve.h"
#include "GreekCore/Numerics/BrentSolver.h"
#include "GreekCore/Rates/Interpolator.h"
#include "GreekCore/Time/DayCounter.h"
#include <cmath>
#include <stdexcept>

namespace GreekCore {

    // Default implementations used if none provided
    static double defaultDayCounter(Date d1, Date d2) {
        return Time::Actual365Fixed::year_fraction(d1, d2);
    }

    static double defaultInterpolator(double x, std::span<const double> x_vals, std::span<const double> y_vals) {
        return LinearInterpolator::interpolate(x, x_vals, y_vals);
    }

    YieldCurve::YieldCurve(Date reference_date,
                           DayCounterFn dc,
                           InterpolatorFn interp)
        : ref_date_(reference_date), 
          dc_(dc ? std::move(dc) : defaultDayCounter),
          interp_(interp ? std::move(interp) : defaultInterpolator)
    {
        // Initialize with today: t=0, DF=1.0 => log_df=0.0
        times_.reserve(20);
        log_dfs_.reserve(20);
        times_.push_back(0.0);
        log_dfs_.push_back(0.0);
    }

    double YieldCurve::getDiscountFactor(Date d) const {
        if (d < ref_date_) throw std::invalid_argument("Date cannot be before reference date");
        double t = dc_(ref_date_, d);
        return getDiscountFactor(t);
    }

    double YieldCurve::getDiscountFactor(double t) const {
        // Log-Linear Interpolation: Interpolate on Log DFs, then Exp
        double log_df = interp_(t, times_, log_dfs_);
        return std::exp(log_df);
    }

    double YieldCurve::getZeroRate(Date d) const {
        double t = dc_(ref_date_, d);
        return getZeroRate(t);
    }

    double YieldCurve::getZeroRate(double t) const {
        if (t < 1e-8) return -log_dfs_[0];
        double df = getDiscountFactor(t);
        return -std::log(df) / t;
    }

    void YieldCurve::build(std::span<const CurveInput> instruments) {
        for (const auto& instr : instruments) {
            double T = dc_(ref_date_, instr.maturity_date);

            if (T <= times_.back()) {
                throw std::invalid_argument("Instruments must be sorted by maturity");
            }

            bootstrapPoint(instr, T);
        }
    }

    void YieldCurve::bootstrapPoint(const CurveInput& instr, double T) {
        double R = instr.rate;
        int freq = instr.frequency;

        auto pricer = [&](double trial_log_df) -> double {
            double prev_time = times_.back();
            double prev_log_df = log_dfs_.back();

            // Helper to interpolate during solver trial
            auto calc_df = [&](double t) {
                if (t <= prev_time) {
                    return std::exp(interp_(t, times_, log_dfs_));
                }
                // Linear interpolation on log_df for the current segment (prev -> current trial)
                double slope = (trial_log_df - prev_log_df) / (T - prev_time);
                double val = prev_log_df + slope * (t - prev_time);
                return std::exp(val);
            };

            if (freq == 0) { 
                // Zero-Coupon / Deposit: (1 + R*T)*DF = 1
                return calc_df(T) * (1.0 + R * T) - 1.0;
            } else {
                // Swap
                double pv = 0.0;
                double dt = 1.0 / freq;
                // Sum cashflows
                for (double t = dt; t <= T + 1e-4; t += dt) {
                    // Check if we are at maturity (within tolerance) for principal repayment
                    // Actually usually standard swaps don't exchange principal, but par swap rate implies PV(Legs) = Par
                    // Here we assume PV(Coupons + Principal) = 1.0 logic often simplified for bootstrapping
                    // Standard Par Swap: 1 = R * sum(DF_i * dt) + DF_T
                    // R is the par swap rate.
                    
                    bool at_maturity = (std::abs(t - T) < 1e-4);
                    double cashflow = (R / freq) + (at_maturity ? 1.0 : 0.0);
                    pv += cashflow * calc_df(t);
                }
                return pv - 1.0;
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

}
