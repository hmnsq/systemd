/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <limits.h>
#شامل <time.h>

#شامل "forward.h"

#تعریف_کرو PRI_NSEC PRIu64
#تعریف_کرو PRI_USEC PRIu64
#تعریف_کرو NSEC_FMT "%" PRI_NSEC
#تعریف_کرو USEC_FMT "%" PRI_USEC

قسمتعریف ساخت dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

قسمتعریف ساخت triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

قسمتعریف شمار TimestampStyle {
        TIMESTAMP_PRETTY,
        TIMESTAMP_US,
        TIMESTAMP_UTC,
        TIMESTAMP_US_UTC,
        TIMESTAMP_UNIX,
        TIMESTAMP_DATE,
        _TIMESTAMP_STYLE_MAX,
        _TIMESTAMP_STYLE_INVALID = -EINVAL,
} TimestampStyle;

#تعریف_کرو USEC_INFINITY ((usec_t) UINT64_MAX)
#تعریف_کرو NSEC_INFINITY ((nsec_t) UINT64_MAX)

#تعریف_کرو MSEC_فی_SEC  1000ULL
#تعریف_کرو USEC_فی_SEC  ((usec_t) 1000000ULL)
#تعریف_کرو USEC_فی_MSEC ((usec_t) 1000ULL)
#تعریف_کرو NSEC_فی_SEC  ((nsec_t) 1000000000ULL)
#تعریف_کرو NSEC_فی_MSEC ((nsec_t) 1000000ULL)
#تعریف_کرو NSEC_فی_USEC ((nsec_t) 1000ULL)

#تعریف_کرو USEC_فی_MINUTE ((usec_t) (60ULL*USEC_فی_SEC))
#تعریف_کرو NSEC_فی_MINUTE ((nsec_t) (60ULL*NSEC_فی_SEC))
#تعریف_کرو USEC_فی_HOUR ((usec_t) (60ULL*USEC_فی_MINUTE))
#تعریف_کرو NSEC_فی_HOUR ((nsec_t) (60ULL*NSEC_فی_MINUTE))
#تعریف_کرو USEC_فی_DAY ((usec_t) (24ULL*USEC_فی_HOUR))
#تعریف_کرو NSEC_فی_DAY ((nsec_t) (24ULL*NSEC_فی_HOUR))
#تعریف_کرو USEC_فی_WEEK ((usec_t) (7ULL*USEC_فی_DAY))
#تعریف_کرو NSEC_فی_WEEK ((nsec_t) (7ULL*NSEC_فی_DAY))
#تعریف_کرو USEC_فی_MONTH ((usec_t) (2629800ULL*USEC_فی_SEC))
#تعریف_کرو NSEC_فی_MONTH ((nsec_t) (2629800ULL*NSEC_فی_SEC))
#تعریف_کرو USEC_فی_YEAR ((usec_t) (31557600ULL*USEC_فی_SEC))
#تعریف_کرو NSEC_فی_YEAR ((nsec_t) (31557600ULL*NSEC_فی_SEC))

شمار {
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
#تعریف_کرو FORMAT_TIMESTAMP_MAX (3U+1U+10U+1U+8U+1U+6U+1U+6U+1U)
#تعریف_کرو FORMAT_TIMESTAMP_RELATIVE_MAX 256U
#تعریف_کرو FORMAT_TIMESPAN_MAX 64U

#تعریف_کرو وقت_T_MAX (time_t)((UINTMAX_C(1) << ((حجم(time_t) << 3) - 1)) - 1)

#تعریف_کرو DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#تعریف_کرو DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#تعریف_کرو TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#تعریف_کرو TIMESPEC_OMIT ((ثابت ساخت timespec) { .tv_nsec = UTIME_OMIT })

usec_t now(clockid_t clock);
nsec_t now_nsec(clockid_t clock);

usec_t map_clock_usec_raw(usec_t from, usec_t from_بنیاد, usec_t مقصد_بنیاد);
usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t مقصد_clock);

