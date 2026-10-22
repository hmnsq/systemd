/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <limits.h>
#अन्तर्भाव <time.h>

#अन्तर्भाव "forward.h"

#define PRI_NSEC PRIu64
#define PRI_USEC PRIu64
#define NSEC_FMT "%" PRI_NSEC
#define USEC_FMT "%" PRI_USEC

प्रकारनिर्देश संरचना dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

प्रकारनिर्देश संरचना triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

प्रकारनिर्देश गणना TimestampStyle {
        TIMESTAMP_PRETTY,
        TIMESTAMP_US,
        TIMESTAMP_UTC,
        TIMESTAMP_US_UTC,
        TIMESTAMP_UNIX,
        TIMESTAMP_DATE,
        _TIMESTAMP_STYLE_MAX,
        _TIMESTAMP_STYLE_INVALID = -EINVAL,
} TimestampStyle;

#define USEC_INFINITY ((usec_t) UINT64_MAX)
#define NSEC_INFINITY ((nsec_t) UINT64_MAX)

#define MSEC_PER_SEC  1000ULL
#define USEC_PER_SEC  ((usec_t) 1000000ULL)
#define USEC_PER_MSEC ((usec_t) 1000ULL)
#define NSEC_PER_SEC  ((nsec_t) 1000000000ULL)
#define NSEC_PER_MSEC ((nsec_t) 1000000ULL)
#define NSEC_PER_USEC ((nsec_t) 1000ULL)

#define USEC_PER_MINUTE ((usec_t) (60ULL*USEC_PER_SEC))
#define NSEC_PER_MINUTE ((nsec_t) (60ULL*NSEC_PER_SEC))
#define USEC_PER_HOUR ((usec_t) (60ULL*USEC_PER_MINUTE))
#define NSEC_PER_HOUR ((nsec_t) (60ULL*NSEC_PER_MINUTE))
#define USEC_PER_DAY ((usec_t) (24ULL*USEC_PER_HOUR))
#define NSEC_PER_DAY ((nsec_t) (24ULL*NSEC_PER_HOUR))
#define USEC_PER_WEEK ((usec_t) (7ULL*USEC_PER_DAY))
#define NSEC_PER_WEEK ((nsec_t) (7ULL*NSEC_PER_DAY))
#define USEC_PER_MONTH ((usec_t) (2629800ULL*USEC_PER_SEC))
#define NSEC_PER_MONTH ((nsec_t) (2629800ULL*NSEC_PER_SEC))
#define USEC_PER_YEAR ((usec_t) (31557600ULL*USEC_PER_SEC))
#define NSEC_PER_YEAR ((nsec_t) (31557600ULL*NSEC_PER_SEC))

गणना {
        WEEKDAY_MON,
        WEEKDAY_TUE,
        WEEKDAY_WED,
        WEEKDAY_THU,
        WEEKDAY_FRI,
        WEEKDAY_SAT,
        WEEKDAY_SUN,
        _WEEKDAY_MAX,
        _WEEKDAY_INVALID = -EINVAL,
};

/* We assume a maximum timezone length of 6. TZNAME_MAX is not defined on Linux, but glibc internally initializes this
 * to 6. Let's rely on that. */
#define FORMAT_TIMESTAMP_MAX (3U+1U+10U+1U+8U+1U+6U+1U+6U+1U)
#define FORMAT_TIMESTAMP_RELATIVE_MAX 256U
#define FORMAT_TIMESPAN_MAX 64U

#define TIME_T_MAX (time_t)((UINTMAX_C(1) << ((परिमाण(time_t) << 3) - 1)) - 1)

#define DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#define DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#define TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#define TIMESPEC_OMIT ((ध्रुव संरचना timespec) { .tv_nsec = UTIME_OMIT })

usec_t now(clockid_t clock);
nsec_t now_nsec(clockid_t clock);

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base);
usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock);

dual_timestamp* dual_timestamp_now(dual_timestamp *ts);
dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u);

triple_timestamp* triple_timestamp_now(triple_timestamp *ts);
triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u);
triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u);

#define DUAL_TIMESTAMP_HAS_CLOCK(clock)                               \
        IN_SET(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC)

#define TRIPLE_TIMESTAMP_HAS_CLOCK(clock)                               \
        IN_SET(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC, CLOCK_BOOTTIME, CLOCK_BOOTTIME_ALARM)

स्थिर अन्तःस्थ bool timestamp_is_set(usec_t timestamp) {
        पटिनिवत्त timestamp > 0 && timestamp != USEC_INFINITY;
}

स्थिर अन्तःस्थ bool dual_timestamp_is_set(ध्रुव dual_timestamp *ts) {
        पटिनिवत्त timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic);
}

