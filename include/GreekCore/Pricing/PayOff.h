#ifndef GREEKCORE_PAYOFF_H
#define GREEKCORE_PAYOFF_H

#include <algorithm>
#include <cmath>
#include <vector>
#include <numeric>

namespace GreekCore {

    enum class OptionType { Call, Put };

    /**
     * @brief CRTP Base Class for Payoff definitions.
     * 
     * Uses the Curiously Recurring Template Pattern (CRTP) to enforce the interface
     * at compile-time, avoiding virtual function overhead for high performance.
     * 
     * @tparam Derived The concrete payoff implementation.
     * @tparam MarketParameters The type of market data required (e.g., `double` for European, `std::vector<double>` for Asian).
     */
    template<typename Derived, typename MarketParameters>
    class PayOff {
    public:
        /**
         * @brief Evaluates the payoff for a given market scenario.
         * Delegates to the derived class's implementation.
         */
        [[nodiscard]] double operator()(const MarketParameters& market_data) const {
            return static_cast<const Derived*>(this)->implementation(market_data);
        }
        PayOff() = default;
    };

    /**
     * @brief A standard European option payoff (Call/Put).
     * 
     * Payoff depends only on the terminal spot price $S_T$.
     * $ P(S_T) = \max(S_T - K, 0) $ for Calls, or $\max(K - S_T, 0)$ for Puts.
     */
    class PayOffVanilla : public PayOff<PayOffVanilla, double> {
        OptionType m_type;
        double m_strike;
    public:
        /**
         * @brief Constructs a Vanilla Option Payoff.
         * @param type Call or Put.
         * @param strike Strike price K.
         */
        PayOffVanilla(OptionType type, double strike) : m_type(type), m_strike(strike) {}

        /**
         * @brief Evaluates the payoff at maturity.
         * @param spot The final spot price $S_T$.
         * @return The intrinsic value of the option.
         */
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
