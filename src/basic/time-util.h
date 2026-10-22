/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <limits.h>
#అన్తర్భావ <time.h>

#అన్తర్భావ "forward.h"

#నిర్వచించు PRI_NSEC PRIu64
#నిర్వచించు PRI_USEC PRIu64
#నిర్వచించు NSEC_FMT "%" PRI_NSEC
#నిర్వచించు USEC_FMT "%" PRI_USEC

రకనిర్వచనం నిర్మాణం dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

రకనిర్వచనం నిర్మాణం triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

రకనిర్వచనం గణన TimestampStyle {
        TIMESTAMP_PRETTY,
        TIMESTAMP_US,
        TIMESTAMP_UTC,
        TIMESTAMP_US_UTC,
        TIMESTAMP_UNIX,
        TIMESTAMP_DATE,
        _TIMESTAMP_STYLE_MAX,
        _TIMESTAMP_STYLE_INVALID = -EINVAL,
} TimestampStyle;

#నిర్వచించు USEC_INFINITY ((usec_t) UINT64_MAX)
#నిర్వచించు NSEC_INFINITY ((nsec_t) UINT64_MAX)

#నిర్వచించు MSEC_చొప్పున_SEC  1000ULL
#నిర్వచించు USEC_చొప్పున_SEC  ((usec_t) 1000000ULL)
#నిర్వచించు USEC_చొప్పున_MSEC ((usec_t) 1000ULL)
#నిర్వచించు NSEC_చొప్పున_SEC  ((nsec_t) 1000000000ULL)
#నిర్వచించు NSEC_చొప్పున_MSEC ((nsec_t) 1000000ULL)
#నిర్వచించు NSEC_చొప్పున_USEC ((nsec_t) 1000ULL)

#నిర్వచించు USEC_చొప్పున_MINUTE ((usec_t) (60ULL*USEC_చొప్పున_SEC))
#నిర్వచించు NSEC_చొప్పున_MINUTE ((nsec_t) (60ULL*NSEC_చొప్పున_SEC))
#నిర్వచించు USEC_చొప్పున_HOUR ((usec_t) (60ULL*USEC_చొప్పున_MINUTE))
#నిర్వచించు NSEC_చొప్పున_HOUR ((nsec_t) (60ULL*NSEC_చొప్పున_MINUTE))
#నిర్వచించు USEC_చొప్పున_DAY ((usec_t) (24ULL*USEC_చొప్పున_HOUR))
#నిర్వచించు NSEC_చొప్పున_DAY ((nsec_t) (24ULL*NSEC_చొప్పున_HOUR))
#నిర్వచించు USEC_చొప్పున_WEEK ((usec_t) (7ULL*USEC_చొప్పున_DAY))
#నిర్వచించు NSEC_చొప్పున_WEEK ((nsec_t) (7ULL*NSEC_చొప్పున_DAY))
#నిర్వచించు USEC_చొప్పున_MONTH ((usec_t) (2629800ULL*USEC_చొప్పున_SEC))
#నిర్వచించు NSEC_చొప్పున_MONTH ((nsec_t) (2629800ULL*NSEC_చొప్పున_SEC))
#నిర్వచించు USEC_చొప్పున_YEAR ((usec_t) (31557600ULL*USEC_చొప్పున_SEC))
#నిర్వచించు NSEC_చొప్పున_YEAR ((nsec_t) (31557600ULL*NSEC_చొప్పున_SEC))

గణన {
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
#నిర్వచించు FORMAT_TIMESTAMP_MAX (3U+1U+10U+1U+8U+1U+6U+1U+6U+1U)
#నిర్వచించు FORMAT_TIMESTAMP_RELATIVE_MAX 256U
#నిర్వచించు FORMAT_TIMESPAN_MAX 64U

#నిర్వచించు సమయం_T_MAX (time_t)((UINTMAX_C(1) << ((పరిమాణం(time_t) << 3) - 1)) - 1)

#నిర్వచించు DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#నిర్వచించు DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#నిర్వచించు TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#నిర్వచించు TIMESPEC_OMIT ((స్థిరరాశి నిర్మాణం timespec) { .tv_nsec = UTIME_OMIT })

usec_t now(clockid_t clock);
nsec_t now_nsec(clockid_t clock);

usec_t map_clock_usec_raw(usec_t from, usec_t from_పునాది, usec_t గమ్యం_పునాది);
usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t గమ్యం_clock);

dual_timestamp* dual_timestamp_now(dual_timestamp *ts);
dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u);

