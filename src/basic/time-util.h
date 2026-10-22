/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <limits.h>
#समावेश <time.h>

#समावेश "forward.h"

#घोषणा PRI_NSEC PRIu64
#घोषणा PRI_USEC PRIu64
#घोषणा NSEC_FMT "%" PRI_NSEC
#घोषणा USEC_FMT "%" PRI_USEC

प्रकार काष्ठा dual_timestamp {
        usec_t realtime;
        usec_t monotonic;
} dual_timestamp;

प्रकार काष्ठा triple_timestamp {
        usec_t realtime;
        usec_t monotonic;
        usec_t boottime;
} triple_timestamp;

प्रकार क्रमागत TimestampStyle {
        TIMESTAMP_PRETTY,
        TIMESTAMP_US,
        TIMESTAMP_UTC,
        TIMESTAMP_US_UTC,
        TIMESTAMP_UNIX,
        TIMESTAMP_DATE,
        _TIMESTAMP_STYLE_MAX,
        _TIMESTAMP_STYLE_INVALID = -EINVAL,
} TimestampStyle;

#घोषणा USEC_INFINITY ((usec_t) UINT64_MAX)
#घोषणा NSEC_INFINITY ((nsec_t) UINT64_MAX)

#घोषणा MSEC_PER_SEC  1000ULL
#घोषणा USEC_PER_SEC  ((usec_t) 1000000ULL)
#घोषणा USEC_PER_MSEC ((usec_t) 1000ULL)
#घोषणा NSEC_PER_SEC  ((nsec_t) 1000000000ULL)
#घोषणा NSEC_PER_MSEC ((nsec_t) 1000000ULL)
#घोषणा NSEC_PER_USEC ((nsec_t) 1000ULL)

#घोषणा USEC_PER_MINUTE ((usec_t) (60ULL*USEC_PER_SEC))
#घोषणा NSEC_PER_MINUTE ((nsec_t) (60ULL*NSEC_PER_SEC))
#घोषणा USEC_PER_HOUR ((usec_t) (60ULL*USEC_PER_MINUTE))
#घोषणा NSEC_PER_HOUR ((nsec_t) (60ULL*NSEC_PER_MINUTE))
#घोषणा USEC_PER_DAY ((usec_t) (24ULL*USEC_PER_HOUR))
#घोषणा NSEC_PER_DAY ((nsec_t) (24ULL*NSEC_PER_HOUR))
#घोषणा USEC_PER_WEEK ((usec_t) (7ULL*USEC_PER_DAY))
#घोषणा NSEC_PER_WEEK ((nsec_t) (7ULL*NSEC_PER_DAY))
#घोषणा USEC_PER_MONTH ((usec_t) (2629800ULL*USEC_PER_SEC))
#घोषणा NSEC_PER_MONTH ((nsec_t) (2629800ULL*NSEC_PER_SEC))
#घोषणा USEC_PER_YEAR ((usec_t) (31557600ULL*USEC_PER_SEC))
#घोषणा NSEC_PER_YEAR ((nsec_t) (31557600ULL*NSEC_PER_SEC))

क्रमागत {
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
#घोषणा FORMAT_TIMESTAMP_MAX (3U+1U+10U+1U+8U+1U+6U+1U+6U+1U)
#घोषणा FORMAT_TIMESTAMP_RELATIVE_MAX 256U
#घोषणा FORMAT_TIMESPAN_MAX 64U

#घोषणा TIME_T_MAX (समय_प्रकार)((UINTMAX_C(1) << ((माप(समय_प्रकार) << 3) - 1)) - 1)

#घोषणा DUAL_TIMESTAMP_NULL ((dual_timestamp) {})
#घोषणा DUAL_TIMESTAMP_INFINITY ((dual_timestamp) { USEC_INFINITY, USEC_INFINITY })
#घोषणा TRIPLE_TIMESTAMP_NULL ((triple_timestamp) {})

#घोषणा TIMESPEC_OMIT ((स्थिर काष्ठा timespec) { .tv_nsec = UTIME_OMIT })

usec_t now(clockid_t घड़ी);
nsec_t now_nsec(clockid_t घड़ी);

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base);
usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock);

dual_timestamp* dual_timestamp_now(dual_timestamp *ts);
dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u);
dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u);

triple_timestamp* triple_timestamp_now(triple_timestamp *ts);
triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u);
triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u);

#घोषणा DUAL_TIMESTAMP_HAS_CLOCK(घड़ी)                               \
        IN_SET(घड़ी, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC)

#घोषणा TRIPLE_TIMESTAMP_HAS_CLOCK(घड़ी)                               \
        IN_SET(घड़ी, CLOCK_REALTIME, CLOCK_REALTIME_ALARM, CLOCK_MONOTONIC, CLOCK_BOOTTIME, CLOCK_BOOTTIME_ALARM)

