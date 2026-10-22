/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/time_types.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/timerfd.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "hexdecoct.h"          /* IWYU pragma: keep */
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "time-util.h"

స్థిర clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        ఎంపిక (c) {

        స్థితి CLOCK_BOOTTIME_ALARM:
                తిరిగి CLOCK_BOOTTIME;

        స్థితి CLOCK_REALTIME_ALARM:
                తిరిగి CLOCK_REALTIME;

        అప్రమేయ:
                తిరిగి c;
        }
}

usec_t now(clockid_t clock_id) {
        నిర్మాణం timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_ఎక్కించు(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        ప్రతిజ్ఞా(n > 0);
        ప్రతిజ్ఞా(n < USEC_INFINITY);

        తిరిగి n;
}

nsec_t now_nsec(clockid_t clock_id) {
        నిర్మాణం timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_ఎక్కించు_nsec(&ts);

        ప్రతిజ్ఞా(n > 0);
        ప్రతిజ్ఞా(n < NSEC_INFINITY);

        తిరిగి n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        ప్రతిజ్ఞా(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        తిరిగి ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        ప్రతిజ్ఞా(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        తిరిగి ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_పునాది, usec_t గమ్యం_పునాది) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        ఒకవేళ (from >= from_పునాది) { /* In the future */
                usec_t delta = from - from_పునాది;

                ఒకవేళ (గమ్యం_పునాది >= USEC_INFINITY - delta) /* overflow? */
                        తిరిగి USEC_INFINITY;

                తిరిగి గమ్యం_పునాది + delta;

        } లేకపోతే { /* In the past */
                usec_t delta = from_పునాది - from;

                ఒకవేళ (గమ్యం_పునాది <= delta) /* underflow? */
                        తిరిగి 0;

                తిరిగి గమ్యం_పునాది - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t గమ్యం_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        ఒకవేళ (map_clock_id(from_clock) == map_clock_id(గమ్యం_clock))
                తిరిగి from;

        /* Keep infinity as is */
        ఒకవేళ (from == USEC_INFINITY)
                తిరిగి from;

        తిరిగి map_clock_usec_raw(from, now(from_clock), now(గమ్యం_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        ప్రతిజ్ఞా(ts);

        ఒకవేళ (!timestamp_is_సమితి(u)) {
                ts->realtime = ts->monotonic = u;
                తిరిగి ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        తిరిగి ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        ప్రతిజ్ఞా(ts);

        ఒకవేళ (!timestamp_is_సమితి(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                తిరిగి ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        తిరిగి ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        ప్రతిజ్ఞా(ts);

        ఒకవేళ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                తిరిగి ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        తిరిగి ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        ప్రతిజ్ఞా(ts);

        ఒకవేళ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                తిరిగి ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        తిరిగి ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        ప్రతిజ్ఞా(ts);

        ఒకవేళ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                తిరిగి ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        తిరిగి ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        ప్రతిజ్ఞా(ts);

        ఎంపిక (clock) {

        స్థితి CLOCK_REALTIME:
        స్థితి CLOCK_REALTIME_ALARM:
                తిరిగి ts->realtime;

        స్థితి CLOCK_MONOTONIC:
                తిరిగి ts->monotonic;

        స్థితి CLOCK_BOOTTIME:
        స్థితి CLOCK_BOOTTIME_ALARM:
                తిరిగి ts->boottime;

        అప్రమేయ:
                తిరిగి USEC_INFINITY;
        }
}

usec_t timespec_ఎక్కించు(స్థిరరాశి నిర్మాణం timespec *ts) {
        ప్రతిజ్ఞా(ts);

        ఒకవేళ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                తిరిగి USEC_INFINITY;

        ఒకవేళ ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_చొప్పున_USEC)) / USEC_చొప్పున_SEC)
                తిరిగి USEC_INFINITY;

        తిరిగి
                (usec_t) ts->tv_sec * USEC_చొప్పున_SEC +
                (usec_t) ts->tv_nsec / NSEC_చొప్పున_USEC;
}

nsec_t timespec_ఎక్కించు_nsec(స్థిరరాశి నిర్మాణం timespec *ts) {
        ప్రతిజ్ఞా(ts);

        ఒకవేళ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                తిరిగి NSEC_INFINITY;

        ఒకవేళ ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_చొప్పున_SEC)
                తిరిగి NSEC_INFINITY;

        తిరిగి (nsec_t) ts->tv_sec * NSEC_చొప్పున_SEC + (nsec_t) ts->tv_nsec;
}

నిర్మాణం timespec *timespec_store(నిర్మాణం timespec *ts, usec_t u) {
        ప్రతిజ్ఞా(ts);

        ఒకవేళ (u == USEC_INFINITY ||
            u / USEC_చొప్పున_SEC >= సమయం_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                తిరిగి ts;
        }

        ts->tv_sec = (time_t) (u / USEC_చొప్పున_SEC);
        ts->tv_nsec = (దీర్ఘ) ((u % USEC_చొప్పున_SEC) * NSEC_చొప్పున_USEC);

        తిరిగి ts;
}

నిర్మాణం timespec *timespec_store_nsec(నిర్మాణం timespec *ts, nsec_t n) {
        ప్రతిజ్ఞా(ts);

        ఒకవేళ (n == NSEC_INFINITY ||
            n / NSEC_చొప్పున_SEC >= సమయం_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                తిరిగి ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_చొప్పున_SEC);
        ts->tv_nsec = (దీర్ఘ) (n % NSEC_చొప్పున_SEC);

        తిరిగి ts;
}

నిర్మాణం __kernel_timespec *kernel_timespec_store(నిర్మాణం __kernel_timespec *ts, usec_t u) {
        ప్రతిజ్ఞా(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        ప్రతిజ్ఞా(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_చొప్పున_SEC;
        ts->tv_nsec = (u % USEC_చొప్పున_SEC) * NSEC_చొప్పున_USEC;

        తిరిగి ts;
}

usec_t timeval_ఎక్కించు(స్థిరరాశి నిర్మాణం timeval *tv) {
        ప్రతిజ్ఞా(tv);

        ఒకవేళ (tv->tv_sec < 0 || tv->tv_usec < 0)
                తిరిగి USEC_INFINITY;

        ఒకవేళ ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_చొప్పున_SEC)
                తిరిగి USEC_INFINITY;

        తిరిగి
                (usec_t) tv->tv_sec * USEC_చొప్పున_SEC +
                (usec_t) tv->tv_usec;
}

నిర్మాణం timeval *timeval_store(నిర్మాణం timeval *tv, usec_t u) {
        ప్రతిజ్ఞా(tv);

        ఒకవేళ (u == USEC_INFINITY ||
            u / USEC_చొప్పున_SEC > సమయం_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } లేకపోతే {
                tv->tv_sec = (time_t) (u / USEC_చొప్పున_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_చొప్పున_SEC);
        }

        తిరిగి tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి weekday_పట్టిక[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(weekday, పూర్ణాంకం);

అక్షరం* format_timestamp_style(
                అక్షరం *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        నిర్మాణం tm tm;
        bool utc, us;
        size_t n;

        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(style >= 0);
        ప్రతిజ్ఞా(style < _TIMESTAMP_STYLE_MAX);

        ఒకవేళ (!timestamp_is_సమితి(t))
                తిరిగి ఖాళీ; /* Timestamp is unset */

        ఒకవేళ (style == TIMESTAMP_UNIX) {
                ఒకవేళ (l < (size_t) (1 + 1 + 1))
                        తిరిగి ఖాళీ; /* not enough space for even the shortest of forms */

                తిరిగి snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_చొప్పున_SEC);  /* round down μs → s */
        }

        utc = లోపల_సమితి(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = లోపల_సమితి(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        ఒకవేళ (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                తిరిగి ఖాళీ; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        ఒకవేళ (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                ప్రతిజ్ఞా(l >= అక్షరశ్రేణి_పొడవు(xxx[style]) + 1);
                తిరిగి strcpy(buf, xxx[style]);
        }

        ఒకవేళ (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                తిరిగి ఖాళీ;

        /* Start with the week day */
        స్థిరరాశి అక్షరం *weekday = weekday_గమ్యం_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        ప్రతిజ్ఞా(weekday);
        జ్ఞాపకం_నకలు(buf, weekday, 4);

        ఒకవేళ (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                ఒకవేళ (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        తిరిగి ఖాళీ; /* Doesn't fit */

                తిరిగి buf;
        }

        /* Add the main components */
        ఒకవేళ (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                తిరిగి ఖాళీ; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        ఒకవేళ (us) {
                n = అక్షరశ్రేణి_పొడవు(buf);
                ఒకవేళ (n + 8 > l)
                        తిరిగి ఖాళీ; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_చొప్పున_SEC);
        }

        /* Append the timezone */
        n = అక్షరశ్రేణి_పొడవు(buf);
        ఒకవేళ (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                ఒకవేళ (n + 5 > l)
                        తిరిగి ఖాళీ; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } లేకపోతే ఒకవేళ (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = అక్షరశ్రేణి_పొడవు(tm.tm_zone);
                ఒకవేళ (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        ఒకవేళ (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                తిరిగి ఖాళీ; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } లేకపోతే {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        తిరిగి buf;
}

అక్షరం* format_timestamp_relative_నిండిన(అక్షరం *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ఎడమ) {
        స్థిరరాశి అక్షరం *s;
        usec_t n, d;

        ప్రతిజ్ఞా(buf);

        ఒకవేళ (!timestamp_is_సమితి(t))
                తిరిగి ఖాళీ;

        n = now(clock);
        ఒకవేళ (n > t) {
                d = n - t;
                s = " ago";
        } లేకపోతే {
                d = t - n;
                s = implicit_ఎడమ ? "" : " left";
        }

        ఒకవేళ (d >= USEC_చొప్పున_YEAR) {
                usec_t years = d / USEC_చొప్పున_YEAR;
                usec_t months = (d % USEC_చొప్పున_YEAR) / USEC_చొప్పున_MONTH;

                (శూన్య) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } లేకపోతే ఒకవేళ (d >= USEC_చొప్పున_MONTH) {
                usec_t months = d / USEC_చొప్పున_MONTH;
                usec_t days = (d % USEC_చొప్పున_MONTH) / USEC_చొప్పున_DAY;

                (శూన్య) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } లేకపోతే ఒకవేళ (d >= USEC_చొప్పున_WEEK) {
                usec_t weeks = d / USEC_చొప్పున_WEEK;
                usec_t days = (d % USEC_చొప్పున_WEEK) / USEC_చొప్పున_DAY;

                (శూన్య) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } లేకపోతే ఒకవేళ (d >= 2*USEC_చొప్పున_DAY)
                (శూన్య) snprintf(buf, l, USEC_FMT " days%s", d / USEC_చొప్పున_DAY,s);
        లేకపోతే ఒకవేళ (d >= 25*USEC_చొప్పున_HOUR)
                (శూన్య) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_చొప్పున_DAY) / USEC_చొప్పున_HOUR, s);
        లేకపోతే ఒకవేళ (d >= 6*USEC_చొప్పున_HOUR)
                (శూన్య) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_చొప్పున_HOUR, s);
        లేకపోతే ఒకవేళ (d >= USEC_చొప్పున_HOUR)
                (శూన్య) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_చొప్పున_HOUR,
                                (d % USEC_చొప్పున_HOUR) / USEC_చొప్పున_MINUTE, s);
        లేకపోతే ఒకవేళ (d >= 5*USEC_చొప్పున_MINUTE)
                (శూన్య) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_చొప్పున_MINUTE, s);
        లేకపోతే ఒకవేళ (d >= USEC_చొప్పున_MINUTE)
                (శూన్య) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_చొప్పున_MINUTE,
                                (d % USEC_చొప్పున_MINUTE) / USEC_చొప్పున_SEC, s);
        లేకపోతే ఒకవేళ (d >= USEC_చొప్పున_SEC)
                (శూన్య) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_చొప్పున_SEC, s);
        లేకపోతే ఒకవేళ (d >= USEC_చొప్పున_MSEC)
                (శూన్య) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_చొప్పున_MSEC, s);
        లేకపోతే ఒకవేళ (d > 0)
                (శూన్య) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        లేకపోతే
                (శూన్య) snprintf(buf, l, "now");

        buf[l-1] = 0;
        తిరిగి buf;
}

అక్షరం* format_timespan(అక్షరం *buf, size_t l, usec_t t, usec_t accuracy) {
        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి అక్షరం *suffix;
                usec_t usec;
        } పట్టిక[] = {
                { "y",     USEC_చొప్పున_YEAR   },
                { "month", USEC_చొప్పున_MONTH  },
                { "w",     USEC_చొప్పున_WEEK   },
                { "d",     USEC_చొప్పున_DAY    },
                { "h",     USEC_చొప్పున_HOUR   },
                { "min",   USEC_చొప్పున_MINUTE },
                { "s",     USEC_చొప్పున_SEC    },
                { "ms",    USEC_చొప్పున_MSEC   },
                { "us",    1               },
        };

        అక్షరం *p = ASSERT_PTR(buf);
        bool something = false;

        ప్రతిజ్ఞా(l > 0);

        ఒకవేళ (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                తిరిగి p;
        }

        ఒకవేళ (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                తిరిగి p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, పట్టిక) {
                పూర్ణాంకం k = 0;
                size_t n;
                bool పూర్తి = false;
                usec_t a, b;

                ఒకవేళ (t <= 0)
                        ఆపు;

                ఒకవేళ (t < accuracy && something)
                        ఆపు;

                ఒకవేళ (t < i->usec)
                        కొనసాగు;

                ఒకవేళ (l <= 1)
                        ఆపు;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                ఒకవేళ (t < USEC_చొప్పున_MINUTE && b > 0) {
                        గుర్తుగల అక్షరం j = 0;

                        కొరకు (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        కొరకు (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        ఒకవేళ (j > 0) {
                                k = snprintf(p, l,
                                             "%s"USEC_FMT".%0*"PRI_USEC"%s",
                                             p > buf ? " " : "",
                                             a,
                                             j,
                                             b,
                                             i->suffix);

                                t = 0;
                                పూర్తి = true;
                        }
                }

                /* No? Then let's show it normally */
                ఒకవేళ (!పూర్తి) {
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

        తిరిగి buf;
}

స్థిరరాశి అక్షరం* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        ఒకవేళ (dst && isempty(tzname[1]))
                dst = false;

        తిరిగి ఖాళీలు_గమ్యం_null(tzname[dst]);
}

పూర్ణాంకం parse_gmtoff(స్థిరరాశి అక్షరం *t, దీర్ఘ *ret) {
        ప్రతిజ్ఞా(t);

        నిర్మాణం tm tm;
        స్థిరరాశి అక్షరం *k = strptime(t, "%z", &tm);
        ఒకవేళ (k && *k == '\0') {
                /* Success! */
                ఒకవేళ (ret)
                        *ret = tm.tm_gmtoff;
                తిరిగి 0;
        }

#నిర్వచితమైతే __GLIBC__
        తిరిగి -EINVAL;
#లేకపోతే
        పూర్ణాంకం r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool ధనాత్మక;
        ఎంపిక (*t) {
        స్థితి '+':
                ధనాత్మక = true;
                ఆపు;
        స్థితి '-':
                ధనాత్మక = false;
                ఆపు;
        అప్రమేయ:
                తిరిగి -EINVAL;
        }

        t++;
        r = undecchar(*t);
        ఒకవేళ (r < 0)
                తిరిగి r;

        usec_t u = r * 10 * USEC_చొప్పున_HOUR;

        t++;
        r = undecchar(*t);
        ఒకవేళ (r < 0)
                తిరిగి r;
        u += r * USEC_చొప్పున_HOUR;

        t++;
        ఒకవేళ (*t == '\0') /* 2 digits case */
                వెళ్ళు finalize;

        ఒకవేళ (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r >= 6) /* refuse minutes equal to or larger than 60 */
                తిరిగి -EINVAL;

        u += r * 10 * USEC_చొప్పున_MINUTE;

        t++;
        r = undecchar(*t);
        ఒకవేళ (r < 0)
                తిరిగి r;

        u += r * USEC_చొప్పున_MINUTE;

        t++;
        ఒకవేళ (*t != '\0')
                తిరిగి -EINVAL;

finalize:
        ఒకవేళ (u > USEC_చొప్పున_DAY) /* refuse larger than one day */
                తిరిగి -EINVAL;

        ఒకవేళ (ret) {
                దీర్ఘ gmtoff = u / USEC_చొప్పున_SEC;
                *ret = ధనాత్మక ? gmtoff : -gmtoff;
        }

        తిరిగి 0;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం parse_timestamp_impl(
                స్థిరరాశి అక్షరం *t,
                size_t max_len,
                bool utc,
                పూర్ణాంకం isdst,
                దీర్ఘ gmtoff,
                usec_t *ret) {

        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి అక్షరం *name;
                స్థిరరాశి పూర్ణాంకం nr;
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

        _cleanup_free_ అక్షరం *t_alloc = ఖాళీ;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        పూర్ణాంకం r, weekday = -1;
        గుర్తులేని fractional = 0;
        స్థిరరాశి అక్షరం *k;
        నిర్మాణం tm tm, copy;

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

        ప్రతిజ్ఞా(t);

        ఒకవేళ (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                ఒకవేళ (max_len == 0) /* Can't be the only field */
                        తిరిగి -EINVAL;

                t_alloc = strndup(t, max_len);
                ఒకవేళ (!t_alloc)
                        తిరిగి -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        ఒకవేళ (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                ఒకవేళ ((usec_t) labs(gmtoff) * USEC_చొప్పున_SEC > USEC_చొప్పున_DAY)
                        తిరిగి -EINVAL;
        } లేకపోతే {
                ఒకవేళ (gmtoff != 0)
                        తిరిగి -EINVAL;
        }

        ఒకవేళ (t[0] == '@' && !with_tz)
                తిరిగి parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        ఒకవేళ (!with_tz) {
                ఒకవేళ (streq(t, "now"))
                        వెళ్ళు ముగింపు;

                ఒకవేళ (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        వెళ్ళు ముగింపు;
                }

                ఒకవేళ (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        వెళ్ళు ముగింపు;
                }

                ఒకవేళ ((k = endswith(t, " ago"))) {
                        _cleanup_free_ అక్షరం *buf = ఖాళీ;

                        buf = strndup(t, k - t);
                        ఒకవేళ (!buf)
                                తిరిగి -ENOMEM;

                        r = parse_sec(buf, &minus);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        వెళ్ళు ముగింపు;
                }

                ఒకవేళ ((k = endswith(t, " left"))) {
                        _cleanup_free_ అక్షరం *buf = ఖాళీ;

                        buf = strndup(t, k - t);
                        ఒకవేళ (!buf)
                                తిరిగి -ENOMEM;

                        r = parse_sec(buf, &plus);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        వెళ్ళు ముగింపు;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        ఒకవేళ (r < 0)
                తిరిగి r;

        tm.tm_isdst = isdst;

        ఒకవేళ (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                వెళ్ళు from_tm;

        } లేకపోతే ఒకవేళ (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                వెళ్ళు from_tm;

        } లేకపోతే ఒకవేళ (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                వెళ్ళు from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                ఒకవేళ (!k || *k != ' ')
                        కొనసాగు;

#నిర్వచితమైతే __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#అయితే_ముగింపు
                t = k + 1;
                ఆపు;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        ఒకవేళ (k) {
                ఒకవేళ (*k == '.')
                        వెళ్ళు parse_usec;
                లేకపోతే ఒకవేళ (*k == 0)
                        వెళ్ళు from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        ఒకవేళ (k) {
                ఒకవేళ (*k == '.')
                        వెళ్ళు parse_usec;
                లేకపోతే ఒకవేళ (*k == 0)
                        వెళ్ళు from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        ఒకవేళ (k) {
                ఒకవేళ (*k == '.')
                        వెళ్ళు parse_usec;
                లేకపోతే ఒకవేళ (*k == 0)
                        వెళ్ళు from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        ఒకవేళ (k) {
                ఒకవేళ (*k == '.')
                        వెళ్ళు parse_usec;
                లేకపోతే ఒకవేళ (*k == 0)
                        వెళ్ళు from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        ఒకవేళ (k && *k == 0) {
                tm.tm_sec = 0;
                వెళ్ళు from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        ఒకవేళ (k && *k == 0) {
                tm.tm_sec = 0;
                వెళ్ళు from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        ఒకవేళ (k && *k == 0) {
                tm.tm_sec = 0;
                వెళ్ళు from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        ఒకవేళ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                వెళ్ళు from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        ఒకవేళ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                వెళ్ళు from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        ఒకవేళ (k) {
                ఒకవేళ (*k == '.')
                        వెళ్ళు parse_usec;
                లేకపోతే ఒకవేళ (*k == 0)
                        వెళ్ళు from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        ఒకవేళ (k && *k == 0) {
                tm.tm_sec = 0;
                వెళ్ళు from_tm;
        }

        తిరిగి -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        ఒకవేళ (r < 0)
                తిరిగి -EINVAL;
        ఒకవేళ (*k != '\0')
                తిరిగి -EINVAL;

from_tm:
        ప్రతిజ్ఞా(plus == 0);
        ప్రతిజ్ఞా(minus == 0);

        ఒకవేళ (weekday >= 0 && tm.tm_wday != weekday)
                తిరిగి -EINVAL;

        ఒకవేళ (gmtoff < 0) {
                plus = -gmtoff * USEC_చొప్పున_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                ఒకవేళ (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_చొప్పున_DAY;
                }
        } లేకపోతే
                minus = gmtoff * USEC_చొప్పున_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        ఒకవేళ (r < 0)
                తిరిగి r;

        usec = usec_కూడు(usec, fractional);

ముగింపు:
        usec = usec_కూడు(usec, plus);

        ఒకవేళ (usec < minus)
                తిరిగి -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        ఒకవేళ (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                తిరిగి -EINVAL;

        ఒకవేళ (ret)
                *ret = usec;
        తిరిగి 0;
}

పూర్ణాంకం parse_timestamp(స్థిరరాశి అక్షరం *t, usec_t *ret) {
        దీర్ఘ gmtoff;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(t);

        size_t t_len = అక్షరశ్రేణి_పొడవు(t);
        ఒకవేళ (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                ఒకవేళ (r >= 0)
                        తిరిగి r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        ఒకవేళ (t_len > 7 && లోపల_సమితి(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                తిరిగి parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        స్థిరరాశి అక్షరం *tz = strrchr(t, ' ');
        ఒకవేళ (!tz)
                తిరిగి parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        ఒకవేళ (streq(tz, "UTC"))
                తిరిగి parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        ఒకవేళ (parse_gmtoff(tz, &gmtoff) >= 0)
                తిరిగి parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        కొరకు (పూర్ణాంకం j = 0; j <= 1; j++) {
                ఒకవేళ (!streq_ptr(tz, get_tzname(j)))
                        కొనసాగు;

                /* The specified timezone matches tzname[] of the local timezone. */
                తిరిగి parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        ఒకవేళ (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                ఒకవేళ (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        తిరిగి negative_errno();

                తిరిగి parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        తిరిగి parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

స్థిర స్థిరరాశి అక్షరం* వెలికితీయి_multiplier(స్థిరరాశి అక్షరం *p, usec_t *ret) {
        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి అక్షరం *suffix;
                usec_t usec;
        } పట్టిక[] = {
                { "seconds", USEC_చొప్పున_SEC    },
                { "second",  USEC_చొప్పున_SEC    },
                { "sec",     USEC_చొప్పున_SEC    },
                { "s",       USEC_చొప్పున_SEC    },
                { "minutes", USEC_చొప్పున_MINUTE },
                { "minute",  USEC_చొప్పున_MINUTE },
                { "min",     USEC_చొప్పున_MINUTE },
                { "months",  USEC_చొప్పున_MONTH  },
                { "month",   USEC_చొప్పున_MONTH  },
                { "M",       USEC_చొప్పున_MONTH  },
                { "msec",    USEC_చొప్పున_MSEC   },
                { "ms",      USEC_చొప్పున_MSEC   },
                { "m",       USEC_చొప్పున_MINUTE },
                { "hours",   USEC_చొప్పున_HOUR   },
                { "hour",    USEC_చొప్పున_HOUR   },
                { "hr",      USEC_చొప్పున_HOUR   },
                { "h",       USEC_చొప్పున_HOUR   },
                { "days",    USEC_చొప్పున_DAY    },
                { "day",     USEC_చొప్పున_DAY    },
                { "d",       USEC_చొప్పున_DAY    },
                { "weeks",   USEC_చొప్పున_WEEK   },
                { "week",    USEC_చొప్పున_WEEK   },
                { "w",       USEC_చొప్పున_WEEK   },
                { "years",   USEC_చొప్పున_YEAR   },
                { "year",    USEC_చొప్పున_YEAR   },
                { "y",       USEC_చొప్పున_YEAR   },
                { "usec",    1ULL            },
                { "us",      1ULL            },
                { "μs",      1ULL            }, /* U+03bc (aka GREEK SMALL LETTER MU) */
                { "µs",      1ULL            }, /* U+b5 (aka MICRO SIGN) */
        };

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        FOREACH_ELEMENT(i, పట్టిక) {
                స్థిరరాశి అక్షరం *e = startswith(p, i->suffix);
                ఒకవేళ (e) {
                        *ret = i->usec;
                        తిరిగి e;
                }
        }

        తిరిగి p;
}

పూర్ణాంకం parse_సమయం(స్థిరరాశి అక్షరం *t, usec_t *ret, usec_t default_unit) {
        స్థిరరాశి అక్షరం *p, *s;

        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ ఖాళీ);
        s = startswith(p, "infinity");
        ఒకవేళ (s) {
                ఒకవేళ (!లోపల_charset(s, WHITESPACE))
                        తిరిగి -EINVAL;

                ఒకవేళ (ret)
                        *ret = USEC_INFINITY;
                తిరిగి 0;
        }

        usec_t usec = 0;

        కొరకు (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                దీర్ఘ దీర్ఘ l;
                అక్షరం *e;

                p = skip_leading_chars(p, /* bad= */ ఖాళీ);
                ఒకవేళ (*p == 0) {
                        ఒకవేళ (!something)
                                తిరిగి -EINVAL;

                        ఆపు;
                }

                ఒకవేళ (*p == '-') /* Don't allow "-0" */
                        తిరిగి -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ఒకవేళ (errno > 0)
                        తిరిగి -errno;
                ఒకవేళ (l < 0)
                        తిరిగి -ERANGE;

                ఒకవేళ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } లేకపోతే ఒకవేళ (e == p)
                        తిరిగి -EINVAL;
                లేకపోతే
                        p = e;

                s = వెలికితీయి_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ఒకవేళ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        తిరిగి -EINVAL;

                p = s;

                ఒకవేళ ((usec_t) l >= USEC_INFINITY / multiplier)
                        తిరిగి -ERANGE;

                k = (usec_t) l * multiplier;
                ఒకవేళ (k >= USEC_INFINITY - usec)
                        తిరిగి -ERANGE;

                usec += k;

                something = true;

                ఒకవేళ (*e == '.') {
                        usec_t m = multiplier / 10;
                        స్థిరరాశి అక్షరం *b;

                        కొరకు (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                ఒకవేళ (k >= USEC_INFINITY - usec)
                                        తిరిగి -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ఒకవేళ (b == e + 1)
                                తిరిగి -EINVAL;
                }
        }

        ఒకవేళ (ret)
                *ret = usec;
        తిరిగి 0;
}

పూర్ణాంకం parse_sec(స్థిరరాశి అక్షరం *t, usec_t *ret) {
        తిరిగి parse_సమయం(t, ret, USEC_చొప్పున_SEC);
}

పూర్ణాంకం parse_sec_fix_0(స్థిరరాశి అక్షరం *t, usec_t *ret) {
        usec_t k;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(ret);

        r = parse_sec(t, &k);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = k == 0 ? USEC_INFINITY : k;
        తిరిగి r;
}

పూర్ణాంకం parse_sec_def_infinity(స్థిరరాశి అక్షరం *t, usec_t *ret) {
        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(ret);

        t += strspn(t, WHITESPACE);
        ఒకవేళ (isempty(t)) {
                *ret = USEC_INFINITY;
                తిరిగి 0;
        }
        తిరిగి parse_sec(t, ret);
}

స్థిర స్థిరరాశి అక్షరం* వెలికితీయి_nsec_multiplier(స్థిరరాశి అక్షరం *p, nsec_t *ret) {
        స్థిర స్థిరరాశి నిర్మాణం {
                స్థిరరాశి అక్షరం *suffix;
                nsec_t nsec;
        } పట్టిక[] = {
                { "seconds", NSEC_చొప్పున_SEC    },
                { "second",  NSEC_చొప్పున_SEC    },
                { "sec",     NSEC_చొప్పున_SEC    },
                { "s",       NSEC_చొప్పున_SEC    },
                { "minutes", NSEC_చొప్పున_MINUTE },
                { "minute",  NSEC_చొప్పున_MINUTE },
                { "min",     NSEC_చొప్పున_MINUTE },
                { "months",  NSEC_చొప్పున_MONTH  },
                { "month",   NSEC_చొప్పున_MONTH  },
                { "M",       NSEC_చొప్పున_MONTH  },
                { "msec",    NSEC_చొప్పున_MSEC   },
                { "ms",      NSEC_చొప్పున_MSEC   },
                { "m",       NSEC_చొప్పున_MINUTE },
                { "hours",   NSEC_చొప్పున_HOUR   },
                { "hour",    NSEC_చొప్పున_HOUR   },
                { "hr",      NSEC_చొప్పున_HOUR   },
                { "h",       NSEC_చొప్పున_HOUR   },
                { "days",    NSEC_చొప్పున_DAY    },
                { "day",     NSEC_చొప్పున_DAY    },
                { "d",       NSEC_చొప్పున_DAY    },
                { "weeks",   NSEC_చొప్పున_WEEK   },
                { "week",    NSEC_చొప్పున_WEEK   },
                { "w",       NSEC_చొప్పున_WEEK   },
                { "years",   NSEC_చొప్పున_YEAR   },
                { "year",    NSEC_చొప్పున_YEAR   },
                { "y",       NSEC_చొప్పున_YEAR   },
                { "usec",    NSEC_చొప్పున_USEC   },
                { "us",      NSEC_చొప్పున_USEC   },
                { "μs",      NSEC_చొప్పున_USEC   }, /* U+03bc (aka GREEK LETTER MU) */
                { "µs",      NSEC_చొప్పున_USEC   }, /* U+b5 (aka MICRO SIGN) */
                { "nsec",    1ULL            },
                { "ns",      1ULL            },
                { "",        1ULL            }, /* default is nsec */
        };

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        FOREACH_ELEMENT(i, పట్టిక) {
                స్థిరరాశి అక్షరం *e = startswith(p, i->suffix);
                ఒకవేళ (e) {
                        *ret = i->nsec;
                        తిరిగి e;
                }
        }

        తిరిగి p;
}

పూర్ణాంకం parse_nsec(స్థిరరాశి అక్షరం *t, nsec_t *ret) {
        స్థిరరాశి అక్షరం *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        ఒకవేళ (s) {
                s += strspn(s, WHITESPACE);
                ఒకవేళ (*s != 0)
                        తిరిగి -EINVAL;

                *ret = NSEC_INFINITY;
                తిరిగి 0;
        }

        కొరకు (;;) {
                nsec_t multiplier = 1, k;
                దీర్ఘ దీర్ఘ l;
                అక్షరం *e;

                p += strspn(p, WHITESPACE);

                ఒకవేళ (*p == 0) {
                        ఒకవేళ (!something)
                                తిరిగి -EINVAL;

                        ఆపు;
                }

                ఒకవేళ (*p == '-') /* Don't allow "-0" */
                        తిరిగి -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ఒకవేళ (errno > 0)
                        తిరిగి -errno;
                ఒకవేళ (l < 0)
                        తిరిగి -ERANGE;

                ఒకవేళ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } లేకపోతే ఒకవేళ (e == p)
                        తిరిగి -EINVAL;
                లేకపోతే
                        p = e;

                s = వెలికితీయి_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ఒకవేళ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        తిరిగి -EINVAL;

                p = s;

                ఒకవేళ ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        తిరిగి -ERANGE;

                k = (nsec_t) l * multiplier;
                ఒకవేళ (k >= NSEC_INFINITY - nsec)
                        తిరిగి -ERANGE;

                nsec += k;

                something = true;

                ఒకవేళ (*e == '.') {
                        nsec_t m = multiplier / 10;
                        స్థిరరాశి అక్షరం *b;

                        కొరకు (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                ఒకవేళ (k >= NSEC_INFINITY - nsec)
                                        తిరిగి -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ఒకవేళ (b == e + 1)
                                తిరిగి -EINVAL;
                }
        }

        *ret = nsec;

        తిరిగి 0;
}

స్థిర పూర్ణాంకం get_timezones_from_zone1970_tab(అక్షరం ***ret) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_strv_free_ అక్షరం **zones = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        ఒకవేళ (!f)
                తిరిగి -errno;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ, *cc = ఖాళీ, *co = ఖాళీ, *tz = ఖాళీ;

                r = read_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                స్థిరరాశి అక్షరం *p = పంక్తి;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = వెలికితీయి_many_పదాలు(&p, ఖాళీ, 0, &cc, &co, &tz);
                ఒకవేళ (r < 0)
                        కొనసాగు;

                /* Lines that start with # are comments. */
                ఒకవేళ (*cc == '#')
                        కొనసాగు;

                ఒకవేళ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        కొనసాగు;

                r = strv_extend(&zones, tz);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret = TAKE_PTR(zones);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం get_timezones_from_tzdata_zi(అక్షరం ***ret) {
        _cleanup_fclose_ FILE *f = ఖాళీ;
        _cleanup_strv_free_ అక్షరం **zones = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        ఒకవేళ (!f)
                తిరిగి -errno;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ, *type = ఖాళీ, *f1 = ఖాళీ, *f2 = ఖాళీ;

                r = read_పంక్తి(f, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                స్థిరరాశి అక్షరం *p = పంక్తి;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = వెలికితీయి_many_పదాలు(&p, ఖాళీ, 0, &type, &f1, &f2);
                ఒకవేళ (r < 0)
                        కొనసాగు;

                అక్షరం *tz;
                ఒకవేళ (లోపల_సమితి(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                లేకపోతే ఒకవేళ (లోపల_సమితి(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                లేకపోతే
                        /* Not a line we care about. */
                        కొనసాగు;

                ఒకవేళ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        కొనసాగు;

                r = strv_extend(&zones, tz);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *ret = TAKE_PTR(zones);
        తిరిగి 0;
}

పూర్ణాంకం get_timezones(అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **zones = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        ఒకవేళ (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                ఒకవేళ (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        ఒకవేళ (r < 0 && r != -ENOENT)
                తిరిగి r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        ఒకవేళ (r < 0)
                తిరిగి r;

        strv_క్రమీకరించు_uniq(zones);

        *ret = TAKE_PTR(zones);
        తిరిగి 0;
}

పూర్ణాంకం verify_timezone(స్థిరరాశి అక్షరం *name, పూర్ణాంకం log_level) {
        bool slash = false;
        స్థిరరాశి అక్షరం *p, *t;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        అక్షరం buf[4];
        పూర్ణాంకం r;

        ఒకవేళ (isempty(name))
                తిరిగి -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        ఒకవేళ (streq(name, "UTC"))
                తిరిగి 0;

        ఒకవేళ (name[0] == '/')
                తిరిగి -EINVAL;

        కొరకు (p = name; *p; p++) {
                ఒకవేళ (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !లోపల_సమితి(*p, '-', '_', '+', '/'))
                        తిరిగి -EINVAL;

                ఒకవేళ (*p == '/') {

                        ఒకవేళ (slash)
                                తిరిగి -EINVAL;

                        slash = true;
                } లేకపోతే
                        slash = false;
        }

        ఒకవేళ (slash)
                తిరిగి -EINVAL;

        ఒకవేళ (p - name >= PATH_MAX)
                తిరిగి -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి log_నిండిన_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        ఒకవేళ (memcmp(buf, "TZif", 4) != 0)
                తిరిగి log_నిండిన_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        తిరిగి 0;
}

శూన్య reset_timezonep(అక్షరం **p) {
        ప్రతిజ్ఞా(p);

        (శూన్య) సమితి_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

అక్షరం* save_timezone(శూన్య) {
        స్థిరరాశి అక్షరం *e = getenv("TZ");
        ఒకవేళ (!e)
                తిరిగి ఖాళీ;

        అక్షరం *s = strdup(e);
        ఒకవేళ (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        తిరిగి s;
}

bool clock_supported(clockid_t clock) {
        నిర్మాణం timespec ts;

        ఎంపిక (clock) {

        స్థితి CLOCK_MONOTONIC:
        స్థితి CLOCK_REALTIME:
        స్థితి CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                తిరిగి true;

        అప్రమేయ:
                /* For everything else, check properly */
                తిరిగి clock_gettime(clock, &ts) >= 0;
        }
}

పూర్ణాంకం get_timezone(అక్షరం **ret) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = readlink_malloc(etc_localtime(), &t);
        ఒకవేళ (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                తిరిగి strdup_గమ్యం(ret, "UTC");
        ఒకవేళ (r < 0)
                తిరిగి r; /* Return EINVAL if not a symlink */

        స్థిరరాశి అక్షరం *e = మార్గం_STARTSWITH_సమితి(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        ఒకవేళ (!e)
                తిరిగి -EINVAL;
        ఒకవేళ (!timezone_is_valid(e, LOG_DEBUG))
                తిరిగి -EINVAL;

        తిరిగి strdup_గమ్యం(ret, e);
}

పూర్ణాంకం get_timezone_prefer_env(అక్షరం **ret) {
        ప్రతిజ్ఞా(ret);

        స్థిరరాశి అక్షరం *e = getenv("TZ");
        ఒకవేళ (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                తిరిగి strdup_గమ్యం(ret, e + 1);

        తిరిగి get_timezone(ret);
}

స్థిరరాశి అక్షరం* etc_localtime(శూన్య) {
        స్థిర స్థిరరాశి అక్షరం *cached = ఖాళీ;

        ఒకవేళ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        తిరిగి cached;
}

పూర్ణాంకం mktime_or_timegm_usec(
                నిర్మాణం tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        ప్రతిజ్ఞా(tm);

        ఒకవేళ (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                తిరిగి -ERANGE;
        ఒకవేళ ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_చొప్పున_YEAR, (usec_t) సమయం_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                తిరిగి -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        ఒకవేళ (t < 0) /* Refuse negative times and errors */
                తిరిగి -ERANGE;
        ఒకవేళ ((usec_t) t >= USEC_INFINITY / USEC_చొప్పున_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                తిరిగి -ERANGE;

        ఒకవేళ (ret)
                *ret = (usec_t) t * USEC_చొప్పున_SEC;
        తిరిగి 0;
}

పూర్ణాంకం localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                నిర్మాణం tm *ret) {

        t /= USEC_చొప్పున_SEC; /* Round down */
        ఒకవేళ (t > (usec_t) సమయం_T_MAX)
                తిరిగి -ERANGE;
        time_t sec = (time_t) t;

        నిర్మాణం tm buf = {};
        ఒకవేళ (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                తిరిగి -EINVAL;

        ఒకవేళ (ret)
                *ret = buf;

        తిరిగి 0;
}

uint64_t sysconf_clock_ticks_cached(శూన్య) {
        స్థిర thread_local uint64_t hz = 0;

        ఒకవేళ (hz != 0)
                తిరిగి hz;

        దీర్ఘ t = sysconf(_SC_CLK_TCK);
        ప్రతిజ్ఞా(t > 0);
        hz = (uint64_t) t;

        తిరిగి hz;
}

uint32_t usec_గమ్యం_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        తిరిగి DIV_విడత_పైకి(u, USEC_చొప్పున_SEC / hz);
}

usec_t jiffies_గమ్యం_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        తిరిగి DIV_విడత_పైకి(j * USEC_చొప్పున_SEC, hz);
}

usec_t usec_జరుపు_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        ఒకవేళ (x == USEC_INFINITY)
                తిరిగి USEC_INFINITY;
        ఒకవేళ (map_clock_id(from) == map_clock_id(to))
                తిరిగి x;

        a = now(from);
        b = now(to);

        ఒకవేళ (x > a)
                /* x lies in the future */
                తిరిగి usec_కూడు(b, usec_sub_unsigned(x, a));
        లేకపోతే
                /* x lies in the past */
                తిరిగి usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool లోపల_utc_timezone(శూన్య) {
        tzset();

        తిరిగి timezone == 0 && daylight == 0;
}

పూర్ణాంకం usleep_సురక్షిత(usec_t usec) {
        పూర్ణాంకం r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        ఒకవేళ (usec == 0)
                తిరిగి 0;

        ఒకవేళ (usec == USEC_INFINITY)
                తిరిగి RET_NERRNO(pause());

        నిర్మాణం timespec t;
        timespec_store(&t, usec);

        కొరకు (;;) {
                నిర్మాణం timespec మిగిలిన;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &మిగిలిన);
                ఒకవేళ (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = మిగిలిన;
                        కొనసాగు;
                }

                తిరిగి r;
        }
}

పూర్ణాంకం సమయం_change_fd(శూన్య) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        స్థిర స్థిరరాశి నిర్మాణం itimerspec its = {
                .it_value.tv_sec = సమయం_T_MAX,
        };

        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        ఒకవేళ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, ఖాళీ) >= 0)
                తిరిగి TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#ఒకవేళ SIZEOF_సమయం_T == 8 && ULONG_MAX < UINT64_MAX
        ఒకవేళ (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                స్థిర స్థిరరాశి నిర్మాణం itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                ఒకవేళ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, ఖాళీ) >= 0)
                        తిరిగి TAKE_FD(fd);
        }
#అయితే_ముగింపు

        తిరిగి -errno;
}

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి timestamp_style_పట్టిక[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(స్థిరరాశి అక్షరం *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_పట్టిక_lookup_from_string(timestamp_style_పట్టిక, ELEMENTSOF(timestamp_style_పట్టిక), s);
        ఒకవేళ (t >= 0)
                తిరిగి t;
        ఒకవేళ (STRPTR_లోపల_సమితి(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                తిరిగి TIMESTAMP_US;
        ఒకవేళ (STRPTR_లోపల_సమితి(s, "µs+utc", "μs+utc"))
                తిరిగి TIMESTAMP_US_UTC;
        తిరిగి t;
}

పూర్ణాంకం parse_calendar_date_నిండిన(స్థిరరాశి అక్షరం *s, bool allow_pre_epoch, usec_t *ret_usec, నిర్మాణం tm *ret_tm) {
        నిర్మాణం tm parsed_tm = {}, copy_tm;
        స్థిరరాశి అక్షరం *k;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        ఒకవేళ (!k || *k)
                తిరిగి -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        ఒకవేళ (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                ఒకవేళ (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        తిరిగి -EINVAL;
        } లేకపోతే {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        ఒకవేళ (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                తిరిగి -EINVAL;

        ఒకవేళ (ret_usec)
                *ret_usec = usec;
        ఒకవేళ (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        తిరిగి 0;
}