स्थिर अन्तःस्थ bool triple_timestamp_is_set(ध्रुव triple_timestamp *ts) {
        पटिनिवत्त timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic) ||
               timestamp_is_set(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_load(ध्रुव संरचना timespec *ts) _pure_;
nsec_t timespec_load_nsec(ध्रुव संरचना timespec *ts) _pure_;
संरचना timespec* timespec_store(संरचना timespec *ts, usec_t u);
संरचना timespec* timespec_store_nsec(संरचना timespec *ts, nsec_t n);

#define TIMESPEC_STORE(u) timespec_store(&(संरचना timespec) {}, (u))
#define TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(संरचना timespec) {}, (n))

संरचना __kernel_timespec* kernel_timespec_store(संरचना __kernel_timespec *ts, usec_t u);

usec_t timeval_load(ध्रुव संरचना timeval *tv) _pure_;
संरचना timeval* timeval_store(संरचना timeval *tv, usec_t u);

#define TIMEVAL_STORE(u) timeval_store(&(संरचना timeval) {}, (u))

वर्ण* format_timestamp_style(वर्ण *buf, size_t l, usec_t t, TimestampStyle style) _warn_unused_result_;
वर्ण* format_timestamp_relative_full(वर्ण *buf, size_t l, usec_t t, clockid_t clock, bool implicit_वामः) _warn_unused_result_;
वर्ण* format_timespan(वर्ण *buf, size_t l, usec_t t, usec_t accuracy) _warn_unused_result_;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
ध्रुव वर्ण* weekday_to_string(पूर्णाङ्क i);

_warn_unused_result_
स्थिर अन्तःस्थ वर्ण* format_timestamp_relative(वर्ण *buf, size_t l, usec_t t) {
        पटिनिवत्त format_timestamp_relative_full(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
_warn_unused_result_
स्थिर अन्तःस्थ वर्ण* format_timestamp_relative_monotonic(वर्ण *buf, size_t l, usec_t t) {
        पटिनिवत्त format_timestamp_relative_full(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

_warn_unused_result_
स्थिर अन्तःस्थ वर्ण* format_timestamp(वर्ण *buf, size_t l, usec_t t) {
        पटिनिवत्त format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_TIMESTAMP(t) format_timestamp((वर्ण[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((वर्ण[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((वर्ण[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESPAN(t, accuracy) format_timespan((वर्ण[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#define FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((वर्ण[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

ध्रुव वर्ण* get_tzname(bool dst);
पूर्णाङ्क parse_gmtoff(ध्रुव वर्ण *t, long *ret);
पूर्णाङ्क parse_timestamp(ध्रुव वर्ण *t, usec_t *ret);

पूर्णाङ्क parse_sec(ध्रुव वर्ण *t, usec_t *ret);
पूर्णाङ्क parse_sec_fix_0(ध्रुव वर्ण *t, usec_t *ret);
पूर्णाङ्क parse_sec_def_infinity(ध्रुव वर्ण *t, usec_t *ret);
पूर्णाङ्क parse_time(ध्रुव वर्ण *t, usec_t *ret, usec_t default_unit);
पूर्णाङ्क parse_nsec(ध्रुव वर्ण *t, nsec_t *ret);

पूर्णाङ्क get_timezones(वर्ण ***ret);
पूर्णाङ्क verify_timezone(ध्रुव वर्ण *name, पूर्णाङ्क log_level);
स्थिर अन्तःस्थ bool timezone_is_valid(ध्रुव वर्ण *name, पूर्णाङ्क log_level) {
        पटिनिवत्त verify_timezone(name, log_level) >= 0;
}

शून्य reset_timezonep(वर्ण **p);
वर्ण* save_timezone(शून्य);
#define SAVE_TIMEZONE                                                   \
        _unused_ _cleanup_(reset_timezonep)                             \
             वर्ण *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to);

पूर्णाङ्क get_timezone(वर्ण **ret);
पूर्णाङ्क get_timezone_prefer_env(वर्ण **ret);
ध्रुव वर्ण* etc_localtime(शून्य);

पूर्णाङ्क mktime_or_timegm_usec(संरचना tm *tm, bool utc, usec_t *ret);
पूर्णाङ्क localtime_or_gmtime_usec(usec_t t, bool utc, संरचना tm *ret);

पूर्णाङ्क parse_calendar_date_full(ध्रुव वर्ण *s, bool allow_pre_epoch, usec_t *ret_usec, संरचना tm *ret_tm);

स्थिर अन्तःस्थ पूर्णाङ्क parse_calendar_date(ध्रुव वर्ण *s, usec_t *ret) {
        पटिनिवत्त parse_calendar_date_full(s, /* allow_pre_epoch= */ false, ret, रिक्त);
}

#define BIRTH_DATE_UNSET                        \
        (ध्रुव संरचना tm) {                     \
                .tm_year = INT_MIN,             \
        }

#define BIRTH_DATE_IS_SET(tm) ((tm).tm_year != INT_MIN)

स्थिर अन्तःस्थ पूर्णाङ्क parse_birth_date(ध्रुव वर्ण *s, संरचना tm *ret) {
        पटिनिवत्त parse_calendar_date_full(s, /* allow_pre_epoch= */ true, रिक्त, ret);
}

uint64_t sysconf_clock_ticks_cached(शून्य);

uint32_t usec_to_jiffies(usec_t usec);
usec_t jiffies_to_usec(uint32_t jiffies);

bool in_utc_timezone(शून्य);

स्थिर अन्तःस्थ usec_t usec_add(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        पटिनिवत्त saturate_add(a, b, USEC_INFINITY);
}

स्थिर अन्तःस्थ usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        सचे (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                पटिनिवत्त USEC_INFINITY;
        सचे (timestamp < delta)
                पटिनिवत्त 0;

        पटिनिवत्त timestamp - delta;
}

स्थिर अन्तःस्थ usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        सचे (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                पटिनिवत्त usec_add(timestamp, (usec_t) INT64_MAX + 1);
        }
        सचे (delta < 0)
                पटिनिवत्त usec_add(timestamp, (usec_t) (-delta));

        पटिनिवत्त usec_sub_unsigned(timestamp, (usec_t) delta);
}

पूर्णाङ्क usleep_safe(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT (((usec_t) INT32_MAX) * USEC_PER_SEC - USEC_PER_DAY)
#सचे SIZEOF_TIME_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT
#नोचेद्यदि SIZEOF_TIME_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT
#अञ्ञथा
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#endif

पूर्णाङ्क time_change_fd(शून्य);

DECLARE_STRING_TABLE_LOOKUP(timestamp_style, TimestampStyle);