dual_timestamp* dual_timestamp_now(dual_timestamp *ts);
dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u);

triple_timestamp* triple_timestamp_now(triple_timestamp *ts);
triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u);
triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u);

#تعریف_کرو DUAL_TIMESTAMP_HAS_CLOCK(clock)                               \
        اندر_سیٹ(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC)

#تعریف_کرو TRIPLE_TIMESTAMP_HAS_CLOCK(clock)                               \
        اندر_سیٹ(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC, CLOCK_BOOTTIME, CLOCK_BOOTTIME_ALARM)

ساکن اندرونی bool timestamp_is_سیٹ(usec_t timestamp) {
        واپس timestamp > 0 && timestamp != USEC_INFINITY;
}

ساکن اندرونی bool dual_timestamp_is_سیٹ(ثابت dual_timestamp *ts) {
        واپس timestamp_is_سیٹ(ts->realtime) ||
               timestamp_is_سیٹ(ts->monotonic);
}

ساکن اندرونی bool triple_timestamp_is_سیٹ(ثابت triple_timestamp *ts) {
        واپس timestamp_is_سیٹ(ts->realtime) ||
               timestamp_is_سیٹ(ts->monotonic) ||
               timestamp_is_سیٹ(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_بھرو(ثابت ساخت timespec *ts) خالص;
nsec_t timespec_بھرو_nsec(ثابت ساخت timespec *ts) خالص;
ساخت timespec* timespec_store(ساخت timespec *ts, usec_t u);
ساخت timespec* timespec_store_nsec(ساخت timespec *ts, nsec_t n);

#تعریف_کرو TIMESPEC_STORE(u) timespec_store(&(ساخت timespec) {}, (u))
#تعریف_کرو TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(ساخت timespec) {}, (n))

ساخت __kernel_timespec* kernel_timespec_store(ساخت __kernel_timespec *ts, usec_t u);

usec_t timeval_بھرو(ثابت ساخت timeval *tv) خالص;
ساخت timeval* timeval_store(ساخت timeval *tv, usec_t u);

#تعریف_کرو TIMEVAL_STORE(u) timeval_store(&(ساخت timeval) {}, (u))

حرف* format_timestamp_style(حرف *buf, size_t l, usec_t t, TimestampStyle style) warn_غیر_مستعمل_نتیجہ;
حرف* format_timestamp_relative_بھرا(حرف *buf, size_t l, usec_t t, clockid_t clock, bool implicit_بایاں) warn_غیر_مستعمل_نتیجہ;
حرف* format_timespan(حرف *buf, size_t l, usec_t t, usec_t accuracy) warn_غیر_مستعمل_نتیجہ;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
ثابت حرف* weekday_مقصد_string(صحیح i);

warn_غیر_مستعمل_نتیجہ
ساکن اندرونی حرف* format_timestamp_relative(حرف *buf, size_t l, usec_t t) {
        واپس format_timestamp_relative_بھرا(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
warn_غیر_مستعمل_نتیجہ
ساکن اندرونی حرف* format_timestamp_relative_monotonic(حرف *buf, size_t l, usec_t t) {
        واپس format_timestamp_relative_بھرا(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

warn_غیر_مستعمل_نتیجہ
ساکن اندرونی حرف* format_timestamp(حرف *buf, size_t l, usec_t t) {
        واپس format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کرو FORMAT_TIMESTAMP(t) format_timestamp((حرف[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#تعریف_کرو FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((حرف[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#تعریف_کرو FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((حرف[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#تعریف_کرو FORMAT_TIMESPAN(t, accuracy) format_timespan((حرف[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#تعریف_کرو FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((حرف[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

ثابت حرف* get_tzname(bool dst);
صحیح parse_gmtoff(ثابت حرف *t, طویل *ret);
صحیح parse_timestamp(ثابت حرف *t, usec_t *ret);

صحیح parse_sec(ثابت حرف *t, usec_t *ret);
صحیح parse_sec_fix_0(ثابت حرف *t, usec_t *ret);
صحیح parse_sec_def_infinity(ثابت حرف *t, usec_t *ret);
صحیح parse_وقت(ثابت حرف *t, usec_t *ret, usec_t default_unit);
صحیح parse_nsec(ثابت حرف *t, nsec_t *ret);

صحیح get_timezones(حرف ***ret);
صحیح verify_timezone(ثابت حرف *name, صحیح log_level);
ساکن اندرونی bool timezone_is_valid(ثابت حرف *name, صحیح log_level) {
        واپس verify_timezone(name, log_level) >= 0;
}

خالی reset_timezonep(حرف **p);
حرف* save_timezone(خالی);
#تعریف_کرو SAVE_TIMEZONE                                                   \
        غیر_مستعمل _cleanup_(reset_timezonep)                             \
             حرف *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_سرکاؤ_clock(usec_t x, clockid_t from, clockid_t to);

صحیح get_timezone(حرف **ret);
صحیح get_timezone_prefer_env(حرف **ret);
ثابت حرف* etc_localtime(خالی);

صحیح mktime_or_timegm_usec(ساخت tm *tm, bool utc, usec_t *ret);
صحیح localtime_or_gmtime_usec(usec_t t, bool utc, ساخت tm *ret);

صحیح parse_calendar_date_بھرا(ثابت حرف *s, bool allow_pre_epoch, usec_t *ret_usec, ساخت tm *ret_tm);

ساکن اندرونی صحیح parse_calendar_date(ثابت حرف *s, usec_t *ret) {
        واپس parse_calendar_date_بھرا(s, /* allow_pre_epoch= */ false, ret, تہی);
}

#تعریف_کرو BIRTH_DATE_UNSET                        \
        (ثابت ساخت tm) {                     \
                .tm_year = INT_MIN,             \
        }

#تعریف_کرو BIRTH_DATE_IS_سیٹ(tm) ((tm).tm_year != INT_MIN)

ساکن اندرونی صحیح parse_birth_date(ثابت حرف *s, ساخت tm *ret) {
        واپس parse_calendar_date_بھرا(s, /* allow_pre_epoch= */ true, تہی, ret);
}

uint64_t sysconf_clock_ticks_cached(خالی);

uint32_t usec_مقصد_jiffies(usec_t usec);
usec_t jiffies_مقصد_usec(uint32_t jiffies);

bool اندر_utc_timezone(خالی);

ساکن اندرونی usec_t usec_جوڑو(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        واپس saturate_جوڑو(a, b, USEC_INFINITY);
}

ساکن اندرونی usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        اگر (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                واپس USEC_INFINITY;
        اگر (timestamp < delta)
                واپس 0;

        واپس timestamp - delta;
}

ساکن اندرونی usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        اگر (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                واپس usec_جوڑو(timestamp, (usec_t) INT64_MAX + 1);
        }
        اگر (delta < 0)
                واپس usec_جوڑو(timestamp, (usec_t) (-delta));

        واپس usec_sub_unsigned(timestamp, (usec_t) delta);
}

صحیح usleep_محفوظ(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#تعریف_کرو USEC_TIMESTAMP_FORMATTABLE_MAX_64_ثنائی_ہندسہ ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#تعریف_کرو USEC_TIMESTAMP_FORMATTABLE_MAX_32_ثنائی_ہندسہ (((usec_t) INT32_MAX) * USEC_فی_SEC - USEC_فی_DAY)
#اگر SIZEOF_وقت_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64_ثنائی_ہندسہ
#وگرنہ_اگر SIZEOF_وقت_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32_ثنائی_ہندسہ
#ورنہ
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#اگر_ختم

صحیح وقت_change_fd(خالی);

DECLARE_STRING_جدول_LOOKUP(timestamp_style, TimestampStyle);
