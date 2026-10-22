/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/time_types.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/timerfd.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "hexdecoct.h"          /* IWYU pragma: keep */
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "time-util.h"

स्थिर clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        विकल्प (c) {

        पक्ष CLOCK_BOOTTIME_ALARM:
                प्रत्यागम CLOCK_BOOTTIME;

        पक्ष CLOCK_REALTIME_ALARM:
                प्रत्यागम CLOCK_REALTIME;

        स्वभाव:
                प्रत्यागम c;
        }
}

usec_t now(clockid_t clock_id) {
        संरचना timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_आरोपय(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        प्रतिज्ञा(n > 0);
        प्रतिज्ञा(n < USEC_INFINITY);

        प्रत्यागम n;
}

nsec_t now_nsec(clockid_t clock_id) {
        संरचना timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_आरोपय_nsec(&ts);

        प्रतिज्ञा(n > 0);
        प्रतिज्ञा(n < NSEC_INFINITY);

        प्रत्यागम n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        प्रतिज्ञा(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        प्रत्यागम ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        प्रतिज्ञा(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        प्रत्यागम ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_आधारः, usec_t गन्तव्यम्_आधारः) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        यदि (from >= from_आधारः) { /* In the future */
                usec_t delta = from - from_आधारः;

                यदि (गन्तव्यम्_आधारः >= USEC_INFINITY - delta) /* overflow? */
                        प्रत्यागम USEC_INFINITY;

                प्रत्यागम गन्तव्यम्_आधारः + delta;

        } अन्यथा { /* In the past */
                usec_t delta = from_आधारः - from;

                यदि (गन्तव्यम्_आधारः <= delta) /* underflow? */
                        प्रत्यागम 0;

                प्रत्यागम गन्तव्यम्_आधारः - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t गन्तव्यम्_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        यदि (map_clock_id(from_clock) == map_clock_id(गन्तव्यम्_clock))
                प्रत्यागम from;

        /* Keep infinity as is */
        यदि (from == USEC_INFINITY)
                प्रत्यागम from;

        प्रत्यागम map_clock_usec_raw(from, now(from_clock), now(गन्तव्यम्_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        प्रतिज्ञा(ts);

        यदि (!timestamp_is_समुच्चयः(u)) {
                ts->realtime = ts->monotonic = u;
                प्रत्यागम ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        प्रत्यागम ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        प्रतिज्ञा(ts);

        यदि (!timestamp_is_समुच्चयः(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                प्रत्यागम ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        प्रत्यागम ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        प्रतिज्ञा(ts);

        यदि (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                प्रत्यागम ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        प्रत्यागम ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        प्रतिज्ञा(ts);

        यदि (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                प्रत्यागम ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        प्रत्यागम ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        प्रतिज्ञा(ts);

        यदि (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                प्रत्यागम ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        प्रत्यागम ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        प्रतिज्ञा(ts);

        विकल्प (clock) {

        पक्ष CLOCK_REALTIME:
        पक्ष CLOCK_REALTIME_ALARM:
                प्रत्यागम ts->realtime;

        पक्ष CLOCK_MONOTONIC:
                प्रत्यागम ts->monotonic;

        पक्ष CLOCK_BOOTTIME:
        पक्ष CLOCK_BOOTTIME_ALARM:
                प्रत्यागम ts->boottime;

        स्वभाव:
                प्रत्यागम USEC_INFINITY;
        }
}

usec_t timespec_आरोपय(ध्रुव संरचना timespec *ts) {
        प्रतिज्ञा(ts);

        यदि (ts->tv_sec < 0 || ts->tv_nsec < 0)
                प्रत्यागम USEC_INFINITY;

        यदि ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_प्रत्येकम्_USEC)) / USEC_प्रत्येकम्_SEC)
                प्रत्यागम USEC_INFINITY;

        प्रत्यागम
                (usec_t) ts->tv_sec * USEC_प्रत्येकम्_SEC +
                (usec_t) ts->tv_nsec / NSEC_प्रत्येकम्_USEC;
}

nsec_t timespec_आरोपय_nsec(ध्रुव संरचना timespec *ts) {
        प्रतिज्ञा(ts);

        यदि (ts->tv_sec < 0 || ts->tv_nsec < 0)
                प्रत्यागम NSEC_INFINITY;

        यदि ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_प्रत्येकम्_SEC)
                प्रत्यागम NSEC_INFINITY;

        प्रत्यागम (nsec_t) ts->tv_sec * NSEC_प्रत्येकम्_SEC + (nsec_t) ts->tv_nsec;
}

संरचना timespec *timespec_store(संरचना timespec *ts, usec_t u) {
        प्रतिज्ञा(ts);

        यदि (u == USEC_INFINITY ||
            u / USEC_प्रत्येकम्_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                प्रत्यागम ts;
        }

        ts->tv_sec = (time_t) (u / USEC_प्रत्येकम्_SEC);
        ts->tv_nsec = (दीर्घ) ((u % USEC_प्रत्येकम्_SEC) * NSEC_प्रत्येकम्_USEC);

        प्रत्यागम ts;
}

संरचना timespec *timespec_store_nsec(संरचना timespec *ts, nsec_t n) {
        प्रतिज्ञा(ts);

        यदि (n == NSEC_INFINITY ||
            n / NSEC_प्रत्येकम्_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                प्रत्यागम ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_प्रत्येकम्_SEC);
        ts->tv_nsec = (दीर्घ) (n % NSEC_प्रत्येकम्_SEC);

        प्रत्यागम ts;
}

संरचना __kernel_timespec *kernel_timespec_store(संरचना __kernel_timespec *ts, usec_t u) {
        प्रतिज्ञा(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        प्रतिज्ञा(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_प्रत्येकम्_SEC;
        ts->tv_nsec = (u % USEC_प्रत्येकम्_SEC) * NSEC_प्रत्येकम्_USEC;

        प्रत्यागम ts;
}

usec_t timeval_आरोपय(ध्रुव संरचना timeval *tv) {
        प्रतिज्ञा(tv);

        यदि (tv->tv_sec < 0 || tv->tv_usec < 0)
                प्रत्यागम USEC_INFINITY;

        यदि ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_प्रत्येकम्_SEC)
                प्रत्यागम USEC_INFINITY;

        प्रत्यागम
                (usec_t) tv->tv_sec * USEC_प्रत्येकम्_SEC +
                (usec_t) tv->tv_usec;
}

संरचना timeval *timeval_store(संरचना timeval *tv, usec_t u) {
        प्रतिज्ञा(tv);

        यदि (u == USEC_INFINITY ||
            u / USEC_प्रत्येकम्_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } अन्यथा {
                tv->tv_sec = (time_t) (u / USEC_प्रत्येकम्_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_प्रत्येकम्_SEC);
        }

        प्रत्यागम tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
स्थिर ध्रुव वर्ण *ध्रुव weekday_सारणी[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING(weekday, पूर्णाङ्क);

वर्ण* format_timestamp_style(
                वर्ण *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        संरचना tm tm;
        bool utc, us;
        size_t n;

        प्रतिज्ञा(buf);
        प्रतिज्ञा(style >= 0);
        प्रतिज्ञा(style < _TIMESTAMP_STYLE_MAX);

        यदि (!timestamp_is_समुच्चयः(t))
                प्रत्यागम रिक्त; /* Timestamp is unset */

        यदि (style == TIMESTAMP_UNIX) {
                यदि (l < (size_t) (1 + 1 + 1))
                        प्रत्यागम रिक्त; /* not enough space for even the shortest of forms */

                प्रत्यागम snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_प्रत्येकम्_SEC);  /* round down μs → s */
        }

        utc = अन्तर्_समुच्चयः(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = अन्तर्_समुच्चयः(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        यदि (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                प्रत्यागम रिक्त; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        यदि (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                स्थिर ध्रुव वर्ण* ध्रुव xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                प्रतिज्ञा(l >= वाक्यदैर्घ्यम्(xxx[style]) + 1);
                प्रत्यागम वाक्यं_प्रतिलिख(buf, xxx[style]);
        }

        यदि (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                प्रत्यागम रिक्त;

        /* Start with the week day */
        ध्रुव वर्ण *weekday = weekday_गन्तव्यम्_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        प्रतिज्ञा(weekday);
        स्मृतिं_प्रतिलिख(buf, weekday, 4);

        यदि (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                यदि (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        प्रत्यागम रिक्त; /* Doesn't fit */

                प्रत्यागम buf;
        }

        /* Add the main components */
        यदि (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                प्रत्यागम रिक्त; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        यदि (us) {
                n = वाक्यदैर्घ्यम्(buf);
                यदि (n + 8 > l)
                        प्रत्यागम रिक्त; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_प्रत्येकम्_SEC);
        }

        /* Append the timezone */
        n = वाक्यदैर्घ्यम्(buf);
        यदि (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                यदि (n + 5 > l)
                        प्रत्यागम रिक्त; /* "UTC" doesn't fit. */

                वाक्यं_प्रतिलिख(buf + n, " UTC");

        } अन्यथा यदि (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = वाक्यदैर्घ्यम्(tm.tm_zone);
                यदि (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        यदि (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                प्रत्यागम रिक्त; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } अन्यथा {
                        buf[n++] = ' ';
                        वाक्यं_प्रतिलिख(buf + n, tm.tm_zone);
                }
        }

        प्रत्यागम buf;
}

वर्ण* format_timestamp_relative_पूर्णम्(वर्ण *buf, size_t l, usec_t t, clockid_t clock, bool implicit_वामः) {
        ध्रुव वर्ण *s;
        usec_t n, d;

        प्रतिज्ञा(buf);

        यदि (!timestamp_is_समुच्चयः(t))
                प्रत्यागम रिक्त;

        n = now(clock);
        यदि (n > t) {
                d = n - t;
                s = " ago";
        } अन्यथा {
                d = t - n;
                s = implicit_वामः ? "" : " left";
        }

        यदि (d >= USEC_प्रत्येकम्_YEAR) {
                usec_t years = d / USEC_प्रत्येकम्_YEAR;
                usec_t months = (d % USEC_प्रत्येकम्_YEAR) / USEC_प्रत्येकम्_MONTH;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } अन्यथा यदि (d >= USEC_प्रत्येकम्_MONTH) {
                usec_t months = d / USEC_प्रत्येकम्_MONTH;
                usec_t days = (d % USEC_प्रत्येकम्_MONTH) / USEC_प्रत्येकम्_DAY;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } अन्यथा यदि (d >= USEC_प्रत्येकम्_WEEK) {
                usec_t weeks = d / USEC_प्रत्येकम्_WEEK;
                usec_t days = (d % USEC_प्रत्येकम्_WEEK) / USEC_प्रत्येकम्_DAY;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } अन्यथा यदि (d >= 2*USEC_प्रत्येकम्_DAY)
                (शून्य) snprintf(buf, l, USEC_FMT " days%s", d / USEC_प्रत्येकम्_DAY,s);
        अन्यथा यदि (d >= 25*USEC_प्रत्येकम्_HOUR)
                (शून्य) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_प्रत्येकम्_DAY) / USEC_प्रत्येकम्_HOUR, s);
        अन्यथा यदि (d >= 6*USEC_प्रत्येकम्_HOUR)
                (शून्य) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_प्रत्येकम्_HOUR, s);
        अन्यथा यदि (d >= USEC_प्रत्येकम्_HOUR)
                (शून्य) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_प्रत्येकम्_HOUR,
                                (d % USEC_प्रत्येकम्_HOUR) / USEC_प्रत्येकम्_MINUTE, s);
        अन्यथा यदि (d >= 5*USEC_प्रत्येकम्_MINUTE)
                (शून्य) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_प्रत्येकम्_MINUTE, s);
        अन्यथा यदि (d >= USEC_प्रत्येकम्_MINUTE)
                (शून्य) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_प्रत्येकम्_MINUTE,
                                (d % USEC_प्रत्येकम्_MINUTE) / USEC_प्रत्येकम्_SEC, s);
        अन्यथा यदि (d >= USEC_प्रत्येकम्_SEC)
                (शून्य) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_प्रत्येकम्_SEC, s);
        अन्यथा यदि (d >= USEC_प्रत्येकम्_MSEC)
                (शून्य) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_प्रत्येकम्_MSEC, s);
        अन्यथा यदि (d > 0)
                (शून्य) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        अन्यथा
                (शून्य) snprintf(buf, l, "now");

        buf[l-1] = 0;
        प्रत्यागम buf;
}

वर्ण* format_timespan(वर्ण *buf, size_t l, usec_t t, usec_t accuracy) {
        स्थिर ध्रुव संरचना {
                ध्रुव वर्ण *suffix;
                usec_t usec;
        } सारणी[] = {
                { "y",     USEC_प्रत्येकम्_YEAR   },
                { "month", USEC_प्रत्येकम्_MONTH  },
                { "w",     USEC_प्रत्येकम्_WEEK   },
                { "d",     USEC_प्रत्येकम्_DAY    },
                { "h",     USEC_प्रत्येकम्_HOUR   },
                { "min",   USEC_प्रत्येकम्_MINUTE },
                { "s",     USEC_प्रत्येकम्_SEC    },
                { "ms",    USEC_प्रत्येकम्_MSEC   },
                { "us",    1               },
        };

        वर्ण *p = ASSERT_PTR(buf);
        bool something = false;

        प्रतिज्ञा(l > 0);

        यदि (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                प्रत्यागम p;
        }

        यदि (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                प्रत्यागम p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, सारणी) {
                पूर्णाङ्क k = 0;
                size_t n;
                bool समाप्तम् = false;
                usec_t a, b;

                यदि (t <= 0)
                        भङ्ग;

                यदि (t < accuracy && something)
                        भङ्ग;

                यदि (t < i->usec)
                        अनुवर्त;

                यदि (l <= 1)
                        भङ्ग;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                यदि (t < USEC_प्रत्येकम्_MINUTE && b > 0) {
                        चिह्नित वर्ण j = 0;

                        प्रति (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        प्रति (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        यदि (j > 0) {
                                k = snprintf(p, l,
                                             "%s"USEC_FMT".%0*"PRI_USEC"%s",
                                             p > buf ? " " : "",
                                             a,
                                             j,
                                             b,
                                             i->suffix);

                                t = 0;
                                समाप्तम् = true;
                        }
                }

                /* No? Then let's show it normally */
                यदि (!समाप्तम्) {
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

        प्रत्यागम buf;
}

ध्रुव वर्ण* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        यदि (dst && isempty(tzname[1]))
                dst = false;

        प्रत्यागम रिक्तानि_गन्तव्यम्_null(tzname[dst]);
}

पूर्णाङ्क parse_gmtoff(ध्रुव वर्ण *t, दीर्घ *ret) {
        प्रतिज्ञा(t);

        संरचना tm tm;
        ध्रुव वर्ण *k = strptime(t, "%z", &tm);
        यदि (k && *k == '\0') {
                /* Success! */
                यदि (ret)
                        *ret = tm.tm_gmtoff;
                प्रत्यागम 0;
        }

#यदि_परिभाषितम् __GLIBC__
        प्रत्यागम -EINVAL;
#अन्यथा
        पूर्णाङ्क r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool धनात्मकम्;
        विकल्प (*t) {
        पक्ष '+':
                धनात्मकम् = true;
                भङ्ग;
        पक्ष '-':
                धनात्मकम् = false;
                भङ्ग;
        स्वभाव:
                प्रत्यागम -EINVAL;
        }

        t++;
        r = undecchar(*t);
        यदि (r < 0)
                प्रत्यागम r;

        usec_t u = r * 10 * USEC_प्रत्येकम्_HOUR;

        t++;
        r = undecchar(*t);
        यदि (r < 0)
                प्रत्यागम r;
        u += r * USEC_प्रत्येकम्_HOUR;

        t++;
        यदि (*t == '\0') /* 2 digits case */
                गच्छ finalize;

        यदि (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r >= 6) /* refuse minutes equal to or larger than 60 */
                प्रत्यागम -EINVAL;

        u += r * 10 * USEC_प्रत्येकम्_MINUTE;

        t++;
        r = undecchar(*t);
        यदि (r < 0)
                प्रत्यागम r;

        u += r * USEC_प्रत्येकम्_MINUTE;

        t++;
        यदि (*t != '\0')
                प्रत्यागम -EINVAL;

finalize:
        यदि (u > USEC_प्रत्येकम्_DAY) /* refuse larger than one day */
                प्रत्यागम -EINVAL;

        यदि (ret) {
                दीर्घ gmtoff = u / USEC_प्रत्येकम्_SEC;
                *ret = धनात्मकम् ? gmtoff : -gmtoff;
        }

        प्रत्यागम 0;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क parse_timestamp_impl(
                ध्रुव वर्ण *t,
                size_t max_len,
                bool utc,
                पूर्णाङ्क isdst,
                दीर्घ gmtoff,
                usec_t *ret) {

        स्थिर ध्रुव संरचना {
                ध्रुव वर्ण *name;
                ध्रुव पूर्णाङ्क nr;
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

        _cleanup_free_ वर्ण *t_alloc = रिक्त;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        पूर्णाङ्क r, weekday = -1;
        अचिह्नित fractional = 0;
        ध्रुव वर्ण *k;
        संरचना tm tm, copy;

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

        प्रतिज्ञा(t);

        यदि (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                यदि (max_len == 0) /* Can't be the only field */
                        प्रत्यागम -EINVAL;

                t_alloc = strndup(t, max_len);
                यदि (!t_alloc)
                        प्रत्यागम -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        यदि (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                यदि ((usec_t) labs(gmtoff) * USEC_प्रत्येकम्_SEC > USEC_प्रत्येकम्_DAY)
                        प्रत्यागम -EINVAL;
        } अन्यथा {
                यदि (gmtoff != 0)
                        प्रत्यागम -EINVAL;
        }

        यदि (t[0] == '@' && !with_tz)
                प्रत्यागम parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        यदि (!with_tz) {
                यदि (streq(t, "now"))
                        गच्छ finish;

                यदि (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        यदि (r < 0)
                                प्रत्यागम r;

                        गच्छ finish;
                }

                यदि (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        यदि (r < 0)
                                प्रत्यागम r;

                        गच्छ finish;
                }

                यदि ((k = endswith(t, " ago"))) {
                        _cleanup_free_ वर्ण *buf = रिक्त;

                        buf = strndup(t, k - t);
                        यदि (!buf)
                                प्रत्यागम -ENOMEM;

                        r = parse_sec(buf, &minus);
                        यदि (r < 0)
                                प्रत्यागम r;

                        गच्छ finish;
                }

                यदि ((k = endswith(t, " left"))) {
                        _cleanup_free_ वर्ण *buf = रिक्त;

                        buf = strndup(t, k - t);
                        यदि (!buf)
                                प्रत्यागम -ENOMEM;

                        r = parse_sec(buf, &plus);
                        यदि (r < 0)
                                प्रत्यागम r;

                        गच्छ finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        यदि (r < 0)
                प्रत्यागम r;

        tm.tm_isdst = isdst;

        यदि (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;

        } अन्यथा यदि (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;

        } अन्यथा यदि (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                यदि (!k || *k != ' ')
                        अनुवर्त;

#यदि_परिभाषितम् __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#यदि_अन्तः
                t = k + 1;
                भङ्ग;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        यदि (k && *k == 0) {
                tm.tm_sec = 0;
                गच्छ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        यदि (k && *k == 0) {
                tm.tm_sec = 0;
                गच्छ from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        यदि (k && *k == 0) {
                tm.tm_sec = 0;
                गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        यदि (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        यदि (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        यदि (k && *k == 0) {
                tm.tm_sec = 0;
                गच्छ from_tm;
        }

        प्रत्यागम -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        यदि (r < 0)
                प्रत्यागम -EINVAL;
        यदि (*k != '\0')
                प्रत्यागम -EINVAL;

from_tm:
        प्रतिज्ञा(plus == 0);
        प्रतिज्ञा(minus == 0);

        यदि (weekday >= 0 && tm.tm_wday != weekday)
                प्रत्यागम -EINVAL;

        यदि (gmtoff < 0) {
                plus = -gmtoff * USEC_प्रत्येकम्_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                यदि (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_प्रत्येकम्_DAY;
                }
        } अन्यथा
                minus = gmtoff * USEC_प्रत्येकम्_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        यदि (r < 0)
                प्रत्यागम r;

        usec = usec_सङ्कलय(usec, fractional);

finish:
        usec = usec_सङ्कलय(usec, plus);

        यदि (usec < minus)
                प्रत्यागम -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        यदि (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                प्रत्यागम -EINVAL;

        यदि (ret)
                *ret = usec;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_timestamp(ध्रुव वर्ण *t, usec_t *ret) {
        दीर्घ gmtoff;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);

        size_t t_len = वाक्यदैर्घ्यम्(t);
        यदि (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                यदि (r >= 0)
                        प्रत्यागम r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        यदि (t_len > 7 && अन्तर्_समुच्चयः(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                प्रत्यागम parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ध्रुव वर्ण *tz = strrchr(t, ' ');
        यदि (!tz)
                प्रत्यागम parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        यदि (streq(tz, "UTC"))
                प्रत्यागम parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        यदि (parse_gmtoff(tz, &gmtoff) >= 0)
                प्रत्यागम parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        प्रति (पूर्णाङ्क j = 0; j <= 1; j++) {
                यदि (!streq_ptr(tz, get_tzname(j)))
                        अनुवर्त;

                /* The specified timezone matches tzname[] of the local timezone. */
                प्रत्यागम parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        यदि (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                यदि (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        प्रत्यागम negative_errno();

                प्रत्यागम parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        प्रत्यागम parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

स्थिर ध्रुव वर्ण* extract_multiplier(ध्रुव वर्ण *p, usec_t *ret) {
        स्थिर ध्रुव संरचना {
                ध्रुव वर्ण *suffix;
                usec_t usec;
        } सारणी[] = {
                { "seconds", USEC_प्रत्येकम्_SEC    },
                { "second",  USEC_प्रत्येकम्_SEC    },
                { "sec",     USEC_प्रत्येकम्_SEC    },
                { "s",       USEC_प्रत्येकम्_SEC    },
                { "minutes", USEC_प्रत्येकम्_MINUTE },
                { "minute",  USEC_प्रत्येकम्_MINUTE },
                { "min",     USEC_प्रत्येकम्_MINUTE },
                { "months",  USEC_प्रत्येकम्_MONTH  },
                { "month",   USEC_प्रत्येकम्_MONTH  },
                { "M",       USEC_प्रत्येकम्_MONTH  },
                { "msec",    USEC_प्रत्येकम्_MSEC   },
                { "ms",      USEC_प्रत्येकम्_MSEC   },
                { "m",       USEC_प्रत्येकम्_MINUTE },
                { "hours",   USEC_प्रत्येकम्_HOUR   },
                { "hour",    USEC_प्रत्येकम्_HOUR   },
                { "hr",      USEC_प्रत्येकम्_HOUR   },
                { "h",       USEC_प्रत्येकम्_HOUR   },
                { "days",    USEC_प्रत्येकम्_DAY    },
                { "day",     USEC_प्रत्येकम्_DAY    },
                { "d",       USEC_प्रत्येकम्_DAY    },
                { "weeks",   USEC_प्रत्येकम्_WEEK   },
                { "week",    USEC_प्रत्येकम्_WEEK   },
                { "w",       USEC_प्रत्येकम्_WEEK   },
                { "years",   USEC_प्रत्येकम्_YEAR   },
                { "year",    USEC_प्रत्येकम्_YEAR   },
                { "y",       USEC_प्रत्येकम्_YEAR   },
                { "usec",    1ULL            },
                { "us",      1ULL            },
                { "μs",      1ULL            }, /* U+03bc (aka GREEK SMALL LETTER MU) */
                { "µs",      1ULL            }, /* U+b5 (aka MICRO SIGN) */
        };

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        FOREACH_ELEMENT(i, सारणी) {
                ध्रुव वर्ण *e = startswith(p, i->suffix);
                यदि (e) {
                        *ret = i->usec;
                        प्रत्यागम e;
                }
        }

        प्रत्यागम p;
}

पूर्णाङ्क parse_time(ध्रुव वर्ण *t, usec_t *ret, usec_t default_unit) {
        ध्रुव वर्ण *p, *s;

        प्रतिज्ञा(t);
        प्रतिज्ञा(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ रिक्त);
        s = startswith(p, "infinity");
        यदि (s) {
                यदि (!अन्तर्_charset(s, WHITESPACE))
                        प्रत्यागम -EINVAL;

                यदि (ret)
                        *ret = USEC_INFINITY;
                प्रत्यागम 0;
        }

        usec_t usec = 0;

        प्रति (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                दीर्घ दीर्घ l;
                वर्ण *e;

                p = skip_leading_chars(p, /* bad= */ रिक्त);
                यदि (*p == 0) {
                        यदि (!something)
                                प्रत्यागम -EINVAL;

                        भङ्ग;
                }

                यदि (*p == '-') /* Don't allow "-0" */
                        प्रत्यागम -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                यदि (errno > 0)
                        प्रत्यागम -errno;
                यदि (l < 0)
                        प्रत्यागम -ERANGE;

                यदि (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } अन्यथा यदि (e == p)
                        प्रत्यागम -EINVAL;
                अन्यथा
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                यदि (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        प्रत्यागम -EINVAL;

                p = s;

                यदि ((usec_t) l >= USEC_INFINITY / multiplier)
                        प्रत्यागम -ERANGE;

                k = (usec_t) l * multiplier;
                यदि (k >= USEC_INFINITY - usec)
                        प्रत्यागम -ERANGE;

                usec += k;

                something = true;

                यदि (*e == '.') {
                        usec_t m = multiplier / 10;
                        ध्रुव वर्ण *b;

                        प्रति (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                यदि (k >= USEC_INFINITY - usec)
                                        प्रत्यागम -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        यदि (b == e + 1)
                                प्रत्यागम -EINVAL;
                }
        }

        यदि (ret)
                *ret = usec;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_sec(ध्रुव वर्ण *t, usec_t *ret) {
        प्रत्यागम parse_time(t, ret, USEC_प्रत्येकम्_SEC);
}

पूर्णाङ्क parse_sec_fix_0(ध्रुव वर्ण *t, usec_t *ret) {
        usec_t k;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        r = parse_sec(t, &k);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = k == 0 ? USEC_INFINITY : k;
        प्रत्यागम r;
}

पूर्णाङ्क parse_sec_def_infinity(ध्रुव वर्ण *t, usec_t *ret) {
        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        t += strspn(t, WHITESPACE);
        यदि (isempty(t)) {
                *ret = USEC_INFINITY;
                प्रत्यागम 0;
        }
        प्रत्यागम parse_sec(t, ret);
}

स्थिर ध्रुव वर्ण* extract_nsec_multiplier(ध्रुव वर्ण *p, nsec_t *ret) {
        स्थिर ध्रुव संरचना {
                ध्रुव वर्ण *suffix;
                nsec_t nsec;
        } सारणी[] = {
                { "seconds", NSEC_प्रत्येकम्_SEC    },
                { "second",  NSEC_प्रत्येकम्_SEC    },
                { "sec",     NSEC_प्रत्येकम्_SEC    },
                { "s",       NSEC_प्रत्येकम्_SEC    },
                { "minutes", NSEC_प्रत्येकम्_MINUTE },
                { "minute",  NSEC_प्रत्येकम्_MINUTE },
                { "min",     NSEC_प्रत्येकम्_MINUTE },
                { "months",  NSEC_प्रत्येकम्_MONTH  },
                { "month",   NSEC_प्रत्येकम्_MONTH  },
                { "M",       NSEC_प्रत्येकम्_MONTH  },
                { "msec",    NSEC_प्रत्येकम्_MSEC   },
                { "ms",      NSEC_प्रत्येकम्_MSEC   },
                { "m",       NSEC_प्रत्येकम्_MINUTE },
                { "hours",   NSEC_प्रत्येकम्_HOUR   },
                { "hour",    NSEC_प्रत्येकम्_HOUR   },
                { "hr",      NSEC_प्रत्येकम्_HOUR   },
                { "h",       NSEC_प्रत्येकम्_HOUR   },
                { "days",    NSEC_प्रत्येकम्_DAY    },
                { "day",     NSEC_प्रत्येकम्_DAY    },
                { "d",       NSEC_प्रत्येकम्_DAY    },
                { "weeks",   NSEC_प्रत्येकम्_WEEK   },
                { "week",    NSEC_प्रत्येकम्_WEEK   },
                { "w",       NSEC_प्रत्येकम्_WEEK   },
                { "years",   NSEC_प्रत्येकम्_YEAR   },
                { "year",    NSEC_प्रत्येकम्_YEAR   },
                { "y",       NSEC_प्रत्येकम्_YEAR   },
                { "usec",    NSEC_प्रत्येकम्_USEC   },
                { "us",      NSEC_प्रत्येकम्_USEC   },
                { "μs",      NSEC_प्रत्येकम्_USEC   }, /* U+03bc (aka GREEK LETTER MU) */
                { "µs",      NSEC_प्रत्येकम्_USEC   }, /* U+b5 (aka MICRO SIGN) */
                { "nsec",    1ULL            },
                { "ns",      1ULL            },
                { "",        1ULL            }, /* default is nsec */
        };

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        FOREACH_ELEMENT(i, सारणी) {
                ध्रुव वर्ण *e = startswith(p, i->suffix);
                यदि (e) {
                        *ret = i->nsec;
                        प्रत्यागम e;
                }
        }

        प्रत्यागम p;
}

पूर्णाङ्क parse_nsec(ध्रुव वर्ण *t, nsec_t *ret) {
        ध्रुव वर्ण *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        यदि (s) {
                s += strspn(s, WHITESPACE);
                यदि (*s != 0)
                        प्रत्यागम -EINVAL;

                *ret = NSEC_INFINITY;
                प्रत्यागम 0;
        }

        प्रति (;;) {
                nsec_t multiplier = 1, k;
                दीर्घ दीर्घ l;
                वर्ण *e;

                p += strspn(p, WHITESPACE);

                यदि (*p == 0) {
                        यदि (!something)
                                प्रत्यागम -EINVAL;

                        भङ्ग;
                }

                यदि (*p == '-') /* Don't allow "-0" */
                        प्रत्यागम -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                यदि (errno > 0)
                        प्रत्यागम -errno;
                यदि (l < 0)
                        प्रत्यागम -ERANGE;

                यदि (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } अन्यथा यदि (e == p)
                        प्रत्यागम -EINVAL;
                अन्यथा
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                यदि (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        प्रत्यागम -EINVAL;

                p = s;

                यदि ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        प्रत्यागम -ERANGE;

                k = (nsec_t) l * multiplier;
                यदि (k >= NSEC_INFINITY - nsec)
                        प्रत्यागम -ERANGE;

                nsec += k;

                something = true;

                यदि (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ध्रुव वर्ण *b;

                        प्रति (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                यदि (k >= NSEC_INFINITY - nsec)
                                        प्रत्यागम -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        यदि (b == e + 1)
                                प्रत्यागम -EINVAL;
                }
        }

        *ret = nsec;

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क get_timezones_from_zone1970_tab(वर्ण ***ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_strv_free_ वर्ण **zones = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        यदि (!f)
                प्रत्यागम -errno;

        प्रति (;;) {
                _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त, *cc = रिक्त, *co = रिक्त, *tz = रिक्त;

                r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;

                ध्रुव वर्ण *p = पङ्क्तिः;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_शब्दाः(&p, रिक्त, 0, &cc, &co, &tz);
                यदि (r < 0)
                        अनुवर्त;

                /* Lines that start with # are comments. */
                यदि (*cc == '#')
                        अनुवर्त;

                यदि (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        अनुवर्त;

                r = strv_extend(&zones, tz);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        *ret = TAKE_PTR(zones);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क get_timezones_from_tzdata_zi(वर्ण ***ret) {
        _cleanup_fclose_ FILE *f = रिक्त;
        _cleanup_strv_free_ वर्ण **zones = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        यदि (!f)
                प्रत्यागम -errno;

        प्रति (;;) {
                _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त, *type = रिक्त, *f1 = रिक्त, *f2 = रिक्त;

                r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;

                ध्रुव वर्ण *p = पङ्क्तिः;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_शब्दाः(&p, रिक्त, 0, &type, &f1, &f2);
                यदि (r < 0)
                        अनुवर्त;

                वर्ण *tz;
                यदि (अन्तर्_समुच्चयः(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                अन्यथा यदि (अन्तर्_समुच्चयः(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                अन्यथा
                        /* Not a line we care about. */
                        अनुवर्त;

                यदि (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        अनुवर्त;

                r = strv_extend(&zones, tz);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        *ret = TAKE_PTR(zones);
        प्रत्यागम 0;
}

पूर्णाङ्क get_timezones(वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **zones = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        यदि (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                यदि (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        यदि (r < 0 && r != -ENOENT)
                प्रत्यागम r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        यदि (r < 0)
                प्रत्यागम r;

        strv_क्रमय_uniq(zones);

        *ret = TAKE_PTR(zones);
        प्रत्यागम 0;
}

पूर्णाङ्क verify_timezone(ध्रुव वर्ण *name, पूर्णाङ्क log_level) {
        bool slash = false;
        ध्रुव वर्ण *p, *t;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        वर्ण buf[4];
        पूर्णाङ्क r;

        यदि (isempty(name))
                प्रत्यागम -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        यदि (streq(name, "UTC"))
                प्रत्यागम 0;

        यदि (name[0] == '/')
                प्रत्यागम -EINVAL;

        प्रति (p = name; *p; p++) {
                यदि (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !अन्तर्_समुच्चयः(*p, '-', '_', '+', '/'))
                        प्रत्यागम -EINVAL;

                यदि (*p == '/') {

                        यदि (slash)
                                प्रत्यागम -EINVAL;

                        slash = true;
                } अन्यथा
                        slash = false;
        }

        यदि (slash)
                प्रत्यागम -EINVAL;

        यदि (p - name >= PATH_MAX)
                प्रत्यागम -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = उद्घाटय(t, O_RDONLY|O_CLOEXEC);
        यदि (fd < 0)
                प्रत्यागम log_पूर्णम्_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        यदि (r < 0)
                प्रत्यागम log_पूर्णम्_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        यदि (r < 0)
                प्रत्यागम log_पूर्णम्_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        यदि (memcmp(buf, "TZif", 4) != 0)
                प्रत्यागम log_पूर्णम्_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        प्रत्यागम 0;
}

शून्य reset_timezonep(वर्ण **p) {
        प्रतिज्ञा(p);

        (शून्य) समुच्चयः_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

वर्ण* save_timezone(शून्य) {
        ध्रुव वर्ण *e = getenv("TZ");
        यदि (!e)
                प्रत्यागम रिक्त;

        वर्ण *s = strdup(e);
        यदि (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        प्रत्यागम s;
}

bool clock_supported(clockid_t clock) {
        संरचना timespec ts;

        विकल्प (clock) {

        पक्ष CLOCK_MONOTONIC:
        पक्ष CLOCK_REALTIME:
        पक्ष CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                प्रत्यागम true;

        स्वभाव:
                /* For everything else, check properly */
                प्रत्यागम clock_gettime(clock, &ts) >= 0;
        }
}

पूर्णाङ्क get_timezone(वर्ण **ret) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = readlink_malloc(etc_localtime(), &t);
        यदि (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                प्रत्यागम strdup_गन्तव्यम्(ret, "UTC");
        यदि (r < 0)
                प्रत्यागम r; /* Return EINVAL if not a symlink */

        ध्रुव वर्ण *e = मार्गः_STARTSWITH_समुच्चयः(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        यदि (!e)
                प्रत्यागम -EINVAL;
        यदि (!timezone_is_valid(e, LOG_DEBUG))
                प्रत्यागम -EINVAL;

        प्रत्यागम strdup_गन्तव्यम्(ret, e);
}

पूर्णाङ्क get_timezone_prefer_env(वर्ण **ret) {
        प्रतिज्ञा(ret);

        ध्रुव वर्ण *e = getenv("TZ");
        यदि (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                प्रत्यागम strdup_गन्तव्यम्(ret, e + 1);

        प्रत्यागम get_timezone(ret);
}

ध्रुव वर्ण* etc_localtime(शून्य) {
        स्थिर ध्रुव वर्ण *cached = रिक्त;

        यदि (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        प्रत्यागम cached;
}

पूर्णाङ्क mktime_or_timegm_usec(
                संरचना tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        प्रतिज्ञा(tm);

        यदि (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                प्रत्यागम -ERANGE;
        यदि ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_प्रत्येकम्_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                प्रत्यागम -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        यदि (t < 0) /* Refuse negative times and errors */
                प्रत्यागम -ERANGE;
        यदि ((usec_t) t >= USEC_INFINITY / USEC_प्रत्येकम्_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                प्रत्यागम -ERANGE;

        यदि (ret)
                *ret = (usec_t) t * USEC_प्रत्येकम्_SEC;
        प्रत्यागम 0;
}

पूर्णाङ्क localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                संरचना tm *ret) {

        t /= USEC_प्रत्येकम्_SEC; /* Round down */
        यदि (t > (usec_t) TIME_T_MAX)
                प्रत्यागम -ERANGE;
        time_t sec = (time_t) t;

        संरचना tm buf = {};
        यदि (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                प्रत्यागम -EINVAL;

        यदि (ret)
                *ret = buf;

        प्रत्यागम 0;
}

uint64_t sysconf_clock_ticks_cached(शून्य) {
        स्थिर thread_local uint64_t hz = 0;

        यदि (hz != 0)
                प्रत्यागम hz;

        दीर्घ t = sysconf(_SC_CLK_TCK);
        प्रतिज्ञा(t > 0);
        hz = (uint64_t) t;

        प्रत्यागम hz;
}

uint32_t usec_गन्तव्यम्_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        प्रत्यागम DIV_आवर्तनम्_ऊर्ध्वम्(u, USEC_प्रत्येकम्_SEC / hz);
}

usec_t jiffies_गन्तव्यम्_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        प्रत्यागम DIV_आवर्तनम्_ऊर्ध्वम्(j * USEC_प्रत्येकम्_SEC, hz);
}

usec_t usec_सरणम्_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        यदि (x == USEC_INFINITY)
                प्रत्यागम USEC_INFINITY;
        यदि (map_clock_id(from) == map_clock_id(to))
                प्रत्यागम x;

        a = now(from);
        b = now(to);

        यदि (x > a)
                /* x lies in the future */
                प्रत्यागम usec_सङ्कलय(b, usec_sub_unsigned(x, a));
        अन्यथा
                /* x lies in the past */
                प्रत्यागम usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool अन्तर्_utc_timezone(शून्य) {
        tzset();

        प्रत्यागम timezone == 0 && daylight == 0;
}

पूर्णाङ्क usleep_safe(usec_t usec) {
        पूर्णाङ्क r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        यदि (usec == 0)
                प्रत्यागम 0;

        यदि (usec == USEC_INFINITY)
                प्रत्यागम RET_NERRNO(pause());

        संरचना timespec t;
        timespec_store(&t, usec);

        प्रति (;;) {
                संरचना timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                यदि (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        अनुवर्त;
                }

                प्रत्यागम r;
        }
}

पूर्णाङ्क time_change_fd(शून्य) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        स्थिर ध्रुव संरचना itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        यदि (fd < 0)
                प्रत्यागम -errno;

        यदि (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, रिक्त) >= 0)
                प्रत्यागम TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#यदि SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        यदि (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                स्थिर ध्रुव संरचना itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                यदि (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, रिक्त) >= 0)
                        प्रत्यागम TAKE_FD(fd);
        }
#यदि_अन्तः

        प्रत्यागम -errno;
}

स्थिर ध्रुव वर्ण* ध्रुव timestamp_style_सारणी[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ध्रुव वर्ण *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_सारणी_lookup_from_string(timestamp_style_सारणी, ELEMENTSOF(timestamp_style_सारणी), s);
        यदि (t >= 0)
                प्रत्यागम t;
        यदि (STRPTR_अन्तर्_समुच्चयः(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                प्रत्यागम TIMESTAMP_US;
        यदि (STRPTR_अन्तर्_समुच्चयः(s, "µs+utc", "μs+utc"))
                प्रत्यागम TIMESTAMP_US_UTC;
        प्रत्यागम t;
}

पूर्णाङ्क parse_calendar_date_पूर्णम्(ध्रुव वर्ण *s, bool allow_pre_epoch, usec_t *ret_usec, संरचना tm *ret_tm) {
        संरचना tm parsed_tm = {}, copy_tm;
        ध्रुव वर्ण *k;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        यदि (!k || *k)
                प्रत्यागम -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        यदि (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                यदि (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        प्रत्यागम -EINVAL;
        } अन्यथा {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        यदि (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                प्रत्यागम -EINVAL;

        यदि (ret_usec)
                *ret_usec = usec;
        यदि (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        प्रत्यागम 0;
}
