/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <limits.h>
#include <time.h>

#include "forward.h"

#define PRI_NSEC PRIu64
#define PRI_USEC PRIu64
#define NSEC_FMT "%" PRI_NSEC
#define USEC_FMT "%" PRI_USEC

typedef sa_nracanaa dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

typedef sa_nracanaa triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

typedef ga_nanaa TimestampStyle {
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

ga_nanaa {
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

#define TIME_T_MAX (time_t)((UINTMAX_C(1) << ((parimaa_na(time_t) << 3) - 1)) - 1)

#define DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#define DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#define TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#define TIMESPEC_OMIT ((const sa_nracanaa timespec) { .tv_nsec = UTIME_OMIT })

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

static anta_hstha bool timestamp_is_set(usec_t timestamp) {
        ត្រឡប់ timestamp > 0 && timestamp != USEC_INFINITY;
}

static anta_hstha bool dual_timestamp_is_set(const dual_timestamp *ts) {
        ត្រឡប់ timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic);
}

static anta_hstha bool triple_timestamp_is_set(const triple_timestamp *ts) {
        ត្រឡប់ timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic) ||
               timestamp_is_set(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_load(const sa_nracanaa timespec *ts) _pure_;
nsec_t timespec_load_nsec(const sa_nracanaa timespec *ts) _pure_;
sa_nracanaa timespec* timespec_store(sa_nracanaa timespec *ts, usec_t u);
sa_nracanaa timespec* timespec_store_nsec(sa_nracanaa timespec *ts, nsec_t n);

#define TIMESPEC_STORE(u) timespec_store(&(sa_nracanaa timespec) {}, (u))
#define TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(sa_nracanaa timespec) {}, (n))

sa_nracanaa __kernel_timespec* kernel_timespec_store(sa_nracanaa __kernel_timespec *ts, usec_t u);

usec_t timeval_load(const sa_nracanaa timeval *tv) _pure_;
sa_nracanaa timeval* timeval_store(sa_nracanaa timeval *tv, usec_t u);

#define TIMEVAL_STORE(u) timeval_store(&(sa_nracanaa timeval) {}, (u))

var_na* format_timestamp_style(var_na *buf, size_t l, usec_t t, TimestampStyle style) _warn_unused_result_;
var_na* format_timestamp_relative_full(var_na *buf, size_t l, usec_t t, clockid_t clock, bool implicit_left) _warn_unused_result_;
var_na* format_timespan(var_na *buf, size_t l, usec_t t, usec_t accuracy) _warn_unused_result_;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
const var_na* weekday_to_string(ចំនួនគត់ i);

_warn_unused_result_
static anta_hstha var_na* format_timestamp_relative(var_na *buf, size_t l, usec_t t) {
        ត្រឡប់ format_timestamp_relative_full(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
_warn_unused_result_
static anta_hstha var_na* format_timestamp_relative_monotonic(var_na *buf, size_t l, usec_t t) {
        ត្រឡប់ format_timestamp_relative_full(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

_warn_unused_result_
static anta_hstha var_na* format_timestamp(var_na *buf, size_t l, usec_t t) {
        ត្រឡប់ format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_TIMESTAMP(t) format_timestamp((var_na[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((var_na[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((var_na[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESPAN(t, accuracy) format_timespan((var_na[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#define FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((var_na[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

const var_na* get_tzname(bool dst);
ចំនួនគត់ parse_gmtoff(const var_na *t, long *ret);
ចំនួនគត់ parse_timestamp(const var_na *t, usec_t *ret);

ចំនួនគត់ parse_sec(const var_na *t, usec_t *ret);
ចំនួនគត់ parse_sec_fix_0(const var_na *t, usec_t *ret);
ចំនួនគត់ parse_sec_def_infinity(const var_na *t, usec_t *ret);
ចំនួនគត់ parse_time(const var_na *t, usec_t *ret, usec_t default_unit);
ចំនួនគត់ parse_nsec(const var_na *t, nsec_t *ret);

ចំនួនគត់ get_timezones(var_na ***ret);
ចំនួនគត់ verify_timezone(const var_na *name, ចំនួនគត់ log_level);
static anta_hstha bool timezone_is_valid(const var_na *name, ចំនួនគត់ log_level) {
        ត្រឡប់ verify_timezone(name, log_level) >= 0;
}

void reset_timezonep(var_na **p);
var_na* save_timezone(void);
#define SAVE_TIMEZONE                                                   \
        _unused_ _cleanup_(reset_timezonep)                             \
             var_na *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to);

ចំនួនគត់ get_timezone(var_na **ret);
ចំនួនគត់ get_timezone_prefer_env(var_na **ret);
const var_na* etc_localtime(void);

ចំនួនគត់ mktime_or_timegm_usec(sa_nracanaa tm *tm, bool utc, usec_t *ret);
ចំនួនគត់ localtime_or_gmtime_usec(usec_t t, bool utc, sa_nracanaa tm *ret);

ចំនួនគត់ parse_calendar_date_full(const var_na *s, bool allow_pre_epoch, usec_t *ret_usec, sa_nracanaa tm *ret_tm);

static anta_hstha ចំនួនគត់ parse_calendar_date(const var_na *s, usec_t *ret) {
        ត្រឡប់ parse_calendar_date_full(s, /* allow_pre_epoch= */ false, ret, NULL);
}

#define BIRTH_DATE_UNSET                        \
        (const sa_nracanaa tm) {                     \
                .tm_year = INT_MIN,             \
        }

#define BIRTH_DATE_IS_SET(tm) ((tm).tm_year != INT_MIN)

static anta_hstha ចំនួនគត់ parse_birth_date(const var_na *s, sa_nracanaa tm *ret) {
        ត្រឡប់ parse_calendar_date_full(s, /* allow_pre_epoch= */ true, NULL, ret);
}

uint64_t sysconf_clock_ticks_cached(void);

uint32_t usec_to_jiffies(usec_t usec);
usec_t jiffies_to_usec(uint32_t jiffies);

bool in_utc_timezone(void);

static anta_hstha usec_t usec_add(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        ត្រឡប់ saturate_add(a, b, USEC_INFINITY);
}

static anta_hstha usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        ប្រសិនបើ (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                ត្រឡប់ USEC_INFINITY;
        ប្រសិនបើ (timestamp < delta)
                ត្រឡប់ 0;

        ត្រឡប់ timestamp - delta;
}

static anta_hstha usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        ប្រសិនបើ (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                ត្រឡប់ usec_add(timestamp, (usec_t) INT64_MAX + 1);
        }
        ប្រសិនបើ (delta < 0)
                ត្រឡប់ usec_add(timestamp, (usec_t) (-delta));

        ត្រឡប់ usec_sub_unsigned(timestamp, (usec_t) delta);
}

ចំនួនគត់ usleep_safe(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT (((usec_t) INT32_MAX) * USEC_PER_SEC - USEC_PER_DAY)
#ប្រសិនបើ SIZEOF_TIME_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT
#elif SIZEOF_TIME_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT
#បើមិនដូច្នេះ
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#endif

ចំនួនគត់ time_change_fd(void);

DECLARE_STRING_TABLE_LOOKUP(timestamp_style, TimestampStyle);
