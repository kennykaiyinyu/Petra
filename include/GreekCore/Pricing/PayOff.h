#ifndef GREEKCORE_PAYOFF_H
#define GREEKCORE_PAYOFF_H

#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>

namespace GreekCore {

    enum class OptionType { Call, Put };

    // CRTP Base Class - Enforces interface at compile time without vtables
    // Contract: The specific option logic and parameters (Strike, Barrier, etc.)
    // MarketParameters: The market data required to evaluate the payoff (Spot, Path, etc.)
    template<typename Derived, typename MarketParameters>
    class PayOff {
    public:
        [[nodiscard]] double operator()(const MarketParameters& market_data) const {
            return static_cast<const Derived*>(this)->implementation(market_data);
        }
        PayOff() = default;
    };

    // Vanilla Option (Call/Put)
    // Depends on a single spot price (double)
    class PayOffVanilla : public PayOff<PayOffVanilla, double> {
        OptionType m_type;
        double m_strike;
    public:
        PayOffVanilla(OptionType type, double strike) : m_type(type), m_strike(strike) {}
        [[nodiscard]] double implementation(double spot) const;
    };

    // Digital Option
    class PayOffDigital : public PayOff<PayOffDigital, double> {
        OptionType m_type;
        double m_strike;
    public:
        PayOffDigital(OptionType type, double strike) : m_type(type), m_strike(strike) {}
        [[nodiscard]] double implementation(double spot) const;
    };

    // Double Digital Option
    class PayOffDoubleDigital : public PayOff<PayOffDoubleDigital, double> {
        double m_lower;
        double m_upper;
    public:
        PayOffDoubleDigital(double lower, double upper) : m_lower(lower), m_upper(upper) {}
        [[nodiscard]] double implementation(double spot) const;
    };

    // Path Dependent: Asian Arithmetic Mean Option
    class PayOffAsian : public PayOff<PayOffAsian, std::vector<double>> {
        OptionType m_type;
        double m_strike;
    public:
        PayOffAsian(OptionType type, double strike) : m_type(type), m_strike(strike) {}
        [[nodiscard]] double implementation(const std::vector<double>& spot_path) const;
    };

}
#endif // GREEKCORE_PAYOFF_H
