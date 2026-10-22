/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <limits.h>
#ଅନ୍ତର୍ଭାଵ <time.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define PRI_NSEC PRIu64
#define PRI_USEC PRIu64
#define NSEC_FMT "%" PRI_NSEC
#define USEC_FMT "%" PRI_USEC

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା TimestampStyle {
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

ଗଣନା {
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

#define TIME_T_MAX (time_t)((UINTMAX_C(1) << ((ଆକାର(time_t) << 3) - 1)) - 1)

#define DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#define DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#define TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#define TIMESPEC_OMIT ((ଅଚଳ ସଂରଚନା timespec) { .tv_nsec = UTIME_OMIT })

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

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool timestamp_is_set(usec_t timestamp) {
        ଫେରନ୍ତୁ timestamp > 0 && timestamp != USEC_INFINITY;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool dual_timestamp_is_set(ଅଚଳ dual_timestamp *ts) {
        ଫେରନ୍ତୁ timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool triple_timestamp_is_set(ଅଚଳ triple_timestamp *ts) {
        ଫେରନ୍ତୁ timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic) ||
               timestamp_is_set(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_load(ଅଚଳ ସଂରଚନା timespec *ts) _pure_;
nsec_t timespec_load_nsec(ଅଚଳ ସଂରଚନା timespec *ts) _pure_;
ସଂରଚନା timespec* timespec_store(ସଂରଚନା timespec *ts, usec_t u);
ସଂରଚନା timespec* timespec_store_nsec(ସଂରଚନା timespec *ts, nsec_t n);

#define TIMESPEC_STORE(u) timespec_store(&(ସଂରଚନା timespec) {}, (u))
#define TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(ସଂରଚନା timespec) {}, (n))

ସଂରଚନା __kernel_timespec* kernel_timespec_store(ସଂରଚନା __kernel_timespec *ts, usec_t u);

usec_t timeval_load(ଅଚଳ ସଂରଚନା timeval *tv) _pure_;
ସଂରଚନା timeval* timeval_store(ସଂରଚନା timeval *tv, usec_t u);

#define TIMEVAL_STORE(u) timeval_store(&(ସଂରଚନା timeval) {}, (u))

ଅକ୍ଷର* format_timestamp_style(ଅକ୍ଷର *buf, size_t l, usec_t t, TimestampStyle style) _warn_unused_result_;
ଅକ୍ଷର* format_timestamp_relative_full(ଅକ୍ଷର *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ବାମ) _warn_unused_result_;
ଅକ୍ଷର* format_timespan(ଅକ୍ଷର *buf, size_t l, usec_t t, usec_t accuracy) _warn_unused_result_;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
ଅଚଳ ଅକ୍ଷର* weekday_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i);

_warn_unused_result_
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* format_timestamp_relative(ଅକ୍ଷର *buf, size_t l, usec_t t) {
        ଫେରନ୍ତୁ format_timestamp_relative_full(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
_warn_unused_result_
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* format_timestamp_relative_monotonic(ଅକ୍ଷର *buf, size_t l, usec_t t) {
        ଫେରନ୍ତୁ format_timestamp_relative_full(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

_warn_unused_result_
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* format_timestamp(ଅକ୍ଷର *buf, size_t l, usec_t t) {
        ଫେରନ୍ତୁ format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#define FORMAT_TIMESTAMP(t) format_timestamp((ଅକ୍ଷର[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((ଅକ୍ଷର[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((ଅକ୍ଷର[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#define FORMAT_TIMESPAN(t, accuracy) format_timespan((ଅକ୍ଷର[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#define FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((ଅକ୍ଷର[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

ଅଚଳ ଅକ୍ଷର* get_tzname(bool dst);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_gmtoff(ଅଚଳ ଅକ୍ଷର *t, long *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_timestamp(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sec(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sec_fix_0(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sec_def_infinity(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_time(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret, usec_t default_unit);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_nsec(ଅଚଳ ଅକ୍ଷର *t, nsec_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_timezones(ଅକ୍ଷର ***ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_timezone(ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool timezone_is_valid(ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
        ଫେରନ୍ତୁ verify_timezone(name, log_level) >= 0;
}

ଶୂନ୍ୟ reset_timezonep(ଅକ୍ଷର **p);
ଅକ୍ଷର* save_timezone(ଶୂନ୍ୟ);
#define SAVE_TIMEZONE                                                   \
        _unused_ _cleanup_(reset_timezonep)                             \
             ଅକ୍ଷର *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_timezone(ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_timezone_prefer_env(ଅକ୍ଷର **ret);
ଅଚଳ ଅକ୍ଷର* etc_localtime(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mktime_or_timegm_usec(ସଂରଚନା tm *tm, bool utc, usec_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା localtime_or_gmtime_usec(usec_t t, bool utc, ସଂରଚନା tm *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_calendar_date_full(ଅଚଳ ଅକ୍ଷର *s, bool allow_pre_epoch, usec_t *ret_usec, ସଂରଚନା tm *ret_tm);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_calendar_date(ଅଚଳ ଅକ୍ଷର *s, usec_t *ret) {
        ଫେରନ୍ତୁ parse_calendar_date_full(s, /* allow_pre_epoch= */ false, ret, ଖାଲି);
}

#define BIRTH_DATE_UNSET                        \
        (ଅଚଳ ସଂରଚନା tm) {                     \
                .tm_year = INT_MIN,             \
        }

#define BIRTH_DATE_IS_SET(tm) ((tm).tm_year != INT_MIN)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_birth_date(ଅଚଳ ଅକ୍ଷର *s, ସଂରଚନା tm *ret) {
        ଫେରନ୍ତୁ parse_calendar_date_full(s, /* allow_pre_epoch= */ true, ଖାଲି, ret);
}

uint64_t sysconf_clock_ticks_cached(ଶୂନ୍ୟ);

uint32_t usec_to_jiffies(usec_t usec);
usec_t jiffies_to_usec(uint32_t jiffies);

bool in_utc_timezone(ଶୂନ୍ୟ);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ usec_t usec_add(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        ଫେରନ୍ତୁ saturate_add(a, b, USEC_INFINITY);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        ଯଦି (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                ଫେରନ୍ତୁ USEC_INFINITY;
        ଯଦି (timestamp < delta)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ timestamp - delta;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        ଯଦି (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                ଫେରନ୍ତୁ usec_add(timestamp, (usec_t) INT64_MAX + 1);
        }
        ଯଦି (delta < 0)
                ଫେରନ୍ତୁ usec_add(timestamp, (usec_t) (-delta));

        ଫେରନ୍ତୁ usec_sub_unsigned(timestamp, (usec_t) delta);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା usleep_safe(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#define USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT (((usec_t) INT32_MAX) * USEC_PER_SEC - USEC_PER_DAY)
#ଯଦି SIZEOF_TIME_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT
#ନଚେତଯଦି SIZEOF_TIME_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT
#ଅନ୍ୟଥା
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା time_change_fd(ଶୂନ୍ୟ);

DECLARE_STRING_TABLE_LOOKUP(timestamp_style, TimestampStyle);
