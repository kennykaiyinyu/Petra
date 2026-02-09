#ifndef GREEKCORE_TIME_DATE_H
#define GREEKCORE_TIME_DATE_H

#include <chrono>
#include <format>
#include <compare>

namespace GreekCore::Time {

    using namespace std::chrono_literals;

    /**
     * @brief Date data type based on C++20 std::chrono::sys_days.
     * Represents a discrete day (count of days since epoch).
     */
    using Date = std::chrono::sys_days;

    Date make_date(int year, int month, int day);
    int day_of_month(Date d);
    int month_of_year(Date d);
    int year_number(Date d);

    /**
     * @brief Computes raw days difference between two dates.
     */
    int days_between(Date start, Date end);
}

#endif // GREEKCORE_TIME_DATE_H
