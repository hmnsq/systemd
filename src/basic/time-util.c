/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/time_types.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/timerfd.h>
#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "hexdecoct.h"          /* IWYU pragma: keep */
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "time-util.h"

நிலையான clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        தேர்வு (c) {

        நிலை CLOCK_BOOTTIME_ALARM:
                திரும்பு CLOCK_BOOTTIME;

        நிலை CLOCK_REALTIME_ALARM:
                திரும்பு CLOCK_REALTIME;

        இயல்பு:
                திரும்பு c;
        }
}

usec_t now(clockid_t clock_id) {
        கட்டமைப்பு timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        ப்ரதிஜ்ஞா(n > 0);
        ப்ரதிஜ்ஞா(n < USEC_INFINITY);

        திரும்பு n;
}

nsec_t now_nsec(clockid_t clock_id) {
        கட்டமைப்பு timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        ப்ரதிஜ்ஞா(n > 0);
        ப்ரதிஜ்ஞா(n < NSEC_INFINITY);

        திரும்பு n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        ப்ரதிஜ்ஞா(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        திரும்பு ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        ப்ரதிஜ்ஞா(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        திரும்பு ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        எனில் (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                எனில் (to_base >= USEC_INFINITY - delta) /* overflow? */
                        திரும்பு USEC_INFINITY;

                திரும்பு to_base + delta;

        } இல்லையெனில் { /* In the past */
                usec_t delta = from_base - from;

                எனில் (to_base <= delta) /* underflow? */
                        திரும்பு 0;

                திரும்பு to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        எனில் (map_clock_id(from_clock) == map_clock_id(to_clock))
                திரும்பு from;

        /* Keep infinity as is */
        எனில் (from == USEC_INFINITY)
                திரும்பு from;

        திரும்பு map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        ப்ரதிஜ்ஞா(ts);

        எனில் (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                திரும்பு ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        திரும்பு ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        ப்ரதிஜ்ஞா(ts);

        எனில் (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                திரும்பு ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        திரும்பு ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        ப்ரதிஜ்ஞா(ts);

        எனில் (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                திரும்பு ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        திரும்பு ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        ப்ரதிஜ்ஞா(ts);

        எனில் (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                திரும்பு ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        திரும்பு ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        ப்ரதிஜ்ஞா(ts);

        எனில் (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                திரும்பு ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        திரும்பு ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        ப்ரதிஜ்ஞா(ts);

        தேர்வு (clock) {

        நிலை CLOCK_REALTIME:
        நிலை CLOCK_REALTIME_ALARM:
                திரும்பு ts->realtime;

        நிலை CLOCK_MONOTONIC:
                திரும்பு ts->monotonic;

        நிலை CLOCK_BOOTTIME:
        நிலை CLOCK_BOOTTIME_ALARM:
                திரும்பு ts->boottime;

        இயல்பு:
                திரும்பு USEC_INFINITY;
        }
}

usec_t timespec_load(மாறிலி கட்டமைப்பு timespec *ts) {
        ப்ரதிஜ்ஞா(ts);

        எனில் (ts->tv_sec < 0 || ts->tv_nsec < 0)
                திரும்பு USEC_INFINITY;

        எனில் ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                திரும்பு USEC_INFINITY;

        திரும்பு
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(மாறிலி கட்டமைப்பு timespec *ts) {
        ப்ரதிஜ்ஞா(ts);

        எனில் (ts->tv_sec < 0 || ts->tv_nsec < 0)
                திரும்பு NSEC_INFINITY;

        எனில் ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                திரும்பு NSEC_INFINITY;

        திரும்பு (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

கட்டமைப்பு timespec *timespec_store(கட்டமைப்பு timespec *ts, usec_t u) {
        ப்ரதிஜ்ஞா(ts);

        எனில் (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                திரும்பு ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        திரும்பு ts;
}

கட்டமைப்பு timespec *timespec_store_nsec(கட்டமைப்பு timespec *ts, nsec_t n) {
        ப்ரதிஜ்ஞா(ts);

        எனில் (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                திரும்பு ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        திரும்பு ts;
}

கட்டமைப்பு __kernel_timespec *kernel_timespec_store(கட்டமைப்பு __kernel_timespec *ts, usec_t u) {
        ப்ரதிஜ்ஞா(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        ப்ரதிஜ்ஞா(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        திரும்பு ts;
}

usec_t timeval_load(மாறிலி கட்டமைப்பு timeval *tv) {
        ப்ரதிஜ்ஞா(tv);

        எனில் (tv->tv_sec < 0 || tv->tv_usec < 0)
                திரும்பு USEC_INFINITY;

        எனில் ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                திரும்பு USEC_INFINITY;

        திரும்பு
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

கட்டமைப்பு timeval *timeval_store(கட்டமைப்பு timeval *tv, usec_t u) {
        ப்ரதிஜ்ஞா(tv);

        எனில் (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } இல்லையெனில் {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        திரும்பு tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
நிலையான மாறிலி எழுத்து *மாறிலி weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, முழு);

எழுத்து* format_timestamp_style(
                எழுத்து *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        கட்டமைப்பு tm tm;
        bool utc, us;
        size_t n;

        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(style >= 0);
        ப்ரதிஜ்ஞா(style < _TIMESTAMP_STYLE_MAX);

        எனில் (!timestamp_is_set(t))
                திரும்பு வெற்று; /* Timestamp is unset */

        எனில் (style == TIMESTAMP_UNIX) {
                எனில் (l < (size_t) (1 + 1 + 1))
                        திரும்பு வெற்று; /* not enough space for even the shortest of forms */

                திரும்பு snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        எனில் (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                திரும்பு வெற்று; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        எனில் (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                நிலையான மாறிலி எழுத்து* மாறிலி xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                ப்ரதிஜ்ஞா(l >= strlen(xxx[style]) + 1);
                திரும்பு strcpy(buf, xxx[style]);
        }

        எனில் (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                திரும்பு வெற்று;

        /* Start with the week day */
        மாறிலி எழுத்து *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        ப்ரதிஜ்ஞா(weekday);
        memcpy(buf, weekday, 4);

        எனில் (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                எனில் (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        திரும்பு வெற்று; /* Doesn't fit */

                திரும்பு buf;
        }

        /* Add the main components */
        எனில் (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                திரும்பு வெற்று; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        எனில் (us) {
                n = strlen(buf);
                எனில் (n + 8 > l)
                        திரும்பு வெற்று; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        எனில் (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                எனில் (n + 5 > l)
                        திரும்பு வெற்று; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } இல்லையெனில் எனில் (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                எனில் (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        எனில் (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                திரும்பு வெற்று; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } இல்லையெனில் {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        திரும்பு buf;
}

எழுத்து* format_timestamp_relative_full(எழுத்து *buf, size_t l, usec_t t, clockid_t clock, bool implicit_இடது) {
        மாறிலி எழுத்து *s;
        usec_t n, d;

        ப்ரதிஜ்ஞா(buf);

        எனில் (!timestamp_is_set(t))
                திரும்பு வெற்று;

        n = now(clock);
        எனில் (n > t) {
                d = n - t;
                s = " ago";
        } இல்லையெனில் {
                d = t - n;
                s = implicit_இடது ? "" : " left";
        }

        எனில் (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (வெறுமை) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } இல்லையெனில் எனில் (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (வெறுமை) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } இல்லையெனில் எனில் (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (வெறுமை) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } இல்லையெனில் எனில் (d >= 2*USEC_PER_DAY)
                (வெறுமை) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        இல்லையெனில் எனில் (d >= 25*USEC_PER_HOUR)
                (வெறுமை) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        இல்லையெனில் எனில் (d >= 6*USEC_PER_HOUR)
                (வெறுமை) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        இல்லையெனில் எனில் (d >= USEC_PER_HOUR)
                (வெறுமை) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        இல்லையெனில் எனில் (d >= 5*USEC_PER_MINUTE)
                (வெறுமை) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        இல்லையெனில் எனில் (d >= USEC_PER_MINUTE)
                (வெறுமை) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        இல்லையெனில் எனில் (d >= USEC_PER_SEC)
                (வெறுமை) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        இல்லையெனில் எனில் (d >= USEC_PER_MSEC)
                (வெறுமை) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        இல்லையெனில் எனில் (d > 0)
                (வெறுமை) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        இல்லையெனில்
                (வெறுமை) snprintf(buf, l, "now");

        buf[l-1] = 0;
        திரும்பு buf;
}

எழுத்து* format_timespan(எழுத்து *buf, size_t l, usec_t t, usec_t accuracy) {
        நிலையான மாறிலி கட்டமைப்பு {
                மாறிலி எழுத்து *suffix;
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

        எழுத்து *p = ASSERT_PTR(buf);
        bool something = false;

        ப்ரதிஜ்ஞா(l > 0);

        எனில் (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                திரும்பு p;
        }

        எனில் (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                திரும்பு p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                முழு k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                எனில் (t <= 0)
                        நிறுத்து;

                எனில் (t < accuracy && something)
                        நிறுத்து;

                எனில் (t < i->usec)
                        தொடர்;

                எனில் (l <= 1)
                        நிறுத்து;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                எனில் (t < USEC_PER_MINUTE && b > 0) {
                        signed எழுத்து j = 0;

                        க்கு (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        க்கு (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        எனில் (j > 0) {
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
                எனில் (!done) {
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

        திரும்பு buf;
}

மாறிலி எழுத்து* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        எனில் (dst && isempty(tzname[1]))
                dst = false;

        திரும்பு empty_to_null(tzname[dst]);
}

முழு parse_gmtoff(மாறிலி எழுத்து *t, long *ret) {
        ப்ரதிஜ்ஞா(t);

        கட்டமைப்பு tm tm;
        மாறிலி எழுத்து *k = strptime(t, "%z", &tm);
        எனில் (k && *k == '\0') {
                /* Success! */
                எனில் (ret)
                        *ret = tm.tm_gmtoff;
                திரும்பு 0;
        }

#ifdef __GLIBC__
        திரும்பு -EINVAL;
#இல்லையெனில்
        முழு r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        தேர்வு (*t) {
        நிலை '+':
                positive = true;
                நிறுத்து;
        நிலை '-':
                positive = false;
                நிறுத்து;
        இயல்பு:
                திரும்பு -EINVAL;
        }

        t++;
        r = undecchar(*t);
        எனில் (r < 0)
                திரும்பு r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        எனில் (r < 0)
                திரும்பு r;
        u += r * USEC_PER_HOUR;

        t++;
        எனில் (*t == '\0') /* 2 digits case */
                செல் finalize;

        எனில் (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r >= 6) /* refuse minutes equal to or larger than 60 */
                திரும்பு -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        எனில் (r < 0)
                திரும்பு r;

        u += r * USEC_PER_MINUTE;

        t++;
        எனில் (*t != '\0')
                திரும்பு -EINVAL;

finalize:
        எனில் (u > USEC_PER_DAY) /* refuse larger than one day */
                திரும்பு -EINVAL;

        எனில் (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        திரும்பு 0;
#endif
}

நிலையான முழு parse_timestamp_impl(
                மாறிலி எழுத்து *t,
                size_t max_len,
                bool utc,
                முழு isdst,
                long gmtoff,
                usec_t *ret) {

        நிலையான மாறிலி கட்டமைப்பு {
                மாறிலி எழுத்து *name;
                மாறிலி முழு nr;
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

        _cleanup_free_ எழுத்து *t_alloc = வெற்று;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        முழு r, weekday = -1;
        unsigned fractional = 0;
        மாறிலி எழுத்து *k;
        கட்டமைப்பு tm tm, copy;

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

        ப்ரதிஜ்ஞா(t);

        எனில் (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                எனில் (max_len == 0) /* Can't be the only field */
                        திரும்பு -EINVAL;

                t_alloc = strndup(t, max_len);
                எனில் (!t_alloc)
                        திரும்பு -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        எனில் (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                எனில் ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        திரும்பு -EINVAL;
        } இல்லையெனில் {
                எனில் (gmtoff != 0)
                        திரும்பு -EINVAL;
        }

        எனில் (t[0] == '@' && !with_tz)
                திரும்பு parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        எனில் (!with_tz) {
                எனில் (streq(t, "now"))
                        செல் finish;

                எனில் (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        எனில் (r < 0)
                                திரும்பு r;

                        செல் finish;
                }

                எனில் (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        எனில் (r < 0)
                                திரும்பு r;

                        செல் finish;
                }

                எனில் ((k = endswith(t, " ago"))) {
                        _cleanup_free_ எழுத்து *buf = வெற்று;

                        buf = strndup(t, k - t);
                        எனில் (!buf)
                                திரும்பு -ENOMEM;

                        r = parse_sec(buf, &minus);
                        எனில் (r < 0)
                                திரும்பு r;

                        செல் finish;
                }

                எனில் ((k = endswith(t, " left"))) {
                        _cleanup_free_ எழுத்து *buf = வெற்று;

                        buf = strndup(t, k - t);
                        எனில் (!buf)
                                திரும்பு -ENOMEM;

                        r = parse_sec(buf, &plus);
                        எனில் (r < 0)
                                திரும்பு r;

                        செல் finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        எனில் (r < 0)
                திரும்பு r;

        tm.tm_isdst = isdst;

        எனில் (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                செல் from_tm;

        } இல்லையெனில் எனில் (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                செல் from_tm;

        } இல்லையெனில் எனில் (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                செல் from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                எனில் (!k || *k != ' ')
                        தொடர்;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                நிறுத்து;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        எனில் (k) {
                எனில் (*k == '.')
                        செல் parse_usec;
                இல்லையெனில் எனில் (*k == 0)
                        செல் from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        எனில் (k) {
                எனில் (*k == '.')
                        செல் parse_usec;
                இல்லையெனில் எனில் (*k == 0)
                        செல் from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        எனில் (k) {
                எனில் (*k == '.')
                        செல் parse_usec;
                இல்லையெனில் எனில் (*k == 0)
                        செல் from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        எனில் (k) {
                எனில் (*k == '.')
                        செல் parse_usec;
                இல்லையெனில் எனில் (*k == 0)
                        செல் from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        எனில் (k && *k == 0) {
                tm.tm_sec = 0;
                செல் from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        எனில் (k && *k == 0) {
                tm.tm_sec = 0;
                செல் from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        எனில் (k && *k == 0) {
                tm.tm_sec = 0;
                செல் from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        எனில் (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                செல் from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        எனில் (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                செல் from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        எனில் (k) {
                எனில் (*k == '.')
                        செல் parse_usec;
                இல்லையெனில் எனில் (*k == 0)
                        செல் from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        எனில் (k && *k == 0) {
                tm.tm_sec = 0;
                செல் from_tm;
        }

        திரும்பு -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        எனில் (r < 0)
                திரும்பு -EINVAL;
        எனில் (*k != '\0')
                திரும்பு -EINVAL;

from_tm:
        ப்ரதிஜ்ஞா(plus == 0);
        ப்ரதிஜ்ஞா(minus == 0);

        எனில் (weekday >= 0 && tm.tm_wday != weekday)
                திரும்பு -EINVAL;

        எனில் (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                எனில் (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } இல்லையெனில்
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        எனில் (r < 0)
                திரும்பு r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        எனில் (usec < minus)
                திரும்பு -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        எனில் (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                திரும்பு -EINVAL;

        எனில் (ret)
                *ret = usec;
        திரும்பு 0;
}

முழு parse_timestamp(மாறிலி எழுத்து *t, usec_t *ret) {
        long gmtoff;
        முழு r;

        ப்ரதிஜ்ஞா(t);

        size_t t_len = strlen(t);
        எனில் (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                எனில் (r >= 0)
                        திரும்பு r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        எனில் (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                திரும்பு parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        மாறிலி எழுத்து *tz = strrchr(t, ' ');
        எனில் (!tz)
                திரும்பு parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        எனில் (streq(tz, "UTC"))
                திரும்பு parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        எனில் (parse_gmtoff(tz, &gmtoff) >= 0)
                திரும்பு parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        க்கு (முழு j = 0; j <= 1; j++) {
                எனில் (!streq_ptr(tz, get_tzname(j)))
                        தொடர்;

                /* The specified timezone matches tzname[] of the local timezone. */
                திரும்பு parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        எனில் (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                எனில் (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        திரும்பு negative_errno();

                திரும்பு parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        திரும்பு parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

நிலையான மாறிலி எழுத்து* extract_multiplier(மாறிலி எழுத்து *p, usec_t *ret) {
        நிலையான மாறிலி கட்டமைப்பு {
                மாறிலி எழுத்து *suffix;
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

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        FOREACH_ELEMENT(i, table) {
                மாறிலி எழுத்து *e = startswith(p, i->suffix);
                எனில் (e) {
                        *ret = i->usec;
                        திரும்பு e;
                }
        }

        திரும்பு p;
}

முழு parse_time(மாறிலி எழுத்து *t, usec_t *ret, usec_t default_unit) {
        மாறிலி எழுத்து *p, *s;

        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ வெற்று);
        s = startswith(p, "infinity");
        எனில் (s) {
                எனில் (!in_charset(s, WHITESPACE))
                        திரும்பு -EINVAL;

                எனில் (ret)
                        *ret = USEC_INFINITY;
                திரும்பு 0;
        }

        usec_t usec = 0;

        க்கு (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                எழுத்து *e;

                p = skip_leading_chars(p, /* bad= */ வெற்று);
                எனில் (*p == 0) {
                        எனில் (!something)
                                திரும்பு -EINVAL;

                        நிறுத்து;
                }

                எனில் (*p == '-') /* Don't allow "-0" */
                        திரும்பு -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                எனில் (errno > 0)
                        திரும்பு -errno;
                எனில் (l < 0)
                        திரும்பு -ERANGE;

                எனில் (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } இல்லையெனில் எனில் (e == p)
                        திரும்பு -EINVAL;
                இல்லையெனில்
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                எனில் (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        திரும்பு -EINVAL;

                p = s;

                எனில் ((usec_t) l >= USEC_INFINITY / multiplier)
                        திரும்பு -ERANGE;

                k = (usec_t) l * multiplier;
                எனில் (k >= USEC_INFINITY - usec)
                        திரும்பு -ERANGE;

                usec += k;

                something = true;

                எனில் (*e == '.') {
                        usec_t m = multiplier / 10;
                        மாறிலி எழுத்து *b;

                        க்கு (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                எனில் (k >= USEC_INFINITY - usec)
                                        திரும்பு -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        எனில் (b == e + 1)
                                திரும்பு -EINVAL;
                }
        }

        எனில் (ret)
                *ret = usec;
        திரும்பு 0;
}

முழு parse_sec(மாறிலி எழுத்து *t, usec_t *ret) {
        திரும்பு parse_time(t, ret, USEC_PER_SEC);
}

முழு parse_sec_fix_0(மாறிலி எழுத்து *t, usec_t *ret) {
        usec_t k;
        முழு r;

        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(ret);

        r = parse_sec(t, &k);
        எனில் (r < 0)
                திரும்பு r;

        *ret = k == 0 ? USEC_INFINITY : k;
        திரும்பு r;
}

முழு parse_sec_def_infinity(மாறிலி எழுத்து *t, usec_t *ret) {
        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(ret);

        t += strspn(t, WHITESPACE);
        எனில் (isempty(t)) {
                *ret = USEC_INFINITY;
                திரும்பு 0;
        }
        திரும்பு parse_sec(t, ret);
}

நிலையான மாறிலி எழுத்து* extract_nsec_multiplier(மாறிலி எழுத்து *p, nsec_t *ret) {
        நிலையான மாறிலி கட்டமைப்பு {
                மாறிலி எழுத்து *suffix;
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

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        FOREACH_ELEMENT(i, table) {
                மாறிலி எழுத்து *e = startswith(p, i->suffix);
                எனில் (e) {
                        *ret = i->nsec;
                        திரும்பு e;
                }
        }

        திரும்பு p;
}

முழு parse_nsec(மாறிலி எழுத்து *t, nsec_t *ret) {
        மாறிலி எழுத்து *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        எனில் (s) {
                s += strspn(s, WHITESPACE);
                எனில் (*s != 0)
                        திரும்பு -EINVAL;

                *ret = NSEC_INFINITY;
                திரும்பு 0;
        }

        க்கு (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                எழுத்து *e;

                p += strspn(p, WHITESPACE);

                எனில் (*p == 0) {
                        எனில் (!something)
                                திரும்பு -EINVAL;

                        நிறுத்து;
                }

                எனில் (*p == '-') /* Don't allow "-0" */
                        திரும்பு -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                எனில் (errno > 0)
                        திரும்பு -errno;
                எனில் (l < 0)
                        திரும்பு -ERANGE;

                எனில் (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } இல்லையெனில் எனில் (e == p)
                        திரும்பு -EINVAL;
                இல்லையெனில்
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                எனில் (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        திரும்பு -EINVAL;

                p = s;

                எனில் ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        திரும்பு -ERANGE;

                k = (nsec_t) l * multiplier;
                எனில் (k >= NSEC_INFINITY - nsec)
                        திரும்பு -ERANGE;

                nsec += k;

                something = true;

                எனில் (*e == '.') {
                        nsec_t m = multiplier / 10;
                        மாறிலி எழுத்து *b;

                        க்கு (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                எனில் (k >= NSEC_INFINITY - nsec)
                                        திரும்பு -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        எனில் (b == e + 1)
                                திரும்பு -EINVAL;
                }
        }

        *ret = nsec;

        திரும்பு 0;
}

நிலையான முழு get_timezones_from_zone1970_tab(எழுத்து ***ret) {
        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_strv_free_ எழுத்து **zones = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        எனில் (!f)
                திரும்பு -errno;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று, *cc = வெற்று, *co = வெற்று, *tz = வெற்று;

                r = read_வரி(f, LONG_வரி_MAX, &வரி);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                மாறிலி எழுத்து *p = வரி;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_சொற்கள்(&p, வெற்று, 0, &cc, &co, &tz);
                எனில் (r < 0)
                        தொடர்;

                /* Lines that start with # are comments. */
                எனில் (*cc == '#')
                        தொடர்;

                எனில் (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        தொடர்;

                r = strv_extend(&zones, tz);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret = TAKE_PTR(zones);
        திரும்பு 0;
}

நிலையான முழு get_timezones_from_tzdata_zi(எழுத்து ***ret) {
        _cleanup_fclose_ FILE *f = வெற்று;
        _cleanup_strv_free_ எழுத்து **zones = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        எனில் (!f)
                திரும்பு -errno;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று, *type = வெற்று, *f1 = வெற்று, *f2 = வெற்று;

                r = read_வரி(f, LONG_வரி_MAX, &வரி);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                மாறிலி எழுத்து *p = வரி;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_சொற்கள்(&p, வெற்று, 0, &type, &f1, &f2);
                எனில் (r < 0)
                        தொடர்;

                எழுத்து *tz;
                எனில் (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                இல்லையெனில் எனில் (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                இல்லையெனில்
                        /* Not a line we care about. */
                        தொடர்;

                எனில் (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        தொடர்;

                r = strv_extend(&zones, tz);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *ret = TAKE_PTR(zones);
        திரும்பு 0;
}

முழு get_timezones(எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **zones = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        எனில் (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                எனில் (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        எனில் (r < 0 && r != -ENOENT)
                திரும்பு r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        எனில் (r < 0)
                திரும்பு r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        திரும்பு 0;
}

முழு verify_timezone(மாறிலி எழுத்து *name, முழு log_level) {
        bool slash = false;
        மாறிலி எழுத்து *p, *t;
        _cleanup_close_ முழு fd = -EBADF;
        எழுத்து buf[4];
        முழு r;

        எனில் (isempty(name))
                திரும்பு -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        எனில் (streq(name, "UTC"))
                திரும்பு 0;

        எனில் (name[0] == '/')
                திரும்பு -EINVAL;

        க்கு (p = name; *p; p++) {
                எனில் (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        திரும்பு -EINVAL;

                எனில் (*p == '/') {

                        எனில் (slash)
                                திரும்பு -EINVAL;

                        slash = true;
                } இல்லையெனில்
                        slash = false;
        }

        எனில் (slash)
                திரும்பு -EINVAL;

        எனில் (p - name >= PATH_MAX)
                திரும்பு -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        எனில் (r < 0)
                திரும்பு log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        எனில் (r < 0)
                திரும்பு log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        எனில் (memcmp(buf, "TZif", 4) != 0)
                திரும்பு log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        திரும்பு 0;
}

வெறுமை reset_timezonep(எழுத்து **p) {
        ப்ரதிஜ்ஞா(p);

        (வெறுமை) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

எழுத்து* save_timezone(வெறுமை) {
        மாறிலி எழுத்து *e = getenv("TZ");
        எனில் (!e)
                திரும்பு வெற்று;

        எழுத்து *s = strdup(e);
        எனில் (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        திரும்பு s;
}

bool clock_supported(clockid_t clock) {
        கட்டமைப்பு timespec ts;

        தேர்வு (clock) {

        நிலை CLOCK_MONOTONIC:
        நிலை CLOCK_REALTIME:
        நிலை CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                திரும்பு true;

        இயல்பு:
                /* For everything else, check properly */
                திரும்பு clock_gettime(clock, &ts) >= 0;
        }
}

முழு get_timezone(எழுத்து **ret) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = readlink_malloc(etc_localtime(), &t);
        எனில் (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                திரும்பு strdup_to(ret, "UTC");
        எனில் (r < 0)
                திரும்பு r; /* Return EINVAL if not a symlink */

        மாறிலி எழுத்து *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        எனில் (!e)
                திரும்பு -EINVAL;
        எனில் (!timezone_is_valid(e, LOG_DEBUG))
                திரும்பு -EINVAL;

        திரும்பு strdup_to(ret, e);
}

முழு get_timezone_prefer_env(எழுத்து **ret) {
        ப்ரதிஜ்ஞா(ret);

        மாறிலி எழுத்து *e = getenv("TZ");
        எனில் (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                திரும்பு strdup_to(ret, e + 1);

        திரும்பு get_timezone(ret);
}

மாறிலி எழுத்து* etc_localtime(வெறுமை) {
        நிலையான மாறிலி எழுத்து *cached = வெற்று;

        எனில் (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        திரும்பு cached;
}

முழு mktime_or_timegm_usec(
                கட்டமைப்பு tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        ப்ரதிஜ்ஞா(tm);

        எனில் (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                திரும்பு -ERANGE;
        எனில் ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                திரும்பு -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        எனில் (t < 0) /* Refuse negative times and errors */
                திரும்பு -ERANGE;
        எனில் ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                திரும்பு -ERANGE;

        எனில் (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        திரும்பு 0;
}

முழு localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                கட்டமைப்பு tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        எனில் (t > (usec_t) TIME_T_MAX)
                திரும்பு -ERANGE;
        time_t sec = (time_t) t;

        கட்டமைப்பு tm buf = {};
        எனில் (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                திரும்பு -EINVAL;

        எனில் (ret)
                *ret = buf;

        திரும்பு 0;
}

uint64_t sysconf_clock_ticks_cached(வெறுமை) {
        நிலையான thread_local uint64_t hz = 0;

        எனில் (hz != 0)
                திரும்பு hz;

        long t = sysconf(_SC_CLK_TCK);
        ப்ரதிஜ்ஞா(t > 0);
        hz = (uint64_t) t;

        திரும்பு hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        திரும்பு DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        திரும்பு DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        எனில் (x == USEC_INFINITY)
                திரும்பு USEC_INFINITY;
        எனில் (map_clock_id(from) == map_clock_id(to))
                திரும்பு x;

        a = now(from);
        b = now(to);

        எனில் (x > a)
                /* x lies in the future */
                திரும்பு usec_add(b, usec_sub_unsigned(x, a));
        இல்லையெனில்
                /* x lies in the past */
                திரும்பு usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(வெறுமை) {
        tzset();

        திரும்பு timezone == 0 && daylight == 0;
}

முழு usleep_safe(usec_t usec) {
        முழு r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        எனில் (usec == 0)
                திரும்பு 0;

        எனில் (usec == USEC_INFINITY)
                திரும்பு RET_NERRNO(pause());

        கட்டமைப்பு timespec t;
        timespec_store(&t, usec);

        க்கு (;;) {
                கட்டமைப்பு timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                எனில் (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        தொடர்;
                }

                திரும்பு r;
        }
}

முழு time_change_fd(வெறுமை) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        நிலையான மாறிலி கட்டமைப்பு itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ முழு fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு -errno;

        எனில் (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, வெற்று) >= 0)
                திரும்பு TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#எனில் SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        எனில் (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                நிலையான மாறிலி கட்டமைப்பு itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                எனில் (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, வெற்று) >= 0)
                        திரும்பு TAKE_FD(fd);
        }
#endif

        திரும்பு -errno;
}

நிலையான மாறிலி எழுத்து* மாறிலி timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(மாறிலி எழுத்து *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        எனில் (t >= 0)
                திரும்பு t;
        எனில் (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                திரும்பு TIMESTAMP_US;
        எனில் (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                திரும்பு TIMESTAMP_US_UTC;
        திரும்பு t;
}

முழு parse_calendar_date_full(மாறிலி எழுத்து *s, bool allow_pre_epoch, usec_t *ret_usec, கட்டமைப்பு tm *ret_tm) {
        கட்டமைப்பு tm parsed_tm = {}, copy_tm;
        மாறிலி எழுத்து *k;
        முழு r;

        ப்ரதிஜ்ஞா(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        எனில் (!k || *k)
                திரும்பு -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        எனில் (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                எனில் (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        திரும்பு -EINVAL;
        } இல்லையெனில் {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                எனில் (r < 0)
                        திரும்பு r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        எனில் (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                திரும்பு -EINVAL;

        எனில் (ret_usec)
                *ret_usec = usec;
        எனில் (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        திரும்பு 0;
}
