/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/time_types.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/timerfd.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"          /* IWYU pragma: keep */
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

ସ୍ଥିର clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        ବାଛ (c) {

        ସ୍ଥିତି CLOCK_BOOTTIME_ALARM:
                ଫେରନ୍ତୁ CLOCK_BOOTTIME;

        ସ୍ଥିତି CLOCK_REALTIME_ALARM:
                ଫେରନ୍ତୁ CLOCK_REALTIME;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ c;
        }
}

usec_t now(clockid_t clock_id) {
        ସଂରଚନା timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        ପ୍ରତିଜ୍ଞା(n > 0);
        ପ୍ରତିଜ୍ଞା(n < USEC_INFINITY);

        ଫେରନ୍ତୁ n;
}

nsec_t now_nsec(clockid_t clock_id) {
        ସଂରଚନା timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        ପ୍ରତିଜ୍ଞା(n > 0);
        ପ୍ରତିଜ୍ଞା(n < NSEC_INFINITY);

        ଫେରନ୍ତୁ n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        ପ୍ରତିଜ୍ଞା(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        ଫେରନ୍ତୁ ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        ପ୍ରତିଜ୍ଞା(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        ଫେରନ୍ତୁ ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        ଯଦି (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                ଯଦି (to_base >= USEC_INFINITY - delta) /* overflow? */
                        ଫେରନ୍ତୁ USEC_INFINITY;

                ଫେରନ୍ତୁ to_base + delta;

        } ଅନ୍ୟଥା { /* In the past */
                usec_t delta = from_base - from;

                ଯଦି (to_base <= delta) /* underflow? */
                        ଫେରନ୍ତୁ 0;

                ଫେରନ୍ତୁ to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        ଯଦି (map_clock_id(from_clock) == map_clock_id(to_clock))
                ଫେରନ୍ତୁ from;

        /* Keep infinity as is */
        ଯଦି (from == USEC_INFINITY)
                ଫେରନ୍ତୁ from;

        ଫେରନ୍ତୁ map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                ଫେରନ୍ତୁ ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        ଫେରନ୍ତୁ ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ଫେରନ୍ତୁ ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        ଫେରନ୍ତୁ ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ଫେରନ୍ତୁ ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        ଫେରନ୍ତୁ ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ଫେରନ୍ତୁ ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        ଫେରନ୍ତୁ ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ଫେରନ୍ତୁ ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        ଫେରନ୍ତୁ ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        ପ୍ରତିଜ୍ଞା(ts);

        ବାଛ (clock) {

        ସ୍ଥିତି CLOCK_REALTIME:
        ସ୍ଥିତି CLOCK_REALTIME_ALARM:
                ଫେରନ୍ତୁ ts->realtime;

        ସ୍ଥିତି CLOCK_MONOTONIC:
                ଫେରନ୍ତୁ ts->monotonic;

        ସ୍ଥିତି CLOCK_BOOTTIME:
        ସ୍ଥିତି CLOCK_BOOTTIME_ALARM:
                ଫେରନ୍ତୁ ts->boottime;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ USEC_INFINITY;
        }
}

usec_t timespec_load(ଅଚଳ ସଂରଚନା timespec *ts) {
        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ଫେରନ୍ତୁ USEC_INFINITY;

        ଯଦି ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                ଫେରନ୍ତୁ USEC_INFINITY;

        ଫେରନ୍ତୁ
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(ଅଚଳ ସଂରଚନା timespec *ts) {
        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ଫେରନ୍ତୁ NSEC_INFINITY;

        ଯଦି ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                ଫେରନ୍ତୁ NSEC_INFINITY;

        ଫେରନ୍ତୁ (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

ସଂରଚନା timespec *timespec_store(ସଂରଚନା timespec *ts, usec_t u) {
        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ଫେରନ୍ତୁ ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        ଫେରନ୍ତୁ ts;
}

ସଂରଚନା timespec *timespec_store_nsec(ସଂରଚନା timespec *ts, nsec_t n) {
        ପ୍ରତିଜ୍ଞା(ts);

        ଯଦି (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ଫେରନ୍ତୁ ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        ଫେରନ୍ତୁ ts;
}

ସଂରଚନା __kernel_timespec *kernel_timespec_store(ସଂରଚନା __kernel_timespec *ts, usec_t u) {
        ପ୍ରତିଜ୍ଞା(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        ପ୍ରତିଜ୍ଞା(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        ଫେରନ୍ତୁ ts;
}

usec_t timeval_load(ଅଚଳ ସଂରଚନା timeval *tv) {
        ପ୍ରତିଜ୍ଞା(tv);

        ଯଦି (tv->tv_sec < 0 || tv->tv_usec < 0)
                ଫେରନ୍ତୁ USEC_INFINITY;

        ଯଦି ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                ଫେରନ୍ତୁ USEC_INFINITY;

        ଫେରନ୍ତୁ
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

ସଂରଚନା timeval *timeval_store(ସଂରଚନା timeval *tv, usec_t u) {
        ପ୍ରତିଜ୍ଞା(tv);

        ଯଦି (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } ଅନ୍ୟଥା {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        ଫେରନ୍ତୁ tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ଅକ୍ଷର* format_timestamp_style(
                ଅକ୍ଷର *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        ସଂରଚନା tm tm;
        bool utc, us;
        size_t n;

        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(style >= 0);
        ପ୍ରତିଜ୍ଞା(style < _TIMESTAMP_STYLE_MAX);

        ଯଦି (!timestamp_is_set(t))
                ଫେରନ୍ତୁ ଖାଲି; /* Timestamp is unset */

        ଯଦି (style == TIMESTAMP_UNIX) {
                ଯଦି (l < (size_t) (1 + 1 + 1))
                        ଫେରନ୍ତୁ ଖାଲି; /* not enough space for even the shortest of forms */

                ଫେରନ୍ତୁ snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        ଯଦି (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                ଫେରନ୍ତୁ ଖାଲି; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        ଯଦି (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                ପ୍ରତିଜ୍ଞା(l >= strlen(xxx[style]) + 1);
                ଫେରନ୍ତୁ strcpy(buf, xxx[style]);
        }

        ଯଦି (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        /* Start with the week day */
        ଅଚଳ ଅକ୍ଷର *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        ପ୍ରତିଜ୍ଞା(weekday);
        memcpy(buf, weekday, 4);

        ଯଦି (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                ଯଦି (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        ଫେରନ୍ତୁ ଖାଲି; /* Doesn't fit */

                ଫେରନ୍ତୁ buf;
        }

        /* Add the main components */
        ଯଦି (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                ଫେରନ୍ତୁ ଖାଲି; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        ଯଦି (us) {
                n = strlen(buf);
                ଯଦି (n + 8 > l)
                        ଫେରନ୍ତୁ ଖାଲି; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        ଯଦି (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                ଯଦି (n + 5 > l)
                        ଫେରନ୍ତୁ ଖାଲି; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } ଅନ୍ୟଥା ଯଦି (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                ଯଦି (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        ଯଦି (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                ଫେରନ୍ତୁ ଖାଲି; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } ଅନ୍ୟଥା {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        ଫେରନ୍ତୁ buf;
}

ଅକ୍ଷର* format_timestamp_relative_full(ଅକ୍ଷର *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ବାମ) {
        ଅଚଳ ଅକ୍ଷର *s;
        usec_t n, d;

        ପ୍ରତିଜ୍ଞା(buf);

        ଯଦି (!timestamp_is_set(t))
                ଫେରନ୍ତୁ ଖାଲି;

        n = now(clock);
        ଯଦି (n > t) {
                d = n - t;
                s = " ago";
        } ଅନ୍ୟଥା {
                d = t - n;
                s = implicit_ବାମ ? "" : " left";
        }

        ଯଦି (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } ଅନ୍ୟଥା ଯଦି (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } ଅନ୍ୟଥା ଯଦି (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } ଅନ୍ୟଥା ଯଦି (d >= 2*USEC_PER_DAY)
                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        ଅନ୍ୟଥା ଯଦି (d >= 25*USEC_PER_HOUR)
                (ଶୂନ୍ୟ) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        ଅନ୍ୟଥା ଯଦି (d >= 6*USEC_PER_HOUR)
                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        ଅନ୍ୟଥା ଯଦି (d >= USEC_PER_HOUR)
                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        ଅନ୍ୟଥା ଯଦି (d >= 5*USEC_PER_MINUTE)
                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        ଅନ୍ୟଥା ଯଦି (d >= USEC_PER_MINUTE)
                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        ଅନ୍ୟଥା ଯଦି (d >= USEC_PER_SEC)
                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        ଅନ୍ୟଥା ଯଦି (d >= USEC_PER_MSEC)
                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        ଅନ୍ୟଥା ଯଦି (d > 0)
                (ଶୂନ୍ୟ) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        ଅନ୍ୟଥା
                (ଶୂନ୍ୟ) snprintf(buf, l, "now");

        buf[l-1] = 0;
        ଫେରନ୍ତୁ buf;
}

ଅକ୍ଷର* format_timespan(ଅକ୍ଷର *buf, size_t l, usec_t t, usec_t accuracy) {
        ସ୍ଥିର ଅଚଳ ସଂରଚନା {
                ଅଚଳ ଅକ୍ଷର *suffix;
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

        ଅକ୍ଷର *p = ASSERT_PTR(buf);
        bool something = false;

        ପ୍ରତିଜ୍ଞା(l > 0);

        ଯଦି (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                ଫେରନ୍ତୁ p;
        }

        ଯଦି (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                ଫେରନ୍ତୁ p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                ଯଦି (t <= 0)
                        ଭାଙ୍ଗ;

                ଯଦି (t < accuracy && something)
                        ଭାଙ୍ଗ;

                ଯଦି (t < i->usec)
                        ଜାରି;

                ଯଦି (l <= 1)
                        ଭାଙ୍ଗ;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                ଯଦି (t < USEC_PER_MINUTE && b > 0) {
                        signed ଅକ୍ଷର j = 0;

                        ପାଇଁ (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        ପାଇଁ (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        ଯଦି (j > 0) {
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
                ଯଦି (!done) {
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

        ଫେରନ୍ତୁ buf;
}

ଅଚଳ ଅକ୍ଷର* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        ଯଦି (dst && isempty(tzname[1]))
                dst = false;

        ଫେରନ୍ତୁ empty_to_null(tzname[dst]);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_gmtoff(ଅଚଳ ଅକ୍ଷର *t, long *ret) {
        ପ୍ରତିଜ୍ଞା(t);

        ସଂରଚନା tm tm;
        ଅଚଳ ଅକ୍ଷର *k = strptime(t, "%z", &tm);
        ଯଦି (k && *k == '\0') {
                /* Success! */
                ଯଦି (ret)
                        *ret = tm.tm_gmtoff;
                ଫେରନ୍ତୁ 0;
        }

#ifdef __GLIBC__
        ଫେରନ୍ତୁ -EINVAL;
#ଅନ୍ୟଥା
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        ବାଛ (*t) {
        ସ୍ଥିତି '+':
                positive = true;
                ଭାଙ୍ଗ;
        ସ୍ଥିତି '-':
                positive = false;
                ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EINVAL;
        }

        t++;
        r = undecchar(*t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        u += r * USEC_PER_HOUR;

        t++;
        ଯଦି (*t == '\0') /* 2 digits case */
                ଯାଅ finalize;

        ଯଦି (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r >= 6) /* refuse minutes equal to or larger than 60 */
                ଫେରନ୍ତୁ -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        u += r * USEC_PER_MINUTE;

        t++;
        ଯଦି (*t != '\0')
                ଫେରନ୍ତୁ -EINVAL;

finalize:
        ଯଦି (u > USEC_PER_DAY) /* refuse larger than one day */
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        ଫେରନ୍ତୁ 0;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_timestamp_impl(
                ଅଚଳ ଅକ୍ଷର *t,
                size_t max_len,
                bool utc,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା isdst,
                long gmtoff,
                usec_t *ret) {

        ସ୍ଥିର ଅଚଳ ସଂରଚନା {
                ଅଚଳ ଅକ୍ଷର *name;
                ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr;
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

        _cleanup_free_ ଅକ୍ଷର *t_alloc = ଖାଲି;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, weekday = -1;
        unsigned fractional = 0;
        ଅଚଳ ଅକ୍ଷର *k;
        ସଂରଚନା tm tm, copy;

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

        ପ୍ରତିଜ୍ଞା(t);

        ଯଦି (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                ଯଦି (max_len == 0) /* Can't be the only field */
                        ଫେରନ୍ତୁ -EINVAL;

                t_alloc = strndup(t, max_len);
                ଯଦି (!t_alloc)
                        ଫେରନ୍ତୁ -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        ଯଦି (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                ଯଦି ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        ଫେରନ୍ତୁ -EINVAL;
        } ଅନ୍ୟଥା {
                ଯଦି (gmtoff != 0)
                        ଫେରନ୍ତୁ -EINVAL;
        }

        ଯଦି (t[0] == '@' && !with_tz)
                ଫେରନ୍ତୁ parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        ଯଦି (!with_tz) {
                ଯଦି (streq(t, "now"))
                        ଯାଅ finish;

                ଯଦି (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        ଯାଅ finish;
                }

                ଯଦି (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        ଯାଅ finish;
                }

                ଯଦି ((k = endswith(t, " ago"))) {
                        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;

                        buf = strndup(t, k - t);
                        ଯଦି (!buf)
                                ଫେରନ୍ତୁ -ENOMEM;

                        r = parse_sec(buf, &minus);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        ଯାଅ finish;
                }

                ଯଦି ((k = endswith(t, " left"))) {
                        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;

                        buf = strndup(t, k - t);
                        ଯଦି (!buf)
                                ଫେରନ୍ତୁ -ENOMEM;

                        r = parse_sec(buf, &plus);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        ଯାଅ finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        tm.tm_isdst = isdst;

        ଯଦି (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ଯାଅ from_tm;

        } ଅନ୍ୟଥା ଯଦି (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ଯାଅ from_tm;

        } ଅନ୍ୟଥା ଯଦି (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ଯାଅ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                ଯଦି (!k || *k != ' ')
                        ଜାରି;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                ଭାଙ୍ଗ;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        ଯଦି (k) {
                ଯଦି (*k == '.')
                        ଯାଅ parse_usec;
                ଅନ୍ୟଥା ଯଦି (*k == 0)
                        ଯାଅ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        ଯଦି (k) {
                ଯଦି (*k == '.')
                        ଯାଅ parse_usec;
                ଅନ୍ୟଥା ଯଦି (*k == 0)
                        ଯାଅ from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        ଯଦି (k) {
                ଯଦି (*k == '.')
                        ଯାଅ parse_usec;
                ଅନ୍ୟଥା ଯଦି (*k == 0)
                        ଯାଅ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        ଯଦି (k) {
                ଯଦି (*k == '.')
                        ଯାଅ parse_usec;
                ଅନ୍ୟଥା ଯଦି (*k == 0)
                        ଯାଅ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        ଯଦି (k && *k == 0) {
                tm.tm_sec = 0;
                ଯାଅ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        ଯଦି (k && *k == 0) {
                tm.tm_sec = 0;
                ଯାଅ from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        ଯଦି (k && *k == 0) {
                tm.tm_sec = 0;
                ଯାଅ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        ଯଦି (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ଯାଅ from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        ଯଦି (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ଯାଅ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        ଯଦି (k) {
                ଯଦି (*k == '.')
                        ଯାଅ parse_usec;
                ଅନ୍ୟଥା ଯଦି (*k == 0)
                        ଯାଅ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        ଯଦି (k && *k == 0) {
                tm.tm_sec = 0;
                ଯାଅ from_tm;
        }

        ଫେରନ୍ତୁ -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (*k != '\0')
                ଫେରନ୍ତୁ -EINVAL;

from_tm:
        ପ୍ରତିଜ୍ଞା(plus == 0);
        ପ୍ରତିଜ୍ଞା(minus == 0);

        ଯଦି (weekday >= 0 && tm.tm_wday != weekday)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                ଯଦି (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } ଅନ୍ୟଥା
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        ଯଦି (usec < minus)
                ଫେରନ୍ତୁ -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        ଯଦି (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret)
                *ret = usec;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_timestamp(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret) {
        long gmtoff;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(t);

        size_t t_len = strlen(t);
        ଯଦି (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        ଯଦି (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                ଫେରନ୍ତୁ parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ଅଚଳ ଅକ୍ଷର *tz = strrchr(t, ' ');
        ଯଦି (!tz)
                ଫେରନ୍ତୁ parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        ଯଦି (streq(tz, "UTC"))
                ଫେରନ୍ତୁ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        ଯଦି (parse_gmtoff(tz, &gmtoff) >= 0)
                ଫେରନ୍ତୁ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j <= 1; j++) {
                ଯଦି (!streq_ptr(tz, get_tzname(j)))
                        ଜାରି;

                /* The specified timezone matches tzname[] of the local timezone. */
                ଫେରନ୍ତୁ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        ଯଦି (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                ଯଦି (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        ଫେରନ୍ତୁ negative_errno();

                ଫେରନ୍ତୁ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        ଫେରନ୍ତୁ parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* extract_multiplier(ଅଚଳ ଅକ୍ଷର *p, usec_t *ret) {
        ସ୍ଥିର ଅଚଳ ସଂରଚନା {
                ଅଚଳ ଅକ୍ଷର *suffix;
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

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        FOREACH_ELEMENT(i, table) {
                ଅଚଳ ଅକ୍ଷର *e = startswith(p, i->suffix);
                ଯଦି (e) {
                        *ret = i->usec;
                        ଫେରନ୍ତୁ e;
                }
        }

        ଫେରନ୍ତୁ p;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_time(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret, usec_t default_unit) {
        ଅଚଳ ଅକ୍ଷର *p, *s;

        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ ଖାଲି);
        s = startswith(p, "infinity");
        ଯଦି (s) {
                ଯଦି (!in_charset(s, WHITESPACE))
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (ret)
                        *ret = USEC_INFINITY;
                ଫେରନ୍ତୁ 0;
        }

        usec_t usec = 0;

        ପାଇଁ (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                ଅକ୍ଷର *e;

                p = skip_leading_chars(p, /* bad= */ ଖାଲି);
                ଯଦି (*p == 0) {
                        ଯଦି (!something)
                                ଫେରନ୍ତୁ -EINVAL;

                        ଭାଙ୍ଗ;
                }

                ଯଦି (*p == '-') /* Don't allow "-0" */
                        ଫେରନ୍ତୁ -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ଯଦି (errno > 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (l < 0)
                        ଫେରନ୍ତୁ -ERANGE;

                ଯଦି (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } ଅନ୍ୟଥା ଯଦି (e == p)
                        ଫେରନ୍ତୁ -EINVAL;
                ଅନ୍ୟଥା
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ଯଦି (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ଫେରନ୍ତୁ -EINVAL;

                p = s;

                ଯଦି ((usec_t) l >= USEC_INFINITY / multiplier)
                        ଫେରନ୍ତୁ -ERANGE;

                k = (usec_t) l * multiplier;
                ଯଦି (k >= USEC_INFINITY - usec)
                        ଫେରନ୍ତୁ -ERANGE;

                usec += k;

                something = true;

                ଯଦି (*e == '.') {
                        usec_t m = multiplier / 10;
                        ଅଚଳ ଅକ୍ଷର *b;

                        ପାଇଁ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                ଯଦି (k >= USEC_INFINITY - usec)
                                        ଫେରନ୍ତୁ -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ଯଦି (b == e + 1)
                                ଫେରନ୍ତୁ -EINVAL;
                }
        }

        ଯଦି (ret)
                *ret = usec;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sec(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret) {
        ଫେରନ୍ତୁ parse_time(t, ret, USEC_PER_SEC);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sec_fix_0(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret) {
        usec_t k;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(ret);

        r = parse_sec(t, &k);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = k == 0 ? USEC_INFINITY : k;
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sec_def_infinity(ଅଚଳ ଅକ୍ଷର *t, usec_t *ret) {
        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(ret);

        t += strspn(t, WHITESPACE);
        ଯଦି (isempty(t)) {
                *ret = USEC_INFINITY;
                ଫେରନ୍ତୁ 0;
        }
        ଫେରନ୍ତୁ parse_sec(t, ret);
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* extract_nsec_multiplier(ଅଚଳ ଅକ୍ଷର *p, nsec_t *ret) {
        ସ୍ଥିର ଅଚଳ ସଂରଚନା {
                ଅଚଳ ଅକ୍ଷର *suffix;
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

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        FOREACH_ELEMENT(i, table) {
                ଅଚଳ ଅକ୍ଷର *e = startswith(p, i->suffix);
                ଯଦି (e) {
                        *ret = i->nsec;
                        ଫେରନ୍ତୁ e;
                }
        }

        ଫେରନ୍ତୁ p;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_nsec(ଅଚଳ ଅକ୍ଷର *t, nsec_t *ret) {
        ଅଚଳ ଅକ୍ଷର *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        ଯଦି (s) {
                s += strspn(s, WHITESPACE);
                ଯଦି (*s != 0)
                        ଫେରନ୍ତୁ -EINVAL;

                *ret = NSEC_INFINITY;
                ଫେରନ୍ତୁ 0;
        }

        ପାଇଁ (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                ଅକ୍ଷର *e;

                p += strspn(p, WHITESPACE);

                ଯଦି (*p == 0) {
                        ଯଦି (!something)
                                ଫେରନ୍ତୁ -EINVAL;

                        ଭାଙ୍ଗ;
                }

                ଯଦି (*p == '-') /* Don't allow "-0" */
                        ଫେରନ୍ତୁ -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ଯଦି (errno > 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (l < 0)
                        ଫେରନ୍ତୁ -ERANGE;

                ଯଦି (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } ଅନ୍ୟଥା ଯଦି (e == p)
                        ଫେରନ୍ତୁ -EINVAL;
                ଅନ୍ୟଥା
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ଯଦି (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ଫେରନ୍ତୁ -EINVAL;

                p = s;

                ଯଦି ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        ଫେରନ୍ତୁ -ERANGE;

                k = (nsec_t) l * multiplier;
                ଯଦି (k >= NSEC_INFINITY - nsec)
                        ଫେରନ୍ତୁ -ERANGE;

                nsec += k;

                something = true;

                ଯଦି (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ଅଚଳ ଅକ୍ଷର *b;

                        ପାଇଁ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                ଯଦି (k >= NSEC_INFINITY - nsec)
                                        ଫେରନ୍ତୁ -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ଯଦି (b == e + 1)
                                ଫେରନ୍ତୁ -EINVAL;
                }
        }

        *ret = nsec;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_timezones_from_zone1970_tab(ଅକ୍ଷର ***ret) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_strv_free_ ଅକ୍ଷର **zones = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି, *cc = ଖାଲି, *co = ଖାଲି, *tz = ଖାଲି;

                r = read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଅଚଳ ଅକ୍ଷର *p = ଧାଡ଼ି;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_ଶବ୍ଦଗୁଡ଼ିକ(&p, ଖାଲି, 0, &cc, &co, &tz);
                ଯଦି (r < 0)
                        ଜାରି;

                /* Lines that start with # are comments. */
                ଯଦି (*cc == '#')
                        ଜାରି;

                ଯଦି (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ଜାରି;

                r = strv_extend(&zones, tz);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret = TAKE_PTR(zones);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_timezones_from_tzdata_zi(ଅକ୍ଷର ***ret) {
        _cleanup_fclose_ FILE *f = ଖାଲି;
        _cleanup_strv_free_ ଅକ୍ଷର **zones = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        ଯଦି (!f)
                ଫେରନ୍ତୁ -errno;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି, *type = ଖାଲି, *f1 = ଖାଲି, *f2 = ଖାଲି;

                r = read_ଧାଡ଼ି(f, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଅଚଳ ଅକ୍ଷର *p = ଧାଡ଼ି;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_ଶବ୍ଦଗୁଡ଼ିକ(&p, ଖାଲି, 0, &type, &f1, &f2);
                ଯଦି (r < 0)
                        ଜାରି;

                ଅକ୍ଷର *tz;
                ଯଦି (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                ଅନ୍ୟଥା ଯଦି (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                ଅନ୍ୟଥା
                        /* Not a line we care about. */
                        ଜାରି;

                ଯଦି (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ଜାରି;

                r = strv_extend(&zones, tz);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *ret = TAKE_PTR(zones);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_timezones(ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **zones = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        ଯଦି (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                ଯଦି (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        ଯଦି (r < 0 && r != -ENOENT)
                ଫେରନ୍ତୁ r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_timezone(ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
        bool slash = false;
        ଅଚଳ ଅକ୍ଷର *p, *t;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ଅକ୍ଷର buf[4];
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (isempty(name))
                ଫେରନ୍ତୁ -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        ଯଦି (streq(name, "UTC"))
                ଫେରନ୍ତୁ 0;

        ଯଦି (name[0] == '/')
                ଫେରନ୍ତୁ -EINVAL;

        ପାଇଁ (p = name; *p; p++) {
                ଯଦି (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (*p == '/') {

                        ଯଦି (slash)
                                ଫେରନ୍ତୁ -EINVAL;

                        slash = true;
                } ଅନ୍ୟଥା
                        slash = false;
        }

        ଯଦି (slash)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (p - name >= PATH_MAX)
                ଫେରନ୍ତୁ -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        ଯଦି (memcmp(buf, "TZif", 4) != 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ reset_timezonep(ଅକ୍ଷର **p) {
        ପ୍ରତିଜ୍ଞା(p);

        (ଶୂନ୍ୟ) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

ଅକ୍ଷର* save_timezone(ଶୂନ୍ୟ) {
        ଅଚଳ ଅକ୍ଷର *e = getenv("TZ");
        ଯଦି (!e)
                ଫେରନ୍ତୁ ଖାଲି;

        ଅକ୍ଷର *s = strdup(e);
        ଯଦି (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        ଫେରନ୍ତୁ s;
}

bool clock_supported(clockid_t clock) {
        ସଂରଚନା timespec ts;

        ବାଛ (clock) {

        ସ୍ଥିତି CLOCK_MONOTONIC:
        ସ୍ଥିତି CLOCK_REALTIME:
        ସ୍ଥିତି CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                ଫେରନ୍ତୁ true;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                /* For everything else, check properly */
                ଫେରନ୍ତୁ clock_gettime(clock, &ts) >= 0;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_timezone(ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = readlink_malloc(etc_localtime(), &t);
        ଯଦି (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                ଫେରନ୍ତୁ strdup_to(ret, "UTC");
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r; /* Return EINVAL if not a symlink */

        ଅଚଳ ଅକ୍ଷର *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        ଯଦି (!e)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (!timezone_is_valid(e, LOG_DEBUG))
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ strdup_to(ret, e);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_timezone_prefer_env(ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଅଚଳ ଅକ୍ଷର *e = getenv("TZ");
        ଯଦି (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                ଫେରନ୍ତୁ strdup_to(ret, e + 1);

        ଫେରନ୍ତୁ get_timezone(ret);
}

ଅଚଳ ଅକ୍ଷର* etc_localtime(ଶୂନ୍ୟ) {
        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *cached = ଖାଲି;

        ଯଦି (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        ଫେରନ୍ତୁ cached;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mktime_or_timegm_usec(
                ସଂରଚନା tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        ପ୍ରତିଜ୍ଞା(tm);

        ଯଦି (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                ଫେରନ୍ତୁ -ERANGE;
        ଯଦି ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                ଫେରନ୍ତୁ -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        ଯଦି (t < 0) /* Refuse negative times and errors */
                ଫେରନ୍ତୁ -ERANGE;
        ଯଦି ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                ସଂରଚନା tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        ଯଦି (t > (usec_t) TIME_T_MAX)
                ଫେରନ୍ତୁ -ERANGE;
        time_t sec = (time_t) t;

        ସଂରଚନା tm buf = {};
        ଯଦି (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret)
                *ret = buf;

        ଫେରନ୍ତୁ 0;
}

uint64_t sysconf_clock_ticks_cached(ଶୂନ୍ୟ) {
        ସ୍ଥିର thread_local uint64_t hz = 0;

        ଯଦି (hz != 0)
                ଫେରନ୍ତୁ hz;

        long t = sysconf(_SC_CLK_TCK);
        ପ୍ରତିଜ୍ଞା(t > 0);
        hz = (uint64_t) t;

        ଫେରନ୍ତୁ hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ଫେରନ୍ତୁ DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ଫେରନ୍ତୁ DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        ଯଦି (x == USEC_INFINITY)
                ଫେରନ୍ତୁ USEC_INFINITY;
        ଯଦି (map_clock_id(from) == map_clock_id(to))
                ଫେରନ୍ତୁ x;

        a = now(from);
        b = now(to);

        ଯଦି (x > a)
                /* x lies in the future */
                ଫେରନ୍ତୁ usec_add(b, usec_sub_unsigned(x, a));
        ଅନ୍ୟଥା
                /* x lies in the past */
                ଫେରନ୍ତୁ usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(ଶୂନ୍ୟ) {
        tzset();

        ଫେରନ୍ତୁ timezone == 0 && daylight == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା usleep_safe(usec_t usec) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        ଯଦି (usec == 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (usec == USEC_INFINITY)
                ଫେରନ୍ତୁ RET_NERRNO(pause());

        ସଂରଚନା timespec t;
        timespec_store(&t, usec);

        ପାଇଁ (;;) {
                ସଂରଚନା timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                ଯଦି (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        ଜାରି;
                }

                ଫେରନ୍ତୁ r;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା time_change_fd(ଶୂନ୍ୟ) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        ସ୍ଥିର ଅଚଳ ସଂରଚନା itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, ଖାଲି) >= 0)
                ଫେରନ୍ତୁ TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#ଯଦି SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        ଯଦି (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                ସ୍ଥିର ଅଚଳ ସଂରଚନା itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                ଯଦି (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, ଖାଲି) >= 0)
                        ଫେରନ୍ତୁ TAKE_FD(fd);
        }
#endif

        ଫେରନ୍ତୁ -errno;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ଅଚଳ ଅକ୍ଷର *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        ଯଦି (t >= 0)
                ଫେରନ୍ତୁ t;
        ଯଦି (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                ଫେରନ୍ତୁ TIMESTAMP_US;
        ଯଦି (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                ଫେରନ୍ତୁ TIMESTAMP_US_UTC;
        ଫେରନ୍ତୁ t;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_calendar_date_full(ଅଚଳ ଅକ୍ଷର *s, bool allow_pre_epoch, usec_t *ret_usec, ସଂରଚନା tm *ret_tm) {
        ସଂରଚନା tm parsed_tm = {}, copy_tm;
        ଅଚଳ ଅକ୍ଷର *k;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        ଯଦି (!k || *k)
                ଫେରନ୍ତୁ -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        ଯଦି (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                ଯଦି (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        ଫେରନ୍ତୁ -EINVAL;
        } ଅନ୍ୟଥା {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        ଯଦି (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret_usec)
                *ret_usec = usec;
        ଯଦି (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        ଫେରନ୍ତୁ 0;
}