triple_timestamp* triple_timestamp_now(triple_timestamp *ts);
triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u);
triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u);

#నిర్వచించు DUAL_TIMESTAMP_HAS_CLOCK(clock)                               \
        లోపల_సమితి(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC)

#నిర్వచించు TRIPLE_TIMESTAMP_HAS_CLOCK(clock)                               \
        లోపల_సమితి(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC, CLOCK_BOOTTIME, CLOCK_BOOTTIME_ALARM)

స్థిర అంతఃవరుస bool timestamp_is_సమితి(usec_t timestamp) {
        తిరిగి timestamp > 0 && timestamp != USEC_INFINITY;
}

స్థిర అంతఃవరుస bool dual_timestamp_is_సమితి(స్థిరరాశి dual_timestamp *ts) {
        తిరిగి timestamp_is_సమితి(ts->realtime) ||
               timestamp_is_సమితి(ts->monotonic);
}

స్థిర అంతఃవరుస bool triple_timestamp_is_సమితి(స్థిరరాశి triple_timestamp *ts) {
        తిరిగి timestamp_is_సమితి(ts->realtime) ||
               timestamp_is_సమితి(ts->monotonic) ||
               timestamp_is_సమితి(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_ఎక్కించు(స్థిరరాశి నిర్మాణం timespec *ts) శుద్ధ;
nsec_t timespec_ఎక్కించు_nsec(స్థిరరాశి నిర్మాణం timespec *ts) శుద్ధ;
నిర్మాణం timespec* timespec_store(నిర్మాణం timespec *ts, usec_t u);
నిర్మాణం timespec* timespec_store_nsec(నిర్మాణం timespec *ts, nsec_t n);

#నిర్వచించు TIMESPEC_STORE(u) timespec_store(&(నిర్మాణం timespec) {}, (u))
#నిర్వచించు TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(నిర్మాణం timespec) {}, (n))

నిర్మాణం __kernel_timespec* kernel_timespec_store(నిర్మాణం __kernel_timespec *ts, usec_t u);

usec_t timeval_ఎక్కించు(స్థిరరాశి నిర్మాణం timeval *tv) శుద్ధ;
నిర్మాణం timeval* timeval_store(నిర్మాణం timeval *tv, usec_t u);

#నిర్వచించు TIMEVAL_STORE(u) timeval_store(&(నిర్మాణం timeval) {}, (u))

అక్షరం* format_timestamp_style(అక్షరం *buf, size_t l, usec_t t, TimestampStyle style) warn_వాడని_ఫలితం;
అక్షరం* format_timestamp_relative_నిండిన(అక్షరం *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ఎడమ) warn_వాడని_ఫలితం;
అక్షరం* format_timespan(అక్షరం *buf, size_t l, usec_t t, usec_t accuracy) warn_వాడని_ఫలితం;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
స్థిరరాశి అక్షరం* weekday_గమ్యం_string(పూర్ణాంకం i);

warn_వాడని_ఫలితం
స్థిర అంతఃవరుస అక్షరం* format_timestamp_relative(అక్షరం *buf, size_t l, usec_t t) {
        తిరిగి format_timestamp_relative_నిండిన(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
warn_వాడని_ఫలితం
స్థిర అంతఃవరుస అక్షరం* format_timestamp_relative_monotonic(అక్షరం *buf, size_t l, usec_t t) {
        తిరిగి format_timestamp_relative_నిండిన(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

warn_వాడని_ఫలితం
స్థిర అంతఃవరుస అక్షరం* format_timestamp(అక్షరం *buf, size_t l, usec_t t) {
        తిరిగి format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#నిర్వచించు FORMAT_TIMESTAMP(t) format_timestamp((అక్షరం[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#నిర్వచించు FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((అక్షరం[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#నిర్వచించు FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((అక్షరం[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#నిర్వచించు FORMAT_TIMESPAN(t, accuracy) format_timespan((అక్షరం[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#నిర్వచించు FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((అక్షరం[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

స్థిరరాశి అక్షరం* get_tzname(bool dst);
పూర్ణాంకం parse_gmtoff(స్థిరరాశి అక్షరం *t, దీర్ఘ *ret);
పూర్ణాంకం parse_timestamp(స్థిరరాశి అక్షరం *t, usec_t *ret);

పూర్ణాంకం parse_sec(స్థిరరాశి అక్షరం *t, usec_t *ret);
పూర్ణాంకం parse_sec_fix_0(స్థిరరాశి అక్షరం *t, usec_t *ret);
పూర్ణాంకం parse_sec_def_infinity(స్థిరరాశి అక్షరం *t, usec_t *ret);
పూర్ణాంకం parse_సమయం(స్థిరరాశి అక్షరం *t, usec_t *ret, usec_t default_unit);
పూర్ణాంకం parse_nsec(స్థిరరాశి అక్షరం *t, nsec_t *ret);

పూర్ణాంకం get_timezones(అక్షరం ***ret);
పూర్ణాంకం verify_timezone(స్థిరరాశి అక్షరం *name, పూర్ణాంకం log_level);
స్థిర అంతఃవరుస bool timezone_is_valid(స్థిరరాశి అక్షరం *name, పూర్ణాంకం log_level) {
        తిరిగి verify_timezone(name, log_level) >= 0;
}

శూన్య reset_timezonep(అక్షరం **p);
అక్షరం* save_timezone(శూన్య);
#నిర్వచించు SAVE_TIMEZONE                                                   \
        వాడని _cleanup_(reset_timezonep)                             \
             అక్షరం *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_జరుపు_clock(usec_t x, clockid_t from, clockid_t to);

పూర్ణాంకం get_timezone(అక్షరం **ret);
పూర్ణాంకం get_timezone_prefer_env(అక్షరం **ret);
స్థిరరాశి అక్షరం* etc_localtime(శూన్య);

పూర్ణాంకం mktime_or_timegm_usec(నిర్మాణం tm *tm, bool utc, usec_t *ret);
పూర్ణాంకం localtime_or_gmtime_usec(usec_t t, bool utc, నిర్మాణం tm *ret);

పూర్ణాంకం parse_calendar_date_నిండిన(స్థిరరాశి అక్షరం *s, bool allow_pre_epoch, usec_t *ret_usec, నిర్మాణం tm *ret_tm);

స్థిర అంతఃవరుస పూర్ణాంకం parse_calendar_date(స్థిరరాశి అక్షరం *s, usec_t *ret) {
        తిరిగి parse_calendar_date_నిండిన(s, /* allow_pre_epoch= */ false, ret, ఖాళీ);
}

#నిర్వచించు BIRTH_DATE_UNSET                        \
        (స్థిరరాశి నిర్మాణం tm) {                     \
                .tm_year = INT_MIN,             \
        }

#నిర్వచించు BIRTH_DATE_IS_సమితి(tm) ((tm).tm_year != INT_MIN)

స్థిర అంతఃవరుస పూర్ణాంకం parse_birth_date(స్థిరరాశి అక్షరం *s, నిర్మాణం tm *ret) {
        తిరిగి parse_calendar_date_నిండిన(s, /* allow_pre_epoch= */ true, ఖాళీ, ret);
}

uint64_t sysconf_clock_ticks_cached(శూన్య);

uint32_t usec_గమ్యం_jiffies(usec_t usec);
usec_t jiffies_గమ్యం_usec(uint32_t jiffies);

bool లోపల_utc_timezone(శూన్య);

స్థిర అంతఃవరుస usec_t usec_కూడు(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        తిరిగి saturate_కూడు(a, b, USEC_INFINITY);
}

స్థిర అంతఃవరుస usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        ఒకవేళ (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                తిరిగి USEC_INFINITY;
        ఒకవేళ (timestamp < delta)
                తిరిగి 0;

        తిరిగి timestamp - delta;
}

స్థిర అంతఃవరుస usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        ఒకవేళ (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                తిరిగి usec_కూడు(timestamp, (usec_t) INT64_MAX + 1);
        }
        ఒకవేళ (delta < 0)
                తిరిగి usec_కూడు(timestamp, (usec_t) (-delta));

        తిరిగి usec_sub_unsigned(timestamp, (usec_t) delta);
}

పూర్ణాంకం usleep_సురక్షిత(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#నిర్వచించు USEC_TIMESTAMP_FORMATTABLE_MAX_64_బిట్టు ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#నిర్వచించు USEC_TIMESTAMP_FORMATTABLE_MAX_32_బిట్టు (((usec_t) INT32_MAX) * USEC_చొప్పున_SEC - USEC_చొప్పున_DAY)
#ఒకవేళ SIZEOF_సమయం_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64_బిట్టు
#లేదావేళ SIZEOF_సమయం_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32_బిట్టు
#లేకపోతే
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#అయితే_ముగింపు

పూర్ణాంకం సమయం_change_fd(శూన్య);

DECLARE_STRING_పట్టిక_LOOKUP(timestamp_style, TimestampStyle);
