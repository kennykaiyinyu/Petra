#include <gtest/gtest.h>
#include "GreekCore/Time/Date.h"
#include "GreekCore/Time/DayCounter.h"
#include "GreekCore/Time/Calendar.h"

using namespace GreekCore::Time;

TEST(TimeTest, DateCreation) {
    auto d = make_date(2023, 1, 1);
    EXPECT_EQ(day_of_month(d), 1);
    EXPECT_EQ(month_of_year(d), 1);
    EXPECT_EQ(year_number(d), 2023);
}

TEST(TimeTest, DayCounters) {
    auto d1 = make_date(2023, 1, 1);
    auto d2 = make_date(2023, 2, 1); // 31 days later

    EXPECT_NEAR(Actual365Fixed::year_fraction(d1, d2), 31.0/365.0, 1e-9);
    EXPECT_NEAR(Actual360::year_fraction(d1, d2), 31.0/360.0, 1e-9);
    EXPECT_NEAR(Thirty360::year_fraction(d1, d2), 30.0/360.0, 1e-9); // 30/360 convention
}

TEST(TimeTest, ActualActual) {
    // 2023 is not leap, 2024 is leap
    auto d1 = make_date(2023, 1, 1);
    auto d2 = make_date(2023, 2, 1);
    // Within non-leap year
    EXPECT_NEAR(ActualActual::year_fraction(d1, d2), 31.0/365.0, 1e-9);

    auto d3 = make_date(2024, 1, 1);
    auto d4 = make_date(2024, 2, 1);
    // Within leap year
    EXPECT_NEAR(ActualActual::year_fraction(d3, d4), 31.0/366.0, 1e-9);

    // Spanning years
    auto start = make_date(2023, 12, 31);
    auto end = make_date(2024, 1, 2);
    // 1 day in 2023 (Dec 31), 1 day in 2024 (Jan 1)
    double expected = 1.0/365.0 + 1.0/366.0;
    EXPECT_NEAR(ActualActual::year_fraction(start, end), expected, 1e-9);
}

TEST(TimeTest, CalendarLogic) {
    CustomCalendar cal;
    // Jan 1 2023 was a Sunday
    auto jan1 = make_date(2023, 1, 1);
    EXPECT_FALSE(cal.is_business_day(jan1));

    // Jan 2 2023 was a Monday
    auto jan2 = make_date(2023, 1, 2);
    EXPECT_TRUE(cal.is_business_day(jan2));

    // Add Jan 2 as holiday
    cal.add_holiday(jan2);
    EXPECT_FALSE(cal.is_business_day(jan2));
}

TEST(TimeTest, DateAdjustment) {
    NullCalendar cal;
    auto friday = make_date(2023, 1, 6);
    auto saturday = make_date(2023, 1, 7);
    auto sunday = make_date(2023, 1, 8);
    auto monday = make_date(2023, 1, 9);

    // Following: Sat -> Mon
    EXPECT_EQ(adjust(saturday, BusinessDayConvention::Following, cal), monday);
    
    // Preceding: Sat -> Fri
    EXPECT_EQ(adjust(saturday, BusinessDayConvention::Preceding, cal), friday);
}
