/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <linux/time_types.h>
#समावेश <stdlib.h>
#समावेश <sys/timerfd.h>
#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "env-util.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "hexdecoct.h"          /* IWYU pragma: keep */
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "fs-util.h"
#समावेश "io-util.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "stat-util.h"
#समावेश "stdio-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "time-util.h"

अटल clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        चयन (c) {

        हाल CLOCK_BOOTTIME_ALARM:
                वापस CLOCK_BOOTTIME;

        हाल CLOCK_REALTIME_ALARM:
                वापस CLOCK_REALTIME;

        शेष:
                वापस c;
        }
}

usec_t now(clockid_t clock_id) {
        काष्ठा timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        निश्चित(n > 0);
        निश्चित(n < USEC_INFINITY);

        वापस n;
}

nsec_t now_nsec(clockid_t clock_id) {
        काष्ठा timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        निश्चित(n > 0);
        निश्चित(n < NSEC_INFINITY);

        वापस n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        निश्चित(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        वापस ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        निश्चित(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        वापस ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        अगर (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                अगर (to_base >= USEC_INFINITY - delta) /* overflow? */
                        वापस USEC_INFINITY;

                वापस to_base + delta;

        } अन्यथा { /* In the past */
                usec_t delta = from_base - from;

                अगर (to_base <= delta) /* underflow? */
                        वापस 0;

                वापस to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        अगर (map_clock_id(from_clock) == map_clock_id(to_clock))
                वापस from;

        /* Keep infinity as is */
        अगर (from == USEC_INFINITY)
                वापस from;

        वापस map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        निश्चित(ts);

        अगर (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                वापस ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        वापस ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        निश्चित(ts);

        अगर (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                वापस ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        वापस ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        निश्चित(ts);

        अगर (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                वापस ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        वापस ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        निश्चित(ts);

        अगर (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                वापस ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        वापस ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        निश्चित(ts);

        अगर (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                वापस ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        वापस ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t घड़ी) {
        निश्चित(ts);

        चयन (घड़ी) {

        हाल CLOCK_REALTIME:
        हाल CLOCK_REALTIME_ALARM:
                वापस ts->realtime;

        हाल CLOCK_MONOTONIC:
                वापस ts->monotonic;

        हाल CLOCK_BOOTTIME:
        हाल CLOCK_BOOTTIME_ALARM:
                वापस ts->boottime;

        शेष:
                वापस USEC_INFINITY;
        }
}

usec_t timespec_load(स्थिर काष्ठा timespec *ts) {
        निश्चित(ts);

        अगर (ts->tv_sec < 0 || ts->tv_nsec < 0)
                वापस USEC_INFINITY;

        अगर ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                वापस USEC_INFINITY;

        वापस
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(स्थिर काष्ठा timespec *ts) {
        निश्चित(ts);

        अगर (ts->tv_sec < 0 || ts->tv_nsec < 0)
                वापस NSEC_INFINITY;

        अगर ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                वापस NSEC_INFINITY;

        वापस (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

काष्ठा timespec *timespec_store(काष्ठा timespec *ts, usec_t u) {
        निश्चित(ts);

        अगर (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (समय_प्रकार) -1;
                ts->tv_nsec = -1L;
                वापस ts;
        }

        ts->tv_sec = (समय_प्रकार) (u / USEC_PER_SEC);
        ts->tv_nsec = (दीर्घ) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        वापस ts;
}

काष्ठा timespec *timespec_store_nsec(काष्ठा timespec *ts, nsec_t n) {
        निश्चित(ts);

        अगर (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (समय_प्रकार) -1;
                ts->tv_nsec = -1L;
                वापस ts;
        }

        ts->tv_sec = (समय_प्रकार) (n / NSEC_PER_SEC);
        ts->tv_nsec = (दीर्घ) (n % NSEC_PER_SEC);

        वापस ts;
}

काष्ठा __kernel_timespec *kernel_timespec_store(काष्ठा __kernel_timespec *ts, usec_t u) {
        निश्चित(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        निश्चित(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        वापस ts;
}

usec_t timeval_load(स्थिर काष्ठा timeval *tv) {
        निश्चित(tv);

        अगर (tv->tv_sec < 0 || tv->tv_usec < 0)
                वापस USEC_INFINITY;

        अगर ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                वापस USEC_INFINITY;

        वापस
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

काष्ठा timeval *timeval_store(काष्ठा timeval *tv, usec_t u) {
        निश्चित(tv);

        अगर (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (समय_प्रकार) -1;
                tv->tv_usec = (suseconds_t) -1;
        } अन्यथा {
                tv->tv_sec = (समय_प्रकार) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        वापस tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
अटल स्थिर अक्षर *स्थिर weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, पूर्णांक);

अक्षर* format_timestamp_style(
                अक्षर *buf,
                माप_प्रकार l,
                usec_t t,
                TimestampStyle style) {
        काष्ठा पंचांग पंचांग;
        bool utc, us;
        माप_प्रकार n;

        निश्चित(buf);
        निश्चित(style >= 0);
        निश्चित(style < _TIMESTAMP_STYLE_MAX);

        अगर (!timestamp_is_set(t))
                वापस शून्य; /* Timestamp is unset */

        अगर (style == TIMESTAMP_UNIX) {
                अगर (l < (माप_प्रकार) (1 + 1 + 1))
                        वापस शून्य; /* not enough space for even the shortest of forms */

                वापस snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        अगर (l < (माप_प्रकार) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                वापस शून्य; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        अगर (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                अटल स्थिर अक्षर* स्थिर xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                निश्चित(l >= म_माप(xxx[style]) + 1);
                वापस म_नकल(buf, xxx[style]);
        }

        अगर (localtime_or_gmtime_usec(t, utc, &पंचांग) < 0)
                वापस शून्य;

        /* Start with the week day */
        स्थिर अक्षर *weekday = weekday_to_string(पंचांग.tm_wday == 0 ? 6 : पंचांग.tm_wday - 1);
        निश्चित(weekday);
        स_नकल(buf, weekday, 4);

        अगर (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                अगर (स_माला(buf + 3, l - 3, " %Y-%m-%d", &पंचांग) <= 0)
                        वापस शून्य; /* Doesn't fit */

                वापस buf;
        }

        /* Add the main components */
        अगर (स_माला(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &पंचांग) <= 0)
                वापस शून्य; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        अगर (us) {
                n = म_माप(buf);
                अगर (n + 8 > l)
                        वापस शून्य; /* Microseconds part doesn't fit. */

                प्र_लिखो(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = म_माप(buf);
        अगर (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                अगर (n + 5 > l)
                        वापस शून्य; /* "UTC" doesn't fit. */

                म_नकल(buf + n, " UTC");

        } अन्यथा अगर (!isempty(पंचांग.tm_zone)) {
                माप_प्रकार tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = म_माप(पंचांग.tm_zone);
                अगर (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        अगर (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                वापस शून्य; /* Not even enough space for the POSIX minimum (of 6)? In that
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

        वापस buf;
}

अक्षर* format_timestamp_relative_full(अक्षर *buf, माप_प्रकार l, usec_t t, clockid_t घड़ी, bool implicit_बायाँ) {
        स्थिर अक्षर *s;
        usec_t n, d;

        निश्चित(buf);

        अगर (!timestamp_is_set(t))
                वापस शून्य;

        n = now(घड़ी);
        अगर (n > t) {
                d = n - t;
                s = " ago";
        } अन्यथा {
                d = t - n;
                s = implicit_बायाँ ? "" : " left";
        }

        अगर (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (व्योम) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } अन्यथा अगर (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (व्योम) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } अन्यथा अगर (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (व्योम) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } अन्यथा अगर (d >= 2*USEC_PER_DAY)
                (व्योम) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        अन्यथा अगर (d >= 25*USEC_PER_HOUR)
                (व्योम) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        अन्यथा अगर (d >= 6*USEC_PER_HOUR)
                (व्योम) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        अन्यथा अगर (d >= USEC_PER_HOUR)
                (व्योम) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        अन्यथा अगर (d >= 5*USEC_PER_MINUTE)
                (व्योम) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        अन्यथा अगर (d >= USEC_PER_MINUTE)
                (व्योम) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        अन्यथा अगर (d >= USEC_PER_SEC)
                (व्योम) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        अन्यथा अगर (d >= USEC_PER_MSEC)
                (व्योम) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        अन्यथा अगर (d > 0)
                (व्योम) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        अन्यथा
                (व्योम) snprintf(buf, l, "now");

        buf[l-1] = 0;
        वापस buf;
}

अक्षर* format_timespan(अक्षर *buf, माप_प्रकार l, usec_t t, usec_t accuracy) {
        अटल स्थिर काष्ठा {
                स्थिर अक्षर *suffix;
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

        अक्षर *p = ASSERT_PTR(buf);
        bool something = false;

        निश्चित(l > 0);

        अगर (t == USEC_INFINITY) {
                म_नकलन(p, "infinity", l-1);
                p[l-1] = 0;
                वापस p;
        }

        अगर (t <= 0) {
                म_नकलन(p, "0", l-1);
                p[l-1] = 0;
                वापस p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                पूर्णांक k = 0;
                माप_प्रकार n;
                bool done = false;
                usec_t a, b;

                अगर (t <= 0)
                        अवरोध;

                अगर (t < accuracy && something)
                        अवरोध;

                अगर (t < i->usec)
                        जारी;

                अगर (l <= 1)
                        अवरोध;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                अगर (t < USEC_PER_MINUTE && b > 0) {
                        चिन्हित अक्षर j = 0;

                        क्रम (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        क्रम (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        अगर (j > 0) {
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
                अगर (!done) {
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

        वापस buf;
}

स्थिर अक्षर* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        अगर (dst && isempty(tzname[1]))
                dst = false;

        वापस empty_to_null(tzname[dst]);
}

पूर्णांक parse_gmtoff(स्थिर अक्षर *t, दीर्घ *ret) {
        निश्चित(t);

        काष्ठा पंचांग पंचांग;
        स्थिर अक्षर *k = strptime(t, "%z", &पंचांग);
        अगर (k && *k == '\0') {
                /* Success! */
                अगर (ret)
                        *ret = पंचांग.tm_gmtoff;
                वापस 0;
        }

#अगर_घोषित __GLIBC__
        वापस -EINVAL;
#अन्यथा
        पूर्णांक r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        चयन (*t) {
        हाल '+':
                positive = true;
                अवरोध;
        हाल '-':
                positive = false;
                अवरोध;
        शेष:
                वापस -EINVAL;
        }

        t++;
        r = undecchar(*t);
        अगर (r < 0)
                वापस r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        अगर (r < 0)
                वापस r;
        u += r * USEC_PER_HOUR;

        t++;
        अगर (*t == '\0') /* 2 digits case */
                जाओ finalize;

        अगर (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        अगर (r < 0)
                वापस r;
        अगर (r >= 6) /* refuse minutes equal to or larger than 60 */
                वापस -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        अगर (r < 0)
                वापस r;

        u += r * USEC_PER_MINUTE;

        t++;
        अगर (*t != '\0')
                वापस -EINVAL;

finalize:
        अगर (u > USEC_PER_DAY) /* refuse larger than one day */
                वापस -EINVAL;

        अगर (ret) {
                दीर्घ gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        वापस 0;
#पूर्ण_अगर
}

अटल पूर्णांक parse_timestamp_impl(
                स्थिर अक्षर *t,
                माप_प्रकार max_len,
                bool utc,
                पूर्णांक isdst,
                दीर्घ gmtoff,
                usec_t *ret) {

        अटल स्थिर काष्ठा {
                स्थिर अक्षर *name;
                स्थिर पूर्णांक nr;
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

        _cleanup_free_ अक्षर *t_alloc = शून्य;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        पूर्णांक r, weekday = -1;
        अचिन्हित fractional = 0;
        स्थिर अक्षर *k;
        काष्ठा पंचांग पंचांग, copy;

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

        निश्चित(t);

        अगर (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                अगर (max_len == 0) /* Can't be the only field */
                        वापस -EINVAL;

                t_alloc = strndup(t, max_len);
                अगर (!t_alloc)
                        वापस -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        अगर (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                अगर ((usec_t) द_असल(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        वापस -EINVAL;
        } अन्यथा {
                अगर (gmtoff != 0)
                        वापस -EINVAL;
        }

        अगर (t[0] == '@' && !with_tz)
                वापस parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        अगर (!with_tz) {
                अगर (streq(t, "now"))
                        जाओ finish;

                अगर (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        अगर (r < 0)
                                वापस r;

                        जाओ finish;
                }

                अगर (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        अगर (r < 0)
                                वापस r;

                        जाओ finish;
                }

                अगर ((k = endswith(t, " ago"))) {
                        _cleanup_free_ अक्षर *buf = शून्य;

                        buf = strndup(t, k - t);
                        अगर (!buf)
                                वापस -ENOMEM;

                        r = parse_sec(buf, &minus);
                        अगर (r < 0)
                                वापस r;

                        जाओ finish;
                }

                अगर ((k = endswith(t, " left"))) {
                        _cleanup_free_ अक्षर *buf = शून्य;

                        buf = strndup(t, k - t);
                        अगर (!buf)
                                वापस -ENOMEM;

                        r = parse_sec(buf, &plus);
                        अगर (r < 0)
                                वापस r;

                        जाओ finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &पंचांग);
        अगर (r < 0)
                वापस r;

        पंचांग.tm_isdst = isdst;

        अगर (streq(t, "today")) {
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                जाओ from_tm;

        } अन्यथा अगर (streq(t, "yesterday")) {
                पंचांग.tm_mday--;
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                जाओ from_tm;

        } अन्यथा अगर (streq(t, "tomorrow")) {
                पंचांग.tm_mday++;
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                जाओ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                अगर (!k || *k != ' ')
                        जारी;

#अगर_घोषित __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#पूर्ण_अगर
                t = k + 1;
                अवरोध;
        }

        copy = पंचांग;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &पंचांग);
        अगर (k) {
                अगर (*k == '.')
                        जाओ parse_usec;
                अन्यथा अगर (*k == 0)
                        जाओ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        पंचांग = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &पंचांग);
        अगर (k) {
                अगर (*k == '.')
                        जाओ parse_usec;
                अन्यथा अगर (*k == 0)
                        जाओ from_tm;
        }

        /* RFC3339 syntax */
        पंचांग = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &पंचांग);
        अगर (k) {
                अगर (*k == '.')
                        जाओ parse_usec;
                अन्यथा अगर (*k == 0)
                        जाओ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        पंचांग = copy;
        k = strptime(t, "%b %d %H:%M:%S", &पंचांग);
        अगर (k) {
                अगर (*k == '.')
                        जाओ parse_usec;
                अन्यथा अगर (*k == 0)
                        जाओ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &पंचांग);
        अगर (k && *k == 0) {
                पंचांग.tm_sec = 0;
                जाओ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        पंचांग = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &पंचांग);
        अगर (k && *k == 0) {
                पंचांग.tm_sec = 0;
                जाओ from_tm;
        }

        /* RFC3339 syntax without seconds */
        पंचांग = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &पंचांग);
        अगर (k && *k == 0) {
                पंचांग.tm_sec = 0;
                जाओ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%y-%m-%d", &पंचांग);
        अगर (k && *k == 0) {
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                जाओ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%Y-%m-%d", &पंचांग);
        अगर (k && *k == 0) {
                पंचांग.tm_sec = पंचांग.tm_min = पंचांग.tm_hour = 0;
                जाओ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%H:%M:%S", &पंचांग);
        अगर (k) {
                अगर (*k == '.')
                        जाओ parse_usec;
                अन्यथा अगर (*k == 0)
                        जाओ from_tm;
        }

        पंचांग = copy;
        k = strptime(t, "%H:%M", &पंचांग);
        अगर (k && *k == 0) {
                पंचांग.tm_sec = 0;
                जाओ from_tm;
        }

        वापस -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        अगर (r < 0)
                वापस -EINVAL;
        अगर (*k != '\0')
                वापस -EINVAL;

from_tm:
        निश्चित(plus == 0);
        निश्चित(minus == 0);

        अगर (weekday >= 0 && पंचांग.tm_wday != weekday)
                वापस -EINVAL;

        अगर (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                अगर (पंचांग.tm_year == 69 && पंचांग.tm_mon == 11 && पंचांग.tm_mday == 31) {
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
        अगर (r < 0)
                वापस r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        अगर (usec < minus)
                वापस -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        अगर (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                वापस -EINVAL;

        अगर (ret)
                *ret = usec;
        वापस 0;
}

पूर्णांक parse_timestamp(स्थिर अक्षर *t, usec_t *ret) {
        दीर्घ gmtoff;
        पूर्णांक r;

        निश्चित(t);

        माप_प्रकार t_len = म_माप(t);
        अगर (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                अगर (r >= 0)
                        वापस r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        अगर (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                वापस parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        स्थिर अक्षर *tz = म_खोजप(t, ' ');
        अगर (!tz)
                वापस parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        माप_प्रकार max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        अगर (streq(tz, "UTC"))
                वापस parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        अगर (parse_gmtoff(tz, &gmtoff) >= 0)
                वापस parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        क्रम (पूर्णांक j = 0; j <= 1; j++) {
                अगर (!streq_ptr(tz, get_tzname(j)))
                        जारी;

                /* The specified timezone matches tzname[] of the local timezone. */
                वापस parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        अगर (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                अगर (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        वापस negative_errno();

                वापस parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        वापस parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

अटल स्थिर अक्षर* extract_multiplier(स्थिर अक्षर *p, usec_t *ret) {
        अटल स्थिर काष्ठा {
                स्थिर अक्षर *suffix;
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

        निश्चित(p);
        निश्चित(ret);

        FOREACH_ELEMENT(i, table) {
                स्थिर अक्षर *e = startswith(p, i->suffix);
                अगर (e) {
                        *ret = i->usec;
                        वापस e;
                }
        }

        वापस p;
}

पूर्णांक parse_time(स्थिर अक्षर *t, usec_t *ret, usec_t default_unit) {
        स्थिर अक्षर *p, *s;

        निश्चित(t);
        निश्चित(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ शून्य);
        s = startswith(p, "infinity");
        अगर (s) {
                अगर (!in_charset(s, WHITESPACE))
                        वापस -EINVAL;

                अगर (ret)
                        *ret = USEC_INFINITY;
                वापस 0;
        }

        usec_t usec = 0;

        क्रम (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                दीर्घ दीर्घ l;
                अक्षर *e;

                p = skip_leading_chars(p, /* bad= */ शून्य);
                अगर (*p == 0) {
                        अगर (!something)
                                वापस -EINVAL;

                        अवरोध;
                }

                अगर (*p == '-') /* Don't allow "-0" */
                        वापस -दुस्फल;

                त्रुटि_सं = 0;
                l = strtoll(p, &e, 10);
                अगर (त्रुटि_सं > 0)
                        वापस -त्रुटि_सं;
                अगर (l < 0)
                        वापस -दुस्फल;

                अगर (*e == '.') {
                        p = e + 1;
                        p += म_अखोज(p, DIGITS);
                } अन्यथा अगर (e == p)
                        वापस -EINVAL;
                अन्यथा
                        p = e;

                s = extract_multiplier(p + म_अखोज(p, WHITESPACE), &multiplier);
                अगर (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        वापस -EINVAL;

                p = s;

                अगर ((usec_t) l >= USEC_INFINITY / multiplier)
                        वापस -दुस्फल;

                k = (usec_t) l * multiplier;
                अगर (k >= USEC_INFINITY - usec)
                        वापस -दुस्फल;

                usec += k;

                something = true;

                अगर (*e == '.') {
                        usec_t m = multiplier / 10;
                        स्थिर अक्षर *b;

                        क्रम (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                अगर (k >= USEC_INFINITY - usec)
                                        वापस -दुस्फल;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        अगर (b == e + 1)
                                वापस -EINVAL;
                }
        }

        अगर (ret)
                *ret = usec;
        वापस 0;
}

पूर्णांक parse_sec(स्थिर अक्षर *t, usec_t *ret) {
        वापस parse_time(t, ret, USEC_PER_SEC);
}

पूर्णांक parse_sec_fix_0(स्थिर अक्षर *t, usec_t *ret) {
        usec_t k;
        पूर्णांक r;

        निश्चित(t);
        निश्चित(ret);

        r = parse_sec(t, &k);
        अगर (r < 0)
                वापस r;

        *ret = k == 0 ? USEC_INFINITY : k;
        वापस r;
}

पूर्णांक parse_sec_def_infinity(स्थिर अक्षर *t, usec_t *ret) {
        निश्चित(t);
        निश्चित(ret);

        t += म_अखोज(t, WHITESPACE);
        अगर (isempty(t)) {
                *ret = USEC_INFINITY;
                वापस 0;
        }
        वापस parse_sec(t, ret);
}

अटल स्थिर अक्षर* extract_nsec_multiplier(स्थिर अक्षर *p, nsec_t *ret) {
        अटल स्थिर काष्ठा {
                स्थिर अक्षर *suffix;
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

        निश्चित(p);
        निश्चित(ret);

        FOREACH_ELEMENT(i, table) {
                स्थिर अक्षर *e = startswith(p, i->suffix);
                अगर (e) {
                        *ret = i->nsec;
                        वापस e;
                }
        }

        वापस p;
}

पूर्णांक parse_nsec(स्थिर अक्षर *t, nsec_t *ret) {
        स्थिर अक्षर *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        निश्चित(t);
        निश्चित(ret);

        p = t;

        p += म_अखोज(p, WHITESPACE);
        s = startswith(p, "infinity");
        अगर (s) {
                s += म_अखोज(s, WHITESPACE);
                अगर (*s != 0)
                        वापस -EINVAL;

                *ret = NSEC_INFINITY;
                वापस 0;
        }

        क्रम (;;) {
                nsec_t multiplier = 1, k;
                दीर्घ दीर्घ l;
                अक्षर *e;

                p += म_अखोज(p, WHITESPACE);

                अगर (*p == 0) {
                        अगर (!something)
                                वापस -EINVAL;

                        अवरोध;
                }

                अगर (*p == '-') /* Don't allow "-0" */
                        वापस -दुस्फल;

                त्रुटि_सं = 0;
                l = strtoll(p, &e, 10);
                अगर (त्रुटि_सं > 0)
                        वापस -त्रुटि_सं;
                अगर (l < 0)
                        वापस -दुस्फल;

                अगर (*e == '.') {
                        p = e + 1;
                        p += म_अखोज(p, DIGITS);
                } अन्यथा अगर (e == p)
                        वापस -EINVAL;
                अन्यथा
                        p = e;

                s = extract_nsec_multiplier(p + म_अखोज(p, WHITESPACE), &multiplier);
                अगर (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        वापस -EINVAL;

                p = s;

                अगर ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        वापस -दुस्फल;

                k = (nsec_t) l * multiplier;
                अगर (k >= NSEC_INFINITY - nsec)
                        वापस -दुस्फल;

                nsec += k;

                something = true;

                अगर (*e == '.') {
                        nsec_t m = multiplier / 10;
                        स्थिर अक्षर *b;

                        क्रम (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                अगर (k >= NSEC_INFINITY - nsec)
                                        वापस -दुस्फल;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        अगर (b == e + 1)
                                वापस -EINVAL;
                }
        }

        *ret = nsec;

        वापस 0;
}

अटल पूर्णांक get_timezones_from_zone1970_tab(अक्षर ***ret) {
        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_strv_free_ अक्षर **zones = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        f = ख_खोलो("/usr/share/zoneinfo/zone1970.tab", "re");
        अगर (!f)
                वापस -त्रुटि_सं;

        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य, *cc = शून्य, *co = शून्य, *tz = शून्य;

                r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                स्थिर अक्षर *p = पंक्ति;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_शब्दगण(&p, शून्य, 0, &cc, &co, &tz);
                अगर (r < 0)
                        जारी;

                /* Lines that start with # are comments. */
                अगर (*cc == '#')
                        जारी;

                अगर (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        जारी;

                r = strv_extend(&zones, tz);
                अगर (r < 0)
                        वापस r;
        }

        *ret = TAKE_PTR(zones);
        वापस 0;
}

अटल पूर्णांक get_timezones_from_tzdata_zi(अक्षर ***ret) {
        _cleanup_fclose_ खाता *f = शून्य;
        _cleanup_strv_free_ अक्षर **zones = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        f = ख_खोलो("/usr/share/zoneinfo/tzdata.zi", "re");
        अगर (!f)
                वापस -त्रुटि_सं;

        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य, *type = शून्य, *f1 = शून्य, *f2 = शून्य;

                r = read_पंक्ति(f, LONG_पंक्ति_MAX, &पंक्ति);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                स्थिर अक्षर *p = पंक्ति;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_शब्दगण(&p, शून्य, 0, &type, &f1, &f2);
                अगर (r < 0)
                        जारी;

                अक्षर *tz;
                अगर (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                अन्यथा अगर (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                अन्यथा
                        /* Not a line we care about. */
                        जारी;

                अगर (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        जारी;

                r = strv_extend(&zones, tz);
                अगर (r < 0)
                        वापस r;
        }

        *ret = TAKE_PTR(zones);
        वापस 0;
}

पूर्णांक get_timezones(अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **zones = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        अगर (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                अगर (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        अगर (r < 0 && r != -ENOENT)
                वापस r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        अगर (r < 0)
                वापस r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        वापस 0;
}

पूर्णांक verify_timezone(स्थिर अक्षर *name, पूर्णांक log_level) {
        bool slash = false;
        स्थिर अक्षर *p, *t;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        अक्षर buf[4];
        पूर्णांक r;

        अगर (isempty(name))
                वापस -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        अगर (streq(name, "UTC"))
                वापस 0;

        अगर (name[0] == '/')
                वापस -EINVAL;

        क्रम (p = name; *p; p++) {
                अगर (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        वापस -EINVAL;

                अगर (*p == '/') {

                        अगर (slash)
                                वापस -EINVAL;

                        slash = true;
                } अन्यथा
                        slash = false;
        }

        अगर (slash)
                वापस -EINVAL;

        अगर (p - name >= PATH_MAX)
                वापस -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = खोलो(t, O_RDONLY|O_CLOEXEC);
        अगर (fd < 0)
                वापस log_full_errno(log_level, त्रुटि_सं, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        अगर (r < 0)
                वापस log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        अगर (r < 0)
                वापस log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        अगर (स_भेद(buf, "TZif", 4) != 0)
                वापस log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        वापस 0;
}

व्योम reset_timezonep(अक्षर **p) {
        निश्चित(p);

        (व्योम) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

अक्षर* save_timezone(व्योम) {
        स्थिर अक्षर *e = दो_पर्या("TZ");
        अगर (!e)
                वापस शून्य;

        अक्षर *s = strdup(e);
        अगर (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        वापस s;
}

bool clock_supported(clockid_t घड़ी) {
        काष्ठा timespec ts;

        चयन (घड़ी) {

        हाल CLOCK_MONOTONIC:
        हाल CLOCK_REALTIME:
        हाल CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                वापस true;

        शेष:
                /* For everything else, check properly */
                वापस clock_gettime(घड़ी, &ts) >= 0;
        }
}

पूर्णांक get_timezone(अक्षर **ret) {
        _cleanup_free_ अक्षर *t = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = readlink_malloc(etc_localtime(), &t);
        अगर (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                वापस strdup_to(ret, "UTC");
        अगर (r < 0)
                वापस r; /* Return EINVAL if not a symlink */

        स्थिर अक्षर *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        अगर (!e)
                वापस -EINVAL;
        अगर (!timezone_is_valid(e, LOG_DEBUG))
                वापस -EINVAL;

        वापस strdup_to(ret, e);
}

पूर्णांक get_timezone_prefer_env(अक्षर **ret) {
        निश्चित(ret);

        स्थिर अक्षर *e = दो_पर्या("TZ");
        अगर (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                वापस strdup_to(ret, e + 1);

        वापस get_timezone(ret);
}

स्थिर अक्षर* etc_localtime(व्योम) {
        अटल स्थिर अक्षर *cached = शून्य;

        अगर (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        वापस cached;
}

पूर्णांक mktime_or_timegm_usec(
                काष्ठा पंचांग *पंचांग, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        समय_प्रकार t;

        निश्चित(पंचांग);

        अगर (पंचांग->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                वापस -दुस्फल;
        अगर ((usec_t) पंचांग->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                वापस -दुस्फल;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(पंचांग) : स_गढ़ो(पंचांग);
        अगर (t < 0) /* Refuse negative times and errors */
                वापस -दुस्फल;
        अगर ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                वापस -दुस्फल;

        अगर (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        वापस 0;
}

पूर्णांक localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                काष्ठा पंचांग *ret) {

        t /= USEC_PER_SEC; /* Round down */
        अगर (t > (usec_t) TIME_T_MAX)
                वापस -दुस्फल;
        समय_प्रकार sec = (समय_प्रकार) t;

        काष्ठा पंचांग buf = {};
        अगर (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                वापस -EINVAL;

        अगर (ret)
                *ret = buf;

        वापस 0;
}

uint64_t sysconf_clock_ticks_cached(व्योम) {
        अटल thread_local uint64_t hz = 0;

        अगर (hz != 0)
                वापस hz;

        दीर्घ t = sysconf(_SC_CLK_TCK);
        निश्चित(t > 0);
        hz = (uint64_t) t;

        वापस hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        वापस DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        वापस DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        अगर (x == USEC_INFINITY)
                वापस USEC_INFINITY;
        अगर (map_clock_id(from) == map_clock_id(to))
                वापस x;

        a = now(from);
        b = now(to);

        अगर (x > a)
                /* x lies in the future */
                वापस usec_add(b, usec_sub_unsigned(x, a));
        अन्यथा
                /* x lies in the past */
                वापस usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(व्योम) {
        tzset();

        वापस timezone == 0 && daylight == 0;
}

पूर्णांक usleep_safe(usec_t usec) {
        पूर्णांक r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        अगर (usec == 0)
                वापस 0;

        अगर (usec == USEC_INFINITY)
                वापस RET_NERRNO(छोड़ो());

        काष्ठा timespec t;
        timespec_store(&t, usec);

        क्रम (;;) {
                काष्ठा timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                अगर (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        जारी;
                }

                वापस r;
        }
}

पूर्णांक time_change_fd(व्योम) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        अटल स्थिर काष्ठा itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ पूर्णांक fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        अगर (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, शून्य) >= 0)
                वापस TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#अगर SIZEOF_TIME_T == 8 && अच_दीर्घ_उच्च < UINT64_MAX
        अगर (ERRNO_IS_NOT_SUPPORTED(त्रुटि_सं) || त्रुटि_सं == EOVERFLOW) {
                अटल स्थिर काष्ठा itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                अगर (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, शून्य) >= 0)
                        वापस TAKE_FD(fd);
        }
#पूर्ण_अगर

        वापस -त्रुटि_सं;
}

अटल स्थिर अक्षर* स्थिर timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(स्थिर अक्षर *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        अगर (t >= 0)
                वापस t;
        अगर (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                वापस TIMESTAMP_US;
        अगर (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                वापस TIMESTAMP_US_UTC;
        वापस t;
}

पूर्णांक parse_calendar_date_full(स्थिर अक्षर *s, bool allow_pre_epoch, usec_t *ret_usec, काष्ठा पंचांग *ret_tm) {
        काष्ठा पंचांग parsed_tm = {}, copy_tm;
        स्थिर अक्षर *k;
        पूर्णांक r;

        निश्चित(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        अगर (!k || *k)
                वापस -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        अगर (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                अगर (timegm(&copy_tm) == (समय_प्रकार) -1 && copy_tm.tm_wday == -1)
                        वापस -EINVAL;
        } अन्यथा {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                अगर (r < 0)
                        वापस r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        अगर (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                वापस -EINVAL;

        अगर (ret_usec)
                *ret_usec = usec;
        अगर (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        वापस 0;
}
