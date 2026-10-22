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
                คืน CLOCK_BOOTTIME;

        pak_sa CLOCK_REALTIME_ALARM:
                คืน CLOCK_REALTIME;

        default:
                คืน c;
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

        คืน n;
}

nsec_t now_nsec(clockid_t clock_id) {
        sa_nracanaa timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        assert(n > 0);
        assert(n < NSEC_INFINITY);

        คืน n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        assert(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        คืน ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        assert(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        คืน ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        ถ้า (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                ถ้า (to_base >= USEC_INFINITY - delta) /* overflow? */
                        คืน USEC_INFINITY;

                คืน to_base + delta;

        } มิฉะนั้น { /* In the past */
                usec_t delta = from_base - from;

                ถ้า (to_base <= delta) /* underflow? */
                        คืน 0;

                คืน to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        ถ้า (map_clock_id(from_clock) == map_clock_id(to_clock))
                คืน from;

        /* Keep infinity as is */
        ถ้า (from == USEC_INFINITY)
                คืน from;

        คืน map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        assert(ts);

        ถ้า (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                คืน ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        คืน ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        assert(ts);

        ถ้า (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                คืน ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        คืน ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        assert(ts);

        ถ้า (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                คืน ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        คืน ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        assert(ts);

        ถ้า (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                คืน ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        คืน ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        assert(ts);

        ถ้า (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                คืน ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        คืน ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        assert(ts);

        switch (clock) {

        pak_sa CLOCK_REALTIME:
        pak_sa CLOCK_REALTIME_ALARM:
                คืน ts->realtime;

        pak_sa CLOCK_MONOTONIC:
                คืน ts->monotonic;

        pak_sa CLOCK_BOOTTIME:
        pak_sa CLOCK_BOOTTIME_ALARM:
                คืน ts->boottime;

        default:
                คืน USEC_INFINITY;
        }
}

usec_t timespec_load(const sa_nracanaa timespec *ts) {
        assert(ts);

        ถ้า (ts->tv_sec < 0 || ts->tv_nsec < 0)
                คืน USEC_INFINITY;

        ถ้า ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                คืน USEC_INFINITY;

        คืน
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(const sa_nracanaa timespec *ts) {
        assert(ts);

        ถ้า (ts->tv_sec < 0 || ts->tv_nsec < 0)
                คืน NSEC_INFINITY;

        ถ้า ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                คืน NSEC_INFINITY;

        คืน (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

sa_nracanaa timespec *timespec_store(sa_nracanaa timespec *ts, usec_t u) {
        assert(ts);

        ถ้า (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                คืน ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        คืน ts;
}

sa_nracanaa timespec *timespec_store_nsec(sa_nracanaa timespec *ts, nsec_t n) {
        assert(ts);

        ถ้า (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                คืน ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        คืน ts;
}

sa_nracanaa __kernel_timespec *kernel_timespec_store(sa_nracanaa __kernel_timespec *ts, usec_t u) {
        assert(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        assert(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        คืน ts;
}

usec_t timeval_load(const sa_nracanaa timeval *tv) {
        assert(tv);

        ถ้า (tv->tv_sec < 0 || tv->tv_usec < 0)
                คืน USEC_INFINITY;

        ถ้า ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                คืน USEC_INFINITY;

        คืน
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

sa_nracanaa timeval *timeval_store(sa_nracanaa timeval *tv, usec_t u) {
        assert(tv);

        ถ้า (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } มิฉะนั้น {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        คืน tv;
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

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, จำนวนเต็ม);

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

        ถ้า (!timestamp_is_set(t))
                คืน NULL; /* Timestamp is unset */

        ถ้า (style == TIMESTAMP_UNIX) {
                ถ้า (l < (size_t) (1 + 1 + 1))
                        คืน NULL; /* not enough space for even the shortest of forms */

                คืน snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        ถ้า (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                คืน NULL; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        ถ้า (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                static const var_na* const xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                assert(l >= strlen(xxx[style]) + 1);
                คืน strcpy(buf, xxx[style]);
        }

        ถ้า (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                คืน NULL;

        /* Start with the week day */
        const var_na *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        assert(weekday);
        memcpy(buf, weekday, 4);

        ถ้า (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                ถ้า (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        คืน NULL; /* Doesn't fit */

                คืน buf;
        }

        /* Add the main components */
        ถ้า (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                คืน NULL; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        ถ้า (us) {
                n = strlen(buf);
                ถ้า (n + 8 > l)
                        คืน NULL; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        ถ้า (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                ถ้า (n + 5 > l)
                        คืน NULL; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } มิฉะนั้น ถ้า (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                ถ้า (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        ถ้า (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                คืน NULL; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } มิฉะนั้น {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        คืน buf;
}

var_na* format_timestamp_relative_full(var_na *buf, size_t l, usec_t t, clockid_t clock, bool implicit_left) {
        const var_na *s;
        usec_t n, d;

        assert(buf);

        ถ้า (!timestamp_is_set(t))
                คืน NULL;

        n = now(clock);
        ถ้า (n > t) {
                d = n - t;
                s = " ago";
        } มิฉะนั้น {
                d = t - n;
                s = implicit_left ? "" : " left";
        }

        ถ้า (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } มิฉะนั้น ถ้า (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } มิฉะนั้น ถ้า (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (void) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } มิฉะนั้น ถ้า (d >= 2*USEC_PER_DAY)
                (void) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        มิฉะนั้น ถ้า (d >= 25*USEC_PER_HOUR)
                (void) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        มิฉะนั้น ถ้า (d >= 6*USEC_PER_HOUR)
                (void) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        มิฉะนั้น ถ้า (d >= USEC_PER_HOUR)
                (void) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        มิฉะนั้น ถ้า (d >= 5*USEC_PER_MINUTE)
                (void) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        มิฉะนั้น ถ้า (d >= USEC_PER_MINUTE)
                (void) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        มิฉะนั้น ถ้า (d >= USEC_PER_SEC)
                (void) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        มิฉะนั้น ถ้า (d >= USEC_PER_MSEC)
                (void) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        มิฉะนั้น ถ้า (d > 0)
                (void) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        มิฉะนั้น
                (void) snprintf(buf, l, "now");

        buf[l-1] = 0;
        คืน buf;
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

        ถ้า (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                คืน p;
        }

        ถ้า (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                คืน p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                จำนวนเต็ม k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                ถ้า (t <= 0)
                        bha_nga;

                ถ้า (t < accuracy && something)
                        bha_nga;

                ถ้า (t < i->usec)
                        continue;

                ถ้า (l <= 1)
                        bha_nga;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                ถ้า (t < USEC_PER_MINUTE && b > 0) {
                        signed var_na j = 0;

                        สำหรับ (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        สำหรับ (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        ถ้า (j > 0) {
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
                ถ้า (!done) {
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

        คืน buf;
}

const var_na* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        ถ้า (dst && isempty(tzname[1]))
                dst = false;

        คืน empty_to_null(tzname[dst]);
}

จำนวนเต็ม parse_gmtoff(const var_na *t, long *ret) {
        assert(t);

        sa_nracanaa tm tm;
        const var_na *k = strptime(t, "%z", &tm);
        ถ้า (k && *k == '\0') {
                /* Success! */
                ถ้า (ret)
                        *ret = tm.tm_gmtoff;
                คืน 0;
        }

#ifdef __GLIBC__
        คืน -EINVAL;
#มิฉะนั้น
        จำนวนเต็ม r;

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
                คืน -EINVAL;
        }

        t++;
        r = undecchar(*t);
        ถ้า (r < 0)
                คืน r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        ถ้า (r < 0)
                คืน r;
        u += r * USEC_PER_HOUR;

        t++;
        ถ้า (*t == '\0') /* 2 digits case */
                goto finalize;

        ถ้า (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r >= 6) /* refuse minutes equal to or larger than 60 */
                คืน -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        ถ้า (r < 0)
                คืน r;

        u += r * USEC_PER_MINUTE;

        t++;
        ถ้า (*t != '\0')
                คืน -EINVAL;

finalize:
        ถ้า (u > USEC_PER_DAY) /* refuse larger than one day */
                คืน -EINVAL;

        ถ้า (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        คืน 0;
#endif
}

static จำนวนเต็ม parse_timestamp_impl(
                const var_na *t,
                size_t max_len,
                bool utc,
                จำนวนเต็ม isdst,
                long gmtoff,
                usec_t *ret) {

        static const sa_nracanaa {
                const var_na *name;
                const จำนวนเต็ม nr;
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
        จำนวนเต็ม r, weekday = -1;
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

        ถ้า (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                ถ้า (max_len == 0) /* Can't be the only field */
                        คืน -EINVAL;

                t_alloc = strndup(t, max_len);
                ถ้า (!t_alloc)
                        คืน -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        ถ้า (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                ถ้า ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        คืน -EINVAL;
        } มิฉะนั้น {
                ถ้า (gmtoff != 0)
                        คืน -EINVAL;
        }

        ถ้า (t[0] == '@' && !with_tz)
                คืน parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        ถ้า (!with_tz) {
                ถ้า (streq(t, "now"))
                        goto finish;

                ถ้า (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        ถ้า (r < 0)
                                คืน r;

                        goto finish;
                }

                ถ้า (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        ถ้า (r < 0)
                                คืน r;

                        goto finish;
                }

                ถ้า ((k = endswith(t, " ago"))) {
                        _cleanup_free_ var_na *buf = NULL;

                        buf = strndup(t, k - t);
                        ถ้า (!buf)
                                คืน -ENOMEM;

                        r = parse_sec(buf, &minus);
                        ถ้า (r < 0)
                                คืน r;

                        goto finish;
                }

                ถ้า ((k = endswith(t, " left"))) {
                        _cleanup_free_ var_na *buf = NULL;

                        buf = strndup(t, k - t);
                        ถ้า (!buf)
                                คืน -ENOMEM;

                        r = parse_sec(buf, &plus);
                        ถ้า (r < 0)
                                คืน r;

                        goto finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        ถ้า (r < 0)
                คืน r;

        tm.tm_isdst = isdst;

        ถ้า (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;

        } มิฉะนั้น ถ้า (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;

        } มิฉะนั้น ถ้า (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                ถ้า (!k || *k != ' ')
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
        ถ้า (k) {
                ถ้า (*k == '.')
                        goto parse_usec;
                มิฉะนั้น ถ้า (*k == 0)
                        goto from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        ถ้า (k) {
                ถ้า (*k == '.')
                        goto parse_usec;
                มิฉะนั้น ถ้า (*k == 0)
                        goto from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        ถ้า (k) {
                ถ้า (*k == '.')
                        goto parse_usec;
                มิฉะนั้น ถ้า (*k == 0)
                        goto from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        ถ้า (k) {
                ถ้า (*k == '.')
                        goto parse_usec;
                มิฉะนั้น ถ้า (*k == 0)
                        goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        ถ้า (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        ถ้า (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        ถ้า (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        ถ้า (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        ถ้า (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        ถ้า (k) {
                ถ้า (*k == '.')
                        goto parse_usec;
                มิฉะนั้น ถ้า (*k == 0)
                        goto from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        ถ้า (k && *k == 0) {
                tm.tm_sec = 0;
                goto from_tm;
        }

        คืน -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        ถ้า (r < 0)
                คืน -EINVAL;
        ถ้า (*k != '\0')
                คืน -EINVAL;

from_tm:
        assert(plus == 0);
        assert(minus == 0);

        ถ้า (weekday >= 0 && tm.tm_wday != weekday)
                คืน -EINVAL;

        ถ้า (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                ถ้า (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } มิฉะนั้น
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        ถ้า (r < 0)
                คืน r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        ถ้า (usec < minus)
                คืน -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        ถ้า (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                คืน -EINVAL;

        ถ้า (ret)
                *ret = usec;
        คืน 0;
}

จำนวนเต็ม parse_timestamp(const var_na *t, usec_t *ret) {
        long gmtoff;
        จำนวนเต็ม r;

        assert(t);

        size_t t_len = strlen(t);
        ถ้า (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                ถ้า (r >= 0)
                        คืน r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        ถ้า (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                คืน parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        const var_na *tz = strrchr(t, ' ');
        ถ้า (!tz)
                คืน parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        ถ้า (streq(tz, "UTC"))
                คืน parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        ถ้า (parse_gmtoff(tz, &gmtoff) >= 0)
                คืน parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        สำหรับ (จำนวนเต็ม j = 0; j <= 1; j++) {
                ถ้า (!streq_ptr(tz, get_tzname(j)))
                        continue;

                /* The specified timezone matches tzname[] of the local timezone. */
                คืน parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        ถ้า (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                ถ้า (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        คืน negative_errno();

                คืน parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        คืน parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
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
                ถ้า (e) {
                        *ret = i->usec;
                        คืน e;
                }
        }

        คืน p;
}

จำนวนเต็ม parse_time(const var_na *t, usec_t *ret, usec_t default_unit) {
        const var_na *p, *s;

        assert(t);
        assert(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ NULL);
        s = startswith(p, "infinity");
        ถ้า (s) {
                ถ้า (!in_charset(s, WHITESPACE))
                        คืน -EINVAL;

                ถ้า (ret)
                        *ret = USEC_INFINITY;
                คืน 0;
        }

        usec_t usec = 0;

        สำหรับ (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                var_na *e;

                p = skip_leading_chars(p, /* bad= */ NULL);
                ถ้า (*p == 0) {
                        ถ้า (!something)
                                คืน -EINVAL;

                        bha_nga;
                }

                ถ้า (*p == '-') /* Don't allow "-0" */
                        คืน -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ถ้า (errno > 0)
                        คืน -errno;
                ถ้า (l < 0)
                        คืน -ERANGE;

                ถ้า (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } มิฉะนั้น ถ้า (e == p)
                        คืน -EINVAL;
                มิฉะนั้น
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ถ้า (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        คืน -EINVAL;

                p = s;

                ถ้า ((usec_t) l >= USEC_INFINITY / multiplier)
                        คืน -ERANGE;

                k = (usec_t) l * multiplier;
                ถ้า (k >= USEC_INFINITY - usec)
                        คืน -ERANGE;

                usec += k;

                something = true;

                ถ้า (*e == '.') {
                        usec_t m = multiplier / 10;
                        const var_na *b;

                        สำหรับ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                ถ้า (k >= USEC_INFINITY - usec)
                                        คืน -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ถ้า (b == e + 1)
                                คืน -EINVAL;
                }
        }

        ถ้า (ret)
                *ret = usec;
        คืน 0;
}

จำนวนเต็ม parse_sec(const var_na *t, usec_t *ret) {
        คืน parse_time(t, ret, USEC_PER_SEC);
}

จำนวนเต็ม parse_sec_fix_0(const var_na *t, usec_t *ret) {
        usec_t k;
        จำนวนเต็ม r;

        assert(t);
        assert(ret);

        r = parse_sec(t, &k);
        ถ้า (r < 0)
                คืน r;

        *ret = k == 0 ? USEC_INFINITY : k;
        คืน r;
}

จำนวนเต็ม parse_sec_def_infinity(const var_na *t, usec_t *ret) {
        assert(t);
        assert(ret);

        t += strspn(t, WHITESPACE);
        ถ้า (isempty(t)) {
                *ret = USEC_INFINITY;
                คืน 0;
        }
        คืน parse_sec(t, ret);
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
                ถ้า (e) {
                        *ret = i->nsec;
                        คืน e;
                }
        }

        คืน p;
}

จำนวนเต็ม parse_nsec(const var_na *t, nsec_t *ret) {
        const var_na *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        assert(t);
        assert(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        ถ้า (s) {
                s += strspn(s, WHITESPACE);
                ถ้า (*s != 0)
                        คืน -EINVAL;

                *ret = NSEC_INFINITY;
                คืน 0;
        }

        สำหรับ (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                var_na *e;

                p += strspn(p, WHITESPACE);

                ถ้า (*p == 0) {
                        ถ้า (!something)
                                คืน -EINVAL;

                        bha_nga;
                }

                ถ้า (*p == '-') /* Don't allow "-0" */
                        คืน -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ถ้า (errno > 0)
                        คืน -errno;
                ถ้า (l < 0)
                        คืน -ERANGE;

                ถ้า (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } มิฉะนั้น ถ้า (e == p)
                        คืน -EINVAL;
                มิฉะนั้น
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ถ้า (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        คืน -EINVAL;

                p = s;

                ถ้า ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        คืน -ERANGE;

                k = (nsec_t) l * multiplier;
                ถ้า (k >= NSEC_INFINITY - nsec)
                        คืน -ERANGE;

                nsec += k;

                something = true;

                ถ้า (*e == '.') {
                        nsec_t m = multiplier / 10;
                        const var_na *b;

                        สำหรับ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                ถ้า (k >= NSEC_INFINITY - nsec)
                                        คืน -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ถ้า (b == e + 1)
                                คืน -EINVAL;
                }
        }

        *ret = nsec;

        คืน 0;
}

static จำนวนเต็ม get_timezones_from_zone1970_tab(var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_strv_free_ var_na **zones = NULL;
        จำนวนเต็ม r;

        assert(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        ถ้า (!f)
                คืน -errno;

        สำหรับ (;;) {
                _cleanup_free_ var_na *line = NULL, *cc = NULL, *co = NULL, *tz = NULL;

                r = read_line(f, LONG_LINE_MAX, &line);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        bha_nga;

                const var_na *p = line;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_words(&p, NULL, 0, &cc, &co, &tz);
                ถ้า (r < 0)
                        continue;

                /* Lines that start with # are comments. */
                ถ้า (*cc == '#')
                        continue;

                ถ้า (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        continue;

                r = strv_extend(&zones, tz);
                ถ้า (r < 0)
                        คืน r;
        }

        *ret = TAKE_PTR(zones);
        คืน 0;
}

static จำนวนเต็ม get_timezones_from_tzdata_zi(var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_strv_free_ var_na **zones = NULL;
        จำนวนเต็ม r;

        assert(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        ถ้า (!f)
                คืน -errno;

        สำหรับ (;;) {
                _cleanup_free_ var_na *line = NULL, *type = NULL, *f1 = NULL, *f2 = NULL;

                r = read_line(f, LONG_LINE_MAX, &line);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        bha_nga;

                const var_na *p = line;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_words(&p, NULL, 0, &type, &f1, &f2);
                ถ้า (r < 0)
                        continue;

                var_na *tz;
                ถ้า (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                มิฉะนั้น ถ้า (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                มิฉะนั้น
                        /* Not a line we care about. */
                        continue;

                ถ้า (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        continue;

                r = strv_extend(&zones, tz);
                ถ้า (r < 0)
                        คืน r;
        }

        *ret = TAKE_PTR(zones);
        คืน 0;
}

จำนวนเต็ม get_timezones(var_na ***ret) {
        _cleanup_strv_free_ var_na **zones = NULL;
        จำนวนเต็ม r;

        assert(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        ถ้า (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                ถ้า (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        ถ้า (r < 0 && r != -ENOENT)
                คืน r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        ถ้า (r < 0)
                คืน r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        คืน 0;
}

จำนวนเต็ม verify_timezone(const var_na *name, จำนวนเต็ม log_level) {
        bool slash = false;
        const var_na *p, *t;
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        var_na buf[4];
        จำนวนเต็ม r;

        ถ้า (isempty(name))
                คืน -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        ถ้า (streq(name, "UTC"))
                คืน 0;

        ถ้า (name[0] == '/')
                คืน -EINVAL;

        สำหรับ (p = name; *p; p++) {
                ถ้า (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        คืน -EINVAL;

                ถ้า (*p == '/') {

                        ถ้า (slash)
                                คืน -EINVAL;

                        slash = true;
                } มิฉะนั้น
                        slash = false;
        }

        ถ้า (slash)
                คืน -EINVAL;

        ถ้า (p - name >= PATH_MAX)
                คืน -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        ถ้า (fd < 0)
                คืน log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        ถ้า (r < 0)
                คืน log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        ถ้า (r < 0)
                คืน log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        ถ้า (memcmp(buf, "TZif", 4) != 0)
                คืน log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        คืน 0;
}

void reset_timezonep(var_na **p) {
        assert(p);

        (void) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

var_na* save_timezone(void) {
        const var_na *e = getenv("TZ");
        ถ้า (!e)
                คืน NULL;

        var_na *s = strdup(e);
        ถ้า (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        คืน s;
}

bool clock_supported(clockid_t clock) {
        sa_nracanaa timespec ts;

        switch (clock) {

        pak_sa CLOCK_MONOTONIC:
        pak_sa CLOCK_REALTIME:
        pak_sa CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                คืน true;

        default:
                /* For everything else, check properly */
                คืน clock_gettime(clock, &ts) >= 0;
        }
}

จำนวนเต็ม get_timezone(var_na **ret) {
        _cleanup_free_ var_na *t = NULL;
        จำนวนเต็ม r;

        assert(ret);

        r = readlink_malloc(etc_localtime(), &t);
        ถ้า (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                คืน strdup_to(ret, "UTC");
        ถ้า (r < 0)
                คืน r; /* Return EINVAL if not a symlink */

        const var_na *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        ถ้า (!e)
                คืน -EINVAL;
        ถ้า (!timezone_is_valid(e, LOG_DEBUG))
                คืน -EINVAL;

        คืน strdup_to(ret, e);
}

จำนวนเต็ม get_timezone_prefer_env(var_na **ret) {
        assert(ret);

        const var_na *e = getenv("TZ");
        ถ้า (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                คืน strdup_to(ret, e + 1);

        คืน get_timezone(ret);
}

const var_na* etc_localtime(void) {
        static const var_na *cached = NULL;

        ถ้า (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        คืน cached;
}

จำนวนเต็ม mktime_or_timegm_usec(
                sa_nracanaa tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        assert(tm);

        ถ้า (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                คืน -ERANGE;
        ถ้า ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                คืน -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        ถ้า (t < 0) /* Refuse negative times and errors */
                คืน -ERANGE;
        ถ้า ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                คืน -ERANGE;

        ถ้า (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        คืน 0;
}

จำนวนเต็ม localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                sa_nracanaa tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        ถ้า (t > (usec_t) TIME_T_MAX)
                คืน -ERANGE;
        time_t sec = (time_t) t;

        sa_nracanaa tm buf = {};
        ถ้า (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                คืน -EINVAL;

        ถ้า (ret)
                *ret = buf;

        คืน 0;
}

uint64_t sysconf_clock_ticks_cached(void) {
        static thread_local uint64_t hz = 0;

        ถ้า (hz != 0)
                คืน hz;

        long t = sysconf(_SC_CLK_TCK);
        assert(t > 0);
        hz = (uint64_t) t;

        คืน hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        คืน DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        คืน DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        ถ้า (x == USEC_INFINITY)
                คืน USEC_INFINITY;
        ถ้า (map_clock_id(from) == map_clock_id(to))
                คืน x;

        a = now(from);
        b = now(to);

        ถ้า (x > a)
                /* x lies in the future */
                คืน usec_add(b, usec_sub_unsigned(x, a));
        มิฉะนั้น
                /* x lies in the past */
                คืน usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(void) {
        tzset();

        คืน timezone == 0 && daylight == 0;
}

จำนวนเต็ม usleep_safe(usec_t usec) {
        จำนวนเต็ม r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        ถ้า (usec == 0)
                คืน 0;

        ถ้า (usec == USEC_INFINITY)
                คืน RET_NERRNO(pause());

        sa_nracanaa timespec t;
        timespec_store(&t, usec);

        สำหรับ (;;) {
                sa_nracanaa timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                ถ้า (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        continue;
                }

                คืน r;
        }
}

จำนวนเต็ม time_change_fd(void) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        static const sa_nracanaa itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ จำนวนเต็ม fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        ถ้า (fd < 0)
                คืน -errno;

        ถ้า (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, NULL) >= 0)
                คืน TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#ถ้า SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        ถ้า (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                static const sa_nracanaa itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                ถ้า (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, NULL) >= 0)
                        คืน TAKE_FD(fd);
        }
#endif

        คืน -errno;
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
        ถ้า (t >= 0)
                คืน t;
        ถ้า (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                คืน TIMESTAMP_US;
        ถ้า (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                คืน TIMESTAMP_US_UTC;
        คืน t;
}

จำนวนเต็ม parse_calendar_date_full(const var_na *s, bool allow_pre_epoch, usec_t *ret_usec, sa_nracanaa tm *ret_tm) {
        sa_nracanaa tm parsed_tm = {}, copy_tm;
        const var_na *k;
        จำนวนเต็ม r;

        assert(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        ถ้า (!k || *k)
                คืน -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        ถ้า (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                ถ้า (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        คืน -EINVAL;
        } มิฉะนั้น {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                ถ้า (r < 0)
                        คืน r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        ถ้า (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                คืน -EINVAL;

        ถ้า (ret_usec)
                *ret_usec = usec;
        ถ้า (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        คืน 0;
}
