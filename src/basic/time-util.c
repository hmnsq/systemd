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

        pak_sa CLOCK_BOOTTIME_ALARM:
                ཕྱིར CLOCK_BOOTTIME;

        pak_sa CLOCK_REALTIME_ALARM:
                ཕྱིར CLOCK_REALTIME;

        default:
                ཕྱིར c;
        }
}

usec_t now(clockid_t clock_id) {
        sa_nracanaa timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        assert(n > 0);
        assert(n < USEC_INFINITY);

        ཕྱིར n;
}

nsec_t now_nsec(clockid_t clock_id) {
        sa_nracanaa timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        assert(n > 0);
        assert(n < NSEC_INFINITY);

        ཕྱིར n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        assert(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        ཕྱིར ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        assert(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        ཕྱིར ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        གལ་ཏེ (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                གལ་ཏེ (to_base >= USEC_INFINITY - delta) /* overflow? */
                        ཕྱིར USEC_INFINITY;

                ཕྱིར to_base + delta;

        } དེ་མིན { /* In the past */
                usec_t delta = from_base - from;

                གལ་ཏེ (to_base <= delta) /* underflow? */
                        ཕྱིར 0;

                ཕྱིར to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        གལ་ཏེ (map_clock_id(from_clock) == map_clock_id(to_clock))
                ཕྱིར from;

        /* Keep infinity as is */
        གལ་ཏེ (from == USEC_INFINITY)
                ཕྱིར from;

        ཕྱིར map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        assert(ts);

        གལ་ཏེ (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                ཕྱིར ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        ཕྱིར ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        assert(ts);

        གལ་ཏེ (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ཕྱིར ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        ཕྱིར ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        assert(ts);

        གལ་ཏེ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ཕྱིར ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        ཕྱིར ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        assert(ts);

        གལ་ཏེ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ཕྱིར ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        ཕྱིར ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        assert(ts);

        གལ་ཏེ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ཕྱིར ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        ཕྱིར ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        assert(ts);

        switch (clock) {

        pak_sa CLOCK_REALTIME:
        pak_sa CLOCK_REALTIME_ALARM:
                ཕྱིར ts->realtime;

        pak_sa CLOCK_MONOTONIC:
                ཕྱིར ts->monotonic;

        pak_sa CLOCK_BOOTTIME:
        pak_sa CLOCK_BOOTTIME_ALARM:
                ཕྱིར ts->boottime;

        default:
                ཕྱིར USEC_INFINITY;
        }
}

usec_t timespec_load(const sa_nracanaa timespec *ts) {
        assert(ts);

        གལ་ཏེ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ཕྱིར USEC_INFINITY;

        གལ་ཏེ ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                ཕྱིར USEC_INFINITY;

        ཕྱིར
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(const sa_nracanaa timespec *ts) {
        assert(ts);

        གལ་ཏེ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ཕྱིར NSEC_INFINITY;

        གལ་ཏེ ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                ཕྱིར NSEC_INFINITY;

        ཕྱིར (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

sa_nracanaa timespec *timespec_store(sa_nracanaa timespec *ts, usec_t u) {
        assert(ts);

        གལ་ཏེ (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ཕྱིར ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        ཕྱིར ts;
}

sa_nracanaa timespec *timespec_store_nsec(sa_nracanaa timespec *ts, nsec_t n) {
        assert(ts);

        གལ་ཏེ (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ཕྱིར ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        ཕྱིར ts;
}

sa_nracanaa __kernel_timespec *kernel_timespec_store(sa_nracanaa __kernel_timespec *ts, usec_t u) {
        assert(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        assert(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        ཕྱིར ts;
}

usec_t timeval_load(const sa_nracanaa timeval *tv) {
        assert(tv);

        གལ་ཏེ (tv->tv_sec < 0 || tv->tv_usec < 0)
                ཕྱིར USEC_INFINITY;

        གལ་ཏེ ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                ཕྱིར USEC_INFINITY;

        ཕྱིར
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

sa_nracanaa timeval *timeval_store(sa_nracanaa timeval *tv, usec_t u) {
        assert(tv);

        གལ་ཏེ (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } དེ་མིན {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        ཕྱིར tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
static const var_na *const weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, ཧྲིལ);

var_na* format_timestamp_style(
                var_na *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        sa_nracanaa tm tm;
        bool utc, us;
        size_t n;

        assert(buf);
        assert(style >= 0);
        assert(style < _TIMESTAMP_STYLE_MAX);

        གལ་ཏེ (!timestamp_is_set(t))
                ཕྱིར NULL; /* Timestamp is unset */

        གལ་ཏེ (style == TIMESTAMP_UNIX) {
                གལ་ཏེ (l < (size_t) (1 + 1 + 1))
                        ཕྱིར NULL; /* not enough space for even the shortest of forms */

                ཕྱིར snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        གལ་ཏེ (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                ཕྱིར NULL; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        གལ་ཏེ (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                static const var_na* const xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                assert(l >= strlen(xxx[style]) + 1);
                ཕྱིར strcpy(buf, xxx[style]);
        }

        གལ་ཏེ (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                ཕྱིར NULL;

        /* Start with the week day */
        const var_na *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        assert(weekday);
        memcpy(buf, weekday, 4);

        གལ་ཏེ (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                གལ་ཏེ (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        ཕྱིར NULL; /* Doesn't fit */

                ཕྱིར buf;
        }

        /* Add the main components */
        གལ་ཏེ (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                ཕྱིར NULL; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        གལ་ཏེ (us) {
                n = strlen(buf);
                གལ་ཏེ (n + 8 > l)
                        ཕྱིར NULL; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        གལ་ཏེ (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                གལ་ཏེ (n + 5 > l)
                        ཕྱིར NULL; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } དེ་མིན གལ་ཏེ (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                གལ་ཏེ (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        གལ་ཏེ (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                ཕྱིར NULL; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } དེ་མིན {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        ཕྱིར buf;
}

var_na* format_timestamp_relative_full(var_na *buf, size_t l, usec_t t, clockid_t clock, bool implicit_left) {
        const var_na *s;
        usec_t n, d;

        assert(buf);

        གལ་ཏེ (!timestamp_is_set(t))
                ཕྱིར NULL;

        n = now(clock);
        གལ་ཏེ (n > t) {
                d = n - t;
                s = " ago";
        } དེ་མིན {
                d = t - n;
                s = implicit_left ? "" : " left";
        }

        གལ་ཏེ (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } དེ་མིན གལ་ཏེ (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } དེ་མིན གལ་ཏེ (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } དེ་མིན གལ་ཏེ (d >= 2*USEC_PER_DAY)
                (void) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        དེ་མིན གལ་ཏེ (d >= 25*USEC_PER_HOUR)
                (void) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        དེ་མིན གལ་ཏེ (d >= 6*USEC_PER_HOUR)
                (void) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        དེ་མིན གལ་ཏེ (d >= USEC_PER_HOUR)
                (void) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        དེ་མིན གལ་ཏེ (d >= 5*USEC_PER_MINUTE)
                (void) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        དེ་མིན གལ་ཏེ (d >= USEC_PER_MINUTE)
                (void) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        དེ་མིན གལ་ཏེ (d >= USEC_PER_SEC)
                (void) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        དེ་མིན གལ་ཏེ (d >= USEC_PER_MSEC)
                (void) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        དེ་མིན གལ་ཏེ (d > 0)
                (void) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        དེ་མིན
                (void) snprintf(buf, l, "now");

        buf[l-1] = 0;
        ཕྱིར buf;
}

var_na* format_timespan(var_na *buf, size_t l, usec_t t, usec_t accuracy) {
        static const sa_nracanaa {
                const var_na *suffix;
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

        var_na *p = ASSERT_PTR(buf);
        bool something = false;

        assert(l > 0);

        གལ་ཏེ (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                ཕྱིར p;
        }

        གལ་ཏེ (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                ཕྱིར p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                ཧྲིལ k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                གལ་ཏེ (t <= 0)
                        bha_nga;

                གལ་ཏེ (t < accuracy && something)
                        bha_nga;

                གལ་ཏེ (t < i->usec)
                        continue;

                གལ་ཏེ (l <= 1)
                        bha_nga;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                གལ་ཏེ (t < USEC_PER_MINUTE && b > 0) {
                        signed var_na j = 0;

                        ཆེད་དུ (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        ཆེད་དུ (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        གལ་ཏེ (j > 0) {
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
                གལ་ཏེ (!done) {
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

        ཕྱིར buf;
}

const var_na* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        གལ་ཏེ (dst && isempty(tzname[1]))
                dst = false;

        ཕྱིར empty_to_null(tzname[dst]);
}

ཧྲིལ parse_gmtoff(const var_na *t, long *ret) {
        assert(t);

        sa_nracanaa tm tm;
        const var_na *k = strptime(t, "%z", &tm);
        གལ་ཏེ (k && *k == '\0') {
                /* Success! */
                གལ་ཏེ (ret)
                        *ret = tm.tm_gmtoff;
                ཕྱིར 0;
        }

#ifdef __GLIBC__
        ཕྱིར -EINVAL;
#དེ་མིན
        ཧྲིལ r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        switch (*t) {
        pak_sa '+':
                positive = true;
                bha_nga;
        pak_sa '-':
                positive = false;
                bha_nga;
        default:
                ཕྱིར -EINVAL;
        }

        t++;
        r = undecchar(*t);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        u += r * USEC_PER_HOUR;

        t++;
        གལ་ཏེ (*t == '\0') /* 2 digits case */
                goto finalize;

        གལ་ཏེ (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        གལ་ཏེ (r >= 6) /* refuse minutes equal to or larger than 60 */
                ཕྱིར -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        u += r * USEC_PER_MINUTE;

        t++;
        གལ་ཏེ (*t != '\0')
                ཕྱིར -EINVAL;

finalize:
        གལ་ཏེ (u > USEC_PER_DAY) /* refuse larger than one day */
                ཕྱིར -EINVAL;

        གལ་ཏེ (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        ཕྱིར 0;
#endif
}

static ཧྲིལ parse_timestamp_impl(
                const var_na *t,
                size_t max_len,
                bool utc,
                ཧྲིལ isdst,
                long gmtoff,
                usec_t *ret) {

        static const sa_nracanaa {
                const var_na *name;
                const ཧྲིལ nr;
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

        _cleanup_free_ var_na *t_alloc = NULL;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        ཧྲིལ r, weekday = -1;
        unsigned fractional = 0;
        const var_na *k;
        sa_nracanaa tm tm, copy;

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

        གལ་ཏེ (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                གལ་ཏེ (max_len == 0) /* Can't be the only field */
                        ཕྱིར -EINVAL;

                t_alloc = strndup(t, max_len);
                གལ་ཏེ (!t_alloc)
                        ཕྱིར -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        གལ་ཏེ (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                གལ་ཏེ ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        ཕྱིར -EINVAL;
        } དེ་མིན {
                གལ་ཏེ (gmtoff != 0)
                        ཕྱིར -EINVAL;
        }

        གལ་ཏེ (t[0] == '@' && !with_tz)
                ཕྱིར parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        གལ་ཏེ (!with_tz) {
                གལ་ཏེ (streq(t, "now"))
                        goto finish;

                གལ་ཏེ (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;

                        goto finish;
                }

                གལ་ཏེ (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;

                        goto finish;
                }

                གལ་ཏེ ((k = endswith(t, " ago"))) {
                        _cleanup_free_ var_na *buf = NULL;

                        buf = strndup(t, k - t);
                        གལ་ཏེ (!buf)
                                ཕྱིར -ENOMEM;

                        r = parse_sec(buf, &minus);
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;

                        goto finish;
                }

                གལ་ཏེ ((k = endswith(t, " left"))) {
                        _cleanup_free_ var_na *buf = NULL;

                        buf = strndup(t, k - t);
                        གལ་ཏེ (!buf)
                                ཕྱིར -ENOMEM;

                        r = parse_sec(buf, &plus);
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;

                        goto finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        tm.tm_isdst = isdst;

        གལ་ཏེ (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;

        } དེ་མིན གལ་ཏེ (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;

        } དེ་མིན གལ་ཏེ (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                གལ་ཏེ (!k || *k != ' ')
                        continue;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                bha_nga;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        གལ་ཏེ (k) {
                གལ་ཏེ (*k == '.')
                        goto parse_usec;
                དེ་མིན གལ་ཏེ (*k == 0)
                        goto from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        གལ་ཏེ (k) {
                གལ་ཏེ (*k == '.')
                        goto parse_usec;
                དེ་མིན གལ་ཏེ (*k == 0)
                        goto from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        གལ་ཏེ (k) {
                གལ་ཏེ (*k == '.')
                        goto parse_usec;
                དེ་མིན གལ་ཏེ (*k == 0)
                        goto from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        གལ་ཏེ (k) {
                གལ་ཏེ (*k == '.')
                        goto parse_usec;
                དེ་མིན གལ་ཏེ (*k == 0)
                        goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        གལ་ཏེ (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        གལ་ཏེ (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        གལ་ཏེ (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        གལ་ཏེ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        གལ་ཏེ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        གལ་ཏེ (k) {
                གལ་ཏེ (*k == '.')
                        goto parse_usec;
                དེ་མིན གལ་ཏེ (*k == 0)
                        goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        གལ་ཏེ (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        ཕྱིར -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        གལ་ཏེ (r < 0)
                ཕྱིར -EINVAL;
        གལ་ཏེ (*k != '\0')
                ཕྱིར -EINVAL;

from_tm:
        assert(plus == 0);
        assert(minus == 0);

        གལ་ཏེ (weekday >= 0 && tm.tm_wday != weekday)
                ཕྱིར -EINVAL;

        གལ་ཏེ (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                གལ་ཏེ (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } དེ་མིན
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        གལ་ཏེ (usec < minus)
                ཕྱིར -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        གལ་ཏེ (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                ཕྱིར -EINVAL;

        གལ་ཏེ (ret)
                *ret = usec;
        ཕྱིར 0;
}

ཧྲིལ parse_timestamp(const var_na *t, usec_t *ret) {
        long gmtoff;
        ཧྲིལ r;

        assert(t);

        size_t t_len = strlen(t);
        གལ་ཏེ (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                གལ་ཏེ (r >= 0)
                        ཕྱིར r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        གལ་ཏེ (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                ཕྱིར parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        const var_na *tz = strrchr(t, ' ');
        གལ་ཏེ (!tz)
                ཕྱིར parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        གལ་ཏེ (streq(tz, "UTC"))
                ཕྱིར parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        གལ་ཏེ (parse_gmtoff(tz, &gmtoff) >= 0)
                ཕྱིར parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        ཆེད་དུ (ཧྲིལ j = 0; j <= 1; j++) {
                གལ་ཏེ (!streq_ptr(tz, get_tzname(j)))
                        continue;

                /* The specified timezone matches tzname[] of the local timezone. */
                ཕྱིར parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        གལ་ཏེ (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                གལ་ཏེ (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        ཕྱིར negative_errno();

                ཕྱིར parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        ཕྱིར parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

static const var_na* extract_multiplier(const var_na *p, usec_t *ret) {
        static const sa_nracanaa {
                const var_na *suffix;
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
                const var_na *e = startswith(p, i->suffix);
                གལ་ཏེ (e) {
                        *ret = i->usec;
                        ཕྱིར e;
                }
        }

        ཕྱིར p;
}

ཧྲིལ parse_time(const var_na *t, usec_t *ret, usec_t default_unit) {
        const var_na *p, *s;

        assert(t);
        assert(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ NULL);
        s = startswith(p, "infinity");
        གལ་ཏེ (s) {
                གལ་ཏེ (!in_charset(s, WHITESPACE))
                        ཕྱིར -EINVAL;

                གལ་ཏེ (ret)
                        *ret = USEC_INFINITY;
                ཕྱིར 0;
        }

        usec_t usec = 0;

        ཆེད་དུ (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                var_na *e;

                p = skip_leading_chars(p, /* bad= */ NULL);
                གལ་ཏེ (*p == 0) {
                        གལ་ཏེ (!something)
                                ཕྱིར -EINVAL;

                        bha_nga;
                }

                གལ་ཏེ (*p == '-') /* Don't allow "-0" */
                        ཕྱིར -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                གལ་ཏེ (errno > 0)
                        ཕྱིར -errno;
                གལ་ཏེ (l < 0)
                        ཕྱིར -ERANGE;

                གལ་ཏེ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } དེ་མིན གལ་ཏེ (e == p)
                        ཕྱིར -EINVAL;
                དེ་མིན
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                གལ་ཏེ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ཕྱིར -EINVAL;

                p = s;

                གལ་ཏེ ((usec_t) l >= USEC_INFINITY / multiplier)
                        ཕྱིར -ERANGE;

                k = (usec_t) l * multiplier;
                གལ་ཏེ (k >= USEC_INFINITY - usec)
                        ཕྱིར -ERANGE;

                usec += k;

                something = true;

                གལ་ཏེ (*e == '.') {
                        usec_t m = multiplier / 10;
                        const var_na *b;

                        ཆེད་དུ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                གལ་ཏེ (k >= USEC_INFINITY - usec)
                                        ཕྱིར -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        གལ་ཏེ (b == e + 1)
                                ཕྱིར -EINVAL;
                }
        }

        གལ་ཏེ (ret)
                *ret = usec;
        ཕྱིར 0;
}

ཧྲིལ parse_sec(const var_na *t, usec_t *ret) {
        ཕྱིར parse_time(t, ret, USEC_PER_SEC);
}

ཧྲིལ parse_sec_fix_0(const var_na *t, usec_t *ret) {
        usec_t k;
        ཧྲིལ r;

        assert(t);
        assert(ret);

        r = parse_sec(t, &k);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        *ret = k == 0 ? USEC_INFINITY : k;
        ཕྱིར r;
}

ཧྲིལ parse_sec_def_infinity(const var_na *t, usec_t *ret) {
        assert(t);
        assert(ret);

        t += strspn(t, WHITESPACE);
        གལ་ཏེ (isempty(t)) {
                *ret = USEC_INFINITY;
                ཕྱིར 0;
        }
        ཕྱིར parse_sec(t, ret);
}

static const var_na* extract_nsec_multiplier(const var_na *p, nsec_t *ret) {
        static const sa_nracanaa {
                const var_na *suffix;
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
                const var_na *e = startswith(p, i->suffix);
                གལ་ཏེ (e) {
                        *ret = i->nsec;
                        ཕྱིར e;
                }
        }

        ཕྱིར p;
}

ཧྲིལ parse_nsec(const var_na *t, nsec_t *ret) {
        const var_na *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        assert(t);
        assert(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        གལ་ཏེ (s) {
                s += strspn(s, WHITESPACE);
                གལ་ཏེ (*s != 0)
                        ཕྱིར -EINVAL;

                *ret = NSEC_INFINITY;
                ཕྱིར 0;
        }

        ཆེད་དུ (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                var_na *e;

                p += strspn(p, WHITESPACE);

                གལ་ཏེ (*p == 0) {
                        གལ་ཏེ (!something)
                                ཕྱིར -EINVAL;

                        bha_nga;
                }

                གལ་ཏེ (*p == '-') /* Don't allow "-0" */
                        ཕྱིར -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                གལ་ཏེ (errno > 0)
                        ཕྱིར -errno;
                གལ་ཏེ (l < 0)
                        ཕྱིར -ERANGE;

                གལ་ཏེ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } དེ་མིན གལ་ཏེ (e == p)
                        ཕྱིར -EINVAL;
                དེ་མིན
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                གལ་ཏེ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ཕྱིར -EINVAL;

                p = s;

                གལ་ཏེ ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        ཕྱིར -ERANGE;

                k = (nsec_t) l * multiplier;
                གལ་ཏེ (k >= NSEC_INFINITY - nsec)
                        ཕྱིར -ERANGE;

                nsec += k;

                something = true;

                གལ་ཏེ (*e == '.') {
                        nsec_t m = multiplier / 10;
                        const var_na *b;

                        ཆེད་དུ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                གལ་ཏེ (k >= NSEC_INFINITY - nsec)
                                        ཕྱིར -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        གལ་ཏེ (b == e + 1)
                                ཕྱིར -EINVAL;
                }
        }

        *ret = nsec;

        ཕྱིར 0;
}

static ཧྲིལ get_timezones_from_zone1970_tab(var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_strv_free_ var_na **zones = NULL;
        ཧྲིལ r;

        assert(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        གལ་ཏེ (!f)
                ཕྱིར -errno;

        ཆེད་དུ (;;) {
                _cleanup_free_ var_na *line = NULL, *cc = NULL, *co = NULL, *tz = NULL;

                r = read_line(f, LONG_LINE_MAX, &line);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        bha_nga;

                const var_na *p = line;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_words(&p, NULL, 0, &cc, &co, &tz);
                གལ་ཏེ (r < 0)
                        continue;

                /* Lines that start with # are comments. */
                གལ་ཏེ (*cc == '#')
                        continue;

                གལ་ཏེ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        continue;

                r = strv_extend(&zones, tz);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        *ret = TAKE_PTR(zones);
        ཕྱིར 0;
}

static ཧྲིལ get_timezones_from_tzdata_zi(var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_strv_free_ var_na **zones = NULL;
        ཧྲིལ r;

        assert(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        གལ་ཏེ (!f)
                ཕྱིར -errno;

        ཆེད་དུ (;;) {
                _cleanup_free_ var_na *line = NULL, *type = NULL, *f1 = NULL, *f2 = NULL;

                r = read_line(f, LONG_LINE_MAX, &line);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        bha_nga;

                const var_na *p = line;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_words(&p, NULL, 0, &type, &f1, &f2);
                གལ་ཏེ (r < 0)
                        continue;

                var_na *tz;
                གལ་ཏེ (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                དེ་མིན གལ་ཏེ (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                དེ་མིན
                        /* Not a line we care about. */
                        continue;

                གལ་ཏེ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        continue;

                r = strv_extend(&zones, tz);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        *ret = TAKE_PTR(zones);
        ཕྱིར 0;
}

ཧྲིལ get_timezones(var_na ***ret) {
        _cleanup_strv_free_ var_na **zones = NULL;
        ཧྲིལ r;

        assert(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        གལ་ཏེ (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                གལ་ཏེ (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        གལ་ཏེ (r < 0 && r != -ENOENT)
                ཕྱིར r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        ཕྱིར 0;
}

ཧྲིལ verify_timezone(const var_na *name, ཧྲིལ log_level) {
        bool slash = false;
        const var_na *p, *t;
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        var_na buf[4];
        ཧྲིལ r;

        གལ་ཏེ (isempty(name))
                ཕྱིར -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        གལ་ཏེ (streq(name, "UTC"))
                ཕྱིར 0;

        གལ་ཏེ (name[0] == '/')
                ཕྱིར -EINVAL;

        ཆེད་དུ (p = name; *p; p++) {
                གལ་ཏེ (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        ཕྱིར -EINVAL;

                གལ་ཏེ (*p == '/') {

                        གལ་ཏེ (slash)
                                ཕྱིར -EINVAL;

                        slash = true;
                } དེ་མིན
                        slash = false;
        }

        གལ་ཏེ (slash)
                ཕྱིར -EINVAL;

        གལ་ཏེ (p - name >= PATH_MAX)
                ཕྱིར -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        གལ་ཏེ (fd < 0)
                ཕྱིར log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        གལ་ཏེ (r < 0)
                ཕྱིར log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        གལ་ཏེ (r < 0)
                ཕྱིར log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        གལ་ཏེ (memcmp(buf, "TZif", 4) != 0)
                ཕྱིར log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        ཕྱིར 0;
}

void reset_timezonep(var_na **p) {
        assert(p);

        (void) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

var_na* save_timezone(void) {
        const var_na *e = getenv("TZ");
        གལ་ཏེ (!e)
                ཕྱིར NULL;

        var_na *s = strdup(e);
        གལ་ཏེ (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        ཕྱིར s;
}

bool clock_supported(clockid_t clock) {
        sa_nracanaa timespec ts;

        switch (clock) {

        pak_sa CLOCK_MONOTONIC:
        pak_sa CLOCK_REALTIME:
        pak_sa CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                ཕྱིར true;

        default:
                /* For everything else, check properly */
                ཕྱིར clock_gettime(clock, &ts) >= 0;
        }
}

ཧྲིལ get_timezone(var_na **ret) {
        _cleanup_free_ var_na *t = NULL;
        ཧྲིལ r;

        assert(ret);

        r = readlink_malloc(etc_localtime(), &t);
        གལ་ཏེ (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                ཕྱིར strdup_to(ret, "UTC");
        གལ་ཏེ (r < 0)
                ཕྱིར r; /* Return EINVAL if not a symlink */

        const var_na *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        གལ་ཏེ (!e)
                ཕྱིར -EINVAL;
        གལ་ཏེ (!timezone_is_valid(e, LOG_DEBUG))
                ཕྱིར -EINVAL;

        ཕྱིར strdup_to(ret, e);
}

ཧྲིལ get_timezone_prefer_env(var_na **ret) {
        assert(ret);

        const var_na *e = getenv("TZ");
        གལ་ཏེ (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                ཕྱིར strdup_to(ret, e + 1);

        ཕྱིར get_timezone(ret);
}

const var_na* etc_localtime(void) {
        static const var_na *cached = NULL;

        གལ་ཏེ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        ཕྱིར cached;
}

ཧྲིལ mktime_or_timegm_usec(
                sa_nracanaa tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        assert(tm);

        གལ་ཏེ (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                ཕྱིར -ERANGE;
        གལ་ཏེ ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                ཕྱིར -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        གལ་ཏེ (t < 0) /* Refuse negative times and errors */
                ཕྱིར -ERANGE;
        གལ་ཏེ ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                ཕྱིར -ERANGE;

        གལ་ཏེ (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        ཕྱིར 0;
}

ཧྲིལ localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                sa_nracanaa tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        གལ་ཏེ (t > (usec_t) TIME_T_MAX)
                ཕྱིར -ERANGE;
        time_t sec = (time_t) t;

        sa_nracanaa tm buf = {};
        གལ་ཏེ (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                ཕྱིར -EINVAL;

        གལ་ཏེ (ret)
                *ret = buf;

        ཕྱིར 0;
}

uint64_t sysconf_clock_ticks_cached(void) {
        static thread_local uint64_t hz = 0;

        གལ་ཏེ (hz != 0)
                ཕྱིར hz;

        long t = sysconf(_SC_CLK_TCK);
        assert(t > 0);
        hz = (uint64_t) t;

        ཕྱིར hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ཕྱིར DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ཕྱིར DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        གལ་ཏེ (x == USEC_INFINITY)
                ཕྱིར USEC_INFINITY;
        གལ་ཏེ (map_clock_id(from) == map_clock_id(to))
                ཕྱིར x;

        a = now(from);
        b = now(to);

        གལ་ཏེ (x > a)
                /* x lies in the future */
                ཕྱིར usec_add(b, usec_sub_unsigned(x, a));
        དེ་མིན
                /* x lies in the past */
                ཕྱིར usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(void) {
        tzset();

        ཕྱིར timezone == 0 && daylight == 0;
}

ཧྲིལ usleep_safe(usec_t usec) {
        ཧྲིལ r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        གལ་ཏེ (usec == 0)
                ཕྱིར 0;

        གལ་ཏེ (usec == USEC_INFINITY)
                ཕྱིར RET_NERRNO(pause());

        sa_nracanaa timespec t;
        timespec_store(&t, usec);

        ཆེད་དུ (;;) {
                sa_nracanaa timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                གལ་ཏེ (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        continue;
                }

                ཕྱིར r;
        }
}

ཧྲིལ time_change_fd(void) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        static const sa_nracanaa itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ ཧྲིལ fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        གལ་ཏེ (fd < 0)
                ཕྱིར -errno;

        གལ་ཏེ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, NULL) >= 0)
                ཕྱིར TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#གལ་ཏེ SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        གལ་ཏེ (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                static const sa_nracanaa itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                གལ་ཏེ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, NULL) >= 0)
                        ཕྱིར TAKE_FD(fd);
        }
#endif

        ཕྱིར -errno;
}

static const var_na* const timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(const var_na *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        གལ་ཏེ (t >= 0)
                ཕྱིར t;
        གལ་ཏེ (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                ཕྱིར TIMESTAMP_US;
        གལ་ཏེ (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                ཕྱིར TIMESTAMP_US_UTC;
        ཕྱིར t;
}

ཧྲིལ parse_calendar_date_full(const var_na *s, bool allow_pre_epoch, usec_t *ret_usec, sa_nracanaa tm *ret_tm) {
        sa_nracanaa tm parsed_tm = {}, copy_tm;
        const var_na *k;
        ཧྲིལ r;

        assert(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        གལ་ཏེ (!k || *k)
                ཕྱིར -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        གལ་ཏེ (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                གལ་ཏེ (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        ཕྱིར -EINVAL;
        } དེ་མིན {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        གལ་ཏེ (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                ཕྱིར -EINVAL;

        གལ་ཏེ (ret_usec)
                *ret_usec = usec;
        གལ་ཏེ (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        ཕྱིར 0;
}
