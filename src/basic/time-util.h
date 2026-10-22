/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <limits.h>
#அந்தர்பாவ <time.h>

#அந்தர்பாவ "forward.h"

#define PRI_NSEC PRIu64
#define PRI_USEC PRIu64
#define NSEC_FMT "%" PRI_NSEC
#define USEC_FMT "%" PRI_USEC

வகைவரை கட்டமைப்பு dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

வகைவரை கட்டமைப்பு triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

வகைவரை எண்ணிக்கை TimestampStyle {
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

எண்ணிக்கை {
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

#define TIME_T_MAX (time_t)((UINTMAX_C(1) << ((அளவு(time_t) << 3) - 1)) - 1)

#define DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#define DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#define TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#define TIMESPEC_OMIT ((மாறிலி கட்டமைப்பு timespec) { .tv_nsec = UTIME_OMIT })

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

நிலையான உள்வரி bool timestamp_is_set(usec_t timestamp) {
        திரும்பு timestamp > 0 && timestamp != USEC_INFINITY;
}

நிலையான உள்வரி bool dual_timestamp_is_set(மாறிலி dual_timestamp *ts) {
        திரும்பு timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic);
}

நிலையான உள்வரி bool triple_timestamp_is_set(மாறிலி triple_timestamp *ts) {
        திரும்பு timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic) ||
               timestamp_is_set(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_load(மாறிலி கட்டமைப்பு timespec *ts) _pure_;
nsec_t timespec_load_nsec(மாறிலி கட்டமைப்பு timespec *ts) _pure_;
கட்டமைப்பு timespec* timespec_store(கட்டமைப்பு timespec *ts, usec_t u);
கட்டமைப்பு timespec* timespec_store_nsec(கட்டமைப்பு timespec *ts, nsec_t n);

#define TIMESPEC_STORE(u) timespec_store(&(கட்டமைப்பு timespec) {}, (u))
#define TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(கட்டமைப்பு timespec) {}, (n))

கட்டமைப்பு __kernel_timespec* kernel_timespec_store(கட்டமைப்பு __kernel_timespec *ts, usec_t u);

usec_t timeval_load(மாறிலி கட்டமைப்பு timeval *tv) _pure_;
கட்டமைப்பு timeval* timeval_store(கட்டமைப்பு timeval *tv, usec_t u);

#define TIMEVAL_STORE(u) timeval_store(&(கட்டமைப்பு timeval) {}, (u))

எழுத்து* format_timestamp_style(எழுத்து *buf, size_t l, usec_t t, TimestampStyle style) _warn_unused_result_;
எழுத்து* format_timestamp_relative_full(எழுத்து *buf, size_t l, usec_t t, clockid_t clock, bool implicit_இடது) _warn_unused_result_;
எழுத்து* format_timespan(எழுத்து *buf, size_t l, usec_t t, usec_t accuracy) _warn_unused_result_;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
மாறிலி எழுத்து* weekday_to_string(முழு i);

_warn_unused_result_
நிலையான உள்வரி எழுத்து* format_timestamp_relative(எழுத்து *buf, size_t l, usec_t t) {
        திரும்பு format_timestamp_relative_full(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
_warn_unused_result_
நிலையான உள்வரி எழுத்து* format_timestamp_relative_monotonic(எழுத்து *buf, size_t l, usec_t t) {
        திரும்பு format_timestamp_relative_full(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

_warn_unused_result_
நிலையான உள்வரி எழுத்து* format_timestamp(எழுத்து *buf, size_t l, usec_t t) {
        திரும்பு format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_TIMESTAMP(t) format_timestamp((எழுத்து[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((எழுத்து[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((எழுத்து[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESPAN(t, accuracy) format_timespan((எழுத்து[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#define FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((எழுத்து[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

மாறிலி எழுத்து* get_tzname(bool dst);
முழு parse_gmtoff(மாறிலி எழுத்து *t, long *ret);
முழு parse_timestamp(மாறிலி எழுத்து *t, usec_t *ret);

முழு parse_sec(மாறிலி எழுத்து *t, usec_t *ret);
முழு parse_sec_fix_0(மாறிலி எழுத்து *t, usec_t *ret);
முழு parse_sec_def_infinity(மாறிலி எழுத்து *t, usec_t *ret);
முழு parse_time(மாறிலி எழுத்து *t, usec_t *ret, usec_t default_unit);
முழு parse_nsec(மாறிலி எழுத்து *t, nsec_t *ret);

முழு get_timezones(எழுத்து ***ret);
முழு verify_timezone(மாறிலி எழுத்து *name, முழு log_level);
நிலையான உள்வரி bool timezone_is_valid(மாறிலி எழுத்து *name, முழு log_level) {
        திரும்பு verify_timezone(name, log_level) >= 0;
}

வெறுமை reset_timezonep(எழுத்து **p);
எழுத்து* save_timezone(வெறுமை);
#define SAVE_TIMEZONE                                                   \
        _unused_ _cleanup_(reset_timezonep)                             \
             எழுத்து *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to);

முழு get_timezone(எழுத்து **ret);
முழு get_timezone_prefer_env(எழுத்து **ret);
மாறிலி எழுத்து* etc_localtime(வெறுமை);

முழு mktime_or_timegm_usec(கட்டமைப்பு tm *tm, bool utc, usec_t *ret);
முழு localtime_or_gmtime_usec(usec_t t, bool utc, கட்டமைப்பு tm *ret);

முழு parse_calendar_date_full(மாறிலி எழுத்து *s, bool allow_pre_epoch, usec_t *ret_usec, கட்டமைப்பு tm *ret_tm);

நிலையான உள்வரி முழு parse_calendar_date(மாறிலி எழுத்து *s, usec_t *ret) {
        திரும்பு parse_calendar_date_full(s, /* allow_pre_epoch= */ false, ret, வெற்று);
}

#define BIRTH_DATE_UNSET                        \
        (மாறிலி கட்டமைப்பு tm) {                     \
                .tm_year = INT_MIN,             \
        }

#define BIRTH_DATE_IS_SET(tm) ((tm).tm_year != INT_MIN)

நிலையான உள்வரி முழு parse_birth_date(மாறிலி எழுத்து *s, கட்டமைப்பு tm *ret) {
        திரும்பு parse_calendar_date_full(s, /* allow_pre_epoch= */ true, வெற்று, ret);
}

uint64_t sysconf_clock_ticks_cached(வெறுமை);

uint32_t usec_to_jiffies(usec_t usec);
usec_t jiffies_to_usec(uint32_t jiffies);

bool in_utc_timezone(வெறுமை);

நிலையான உள்வரி usec_t usec_add(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        திரும்பு saturate_add(a, b, USEC_INFINITY);
}

நிலையான உள்வரி usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        எனில் (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                திரும்பு USEC_INFINITY;
        எனில் (timestamp < delta)
                திரும்பு 0;

        திரும்பு timestamp - delta;
}

நிலையான உள்வரி usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        எனில் (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                திரும்பு usec_add(timestamp, (usec_t) INT64_MAX + 1);
        }
        எனில் (delta < 0)
                திரும்பு usec_add(timestamp, (usec_t) (-delta));

        திரும்பு usec_sub_unsigned(timestamp, (usec_t) delta);
}

முழு usleep_safe(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT (((usec_t) INT32_MAX) * USEC_PER_SEC - USEC_PER_DAY)
#எனில் SIZEOF_TIME_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT
#அல்லதெனில் SIZEOF_TIME_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT
#இல்லையெனில்
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#endif

முழு time_change_fd(வெறுமை);

DECLARE_STRING_TABLE_LOOKUP(timestamp_style, TimestampStyle);