अटल अंतरभूत bool timestamp_is_set(usec_t timestamp) {
        वापस timestamp > 0 && timestamp != USEC_INFINITY;
}

अटल अंतरभूत bool dual_timestamp_is_set(स्थिर dual_timestamp *ts) {
        वापस timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic);
}

अटल अंतरभूत bool triple_timestamp_is_set(स्थिर triple_timestamp *ts) {
        वापस timestamp_is_set(ts->realtime) ||
               timestamp_is_set(ts->monotonic) ||
               timestamp_is_set(ts->boottime);
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t घड़ी);

usec_t timespec_load(स्थिर काष्ठा timespec *ts) _pure_;
nsec_t timespec_load_nsec(स्थिर काष्ठा timespec *ts) _pure_;
काष्ठा timespec* timespec_store(काष्ठा timespec *ts, usec_t u);
काष्ठा timespec* timespec_store_nsec(काष्ठा timespec *ts, nsec_t n);

#घोषणा TIMESPEC_STORE(u) timespec_store(&(काष्ठा timespec) {}, (u))
#घोषणा TIMESPEC_STORE_NSEC(n) timespec_store_nsec(&(काष्ठा timespec) {}, (n))

काष्ठा __kernel_timespec* kernel_timespec_store(काष्ठा __kernel_timespec *ts, usec_t u);

usec_t timeval_load(स्थिर काष्ठा timeval *tv) _pure_;
काष्ठा timeval* timeval_store(काष्ठा timeval *tv, usec_t u);

#घोषणा TIMEVAL_STORE(u) timeval_store(&(काष्ठा timeval) {}, (u))

