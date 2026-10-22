/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <linux/time_types.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/timerfd.h>
#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "hexdecoct.h"          /* IWYU pragma: keep */
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "time-util.h"

স্থির clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        নির্বাচন (c) {

        ক্ষেত্র CLOCK_BOOTTIME_ALARM:
                ফেরত CLOCK_BOOTTIME;

        ক্ষেত্র CLOCK_REALTIME_ALARM:
                ফেরত CLOCK_REALTIME;

        পূর্বনির্ধারিত:
                ফেরত c;
        }
}

usec_t now(clockid_t clock_id) {
        কাঠামো timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        প্রতিজ্ঞা(n > 0);
        প্রতিজ্ঞা(n < USEC_INFINITY);

        ফেরত n;
}

nsec_t now_nsec(clockid_t clock_id) {
        কাঠামো timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        প্রতিজ্ঞা(n > 0);
        প্রতিজ্ঞা(n < NSEC_INFINITY);

        ফেরত n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        প্রতিজ্ঞা(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        ফেরত ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        প্রতিজ্ঞা(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        ফেরত ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        যদি (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                যদি (to_base >= USEC_INFINITY - delta) /* overflow? */
                        ফেরত USEC_INFINITY;

                ফেরত to_base + delta;

        } অন্যথা { /* In the past */
                usec_t delta = from_base - from;

                যদি (to_base <= delta) /* underflow? */
                        ফেরত 0;

                ফেরত to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        যদি (map_clock_id(from_clock) == map_clock_id(to_clock))
                ফেরত from;

        /* Keep infinity as is */
        যদি (from == USEC_INFINITY)
                ফেরত from;

        ফেরত map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        প্রতিজ্ঞা(ts);

        যদি (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                ফেরত ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        ফেরত ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        প্রতিজ্ঞা(ts);

        যদি (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ফেরত ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        ফেরত ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        প্রতিজ্ঞা(ts);

        যদি (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ফেরত ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        ফেরত ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        প্রতিজ্ঞা(ts);

        যদি (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ফেরত ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        ফেরত ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        প্রতিজ্ঞা(ts);

        যদি (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ফেরত ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        ফেরত ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        প্রতিজ্ঞা(ts);

        নির্বাচন (clock) {

        ক্ষেত্র CLOCK_REALTIME:
        ক্ষেত্র CLOCK_REALTIME_ALARM:
                ফেরত ts->realtime;

        ক্ষেত্র CLOCK_MONOTONIC:
                ফেরত ts->monotonic;

        ক্ষেত্র CLOCK_BOOTTIME:
        ক্ষেত্র CLOCK_BOOTTIME_ALARM:
                ফেরত ts->boottime;

        পূর্বনির্ধারিত:
                ফেরত USEC_INFINITY;
        }
}

usec_t timespec_load(ধ্রুবক কাঠামো timespec *ts) {
        প্রতিজ্ঞা(ts);

        যদি (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ফেরত USEC_INFINITY;

        যদি ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                ফেরত USEC_INFINITY;

        ফেরত
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(ধ্রুবক কাঠামো timespec *ts) {
        প্রতিজ্ঞা(ts);

        যদি (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ফেরত NSEC_INFINITY;

        যদি ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                ফেরত NSEC_INFINITY;

        ফেরত (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

কাঠামো timespec *timespec_store(কাঠামো timespec *ts, usec_t u) {
        প্রতিজ্ঞা(ts);

        যদি (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ফেরত ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        ফেরত ts;
}

কাঠামো timespec *timespec_store_nsec(কাঠামো timespec *ts, nsec_t n) {
        প্রতিজ্ঞা(ts);

        যদি (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ফেরত ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        ফেরত ts;
}

কাঠামো __kernel_timespec *kernel_timespec_store(কাঠামো __kernel_timespec *ts, usec_t u) {
        প্রতিজ্ঞা(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        প্রতিজ্ঞা(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        ফেরত ts;
}

usec_t timeval_load(ধ্রুবক কাঠামো timeval *tv) {
        প্রতিজ্ঞা(tv);

        যদি (tv->tv_sec < 0 || tv->tv_usec < 0)
                ফেরত USEC_INFINITY;

        যদি ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                ফেরত USEC_INFINITY;

        ফেরত
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

কাঠামো timeval *timeval_store(কাঠামো timeval *tv, usec_t u) {
        প্রতিজ্ঞা(tv);

        যদি (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } অন্যথা {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        ফেরত tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
স্থির ধ্রুবক অক্ষর *ধ্রুবক weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, পূর্ণসংখ্যা);

অক্ষর* format_timestamp_style(
                অক্ষর *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        কাঠামো tm tm;
        bool utc, us;
        size_t n;

        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(style >= 0);
        প্রতিজ্ঞা(style < _TIMESTAMP_STYLE_MAX);

        যদি (!timestamp_is_set(t))
                ফেরত ফাঁকা; /* Timestamp is unset */

        যদি (style == TIMESTAMP_UNIX) {
                যদি (l < (size_t) (1 + 1 + 1))
                        ফেরত ফাঁকা; /* not enough space for even the shortest of forms */

                ফেরত snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        যদি (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                ফেরত ফাঁকা; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        যদি (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                স্থির ধ্রুবক অক্ষর* ধ্রুবক xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                প্রতিজ্ঞা(l >= strlen(xxx[style]) + 1);
                ফেরত strcpy(buf, xxx[style]);
        }

        যদি (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                ফেরত ফাঁকা;

        /* Start with the week day */
        ধ্রুবক অক্ষর *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        প্রতিজ্ঞা(weekday);
        memcpy(buf, weekday, 4);

        যদি (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                যদি (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        ফেরত ফাঁকা; /* Doesn't fit */

                ফেরত buf;
        }

        /* Add the main components */
        যদি (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                ফেরত ফাঁকা; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        যদি (us) {
                n = strlen(buf);
                যদি (n + 8 > l)
                        ফেরত ফাঁকা; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        যদি (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                যদি (n + 5 > l)
                        ফেরত ফাঁকা; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } অন্যথা যদি (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                যদি (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        যদি (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                ফেরত ফাঁকা; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } অন্যথা {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        ফেরত buf;
}

অক্ষর* format_timestamp_relative_full(অক্ষর *buf, size_t l, usec_t t, clockid_t clock, bool implicit_বাম) {
        ধ্রুবক অক্ষর *s;
        usec_t n, d;

        প্রতিজ্ঞা(buf);

        যদি (!timestamp_is_set(t))
                ফেরত ফাঁকা;

        n = now(clock);
        যদি (n > t) {
                d = n - t;
                s = " ago";
        } অন্যথা {
                d = t - n;
                s = implicit_বাম ? "" : " left";
        }

        যদি (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (শূন্য) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } অন্যথা যদি (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (শূন্য) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } অন্যথা যদি (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (শূন্য) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } অন্যথা যদি (d >= 2*USEC_PER_DAY)
                (শূন্য) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        অন্যথা যদি (d >= 25*USEC_PER_HOUR)
                (শূন্য) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        অন্যথা যদি (d >= 6*USEC_PER_HOUR)
                (শূন্য) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        অন্যথা যদি (d >= USEC_PER_HOUR)
                (শূন্য) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        অন্যথা যদি (d >= 5*USEC_PER_MINUTE)
                (শূন্য) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        অন্যথা যদি (d >= USEC_PER_MINUTE)
                (শূন্য) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        অন্যথা যদি (d >= USEC_PER_SEC)
                (শূন্য) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        অন্যথা যদি (d >= USEC_PER_MSEC)
                (শূন্য) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        অন্যথা যদি (d > 0)
                (শূন্য) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        অন্যথা
                (শূন্য) snprintf(buf, l, "now");

        buf[l-1] = 0;
        ফেরত buf;
}

অক্ষর* format_timespan(অক্ষর *buf, size_t l, usec_t t, usec_t accuracy) {
        স্থির ধ্রুবক কাঠামো {
                ধ্রুবক অক্ষর *suffix;
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

        অক্ষর *p = ASSERT_PTR(buf);
        bool something = false;

        প্রতিজ্ঞা(l > 0);

        যদি (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                ফেরত p;
        }

        যদি (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                ফেরত p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                পূর্ণসংখ্যা k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                যদি (t <= 0)
                        বিরতি;

                যদি (t < accuracy && something)
                        বিরতি;

                যদি (t < i->usec)
                        চালু;

                যদি (l <= 1)
                        বিরতি;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                যদি (t < USEC_PER_MINUTE && b > 0) {
                        signed অক্ষর j = 0;

                        জন্য (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        জন্য (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        যদি (j > 0) {
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
                যদি (!done) {
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

        ফেরত buf;
}

ধ্রুবক অক্ষর* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        যদি (dst && isempty(tzname[1]))
                dst = false;

        ফেরত empty_to_null(tzname[dst]);
}

পূর্ণসংখ্যা parse_gmtoff(ধ্রুবক অক্ষর *t, long *ret) {
        প্রতিজ্ঞা(t);

        কাঠামো tm tm;
        ধ্রুবক অক্ষর *k = strptime(t, "%z", &tm);
        যদি (k && *k == '\0') {
                /* Success! */
                যদি (ret)
                        *ret = tm.tm_gmtoff;
                ফেরত 0;
        }

#ifdef __GLIBC__
        ফেরত -EINVAL;
#অন্যথা
        পূর্ণসংখ্যা r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        নির্বাচন (*t) {
        ক্ষেত্র '+':
                positive = true;
                বিরতি;
        ক্ষেত্র '-':
                positive = false;
                বিরতি;
        পূর্বনির্ধারিত:
                ফেরত -EINVAL;
        }

        t++;
        r = undecchar(*t);
        যদি (r < 0)
                ফেরত r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        যদি (r < 0)
                ফেরত r;
        u += r * USEC_PER_HOUR;

        t++;
        যদি (*t == '\0') /* 2 digits case */
                যাও finalize;

        যদি (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        যদি (r < 0)
                ফেরত r;
        যদি (r >= 6) /* refuse minutes equal to or larger than 60 */
                ফেরত -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        যদি (r < 0)
                ফেরত r;

        u += r * USEC_PER_MINUTE;

        t++;
        যদি (*t != '\0')
                ফেরত -EINVAL;

finalize:
        যদি (u > USEC_PER_DAY) /* refuse larger than one day */
                ফেরত -EINVAL;

        যদি (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        ফেরত 0;
#endif
}

স্থির পূর্ণসংখ্যা parse_timestamp_impl(
                ধ্রুবক অক্ষর *t,
                size_t max_len,
                bool utc,
                পূর্ণসংখ্যা isdst,
                long gmtoff,
                usec_t *ret) {

        স্থির ধ্রুবক কাঠামো {
                ধ্রুবক অক্ষর *name;
                ধ্রুবক পূর্ণসংখ্যা nr;
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

        _cleanup_free_ অক্ষর *t_alloc = ফাঁকা;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        পূর্ণসংখ্যা r, weekday = -1;
        unsigned fractional = 0;
        ধ্রুবক অক্ষর *k;
        কাঠামো tm tm, copy;

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

        প্রতিজ্ঞা(t);

        যদি (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                যদি (max_len == 0) /* Can't be the only field */
                        ফেরত -EINVAL;

                t_alloc = strndup(t, max_len);
                যদি (!t_alloc)
                        ফেরত -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        যদি (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                যদি ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        ফেরত -EINVAL;
        } অন্যথা {
                যদি (gmtoff != 0)
                        ফেরত -EINVAL;
        }

        যদি (t[0] == '@' && !with_tz)
                ফেরত parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        যদি (!with_tz) {
                যদি (streq(t, "now"))
                        যাও finish;

                যদি (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        যদি (r < 0)
                                ফেরত r;

                        যাও finish;
                }

                যদি (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        যদি (r < 0)
                                ফেরত r;

                        যাও finish;
                }

                যদি ((k = endswith(t, " ago"))) {
                        _cleanup_free_ অক্ষর *buf = ফাঁকা;

                        buf = strndup(t, k - t);
                        যদি (!buf)
                                ফেরত -ENOMEM;

                        r = parse_sec(buf, &minus);
                        যদি (r < 0)
                                ফেরত r;

                        যাও finish;
                }

                যদি ((k = endswith(t, " left"))) {
                        _cleanup_free_ অক্ষর *buf = ফাঁকা;

                        buf = strndup(t, k - t);
                        যদি (!buf)
                                ফেরত -ENOMEM;

                        r = parse_sec(buf, &plus);
                        যদি (r < 0)
                                ফেরত r;

                        যাও finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        যদি (r < 0)
                ফেরত r;

        tm.tm_isdst = isdst;

        যদি (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                যাও from_tm;

        } অন্যথা যদি (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                যাও from_tm;

        } অন্যথা যদি (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                যাও from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                যদি (!k || *k != ' ')
                        চালু;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                বিরতি;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        যাও parse_usec;
                অন্যথা যদি (*k == 0)
                        যাও from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        যাও parse_usec;
                অন্যথা যদি (*k == 0)
                        যাও from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        যাও parse_usec;
                অন্যথা যদি (*k == 0)
                        যাও from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        যাও parse_usec;
                অন্যথা যদি (*k == 0)
                        যাও from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = 0;
                যাও from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = 0;
                যাও from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = 0;
                যাও from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                যাও from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                যাও from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        যাও parse_usec;
                অন্যথা যদি (*k == 0)
                        যাও from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = 0;
                যাও from_tm;
        }

        ফেরত -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        যদি (r < 0)
                ফেরত -EINVAL;
        যদি (*k != '\0')
                ফেরত -EINVAL;

from_tm:
        প্রতিজ্ঞা(plus == 0);
        প্রতিজ্ঞা(minus == 0);

        যদি (weekday >= 0 && tm.tm_wday != weekday)
                ফেরত -EINVAL;

        যদি (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                যদি (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } অন্যথা
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        যদি (r < 0)
                ফেরত r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        যদি (usec < minus)
                ফেরত -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        যদি (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                ফেরত -EINVAL;

        যদি (ret)
                *ret = usec;
        ফেরত 0;
}

পূর্ণসংখ্যা parse_timestamp(ধ্রুবক অক্ষর *t, usec_t *ret) {
        long gmtoff;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(t);

        size_t t_len = strlen(t);
        যদি (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                যদি (r >= 0)
                        ফেরত r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        যদি (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                ফেরত parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ধ্রুবক অক্ষর *tz = strrchr(t, ' ');
        যদি (!tz)
                ফেরত parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        যদি (streq(tz, "UTC"))
                ফেরত parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        যদি (parse_gmtoff(tz, &gmtoff) >= 0)
                ফেরত parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        জন্য (পূর্ণসংখ্যা j = 0; j <= 1; j++) {
                যদি (!streq_ptr(tz, get_tzname(j)))
                        চালু;

                /* The specified timezone matches tzname[] of the local timezone. */
                ফেরত parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        যদি (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                যদি (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        ফেরত negative_errno();

                ফেরত parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        ফেরত parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

স্থির ধ্রুবক অক্ষর* extract_multiplier(ধ্রুবক অক্ষর *p, usec_t *ret) {
        স্থির ধ্রুবক কাঠামো {
                ধ্রুবক অক্ষর *suffix;
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

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        FOREACH_ELEMENT(i, table) {
                ধ্রুবক অক্ষর *e = startswith(p, i->suffix);
                যদি (e) {
                        *ret = i->usec;
                        ফেরত e;
                }
        }

        ফেরত p;
}

পূর্ণসংখ্যা parse_time(ধ্রুবক অক্ষর *t, usec_t *ret, usec_t default_unit) {
        ধ্রুবক অক্ষর *p, *s;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ ফাঁকা);
        s = startswith(p, "infinity");
        যদি (s) {
                যদি (!in_charset(s, WHITESPACE))
                        ফেরত -EINVAL;

                যদি (ret)
                        *ret = USEC_INFINITY;
                ফেরত 0;
        }

        usec_t usec = 0;

        জন্য (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                অক্ষর *e;

                p = skip_leading_chars(p, /* bad= */ ফাঁকা);
                যদি (*p == 0) {
                        যদি (!something)
                                ফেরত -EINVAL;

                        বিরতি;
                }

                যদি (*p == '-') /* Don't allow "-0" */
                        ফেরত -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                যদি (errno > 0)
                        ফেরত -errno;
                যদি (l < 0)
                        ফেরত -ERANGE;

                যদি (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } অন্যথা যদি (e == p)
                        ফেরত -EINVAL;
                অন্যথা
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                যদি (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ফেরত -EINVAL;

                p = s;

                যদি ((usec_t) l >= USEC_INFINITY / multiplier)
                        ফেরত -ERANGE;

                k = (usec_t) l * multiplier;
                যদি (k >= USEC_INFINITY - usec)
                        ফেরত -ERANGE;

                usec += k;

                something = true;

                যদি (*e == '.') {
                        usec_t m = multiplier / 10;
                        ধ্রুবক অক্ষর *b;

                        জন্য (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                যদি (k >= USEC_INFINITY - usec)
                                        ফেরত -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        যদি (b == e + 1)
                                ফেরত -EINVAL;
                }
        }

        যদি (ret)
                *ret = usec;
        ফেরত 0;
}

পূর্ণসংখ্যা parse_sec(ধ্রুবক অক্ষর *t, usec_t *ret) {
        ফেরত parse_time(t, ret, USEC_PER_SEC);
}

পূর্ণসংখ্যা parse_sec_fix_0(ধ্রুবক অক্ষর *t, usec_t *ret) {
        usec_t k;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(ret);

        r = parse_sec(t, &k);
        যদি (r < 0)
                ফেরত r;

        *ret = k == 0 ? USEC_INFINITY : k;
        ফেরত r;
}

পূর্ণসংখ্যা parse_sec_def_infinity(ধ্রুবক অক্ষর *t, usec_t *ret) {
        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(ret);

        t += strspn(t, WHITESPACE);
        যদি (isempty(t)) {
                *ret = USEC_INFINITY;
                ফেরত 0;
        }
        ফেরত parse_sec(t, ret);
}

স্থির ধ্রুবক অক্ষর* extract_nsec_multiplier(ধ্রুবক অক্ষর *p, nsec_t *ret) {
        স্থির ধ্রুবক কাঠামো {
                ধ্রুবক অক্ষর *suffix;
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

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        FOREACH_ELEMENT(i, table) {
                ধ্রুবক অক্ষর *e = startswith(p, i->suffix);
                যদি (e) {
                        *ret = i->nsec;
                        ফেরত e;
                }
        }

        ফেরত p;
}

পূর্ণসংখ্যা parse_nsec(ধ্রুবক অক্ষর *t, nsec_t *ret) {
        ধ্রুবক অক্ষর *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        যদি (s) {
                s += strspn(s, WHITESPACE);
                যদি (*s != 0)
                        ফেরত -EINVAL;

                *ret = NSEC_INFINITY;
                ফেরত 0;
        }

        জন্য (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                অক্ষর *e;

                p += strspn(p, WHITESPACE);

                যদি (*p == 0) {
                        যদি (!something)
                                ফেরত -EINVAL;

                        বিরতি;
                }

                যদি (*p == '-') /* Don't allow "-0" */
                        ফেরত -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                যদি (errno > 0)
                        ফেরত -errno;
                যদি (l < 0)
                        ফেরত -ERANGE;

                যদি (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } অন্যথা যদি (e == p)
                        ফেরত -EINVAL;
                অন্যথা
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                যদি (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ফেরত -EINVAL;

                p = s;

                যদি ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        ফেরত -ERANGE;

                k = (nsec_t) l * multiplier;
                যদি (k >= NSEC_INFINITY - nsec)
                        ফেরত -ERANGE;

                nsec += k;

                something = true;

                যদি (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ধ্রুবক অক্ষর *b;

                        জন্য (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                যদি (k >= NSEC_INFINITY - nsec)
                                        ফেরত -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        যদি (b == e + 1)
                                ফেরত -EINVAL;
                }
        }

        *ret = nsec;

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা get_timezones_from_zone1970_tab(অক্ষর ***ret) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        _cleanup_strv_free_ অক্ষর **zones = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        যদি (!f)
                ফেরত -errno;

        জন্য (;;) {
                _cleanup_free_ অক্ষর *সারি = ফাঁকা, *cc = ফাঁকা, *co = ফাঁকা, *tz = ফাঁকা;

                r = read_সারি(f, LONG_সারি_MAX, &সারি);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                ধ্রুবক অক্ষর *p = সারি;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_শব্দগুলি(&p, ফাঁকা, 0, &cc, &co, &tz);
                যদি (r < 0)
                        চালু;

                /* Lines that start with # are comments. */
                যদি (*cc == '#')
                        চালু;

                যদি (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        চালু;

                r = strv_extend(&zones, tz);
                যদি (r < 0)
                        ফেরত r;
        }

        *ret = TAKE_PTR(zones);
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা get_timezones_from_tzdata_zi(অক্ষর ***ret) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        _cleanup_strv_free_ অক্ষর **zones = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        যদি (!f)
                ফেরত -errno;

        জন্য (;;) {
                _cleanup_free_ অক্ষর *সারি = ফাঁকা, *type = ফাঁকা, *f1 = ফাঁকা, *f2 = ফাঁকা;

                r = read_সারি(f, LONG_সারি_MAX, &সারি);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                ধ্রুবক অক্ষর *p = সারি;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_শব্দগুলি(&p, ফাঁকা, 0, &type, &f1, &f2);
                যদি (r < 0)
                        চালু;

                অক্ষর *tz;
                যদি (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                অন্যথা যদি (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                অন্যথা
                        /* Not a line we care about. */
                        চালু;

                যদি (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        চালু;

                r = strv_extend(&zones, tz);
                যদি (r < 0)
                        ফেরত r;
        }

        *ret = TAKE_PTR(zones);
        ফেরত 0;
}

পূর্ণসংখ্যা get_timezones(অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **zones = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        যদি (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                যদি (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        যদি (r < 0 && r != -ENOENT)
                ফেরত r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        যদি (r < 0)
                ফেরত r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        ফেরত 0;
}

পূর্ণসংখ্যা verify_timezone(ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা log_level) {
        bool slash = false;
        ধ্রুবক অক্ষর *p, *t;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        অক্ষর buf[4];
        পূর্ণসংখ্যা r;

        যদি (isempty(name))
                ফেরত -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        যদি (streq(name, "UTC"))
                ফেরত 0;

        যদি (name[0] == '/')
                ফেরত -EINVAL;

        জন্য (p = name; *p; p++) {
                যদি (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        ফেরত -EINVAL;

                যদি (*p == '/') {

                        যদি (slash)
                                ফেরত -EINVAL;

                        slash = true;
                } অন্যথা
                        slash = false;
        }

        যদি (slash)
                ফেরত -EINVAL;

        যদি (p - name >= PATH_MAX)
                ফেরত -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        যদি (fd < 0)
                ফেরত log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        যদি (r < 0)
                ফেরত log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        যদি (r < 0)
                ফেরত log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        যদি (memcmp(buf, "TZif", 4) != 0)
                ফেরত log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        ফেরত 0;
}

শূন্য reset_timezonep(অক্ষর **p) {
        প্রতিজ্ঞা(p);

        (শূন্য) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

অক্ষর* save_timezone(শূন্য) {
        ধ্রুবক অক্ষর *e = getenv("TZ");
        যদি (!e)
                ফেরত ফাঁকা;

        অক্ষর *s = strdup(e);
        যদি (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        ফেরত s;
}

bool clock_supported(clockid_t clock) {
        কাঠামো timespec ts;

        নির্বাচন (clock) {

        ক্ষেত্র CLOCK_MONOTONIC:
        ক্ষেত্র CLOCK_REALTIME:
        ক্ষেত্র CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                ফেরত true;

        পূর্বনির্ধারিত:
                /* For everything else, check properly */
                ফেরত clock_gettime(clock, &ts) >= 0;
        }
}

পূর্ণসংখ্যা get_timezone(অক্ষর **ret) {
        _cleanup_free_ অক্ষর *t = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = readlink_malloc(etc_localtime(), &t);
        যদি (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                ফেরত strdup_to(ret, "UTC");
        যদি (r < 0)
                ফেরত r; /* Return EINVAL if not a symlink */

        ধ্রুবক অক্ষর *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        যদি (!e)
                ফেরত -EINVAL;
        যদি (!timezone_is_valid(e, LOG_DEBUG))
                ফেরত -EINVAL;

        ফেরত strdup_to(ret, e);
}

পূর্ণসংখ্যা get_timezone_prefer_env(অক্ষর **ret) {
        প্রতিজ্ঞা(ret);

        ধ্রুবক অক্ষর *e = getenv("TZ");
        যদি (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                ফেরত strdup_to(ret, e + 1);

        ফেরত get_timezone(ret);
}

ধ্রুবক অক্ষর* etc_localtime(শূন্য) {
        স্থির ধ্রুবক অক্ষর *cached = ফাঁকা;

        যদি (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        ফেরত cached;
}

পূর্ণসংখ্যা mktime_or_timegm_usec(
                কাঠামো tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        প্রতিজ্ঞা(tm);

        যদি (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                ফেরত -ERANGE;
        যদি ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                ফেরত -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        যদি (t < 0) /* Refuse negative times and errors */
                ফেরত -ERANGE;
        যদি ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                ফেরত -ERANGE;

        যদি (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        ফেরত 0;
}

পূর্ণসংখ্যা localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                কাঠামো tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        যদি (t > (usec_t) TIME_T_MAX)
                ফেরত -ERANGE;
        time_t sec = (time_t) t;

        কাঠামো tm buf = {};
        যদি (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                ফেরত -EINVAL;

        যদি (ret)
                *ret = buf;

        ফেরত 0;
}

uint64_t sysconf_clock_ticks_cached(শূন্য) {
        স্থির thread_local uint64_t hz = 0;

        যদি (hz != 0)
                ফেরত hz;

        long t = sysconf(_SC_CLK_TCK);
        প্রতিজ্ঞা(t > 0);
        hz = (uint64_t) t;

        ফেরত hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ফেরত DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ফেরত DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        যদি (x == USEC_INFINITY)
                ফেরত USEC_INFINITY;
        যদি (map_clock_id(from) == map_clock_id(to))
                ফেরত x;

        a = now(from);
        b = now(to);

        যদি (x > a)
                /* x lies in the future */
                ফেরত usec_add(b, usec_sub_unsigned(x, a));
        অন্যথা
                /* x lies in the past */
                ফেরত usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(শূন্য) {
        tzset();

        ফেরত timezone == 0 && daylight == 0;
}

পূর্ণসংখ্যা usleep_safe(usec_t usec) {
        পূর্ণসংখ্যা r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        যদি (usec == 0)
                ফেরত 0;

        যদি (usec == USEC_INFINITY)
                ফেরত RET_NERRNO(pause());

        কাঠামো timespec t;
        timespec_store(&t, usec);

        জন্য (;;) {
                কাঠামো timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                যদি (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        চালু;
                }

                ফেরত r;
        }
}

পূর্ণসংখ্যা time_change_fd(শূন্য) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        স্থির ধ্রুবক কাঠামো itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        যদি (fd < 0)
                ফেরত -errno;

        যদি (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, ফাঁকা) >= 0)
                ফেরত TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#যদি SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        যদি (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                স্থির ধ্রুবক কাঠামো itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                যদি (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, ফাঁকা) >= 0)
                        ফেরত TAKE_FD(fd);
        }
#endif

        ফেরত -errno;
}

স্থির ধ্রুবক অক্ষর* ধ্রুবক timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ধ্রুবক অক্ষর *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        যদি (t >= 0)
                ফেরত t;
        যদি (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                ফেরত TIMESTAMP_US;
        যদি (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                ফেরত TIMESTAMP_US_UTC;
        ফেরত t;
}

পূর্ণসংখ্যা parse_calendar_date_full(ধ্রুবক অক্ষর *s, bool allow_pre_epoch, usec_t *ret_usec, কাঠামো tm *ret_tm) {
        কাঠামো tm parsed_tm = {}, copy_tm;
        ধ্রুবক অক্ষর *k;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        যদি (!k || *k)
                ফেরত -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        যদি (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                যদি (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        ফেরত -EINVAL;
        } অন্যথা {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                যদি (r < 0)
                        ফেরত r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        যদি (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                ফেরত -EINVAL;

        যদি (ret_usec)
                *ret_usec = usec;
        যদি (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        ফেরত 0;
}
