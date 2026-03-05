#ifndef GREEKCORE_TIME_NYSECALENDAR_H
#define GREEKCORE_TIME_NYSECALENDAR_H

#include "GreekCore/Time/Date.h"

namespace GreekCore::Time {

    // NYSE Calendar
    struct NYSECalendar {
        // Callable wrapper for generic usage
        [[nodiscard]] bool operator()(Date d) const;

        [[nodiscard]]
        bool is_business_day(Date d) const;

        [[nodiscard]]
        const char* name() const;
    };

} // namespace GreekCore::Time

#endif // GREEKCORE_TIME_NYSECALENDAR_H