अक्षर* format_timestamp_style(अक्षर *buf, माप_प्रकार l, usec_t t, TimestampStyle style) warn_unused_परिणाम;
अक्षर* format_timestamp_relative_full(अक्षर *buf, माप_प्रकार l, usec_t t, clockid_t घड़ी, bool implicit_बायाँ) warn_unused_परिणाम;
अक्षर* format_timespan(अक्षर *buf, माप_प्रकार l, usec_t t, usec_t accuracy) warn_unused_परिणाम;

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order
 * (matching systemd's weekdays_bits layout). */
स्थिर अक्षर* weekday_to_string(पूर्णांक i);

warn_unused_परिणाम
अटल अंतरभूत अक्षर* format_timestamp_relative(अक्षर *buf, माप_प्रकार l, usec_t t) {
        वापस format_timestamp_relative_full(buf, l, t, CLOCK_REALTIME, /* implicit_left= */ false);
}
warn_unused_परिणाम
अटल अंतरभूत अक्षर* format_timestamp_relative_monotonic(अक्षर *buf, माप_प्रकार l, usec_t t) {
        वापस format_timestamp_relative_full(buf, l, t, CLOCK_MONOTONIC, /* implicit_left= */ false);
}

warn_unused_परिणाम
अटल अंतरभूत अक्षर* format_timestamp(अक्षर *buf, माप_प्रकार l, usec_t t) {
        वापस format_timestamp_style(buf, l, t, TIMESTAMP_PRETTY);
}

/* Note: the lifetime of the compound literal is the immediately surrounding block,
 * see C11 §6.5.2.5, and
 * https://stackoverflow.com/questions/34880638/compound-literal-lifetime-and-if-blocks */
#घोषणा FORMAT_TIMESTAMP(t) format_timestamp((अक्षर[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t)
#घोषणा FORMAT_TIMESTAMP_RELATIVE(t)                                    \
        format_timestamp_relative((अक्षर[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#घोषणा FORMAT_TIMESTAMP_RELATIVE_MONOTONIC(t)                          \
        format_timestamp_relative_monotonic((अक्षर[FORMAT_TIMESTAMP_RELATIVE_MAX]){}, FORMAT_TIMESTAMP_RELATIVE_MAX, t)
#घोषणा FORMAT_TIMESPAN(t, accuracy) format_timespan((अक्षर[FORMAT_TIMESPAN_MAX]){}, FORMAT_TIMESPAN_MAX, t, accuracy)
#घोषणा FORMAT_TIMESTAMP_STYLE(t, style) \
        format_timestamp_style((अक्षर[FORMAT_TIMESTAMP_MAX]){}, FORMAT_TIMESTAMP_MAX, t, style)

स्थिर अक्षर* get_tzname(bool dst);
पूर्णांक parse_gmtoff(स्थिर अक्षर *t, दीर्घ *ret);
पूर्णांक parse_timestamp(स्थिर अक्षर *t, usec_t *ret);

पूर्णांक parse_sec(स्थिर अक्षर *t, usec_t *ret);
पूर्णांक parse_sec_fix_0(स्थिर अक्षर *t, usec_t *ret);
पूर्णांक parse_sec_def_infinity(स्थिर अक्षर *t, usec_t *ret);
पूर्णांक parse_time(स्थिर अक्षर *t, usec_t *ret, usec_t default_unit);
पूर्णांक parse_nsec(स्थिर अक्षर *t, nsec_t *ret);

पूर्णांक get_timezones(अक्षर ***ret);
पूर्णांक verify_timezone(स्थिर अक्षर *name, पूर्णांक log_level);
अटल अंतरभूत bool timezone_is_valid(स्थिर अक्षर *name, पूर्णांक log_level) {
        वापस verify_timezone(name, log_level) >= 0;
}

व्योम reset_timezonep(अक्षर **p);
अक्षर* save_timezone(व्योम);
#घोषणा SAVE_TIMEZONE                                                   \
        _unused_ _cleanup_(reset_timezonep)                             \
             अक्षर *_saved_timezone_ = save_timezone()

bool clock_supported(clockid_t घड़ी);

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to);

पूर्णांक get_timezone(अक्षर **ret);
पूर्णांक get_timezone_prefer_env(अक्षर **ret);
स्थिर अक्षर* etc_localtime(व्योम);

पूर्णांक mktime_or_timegm_usec(काष्ठा पंचांग *पंचांग, bool utc, usec_t *ret);
पूर्णांक localtime_or_gmtime_usec(usec_t t, bool utc, काष्ठा पंचांग *ret);

पूर्णांक parse_calendar_date_full(स्थिर अक्षर *s, bool allow_pre_epoch, usec_t *ret_usec, काष्ठा पंचांग *ret_tm);

अटल अंतरभूत पूर्णांक parse_calendar_date(स्थिर अक्षर *s, usec_t *ret) {
        वापस parse_calendar_date_full(s, /* allow_pre_epoch= */ false, ret, शून्य);
}

#घोषणा BIRTH_DATE_UNSET                        \
        (स्थिर काष्ठा पंचांग) {                     \
                .tm_year = पूर्णांक_न्यून,             \
        }

#घोषणा BIRTH_DATE_IS_SET(पंचांग) ((पंचांग).tm_year != पूर्णांक_न्यून)

अटल अंतरभूत पूर्णांक parse_birth_date(स्थिर अक्षर *s, काष्ठा पंचांग *ret) {
        वापस parse_calendar_date_full(s, /* allow_pre_epoch= */ true, शून्य, ret);
}

uint64_t sysconf_clock_ticks_cached(व्योम);

uint32_t usec_to_jiffies(usec_t usec);
usec_t jiffies_to_usec(uint32_t jiffies);

bool in_utc_timezone(व्योम);

अटल अंतरभूत usec_t usec_add(usec_t a, usec_t b) {
        /* Adds two time values, and makes sure USEC_INFINITY as input results as USEC_INFINITY in output,
         * and doesn't overflow. */
        वापस saturate_add(a, b, USEC_INFINITY);
}

अटल अंतरभूत usec_t usec_sub_unsigned(usec_t timestamp, usec_t delta) {
        अगर (timestamp == USEC_INFINITY) /* Make sure infinity doesn't degrade */
                वापस USEC_INFINITY;
        अगर (timestamp < delta)
                वापस 0;

        वापस timestamp - delta;
}

अटल अंतरभूत usec_t usec_sub_signed(usec_t timestamp, int64_t delta) {
        अगर (delta == INT64_MIN) { /* prevent overflow */
                assert_cc(-(INT64_MIN + 1) == INT64_MAX);
                assert_cc(USEC_INFINITY > INT64_MAX);
                वापस usec_add(timestamp, (usec_t) INT64_MAX + 1);
        }
        अगर (delta < 0)
                वापस usec_add(timestamp, (usec_t) (-delta));

        वापस usec_sub_unsigned(timestamp, (usec_t) delta);
}

पूर्णांक usleep_safe(usec_t usec);

/* The last second we can format is 31. Dec 9999, 1s before midnight, because otherwise we'd enter 5 digit
 * year territory. However, since we want to stay away from this in all timezones we take one day off. */
#घोषणा USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT ((usec_t) 253402214399000000) /* Thu 9999-12-30 23:59:59 UTC */
/* With a 32-bit time_t we can't go beyond 2038...
 * We parse timestamp with RFC-822/ISO 8601 (e.g. +06, or -03:00) as UTC, hence the upper bound must be off
 * by USEC_PER_DAY. See parse_timestamp() for more details. */
#घोषणा USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT (((usec_t) INT32_MAX) * USEC_PER_SEC - USEC_PER_DAY)
#अगर SIZEOF_TIME_T == 8
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_64BIT
#या_अगर SIZEOF_TIME_T == 4
#  define USEC_TIMESTAMP_FORMATTABLE_MAX USEC_TIMESTAMP_FORMATTABLE_MAX_32BIT
#अन्यथा
#  error "Yuck, time_t is neither 4 nor 8 bytes wide?"
#पूर्ण_अगर

पूर्णांक time_change_fd(व्योम);

DECLARE_STRING_TABLE_LOOKUP(timestamp_style, TimestampStyle);
