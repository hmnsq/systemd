/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <limits.h>
#ಅನ್ತರ್ಭಾವ <time.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು PRI_NSEC PRIu64
#ವ್ಯಾಖ್ಯಾನಿಸು PRI_USEC PRIu64
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_FMT "%" PRI_NSEC
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_FMT "%" PRI_USEC

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ TimestampStyle {
        TIMESTAMP_PRETTY,
        TIMESTAMP_US,
        TIMESTAMP_UTC,
        TIMESTAMP_US_UTC,
        TIMESTAMP_UNIX,
        TIMESTAMP_DATE,
        _TIMESTAMP_STYLE_MAX,
        _TIMESTAMP_STYLE_INVALID = -EINVAL,
} TimestampStyle;

#ವ್ಯಾಖ್ಯಾನಿಸು USEC_INFINITY ((usec_t) UINT64_MAX)
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_INFINITY ((nsec_t) UINT64_MAX)

#ವ್ಯಾಖ್ಯಾನಿಸು MSEC_ಪ್ರತಿ_SEC  1000ULL
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_ಪ್ರತಿ_SEC  ((usec_t) 1000000ULL)
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_ಪ್ರತಿ_MSEC ((usec_t) 1000ULL)
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_SEC  ((nsec_t) 1000000000ULL)
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_MSEC ((nsec_t) 1000000ULL)
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_USEC ((nsec_t) 1000ULL)

#ವ್ಯಾಖ್ಯಾನಿಸು USEC_ಪ್ರತಿ_MINUTE ((usec_t) (60ULL*USEC_ಪ್ರತಿ_SEC))
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_MINUTE ((nsec_t) (60ULL*NSEC_ಪ್ರತಿ_SEC))
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_ಪ್ರತಿ_HOUR ((usec_t) (60ULL*USEC_ಪ್ರತಿ_MINUTE))
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_HOUR ((nsec_t) (60ULL*NSEC_ಪ್ರತಿ_MINUTE))
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_ಪ್ರತಿ_DAY ((usec_t) (24ULL*USEC_ಪ್ರತಿ_HOUR))
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_DAY ((nsec_t) (24ULL*NSEC_ಪ್ರತಿ_HOUR))
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_ಪ್ರತಿ_WEEK ((usec_t) (7ULL*USEC_ಪ್ರತಿ_DAY))
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_WEEK ((nsec_t) (7ULL*NSEC_ಪ್ರತಿ_DAY))
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_ಪ್ರತಿ_MONTH ((usec_t) (2629800ULL*USEC_ಪ್ರತಿ_SEC))
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_MONTH ((nsec_t) (2629800ULL*NSEC_ಪ್ರತಿ_SEC))
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_ಪ್ರತಿ_YEAR ((usec_t) (31557600ULL*USEC_ಪ್ರತಿ_SEC))
#ವ್ಯಾಖ್ಯಾನಿಸು NSEC_ಪ್ರತಿ_YEAR ((nsec_t) (31557600ULL*NSEC_ಪ್ರತಿ_SEC))

ಎಣಿಕೆ {
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
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_TIMESTAMP_MAX (3U+1U+10U+1U+8U+1U+6U+1U+6U+1U)
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_TIMESTAMP_RELATIVE_MAX 256U
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_TIMESPAN_MAX 64U

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಮಯ_T_MAX (time_t)((UINTMAX_C(1) << ((ಗಾತ್ರ(time_t) << 3) - 1)) - 1)

#ವ್ಯಾಖ್ಯಾನಿಸು DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#ವ್ಯಾಖ್ಯಾನಿಸು DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#ವ್ಯಾಖ್ಯಾನಿಸು TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#ವ್ಯಾಖ್ಯಾನಿಸು TIMESPEC_OMIT ((ಸ್ಥಿರಾಂಕ ರಚನೆ timespec) { .tv_nsec = UTIME_OMIT })

usec_t now(clockid_t clock);
nsec_t now_nsec(clockid_t clock);

usec_t map_clock_usec_raw(usec_t from, usec_t from_ತಳ, usec_t ಗಮ್ಯ_ತಳ);
usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t ಗಮ್ಯ_clock);

dual_timestamp* dual_timestamp_now(dual_timestamp *ts);
dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u);

triple_timestamp* triple_timestamp_now(triple_timestamp *ts);
triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u);
triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u);

#ವ್ಯಾಖ್ಯಾನಿಸು DUAL_TIMESTAMP_HAS_CLOCK(clock)                               \
        ಒಳ_ಗಣ(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC)

