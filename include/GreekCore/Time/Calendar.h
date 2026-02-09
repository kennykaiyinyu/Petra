#ifndef GREEKCORE_TIME_CALENDAR_H
#define GREEKCORE_TIME_CALENDAR_H

#include "Date.h"
#include <set>
#include <functional>
#include <concepts>

namespace GreekCore::Time {

    // Concept for Calendar strategies
    template<typename T>
    concept CalendarStrategy = requires(T c, Date d) {
        { c.is_business_day(d) } -> std::convertible_to<bool>;
        { c.name() } -> std::convertible_to<const char*>;
    };

    enum class BusinessDayConvention {
        Following,
        Preceding,
        ModifiedFollowing,
        Unadjusted
    };

    // -----------------------------------------------------------
    // Base Logic for Weekends
    // -----------------------------------------------------------
    bool is_weekend(Date d);

    // -----------------------------------------------------------
    // Implementation: Null Calendar (No Holidays)
    // -----------------------------------------------------------
    struct NullCalendar {
        // Callable wrapper for generic usage
        [[nodiscard]] bool operator()(Date d) const;

        [[nodiscard]]
        bool is_business_day(Date d) const;

        [[nodiscard]]
        const char* name() const;
    };

    // -----------------------------------------------------------
    // Generic Calendar (Weekends + Custom Holidays)
    // -----------------------------------------------------------
    class CustomCalendar {
    public:
        // Callable wrapper for generic usage
        [[nodiscard]] bool operator()(Date d) const;

        void add_holiday(Date d);

        [[nodiscard]]
        bool is_business_day(Date d) const;

        [[nodiscard]]
        const char* name() const;

    private:
        std::set<Date> holidays_;
    };

    // -----------------------------------------------------------
    // Date Adjustment Utilities
    // -----------------------------------------------------------
    
    /**
     * @brief Adjusts a date according to the business day convention.
     * Logic hidden in compiled source.
     * Uses std::function to allow any CalendarStrategy (via implicit conversion or lambda).
     */
    Date adjust(Date d, BusinessDayConvention convention, std::function<bool(Date)> calendar);

}

#endif // GREEKCORE_TIME_CALENDAR_H
