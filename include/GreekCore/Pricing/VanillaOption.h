#ifndef GREEKCORE_VANILLA_OPTION_H
#define GREEKCORE_VANILLA_OPTION_H

namespace GreekCore {

    /**
     * @brief A simple wrapper bundling a PayOff strategy with an Expiry time.
     * 
     * Useful for passing a fully defined financial instrument (Payoff + T) to a pricing engine
     * that expects $T$ as an argument, or for building portfolios.
     * 
     * @tparam PayOffType The concrete payoff strategy (e.g., PayOffVanilla).
     */
    template<typename PayOffType>
    class VanillaOption {
    public:
        /**
         * @brief Constructs an option with a specific payoff and expiry.
         * @param payOff The payoff strategy.
         * @param expiry Time to maturity in years.
         */
        VanillaOption(const PayOffType& payOff, double expiry)
            : m_payOff(payOff), m_expiry(expiry) {}

        /**
         * @return The expiry time $T$.
         */
        double getExpiry() const { return m_expiry; }
        
        /**
         * @brief Evaluates the payoff.
         * Delegates to the underlying payoff object.
         */
        template<typename MarketData>
        double operator()(const MarketData& spot) const {
            return m_payOff(spot);
        }

        /**
         * @return A reference to the underlying payoff object.
         */
        const PayOffType& getPayOff() const { return m_payOff; }

    private:
        PayOffType m_payOff;
        double m_expiry;
    };

}

#endif
