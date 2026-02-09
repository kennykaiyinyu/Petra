#include "GreekCore/Time/Date.h"

namespace GreekCore::Time {

    using namespace std::chrono;

    Date make_date(int year, int month, int day) {
        return Date{year_month_day{std::chrono::year(year), std::chrono::month(month), std::chrono::day(day)}};
    }

    int day_of_month(Date d) {
        return static_cast<unsigned>(year_month_day{d}.day());
    }

    int month_of_year(Date d) {
        return static_cast<unsigned>(year_month_day{d}.month());
    }

    int year_number(Date d) {
        return static_cast<int>(year_month_day{d}.year());
    }

    int days_between(Date start, Date end) {
        return (end - start).count();
    }
}
