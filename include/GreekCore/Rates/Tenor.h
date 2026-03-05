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
     * @brief Represents a standard financial tenor (period of time).
     * 
     * Used for defining instrument maturities (e.g., "3M" LIBOR, "10Y" Swap).
     * Supports Days, Weeks, Months, Years.
     */
    struct Tenor {
        int amount;     ///< Number of units (e.g., 3).
        TimeUnit unit;  ///< Time unit (e.g., Months).

        /**
         * @brief Parses a tenor string like "3M", "1Y".
         * 
         * Supported units: D, W, M, Y.
         * Case-insensitive.
         */
        static Tenor parse(std::string_view s);

        /**
         * @brief Adds the tenor to a Date.
         * 
         * Simple implementation: Does NOT handle business day rolling.
         * Result may fall on a holiday or weekend.
         * 
         * @param start The start date.
         * @return The raw calendar date after the duration.
         */
        GreekCore::Time::Date add_to(const GreekCore::Time::Date& start) const;

        /**
         * @brief Adds the tenor to a Date and adjusts using a business day convention.
         * 
         * @param start The start date.
         * @param convention Convention to handle holidays (e.g., Following, ModifiedFollowing).
         * @param calendar Function or object returning true if a date is a business day (false if holiday).
         * @return The adjusted business date.
         */
        GreekCore::Time::Date add_to(const GreekCore::Time::Date& start, 
                                     GreekCore::Time::BusinessDayConvention convention,
                                     std::function<bool(GreekCore::Time::Date)> calendar) const;
    };
}
#endif
