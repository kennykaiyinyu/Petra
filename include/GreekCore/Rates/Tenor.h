#ifndef GREEKCORE_RATES_TENOR_H
#define GREEKCORE_RATES_TENOR_H

#include <string>
#include <stdexcept>
#include <cctype>
#include <functional>
#include "../Time/Date.h"
#include "GreekCore/Time/Calendar.h"

namespace GreekCore::Rates {

    enum class TimeUnit { Days, Weeks, Months, Years };

    /**
     * @brief Represents a standard financial tenor (e.g., "1D", "2W", "3M", "10Y").
     */
    struct Tenor {
        int amount;
        TimeUnit unit;

        /**
         * @brief Parses a tenor string like "3M", "1Y".
         */
        static Tenor parse(std::string_view s);

        /**
         * @brief Adds the tenor to a Date.
         * Simple implementation: Does NOT handle business day rolling (must be done by Calendar).
         */
        GreekCore::Time::Date add_to(const GreekCore::Time::Date& start) const;

        /**
         * @brief Adds the tenor to a Date and adjusts the result using the business day convention.
         */
        GreekCore::Time::Date add_to(const GreekCore::Time::Date& start, 
                                     GreekCore::Time::BusinessDayConvention convention,
                                     std::function<bool(GreekCore::Time::Date)> calendar) const;
    };
}
#endif
