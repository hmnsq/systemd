/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <limits.h>
#ضمن <time.h>

#ضمن "forward.h"

#تعریف_کړه PRI_NSEC PRIu64
#تعریف_کړه PRI_USEC PRIu64
#تعریف_کړه NSEC_FMT "%" PRI_NSEC
#تعریف_کړه USEC_FMT "%" PRI_USEC

تعريفنوع بنية dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

تعريفنوع بنية triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

تعريفنوع تعداد TimestampStyle {
        TIMESTAMP_PRETTY,
        TIMESTAMP_US,
        TIMESTAMP_UTC,
        TIMESTAMP_US_UTC,
        TIMESTAMP_UNIX,
        TIMESTAMP_DATE,
        _TIMESTAMP_STYLE_MAX,
        _TIMESTAMP_STYLE_INVALID = -EINVAL,
} TimestampStyle;

#تعریف_کړه USEC_INFINITY ((usec_t) UINT64_MAX)
#تعریف_کړه NSEC_INFINITY ((nsec_t) UINT64_MAX)

#تعریف_کړه MSEC_فی_SEC  1000ULL
#تعریف_کړه USEC_فی_SEC  ((usec_t) 1000000ULL)
#تعریف_کړه USEC_فی_MSEC ((usec_t) 1000ULL)
#تعریف_کړه NSEC_فی_SEC  ((nsec_t) 1000000000ULL)
#تعریف_کړه NSEC_فی_MSEC ((nsec_t) 1000000ULL)
#تعریف_کړه NSEC_فی_USEC ((nsec_t) 1000ULL)

#تعریف_کړه USEC_فی_MINUTE ((usec_t) (60ULL*USEC_فی_SEC))
#تعریف_کړه NSEC_فی_MINUTE ((nsec_t) (60ULL*NSEC_فی_SEC))
#تعریف_کړه USEC_فی_HOUR ((usec_t) (60ULL*USEC_فی_MINUTE))
#تعریف_کړه NSEC_فی_HOUR ((nsec_t) (60ULL*NSEC_فی_MINUTE))
#تعریف_کړه USEC_فی_DAY ((usec_t) (24ULL*USEC_فی_HOUR))
#تعریف_کړه NSEC_فی_DAY ((nsec_t) (24ULL*NSEC_فی_HOUR))
#تعریف_کړه USEC_فی_WEEK ((usec_t) (7ULL*USEC_فی_DAY))
#تعریف_کړه NSEC_فی_WEEK ((nsec_t) (7ULL*NSEC_فی_DAY))
#تعریف_کړه USEC_فی_MONTH ((usec_t) (2629800ULL*USEC_فی_SEC))
#تعریف_کړه NSEC_فی_MONTH ((nsec_t) (2629800ULL*NSEC_فی_SEC))
#تعریف_کړه USEC_فی_YEAR ((usec_t) (31557600ULL*USEC_فی_SEC))
#تعریف_کړه NSEC_فی_YEAR ((nsec_t) (31557600ULL*NSEC_فی_SEC))

تعداد {
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
#تعریف_کړه FORMAT_TIMESTAMP_MAX (3U+1U+10U+1U+8U+1U+6U+1U+6U+1U)
#تعریف_کړه FORMAT_TIMESTAMP_RELATIVE_MAX 256U
#تعریف_کړه FORMAT_TIMESPAN_MAX 64U

#تعریف_کړه وخت_T_MAX (time_t)((UINTMAX_C(1) << ((حجم(time_t) << 3) - 1)) - 1)

#تعریف_کړه DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#تعریف_کړه DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#تعریف_کړه TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#تعریف_کړه TIMESPEC_OMIT ((ثابت بنية timespec) { .tv_nsec = UTIME_OMIT })

usec_t now(clockid_t clock);
nsec_t now_nsec(clockid_t clock);

usec_t map_clock_usec_raw(usec_t from, usec_t from_بنسټ, usec_t منزل_بنسټ);
usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t منزل_clock);

dual_timestamp* dual_timestamp_now(dual_timestamp *ts);
dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u);

triple_timestamp* triple_timestamp_now(triple_timestamp *ts);
triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u);
triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u);

#تعریف_کړه DUAL_TIMESTAMP_HAS_CLOCK(clock)                               \
        دننه_سېټ(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC)

#تعریف_کړه TRIPLE_TIMESTAMP_HAS_CLOCK(clock)                               \
        دننه_سېټ(clock, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC, CLOCK_BOOTTIME, CLOCK_BOOTTIME_ALARM)

ساكن مضمن bool timestamp_is_سېټ(usec_t timestamp) {
        أرجع timestamp > 0 && timestamp != USEC_INFINITY;
}

ساكن مضمن bool dual_timestamp_is_سېټ(ثابت dual_timestamp *ts) {
        أرجع timestamp_is_سېټ(ts->realtime) ||
               timestamp_is_سېټ(ts->monotonic);
}

