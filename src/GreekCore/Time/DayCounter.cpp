#include "GreekCore/Time/DayCounter.h"

namespace GreekCore::Time {

    double Actual365Fixed::year_fraction(Date start, Date end) {
        return days_between(start, end) / 365.0;
    }

    double ActualActual::year_fraction(Date start, Date end) {
        if (start == end) return 0.0;
        if (start > end) return -1.0 * year_fraction(end, start);

        int y1 = year_number(start);
        int y2 = year_number(end);

        auto is_leap = [](int y) { return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0); };
        auto days_in_year = [&](int y) { return is_leap(y) ? 366.0 : 365.0; };

        if (y1 == y2) {
            return days_between(start, end) / days_in_year(y1);
        } 
        
        // Split into parts
        // 1. Days remaining in first year
        Date next_year_start = make_date(y1 + 1, 1, 1);
        double part1 = days_between(start, next_year_start) / days_in_year(y1);

        // 2. Days elapsed in last year
        Date last_year_start = make_date(y2, 1, 1);
        double part2 = days_between(last_year_start, end) / days_in_year(y2);

        // 3. Full years in between
        return part1 + (double)(y2 - y1 - 1) + part2;
    }

    double Actual360::year_fraction(Date start, Date end) {
        return days_between(start, end) / 360.0;
    }

    double Thirty360::year_fraction(Date start, Date end) {
        int d1 = day_of_month(start);
        int m1 = month_of_year(start);
        int y1 = year_number(start);

        int d2 = day_of_month(end);
        int m2 = month_of_year(end);
        int y2 = year_number(end);

        if (d1 == 31) d1 = 30;
        if (d2 == 31 && d1 == 30) d2 = 30;

        double days = 360.0 * (y2 - y1) + 30.0 * (m2 - m1) + (d2 - d1);
        return days / 360.0;
    }

}
