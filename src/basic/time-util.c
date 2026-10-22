/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <linux/time_types.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <sys/timerfd.h>
#অন্তর্ভাৱ <threads.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "env-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "hexdecoct.h"          /* IWYU pragma: keep */
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "fs-util.h"
#অন্তর্ভাৱ "io-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-table.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "time-util.h"

স্থির clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        ৱিকল্প (c) {

        পক্ষ CLOCK_BOOTTIME_ALARM:
                ঘূৰাই CLOCK_BOOTTIME;

        পক্ষ CLOCK_REALTIME_ALARM:
                ঘূৰাই CLOCK_REALTIME;

        স্ৱভাৱ:
                ঘূৰাই c;
        }
}

usec_t now(clockid_t clock_id) {
        সংরচনা timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        প্রতিজ্ঞা(n > 0);
        প্রতিজ্ঞা(n < USEC_INFINITY);

        ঘূৰাই n;
}

nsec_t now_nsec(clockid_t clock_id) {
        সংরচনা timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        প্রতিজ্ঞা(n > 0);
        প্রতিজ্ঞা(n < NSEC_INFINITY);

        ঘূৰাই n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        প্রতিজ্ঞা(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        ঘূৰাই ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        প্রতিজ্ঞা(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        ঘূৰাই ts;
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
                        ঘূৰাই USEC_INFINITY;

                ঘূৰাই to_base + delta;

        } অন্যথা { /* In the past */
                usec_t delta = from_base - from;

                যদি (to_base <= delta) /* underflow? */
                        ঘূৰাই 0;

                ঘূৰাই to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        যদি (map_clock_id(from_clock) == map_clock_id(to_clock))
                ঘূৰাই from;

        /* Keep infinity as is */
        যদি (from == USEC_INFINITY)
                ঘূৰাই from;

        ঘূৰাই map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        প্রতিজ্ঞা(ts);

        যদি (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                ঘূৰাই ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        ঘূৰাই ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        প্রতিজ্ঞা(ts);

        যদি (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ঘূৰাই ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        ঘূৰাই ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        প্রতিজ্ঞা(ts);

        যদি (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ঘূৰাই ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        ঘূৰাই ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        প্রতিজ্ঞা(ts);

        যদি (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ঘূৰাই ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        ঘূৰাই ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        প্রতিজ্ঞা(ts);

        যদি (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ঘূৰাই ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        ঘূৰাই ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        প্রতিজ্ঞা(ts);

        ৱিকল্প (clock) {

        পক্ষ CLOCK_REALTIME:
        পক্ষ CLOCK_REALTIME_ALARM:
                ঘূৰাই ts->realtime;

        পক্ষ CLOCK_MONOTONIC:
                ঘূৰাই ts->monotonic;

        পক্ষ CLOCK_BOOTTIME:
        পক্ষ CLOCK_BOOTTIME_ALARM:
                ঘূৰাই ts->boottime;

        স্ৱভাৱ:
                ঘূৰাই USEC_INFINITY;
        }
}

usec_t timespec_load(ধ্রুৱ সংরচনা timespec *ts) {
        প্রতিজ্ঞা(ts);

        যদি (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ঘূৰাই USEC_INFINITY;

        যদি ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                ঘূৰাই USEC_INFINITY;

        ঘূৰাই
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(ধ্রুৱ সংরচনা timespec *ts) {
        প্রতিজ্ঞা(ts);

        যদি (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ঘূৰাই NSEC_INFINITY;

        যদি ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                ঘূৰাই NSEC_INFINITY;

        ঘূৰাই (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

সংরচনা timespec *timespec_store(সংরচনা timespec *ts, usec_t u) {
        প্রতিজ্ঞা(ts);

        যদি (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ঘূৰাই ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        ঘূৰাই ts;
}

সংরচনা timespec *timespec_store_nsec(সংরচনা timespec *ts, nsec_t n) {
        প্রতিজ্ঞা(ts);

        যদি (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ঘূৰাই ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        ঘূৰাই ts;
}

সংরচনা __kernel_timespec *kernel_timespec_store(সংরচনা __kernel_timespec *ts, usec_t u) {
        প্রতিজ্ঞা(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        প্রতিজ্ঞা(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        ঘূৰাই ts;
}

usec_t timeval_load(ধ্রুৱ সংরচনা timeval *tv) {
        প্রতিজ্ঞা(tv);

        যদি (tv->tv_sec < 0 || tv->tv_usec < 0)
                ঘূৰাই USEC_INFINITY;

        যদি ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                ঘূৰাই USEC_INFINITY;

        ঘূৰাই
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

সংরচনা timeval *timeval_store(সংরচনা timeval *tv, usec_t u) {
        প্রতিজ্ঞা(tv);

        যদি (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } অন্যথা {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        ঘূৰাই tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
স্থির ধ্রুৱ ৱর্ণ *ধ্রুৱ weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, পূর্ণাঙ্ক);

ৱর্ণ* format_timestamp_style(
                ৱর্ণ *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        সংরচনা tm tm;
        bool utc, us;
        size_t n;

        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(style >= 0);
        প্রতিজ্ঞা(style < _TIMESTAMP_STYLE_MAX);

        যদি (!timestamp_is_set(t))
                ঘূৰাই রিক্ত; /* Timestamp is unset */

        যদি (style == TIMESTAMP_UNIX) {
                যদি (l < (size_t) (1 + 1 + 1))
                        ঘূৰাই রিক্ত; /* not enough space for even the shortest of forms */

                ঘূৰাই snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
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
                ঘূৰাই রিক্ত; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        যদি (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                স্থির ধ্রুৱ ৱর্ণ* ধ্রুৱ xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                প্রতিজ্ঞা(l >= strlen(xxx[style]) + 1);
                ঘূৰাই strcpy(buf, xxx[style]);
        }

        যদি (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                ঘূৰাই রিক্ত;

        /* Start with the week day */
        ধ্রুৱ ৱর্ণ *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        প্রতিজ্ঞা(weekday);
        memcpy(buf, weekday, 4);

        যদি (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                যদি (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        ঘূৰাই রিক্ত; /* Doesn't fit */

                ঘূৰাই buf;
        }

        /* Add the main components */
        যদি (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                ঘূৰাই রিক্ত; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        যদি (us) {
                n = strlen(buf);
                যদি (n + 8 > l)
                        ঘূৰাই রিক্ত; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        যদি (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                যদি (n + 5 > l)
                        ঘূৰাই রিক্ত; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } অন্যথা যদি (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                যদি (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        যদি (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                ঘূৰাই রিক্ত; /* Not even enough space for the POSIX minimum (of 6)? In that
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

        ঘূৰাই buf;
}

ৱর্ণ* format_timestamp_relative_full(ৱর্ণ *buf, size_t l, usec_t t, clockid_t clock, bool implicit_বাম) {
        ধ্রুৱ ৱর্ণ *s;
        usec_t n, d;

        প্রতিজ্ঞা(buf);

        যদি (!timestamp_is_set(t))
                ঘূৰাই রিক্ত;

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
        ঘূৰাই buf;
}

ৱর্ণ* format_timespan(ৱর্ণ *buf, size_t l, usec_t t, usec_t accuracy) {
        স্থির ধ্রুৱ সংরচনা {
                ধ্রুৱ ৱর্ণ *suffix;
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

        ৱর্ণ *p = ASSERT_PTR(buf);
        bool something = false;

        প্রতিজ্ঞা(l > 0);

        যদি (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                ঘূৰাই p;
        }

        যদি (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                ঘূৰাই p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                পূর্ণাঙ্ক k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                যদি (t <= 0)
                        ভঙ্গ;

                যদি (t < accuracy && something)
                        ভঙ্গ;

                যদি (t < i->usec)
                        অনুৱর্ত;

                যদি (l <= 1)
                        ভঙ্গ;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                যদি (t < USEC_PER_MINUTE && b > 0) {
                        signed ৱর্ণ j = 0;

                        বাবে (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        বাবে (usec_t cc = accuracy; cc > 1; cc /= 10) {
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

        ঘূৰাই buf;
}

ধ্রুৱ ৱর্ণ* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        যদি (dst && isempty(tzname[1]))
                dst = false;

        ঘূৰাই empty_to_null(tzname[dst]);
}

পূর্ণাঙ্ক parse_gmtoff(ধ্রুৱ ৱর্ণ *t, long *ret) {
        প্রতিজ্ঞা(t);

        সংরচনা tm tm;
        ধ্রুৱ ৱর্ণ *k = strptime(t, "%z", &tm);
        যদি (k && *k == '\0') {
                /* Success! */
                যদি (ret)
                        *ret = tm.tm_gmtoff;
                ঘূৰাই 0;
        }

#ifdef __GLIBC__
        ঘূৰাই -EINVAL;
#অন্যথা
        পূর্ণাঙ্ক r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        ৱিকল্প (*t) {
        পক্ষ '+':
                positive = true;
                ভঙ্গ;
        পক্ষ '-':
                positive = false;
                ভঙ্গ;
        স্ৱভাৱ:
                ঘূৰাই -EINVAL;
        }

        t++;
        r = undecchar(*t);
        যদি (r < 0)
                ঘূৰাই r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        যদি (r < 0)
                ঘূৰাই r;
        u += r * USEC_PER_HOUR;

        t++;
        যদি (*t == '\0') /* 2 digits case */
                গচ্ছ finalize;

        যদি (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r >= 6) /* refuse minutes equal to or larger than 60 */
                ঘূৰাই -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        যদি (r < 0)
                ঘূৰাই r;

        u += r * USEC_PER_MINUTE;

        t++;
        যদি (*t != '\0')
                ঘূৰাই -EINVAL;

finalize:
        যদি (u > USEC_PER_DAY) /* refuse larger than one day */
                ঘূৰাই -EINVAL;

        যদি (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        ঘূৰাই 0;
#endif
}

স্থির পূর্ণাঙ্ক parse_timestamp_impl(
                ধ্রুৱ ৱর্ণ *t,
                size_t max_len,
                bool utc,
                পূর্ণাঙ্ক isdst,
                long gmtoff,
                usec_t *ret) {

        স্থির ধ্রুৱ সংরচনা {
                ধ্রুৱ ৱর্ণ *name;
                ধ্রুৱ পূর্ণাঙ্ক nr;
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

        _cleanup_free_ ৱর্ণ *t_alloc = রিক্ত;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        পূর্ণাঙ্ক r, weekday = -1;
        unsigned fractional = 0;
        ধ্রুৱ ৱর্ণ *k;
        সংরচনা tm tm, copy;

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
                        ঘূৰাই -EINVAL;

                t_alloc = strndup(t, max_len);
                যদি (!t_alloc)
                        ঘূৰাই -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        যদি (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                যদি ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        ঘূৰাই -EINVAL;
        } অন্যথা {
                যদি (gmtoff != 0)
                        ঘূৰাই -EINVAL;
        }

        যদি (t[0] == '@' && !with_tz)
                ঘূৰাই parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        যদি (!with_tz) {
                যদি (streq(t, "now"))
                        গচ্ছ finish;

                যদি (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        যদি (r < 0)
                                ঘূৰাই r;

                        গচ্ছ finish;
                }

                যদি (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        যদি (r < 0)
                                ঘূৰাই r;

                        গচ্ছ finish;
                }

                যদি ((k = endswith(t, " ago"))) {
                        _cleanup_free_ ৱর্ণ *buf = রিক্ত;

                        buf = strndup(t, k - t);
                        যদি (!buf)
                                ঘূৰাই -ENOMEM;

                        r = parse_sec(buf, &minus);
                        যদি (r < 0)
                                ঘূৰাই r;

                        গচ্ছ finish;
                }

                যদি ((k = endswith(t, " left"))) {
                        _cleanup_free_ ৱর্ণ *buf = রিক্ত;

                        buf = strndup(t, k - t);
                        যদি (!buf)
                                ঘূৰাই -ENOMEM;

                        r = parse_sec(buf, &plus);
                        যদি (r < 0)
                                ঘূৰাই r;

                        গচ্ছ finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        যদি (r < 0)
                ঘূৰাই r;

        tm.tm_isdst = isdst;

        যদি (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                গচ্ছ from_tm;

        } অন্যথা যদি (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                গচ্ছ from_tm;

        } অন্যথা যদি (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                গচ্ছ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                যদি (!k || *k != ' ')
                        অনুৱর্ত;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                ভঙ্গ;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        গচ্ছ parse_usec;
                অন্যথা যদি (*k == 0)
                        গচ্ছ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        গচ্ছ parse_usec;
                অন্যথা যদি (*k == 0)
                        গচ্ছ from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        গচ্ছ parse_usec;
                অন্যথা যদি (*k == 0)
                        গচ্ছ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        গচ্ছ parse_usec;
                অন্যথা যদি (*k == 0)
                        গচ্ছ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = 0;
                গচ্ছ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = 0;
                গচ্ছ from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = 0;
                গচ্ছ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                গচ্ছ from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                গচ্ছ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        যদি (k) {
                যদি (*k == '.')
                        গচ্ছ parse_usec;
                অন্যথা যদি (*k == 0)
                        গচ্ছ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        যদি (k && *k == 0) {
                tm.tm_sec = 0;
                গচ্ছ from_tm;
        }

        ঘূৰাই -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        যদি (r < 0)
                ঘূৰাই -EINVAL;
        যদি (*k != '\0')
                ঘূৰাই -EINVAL;

from_tm:
        প্রতিজ্ঞা(plus == 0);
        প্রতিজ্ঞা(minus == 0);

        যদি (weekday >= 0 && tm.tm_wday != weekday)
                ঘূৰাই -EINVAL;

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
                ঘূৰাই r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        যদি (usec < minus)
                ঘূৰাই -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        যদি (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                ঘূৰাই -EINVAL;

        যদি (ret)
                *ret = usec;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_timestamp(ধ্রুৱ ৱর্ণ *t, usec_t *ret) {
        long gmtoff;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);

        size_t t_len = strlen(t);
        যদি (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                যদি (r >= 0)
                        ঘূৰাই r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        যদি (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                ঘূৰাই parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ধ্রুৱ ৱর্ণ *tz = strrchr(t, ' ');
        যদি (!tz)
                ঘূৰাই parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        যদি (streq(tz, "UTC"))
                ঘূৰাই parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        যদি (parse_gmtoff(tz, &gmtoff) >= 0)
                ঘূৰাই parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        বাবে (পূর্ণাঙ্ক j = 0; j <= 1; j++) {
                যদি (!streq_ptr(tz, get_tzname(j)))
                        অনুৱর্ত;

                /* The specified timezone matches tzname[] of the local timezone. */
                ঘূৰাই parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        যদি (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                যদি (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        ঘূৰাই negative_errno();

                ঘূৰাই parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        ঘূৰাই parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

স্থির ধ্রুৱ ৱর্ণ* extract_multiplier(ধ্রুৱ ৱর্ণ *p, usec_t *ret) {
        স্থির ধ্রুৱ সংরচনা {
                ধ্রুৱ ৱর্ণ *suffix;
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
                ধ্রুৱ ৱর্ণ *e = startswith(p, i->suffix);
                যদি (e) {
                        *ret = i->usec;
                        ঘূৰাই e;
                }
        }

        ঘূৰাই p;
}

পূর্ণাঙ্ক parse_time(ধ্রুৱ ৱর্ণ *t, usec_t *ret, usec_t default_unit) {
        ধ্রুৱ ৱর্ণ *p, *s;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ রিক্ত);
        s = startswith(p, "infinity");
        যদি (s) {
                যদি (!in_charset(s, WHITESPACE))
                        ঘূৰাই -EINVAL;

                যদি (ret)
                        *ret = USEC_INFINITY;
                ঘূৰাই 0;
        }

        usec_t usec = 0;

        বাবে (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                ৱর্ণ *e;

                p = skip_leading_chars(p, /* bad= */ রিক্ত);
                যদি (*p == 0) {
                        যদি (!something)
                                ঘূৰাই -EINVAL;

                        ভঙ্গ;
                }

                যদি (*p == '-') /* Don't allow "-0" */
                        ঘূৰাই -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                যদি (errno > 0)
                        ঘূৰাই -errno;
                যদি (l < 0)
                        ঘূৰাই -ERANGE;

                যদি (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } অন্যথা যদি (e == p)
                        ঘূৰাই -EINVAL;
                অন্যথা
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                যদি (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ঘূৰাই -EINVAL;

                p = s;

                যদি ((usec_t) l >= USEC_INFINITY / multiplier)
                        ঘূৰাই -ERANGE;

                k = (usec_t) l * multiplier;
                যদি (k >= USEC_INFINITY - usec)
                        ঘূৰাই -ERANGE;

                usec += k;

                something = true;

                যদি (*e == '.') {
                        usec_t m = multiplier / 10;
                        ধ্রুৱ ৱর্ণ *b;

                        বাবে (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                যদি (k >= USEC_INFINITY - usec)
                                        ঘূৰাই -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        যদি (b == e + 1)
                                ঘূৰাই -EINVAL;
                }
        }

        যদি (ret)
                *ret = usec;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_sec(ধ্রুৱ ৱর্ণ *t, usec_t *ret) {
        ঘূৰাই parse_time(t, ret, USEC_PER_SEC);
}

পূর্ণাঙ্ক parse_sec_fix_0(ধ্রুৱ ৱর্ণ *t, usec_t *ret) {
        usec_t k;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(ret);

        r = parse_sec(t, &k);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = k == 0 ? USEC_INFINITY : k;
        ঘূৰাই r;
}

পূর্ণাঙ্ক parse_sec_def_infinity(ধ্রুৱ ৱর্ণ *t, usec_t *ret) {
        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(ret);

        t += strspn(t, WHITESPACE);
        যদি (isempty(t)) {
                *ret = USEC_INFINITY;
                ঘূৰাই 0;
        }
        ঘূৰাই parse_sec(t, ret);
}

স্থির ধ্রুৱ ৱর্ণ* extract_nsec_multiplier(ধ্রুৱ ৱর্ণ *p, nsec_t *ret) {
        স্থির ধ্রুৱ সংরচনা {
                ধ্রুৱ ৱর্ণ *suffix;
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
                ধ্রুৱ ৱর্ণ *e = startswith(p, i->suffix);
                যদি (e) {
                        *ret = i->nsec;
                        ঘূৰাই e;
                }
        }

        ঘূৰাই p;
}

পূর্ণাঙ্ক parse_nsec(ধ্রুৱ ৱর্ণ *t, nsec_t *ret) {
        ধ্রুৱ ৱর্ণ *p, *s;
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
                        ঘূৰাই -EINVAL;

                *ret = NSEC_INFINITY;
                ঘূৰাই 0;
        }

        বাবে (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                ৱর্ণ *e;

                p += strspn(p, WHITESPACE);

                যদি (*p == 0) {
                        যদি (!something)
                                ঘূৰাই -EINVAL;

                        ভঙ্গ;
                }

                যদি (*p == '-') /* Don't allow "-0" */
                        ঘূৰাই -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                যদি (errno > 0)
                        ঘূৰাই -errno;
                যদি (l < 0)
                        ঘূৰাই -ERANGE;

                যদি (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } অন্যথা যদি (e == p)
                        ঘূৰাই -EINVAL;
                অন্যথা
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                যদি (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ঘূৰাই -EINVAL;

                p = s;

                যদি ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        ঘূৰাই -ERANGE;

                k = (nsec_t) l * multiplier;
                যদি (k >= NSEC_INFINITY - nsec)
                        ঘূৰাই -ERANGE;

                nsec += k;

                something = true;

                যদি (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ধ্রুৱ ৱর্ণ *b;

                        বাবে (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                যদি (k >= NSEC_INFINITY - nsec)
                                        ঘূৰাই -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        যদি (b == e + 1)
                                ঘূৰাই -EINVAL;
                }
        }

        *ret = nsec;

        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক get_timezones_from_zone1970_tab(ৱর্ণ ***ret) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        _cleanup_strv_free_ ৱর্ণ **zones = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        যদি (!f)
                ঘূৰাই -errno;

        বাবে (;;) {
                _cleanup_free_ ৱর্ণ *সাৰি = রিক্ত, *cc = রিক্ত, *co = রিক্ত, *tz = রিক্ত;

                r = read_সাৰি(f, LONG_সাৰি_MAX, &সাৰি);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0)
                        ভঙ্গ;

                ধ্রুৱ ৱর্ণ *p = সাৰি;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_শব্দগুলি(&p, রিক্ত, 0, &cc, &co, &tz);
                যদি (r < 0)
                        অনুৱর্ত;

                /* Lines that start with # are comments. */
                যদি (*cc == '#')
                        অনুৱর্ত;

                যদি (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        অনুৱর্ত;

                r = strv_extend(&zones, tz);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        *ret = TAKE_PTR(zones);
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক get_timezones_from_tzdata_zi(ৱর্ণ ***ret) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        _cleanup_strv_free_ ৱর্ণ **zones = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        যদি (!f)
                ঘূৰাই -errno;

        বাবে (;;) {
                _cleanup_free_ ৱর্ণ *সাৰি = রিক্ত, *type = রিক্ত, *f1 = রিক্ত, *f2 = রিক্ত;

                r = read_সাৰি(f, LONG_সাৰি_MAX, &সাৰি);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (r == 0)
                        ভঙ্গ;

                ধ্রুৱ ৱর্ণ *p = সাৰি;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_শব্দগুলি(&p, রিক্ত, 0, &type, &f1, &f2);
                যদি (r < 0)
                        অনুৱর্ত;

                ৱর্ণ *tz;
                যদি (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                অন্যথা যদি (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                অন্যথা
                        /* Not a line we care about. */
                        অনুৱর্ত;

                যদি (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        অনুৱর্ত;

                r = strv_extend(&zones, tz);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        *ret = TAKE_PTR(zones);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক get_timezones(ৱর্ণ ***ret) {
        _cleanup_strv_free_ ৱর্ণ **zones = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        যদি (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                যদি (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        যদি (r < 0 && r != -ENOENT)
                ঘূৰাই r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        যদি (r < 0)
                ঘূৰাই r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক verify_timezone(ধ্রুৱ ৱর্ণ *name, পূর্ণাঙ্ক log_level) {
        bool slash = false;
        ধ্রুৱ ৱর্ণ *p, *t;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        ৱর্ণ buf[4];
        পূর্ণাঙ্ক r;

        যদি (isempty(name))
                ঘূৰাই -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        যদি (streq(name, "UTC"))
                ঘূৰাই 0;

        যদি (name[0] == '/')
                ঘূৰাই -EINVAL;

        বাবে (p = name; *p; p++) {
                যদি (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        ঘূৰাই -EINVAL;

                যদি (*p == '/') {

                        যদি (slash)
                                ঘূৰাই -EINVAL;

                        slash = true;
                } অন্যথা
                        slash = false;
        }

        যদি (slash)
                ঘূৰাই -EINVAL;

        যদি (p - name >= PATH_MAX)
                ঘূৰাই -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        যদি (fd < 0)
                ঘূৰাই log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        যদি (r < 0)
                ঘূৰাই log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        যদি (r < 0)
                ঘূৰাই log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        যদি (memcmp(buf, "TZif", 4) != 0)
                ঘূৰাই log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        ঘূৰাই 0;
}

শূন্য reset_timezonep(ৱর্ণ **p) {
        প্রতিজ্ঞা(p);

        (শূন্য) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

ৱর্ণ* save_timezone(শূন্য) {
        ধ্রুৱ ৱর্ণ *e = getenv("TZ");
        যদি (!e)
                ঘূৰাই রিক্ত;

        ৱর্ণ *s = strdup(e);
        যদি (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        ঘূৰাই s;
}

bool clock_supported(clockid_t clock) {
        সংরচনা timespec ts;

        ৱিকল্প (clock) {

        পক্ষ CLOCK_MONOTONIC:
        পক্ষ CLOCK_REALTIME:
        পক্ষ CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                ঘূৰাই true;

        স্ৱভাৱ:
                /* For everything else, check properly */
                ঘূৰাই clock_gettime(clock, &ts) >= 0;
        }
}

পূর্ণাঙ্ক get_timezone(ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *t = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = readlink_malloc(etc_localtime(), &t);
        যদি (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                ঘূৰাই strdup_to(ret, "UTC");
        যদি (r < 0)
                ঘূৰাই r; /* Return EINVAL if not a symlink */

        ধ্রুৱ ৱর্ণ *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        যদি (!e)
                ঘূৰাই -EINVAL;
        যদি (!timezone_is_valid(e, LOG_DEBUG))
                ঘূৰাই -EINVAL;

        ঘূৰাই strdup_to(ret, e);
}

পূর্ণাঙ্ক get_timezone_prefer_env(ৱর্ণ **ret) {
        প্রতিজ্ঞা(ret);

        ধ্রুৱ ৱর্ণ *e = getenv("TZ");
        যদি (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                ঘূৰাই strdup_to(ret, e + 1);

        ঘূৰাই get_timezone(ret);
}

ধ্রুৱ ৱর্ণ* etc_localtime(শূন্য) {
        স্থির ধ্রুৱ ৱর্ণ *cached = রিক্ত;

        যদি (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        ঘূৰাই cached;
}

পূর্ণাঙ্ক mktime_or_timegm_usec(
                সংরচনা tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        প্রতিজ্ঞা(tm);

        যদি (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                ঘূৰাই -ERANGE;
        যদি ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                ঘূৰাই -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        যদি (t < 0) /* Refuse negative times and errors */
                ঘূৰাই -ERANGE;
        যদি ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                ঘূৰাই -ERANGE;

        যদি (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                সংরচনা tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        যদি (t > (usec_t) TIME_T_MAX)
                ঘূৰাই -ERANGE;
        time_t sec = (time_t) t;

        সংরচনা tm buf = {};
        যদি (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                ঘূৰাই -EINVAL;

        যদি (ret)
                *ret = buf;

        ঘূৰাই 0;
}

uint64_t sysconf_clock_ticks_cached(শূন্য) {
        স্থির thread_local uint64_t hz = 0;

        যদি (hz != 0)
                ঘূৰাই hz;

        long t = sysconf(_SC_CLK_TCK);
        প্রতিজ্ঞা(t > 0);
        hz = (uint64_t) t;

        ঘূৰাই hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ঘূৰাই DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ঘূৰাই DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        যদি (x == USEC_INFINITY)
                ঘূৰাই USEC_INFINITY;
        যদি (map_clock_id(from) == map_clock_id(to))
                ঘূৰাই x;

        a = now(from);
        b = now(to);

        যদি (x > a)
                /* x lies in the future */
                ঘূৰাই usec_add(b, usec_sub_unsigned(x, a));
        অন্যথা
                /* x lies in the past */
                ঘূৰাই usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(শূন্য) {
        tzset();

        ঘূৰাই timezone == 0 && daylight == 0;
}

পূর্ণাঙ্ক usleep_safe(usec_t usec) {
        পূর্ণাঙ্ক r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        যদি (usec == 0)
                ঘূৰাই 0;

        যদি (usec == USEC_INFINITY)
                ঘূৰাই RET_NERRNO(pause());

        সংরচনা timespec t;
        timespec_store(&t, usec);

        বাবে (;;) {
                সংরচনা timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                যদি (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        অনুৱর্ত;
                }

                ঘূৰাই r;
        }
}

পূর্ণাঙ্ক time_change_fd(শূন্য) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        স্থির ধ্রুৱ সংরচনা itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        যদি (fd < 0)
                ঘূৰাই -errno;

        যদি (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, রিক্ত) >= 0)
                ঘূৰাই TAKE_FD(fd);

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
                স্থির ধ্রুৱ সংরচনা itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                যদি (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, রিক্ত) >= 0)
                        ঘূৰাই TAKE_FD(fd);
        }
#endif

        ঘূৰাই -errno;
}

স্থির ধ্রুৱ ৱর্ণ* ধ্রুৱ timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ধ্রুৱ ৱর্ণ *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        যদি (t >= 0)
                ঘূৰাই t;
        যদি (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                ঘূৰাই TIMESTAMP_US;
        যদি (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                ঘূৰাই TIMESTAMP_US_UTC;
        ঘূৰাই t;
}

পূর্ণাঙ্ক parse_calendar_date_full(ধ্রুৱ ৱর্ণ *s, bool allow_pre_epoch, usec_t *ret_usec, সংরচনা tm *ret_tm) {
        সংরচনা tm parsed_tm = {}, copy_tm;
        ধ্রুৱ ৱর্ণ *k;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        যদি (!k || *k)
                ঘূৰাই -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        যদি (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                যদি (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        ঘূৰাই -EINVAL;
        } অন্যথা {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        যদি (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                ঘূৰাই -EINVAL;

        যদি (ret_usec)
                *ret_usec = usec;
        যদি (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        ঘূৰাই 0;
}
