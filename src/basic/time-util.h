/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <limits.h>
#অন্তর্ভাৱ <time.h>

#অন্তর্ভাৱ "forward.h"

#define PRI_NSEC PRIu64
#define PRI_USEC PRIu64
#define NSEC_FMT "%" PRI_NSEC
#define USEC_FMT "%" PRI_USEC

প্রকারনির্দেশ সংরচনা dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

প্রকারনির্দেশ সংরচনা triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

প্রকারনির্দেশ গণনা TimestampStyle {
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

গণনা {
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

#define TIME_T_MAX (time_t)((UINTMAX_C(1) << ((পরিমাণ(time_t) << 3) - 1)) - 1)

#define DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#define DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#define TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#define TIMESPEC_OMIT ((ধ্রুৱ সংরচনা timespec) { .tv_nsec = UTIME_OMIT })

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

স্থির অন্তঃস্থ bool timestamp_is_set(usec_t timestamp) {
        ঘূৰাই timestamp > 0 && timestamp != USEC_INFINITY;
}

স্থির অন্তঃস্থ bool dual_timestamp_is_set(ধ্রুৱ dual_timestamp *ts) {
        ঘূৰাই timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic);
}

স্থির অন্তঃস্থ bool triple_timestamp_is_set(ধ্রুৱ triple_timestamp *ts) {
        ঘূৰাই timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic) ||
               timestamp_is_set(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_load(ধ্রুৱ সংরচনা timespec *ts) _pure_;
nsec_t timespec_load_nsec(ধ্রুৱ সংরচনা timespec *ts) _pure_;
সংরচনা timespec* timespec_store(সংরচনা timespec *ts, usec_t u);
সংরচনা timespec* timespec_store_nsec(সংরচনা timespec *ts, nsec_t n);

#define TIMESPEC_STORE(u) timespec_store(&(সংরচনা timespec) {}, (u))
#define TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(সংরচনা timespec) {}, (n))

সংরচনা __kernel_timespec* kernel_timespec_store(সংরচনা __kernel_timespec *ts, usec_t u);

usec_t timeval_load(ধ্রুৱ সংরচনা timeval *tv) _pure_;
সংরচনা timeval* timeval_store(সংরচনা timeval *tv, usec_t u);

#define TIMEVAL_STORE(u) timeval_store(&(সংরচনা timeval) {}, (u))

ৱর্ণ* format_timestamp_style(ৱর্ণ *buf, size_t l, usec_t t, TimestampStyle style) _warn_unused_result_;
ৱর্ণ* format_timestamp_relative_full(ৱর্ণ *buf, size_t l, usec_t t, clockid_t clock, bool implicit_বাম) _warn_unused_result_;
ৱর্ণ* format_timespan(ৱর্ণ *buf, size_t l, usec_t t, usec_t accuracy) _warn_unused_result_;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
ধ্রুৱ ৱর্ণ* weekday_to_string(পূর্ণাঙ্ক i);

_warn_unused_result_
স্থির অন্তঃস্থ ৱর্ণ* format_timestamp_relative(ৱর্ণ *buf, size_t l, usec_t t) {
        ঘূৰাই format_timestamp_relative_full(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
_warn_unused_result_
স্থির অন্তঃস্থ ৱর্ণ* format_timestamp_relative_monotonic(ৱর্ণ *buf, size_t l, usec_t t) {
        ঘূৰাই format_timestamp_relative_full(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

_warn_unused_result_
স্থির অন্তঃস্থ ৱর্ণ* format_timestamp(ৱর্ণ *buf, size_t l, usec_t t) {
        ঘূৰাই format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_TIMESTAMP(t) format_timestamp((ৱর্ণ[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((ৱর্ণ[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((ৱর্ণ[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESPAN(t, accuracy) format_timespan((ৱর্ণ[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#define FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((ৱর্ণ[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

ধ্রুৱ ৱর্ণ* get_tzname(bool dst);
পূর্ণাঙ্ক parse_gmtoff(ধ্রুৱ ৱর্ণ *t, long *ret);
পূর্ণাঙ্ক parse_timestamp(ধ্রুৱ ৱর্ণ *t, usec_t *ret);

পূর্ণাঙ্ক parse_sec(ধ্রুৱ ৱর্ণ *t, usec_t *ret);
পূর্ণাঙ্ক parse_sec_fix_0(ধ্রুৱ ৱর্ণ *t, usec_t *ret);
পূর্ণাঙ্ক parse_sec_def_infinity(ধ্রুৱ ৱর্ণ *t, usec_t *ret);
পূর্ণাঙ্ক parse_time(ধ্রুৱ ৱর্ণ *t, usec_t *ret, usec_t default_unit);
পূর্ণাঙ্ক parse_nsec(ধ্রুৱ ৱর্ণ *t, nsec_t *ret);

পূর্ণাঙ্ক get_timezones(ৱর্ণ ***ret);
পূর্ণাঙ্ক verify_timezone(ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক log_level);
স্থির অন্তঃস্থ bool timezone_is_valid(ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক log_level) {
        ঘূৰাই verify_timezone(name, log_level) >= 0;
}

শূন্য reset_timezonep(ৱর্ণ **p);
ৱর্ণ* save_timezone(শূন্য);
#define SAVE_TIMEZONE                                                   \
        _unused_ _cleanup_(reset_timezonep)                             \
             ৱর্ণ *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to);

পূর্ণাঙ্ক get_timezone(ৱর্ণ **ret);
পূর্ণাঙ্ক get_timezone_prefer_env(ৱর্ণ **ret);
ধ্রুৱ ৱর্ণ* etc_localtime(শূন্য);

পূর্ণাঙ্ক mktime_or_timegm_usec(সংরচনা tm *tm, bool utc, usec_t *ret);
পূর্ণাঙ্ক localtime_or_gmtime_usec(usec_t t, bool utc, সংরচনা tm *ret);

পূর্ণাঙ্ক parse_calendar_date_full(ধ্রুৱ ৱর্ণ *s, bool allow_pre_epoch, usec_t *ret_usec, সংরচনা tm *ret_tm);

স্থির অন্তঃস্থ পূর্ণাঙ্ক parse_calendar_date(ধ্রুৱ ৱর্ণ *s, usec_t *ret) {
        ঘূৰাই parse_calendar_date_full(s, /* allow_pre_epoch= */ false, ret, রিক্ত);
}

#define BIRTH_DATE_UNSET                        \
        (ধ্রুৱ সংরচনা tm) {                     \
                .tm_year = INT_MIN,             \
        }

#define BIRTH_DATE_IS_SET(tm) ((tm).tm_year != INT_MIN)

স্থির অন্তঃস্থ পূর্ণাঙ্ক parse_birth_date(ধ্রুৱ ৱর্ণ *s, সংরচনা tm *ret) {
        ঘূৰাই parse_calendar_date_full(s, /* allow_pre_epoch= */ true, রিক্ত, ret);
}

uint64_t sysconf_clock_ticks_cached(শূন্য);

uint32_t usec_to_jiffies(usec_t usec);
usec_t jiffies_to_usec(uint32_t jiffies);

bool in_utc_timezone(শূন্য);

স্থির অন্তঃস্থ usec_t usec_add(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        ঘূৰাই saturate_add(a, b, USEC_INFINITY);
}

স্থির অন্তঃস্থ usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        যদি (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                ঘূৰাই USEC_INFINITY;
        যদি (timestamp < delta)
                ঘূৰাই 0;

        ঘূৰাই timestamp - delta;
}

স্থির অন্তঃস্থ usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        যদি (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                ঘূৰাই usec_add(timestamp, (usec_t) INT64_MAX + 1);
        }
        যদি (delta < 0)
                ঘূৰাই usec_add(timestamp, (usec_t) (-delta));

        ঘূৰাই usec_sub_unsigned(timestamp, (usec_t) delta);
}

পূর্ণাঙ্ক usleep_safe(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT (((usec_t) INT32_MAX) * USEC_PER_SEC - USEC_PER_DAY)
#যদি SIZEOF_TIME_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT
#নোচেদ্যদি SIZEOF_TIME_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT
#অন্যথা
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#endif

পূর্ণাঙ্ক time_change_fd(শূন্য);

DECLARE_STRING_TABLE_LOOKUP(timestamp_style, TimestampStyle);
