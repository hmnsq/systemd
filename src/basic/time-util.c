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

        usec_t n = timespec_load(&ts);

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

        nsec_t n = timespec_load_nsec(&ts);

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

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        जइ (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                जइ (to_base >= USEC_INFINITY - delta) /* overflow? */
                        प्रत्यागम USEC_INFINITY;

                प्रत्यागम to_base + delta;

        } अण्णहा { /* In the past */
                usec_t delta = from_base - from;

                जइ (to_base <= delta) /* underflow? */
                        प्रत्यागम 0;

                प्रत्यागम to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        जइ (map_clock_id(from_clock) == map_clock_id(to_clock))
                प्रत्यागम from;

        /* Keep infinity as is */
        जइ (from == USEC_INFINITY)
                प्रत्यागम from;

        प्रत्यागम map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        प्रतिज्ञा(ts);

        जइ (!timestamp_is_set(u)) {
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

        जइ (!timestamp_is_set(u)) {
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

        जइ (u == USEC_INFINITY) {
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

        जइ (u == USEC_INFINITY) {
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

        जइ (u == USEC_INFINITY) {
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

usec_t timespec_load(ध्रुव संरचना timespec *ts) {
        प्रतिज्ञा(ts);

        जइ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                प्रत्यागम USEC_INFINITY;

        जइ ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                प्रत्यागम USEC_INFINITY;

        प्रत्यागम
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(ध्रुव संरचना timespec *ts) {
        प्रतिज्ञा(ts);

        जइ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                प्रत्यागम NSEC_INFINITY;

        जइ ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                प्रत्यागम NSEC_INFINITY;

        प्रत्यागम (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

संरचना timespec *timespec_store(संरचना timespec *ts, usec_t u) {
        प्रतिज्ञा(ts);

        जइ (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                प्रत्यागम ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        प्रत्यागम ts;
}

संरचना timespec *timespec_store_nsec(संरचना timespec *ts, nsec_t n) {
        प्रतिज्ञा(ts);

        जइ (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                प्रत्यागम ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        प्रत्यागम ts;
}

संरचना __kernel_timespec *kernel_timespec_store(संरचना __kernel_timespec *ts, usec_t u) {
        प्रतिज्ञा(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        प्रतिज्ञा(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        प्रत्यागम ts;
}

usec_t timeval_load(ध्रुव संरचना timeval *tv) {
        प्रतिज्ञा(tv);

        जइ (tv->tv_sec < 0 || tv->tv_usec < 0)
                प्रत्यागम USEC_INFINITY;

        जइ ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                प्रत्यागम USEC_INFINITY;

        प्रत्यागम
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

संरचना timeval *timeval_store(संरचना timeval *tv, usec_t u) {
        प्रतिज्ञा(tv);

        जइ (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } अण्णहा {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        प्रत्यागम tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
स्थिर ध्रुव वर्ण *ध्रुव weekday_table[_WEEKDAY_MAX] = {
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
                size_t l,
                usec_t t,
                TimestampStyle style) {
        संरचना tm tm;
        bool utc, us;
        size_t n;

        प्रतिज्ञा(buf);
        प्रतिज्ञा(style >= 0);
        प्रतिज्ञा(style < _TIMESTAMP_STYLE_MAX);

        जइ (!timestamp_is_set(t))
                प्रत्यागम रिक्त; /* Timestamp is unset */

        जइ (style == TIMESTAMP_UNIX) {
                जइ (l < (size_t) (1 + 1 + 1))
                        प्रत्यागम रिक्त; /* not enough space for even the shortest of forms */

                प्रत्यागम snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        जइ (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                प्रत्यागम रिक्त; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        जइ (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                स्थिर ध्रुव वर्ण* ध्रुव xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                प्रतिज्ञा(l >= strlen(xxx[style]) + 1);
                प्रत्यागम strcpy(buf, xxx[style]);
        }

        जइ (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                प्रत्यागम रिक्त;

        /* Start with the week day */
        ध्रुव वर्ण *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        प्रतिज्ञा(weekday);
        memcpy(buf, weekday, 4);

        जइ (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                जइ (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        प्रत्यागम रिक्त; /* Doesn't fit */

                प्रत्यागम buf;
        }

        /* Add the main components */
        जइ (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                प्रत्यागम रिक्त; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        जइ (us) {
                n = strlen(buf);
                जइ (n + 8 > l)
                        प्रत्यागम रिक्त; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        जइ (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                जइ (n + 5 > l)
                        प्रत्यागम रिक्त; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } अण्णहा जइ (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                जइ (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        जइ (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                प्रत्यागम रिक्त; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } अण्णहा {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        प्रत्यागम buf;
}

वर्ण* format_timestamp_relative_full(वर्ण *buf, size_t l, usec_t t, clockid_t clock, bool implicit_वामः) {
        ध्रुव वर्ण *s;
        usec_t n, d;

        प्रतिज्ञा(buf);

        जइ (!timestamp_is_set(t))
                प्रत्यागम रिक्त;

        n = now(clock);
        जइ (n > t) {
                d = n - t;
                s = " ago";
        } अण्णहा {
                d = t - n;
                s = implicit_वामः ? "" : " left";
        }

        जइ (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } अण्णहा जइ (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } अण्णहा जइ (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (शून्य) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } अण्णहा जइ (d >= 2*USEC_PER_DAY)
                (शून्य) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        अण्णहा जइ (d >= 25*USEC_PER_HOUR)
                (शून्य) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        अण्णहा जइ (d >= 6*USEC_PER_HOUR)
                (शून्य) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        अण्णहा जइ (d >= USEC_PER_HOUR)
                (शून्य) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        अण्णहा जइ (d >= 5*USEC_PER_MINUTE)
                (शून्य) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        अण्णहा जइ (d >= USEC_PER_MINUTE)
                (शून्य) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        अण्णहा जइ (d >= USEC_PER_SEC)
                (शून्य) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        अण्णहा जइ (d >= USEC_PER_MSEC)
                (शून्य) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        अण्णहा जइ (d > 0)
                (शून्य) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        अण्णहा
                (शून्य) snprintf(buf, l, "now");

        buf[l-1] = 0;
        प्रत्यागम buf;
}

वर्ण* format_timespan(वर्ण *buf, size_t l, usec_t t, usec_t accuracy) {
        स्थिर ध्रुव संरचना {
                ध्रुव वर्ण *suffix;
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

        जइ (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                प्रत्यागम p;
        }

        जइ (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                प्रत्यागम p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                पूर्णाङ्क k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                जइ (t <= 0)
                        भङ्ग;

                जइ (t < accuracy && something)
                        भङ्ग;

                जइ (t < i->usec)
                        अनुवर्त;

                जइ (l <= 1)
                        भङ्ग;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                जइ (t < USEC_PER_MINUTE && b > 0) {
                        signed वर्ण j = 0;

                        प्रति (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        प्रति (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        जइ (j > 0) {
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
                जइ (!done) {
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
        जइ (dst && isempty(tzname[1]))
                dst = false;

        प्रत्यागम empty_to_null(tzname[dst]);
}

पूर्णाङ्क parse_gmtoff(ध्रुव वर्ण *t, long *ret) {
        प्रतिज्ञा(t);

        संरचना tm tm;
        ध्रुव वर्ण *k = strptime(t, "%z", &tm);
        जइ (k && *k == '\0') {
                /* Success! */
                जइ (ret)
                        *ret = tm.tm_gmtoff;
                प्रत्यागम 0;
        }

#ifdef __GLIBC__
        प्रत्यागम -EINVAL;
#अण्णहा
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
                भङ्ग;
        पक्ष '-':
                positive = false;
                भङ्ग;
        स्वभाव:
                प्रत्यागम -EINVAL;
        }

        t++;
        r = undecchar(*t);
        जइ (r < 0)
                प्रत्यागम r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        जइ (r < 0)
                प्रत्यागम r;
        u += r * USEC_PER_HOUR;

        t++;
        जइ (*t == '\0') /* 2 digits case */
                गच्छ finalize;

        जइ (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        जइ (r < 0)
                प्रत्यागम r;
        जइ (r >= 6) /* refuse minutes equal to or larger than 60 */
                प्रत्यागम -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        जइ (r < 0)
                प्रत्यागम r;

        u += r * USEC_PER_MINUTE;

        t++;
        जइ (*t != '\0')
                प्रत्यागम -EINVAL;

finalize:
        जइ (u > USEC_PER_DAY) /* refuse larger than one day */
                प्रत्यागम -EINVAL;

        जइ (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        प्रत्यागम 0;
#endif
}

स्थिर पूर्णाङ्क parse_timestamp_impl(
                ध्रुव वर्ण *t,
                size_t max_len,
                bool utc,
                पूर्णाङ्क isdst,
                long gmtoff,
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
        unsigned fractional = 0;
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

        जइ (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                जइ (max_len == 0) /* Can't be the only field */
                        प्रत्यागम -EINVAL;

                t_alloc = strndup(t, max_len);
                जइ (!t_alloc)
                        प्रत्यागम -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        जइ (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                जइ ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        प्रत्यागम -EINVAL;
        } अण्णहा {
                जइ (gmtoff != 0)
                        प्रत्यागम -EINVAL;
        }

        जइ (t[0] == '@' && !with_tz)
                प्रत्यागम parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        जइ (!with_tz) {
                जइ (streq(t, "now"))
                        गच्छ finish;

                जइ (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        जइ (r < 0)
                                प्रत्यागम r;

                        गच्छ finish;
                }

                जइ (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        जइ (r < 0)
                                प्रत्यागम r;

                        गच्छ finish;
                }

                जइ ((k = endswith(t, " ago"))) {
                        _cleanup_free_ वर्ण *buf = रिक्त;

                        buf = strndup(t, k - t);
                        जइ (!buf)
                                प्रत्यागम -ENOMEM;

                        r = parse_sec(buf, &minus);
                        जइ (r < 0)
                                प्रत्यागम r;

                        गच्छ finish;
                }

                जइ ((k = endswith(t, " left"))) {
                        _cleanup_free_ वर्ण *buf = रिक्त;

                        buf = strndup(t, k - t);
                        जइ (!buf)
                                प्रत्यागम -ENOMEM;

                        r = parse_sec(buf, &plus);
                        जइ (r < 0)
                                प्रत्यागम r;

                        गच्छ finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        जइ (r < 0)
                प्रत्यागम r;

        tm.tm_isdst = isdst;

        जइ (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;

        } अण्णहा जइ (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;

        } अण्णहा जइ (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                जइ (!k || *k != ' ')
                        अनुवर्त;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                भङ्ग;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        जइ (k) {
                जइ (*k == '.')
                        गच्छ parse_usec;
                अण्णहा जइ (*k == 0)
                        गच्छ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        जइ (k) {
                जइ (*k == '.')
                        गच्छ parse_usec;
                अण्णहा जइ (*k == 0)
                        गच्छ from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        जइ (k) {
                जइ (*k == '.')
                        गच्छ parse_usec;
                अण्णहा जइ (*k == 0)
                        गच्छ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        जइ (k) {
                जइ (*k == '.')
                        गच्छ parse_usec;
                अण्णहा जइ (*k == 0)
                        गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        जइ (k && *k == 0) {
                tm.tm_sec = 0;
                गच्छ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        जइ (k && *k == 0) {
                tm.tm_sec = 0;
                गच्छ from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        जइ (k && *k == 0) {
                tm.tm_sec = 0;
                गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        जइ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        जइ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        जइ (k) {
                जइ (*k == '.')
                        गच्छ parse_usec;
                अण्णहा जइ (*k == 0)
                        गच्छ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        जइ (k && *k == 0) {
                tm.tm_sec = 0;
                गच्छ from_tm;
        }

        प्रत्यागम -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        जइ (r < 0)
                प्रत्यागम -EINVAL;
        जइ (*k != '\0')
                प्रत्यागम -EINVAL;

from_tm:
        प्रतिज्ञा(plus == 0);
        प्रतिज्ञा(minus == 0);

        जइ (weekday >= 0 && tm.tm_wday != weekday)
                प्रत्यागम -EINVAL;

        जइ (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                जइ (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } अण्णहा
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        जइ (r < 0)
                प्रत्यागम r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        जइ (usec < minus)
                प्रत्यागम -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        जइ (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                प्रत्यागम -EINVAL;

        जइ (ret)
                *ret = usec;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_timestamp(ध्रुव वर्ण *t, usec_t *ret) {
        long gmtoff;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);

        size_t t_len = strlen(t);
        जइ (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                जइ (r >= 0)
                        प्रत्यागम r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        जइ (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                प्रत्यागम parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ध्रुव वर्ण *tz = strrchr(t, ' ');
        जइ (!tz)
                प्रत्यागम parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        जइ (streq(tz, "UTC"))
                प्रत्यागम parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        जइ (parse_gmtoff(tz, &gmtoff) >= 0)
                प्रत्यागम parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        प्रति (पूर्णाङ्क j = 0; j <= 1; j++) {
                जइ (!streq_ptr(tz, get_tzname(j)))
                        अनुवर्त;

                /* The specified timezone matches tzname[] of the local timezone. */
                प्रत्यागम parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        जइ (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                जइ (setenv("TZ", tz, /* overwrite= */ true) < 0)
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
                ध्रुव वर्ण *e = startswith(p, i->suffix);
                जइ (e) {
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
        जइ (s) {
                जइ (!in_charset(s, WHITESPACE))
                        प्रत्यागम -EINVAL;

                जइ (ret)
                        *ret = USEC_INFINITY;
                प्रत्यागम 0;
        }

        usec_t usec = 0;

        प्रति (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                वर्ण *e;

                p = skip_leading_chars(p, /* bad= */ रिक्त);
                जइ (*p == 0) {
                        जइ (!something)
                                प्रत्यागम -EINVAL;

                        भङ्ग;
                }

                जइ (*p == '-') /* Don't allow "-0" */
                        प्रत्यागम -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                जइ (errno > 0)
                        प्रत्यागम -errno;
                जइ (l < 0)
                        प्रत्यागम -ERANGE;

                जइ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } अण्णहा जइ (e == p)
                        प्रत्यागम -EINVAL;
                अण्णहा
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                जइ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        प्रत्यागम -EINVAL;

                p = s;

                जइ ((usec_t) l >= USEC_INFINITY / multiplier)
                        प्रत्यागम -ERANGE;

                k = (usec_t) l * multiplier;
                जइ (k >= USEC_INFINITY - usec)
                        प्रत्यागम -ERANGE;

                usec += k;

                something = true;

                जइ (*e == '.') {
                        usec_t m = multiplier / 10;
                        ध्रुव वर्ण *b;

                        प्रति (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                जइ (k >= USEC_INFINITY - usec)
                                        प्रत्यागम -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        जइ (b == e + 1)
                                प्रत्यागम -EINVAL;
                }
        }

        जइ (ret)
                *ret = usec;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_sec(ध्रुव वर्ण *t, usec_t *ret) {
        प्रत्यागम parse_time(t, ret, USEC_PER_SEC);
}

पूर्णाङ्क parse_sec_fix_0(ध्रुव वर्ण *t, usec_t *ret) {
        usec_t k;
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        r = parse_sec(t, &k);
        जइ (r < 0)
                प्रत्यागम r;

        *ret = k == 0 ? USEC_INFINITY : k;
        प्रत्यागम r;
}

पूर्णाङ्क parse_sec_def_infinity(ध्रुव वर्ण *t, usec_t *ret) {
        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        t += strspn(t, WHITESPACE);
        जइ (isempty(t)) {
                *ret = USEC_INFINITY;
                प्रत्यागम 0;
        }
        प्रत्यागम parse_sec(t, ret);
}

स्थिर ध्रुव वर्ण* extract_nsec_multiplier(ध्रुव वर्ण *p, nsec_t *ret) {
        स्थिर ध्रुव संरचना {
                ध्रुव वर्ण *suffix;
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
                ध्रुव वर्ण *e = startswith(p, i->suffix);
                जइ (e) {
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
        जइ (s) {
                s += strspn(s, WHITESPACE);
                जइ (*s != 0)
                        प्रत्यागम -EINVAL;

                *ret = NSEC_INFINITY;
                प्रत्यागम 0;
        }

        प्रति (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                वर्ण *e;

                p += strspn(p, WHITESPACE);

                जइ (*p == 0) {
                        जइ (!something)
                                प्रत्यागम -EINVAL;

                        भङ्ग;
                }

                जइ (*p == '-') /* Don't allow "-0" */
                        प्रत्यागम -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                जइ (errno > 0)
                        प्रत्यागम -errno;
                जइ (l < 0)
                        प्रत्यागम -ERANGE;

                जइ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } अण्णहा जइ (e == p)
                        प्रत्यागम -EINVAL;
                अण्णहा
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                जइ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        प्रत्यागम -EINVAL;

                p = s;

                जइ ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        प्रत्यागम -ERANGE;

                k = (nsec_t) l * multiplier;
                जइ (k >= NSEC_INFINITY - nsec)
                        प्रत्यागम -ERANGE;

                nsec += k;

                something = true;

                जइ (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ध्रुव वर्ण *b;

                        प्रति (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                जइ (k >= NSEC_INFINITY - nsec)
                                        प्रत्यागम -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        जइ (b == e + 1)
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
        जइ (!f)
                प्रत्यागम -errno;

        प्रति (;;) {
                _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त, *cc = रिक्त, *co = रिक्त, *tz = रिक्त;

                r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (r == 0)
                        भङ्ग;

                ध्रुव वर्ण *p = पङ्क्तिः;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_शब्दाः(&p, रिक्त, 0, &cc, &co, &tz);
                जइ (r < 0)
                        अनुवर्त;

                /* Lines that start with # are comments. */
                जइ (*cc == '#')
                        अनुवर्त;

                जइ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        अनुवर्त;

                r = strv_extend(&zones, tz);
                जइ (r < 0)
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
        जइ (!f)
                प्रत्यागम -errno;

        प्रति (;;) {
                _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त, *type = रिक्त, *f1 = रिक्त, *f2 = रिक्त;

                r = read_पङ्क्तिः(f, LONG_पङ्क्तिः_MAX, &पङ्क्तिः);
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (r == 0)
                        भङ्ग;

                ध्रुव वर्ण *p = पङ्क्तिः;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_शब्दाः(&p, रिक्त, 0, &type, &f1, &f2);
                जइ (r < 0)
                        अनुवर्त;

                वर्ण *tz;
                जइ (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                अण्णहा जइ (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                अण्णहा
                        /* Not a line we care about. */
                        अनुवर्त;

                जइ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        अनुवर्त;

                r = strv_extend(&zones, tz);
                जइ (r < 0)
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
        जइ (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                जइ (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        जइ (r < 0 && r != -ENOENT)
                प्रत्यागम r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        जइ (r < 0)
                प्रत्यागम r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        प्रत्यागम 0;
}

पूर्णाङ्क verify_timezone(ध्रुव वर्ण *name, पूर्णाङ्क log_level) {
        bool slash = false;
        ध्रुव वर्ण *p, *t;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        वर्ण buf[4];
        पूर्णाङ्क r;

        जइ (isempty(name))
                प्रत्यागम -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        जइ (streq(name, "UTC"))
                प्रत्यागम 0;

        जइ (name[0] == '/')
                प्रत्यागम -EINVAL;

        प्रति (p = name; *p; p++) {
                जइ (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        प्रत्यागम -EINVAL;

                जइ (*p == '/') {

                        जइ (slash)
                                प्रत्यागम -EINVAL;

                        slash = true;
                } अण्णहा
                        slash = false;
        }

        जइ (slash)
                प्रत्यागम -EINVAL;

        जइ (p - name >= PATH_MAX)
                प्रत्यागम -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        जइ (fd < 0)
                प्रत्यागम log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        जइ (r < 0)
                प्रत्यागम log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        जइ (r < 0)
                प्रत्यागम log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        जइ (memcmp(buf, "TZif", 4) != 0)
                प्रत्यागम log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        प्रत्यागम 0;
}

शून्य reset_timezonep(वर्ण **p) {
        प्रतिज्ञा(p);

        (शून्य) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

वर्ण* save_timezone(शून्य) {
        ध्रुव वर्ण *e = getenv("TZ");
        जइ (!e)
                प्रत्यागम रिक्त;

        वर्ण *s = strdup(e);
        जइ (!s)
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
        जइ (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                प्रत्यागम strdup_to(ret, "UTC");
        जइ (r < 0)
                प्रत्यागम r; /* Return EINVAL if not a symlink */

        ध्रुव वर्ण *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        जइ (!e)
                प्रत्यागम -EINVAL;
        जइ (!timezone_is_valid(e, LOG_DEBUG))
                प्रत्यागम -EINVAL;

        प्रत्यागम strdup_to(ret, e);
}

पूर्णाङ्क get_timezone_prefer_env(वर्ण **ret) {
        प्रतिज्ञा(ret);

        ध्रुव वर्ण *e = getenv("TZ");
        जइ (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                प्रत्यागम strdup_to(ret, e + 1);

        प्रत्यागम get_timezone(ret);
}

ध्रुव वर्ण* etc_localtime(शून्य) {
        स्थिर ध्रुव वर्ण *cached = रिक्त;

        जइ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        प्रत्यागम cached;
}

पूर्णाङ्क mktime_or_timegm_usec(
                संरचना tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        प्रतिज्ञा(tm);

        जइ (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                प्रत्यागम -ERANGE;
        जइ ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                प्रत्यागम -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        जइ (t < 0) /* Refuse negative times and errors */
                प्रत्यागम -ERANGE;
        जइ ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                प्रत्यागम -ERANGE;

        जइ (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        प्रत्यागम 0;
}

पूर्णाङ्क localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                संरचना tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        जइ (t > (usec_t) TIME_T_MAX)
                प्रत्यागम -ERANGE;
        time_t sec = (time_t) t;

        संरचना tm buf = {};
        जइ (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                प्रत्यागम -EINVAL;

        जइ (ret)
                *ret = buf;

        प्रत्यागम 0;
}

uint64_t sysconf_clock_ticks_cached(शून्य) {
        स्थिर thread_local uint64_t hz = 0;

        जइ (hz != 0)
                प्रत्यागम hz;

        long t = sysconf(_SC_CLK_TCK);
        प्रतिज्ञा(t > 0);
        hz = (uint64_t) t;

        प्रत्यागम hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        प्रत्यागम DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        प्रत्यागम DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        जइ (x == USEC_INFINITY)
                प्रत्यागम USEC_INFINITY;
        जइ (map_clock_id(from) == map_clock_id(to))
                प्रत्यागम x;

        a = now(from);
        b = now(to);

        जइ (x > a)
                /* x lies in the future */
                प्रत्यागम usec_add(b, usec_sub_unsigned(x, a));
        अण्णहा
                /* x lies in the past */
                प्रत्यागम usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(शून्य) {
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

        जइ (usec == 0)
                प्रत्यागम 0;

        जइ (usec == USEC_INFINITY)
                प्रत्यागम RET_NERRNO(pause());

        संरचना timespec t;
        timespec_store(&t, usec);

        प्रति (;;) {
                संरचना timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                जइ (r == -EINTR) {
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
        जइ (fd < 0)
                प्रत्यागम -errno;

        जइ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, रिक्त) >= 0)
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

#जइ SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        जइ (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                स्थिर ध्रुव संरचना itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                जइ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, रिक्त) >= 0)
                        प्रत्यागम TAKE_FD(fd);
        }
#endif

        प्रत्यागम -errno;
}

स्थिर ध्रुव वर्ण* ध्रुव timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ध्रुव वर्ण *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        जइ (t >= 0)
                प्रत्यागम t;
        जइ (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                प्रत्यागम TIMESTAMP_US;
        जइ (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                प्रत्यागम TIMESTAMP_US_UTC;
        प्रत्यागम t;
}

पूर्णाङ्क parse_calendar_date_full(ध्रुव वर्ण *s, bool allow_pre_epoch, usec_t *ret_usec, संरचना tm *ret_tm) {
        संरचना tm parsed_tm = {}, copy_tm;
        ध्रुव वर्ण *k;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        जइ (!k || *k)
                प्रत्यागम -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        जइ (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                जइ (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        प्रत्यागम -EINVAL;
        } अण्णहा {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                जइ (r < 0)
                        प्रत्यागम r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        जइ (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                प्रत्यागम -EINVAL;

        जइ (ret_usec)
                *ret_usec = usec;
        जइ (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        प्रत्यागम 0;
}
