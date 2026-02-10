#ifndef GREEKCORE_VANILLA_OPTION_H
#define GREEKCORE_VANILLA_OPTION_H

namespace GreekCore {

    // A wrapper to bundle PayOff and Expiry
    // Uses templates to compatible with the high-performance CRTP PayOff classes
    template<typename PayOffType>
    class VanillaOption {
        PayOffType m_payOff;
        double m_expiry;

    public:
        VanillaOption(const PayOffType& payOff, double expiry)
            : m_payOff(payOff), m_expiry(expiry) {}

        double getExpiry() const { return m_expiry; }
        
        // Forward the call operator
        template<typename MarketData>
        double operator()(const MarketData& spot) const {
            return m_payOff(spot);
        }

        const PayOffType& getPayOff() const { return m_payOff; }
    };

}

#endif
