#include "GreekCore/Time/Calendar.h"

namespace GreekCore::Time {

    bool is_weekend(Date d) {
        using namespace std::chrono;
        weekday wd{d};
        return (wd == Saturday || wd == Sunday);
    }

    // NullCalendar
    bool NullCalendar::operator()(Date d) const { return is_business_day(d); }
    bool NullCalendar::is_business_day(Date d) const { return !is_weekend(d); }
    const char* NullCalendar::name() const { return "NullCalendar"; }
    
    // CustomCalendar
    bool CustomCalendar::operator()(Date d) const { return is_business_day(d); }
    
    void CustomCalendar::add_holiday(Date d) {
        holidays_.insert(d);
    }

    bool CustomCalendar::is_business_day(Date d) const {
        if (is_weekend(d)) return false;
        return !holidays_.contains(d);
    }
    
    const char* CustomCalendar::name() const { return "CustomCalendar"; }

    // Adjust Implementation
    Date adjust(Date d, BusinessDayConvention convention, std::function<bool(Date)> calendar) {
        if (convention == BusinessDayConvention::Unadjusted) return d;
        if (calendar(d)) return d;

        if (convention == BusinessDayConvention::Following) {
            Date curr = d;
            while (!calendar(curr)) {
                curr += std::chrono::days(1);
            }
            return curr;
        }

        if (convention == BusinessDayConvention::ModifiedFollowing) {
            Date curr = d;
            while (!calendar(curr)) {
                curr += std::chrono::days(1);
            }
            if (month_of_year(curr) != month_of_year(d)) {
                // Recursively call for Preceding, but with same calendar function
                return adjust(d, BusinessDayConvention::Preceding, calendar);
            }
            return curr;
        }

        if (convention == BusinessDayConvention::Preceding) {
            Date curr = d;
            while (!calendar(curr)) {
                curr -= std::chrono::days(1);
            }
            return curr;
        }

        return d;
    }

}
