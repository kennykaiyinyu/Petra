#include <gtest/gtest.h>
#include "GreekCore/Rates/Tenor.h"

using namespace GreekCore::Rates;
using namespace GreekCore::Time;

TEST(TenorTest, ParseString) {
    auto t1 = Tenor::parse("1D");
    EXPECT_EQ(t1.amount, 1);
    EXPECT_EQ(t1.unit, TimeUnit::Days);

    auto t2 = Tenor::parse("6M");
    EXPECT_EQ(t2.amount, 6);
    EXPECT_EQ(t2.unit, TimeUnit::Months);

    auto t3 = Tenor::parse("10Y");
    EXPECT_EQ(t3.amount, 10);
    EXPECT_EQ(t3.unit, TimeUnit::Years);
}

TEST(TenorTest, DateAddition) {
    auto start = make_date(2023, 1, 1); // Jan 1 2023

    // 1 Week
    EXPECT_EQ(Tenor::parse("1W").add_to(start), make_date(2023, 1, 8));

    // 1 Month
    EXPECT_EQ(Tenor::parse("1M").add_to(start), make_date(2023, 2, 1));

    // 1 Year
    EXPECT_EQ(Tenor::parse("1Y").add_to(start), make_date(2024, 1, 1));
}
