#ifndef GREEKCORE_DAYCOUNTSTRATEGY_H
#define GREEKCORE_DAYCOUNTSTRATEGY_H

#include <concepts>
#include "GreekCore/Time/Date.h"
#include "GreekCore/Time/DayCounter.h"

namespace GreekCore {

    using Time::Date;

    /**
     * @brief Concept to ensure the day count strategy is followed at compile-time.
     */
    template<typename T>
    concept DayCountStrategy = requires(T t, Date d1, Date d2) {
        { t(d1, d2) } -> std::convertible_to<double>;
    };

    /**
     * @brief Default Day Counter (Actual/365 Fixed).
     */
    struct Act365DayCounter {
        [[nodiscard]]
        double operator()(Date d1, Date d2) const {
            return Time::Actual365Fixed::year_fraction(d1, d2);
        }
    };

    /**
     * @brief Actual/360 Day Counter.
     */
    struct Act360DayCounter {
        [[nodiscard]]
        double operator()(Date d1, Date d2) const {
            return Time::Actual360::year_fraction(d1, d2);
        }
    };

    /**
     * @brief Actual/Actual (ISDA) Day Counter.
     */
    struct ActActDayCounter {
        [[nodiscard]]
        double operator()(Date d1, Date d2) const {
            return Time::ActualActual::year_fraction(d1, d2);
        }
    };

    /**
     * @brief 30/360 Day Counter.
     */
    struct Thirty360DayCounter {
        [[nodiscard]]
        double operator()(Date d1, Date d2) const {
            return Time::Thirty360::year_fraction(d1, d2);
        }
    };
}

#endif // GREEKCORE_DAYCOUNTSTRATEGY_H