#ವ್ಯಾಖ್ಯಾನಿಸು TRIPLE_TIMESTAMP_HAS_CLOCK(clock)                               \
        ಒಳ_ಗಣ(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC, CLOCK_BOOTTIME, CLOCK_BOOTTIME_ALARM)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool timestamp_is_ಗಣ(usec_t timestamp) {
        ಹಿಂತಿರುಗಿ timestamp > 0 && timestamp != USEC_INFINITY;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool dual_timestamp_is_ಗಣ(ಸ್ಥಿರಾಂಕ dual_timestamp *ts) {
        ಹಿಂತಿರುಗಿ timestamp_is_ಗಣ(ts->realtime) ||
               timestamp_is_ಗಣ(ts->monotonic);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool triple_timestamp_is_ಗಣ(ಸ್ಥಿರಾಂಕ triple_timestamp *ts) {
        ಹಿಂತಿರುಗಿ timestamp_is_ಗಣ(ts->realtime) ||
               timestamp_is_ಗಣ(ts->monotonic) ||
               timestamp_is_ಗಣ(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_ತುಂಬು(ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts) ಶುದ್ಧ;
nsec_t timespec_ತುಂಬು_nsec(ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts) ಶುದ್ಧ;
ರಚನೆ timespec* timespec_store(ರಚನೆ timespec *ts, usec_t u);
ರಚನೆ timespec* timespec_store_nsec(ರಚನೆ timespec *ts, nsec_t n);

#ವ್ಯಾಖ್ಯಾನಿಸು TIMESPEC_STORE(u) timespec_store(&(ರಚನೆ timespec) {}, (u))
#ವ್ಯಾಖ್ಯಾನಿಸು TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(ರಚನೆ timespec) {}, (n))

ರಚನೆ __kernel_timespec* kernel_timespec_store(ರಚನೆ __kernel_timespec *ts, usec_t u);

usec_t timeval_ತುಂಬು(ಸ್ಥಿರಾಂಕ ರಚನೆ timeval *tv) ಶುದ್ಧ;
ರಚನೆ timeval* timeval_store(ರಚನೆ timeval *tv, usec_t u);

#ವ್ಯಾಖ್ಯಾನಿಸು TIMEVAL_STORE(u) timeval_store(&(ರಚನೆ timeval) {}, (u))

ಅಕ್ಷರ* format_timestamp_style(ಅಕ್ಷರ *buf, size_t l, usec_t t, TimestampStyle style) warn_ಬಳಸದ_ಫಲಿತಾಂಶ;
ಅಕ್ಷರ* format_timestamp_relative_ತುಂಬಿದ(ಅಕ್ಷರ *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ಎಡ) warn_ಬಳಸದ_ಫಲಿತಾಂಶ;
ಅಕ್ಷರ* format_timespan(ಅಕ್ಷರ *buf, size_t l, usec_t t, usec_t accuracy) warn_ಬಳಸದ_ಫಲಿತಾಂಶ;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* weekday_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ i);

warn_ಬಳಸದ_ಫಲಿತಾಂಶ
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* format_timestamp_relative(ಅಕ್ಷರ *buf, size_t l, usec_t t) {
        ಹಿಂತಿರುಗಿ format_timestamp_relative_ತುಂಬಿದ(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
warn_ಬಳಸದ_ಫಲಿತಾಂಶ
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* format_timestamp_relative_monotonic(ಅಕ್ಷರ *buf, size_t l, usec_t t) {
        ಹಿಂತಿರುಗಿ format_timestamp_relative_ತುಂಬಿದ(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

warn_ಬಳಸದ_ಫಲಿತಾಂಶ
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* format_timestamp(ಅಕ್ಷರ *buf, size_t l, usec_t t) {
        ಹಿಂತಿರುಗಿ format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_TIMESTAMP(t) format_timestamp((ಅಕ್ಷರ[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((ಅಕ್ಷರ[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((ಅಕ್ಷರ[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_TIMESPAN(t, accuracy) format_timespan((ಅಕ್ಷರ[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#ವ್ಯಾಖ್ಯಾನಿಸು FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((ಅಕ್ಷರ[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* get_tzname(bool dst);
ಪೂರ್ಣಾಂಕ parse_gmtoff(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, ದೀರ್ಘ *ret);
ಪೂರ್ಣಾಂಕ parse_timestamp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret);

ಪೂರ್ಣಾಂಕ parse_sec(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret);
ಪೂರ್ಣಾಂಕ parse_sec_fix_0(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret);
ಪೂರ್ಣಾಂಕ parse_sec_def_infinity(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret);
ಪೂರ್ಣಾಂಕ parse_ಸಮಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret, usec_t default_unit);
ಪೂರ್ಣಾಂಕ parse_nsec(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, nsec_t *ret);

ಪೂರ್ಣಾಂಕ get_timezones(ಅಕ್ಷರ ***ret);
ಪೂರ್ಣಾಂಕ verify_timezone(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ log_level);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool timezone_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ log_level) {
        ಹಿಂತಿರುಗಿ verify_timezone(name, log_level) >= 0;
}

ಶೂನ್ಯ reset_timezonep(ಅಕ್ಷರ **p);
ಅಕ್ಷರ* save_timezone(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿಸು SAVE_TIMEZONE                                                   \
        ಬಳಸದ _cleanup_(reset_timezonep)                             \
             ಅಕ್ಷರ *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_ಸರಿಸು_clock(usec_t x, clockid_t from, clockid_t to);

ಪೂರ್ಣಾಂಕ get_timezone(ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ get_timezone_prefer_env(ಅಕ್ಷರ **ret);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* etc_localtime(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ mktime_or_timegm_usec(ರಚನೆ tm *tm, bool utc, usec_t *ret);
ಪೂರ್ಣಾಂಕ localtime_or_gmtime_usec(usec_t t, bool utc, ರಚನೆ tm *ret);

ಪೂರ್ಣಾಂಕ parse_calendar_date_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, bool allow_pre_epoch, usec_t *ret_usec, ರಚನೆ tm *ret_tm);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ parse_calendar_date(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, usec_t *ret) {
        ಹಿಂತಿರುಗಿ parse_calendar_date_ತುಂಬಿದ(s, /* allow_pre_epoch= */ false, ret, ಖಾಲಿ);
}

#ವ್ಯಾಖ್ಯಾನಿಸು BIRTH_DATE_UNSET                        \
        (ಸ್ಥಿರಾಂಕ ರಚನೆ tm) {                     \
                .tm_year = INT_MIN,             \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು BIRTH_DATE_IS_ಗಣ(tm) ((tm).tm_year != INT_MIN)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ parse_birth_date(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ರಚನೆ tm *ret) {
        ಹಿಂತಿರುಗಿ parse_calendar_date_ತುಂಬಿದ(s, /* allow_pre_epoch= */ true, ಖಾಲಿ, ret);
}

uint64_t sysconf_clock_ticks_cached(ಶೂನ್ಯ);

uint32_t usec_ಗಮ್ಯ_jiffies(usec_t usec);
usec_t jiffies_ಗಮ್ಯ_usec(uint32_t jiffies);

bool ಒಳ_utc_timezone(ಶೂನ್ಯ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ usec_t usec_ಕೂಡಿಸು(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        ಹಿಂತಿರುಗಿ saturate_ಕೂಡಿಸು(a, b, USEC_INFINITY);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        ಆದರೆ (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                ಹಿಂತಿರುಗಿ USEC_INFINITY;
        ಆದರೆ (timestamp < delta)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ timestamp - delta;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        ಆದರೆ (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                ಹಿಂತಿರುಗಿ usec_ಕೂಡಿಸು(timestamp, (usec_t) INT64_MAX + 1);
        }
        ಆದರೆ (delta < 0)
                ಹಿಂತಿರುಗಿ usec_ಕೂಡಿಸು(timestamp, (usec_t) (-delta));

        ಹಿಂತಿರುಗಿ usec_sub_unsigned(timestamp, (usec_t) delta);
}

ಪೂರ್ಣಾಂಕ usleep_ಸುರಕ್ಷಿತ(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_TIMESTAMP_FORMATTABLE_MAX_64_ಬಿಟ್ ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#ವ್ಯಾಖ್ಯಾನಿಸು USEC_TIMESTAMP_FORMATTABLE_MAX_32_ಬಿಟ್ (((usec_t) INT32_MAX) * USEC_ಪ್ರತಿ_SEC - USEC_ಪ್ರತಿ_DAY)
#ಆದರೆ SIZEOF_ಸಮಯ_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64_ಬಿಟ್
#ಅಥವಾಆದರೆ SIZEOF_ಸಮಯ_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32_ಬಿಟ್
#ಇಲ್ಲದಿದ್ದರೆ
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ ಸಮಯ_change_fd(ಶೂನ್ಯ);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(timestamp_style, TimestampStyle);
