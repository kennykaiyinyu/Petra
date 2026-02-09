#ifndef GREEKCORE_TIME_DAYCOUNTER_H
#define GREEKCORE_TIME_DAYCOUNTER_H

#include "Date.h"
#include <algorithm>
#include <concepts>

namespace GreekCore::Time {

    // Concept for a wrapper that accepts a DayCounter
    template<typename T>
    concept DayCounterStrategy = requires(T t, Date d1, Date d2) {
        { t.year_fraction(d1, d2) } -> std::convertible_to<double>;
        { t.name() } -> std::convertible_to<const char*>;
    };

    // -----------------------------------------------------------
    // Implementation: Actual/365 (Fixed)
    // -----------------------------------------------------------
    struct Actual365Fixed {
        [[nodiscard]]
        static double year_fraction(Date start, Date end);

        [[nodiscard]]
        static const char* name() { return "Actual/365 (Fixed)"; }
    };

    // -----------------------------------------------------------
    // Implementation: Actual/Actual (ISDA)
    // -----------------------------------------------------------
    struct ActualActual {
        [[nodiscard]]
        static double year_fraction(Date start, Date end);

        [[nodiscard]]
        static const char* name() { return "Actual/Actual (ISDA)"; }
    };

    // -----------------------------------------------------------
    // Implementation: Actual/360
    // -----------------------------------------------------------
    struct Actual360 {
        [[nodiscard]]
        static double year_fraction(Date start, Date end);

        [[nodiscard]]
        static const char* name() { return "Actual/360"; }
    };

    // -----------------------------------------------------------
    // Implementation: Thirty/360 (Bond Basis)
    // -----------------------------------------------------------
    struct Thirty360 {
        [[nodiscard]]
        static double year_fraction(Date start, Date end);

        [[nodiscard]]
        static const char* name() { return "30/360"; }
    };
}

#endif // GREEKCORE_TIME_DAYCOUNTER_H
