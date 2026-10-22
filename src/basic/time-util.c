/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/time_types.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <threads.h>
#include <unistd.h>

#include "alloc-util.h"
#include "env-util.h"
#include "errno-util.h"
#include "extract-word.h"
#include "hexdecoct.h"          /* IWYU pragma: keep */
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "io-util.h"
#include "log.h"
#include "parse-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "time-util.h"

static clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        switch (c) {

        case CLOCK_BOOTTIME_ALARM:
                返回 CLOCK_BOOTTIME;

        case CLOCK_REALTIME_ALARM:
                返回 CLOCK_REALTIME;

        default:
                返回 c;
        }
}

usec_t now(clockid_t clock_id) {
        struct timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        assert(n > 0);
        assert(n < USEC_INFINITY);

        返回 n;
}

nsec_t now_nsec(clockid_t clock_id) {
        struct timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        assert(n > 0);
        assert(n < NSEC_INFINITY);

        返回 n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        assert(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        返回 ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        assert(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        返回 ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        如果 (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                如果 (to_base >= USEC_INFINITY - delta) /* overflow? */
                        返回 USEC_INFINITY;

                返回 to_base + delta;

        } 否则 { /* In the past */
                usec_t delta = from_base - from;

                如果 (to_base <= delta) /* underflow? */
                        返回 0;

                返回 to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        如果 (map_clock_id(from_clock) == map_clock_id(to_clock))
                返回 from;

        /* Keep infinity as is */
        如果 (from == USEC_INFINITY)
                返回 from;

        返回 map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        assert(ts);

        如果 (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                返回 ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        返回 ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        assert(ts);

        如果 (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                返回 ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        返回 ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        assert(ts);

        如果 (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                返回 ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        返回 ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        assert(ts);

        如果 (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                返回 ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        返回 ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        assert(ts);

        如果 (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                返回 ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        返回 ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        assert(ts);

        switch (clock) {

        case CLOCK_REALTIME:
        case CLOCK_REALTIME_ALARM:
                返回 ts->realtime;

        case CLOCK_MONOTONIC:
                返回 ts->monotonic;

        case CLOCK_BOOTTIME:
        case CLOCK_BOOTTIME_ALARM:
                返回 ts->boottime;

        default:
                返回 USEC_INFINITY;
        }
}

usec_t timespec_load(const struct timespec *ts) {
        assert(ts);

        如果 (ts->tv_sec < 0 || ts->tv_nsec < 0)
                返回 USEC_INFINITY;

        如果 ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                返回 USEC_INFINITY;

        返回
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(const struct timespec *ts) {
        assert(ts);

        如果 (ts->tv_sec < 0 || ts->tv_nsec < 0)
                返回 NSEC_INFINITY;

        如果 ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                返回 NSEC_INFINITY;

        返回 (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

struct timespec *timespec_store(struct timespec *ts, usec_t u) {
        assert(ts);

        如果 (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                返回 ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        返回 ts;
}

struct timespec *timespec_store_nsec(struct timespec *ts, nsec_t n) {
        assert(ts);

        如果 (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                返回 ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        返回 ts;
}

struct __kernel_timespec *kernel_timespec_store(struct __kernel_timespec *ts, usec_t u) {
        assert(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        assert(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        返回 ts;
}

usec_t timeval_load(const struct timeval *tv) {
        assert(tv);

        如果 (tv->tv_sec < 0 || tv->tv_usec < 0)
                返回 USEC_INFINITY;

        如果 ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                返回 USEC_INFINITY;

        返回
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

struct timeval *timeval_store(struct timeval *tv, usec_t u) {
        assert(tv);

        如果 (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } 否则 {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        返回 tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
static const char *const weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, 整数);

char* format_timestamp_style(
                char *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        struct tm tm;
        bool utc, us;
        size_t n;

        assert(buf);
        assert(style >= 0);
        assert(style < _TIMESTAMP_STYLE_MAX);

        如果 (!timestamp_is_set(t))
                返回 NULL; /* Timestamp is unset */

        如果 (style == TIMESTAMP_UNIX) {
                如果 (l < (size_t) (1 + 1 + 1))
                        返回 NULL; /* not enough space for even the shortest of forms */

                返回 snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        如果 (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                返回 NULL; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        如果 (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                static const char* const xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                assert(l >= strlen(xxx[style]) + 1);
                返回 strcpy(buf, xxx[style]);
        }

        如果 (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                返回 NULL;

        /* Start with the week day */
        const char *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        assert(weekday);
        memcpy(buf, weekday, 4);

        如果 (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                如果 (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        返回 NULL; /* Doesn't fit */

                返回 buf;
        }

        /* Add the main components */
        如果 (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                返回 NULL; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        如果 (us) {
                n = strlen(buf);
                如果 (n + 8 > l)
                        返回 NULL; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        如果 (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                如果 (n + 5 > l)
                        返回 NULL; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } 否则 如果 (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                如果 (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        如果 (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                返回 NULL; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } 否则 {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        返回 buf;
}

char* format_timestamp_relative_full(char *buf, size_t l, usec_t t, clockid_t clock, bool implicit_left) {
        const char *s;
        usec_t n, d;

        assert(buf);

        如果 (!timestamp_is_set(t))
                返回 NULL;

        n = now(clock);
        如果 (n > t) {
                d = n - t;
                s = " ago";
        } 否则 {
                d = t - n;
                s = implicit_left ? "" : " left";
        }

        如果 (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } 否则 如果 (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } 否则 如果 (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } 否则 如果 (d >= 2*USEC_PER_DAY)
                (void) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        否则 如果 (d >= 25*USEC_PER_HOUR)
                (void) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        否则 如果 (d >= 6*USEC_PER_HOUR)
                (void) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        否则 如果 (d >= USEC_PER_HOUR)
                (void) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        否则 如果 (d >= 5*USEC_PER_MINUTE)
                (void) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        否则 如果 (d >= USEC_PER_MINUTE)
                (void) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        否则 如果 (d >= USEC_PER_SEC)
                (void) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        否则 如果 (d >= USEC_PER_MSEC)
                (void) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        否则 如果 (d > 0)
                (void) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        否则
                (void) snprintf(buf, l, "now");

        buf[l-1] = 0;
        返回 buf;
}

char* format_timespan(char *buf, size_t l, usec_t t, usec_t accuracy) {
        static const struct {
                const char *suffix;
                usec_t usec;
        } table[] = {
                { "y",     USEC_PER_YEAR   },
                { "month", USEC_PER_MONTH  },
                { "w",     USEC_PER_WEEK   },
                { "d",     USEC_PER_DAY    },
                { "h",     USEC_PER_HOUR   },
                { "min",   USEC_PER_MINUTE },
                { "s",     USEC_PER_SEC    },
                { "ms",    USEC_PER_MSEC   },
                { "us",    1               },
        };

        char *p = ASSERT_PTR(buf);
        bool something = false;

        assert(l > 0);

        如果 (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                返回 p;
        }

        如果 (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                返回 p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                整数 k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                如果 (t <= 0)
                        break;

                如果 (t < accuracy && something)
                        break;

                如果 (t < i->usec)
                        continue;

                如果 (l <= 1)
                        break;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                如果 (t < USEC_PER_MINUTE && b > 0) {
                        signed char j = 0;

                        为 (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        为 (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        如果 (j > 0) {
                                k = snprintf(p, l,
                                             "%s"USEC_FMT".%0*"PRI_USEC"%s",
                                             p > buf ? " " : "",
                                             a,
                                             j,
                                             b,
                                             i->suffix);

                                t = 0;
                                done = true;
                        }
                }

                /* No? Then let's show it normally */
                如果 (!done) {
                        k = snprintf(p, l,
                                     "%s"USEC_FMT"%s",
                                     p > buf ? " " : "",
                                     a,
                                     i->suffix);

                        t = b;
                }

                n = MIN((size_t) k, l-1);

                l -= n;
                p += n;

                something = true;
        }

        *p = 0;

        返回 buf;
}

const char* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        如果 (dst && isempty(tzname[1]))
                dst = false;

        返回 empty_to_null(tzname[dst]);
}

整数 parse_gmtoff(const char *t, long *ret) {
        assert(t);

        struct tm tm;
        const char *k = strptime(t, "%z", &tm);
        如果 (k && *k == '\0') {
                /* Success! */
                如果 (ret)
                        *ret = tm.tm_gmtoff;
                返回 0;
        }

#ifdef __GLIBC__
        返回 -EINVAL;
#否则
        整数 r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        switch (*t) {
        case '+':
                positive = true;
                break;
        case '-':
                positive = false;
                break;
        default:
                返回 -EINVAL;
        }

        t++;
        r = undecchar(*t);
        如果 (r < 0)
                返回 r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        如果 (r < 0)
                返回 r;
        u += r * USEC_PER_HOUR;

        t++;
        如果 (*t == '\0') /* 2 digits case */
                goto finalize;

        如果 (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        如果 (r < 0)
                返回 r;
        如果 (r >= 6) /* refuse minutes equal to or larger than 60 */
                返回 -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        如果 (r < 0)
                返回 r;

        u += r * USEC_PER_MINUTE;

        t++;
        如果 (*t != '\0')
                返回 -EINVAL;

finalize:
        如果 (u > USEC_PER_DAY) /* refuse larger than one day */
                返回 -EINVAL;

        如果 (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        返回 0;
#endif
}

static 整数 parse_timestamp_impl(
                const char *t,
                size_t max_len,
                bool utc,
                整数 isdst,
                long gmtoff,
                usec_t *ret) {

        static const struct {
                const char *name;
                const 整数 nr;
        } day_nr[] = {
                { "Sunday",    0 },
                { "Sun",       0 },
                { "Monday",    1 },
                { "Mon",       1 },
                { "Tuesday",   2 },
                { "Tue",       2 },
                { "Wednesday", 3 },
                { "Wed",       3 },
                { "Thursday",  4 },
                { "Thu",       4 },
                { "Friday",    5 },
                { "Fri",       5 },
                { "Saturday",  6 },
                { "Sat",       6 },
        };

        _cleanup_free_ char *t_alloc = NULL;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        整数 r, weekday = -1;
        unsigned fractional = 0;
        const char *k;
        struct tm tm, copy;

        /* Allowed syntaxes:
         *
         *   2012-09-22 16:34:22.1[2[3[4[5[6]]]]]
         *   2012-09-22 16:34:22  (µsec will be set to 0)
         *   2012-09-22 16:34     (seconds will be set to 0)
         *   2012-09-22T16:34:22.1[2[3[4[5[6]]]]]
         *   2012-09-22T16:34:22  (µsec will be set to 0)
         *   2012-09-22T16:34     (seconds will be set to 0)
         *   2012-09-22           (time will be set to 00:00:00)
         *   16:34:22             (date will be set to today)
         *   16:34                (date will be set to today, seconds to 0)
         *   now
         *   yesterday            (time is set to 00:00:00)
         *   today                (time is set to 00:00:00)
         *   tomorrow             (time is set to 00:00:00)
         *   +5min
         *   -5days
         *   @2147483647          (seconds since epoch)
         *
         * Note, on DST change, 00:00:00 may not exist and in that case the time part may be shifted.
         * E.g. "Sun 2023-03-13 America/Havana" is parsed as "Sun 2023-03-13 01:00:00 CDT".
         *
         * A simplified strptime-spelled RFC3339 ABNF looks like
         *   "%Y-%m-%d" "T" "%H" ":" "%M" ":" "%S" [".%N"] ("Z" / (("+" / "-") "%H:%M"))
         * We additionally allow no seconds and inherited timezone
         * for symmetry with our other syntaxes and improved interactive usability:
         *   "%Y-%m-%d" "T" "%H" ":" "%M" ":" ["%S" [".%N"]] ["Z" / (("+" / "-") "%H:%M")]
         * RFC3339 defines time-secfrac to as "." 1*DIGIT, but we limit to 6 digits,
         * since we're limited to 1µs resolution.
         * We also accept "Sat 2012-09-22T16:34:22", RFC3339 warns against it.
         */

        assert(t);

        如果 (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                如果 (max_len == 0) /* Can't be the only field */
                        返回 -EINVAL;

                t_alloc = strndup(t, max_len);
                如果 (!t_alloc)
                        返回 -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        如果 (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                如果 ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        返回 -EINVAL;
        } 否则 {
                如果 (gmtoff != 0)
                        返回 -EINVAL;
        }

        如果 (t[0] == '@' && !with_tz)
                返回 parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        如果 (!with_tz) {
                如果 (streq(t, "now"))
                        goto finish;

                如果 (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        如果 (r < 0)
                                返回 r;

                        goto finish;
                }

                如果 (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        如果 (r < 0)
                                返回 r;

                        goto finish;
                }

                如果 ((k = endswith(t, " ago"))) {
                        _cleanup_free_ char *buf = NULL;

                        buf = strndup(t, k - t);
                        如果 (!buf)
                                返回 -ENOMEM;

                        r = parse_sec(buf, &minus);
                        如果 (r < 0)
                                返回 r;

                        goto finish;
                }

                如果 ((k = endswith(t, " left"))) {
                        _cleanup_free_ char *buf = NULL;

                        buf = strndup(t, k - t);
                        如果 (!buf)
                                返回 -ENOMEM;

                        r = parse_sec(buf, &plus);
                        如果 (r < 0)
                                返回 r;

                        goto finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        如果 (r < 0)
                返回 r;

        tm.tm_isdst = isdst;

        如果 (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;

        } 否则 如果 (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;

        } 否则 如果 (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                如果 (!k || *k != ' ')
                        continue;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                break;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        如果 (k) {
                如果 (*k == '.')
                        goto parse_usec;
                否则 如果 (*k == 0)
                        goto from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        如果 (k) {
                如果 (*k == '.')
                        goto parse_usec;
                否则 如果 (*k == 0)
                        goto from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        如果 (k) {
                如果 (*k == '.')
                        goto parse_usec;
                否则 如果 (*k == 0)
                        goto from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        如果 (k) {
                如果 (*k == '.')
                        goto parse_usec;
                否则 如果 (*k == 0)
                        goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        如果 (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        如果 (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        如果 (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        如果 (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        如果 (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        如果 (k) {
                如果 (*k == '.')
                        goto parse_usec;
                否则 如果 (*k == 0)
                        goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        如果 (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        返回 -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        如果 (r < 0)
                返回 -EINVAL;
        如果 (*k != '\0')
                返回 -EINVAL;

from_tm:
        assert(plus == 0);
        assert(minus == 0);

        如果 (weekday >= 0 && tm.tm_wday != weekday)
                返回 -EINVAL;

        如果 (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                如果 (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } 否则
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        如果 (r < 0)
                返回 r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        如果 (usec < minus)
                返回 -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        如果 (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                返回 -EINVAL;

        如果 (ret)
                *ret = usec;
        返回 0;
}

整数 parse_timestamp(const char *t, usec_t *ret) {
        long gmtoff;
        整数 r;

        assert(t);

        size_t t_len = strlen(t);
        如果 (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                如果 (r >= 0)
                        返回 r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        如果 (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                返回 parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        const char *tz = strrchr(t, ' ');
        如果 (!tz)
                返回 parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        如果 (streq(tz, "UTC"))
                返回 parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        如果 (parse_gmtoff(tz, &gmtoff) >= 0)
                返回 parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        为 (整数 j = 0; j <= 1; j++) {
                如果 (!streq_ptr(tz, get_tzname(j)))
                        continue;

                /* The specified timezone matches tzname[] of the local timezone. */
                返回 parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        如果 (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                如果 (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        返回 negative_errno();

                返回 parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        返回 parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

static const char* extract_multiplier(const char *p, usec_t *ret) {
        static const struct {
                const char *suffix;
                usec_t usec;
        } table[] = {
                { "seconds", USEC_PER_SEC    },
                { "second",  USEC_PER_SEC    },
                { "sec",     USEC_PER_SEC    },
                { "s",       USEC_PER_SEC    },
                { "minutes", USEC_PER_MINUTE },
                { "minute",  USEC_PER_MINUTE },
                { "min",     USEC_PER_MINUTE },
                { "months",  USEC_PER_MONTH  },
                { "month",   USEC_PER_MONTH  },
                { "M",       USEC_PER_MONTH  },
                { "msec",    USEC_PER_MSEC   },
                { "ms",      USEC_PER_MSEC   },
                { "m",       USEC_PER_MINUTE },
                { "hours",   USEC_PER_HOUR   },
                { "hour",    USEC_PER_HOUR   },
                { "hr",      USEC_PER_HOUR   },
                { "h",       USEC_PER_HOUR   },
                { "days",    USEC_PER_DAY    },
                { "day",     USEC_PER_DAY    },
                { "d",       USEC_PER_DAY    },
                { "weeks",   USEC_PER_WEEK   },
                { "week",    USEC_PER_WEEK   },
                { "w",       USEC_PER_WEEK   },
                { "years",   USEC_PER_YEAR   },
                { "year",    USEC_PER_YEAR   },
                { "y",       USEC_PER_YEAR   },
                { "usec",    1ULL            },
                { "us",      1ULL            },
                { "μs",      1ULL            }, /* U+03bc (aka GREEK SMALL LETTER MU) */
                { "µs",      1ULL            }, /* U+b5 (aka MICRO SIGN) */
        };

        assert(p);
        assert(ret);

        FOREACH_ELEMENT(i, table) {
                const char *e = startswith(p, i->suffix);
                如果 (e) {
                        *ret = i->usec;
                        返回 e;
                }
        }

        返回 p;
}

整数 parse_time(const char *t, usec_t *ret, usec_t default_unit) {
        const char *p, *s;

        assert(t);
        assert(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ NULL);
        s = startswith(p, "infinity");
        如果 (s) {
                如果 (!in_charset(s, WHITESPACE))
                        返回 -EINVAL;

                如果 (ret)
                        *ret = USEC_INFINITY;
                返回 0;
        }

        usec_t usec = 0;

        为 (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                char *e;

                p = skip_leading_chars(p, /* bad= */ NULL);
                如果 (*p == 0) {
                        如果 (!something)
                                返回 -EINVAL;

                        break;
                }

                如果 (*p == '-') /* Don't allow "-0" */
                        返回 -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                如果 (errno > 0)
                        返回 -errno;
                如果 (l < 0)
                        返回 -ERANGE;

                如果 (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } 否则 如果 (e == p)
                        返回 -EINVAL;
                否则
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                如果 (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        返回 -EINVAL;

                p = s;

                如果 ((usec_t) l >= USEC_INFINITY / multiplier)
                        返回 -ERANGE;

                k = (usec_t) l * multiplier;
                如果 (k >= USEC_INFINITY - usec)
                        返回 -ERANGE;

                usec += k;

                something = true;

                如果 (*e == '.') {
                        usec_t m = multiplier / 10;
                        const char *b;

                        为 (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                如果 (k >= USEC_INFINITY - usec)
                                        返回 -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        如果 (b == e + 1)
                                返回 -EINVAL;
                }
        }

        如果 (ret)
                *ret = usec;
        返回 0;
}

整数 parse_sec(const char *t, usec_t *ret) {
        返回 parse_time(t, ret, USEC_PER_SEC);
}

整数 parse_sec_fix_0(const char *t, usec_t *ret) {
        usec_t k;
        整数 r;

        assert(t);
        assert(ret);

        r = parse_sec(t, &k);
        如果 (r < 0)
                返回 r;

        *ret = k == 0 ? USEC_INFINITY : k;
        返回 r;
}

整数 parse_sec_def_infinity(const char *t, usec_t *ret) {
        assert(t);
        assert(ret);

        t += strspn(t, WHITESPACE);
        如果 (isempty(t)) {
                *ret = USEC_INFINITY;
                返回 0;
        }
        返回 parse_sec(t, ret);
}

static const char* extract_nsec_multiplier(const char *p, nsec_t *ret) {
        static const struct {
                const char *suffix;
                nsec_t nsec;
        } table[] = {
                { "seconds", NSEC_PER_SEC    },
                { "second",  NSEC_PER_SEC    },
                { "sec",     NSEC_PER_SEC    },
                { "s",       NSEC_PER_SEC    },
                { "minutes", NSEC_PER_MINUTE },
                { "minute",  NSEC_PER_MINUTE },
                { "min",     NSEC_PER_MINUTE },
                { "months",  NSEC_PER_MONTH  },
                { "month",   NSEC_PER_MONTH  },
                { "M",       NSEC_PER_MONTH  },
                { "msec",    NSEC_PER_MSEC   },
                { "ms",      NSEC_PER_MSEC   },
                { "m",       NSEC_PER_MINUTE },
                { "hours",   NSEC_PER_HOUR   },
                { "hour",    NSEC_PER_HOUR   },
                { "hr",      NSEC_PER_HOUR   },
                { "h",       NSEC_PER_HOUR   },
                { "days",    NSEC_PER_DAY    },
                { "day",     NSEC_PER_DAY    },
                { "d",       NSEC_PER_DAY    },
                { "weeks",   NSEC_PER_WEEK   },
                { "week",    NSEC_PER_WEEK   },
                { "w",       NSEC_PER_WEEK   },
                { "years",   NSEC_PER_YEAR   },
                { "year",    NSEC_PER_YEAR   },
                { "y",       NSEC_PER_YEAR   },
                { "usec",    NSEC_PER_USEC   },
                { "us",      NSEC_PER_USEC   },
                { "μs",      NSEC_PER_USEC   }, /* U+03bc (aka GREEK LETTER MU) */
                { "µs",      NSEC_PER_USEC   }, /* U+b5 (aka MICRO SIGN) */
                { "nsec",    1ULL            },
                { "ns",      1ULL            },
                { "",        1ULL            }, /* default is nsec */
        };

        assert(p);
        assert(ret);

        FOREACH_ELEMENT(i, table) {
                const char *e = startswith(p, i->suffix);
                如果 (e) {
                        *ret = i->nsec;
                        返回 e;
                }
        }

        返回 p;
}

整数 parse_nsec(const char *t, nsec_t *ret) {
        const char *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        assert(t);
        assert(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        如果 (s) {
                s += strspn(s, WHITESPACE);
                如果 (*s != 0)
                        返回 -EINVAL;

                *ret = NSEC_INFINITY;
                返回 0;
        }

        为 (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                char *e;

                p += strspn(p, WHITESPACE);

                如果 (*p == 0) {
                        如果 (!something)
                                返回 -EINVAL;

                        break;
                }

                如果 (*p == '-') /* Don't allow "-0" */
                        返回 -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                如果 (errno > 0)
                        返回 -errno;
                如果 (l < 0)
                        返回 -ERANGE;

                如果 (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } 否则 如果 (e == p)
                        返回 -EINVAL;
                否则
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                如果 (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        返回 -EINVAL;

                p = s;

                如果 ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        返回 -ERANGE;

                k = (nsec_t) l * multiplier;
                如果 (k >= NSEC_INFINITY - nsec)
                        返回 -ERANGE;

                nsec += k;

                something = true;

                如果 (*e == '.') {
                        nsec_t m = multiplier / 10;
                        const char *b;

                        为 (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                如果 (k >= NSEC_INFINITY - nsec)
                                        返回 -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        如果 (b == e + 1)
                                返回 -EINVAL;
                }
        }

        *ret = nsec;

        返回 0;
}

static 整数 get_timezones_from_zone1970_tab(char ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_strv_free_ char **zones = NULL;
        整数 r;

        assert(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        如果 (!f)
                返回 -errno;

        为 (;;) {
                _cleanup_free_ char *line = NULL, *cc = NULL, *co = NULL, *tz = NULL;

                r = read_line(f, LONG_LINE_MAX, &line);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;

                const char *p = line;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_words(&p, NULL, 0, &cc, &co, &tz);
                如果 (r < 0)
                        continue;

                /* Lines that start with # are comments. */
                如果 (*cc == '#')
                        continue;

                如果 (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        continue;

                r = strv_extend(&zones, tz);
                如果 (r < 0)
                        返回 r;
        }

        *ret = TAKE_PTR(zones);
        返回 0;
}

static 整数 get_timezones_from_tzdata_zi(char ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_strv_free_ char **zones = NULL;
        整数 r;

        assert(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        如果 (!f)
                返回 -errno;

        为 (;;) {
                _cleanup_free_ char *line = NULL, *type = NULL, *f1 = NULL, *f2 = NULL;

                r = read_line(f, LONG_LINE_MAX, &line);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;

                const char *p = line;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_words(&p, NULL, 0, &type, &f1, &f2);
                如果 (r < 0)
                        continue;

                char *tz;
                如果 (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                否则 如果 (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                否则
                        /* Not a line we care about. */
                        continue;

                如果 (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        continue;

                r = strv_extend(&zones, tz);
                如果 (r < 0)
                        返回 r;
        }

        *ret = TAKE_PTR(zones);
        返回 0;
}

整数 get_timezones(char ***ret) {
        _cleanup_strv_free_ char **zones = NULL;
        整数 r;

        assert(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        如果 (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                如果 (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        如果 (r < 0 && r != -ENOENT)
                返回 r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        如果 (r < 0)
                返回 r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        返回 0;
}

整数 verify_timezone(const char *name, 整数 log_level) {
        bool slash = false;
        const char *p, *t;
        _cleanup_close_ 整数 fd = -EBADF;
        char buf[4];
        整数 r;

        如果 (isempty(name))
                返回 -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        如果 (streq(name, "UTC"))
                返回 0;

        如果 (name[0] == '/')
                返回 -EINVAL;

        为 (p = name; *p; p++) {
                如果 (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        返回 -EINVAL;

                如果 (*p == '/') {

                        如果 (slash)
                                返回 -EINVAL;

                        slash = true;
                } 否则
                        slash = false;
        }

        如果 (slash)
                返回 -EINVAL;

        如果 (p - name >= PATH_MAX)
                返回 -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        如果 (fd < 0)
                返回 log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        如果 (r < 0)
                返回 log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        如果 (r < 0)
                返回 log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        如果 (memcmp(buf, "TZif", 4) != 0)
                返回 log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        返回 0;
}

void reset_timezonep(char **p) {
        assert(p);

        (void) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

char* save_timezone(void) {
        const char *e = getenv("TZ");
        如果 (!e)
                返回 NULL;

        char *s = strdup(e);
        如果 (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        返回 s;
}

bool clock_supported(clockid_t clock) {
        struct timespec ts;

        switch (clock) {

        case CLOCK_MONOTONIC:
        case CLOCK_REALTIME:
        case CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                返回 true;

        default:
                /* For everything else, check properly */
                返回 clock_gettime(clock, &ts) >= 0;
        }
}

整数 get_timezone(char **ret) {
        _cleanup_free_ char *t = NULL;
        整数 r;

        assert(ret);

        r = readlink_malloc(etc_localtime(), &t);
        如果 (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                返回 strdup_to(ret, "UTC");
        如果 (r < 0)
                返回 r; /* Return EINVAL if not a symlink */

        const char *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        如果 (!e)
                返回 -EINVAL;
        如果 (!timezone_is_valid(e, LOG_DEBUG))
                返回 -EINVAL;

        返回 strdup_to(ret, e);
}

整数 get_timezone_prefer_env(char **ret) {
        assert(ret);

        const char *e = getenv("TZ");
        如果 (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                返回 strdup_to(ret, e + 1);

        返回 get_timezone(ret);
}

const char* etc_localtime(void) {
        static const char *cached = NULL;

        如果 (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        返回 cached;
}

整数 mktime_or_timegm_usec(
                struct tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        assert(tm);

        如果 (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                返回 -ERANGE;
        如果 ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                返回 -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        如果 (t < 0) /* Refuse negative times and errors */
                返回 -ERANGE;
        如果 ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                返回 -ERANGE;

        如果 (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        返回 0;
}

整数 localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                struct tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        如果 (t > (usec_t) TIME_T_MAX)
                返回 -ERANGE;
        time_t sec = (time_t) t;

        struct tm buf = {};
        如果 (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                返回 -EINVAL;

        如果 (ret)
                *ret = buf;

        返回 0;
}

uint64_t sysconf_clock_ticks_cached(void) {
        static thread_local uint64_t hz = 0;

        如果 (hz != 0)
                返回 hz;

        long t = sysconf(_SC_CLK_TCK);
        assert(t > 0);
        hz = (uint64_t) t;

        返回 hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        返回 DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        返回 DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        如果 (x == USEC_INFINITY)
                返回 USEC_INFINITY;
        如果 (map_clock_id(from) == map_clock_id(to))
                返回 x;

        a = now(from);
        b = now(to);

        如果 (x > a)
                /* x lies in the future */
                返回 usec_add(b, usec_sub_unsigned(x, a));
        否则
                /* x lies in the past */
                返回 usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(void) {
        tzset();

        返回 timezone == 0 && daylight == 0;
}

整数 usleep_safe(usec_t usec) {
        整数 r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        如果 (usec == 0)
                返回 0;

        如果 (usec == USEC_INFINITY)
                返回 RET_NERRNO(pause());

        struct timespec t;
        timespec_store(&t, usec);

        为 (;;) {
                struct timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                如果 (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        continue;
                }

                返回 r;
        }
}

整数 time_change_fd(void) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        static const struct itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ 整数 fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        如果 (fd < 0)
                返回 -errno;

        如果 (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, NULL) >= 0)
                返回 TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#如果 SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        如果 (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                static const struct itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                如果 (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, NULL) >= 0)
                        返回 TAKE_FD(fd);
        }
#endif

        返回 -errno;
}

static const char* const timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(const char *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        如果 (t >= 0)
                返回 t;
        如果 (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                返回 TIMESTAMP_US;
        如果 (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                返回 TIMESTAMP_US_UTC;
        返回 t;
}

整数 parse_calendar_date_full(const char *s, bool allow_pre_epoch, usec_t *ret_usec, struct tm *ret_tm) {
        struct tm parsed_tm = {}, copy_tm;
        const char *k;
        整数 r;

        assert(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        如果 (!k || *k)
                返回 -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        如果 (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                如果 (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        返回 -EINVAL;
        } 否则 {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                如果 (r < 0)
                        返回 r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        如果 (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                返回 -EINVAL;

        如果 (ret_usec)
                *ret_usec = usec;
        如果 (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        返回 0;
}
