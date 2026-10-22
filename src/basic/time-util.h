/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRI_NSEC PRIu64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PRI_USEC PRIu64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_FMT "%" PRI_NSEC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_FMT "%" PRI_USEC

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ TimestampStyle {
        TIMESTAMP_PRETTY,
        TIMESTAMP_US,
        TIMESTAMP_UTC,
        TIMESTAMP_US_UTC,
        TIMESTAMP_UNIX,
        TIMESTAMP_DATE,
        _TIMESTAMP_STYLE_MAX,
        _TIMESTAMP_STYLE_INVALID = -EINVAL,
} TimestampStyle;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_INFINITY ((usec_t) UINT64_MAX)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_INFINITY ((nsec_t) UINT64_MAX)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSEC_ਪ੍ਰਤੀ_SEC  1000ULL
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_ਪ੍ਰਤੀ_SEC  ((usec_t) 1000000ULL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_ਪ੍ਰਤੀ_MSEC ((usec_t) 1000ULL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_SEC  ((nsec_t) 1000000000ULL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_MSEC ((nsec_t) 1000000ULL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_USEC ((nsec_t) 1000ULL)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_ਪ੍ਰਤੀ_MINUTE ((usec_t) (60ULL*USEC_ਪ੍ਰਤੀ_SEC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_MINUTE ((nsec_t) (60ULL*NSEC_ਪ੍ਰਤੀ_SEC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_ਪ੍ਰਤੀ_HOUR ((usec_t) (60ULL*USEC_ਪ੍ਰਤੀ_MINUTE))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_HOUR ((nsec_t) (60ULL*NSEC_ਪ੍ਰਤੀ_MINUTE))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_ਪ੍ਰਤੀ_DAY ((usec_t) (24ULL*USEC_ਪ੍ਰਤੀ_HOUR))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_DAY ((nsec_t) (24ULL*NSEC_ਪ੍ਰਤੀ_HOUR))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_ਪ੍ਰਤੀ_WEEK ((usec_t) (7ULL*USEC_ਪ੍ਰਤੀ_DAY))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_WEEK ((nsec_t) (7ULL*NSEC_ਪ੍ਰਤੀ_DAY))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_ਪ੍ਰਤੀ_MONTH ((usec_t) (2629800ULL*USEC_ਪ੍ਰਤੀ_SEC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_MONTH ((nsec_t) (2629800ULL*NSEC_ਪ੍ਰਤੀ_SEC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_ਪ੍ਰਤੀ_YEAR ((usec_t) (31557600ULL*USEC_ਪ੍ਰਤੀ_SEC))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NSEC_ਪ੍ਰਤੀ_YEAR ((nsec_t) (31557600ULL*NSEC_ਪ੍ਰਤੀ_SEC))

ਗਿਣਤੀ {
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_TIMESTAMP_MAX (3U+1U+10U+1U+8U+1U+6U+1U+6U+1U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_TIMESTAMP_RELATIVE_MAX 256U
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_TIMESPAN_MAX 64U

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮਾਂ_T_MAX (time_t)((UINTMAX_C(1) << ((ਆਕਾਰ(time_t) << 3) - 1)) - 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMESPEC_OMIT ((ਅਚਲ ਬਣਤਰ timespec) { .tv_nsec = UTIME_OMIT })

usec_t now(clockid_t clock);
nsec_t now_nsec(clockid_t clock);

usec_t map_clock_usec_raw(usec_t from, usec_t from_ਅਧਾਰ, usec_t ਮੰਜ਼ਿਲ_ਅਧਾਰ);
usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t ਮੰਜ਼ਿਲ_clock);

dual_timestamp* dual_timestamp_now(dual_timestamp *ts);
dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u);

triple_timestamp* triple_timestamp_now(triple_timestamp *ts);
triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u);
triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUAL_TIMESTAMP_HAS_CLOCK(clock)                               \
        ਵਿੱਚ_ਸੈੱਟ(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRIPLE_TIMESTAMP_HAS_CLOCK(clock)                               \
        ਵਿੱਚ_ਸੈੱਟ(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC, CLOCK_BOOTTIME, CLOCK_BOOTTIME_ALARM)

ਸਥਿਰ ਸਤਰਅੰਦਰ bool timestamp_is_ਸੈੱਟ(usec_t timestamp) {
        ਵਾਪਸ timestamp > 0 && timestamp != USEC_INFINITY;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool dual_timestamp_is_ਸੈੱਟ(ਅਚਲ dual_timestamp *ts) {
        ਵਾਪਸ timestamp_is_ਸੈੱਟ(ts->realtime) ||
               timestamp_is_ਸੈੱਟ(ts->monotonic);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool triple_timestamp_is_ਸੈੱਟ(ਅਚਲ triple_timestamp *ts) {
        ਵਾਪਸ timestamp_is_ਸੈੱਟ(ts->realtime) ||
               timestamp_is_ਸੈੱਟ(ts->monotonic) ||
               timestamp_is_ਸੈੱਟ(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_ਲੱਦੋ(ਅਚਲ ਬਣਤਰ timespec *ts) ਸ਼ੁੱਧ;
nsec_t timespec_ਲੱਦੋ_nsec(ਅਚਲ ਬਣਤਰ timespec *ts) ਸ਼ੁੱਧ;
ਬਣਤਰ timespec* timespec_store(ਬਣਤਰ timespec *ts, usec_t u);
ਬਣਤਰ timespec* timespec_store_nsec(ਬਣਤਰ timespec *ts, nsec_t n);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMESPEC_STORE(u) timespec_store(&(ਬਣਤਰ timespec) {}, (u))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(ਬਣਤਰ timespec) {}, (n))

ਬਣਤਰ __kernel_timespec* kernel_timespec_store(ਬਣਤਰ __kernel_timespec *ts, usec_t u);

usec_t timeval_ਲੱਦੋ(ਅਚਲ ਬਣਤਰ timeval *tv) ਸ਼ੁੱਧ;
ਬਣਤਰ timeval* timeval_store(ਬਣਤਰ timeval *tv, usec_t u);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMEVAL_STORE(u) timeval_store(&(ਬਣਤਰ timeval) {}, (u))

ਅੱਖਰ* format_timestamp_style(ਅੱਖਰ *buf, size_t l, usec_t t, TimestampStyle style) warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
ਅੱਖਰ* format_timestamp_relative_ਭਰਿਆ(ਅੱਖਰ *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ਖੱਬਾ) warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
ਅੱਖਰ* format_timespan(ਅੱਖਰ *buf, size_t l, usec_t t, usec_t accuracy) warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
ਅਚਲ ਅੱਖਰ* weekday_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ i);

warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* format_timestamp_relative(ਅੱਖਰ *buf, size_t l, usec_t t) {
        ਵਾਪਸ format_timestamp_relative_ਭਰਿਆ(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* format_timestamp_relative_monotonic(ਅੱਖਰ *buf, size_t l, usec_t t) {
        ਵਾਪਸ format_timestamp_relative_ਭਰਿਆ(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

warn_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* format_timestamp(ਅੱਖਰ *buf, size_t l, usec_t t) {
        ਵਾਪਸ format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_TIMESTAMP(t) format_timestamp((ਅੱਖਰ[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((ਅੱਖਰ[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((ਅੱਖਰ[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_TIMESPAN(t, accuracy) format_timespan((ਅੱਖਰ[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((ਅੱਖਰ[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

ਅਚਲ ਅੱਖਰ* get_tzname(bool dst);
ਪੂਰਨ parse_gmtoff(ਅਚਲ ਅੱਖਰ *t, ਲੰਮਾ *ret);
ਪੂਰਨ parse_timestamp(ਅਚਲ ਅੱਖਰ *t, usec_t *ret);

ਪੂਰਨ parse_sec(ਅਚਲ ਅੱਖਰ *t, usec_t *ret);
ਪੂਰਨ parse_sec_fix_0(ਅਚਲ ਅੱਖਰ *t, usec_t *ret);
ਪੂਰਨ parse_sec_def_infinity(ਅਚਲ ਅੱਖਰ *t, usec_t *ret);
ਪੂਰਨ parse_ਸਮਾਂ(ਅਚਲ ਅੱਖਰ *t, usec_t *ret, usec_t default_unit);
ਪੂਰਨ parse_nsec(ਅਚਲ ਅੱਖਰ *t, nsec_t *ret);

ਪੂਰਨ get_timezones(ਅੱਖਰ ***ret);
ਪੂਰਨ verify_timezone(ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ log_level);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool timezone_is_valid(ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ log_level) {
        ਵਾਪਸ verify_timezone(name, log_level) >= 0;
}

ਖਾਲੀ reset_timezonep(ਅੱਖਰ **p);
ਅੱਖਰ* save_timezone(ਖਾਲੀ);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAVE_TIMEZONE                                                   \
        ਅਣਵਰਤਿਆ _cleanup_(reset_timezonep)                             \
             ਅੱਖਰ *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_ਖਿਸਕਾਓ_clock(usec_t x, clockid_t from, clockid_t to);

ਪੂਰਨ get_timezone(ਅੱਖਰ **ret);
ਪੂਰਨ get_timezone_prefer_env(ਅੱਖਰ **ret);
ਅਚਲ ਅੱਖਰ* etc_localtime(ਖਾਲੀ);

ਪੂਰਨ mktime_or_timegm_usec(ਬਣਤਰ tm *tm, bool utc, usec_t *ret);
ਪੂਰਨ localtime_or_gmtime_usec(usec_t t, bool utc, ਬਣਤਰ tm *ret);

ਪੂਰਨ parse_calendar_date_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, bool allow_pre_epoch, usec_t *ret_usec, ਬਣਤਰ tm *ret_tm);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ parse_calendar_date(ਅਚਲ ਅੱਖਰ *s, usec_t *ret) {
        ਵਾਪਸ parse_calendar_date_ਭਰਿਆ(s, /* allow_pre_epoch= */ false, ret, ਨਿਰਾ);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIRTH_DATE_UNSET                        \
        (ਅਚਲ ਬਣਤਰ tm) {                     \
                .tm_year = INT_MIN,             \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BIRTH_DATE_IS_ਸੈੱਟ(tm) ((tm).tm_year != INT_MIN)

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ parse_birth_date(ਅਚਲ ਅੱਖਰ *s, ਬਣਤਰ tm *ret) {
        ਵਾਪਸ parse_calendar_date_ਭਰਿਆ(s, /* allow_pre_epoch= */ true, ਨਿਰਾ, ret);
}

uint64_t sysconf_clock_ticks_cached(ਖਾਲੀ);

uint32_t usec_ਮੰਜ਼ਿਲ_jiffies(usec_t usec);
usec_t jiffies_ਮੰਜ਼ਿਲ_usec(uint32_t jiffies);

bool ਵਿੱਚ_utc_timezone(ਖਾਲੀ);

ਸਥਿਰ ਸਤਰਅੰਦਰ usec_t usec_ਜੋੜੋ(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        ਵਾਪਸ saturate_ਜੋੜੋ(a, b, USEC_INFINITY);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        ਜੇ (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                ਵਾਪਸ USEC_INFINITY;
        ਜੇ (timestamp < delta)
                ਵਾਪਸ 0;

        ਵਾਪਸ timestamp - delta;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        ਜੇ (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                ਵਾਪਸ usec_ਜੋੜੋ(timestamp, (usec_t) INT64_MAX + 1);
        }
        ਜੇ (delta < 0)
                ਵਾਪਸ usec_ਜੋੜੋ(timestamp, (usec_t) (-delta));

        ਵਾਪਸ usec_sub_unsigned(timestamp, (usec_t) delta);
}

ਪੂਰਨ usleep_ਸੁਰੱਖਿਅਤ(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_TIMESTAMP_FORMATTABLE_MAX_64_ਦੋਆਧਾਰੀ_ਅੰਕ ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USEC_TIMESTAMP_FORMATTABLE_MAX_32_ਦੋਆਧਾਰੀ_ਅੰਕ (((usec_t) INT32_MAX) * USEC_ਪ੍ਰਤੀ_SEC - USEC_ਪ੍ਰਤੀ_DAY)
#ਜੇ SIZEOF_ਸਮਾਂ_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64_ਦੋਆਧਾਰੀ_ਅੰਕ
#ਜਾਂਜੇ SIZEOF_ਸਮਾਂ_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32_ਦੋਆਧਾਰੀ_ਅੰਕ
#ਨਹੀਂਤਾਂ
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#ਜੇ_ਅੰਤ

ਪੂਰਨ ਸਮਾਂ_change_fd(ਖਾਲੀ);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(timestamp_style, TimestampStyle);
