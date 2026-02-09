#include "GreekCore/Pricing/PayOff.h"

namespace GreekCore {

    double PayOffVanilla::implementation(double spot) const {
        switch (m_type) {
            case OptionType::Call: return std::max(spot - m_strike, 0.0);
            case OptionType::Put:  return std::max(m_strike - spot, 0.0);
            default: return 0.0;
        }
    }

    double PayOffDigital::implementation(double spot) const {
        switch (m_type) {
            case OptionType::Call: return (spot > m_strike) ? 1.0 : 0.0;
            case OptionType::Put:  return (spot < m_strike) ? 1.0 : 0.0;
            default: return 0.0;
        }
    }

    double PayOffDoubleDigital::implementation(double spot) const {
        return (spot > m_lower && spot < m_upper) ? 1.0 : 0.0;
    }

    double PayOffAsian::implementation(const std::vector<double>& spot_path) const {
        if (spot_path.empty()) return 0.0;
        double sum = std::accumulate(spot_path.begin(), spot_path.end(), 0.0);
        double avg = sum / spot_path.size();
        
        switch (m_type) {
            case OptionType::Call: return std::max(avg - m_strike, 0.0);
            case OptionType::Put:  return std::max(m_strike - avg, 0.0);
            default: return 0.0;
        }
    }
}
