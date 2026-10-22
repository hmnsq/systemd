/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/time_types.h>
#ضمن <stdlib.h>
#ضمن <sys/timerfd.h>
#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "env-util.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "hexdecoct.h"          /* IWYU pragma: keep */
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "fs-util.h"
#ضمن "io-util.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "time-util.h"

ساكن clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        بدل (c) {

        حالة CLOCK_BOOTTIME_ALARM:
                أرجع CLOCK_BOOTTIME;

        حالة CLOCK_REALTIME_ALARM:
                أرجع CLOCK_REALTIME;

        افتراضي:
                أرجع c;
        }
}

usec_t now(clockid_t clock_id) {
        بنية timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        أكد(n > 0);
        أكد(n < USEC_INFINITY);

        أرجع n;
}

nsec_t now_nsec(clockid_t clock_id) {
        بنية timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        أكد(n > 0);
        أكد(n < NSEC_INFINITY);

        أرجع n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        أكد(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        أرجع ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        أكد(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        أرجع ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        إذا (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                إذا (to_base >= USEC_INFINITY - delta) /* overflow? */
                        أرجع USEC_INFINITY;

                أرجع to_base + delta;

        } وإلا { /* In the past */
                usec_t delta = from_base - from;

                إذا (to_base <= delta) /* underflow? */
                        أرجع 0;

                أرجع to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        إذا (map_clock_id(from_clock) == map_clock_id(to_clock))
                أرجع from;

        /* Keep infinity as is */
        إذا (from == USEC_INFINITY)
                أرجع from;

        أرجع map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        أكد(ts);

        إذا (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                أرجع ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        أرجع ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        أكد(ts);

        إذا (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                أرجع ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        أرجع ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        أكد(ts);

        إذا (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                أرجع ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        أرجع ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        أكد(ts);

        إذا (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                أرجع ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        أرجع ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        أكد(ts);

        إذا (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                أرجع ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        أرجع ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        أكد(ts);

        بدل (clock) {

        حالة CLOCK_REALTIME:
        حالة CLOCK_REALTIME_ALARM:
                أرجع ts->realtime;

        حالة CLOCK_MONOTONIC:
                أرجع ts->monotonic;

        حالة CLOCK_BOOTTIME:
        حالة CLOCK_BOOTTIME_ALARM:
                أرجع ts->boottime;

        افتراضي:
                أرجع USEC_INFINITY;
        }
}

usec_t timespec_load(ثابت بنية timespec *ts) {
        أكد(ts);

        إذا (ts->tv_sec < 0 || ts->tv_nsec < 0)
                أرجع USEC_INFINITY;

        إذا ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                أرجع USEC_INFINITY;

        أرجع
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(ثابت بنية timespec *ts) {
        أكد(ts);

        إذا (ts->tv_sec < 0 || ts->tv_nsec < 0)
                أرجع NSEC_INFINITY;

        إذا ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                أرجع NSEC_INFINITY;

        أرجع (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

بنية timespec *timespec_store(بنية timespec *ts, usec_t u) {
        أكد(ts);

        إذا (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                أرجع ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        أرجع ts;
}

بنية timespec *timespec_store_nsec(بنية timespec *ts, nsec_t n) {
        أكد(ts);

        إذا (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                أرجع ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        أرجع ts;
}

بنية __kernel_timespec *kernel_timespec_store(بنية __kernel_timespec *ts, usec_t u) {
        أكد(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        أكد(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        أرجع ts;
}

usec_t timeval_load(ثابت بنية timeval *tv) {
        أكد(tv);

        إذا (tv->tv_sec < 0 || tv->tv_usec < 0)
                أرجع USEC_INFINITY;

        إذا ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                أرجع USEC_INFINITY;

        أرجع
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

بنية timeval *timeval_store(بنية timeval *tv, usec_t u) {
        أكد(tv);

        إذا (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } وإلا {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        أرجع tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
ساكن ثابت حرف *ثابت weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, صحيح);

حرف* format_timestamp_style(
                حرف *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        بنية tm tm;
        bool utc, us;
        size_t n;

        أكد(buf);
        أكد(style >= 0);
        أكد(style < _TIMESTAMP_STYLE_MAX);

        إذا (!timestamp_is_set(t))
                أرجع عدم; /* Timestamp is unset */

        إذا (style == TIMESTAMP_UNIX) {
                إذا (l < (size_t) (1 + 1 + 1))
                        أرجع عدم; /* not enough space for even the shortest of forms */

                أرجع snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        إذا (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                أرجع عدم; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        إذا (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                ساكن ثابت حرف* ثابت xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                أكد(l >= strlen(xxx[style]) + 1);
                أرجع strcpy(buf, xxx[style]);
        }

        إذا (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                أرجع عدم;

        /* Start with the week day */
        ثابت حرف *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        أكد(weekday);
        memcpy(buf, weekday, 4);

        إذا (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                إذا (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        أرجع عدم; /* Doesn't fit */

                أرجع buf;
        }

        /* Add the main components */
        إذا (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                أرجع عدم; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        إذا (us) {
                n = strlen(buf);
                إذا (n + 8 > l)
                        أرجع عدم; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        إذا (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                إذا (n + 5 > l)
                        أرجع عدم; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } وإلا إذا (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                إذا (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        إذا (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                أرجع عدم; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } وإلا {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        أرجع buf;
}

حرف* format_timestamp_relative_full(حرف *buf, size_t l, usec_t t, clockid_t clock, bool implicit_چەپ) {
        ثابت حرف *s;
        usec_t n, d;

        أكد(buf);

        إذا (!timestamp_is_set(t))
                أرجع عدم;

        n = now(clock);
        إذا (n > t) {
                d = n - t;
                s = " ago";
        } وإلا {
                d = t - n;
                s = implicit_چەپ ? "" : " left";
        }

        إذا (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (فراغ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } وإلا إذا (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (فراغ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } وإلا إذا (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (فراغ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } وإلا إذا (d >= 2*USEC_PER_DAY)
                (فراغ) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        وإلا إذا (d >= 25*USEC_PER_HOUR)
                (فراغ) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        وإلا إذا (d >= 6*USEC_PER_HOUR)
                (فراغ) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        وإلا إذا (d >= USEC_PER_HOUR)
                (فراغ) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        وإلا إذا (d >= 5*USEC_PER_MINUTE)
                (فراغ) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        وإلا إذا (d >= USEC_PER_MINUTE)
                (فراغ) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        وإلا إذا (d >= USEC_PER_SEC)
                (فراغ) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        وإلا إذا (d >= USEC_PER_MSEC)
                (فراغ) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        وإلا إذا (d > 0)
                (فراغ) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        وإلا
                (فراغ) snprintf(buf, l, "now");

        buf[l-1] = 0;
        أرجع buf;
}

حرف* format_timespan(حرف *buf, size_t l, usec_t t, usec_t accuracy) {
        ساكن ثابت بنية {
                ثابت حرف *suffix;
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

        حرف *p = ASSERT_PTR(buf);
        bool something = false;

        أكد(l > 0);

        إذا (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                أرجع p;
        }

        إذا (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                أرجع p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                صحيح k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                إذا (t <= 0)
                        اكسر;

                إذا (t < accuracy && something)
                        اكسر;

                إذا (t < i->usec)
                        استمر;

                إذا (l <= 1)
                        اكسر;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                إذا (t < USEC_PER_MINUTE && b > 0) {
                        signed حرف j = 0;

                        لكل (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        لكل (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        إذا (j > 0) {
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
                إذا (!done) {
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

        أرجع buf;
}

ثابت حرف* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        إذا (dst && isempty(tzname[1]))
                dst = false;

        أرجع empty_to_null(tzname[dst]);
}

صحيح parse_gmtoff(ثابت حرف *t, long *ret) {
        أكد(t);

        بنية tm tm;
        ثابت حرف *k = strptime(t, "%z", &tm);
        إذا (k && *k == '\0') {
                /* Success! */
                إذا (ret)
                        *ret = tm.tm_gmtoff;
                أرجع 0;
        }

#ifdef __GLIBC__
        أرجع -EINVAL;
#وإلا
        صحيح r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        بدل (*t) {
        حالة '+':
                positive = true;
                اكسر;
        حالة '-':
                positive = false;
                اكسر;
        افتراضي:
                أرجع -EINVAL;
        }

        t++;
        r = undecchar(*t);
        إذا (r < 0)
                أرجع r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        إذا (r < 0)
                أرجع r;
        u += r * USEC_PER_HOUR;

        t++;
        إذا (*t == '\0') /* 2 digits case */
                اذهب finalize;

        إذا (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        إذا (r < 0)
                أرجع r;
        إذا (r >= 6) /* refuse minutes equal to or larger than 60 */
                أرجع -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        إذا (r < 0)
                أرجع r;

        u += r * USEC_PER_MINUTE;

        t++;
        إذا (*t != '\0')
                أرجع -EINVAL;

finalize:
        إذا (u > USEC_PER_DAY) /* refuse larger than one day */
                أرجع -EINVAL;

        إذا (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        أرجع 0;
#endif
}

ساكن صحيح parse_timestamp_impl(
                ثابت حرف *t,
                size_t max_len,
                bool utc,
                صحيح isdst,
                long gmtoff,
                usec_t *ret) {

        ساكن ثابت بنية {
                ثابت حرف *name;
                ثابت صحيح nr;
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

        _cleanup_free_ حرف *t_alloc = عدم;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        صحيح r, weekday = -1;
        unsigned fractional = 0;
        ثابت حرف *k;
        بنية tm tm, copy;

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

        أكد(t);

        إذا (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                إذا (max_len == 0) /* Can't be the only field */
                        أرجع -EINVAL;

                t_alloc = strndup(t, max_len);
                إذا (!t_alloc)
                        أرجع -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        إذا (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                إذا ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        أرجع -EINVAL;
        } وإلا {
                إذا (gmtoff != 0)
                        أرجع -EINVAL;
        }

        إذا (t[0] == '@' && !with_tz)
                أرجع parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        إذا (!with_tz) {
                إذا (streq(t, "now"))
                        اذهب finish;

                إذا (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        إذا (r < 0)
                                أرجع r;

                        اذهب finish;
                }

                إذا (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        إذا (r < 0)
                                أرجع r;

                        اذهب finish;
                }

                إذا ((k = endswith(t, " ago"))) {
                        _cleanup_free_ حرف *buf = عدم;

                        buf = strndup(t, k - t);
                        إذا (!buf)
                                أرجع -ENOMEM;

                        r = parse_sec(buf, &minus);
                        إذا (r < 0)
                                أرجع r;

                        اذهب finish;
                }

                إذا ((k = endswith(t, " left"))) {
                        _cleanup_free_ حرف *buf = عدم;

                        buf = strndup(t, k - t);
                        إذا (!buf)
                                أرجع -ENOMEM;

                        r = parse_sec(buf, &plus);
                        إذا (r < 0)
                                أرجع r;

                        اذهب finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        إذا (r < 0)
                أرجع r;

        tm.tm_isdst = isdst;

        إذا (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                اذهب from_tm;

        } وإلا إذا (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                اذهب from_tm;

        } وإلا إذا (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                اذهب from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                إذا (!k || *k != ' ')
                        استمر;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                اكسر;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        إذا (k) {
                إذا (*k == '.')
                        اذهب parse_usec;
                وإلا إذا (*k == 0)
                        اذهب from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        إذا (k) {
                إذا (*k == '.')
                        اذهب parse_usec;
                وإلا إذا (*k == 0)
                        اذهب from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        إذا (k) {
                إذا (*k == '.')
                        اذهب parse_usec;
                وإلا إذا (*k == 0)
                        اذهب from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        إذا (k) {
                إذا (*k == '.')
                        اذهب parse_usec;
                وإلا إذا (*k == 0)
                        اذهب from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        إذا (k && *k == 0) {
                tm.tm_sec = 0;
                اذهب from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        إذا (k && *k == 0) {
                tm.tm_sec = 0;
                اذهب from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        إذا (k && *k == 0) {
                tm.tm_sec = 0;
                اذهب from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        إذا (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                اذهب from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        إذا (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                اذهب from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        إذا (k) {
                إذا (*k == '.')
                        اذهب parse_usec;
                وإلا إذا (*k == 0)
                        اذهب from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        إذا (k && *k == 0) {
                tm.tm_sec = 0;
                اذهب from_tm;
        }

        أرجع -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        إذا (r < 0)
                أرجع -EINVAL;
        إذا (*k != '\0')
                أرجع -EINVAL;

from_tm:
        أكد(plus == 0);
        أكد(minus == 0);

        إذا (weekday >= 0 && tm.tm_wday != weekday)
                أرجع -EINVAL;

        إذا (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                إذا (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } وإلا
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        إذا (r < 0)
                أرجع r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        إذا (usec < minus)
                أرجع -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        إذا (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                أرجع -EINVAL;

        إذا (ret)
                *ret = usec;
        أرجع 0;
}

صحيح parse_timestamp(ثابت حرف *t, usec_t *ret) {
        long gmtoff;
        صحيح r;

        أكد(t);

        size_t t_len = strlen(t);
        إذا (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                إذا (r >= 0)
                        أرجع r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        إذا (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                أرجع parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ثابت حرف *tz = strrchr(t, ' ');
        إذا (!tz)
                أرجع parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        إذا (streq(tz, "UTC"))
                أرجع parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        إذا (parse_gmtoff(tz, &gmtoff) >= 0)
                أرجع parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        لكل (صحيح j = 0; j <= 1; j++) {
                إذا (!streq_ptr(tz, get_tzname(j)))
                        استمر;

                /* The specified timezone matches tzname[] of the local timezone. */
                أرجع parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        إذا (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                إذا (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        أرجع negative_errno();

                أرجع parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        أرجع parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

ساكن ثابت حرف* extract_multiplier(ثابت حرف *p, usec_t *ret) {
        ساكن ثابت بنية {
                ثابت حرف *suffix;
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

        أكد(p);
        أكد(ret);

        FOREACH_ELEMENT(i, table) {
                ثابت حرف *e = startswith(p, i->suffix);
                إذا (e) {
                        *ret = i->usec;
                        أرجع e;
                }
        }

        أرجع p;
}

صحيح parse_time(ثابت حرف *t, usec_t *ret, usec_t default_unit) {
        ثابت حرف *p, *s;

        أكد(t);
        أكد(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ عدم);
        s = startswith(p, "infinity");
        إذا (s) {
                إذا (!in_charset(s, WHITESPACE))
                        أرجع -EINVAL;

                إذا (ret)
                        *ret = USEC_INFINITY;
                أرجع 0;
        }

        usec_t usec = 0;

        لكل (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                حرف *e;

                p = skip_leading_chars(p, /* bad= */ عدم);
                إذا (*p == 0) {
                        إذا (!something)
                                أرجع -EINVAL;

                        اكسر;
                }

                إذا (*p == '-') /* Don't allow "-0" */
                        أرجع -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                إذا (errno > 0)
                        أرجع -errno;
                إذا (l < 0)
                        أرجع -ERANGE;

                إذا (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } وإلا إذا (e == p)
                        أرجع -EINVAL;
                وإلا
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                إذا (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        أرجع -EINVAL;

                p = s;

                إذا ((usec_t) l >= USEC_INFINITY / multiplier)
                        أرجع -ERANGE;

                k = (usec_t) l * multiplier;
                إذا (k >= USEC_INFINITY - usec)
                        أرجع -ERANGE;

                usec += k;

                something = true;

                إذا (*e == '.') {
                        usec_t m = multiplier / 10;
                        ثابت حرف *b;

                        لكل (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                إذا (k >= USEC_INFINITY - usec)
                                        أرجع -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        إذا (b == e + 1)
                                أرجع -EINVAL;
                }
        }

        إذا (ret)
                *ret = usec;
        أرجع 0;
}

صحيح parse_sec(ثابت حرف *t, usec_t *ret) {
        أرجع parse_time(t, ret, USEC_PER_SEC);
}

صحيح parse_sec_fix_0(ثابت حرف *t, usec_t *ret) {
        usec_t k;
        صحيح r;

        أكد(t);
        أكد(ret);

        r = parse_sec(t, &k);
        إذا (r < 0)
                أرجع r;

        *ret = k == 0 ? USEC_INFINITY : k;
        أرجع r;
}

صحيح parse_sec_def_infinity(ثابت حرف *t, usec_t *ret) {
        أكد(t);
        أكد(ret);

        t += strspn(t, WHITESPACE);
        إذا (isempty(t)) {
                *ret = USEC_INFINITY;
                أرجع 0;
        }
        أرجع parse_sec(t, ret);
}

ساكن ثابت حرف* extract_nsec_multiplier(ثابت حرف *p, nsec_t *ret) {
        ساكن ثابت بنية {
                ثابت حرف *suffix;
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

        أكد(p);
        أكد(ret);

        FOREACH_ELEMENT(i, table) {
                ثابت حرف *e = startswith(p, i->suffix);
                إذا (e) {
                        *ret = i->nsec;
                        أرجع e;
                }
        }

        أرجع p;
}

صحيح parse_nsec(ثابت حرف *t, nsec_t *ret) {
        ثابت حرف *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        أكد(t);
        أكد(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        إذا (s) {
                s += strspn(s, WHITESPACE);
                إذا (*s != 0)
                        أرجع -EINVAL;

                *ret = NSEC_INFINITY;
                أرجع 0;
        }

        لكل (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                حرف *e;

                p += strspn(p, WHITESPACE);

                إذا (*p == 0) {
                        إذا (!something)
                                أرجع -EINVAL;

                        اكسر;
                }

                إذا (*p == '-') /* Don't allow "-0" */
                        أرجع -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                إذا (errno > 0)
                        أرجع -errno;
                إذا (l < 0)
                        أرجع -ERANGE;

                إذا (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } وإلا إذا (e == p)
                        أرجع -EINVAL;
                وإلا
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                إذا (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        أرجع -EINVAL;

                p = s;

                إذا ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        أرجع -ERANGE;

                k = (nsec_t) l * multiplier;
                إذا (k >= NSEC_INFINITY - nsec)
                        أرجع -ERANGE;

                nsec += k;

                something = true;

                إذا (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ثابت حرف *b;

                        لكل (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                إذا (k >= NSEC_INFINITY - nsec)
                                        أرجع -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        إذا (b == e + 1)
                                أرجع -EINVAL;
                }
        }

        *ret = nsec;

        أرجع 0;
}

ساكن صحيح get_timezones_from_zone1970_tab(حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_strv_free_ حرف **zones = عدم;
        صحيح r;

        أكد(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        إذا (!f)
                أرجع -errno;

        لكل (;;) {
                _cleanup_free_ حرف *هێڵ = عدم, *cc = عدم, *co = عدم, *tz = عدم;

                r = read_هێڵ(f, LONG_هێڵ_MAX, &هێڵ);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                ثابت حرف *p = هێڵ;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_وشەکان(&p, عدم, 0, &cc, &co, &tz);
                إذا (r < 0)
                        استمر;

                /* Lines that start with # are comments. */
                إذا (*cc == '#')
                        استمر;

                إذا (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        استمر;

                r = strv_extend(&zones, tz);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(zones);
        أرجع 0;
}

ساكن صحيح get_timezones_from_tzdata_zi(حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_strv_free_ حرف **zones = عدم;
        صحيح r;

        أكد(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        إذا (!f)
                أرجع -errno;

        لكل (;;) {
                _cleanup_free_ حرف *هێڵ = عدم, *type = عدم, *f1 = عدم, *f2 = عدم;

                r = read_هێڵ(f, LONG_هێڵ_MAX, &هێڵ);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                ثابت حرف *p = هێڵ;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_وشەکان(&p, عدم, 0, &type, &f1, &f2);
                إذا (r < 0)
                        استمر;

                حرف *tz;
                إذا (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                وإلا إذا (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                وإلا
                        /* Not a line we care about. */
                        استمر;

                إذا (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        استمر;

                r = strv_extend(&zones, tz);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(zones);
        أرجع 0;
}

صحيح get_timezones(حرف ***ret) {
        _cleanup_strv_free_ حرف **zones = عدم;
        صحيح r;

        أكد(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        إذا (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                إذا (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        إذا (r < 0 && r != -ENOENT)
                أرجع r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        إذا (r < 0)
                أرجع r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        أرجع 0;
}

صحيح verify_timezone(ثابت حرف *name, صحيح log_level) {
        bool slash = false;
        ثابت حرف *p, *t;
        _cleanup_close_ صحيح fd = -EBADF;
        حرف buf[4];
        صحيح r;

        إذا (isempty(name))
                أرجع -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        إذا (streq(name, "UTC"))
                أرجع 0;

        إذا (name[0] == '/')
                أرجع -EINVAL;

        لكل (p = name; *p; p++) {
                إذا (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        أرجع -EINVAL;

                إذا (*p == '/') {

                        إذا (slash)
                                أرجع -EINVAL;

                        slash = true;
                } وإلا
                        slash = false;
        }

        إذا (slash)
                أرجع -EINVAL;

        إذا (p - name >= PATH_MAX)
                أرجع -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        إذا (r < 0)
                أرجع log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        إذا (r < 0)
                أرجع log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        إذا (memcmp(buf, "TZif", 4) != 0)
                أرجع log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        أرجع 0;
}

فراغ reset_timezonep(حرف **p) {
        أكد(p);

        (فراغ) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

حرف* save_timezone(فراغ) {
        ثابت حرف *e = getenv("TZ");
        إذا (!e)
                أرجع عدم;

        حرف *s = strdup(e);
        إذا (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        أرجع s;
}

bool clock_supported(clockid_t clock) {
        بنية timespec ts;

        بدل (clock) {

        حالة CLOCK_MONOTONIC:
        حالة CLOCK_REALTIME:
        حالة CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                أرجع true;

        افتراضي:
                /* For everything else, check properly */
                أرجع clock_gettime(clock, &ts) >= 0;
        }
}

صحيح get_timezone(حرف **ret) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(ret);

        r = readlink_malloc(etc_localtime(), &t);
        إذا (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                أرجع strdup_to(ret, "UTC");
        إذا (r < 0)
                أرجع r; /* Return EINVAL if not a symlink */

        ثابت حرف *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        إذا (!e)
                أرجع -EINVAL;
        إذا (!timezone_is_valid(e, LOG_DEBUG))
                أرجع -EINVAL;

        أرجع strdup_to(ret, e);
}

صحيح get_timezone_prefer_env(حرف **ret) {
        أكد(ret);

        ثابت حرف *e = getenv("TZ");
        إذا (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                أرجع strdup_to(ret, e + 1);

        أرجع get_timezone(ret);
}

ثابت حرف* etc_localtime(فراغ) {
        ساكن ثابت حرف *cached = عدم;

        إذا (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        أرجع cached;
}

صحيح mktime_or_timegm_usec(
                بنية tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        أكد(tm);

        إذا (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                أرجع -ERANGE;
        إذا ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                أرجع -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        إذا (t < 0) /* Refuse negative times and errors */
                أرجع -ERANGE;
        إذا ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                أرجع -ERANGE;

        إذا (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        أرجع 0;
}

صحيح localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                بنية tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        إذا (t > (usec_t) TIME_T_MAX)
                أرجع -ERANGE;
        time_t sec = (time_t) t;

        بنية tm buf = {};
        إذا (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                أرجع -EINVAL;

        إذا (ret)
                *ret = buf;

        أرجع 0;
}

uint64_t sysconf_clock_ticks_cached(فراغ) {
        ساكن thread_local uint64_t hz = 0;

        إذا (hz != 0)
                أرجع hz;

        long t = sysconf(_SC_CLK_TCK);
        أكد(t > 0);
        hz = (uint64_t) t;

        أرجع hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        أرجع DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        أرجع DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        إذا (x == USEC_INFINITY)
                أرجع USEC_INFINITY;
        إذا (map_clock_id(from) == map_clock_id(to))
                أرجع x;

        a = now(from);
        b = now(to);

        إذا (x > a)
                /* x lies in the future */
                أرجع usec_add(b, usec_sub_unsigned(x, a));
        وإلا
                /* x lies in the past */
                أرجع usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(فراغ) {
        tzset();

        أرجع timezone == 0 && daylight == 0;
}

صحيح usleep_safe(usec_t usec) {
        صحيح r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        إذا (usec == 0)
                أرجع 0;

        إذا (usec == USEC_INFINITY)
                أرجع RET_NERRNO(pause());

        بنية timespec t;
        timespec_store(&t, usec);

        لكل (;;) {
                بنية timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                إذا (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        استمر;
                }

                أرجع r;
        }
}

صحيح time_change_fd(فراغ) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        ساكن ثابت بنية itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ صحيح fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        إذا (fd < 0)
                أرجع -errno;

        إذا (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, عدم) >= 0)
                أرجع TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#إذا SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        إذا (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                ساكن ثابت بنية itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                إذا (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, عدم) >= 0)
                        أرجع TAKE_FD(fd);
        }
#endif

        أرجع -errno;
}

ساكن ثابت حرف* ثابت timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ثابت حرف *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        إذا (t >= 0)
                أرجع t;
        إذا (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                أرجع TIMESTAMP_US;
        إذا (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                أرجع TIMESTAMP_US_UTC;
        أرجع t;
}

صحيح parse_calendar_date_full(ثابت حرف *s, bool allow_pre_epoch, usec_t *ret_usec, بنية tm *ret_tm) {
        بنية tm parsed_tm = {}, copy_tm;
        ثابت حرف *k;
        صحيح r;

        أكد(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        إذا (!k || *k)
                أرجع -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        إذا (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                إذا (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        أرجع -EINVAL;
        } وإلا {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                إذا (r < 0)
                        أرجع r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        إذا (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                أرجع -EINVAL;

        إذا (ret_usec)
                *ret_usec = usec;
        إذا (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        أرجع 0;
}
