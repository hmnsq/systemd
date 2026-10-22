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
                फर्काऊ CLOCK_BOOTTIME;

        पक्ष CLOCK_REALTIME_ALARM:
                फर्काऊ CLOCK_REALTIME;

        स्वभाव:
                फर्काऊ c;
        }
}

usec_t now(clockid_t clock_id) {
        संरचना timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        प्रतिज्ञा(n > 0);
        प्रतिज्ञा(n < USEC_INFINITY);

        फर्काऊ n;
}

nsec_t now_nsec(clockid_t clock_id) {
        संरचना timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        प्रतिज्ञा(n > 0);
        प्रतिज्ञा(n < NSEC_INFINITY);

        फर्काऊ n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        प्रतिज्ञा(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        फर्काऊ ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        प्रतिज्ञा(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        फर्काऊ ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        यदि (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                यदि (to_base >= USEC_INFINITY - delta) /* overflow? */
                        फर्काऊ USEC_INFINITY;

                फर्काऊ to_base + delta;

        } अन्यथा { /* In the past */
                usec_t delta = from_base - from;

                यदि (to_base <= delta) /* underflow? */
                        फर्काऊ 0;

                फर्काऊ to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        यदि (map_clock_id(from_clock) == map_clock_id(to_clock))
                फर्काऊ from;

        /* Keep infinity as is */
        यदि (from == USEC_INFINITY)
                फर्काऊ from;

        फर्काऊ map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        प्रतिज्ञा(ts);

        यदि (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                फर्काऊ ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        फर्काऊ ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        प्रतिज्ञा(ts);

        यदि (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                फर्काऊ ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        फर्काऊ ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        प्रतिज्ञा(ts);

        यदि (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                फर्काऊ ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        फर्काऊ ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        प्रतिज्ञा(ts);

        यदि (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                फर्काऊ ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        फर्काऊ ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        प्रतिज्ञा(ts);

        यदि (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                फर्काऊ ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        फर्काऊ ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t घड़ी) {
        प्रतिज्ञा(ts);

        विकल्प (घड़ी) {

        पक्ष CLOCK_REALTIME:
        पक्ष CLOCK_REALTIME_ALARM:
                फर्काऊ ts->realtime;

        पक्ष CLOCK_MONOTONIC:
                फर्काऊ ts->monotonic;

        पक्ष CLOCK_BOOTTIME:
        पक्ष CLOCK_BOOTTIME_ALARM:
                फर्काऊ ts->boottime;

        स्वभाव:
                फर्काऊ USEC_INFINITY;
        }
}

usec_t timespec_load(अचर संरचना timespec *ts) {
        प्रतिज्ञा(ts);

        यदि (ts->tv_sec < 0 || ts->tv_nsec < 0)
                फर्काऊ USEC_INFINITY;

        यदि ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                फर्काऊ USEC_INFINITY;

        फर्काऊ
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(अचर संरचना timespec *ts) {
        प्रतिज्ञा(ts);

        यदि (ts->tv_sec < 0 || ts->tv_nsec < 0)
                फर्काऊ NSEC_INFINITY;

        यदि ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                फर्काऊ NSEC_INFINITY;

        फर्काऊ (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

संरचना timespec *timespec_store(संरचना timespec *ts, usec_t u) {
        प्रतिज्ञा(ts);

        यदि (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (समय_प्रकार) -1;
                ts->tv_nsec = -1L;
                फर्काऊ ts;
        }

        ts->tv_sec = (समय_प्रकार) (u / USEC_PER_SEC);
        ts->tv_nsec = (दीर्घ) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        फर्काऊ ts;
}

संरचना timespec *timespec_store_nsec(संरचना timespec *ts, nsec_t n) {
        प्रतिज्ञा(ts);

        यदि (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (समय_प्रकार) -1;
                ts->tv_nsec = -1L;
                फर्काऊ ts;
        }

        ts->tv_sec = (समय_प्रकार) (n / NSEC_PER_SEC);
        ts->tv_nsec = (दीर्घ) (n % NSEC_PER_SEC);

        फर्काऊ ts;
}

संरचना __kernel_timespec *kernel_timespec_store(संरचना __kernel_timespec *ts, usec_t u) {
        प्रतिज्ञा(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        प्रतिज्ञा(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        फर्काऊ ts;
}

usec_t timeval_load(अचर संरचना timeval *tv) {
        प्रतिज्ञा(tv);

        यदि (tv->tv_sec < 0 || tv->tv_usec < 0)
                फर्काऊ USEC_INFINITY;

        यदि ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                फर्काऊ USEC_INFINITY;

        फर्काऊ
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

संरचना timeval *timeval_store(संरचना timeval *tv, usec_t u) {
        प्रतिज्ञा(tv);

        यदि (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (समय_प्रकार) -1;
                tv->tv_usec = (suseconds_t) -1;
        } अन्यथा {
                tv->tv_sec = (समय_प्रकार) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        फर्काऊ tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
स्थिर अचर वर्ण *अचर weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, पूर्णाङ्क);

वर्ण* format_timestamp_style(
                वर्ण *buf,
                माप_प्रकार l,
                usec_t t,
                TimestampStyle style) {
        संरचना पंचांग पंचांग;
        bool utc, us;
        माप_प्रकार n;

        प्रतिज्ञा(buf);
        प्रतिज्ञा(style >= 0);
        प्रतिज्ञा(style < _TIMESTAMP_STYLE_MAX);

        यदि (!timestamp_is_set(t))
                फर्काऊ रिक्त; /* Timestamp is unset */

        यदि (style == TIMESTAMP_UNIX) {
                यदि (l < (माप_प्रकार) (1 + 1 + 1))
                        फर्काऊ रिक्त; /* not enough space for even the shortest of forms */

                फर्काऊ snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        यदि (l < (माप_प्रकार) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                फर्काऊ रिक्त; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        यदि (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                स्थिर अचर वर्ण* अचर xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                प्रतिज्ञा(l >= म_माप(xxx[style]) + 1);
                फर्काऊ म_नकल(buf, xxx[style]);
        }

        यदि (localtime_or_gmtime_usec(t, utc, &पंचांग) < 0)
                फर्काऊ रिक्त;

        /* Start with the week day */
        अचर वर्ण *weekday = weekday_to_string(पंचांग.tm_wday == 0 ? 6 : पंचांग.tm_wday - 1);
        प्रतिज्ञा(weekday);
        स_नकल(buf, weekday, 4);

        यदि (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                यदि (स_माला(buf + 3, l - 3, " %Y-%m-%d", &पंचांग) <= 0)
                        फर्काऊ रिक्त; /* Doesn't fit */

                फर्काऊ buf;
        }

        /* Add the main components */
        यदि (स_माला(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &पंचांग) <= 0)
                फर्काऊ रिक्त; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        यदि (us) {
                n = म_माप(buf);
                यदि (n + 8 > l)
                        फर्काऊ रिक्त; /* Microseconds part doesn't fit. */

                प्र_लिखो(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = म_माप(buf);
        यदि (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                यदि (n + 5 > l)
                        फर्काऊ रिक्त; /* "UTC" doesn't fit. */

                म_नकल(buf + n, " UTC");

        } अन्यथा यदि (!isempty(पंचांग.tm_zone)) {
                माप_प्रकार tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = म_माप(पंचांग.tm_zone);
                यदि (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        यदि (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                फर्काऊ रिक्त; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } अन्यथा {
                        buf[n++] = ' ';
                        म_नकल(buf + n, पंचांग.tm_zone);
                }
        }

        फर्काऊ buf;
}

वर्ण* format_timestamp_relative_full(वर्ण *buf, माप_प्रकार l, usec_t t, clockid_t घड़ी, bool implicit_बायाँ) {
        अचर वर्ण *s;
        usec_t n, d;

        प्रतिज्ञा(buf);

        यदि (!timestamp_is_set(t))
                फर्काऊ रिक्त;

        n = now(घड़ी);
        यदि (n > t) {
                d = n - t;
                s = " ago";
        } अन्यथा {
                d = t - n;
                s = implicit_बायाँ ? "" : " left";
        }

        यदि (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } अन्यथा यदि (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } अन्यथा यदि (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } अन्यथा यदि (d >= 2*USEC_PER_DAY)
                (शून्य) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        अन्यथा यदि (d >= 25*USEC_PER_HOUR)
                (शून्य) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        अन्यथा यदि (d >= 6*USEC_PER_HOUR)
                (शून्य) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        अन्यथा यदि (d >= USEC_PER_HOUR)
                (शून्य) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        अन्यथा यदि (d >= 5*USEC_PER_MINUTE)
                (शून्य) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        अन्यथा यदि (d >= USEC_PER_MINUTE)
                (शून्य) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        अन्यथा यदि (d >= USEC_PER_SEC)
                (शून्य) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        अन्यथा यदि (d >= USEC_PER_MSEC)
                (शून्य) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        अन्यथा यदि (d > 0)
                (शून्य) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        अन्यथा
                (शून्य) snprintf(buf, l, "now");

        buf[l-1] = 0;
        फर्काऊ buf;
}

वर्ण* format_timespan(वर्ण *buf, माप_प्रकार l, usec_t t, usec_t accuracy) {
        स्थिर अचर संरचना {
                अचर वर्ण *suffix;
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

        वर्ण *p = ASSERT_PTR(buf);
        bool something = false;

        प्रतिज्ञा(l > 0);

        यदि (t == USEC_INFINITY) {
                म_नकलन(p, "infinity", l-1);
                p[l-1] = 0;
                फर्काऊ p;
        }

        यदि (t <= 0) {
                म_नकलन(p, "0", l-1);
                p[l-1] = 0;
                फर्काऊ p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                पूर्णाङ्क k = 0;
                माप_प्रकार n;
                bool done = false;
                usec_t a, b;

                यदि (t <= 0)
                        रोक;

                यदि (t < accuracy && something)
                        रोक;

                यदि (t < i->usec)
                        जारी;

                यदि (l <= 1)
                        रोक;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                यदि (t < USEC_PER_MINUTE && b > 0) {
                        चिन्हित वर्ण j = 0;

                        लागि (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        लागि (usec_t cc = accuracy; cc > 1; cc /= 10) {
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
                                done = true;
                        }
                }

                /* No? Then let's show it normally */
                यदि (!done) {
                        k = snprintf(p, l,
                                     "%s"USEC_FMT"%s",
                                     p > buf ? " " : "",
                                     a,
                                     i->suffix);

                        t = b;
                }

                n = MIN((माप_प्रकार) k, l-1);

                l -= n;
                p += n;

                something = true;
        }

        *p = 0;

        फर्काऊ buf;
}

अचर वर्ण* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        यदि (dst && isempty(tzname[1]))
                dst = false;

        फर्काऊ empty_to_null(tzname[dst]);
}

पूर्णाङ्क parse_gmtoff(अचर वर्ण *t, दीर्घ *ret) {
        प्रतिज्ञा(t);

        संरचना पंचांग पंचांग;
        अचर वर्ण *k = strptime(t, "%z", &पंचांग);
        यदि (k && *k == '\0') {
                /* Success! */
                यदि (ret)
                        *ret = पंचांग.tm_gmtoff;
                फर्काऊ 0;
        }

#ifdef __GLIBC__
        फर्काऊ -EINVAL;
#अन्यथा
        पूर्णाङ्क r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        विकल्प (*t) {
        पक्ष '+':
                positive = true;
                रोक;
        पक्ष '-':
                positive = false;
                रोक;
        स्वभाव:
                फर्काऊ -EINVAL;
        }

        t++;
        r = undecchar(*t);
        यदि (r < 0)
                फर्काऊ r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        यदि (r < 0)
                फर्काऊ r;
        u += r * USEC_PER_HOUR;

        t++;
        यदि (*t == '\0') /* 2 digits case */
                गच्छ finalize;

        यदि (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        यदि (r < 0)
                फर्काऊ r;
        यदि (r >= 6) /* refuse minutes equal to or larger than 60 */
                फर्काऊ -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        यदि (r < 0)
                फर्काऊ r;

        u += r * USEC_PER_MINUTE;

        t++;
        यदि (*t != '\0')
                फर्काऊ -EINVAL;

finalize:
        यदि (u > USEC_PER_DAY) /* refuse larger than one day */
                फर्काऊ -EINVAL;

        यदि (ret) {
                दीर्घ gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        फर्काऊ 0;
#endif
}

स्थिर पूर्णाङ्क parse_timestamp_impl(
                अचर वर्ण *t,
                माप_प्रकार max_len,
                bool utc,
                पूर्णाङ्क isdst,
                दीर्घ gmtoff,
                usec_t *ret) {

        स्थिर अचर संरचना {
                अचर वर्ण *name;
                अचर पूर्णाङ्क nr;
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
        अचिन्हित fractional = 0;
        अचर वर्ण *k;
        संरचना पंचांग पंचांग, copy;

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
                        फर्काऊ -EINVAL;

                t_alloc = strndup(t, max_len);
                यदि (!t_alloc)
                        फर्काऊ -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        यदि (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                यदि ((usec_t) द_असल(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        फर्काऊ -EINVAL;
        } अन्यथा {
                यदि (gmtoff != 0)
                        फर्काऊ -EINVAL;
        }

        यदि (t[0] == '@' && !with_tz)
                फर्काऊ parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        यदि (!with_tz) {
                यदि (streq(t, "now"))
                        गच्छ finish;

                यदि (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        यदि (r < 0)
                                फर्काऊ r;

                        गच्छ finish;
                }

                यदि (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        यदि (r < 0)
                                फर्काऊ r;

                        गच्छ finish;
                }

                यदि ((k = endswith(t, " ago"))) {
                        _cleanup_free_ वर्ण *buf = रिक्त;

                        buf = strndup(t, k - t);
                        यदि (!buf)
                                फर्काऊ -ENOMEM;

                        r = parse_sec(buf, &minus);
                        यदि (r < 0)
                                फर्काऊ r;

                        गच्छ finish;
                }

                यदि ((k = endswith(t, " left"))) {
                        _cleanup_free_ वर्ण *buf = रिक्त;

                        buf = strndup(t, k - t);
                        यदि (!buf)
                                फर्काऊ -ENOMEM;

                        r = parse_sec(buf, &plus);
                        यदि (r < 0)
                                फर्काऊ r;

                        गच्छ finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &पंचांग);
        यदि (r < 0)
                फर्काऊ r;

        पंचांग.tm_isdst = isdst;

        यदि (streq(t, "today")) {
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                गच्छ from_tm;

        } अन्यथा यदि (streq(t, "yesterday")) {
                पंचांग.tm_mday--;
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                गच्छ from_tm;

        } अन्यथा यदि (streq(t, "tomorrow")) {
                पंचांग.tm_mday++;
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                गच्छ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                यदि (!k || *k != ' ')
                        जारी;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                रोक;
        }

        copy = पंचांग;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &पंचांग);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        पंचांग = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &पंचांग);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        /* RFC3339 syntax */
        पंचांग = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &पंचांग);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        पंचांग = copy;
        k = strptime(t, "%b %d %H:%M:%S", &पंचांग);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &पंचांग);
        यदि (k && *k == 0) {
                पंचांग.tm_sec = 0;
                गच्छ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        पंचांग = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &पंचांग);
        यदि (k && *k == 0) {
                पंचांग.tm_sec = 0;
                गच्छ from_tm;
        }

        /* RFC3339 syntax without seconds */
        पंचांग = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &पंचांग);
        यदि (k && *k == 0) {
                पंचांग.tm_sec = 0;
                गच्छ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%y-%m-%d", &पंचांग);
        यदि (k && *k == 0) {
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                गच्छ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%Y-%m-%d", &पंचांग);
        यदि (k && *k == 0) {
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                गच्छ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%H:%M:%S", &पंचांग);
        यदि (k) {
                यदि (*k == '.')
                        गच्छ parse_usec;
                अन्यथा यदि (*k == 0)
                        गच्छ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%H:%M", &पंचांग);
        यदि (k && *k == 0) {
                पंचांग.tm_sec = 0;
                गच्छ from_tm;
        }

        फर्काऊ -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        यदि (r < 0)
                फर्काऊ -EINVAL;
        यदि (*k != '\0')
                फर्काऊ -EINVAL;

from_tm:
        प्रतिज्ञा(plus == 0);
        प्रतिज्ञा(minus == 0);

        यदि (weekday >= 0 && पंचांग.tm_wday != weekday)
                फर्काऊ -EINVAL;

        यदि (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                यदि (पंचांग.tm_year == 69 && पंचांग.tm_mon == 11 && पंचांग.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        पंचांग.tm_year = 70;
                        पंचांग.tm_mon = 0;
                        पंचांग.tm_mday = 1;
                        पंचांग.tm_wday = 4;
                        पंचांग.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } अन्यथा
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&पंचांग, utc, &usec);
        यदि (r < 0)
                फर्काऊ r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        यदि (usec < minus)
                फर्काऊ -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        यदि (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                फर्काऊ -EINVAL;

        यदि (ret)
                *ret = usec;
        फर्काऊ 0;
}

पूर्णाङ्क parse_timestamp(अचर वर्ण *t, usec_t *ret) {
        दीर्घ gmtoff;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);

        माप_प्रकार t_len = म_माप(t);
        यदि (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                यदि (r >= 0)
                        फर्काऊ r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        यदि (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                फर्काऊ parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        अचर वर्ण *tz = म_खोजप(t, ' ');
        यदि (!tz)
                फर्काऊ parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        माप_प्रकार max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        यदि (streq(tz, "UTC"))
                फर्काऊ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        यदि (parse_gmtoff(tz, &gmtoff) >= 0)
                फर्काऊ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        लागि (पूर्णाङ्क j = 0; j <= 1; j++) {
                यदि (!streq_ptr(tz, get_tzname(j)))
                        जारी;

                /* The specified timezone matches tzname[] of the local timezone. */
                फर्काऊ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        यदि (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                यदि (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        फर्काऊ negative_errno();

                फर्काऊ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        फर्काऊ parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

स्थिर अचर वर्ण* extract_multiplier(अचर वर्ण *p, usec_t *ret) {
        स्थिर अचर संरचना {
                अचर वर्ण *suffix;
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

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        FOREACH_ELEMENT(i, table) {
                अचर वर्ण *e = startswith(p, i->suffix);
                यदि (e) {
                        *ret = i->usec;
                        फर्काऊ e;
                }
        }

        फर्काऊ p;
}

पूर्णाङ्क parse_time(अचर वर्ण *t, usec_t *ret, usec_t default_unit) {
        अचर वर्ण *p, *s;

        प्रतिज्ञा(t);
        प्रतिज्ञा(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ रिक्त);
        s = startswith(p, "infinity");
        यदि (s) {
                यदि (!in_charset(s, WHITESPACE))
                        फर्काऊ -EINVAL;

                यदि (ret)
                        *ret = USEC_INFINITY;
                फर्काऊ 0;
        }

        usec_t usec = 0;

        लागि (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                दीर्घ दीर्घ l;
                वर्ण *e;

                p = skip_leading_chars(p, /* bad= */ रिक्त);
                यदि (*p == 0) {
                        यदि (!something)
                                फर्काऊ -EINVAL;

                        रोक;
                }

                यदि (*p == '-') /* Don't allow "-0" */
                        फर्काऊ -दुस्फल;

                त्रुटि_सं = 0;
                l = strtoll(p, &e, 10);
                यदि (त्रुटि_सं > 0)
                        फर्काऊ -त्रुटि_सं;
                यदि (l < 0)
                        फर्काऊ -दुस्फल;

                यदि (*e == '.') {
                        p = e + 1;
                        p += म_अखोज(p, DIGITS);
                } अन्यथा यदि (e == p)
                        फर्काऊ -EINVAL;
                अन्यथा
                        p = e;

                s = extract_multiplier(p + म_अखोज(p, WHITESPACE), &multiplier);
                यदि (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        फर्काऊ -EINVAL;

                p = s;

                यदि ((usec_t) l >= USEC_INFINITY / multiplier)
                        फर्काऊ -दुस्फल;

                k = (usec_t) l * multiplier;
                यदि (k >= USEC_INFINITY - usec)
                        फर्काऊ -दुस्फल;

                usec += k;

                something = true;

                यदि (*e == '.') {
                        usec_t m = multiplier / 10;
                        अचर वर्ण *b;

                        लागि (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                यदि (k >= USEC_INFINITY - usec)
                                        फर्काऊ -दुस्फल;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        यदि (b == e + 1)
                                फर्काऊ -EINVAL;
                }
        }

        यदि (ret)
                *ret = usec;
        फर्काऊ 0;
}

पूर्णाङ्क parse_sec(अचर वर्ण *t, usec_t *ret) {
        फर्काऊ parse_time(t, ret, USEC_PER_SEC);
}

पूर्णाङ्क parse_sec_fix_0(अचर वर्ण *t, usec_t *ret) {
        usec_t k;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        r = parse_sec(t, &k);
        यदि (r < 0)
                फर्काऊ r;

        *ret = k == 0 ? USEC_INFINITY : k;
        फर्काऊ r;
}

पूर्णाङ्क parse_sec_def_infinity(अचर वर्ण *t, usec_t *ret) {
        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        t += म_अखोज(t, WHITESPACE);
        यदि (isempty(t)) {
                *ret = USEC_INFINITY;
                फर्काऊ 0;
        }
        फर्काऊ parse_sec(t, ret);
}

स्थिर अचर वर्ण* extract_nsec_multiplier(अचर वर्ण *p, nsec_t *ret) {
        स्थिर अचर संरचना {
                अचर वर्ण *suffix;
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

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        FOREACH_ELEMENT(i, table) {
                अचर वर्ण *e = startswith(p, i->suffix);
                यदि (e) {
                        *ret = i->nsec;
                        फर्काऊ e;
                }
        }

        फर्काऊ p;
}

पूर्णाङ्क parse_nsec(अचर वर्ण *t, nsec_t *ret) {
        अचर वर्ण *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        p = t;

        p += म_अखोज(p, WHITESPACE);
        s = startswith(p, "infinity");
        यदि (s) {
                s += म_अखोज(s, WHITESPACE);
                यदि (*s != 0)
                        फर्काऊ -EINVAL;

                *ret = NSEC_INFINITY;
                फर्काऊ 0;
        }

        लागि (;;) {
                nsec_t multiplier = 1, k;
                दीर्घ दीर्घ l;
                वर्ण *e;

                p += म_अखोज(p, WHITESPACE);

                यदि (*p == 0) {
                        यदि (!something)
                                फर्काऊ -EINVAL;

                        रोक;
                }

                यदि (*p == '-') /* Don't allow "-0" */
                        फर्काऊ -दुस्फल;

                त्रुटि_सं = 0;
                l = strtoll(p, &e, 10);
                यदि (त्रुटि_सं > 0)
                        फर्काऊ -त्रुटि_सं;
                यदि (l < 0)
                        फर्काऊ -दुस्फल;

                यदि (*e == '.') {
                        p = e + 1;
                        p += म_अखोज(p, DIGITS);
                } अन्यथा यदि (e == p)
                        फर्काऊ -EINVAL;
                अन्यथा
                        p = e;

                s = extract_nsec_multiplier(p + म_अखोज(p, WHITESPACE), &multiplier);
                यदि (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        फर्काऊ -EINVAL;

                p = s;

                यदि ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        फर्काऊ -दुस्फल;

                k = (nsec_t) l * multiplier;
                यदि (k >= NSEC_INFINITY - nsec)
                        फर्काऊ -दुस्फल;

                nsec += k;

                something = true;

                यदि (*e == '.') {
                        nsec_t m = multiplier / 10;
                        अचर वर्ण *b;

                        लागि (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                यदि (k >= NSEC_INFINITY - nsec)
                                        फर्काऊ -दुस्फल;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        यदि (b == e + 1)
                                फर्काऊ -EINVAL;
                }
        }

        *ret = nsec;

        फर्काऊ 0;
}

स्थिर पूर्णाङ्क get_timezones_from_zone1970_tab(वर्ण ***ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_strv_free_ वर्ण **zones = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        f = ख_खोलो("/usr/share/zoneinfo/zone1970.tab", "re");
        यदि (!f)
                फर्काऊ -त्रुटि_सं;

        लागि (;;) {
                _cleanup_free_ वर्ण *पङ्क्ति = रिक्त, *cc = रिक्त, *co = रिक्त, *tz = रिक्त;

                r = read_पङ्क्ति(f, LONG_पङ्क्ति_MAX, &पङ्क्ति);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        रोक;

                अचर वर्ण *p = पङ्क्ति;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_शब्दहरू(&p, रिक्त, 0, &cc, &co, &tz);
                यदि (r < 0)
                        जारी;

                /* Lines that start with # are comments. */
                यदि (*cc == '#')
                        जारी;

                यदि (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        जारी;

                r = strv_extend(&zones, tz);
                यदि (r < 0)
                        फर्काऊ r;
        }

        *ret = TAKE_PTR(zones);
        फर्काऊ 0;
}

स्थिर पूर्णाङ्क get_timezones_from_tzdata_zi(वर्ण ***ret) {
        _cleanup_fclose_ खाता *f = रिक्त;
        _cleanup_strv_free_ वर्ण **zones = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        f = ख_खोलो("/usr/share/zoneinfo/tzdata.zi", "re");
        यदि (!f)
                फर्काऊ -त्रुटि_सं;

        लागि (;;) {
                _cleanup_free_ वर्ण *पङ्क्ति = रिक्त, *type = रिक्त, *f1 = रिक्त, *f2 = रिक्त;

                r = read_पङ्क्ति(f, LONG_पङ्क्ति_MAX, &पङ्क्ति);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        रोक;

                अचर वर्ण *p = पङ्क्ति;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_शब्दहरू(&p, रिक्त, 0, &type, &f1, &f2);
                यदि (r < 0)
                        जारी;

                वर्ण *tz;
                यदि (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                अन्यथा यदि (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                अन्यथा
                        /* Not a line we care about. */
                        जारी;

                यदि (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        जारी;

                r = strv_extend(&zones, tz);
                यदि (r < 0)
                        फर्काऊ r;
        }

        *ret = TAKE_PTR(zones);
        फर्काऊ 0;
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
                फर्काऊ r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        यदि (r < 0)
                फर्काऊ r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        फर्काऊ 0;
}

पूर्णाङ्क verify_timezone(अचर वर्ण *name, पूर्णाङ्क log_level) {
        bool slash = false;
        अचर वर्ण *p, *t;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        वर्ण buf[4];
        पूर्णाङ्क r;

        यदि (isempty(name))
                फर्काऊ -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        यदि (streq(name, "UTC"))
                फर्काऊ 0;

        यदि (name[0] == '/')
                फर्काऊ -EINVAL;

        लागि (p = name; *p; p++) {
                यदि (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        फर्काऊ -EINVAL;

                यदि (*p == '/') {

                        यदि (slash)
                                फर्काऊ -EINVAL;

                        slash = true;
                } अन्यथा
                        slash = false;
        }

        यदि (slash)
                फर्काऊ -EINVAL;

        यदि (p - name >= PATH_MAX)
                फर्काऊ -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = खोलो(t, O_RDONLY|O_CLOEXEC);
        यदि (fd < 0)
                फर्काऊ log_full_errno(log_level, त्रुटि_सं, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        यदि (r < 0)
                फर्काऊ log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        यदि (r < 0)
                फर्काऊ log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        यदि (स_भेद(buf, "TZif", 4) != 0)
                फर्काऊ log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        फर्काऊ 0;
}

शून्य reset_timezonep(वर्ण **p) {
        प्रतिज्ञा(p);

        (शून्य) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

वर्ण* save_timezone(शून्य) {
        अचर वर्ण *e = दो_पर्या("TZ");
        यदि (!e)
                फर्काऊ रिक्त;

        वर्ण *s = strdup(e);
        यदि (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        फर्काऊ s;
}

bool clock_supported(clockid_t घड़ी) {
        संरचना timespec ts;

        विकल्प (घड़ी) {

        पक्ष CLOCK_MONOTONIC:
        पक्ष CLOCK_REALTIME:
        पक्ष CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                फर्काऊ true;

        स्वभाव:
                /* For everything else, check properly */
                फर्काऊ clock_gettime(घड़ी, &ts) >= 0;
        }
}

पूर्णाङ्क get_timezone(वर्ण **ret) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = readlink_malloc(etc_localtime(), &t);
        यदि (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                फर्काऊ strdup_to(ret, "UTC");
        यदि (r < 0)
                फर्काऊ r; /* Return EINVAL if not a symlink */

        अचर वर्ण *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        यदि (!e)
                फर्काऊ -EINVAL;
        यदि (!timezone_is_valid(e, LOG_DEBUG))
                फर्काऊ -EINVAL;

        फर्काऊ strdup_to(ret, e);
}

पूर्णाङ्क get_timezone_prefer_env(वर्ण **ret) {
        प्रतिज्ञा(ret);

        अचर वर्ण *e = दो_पर्या("TZ");
        यदि (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                फर्काऊ strdup_to(ret, e + 1);

        फर्काऊ get_timezone(ret);
}

अचर वर्ण* etc_localtime(शून्य) {
        स्थिर अचर वर्ण *cached = रिक्त;

        यदि (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        फर्काऊ cached;
}

पूर्णाङ्क mktime_or_timegm_usec(
                संरचना पंचांग *पंचांग, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        समय_प्रकार t;

        प्रतिज्ञा(पंचांग);

        यदि (पंचांग->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                फर्काऊ -दुस्फल;
        यदि ((usec_t) पंचांग->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                फर्काऊ -दुस्फल;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(पंचांग) : स_गढ़ो(पंचांग);
        यदि (t < 0) /* Refuse negative times and errors */
                फर्काऊ -दुस्फल;
        यदि ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                फर्काऊ -दुस्फल;

        यदि (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        फर्काऊ 0;
}

पूर्णाङ्क localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                संरचना पंचांग *ret) {

        t /= USEC_PER_SEC; /* Round down */
        यदि (t > (usec_t) TIME_T_MAX)
                फर्काऊ -दुस्फल;
        समय_प्रकार sec = (समय_प्रकार) t;

        संरचना पंचांग buf = {};
        यदि (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                फर्काऊ -EINVAL;

        यदि (ret)
                *ret = buf;

        फर्काऊ 0;
}

uint64_t sysconf_clock_ticks_cached(शून्य) {
        स्थिर thread_local uint64_t hz = 0;

        यदि (hz != 0)
                फर्काऊ hz;

        दीर्घ t = sysconf(_SC_CLK_TCK);
        प्रतिज्ञा(t > 0);
        hz = (uint64_t) t;

        फर्काऊ hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        फर्काऊ DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        फर्काऊ DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        यदि (x == USEC_INFINITY)
                फर्काऊ USEC_INFINITY;
        यदि (map_clock_id(from) == map_clock_id(to))
                फर्काऊ x;

        a = now(from);
        b = now(to);

        यदि (x > a)
                /* x lies in the future */
                फर्काऊ usec_add(b, usec_sub_unsigned(x, a));
        अन्यथा
                /* x lies in the past */
                फर्काऊ usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(शून्य) {
        tzset();

        फर्काऊ timezone == 0 && daylight == 0;
}

पूर्णाङ्क usleep_safe(usec_t usec) {
        पूर्णाङ्क r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        यदि (usec == 0)
                फर्काऊ 0;

        यदि (usec == USEC_INFINITY)
                फर्काऊ RET_NERRNO(छोड़ो());

        संरचना timespec t;
        timespec_store(&t, usec);

        लागि (;;) {
                संरचना timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                यदि (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        जारी;
                }

                फर्काऊ r;
        }
}

पूर्णाङ्क time_change_fd(शून्य) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        स्थिर अचर संरचना itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        यदि (fd < 0)
                फर्काऊ -त्रुटि_सं;

        यदि (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, रिक्त) >= 0)
                फर्काऊ TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#यदि SIZEOF_TIME_T == 8 && अच_दीर्घ_उच्च < UINT64_MAX
        यदि (ERRNO_IS_NOT_SUPPORTED(त्रुटि_सं) || त्रुटि_सं == EOVERFLOW) {
                स्थिर अचर संरचना itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                यदि (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, रिक्त) >= 0)
                        फर्काऊ TAKE_FD(fd);
        }
#endif

        फर्काऊ -त्रुटि_सं;
}

स्थिर अचर वर्ण* अचर timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(अचर वर्ण *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        यदि (t >= 0)
                फर्काऊ t;
        यदि (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                फर्काऊ TIMESTAMP_US;
        यदि (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                फर्काऊ TIMESTAMP_US_UTC;
        फर्काऊ t;
}

पूर्णाङ्क parse_calendar_date_full(अचर वर्ण *s, bool allow_pre_epoch, usec_t *ret_usec, संरचना पंचांग *ret_tm) {
        संरचना पंचांग parsed_tm = {}, copy_tm;
        अचर वर्ण *k;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        यदि (!k || *k)
                फर्काऊ -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        यदि (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                यदि (timegm(&copy_tm) == (समय_प्रकार) -1 && copy_tm.tm_wday == -1)
                        फर्काऊ -EINVAL;
        } अन्यथा {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                यदि (r < 0)
                        फर्काऊ r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        यदि (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                फर्काऊ -EINVAL;

        यदि (ret_usec)
                *ret_usec = usec;
        यदि (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        फर्काऊ 0;
}