ساكن مضمن bool triple_timestamp_is_سېټ(ثابت triple_timestamp *ts) {
        أرجع timestamp_is_سېټ(ts->realtime) ||
               timestamp_is_سېټ(ts->monotonic) ||
               timestamp_is_سېټ(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock);

usec_t timespec_بارول(ثابت بنية timespec *ts) خالص;
nsec_t timespec_بارول_nsec(ثابت بنية timespec *ts) خالص;
بنية timespec* timespec_store(بنية timespec *ts, usec_t u);
بنية timespec* timespec_store_nsec(بنية timespec *ts, nsec_t n);

#تعریف_کړه TIMESPEC_STORE(u) timespec_store(&(بنية timespec) {}, (u))
#تعریف_کړه TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(بنية timespec) {}, (n))

بنية __kernel_timespec* kernel_timespec_store(بنية __kernel_timespec *ts, usec_t u);

usec_t timeval_بارول(ثابت بنية timeval *tv) خالص;
بنية timeval* timeval_store(بنية timeval *tv, usec_t u);

#تعریف_کړه TIMEVAL_STORE(u) timeval_store(&(بنية timeval) {}, (u))

حرف* format_timestamp_style(حرف *buf, size_t l, usec_t t, TimestampStyle style) warn_ناکارول_شوی_پایله;
حرف* format_timestamp_relative_ډک(حرف *buf, size_t l, usec_t t, clockid_t clock, bool implicit_کیڼ) warn_ناکارول_شوی_پایله;
حرف* format_timespan(حرف *buf, size_t l, usec_t t, usec_t accuracy) warn_ناکارول_شوی_پایله;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
ثابت حرف* weekday_منزل_string(صحيح i);

warn_ناکارول_شوی_پایله
ساكن مضمن حرف* format_timestamp_relative(حرف *buf, size_t l, usec_t t) {
        أرجع format_timestamp_relative_ډک(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
warn_ناکارول_شوی_پایله
ساكن مضمن حرف* format_timestamp_relative_monotonic(حرف *buf, size_t l, usec_t t) {
        أرجع format_timestamp_relative_ډک(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

warn_ناکارول_شوی_پایله
ساكن مضمن حرف* format_timestamp(حرف *buf, size_t l, usec_t t) {
        أرجع format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#تعریف_کړه FORMAT_TIMESTAMP(t) format_timestamp((حرف[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#تعریف_کړه FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((حرف[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#تعریف_کړه FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((حرف[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#تعریف_کړه FORMAT_TIMESPAN(t, accuracy) format_timespan((حرف[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#تعریف_کړه FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((حرف[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

ثابت حرف* get_tzname(bool dst);
صحيح parse_gmtoff(ثابت حرف *t, اوږد *ret);
صحيح parse_timestamp(ثابت حرف *t, usec_t *ret);

صحيح parse_sec(ثابت حرف *t, usec_t *ret);
صحيح parse_sec_fix_0(ثابت حرف *t, usec_t *ret);
صحيح parse_sec_def_infinity(ثابت حرف *t, usec_t *ret);
صحيح parse_وخت(ثابت حرف *t, usec_t *ret, usec_t default_unit);
صحيح parse_nsec(ثابت حرف *t, nsec_t *ret);

صحيح get_timezones(حرف ***ret);
صحيح verify_timezone(ثابت حرف *name, صحيح log_level);
ساكن مضمن bool timezone_is_valid(ثابت حرف *name, صحيح log_level) {
        أرجع verify_timezone(name, log_level) >= 0;
}

فراغ reset_timezonep(حرف **p);
حرف* save_timezone(فراغ);
#تعریف_کړه SAVE_TIMEZONE                                                   \
        ناکارول_شوی _cleanup_(reset_timezonep)                             \
             حرف *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t clock);

usec_t usec_ښويدنه_clock(usec_t x, clockid_t from, clockid_t to);

صحيح get_timezone(حرف **ret);
صحيح get_timezone_prefer_env(حرف **ret);
ثابت حرف* etc_localtime(فراغ);

صحيح mktime_or_timegm_usec(بنية tm *tm, bool utc, usec_t *ret);
صحيح localtime_or_gmtime_usec(usec_t t, bool utc, بنية tm *ret);

صحيح parse_calendar_date_ډک(ثابت حرف *s, bool allow_pre_epoch, usec_t *ret_usec, بنية tm *ret_tm);

ساكن مضمن صحيح parse_calendar_date(ثابت حرف *s, usec_t *ret) {
        أرجع parse_calendar_date_ډک(s, /* allow_pre_epoch= */ false, ret, عدم);
}

#تعریف_کړه BIRTH_DATE_UNSET                        \
        (ثابت بنية tm) {                     \
                .tm_year = INT_MIN,             \
        }

#تعریف_کړه BIRTH_DATE_IS_سېټ(tm) ((tm).tm_year != INT_MIN)

ساكن مضمن صحيح parse_birth_date(ثابت حرف *s, بنية tm *ret) {
        أرجع parse_calendar_date_ډک(s, /* allow_pre_epoch= */ true, عدم, ret);
}

uint64_t sysconf_clock_ticks_cached(فراغ);

uint32_t usec_منزل_jiffies(usec_t usec);
usec_t jiffies_منزل_usec(uint32_t jiffies);

bool دننه_utc_timezone(فراغ);

ساكن مضمن usec_t usec_جمع_کړه(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        أرجع saturate_جمع_کړه(a, b, USEC_INFINITY);
}

ساكن مضمن usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        إذا (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                أرجع USEC_INFINITY;
        إذا (timestamp < delta)
                أرجع 0;

        أرجع timestamp - delta;
}

ساكن مضمن usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        إذا (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                أرجع usec_جمع_کړه(timestamp, (usec_t) INT64_MAX + 1);
        }
        إذا (delta < 0)
                أرجع usec_جمع_کړه(timestamp, (usec_t) (-delta));

        أرجع usec_sub_unsigned(timestamp, (usec_t) delta);
}

صحيح usleep_خوندي(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#تعریف_کړه USEC_TIMESTAMP_FORMATTABLE_MAX_64_بټ ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#تعریف_کړه USEC_TIMESTAMP_FORMATTABLE_MAX_32_بټ (((usec_t) INT32_MAX) * USEC_فی_SEC - USEC_فی_DAY)
#إذا SIZEOF_وخت_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64_بټ
#وإلاإذا SIZEOF_وخت_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32_بټ
#وإلا
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#د_که_پای

صحيح وخت_change_fd(فراغ);

DECLARE_STRING_جدول_LOOKUP(timestamp_style, TimestampStyle);
