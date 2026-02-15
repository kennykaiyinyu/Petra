#include "GreekCore/Time/NYSECalendar.h"
#include "GreekCore/Time/Date.h"
#include <vector>
#include <algorithm>
#include <chrono>

namespace GreekCore::Time {

    namespace {
        using namespace std::chrono;

        const std::vector<Date>& get_holidays() {
            static const std::vector<Date> holidays = []{
                std::vector<Date> v = {
                    // 2026
                    make_date(2026, 1, 1),
                    make_date(2026, 1, 19),
                    make_date(2026, 2, 16),
                    make_date(2026, 4, 3),
                    make_date(2026, 5, 25),
                    make_date(2026, 6, 19),
                    make_date(2026, 7, 3),
                    make_date(2026, 9, 7),
                    make_date(2026, 11, 26),
                    make_date(2026, 12, 25),
                    // 2027
                    make_date(2027, 1, 1),
                    make_date(2027, 1, 18),
                    make_date(2027, 2, 15),
                    make_date(2027, 3, 26),
                    make_date(2027, 5, 31),
                    make_date(2027, 6, 18),
                    make_date(2027, 7, 5),
                    make_date(2027, 9, 6),
                    make_date(2027, 11, 25),
                    make_date(2027, 12, 24),
                    // 2028
                    make_date(2027, 12, 31),
                    make_date(2028, 1, 17),
                    make_date(2028, 2, 21),
                    make_date(2028, 4, 14),
                    make_date(2028, 5, 29),
                    make_date(2028, 6, 19),
                    make_date(2028, 7, 4),
                    make_date(2028, 9, 4),
                    make_date(2028, 11, 23),
                    make_date(2028, 12, 25),
                    // 2029
                    make_date(2029, 1, 1),
                    make_date(2029, 1, 15),
                    make_date(2029, 2, 19),
                    make_date(2029, 3, 30),
                    make_date(2029, 5, 28),
                    make_date(2029, 6, 19),
                    make_date(2029, 7, 4),
                    make_date(2029, 9, 3),
                    make_date(2029, 11, 22),
                    make_date(2029, 12, 25),
                    // 2030
                    make_date(2030, 1, 1),
                    make_date(2030, 1, 21),
                    make_date(2030, 2, 18),
                    make_date(2030, 4, 19),
                    make_date(2030, 5, 27),
                    make_date(2030, 6, 19),
                    make_date(2030, 7, 4),
                    make_date(2030, 9, 2),
                    make_date(2030, 11, 28),
                    make_date(2030, 12, 25),
                    // 2031
                    make_date(2031, 1, 1),
                    make_date(2031, 1, 20),
                    make_date(2031, 2, 17),
                    make_date(2031, 4, 11),
                    make_date(2031, 5, 26),
                    make_date(2031, 6, 19),
                    make_date(2031, 7, 4),
                    make_date(2031, 9, 1),
                    make_date(2031, 11, 27),
                    make_date(2031, 12, 25),
                    // 2032
                    make_date(2032, 1, 1),
                    make_date(2032, 1, 19),
                    make_date(2032, 2, 16),
                    make_date(2032, 3, 26),
                    make_date(2032, 5, 31),
                    make_date(2032, 6, 18),
                    make_date(2032, 7, 5),
                    make_date(2032, 9, 6),
                    make_date(2032, 11, 25),
                    make_date(2032, 12, 24),
                    // 2033
                    make_date(2032, 12, 31),
                    make_date(2033, 1, 17),
                    make_date(2033, 2, 21),
                    make_date(2033, 4, 15),
                    make_date(2033, 5, 30),
                    make_date(2033, 6, 20),
                    make_date(2033, 7, 4),
                    make_date(2033, 9, 5),
                    make_date(2033, 11, 24),
                    make_date(2033, 12, 26),
                    // 2034
                    make_date(2034, 1, 2),
                    make_date(2034, 1, 16),
                    make_date(2034, 2, 20),
                    make_date(2034, 4, 7),
                    make_date(2034, 5, 29),
                    make_date(2034, 6, 19),
                    make_date(2034, 7, 4),
                    make_date(2034, 9, 4),
                    make_date(2034, 11, 23),
                    make_date(2034, 12, 25),
                    // 2035
                    make_date(2035, 1, 1),
                    make_date(2035, 1, 15),
                    make_date(2035, 2, 19),
                    make_date(2035, 3, 23),
                    make_date(2035, 5, 28),
                    make_date(2035, 6, 19),
                    make_date(2035, 7, 4),
                    make_date(2035, 9, 3),
                    make_date(2035, 11, 22),
                    make_date(2035, 12, 25),
                    // 2036
                    make_date(2036, 1, 1),
                    make_date(2036, 1, 21),
                    make_date(2036, 2, 18),
                    make_date(2036, 4, 11),
                    make_date(2036, 5, 26),
                    make_date(2036, 6, 19),
                    make_date(2036, 7, 4),
                    make_date(2036, 9, 1),
                    make_date(2036, 11, 27),
                    make_date(2036, 12, 25),
                    // 2037
                    make_date(2037, 1, 1),
                    make_date(2037, 1, 19),
                    make_date(2037, 2, 16),
                    make_date(2037, 4, 3),
                    make_date(2037, 5, 25),
                    make_date(2037, 6, 19),
                    make_date(2037, 7, 3),
                    make_date(2037, 9, 7),
                    make_date(2037, 11, 26),
                    make_date(2037, 12, 25),
                    // 2038
                    make_date(2038, 1, 1),
                    make_date(2038, 1, 18),
                    make_date(2038, 2, 15),
                    make_date(2038, 4, 23),
                    make_date(2038, 5, 31),
                    make_date(2038, 6, 18),
                    make_date(2038, 7, 5),
                    make_date(2038, 9, 6),
                    make_date(2038, 11, 25),
                    make_date(2038, 12, 24),
                    // 2039
                    make_date(2038, 12, 31),
                    make_date(2039, 1, 17),
                    make_date(2039, 2, 21),
                    make_date(2039, 4, 8),
                    make_date(2039, 5, 30),
                    make_date(2039, 6, 20),
                    make_date(2039, 7, 4),
                    make_date(2039, 9, 5),
                    make_date(2039, 11, 24),
                    make_date(2039, 12, 26),
                    // 2040
                    make_date(2040, 1, 2),
                    make_date(2040, 1, 16),
                    make_date(2040, 2, 20),
                    make_date(2040, 3, 30),
                    make_date(2040, 5, 28),
                    make_date(2040, 6, 19),
                    make_date(2040, 7, 4),
                    make_date(2040, 9, 3),
                    make_date(2040, 11, 22),
                    make_date(2040, 12, 25),
                    // 2041
                    make_date(2041, 1, 1),
                    make_date(2041, 1, 21),
                    make_date(2041, 2, 18),
                    make_date(2041, 4, 19),
                    make_date(2041, 5, 27),
                    make_date(2041, 6, 19),
                    make_date(2041, 7, 4),
                    make_date(2041, 9, 2),
                    make_date(2041, 11, 28),
                    make_date(2041, 12, 25),
                    // 2042
                    make_date(2042, 1, 1),
                    make_date(2042, 1, 20),
                    make_date(2042, 2, 17),
                    make_date(2042, 4, 4),
                    make_date(2042, 5, 26),
                    make_date(2042, 6, 19),
                    make_date(2042, 7, 4),
                    make_date(2042, 9, 1),
                    make_date(2042, 11, 27),
                    make_date(2042, 12, 25),
                    // 2043
                    make_date(2043, 1, 1),
                    make_date(2043, 1, 19),
                    make_date(2043, 2, 16),
                    make_date(2043, 3, 27),
                    make_date(2043, 5, 25),
                    make_date(2043, 6, 19),
                    make_date(2043, 7, 3),
                    make_date(2043, 9, 7),
                    make_date(2043, 11, 26),
                    make_date(2043, 12, 25),
                    // 2044
                    make_date(2044, 1, 1),
                    make_date(2044, 1, 18),
                    make_date(2044, 2, 15),
                    make_date(2044, 4, 15),
                    make_date(2044, 5, 30),
                    make_date(2044, 6, 20),
                    make_date(2044, 7, 4),
                    make_date(2044, 9, 5),
                    make_date(2044, 11, 24),
                    make_date(2044, 12, 26),
                    // 2045
                    make_date(2045, 1, 2),
                    make_date(2045, 1, 16),
                    make_date(2045, 2, 20),
                    make_date(2045, 4, 7),
                    make_date(2045, 5, 29),
                    make_date(2045, 6, 19),
                    make_date(2045, 7, 4),
                    make_date(2045, 9, 4),
                    make_date(2045, 11, 23),
                    make_date(2045, 12, 25),
                    // 2046
                    make_date(2046, 1, 1),
                    make_date(2046, 1, 15),
                    make_date(2046, 2, 19),
                    make_date(2046, 3, 23),
                    make_date(2046, 5, 28),
                    make_date(2046, 6, 19),
                    make_date(2046, 7, 4),
                    make_date(2046, 9, 3),
                    make_date(2046, 11, 22),
                    make_date(2046, 12, 25),
                    // 2047
                    make_date(2047, 1, 1),
                    make_date(2047, 1, 21),
                    make_date(2047, 2, 18),
                    make_date(2047, 4, 12),
                    make_date(2047, 5, 27),
                    make_date(2047, 6, 19),
                    make_date(2047, 7, 4),
                    make_date(2047, 9, 2),
                    make_date(2047, 11, 28),
                    make_date(2047, 12, 25),
                    // 2048
                    make_date(2048, 1, 1),
                    make_date(2048, 1, 20),
                    make_date(2048, 2, 17),
                    make_date(2048, 4, 3),
                    make_date(2048, 5, 25),
                    make_date(2048, 6, 19),
                    make_date(2048, 7, 3),
                    make_date(2048, 9, 7),
                    make_date(2048, 11, 26),
                    make_date(2048, 12, 25),
                    // 2049
                    make_date(2049, 1, 1),
                    make_date(2049, 1, 18),
                    make_date(2049, 2, 15),
                    make_date(2049, 4, 16),
                    make_date(2049, 5, 31),
                    make_date(2049, 6, 18),
                    make_date(2049, 7, 5),
                    make_date(2049, 9, 6),
                    make_date(2049, 11, 25),
                    make_date(2049, 12, 24),
                    // 2050
                    make_date(2049, 12, 31),
                    make_date(2050, 1, 17),
                    make_date(2050, 2, 21),
                    make_date(2050, 4, 8),
                    make_date(2050, 5, 30),
                    make_date(2050, 6, 20),
                    make_date(2050, 7, 4),
                    make_date(2050, 9, 5),
                    make_date(2050, 11, 24),
                    make_date(2050, 12, 26),
                    // 2051
                    make_date(2051, 1, 2),
                    make_date(2051, 1, 16),
                    make_date(2051, 2, 20),
                    make_date(2051, 3, 31),
                    make_date(2051, 5, 29),
                    make_date(2051, 6, 19),
                    make_date(2051, 7, 4),
                    make_date(2051, 9, 4),
                    make_date(2051, 11, 23),
                    make_date(2051, 12, 25),
                    // 2052
                    make_date(2052, 1, 1),
                    make_date(2052, 1, 15),
                    make_date(2052, 2, 19),
                    make_date(2052, 4, 19),
                    make_date(2052, 5, 27),
                    make_date(2052, 6, 19),
                    make_date(2052, 7, 4),
                    make_date(2052, 9, 2),
                    make_date(2052, 11, 28),
                    make_date(2052, 12, 25),
                    // 2053
                    make_date(2053, 1, 1),
                    make_date(2053, 1, 20),
                    make_date(2053, 2, 17),
                    make_date(2053, 4, 4),
                    make_date(2053, 5, 26),
                    make_date(2053, 6, 19),
                    make_date(2053, 7, 4),
                    make_date(2053, 9, 1),
                    make_date(2053, 11, 27),
                    make_date(2053, 12, 25),
                    // 2054
                    make_date(2054, 1, 1),
                    make_date(2054, 1, 19),
                    make_date(2054, 2, 16),
                    make_date(2054, 3, 27),
                    make_date(2054, 5, 25),
                    make_date(2054, 6, 19),
                    make_date(2054, 7, 3),
                    make_date(2054, 9, 7),
                    make_date(2054, 11, 26),
                    make_date(2054, 12, 25),
                    // 2055
                    make_date(2055, 1, 1),
                    make_date(2055, 1, 18),
                    make_date(2055, 2, 15),
                    make_date(2055, 4, 16),
                    make_date(2055, 5, 31),
                    make_date(2055, 6, 18),
                    make_date(2055, 7, 5),
                    make_date(2055, 9, 6),
                    make_date(2055, 11, 25),
                    make_date(2055, 12, 24),
                    // 2056
                    make_date(2055, 12, 31),
                    make_date(2056, 1, 17),
                    make_date(2056, 2, 21),
                    make_date(2056, 3, 31),
                    make_date(2056, 5, 29),
                    make_date(2056, 6, 19),
                    make_date(2056, 7, 4),
                    make_date(2056, 9, 4),
                    make_date(2056, 11, 23),
                    make_date(2056, 12, 25),
                };
                // The generator produces sorted dates per year, but observed holidays can fall
                // in the previous year (e.g. Jan 1 is a saturday).
                // A final sort ensures the entire list is chronological.
                std::sort(v.begin(), v.end());
                return v;
            }();
            return holidays;
        }
    }

    bool NYSECalendar::operator()(Date d) const {
        return is_business_day(d);
    }

    bool NYSECalendar::is_business_day(Date d) const {
        using namespace std::chrono;
        
        // 1. Weekend check
        weekday wd{d};
        if (wd == Saturday || wd == Sunday) return false;

        // 2. Holiday Lookup
        const auto& holidays = get_holidays();
        return !std::binary_search(holidays.begin(), holidays.end(), d);
    }

    const char* NYSECalendar::name() const {
        return "NYSE";
    }

} // namespace GreekCore::Time
