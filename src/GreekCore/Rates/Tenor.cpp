#include "GreekCore/Rates/Tenor.h"
#include "GreekCore/Time/Date.h"
#include <stdexcept>
#include <string>
#include <cctype>

namespace GreekCore::Rates {

    Tenor Tenor::parse(std::string_view s) {
        if (s.empty()) throw std::invalid_argument("Empty tenor string");
        
        size_t unit_pos = 0;
        while (unit_pos < s.size() && std::isdigit(s[unit_pos])) {
            unit_pos++;
        }

        if (unit_pos == 0 || unit_pos >= s.size()) {
            throw std::invalid_argument("Invalid tenor format");
        }

        int amt = std::stoi(std::string(s.substr(0, unit_pos)));
        char u = std::toupper(s[unit_pos]);

        TimeUnit tu;
        switch(u) {
            case 'D': tu = TimeUnit::Days; break;
            case 'W': tu = TimeUnit::Weeks; break;
            case 'M': tu = TimeUnit::Months; break;
            case 'Y': tu = TimeUnit::Years; break;
            default: throw std::invalid_argument("Unknown tenor unit");
        }

        return {amt, tu};
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
