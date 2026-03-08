#include "GreekCore/Rates/Tenor.h"
#include "GreekCore/Time/Date.h"
#include "GreekCore/Time/Calendar.h"
#include <stdexcept>
#include <string>
#include <cctype>
#include <charconv>

namespace GreekCore::Rates {

    Tenor Tenor::parse(std::string_view s) {
        auto t = parse_noexcept(s);
        if (!t) [[unlikely]] {
            throw std::invalid_argument("Invalid/Empty tenor string");
        }
        return *t;
    }

    std::optional<Tenor> Tenor::parse_noexcept(std::string_view s) noexcept {
        if (s.empty()) return std::nullopt;
        
        if (s == "ON" || s == "O/N" || s == "TN" || s == "T/N" || s == "SN" || s == "S/N") {
            return Tenor{1, TimeUnit::Days};
        }

        size_t unit_pos = 0;
        while (unit_pos < s.size() && std::isdigit(s[unit_pos])) {
            unit_pos++;
        }

        if (unit_pos == 0 || unit_pos >= s.size()) [[unlikely]] {
            return std::nullopt;
        }

        int amt = 0;
        auto [ptr, ec] = std::from_chars(s.data(), s.data() + unit_pos, amt);
        if (ec != std::errc()) [[unlikely]] {
            return std::nullopt;
        }

        char u = std::toupper(s[unit_pos]);

        TimeUnit tu;
        switch(u) {
            case 'D': tu = TimeUnit::Days; break;
            case 'W': tu = TimeUnit::Weeks; break;
            case 'M': tu = TimeUnit::Months; break;
            case 'Y': tu = TimeUnit::Years; break;
            default: return std::nullopt;
        }

        return Tenor{amt, tu};
    }

    GreekCore::Time::Date Tenor::add_to(const GreekCore::Time::Date& start) const {
        using namespace std::chrono;
        using namespace GreekCore::Time;

        if (unit == TimeUnit::Days) return start + days(amount);
        if (unit == TimeUnit::Weeks) return start + days(amount * 7);

        // Months/Years arithmetic using C++20 year_month_day
        year_month_day ymd{start};
        
        if (unit == TimeUnit::Years) {
            return Date{ymd + years(amount)};
        }
        if (unit == TimeUnit::Months) {
            return Date{ymd + months(amount)};
        }
        return start;
    }
}
