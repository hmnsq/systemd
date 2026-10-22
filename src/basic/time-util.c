/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/time_types.h>
#شامل <stdlib.h>
#شامل <sys/timerfd.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "env-util.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "hexdecoct.h"          /* IWYU pragma: keep */
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "fs-util.h"
#شامل "io-util.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "stdio-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "time-util.h"

ایستا clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        انتخاب (c) {

        حالت CLOCK_BOOTTIME_ALARM:
                بازگشت CLOCK_BOOTTIME;

        حالت CLOCK_REALTIME_ALARM:
                بازگشت CLOCK_REALTIME;

        پیشفرض:
                بازگشت c;
        }
}

usec_t now(clockid_t clock_id) {
        ساختار timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        أكد(n > 0);
        أكد(n < USEC_INFINITY);

        بازگشت n;
}

nsec_t now_nsec(clockid_t clock_id) {
        ساختار timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        أكد(n > 0);
        أكد(n < NSEC_INFINITY);

        بازگشت n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        أكد(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        بازگشت ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        أكد(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        بازگشت ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        اگر (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                اگر (to_base >= USEC_INFINITY - delta) /* overflow? */
                        بازگشت USEC_INFINITY;

                بازگشت to_base + delta;

        } وگرنه { /* In the past */
                usec_t delta = from_base - from;

                اگر (to_base <= delta) /* underflow? */
                        بازگشت 0;

                بازگشت to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        اگر (map_clock_id(from_clock) == map_clock_id(to_clock))
                بازگشت from;

        /* Keep infinity as is */
        اگر (from == USEC_INFINITY)
                بازگشت from;

        بازگشت map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        أكد(ts);

        اگر (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                بازگشت ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        بازگشت ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        أكد(ts);

        اگر (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                بازگشت ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        بازگشت ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        أكد(ts);

        اگر (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                بازگشت ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        بازگشت ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        أكد(ts);

        اگر (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                بازگشت ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        بازگشت ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        أكد(ts);

        اگر (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                بازگشت ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        بازگشت ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        أكد(ts);

        انتخاب (clock) {

        حالت CLOCK_REALTIME:
        حالت CLOCK_REALTIME_ALARM:
                بازگشت ts->realtime;

        حالت CLOCK_MONOTONIC:
                بازگشت ts->monotonic;

        حالت CLOCK_BOOTTIME:
        حالت CLOCK_BOOTTIME_ALARM:
                بازگشت ts->boottime;

        پیشفرض:
                بازگشت USEC_INFINITY;
        }
}

usec_t timespec_load(ثابت ساختار timespec *ts) {
        أكد(ts);

        اگر (ts->tv_sec < 0 || ts->tv_nsec < 0)
                بازگشت USEC_INFINITY;

        اگر ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                بازگشت USEC_INFINITY;

        بازگشت
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(ثابت ساختار timespec *ts) {
        أكد(ts);

        اگر (ts->tv_sec < 0 || ts->tv_nsec < 0)
                بازگشت NSEC_INFINITY;

        اگر ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                بازگشت NSEC_INFINITY;

        بازگشت (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

ساختار timespec *timespec_store(ساختار timespec *ts, usec_t u) {
        أكد(ts);

        اگر (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                بازگشت ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        بازگشت ts;
}

ساختار timespec *timespec_store_nsec(ساختار timespec *ts, nsec_t n) {
        أكد(ts);

        اگر (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                بازگشت ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        بازگشت ts;
}

ساختار __kernel_timespec *kernel_timespec_store(ساختار __kernel_timespec *ts, usec_t u) {
        أكد(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        أكد(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        بازگشت ts;
}

usec_t timeval_load(ثابت ساختار timeval *tv) {
        أكد(tv);

        اگر (tv->tv_sec < 0 || tv->tv_usec < 0)
                بازگشت USEC_INFINITY;

        اگر ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                بازگشت USEC_INFINITY;

        بازگشت
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

ساختار timeval *timeval_store(ساختار timeval *tv, usec_t u) {
        أكد(tv);

        اگر (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } وگرنه {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        بازگشت tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
ایستا ثابت نویسه *ثابت weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, صحیح);

نویسه* format_timestamp_style(
                نویسه *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        ساختار tm tm;
        bool utc, us;
        size_t n;

        أكد(buf);
        أكد(style >= 0);
        أكد(style < _TIMESTAMP_STYLE_MAX);

        اگر (!timestamp_is_set(t))
                بازگشت پوچ; /* Timestamp is unset */

        اگر (style == TIMESTAMP_UNIX) {
                اگر (l < (size_t) (1 + 1 + 1))
                        بازگشت پوچ; /* not enough space for even the shortest of forms */

                بازگشت snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        اگر (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                بازگشت پوچ; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        اگر (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                ایستا ثابت نویسه* ثابت xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                أكد(l >= strlen(xxx[style]) + 1);
                بازگشت strcpy(buf, xxx[style]);
        }

        اگر (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                بازگشت پوچ;

        /* Start with the week day */
        ثابت نویسه *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        أكد(weekday);
        memcpy(buf, weekday, 4);

        اگر (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                اگر (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        بازگشت پوچ; /* Doesn't fit */

                بازگشت buf;
        }

        /* Add the main components */
        اگر (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                بازگشت پوچ; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        اگر (us) {
                n = strlen(buf);
                اگر (n + 8 > l)
                        بازگشت پوچ; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        اگر (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                اگر (n + 5 > l)
                        بازگشت پوچ; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } وگرنه اگر (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                اگر (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        اگر (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                بازگشت پوچ; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } وگرنه {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        بازگشت buf;
}

نویسه* format_timestamp_relative_full(نویسه *buf, size_t l, usec_t t, clockid_t clock, bool implicit_چپ) {
        ثابت نویسه *s;
        usec_t n, d;

        أكد(buf);

        اگر (!timestamp_is_set(t))
                بازگشت پوچ;

        n = now(clock);
        اگر (n > t) {
                d = n - t;
                s = " ago";
        } وگرنه {
                d = t - n;
                s = implicit_چپ ? "" : " left";
        }

        اگر (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (تهی) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } وگرنه اگر (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (تهی) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } وگرنه اگر (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (تهی) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } وگرنه اگر (d >= 2*USEC_PER_DAY)
                (تهی) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        وگرنه اگر (d >= 25*USEC_PER_HOUR)
                (تهی) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        وگرنه اگر (d >= 6*USEC_PER_HOUR)
                (تهی) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        وگرنه اگر (d >= USEC_PER_HOUR)
                (تهی) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        وگرنه اگر (d >= 5*USEC_PER_MINUTE)
                (تهی) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        وگرنه اگر (d >= USEC_PER_MINUTE)
                (تهی) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        وگرنه اگر (d >= USEC_PER_SEC)
                (تهی) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        وگرنه اگر (d >= USEC_PER_MSEC)
                (تهی) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        وگرنه اگر (d > 0)
                (تهی) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        وگرنه
                (تهی) snprintf(buf, l, "now");

        buf[l-1] = 0;
        بازگشت buf;
}

نویسه* format_timespan(نویسه *buf, size_t l, usec_t t, usec_t accuracy) {
        ایستا ثابت ساختار {
                ثابت نویسه *suffix;
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

        نویسه *p = ASSERT_PTR(buf);
        bool something = false;

        أكد(l > 0);

        اگر (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                بازگشت p;
        }

        اگر (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                بازگشت p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                صحیح k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                اگر (t <= 0)
                        بشکن;

                اگر (t < accuracy && something)
                        بشکن;

                اگر (t < i->usec)
                        ادامه;

                اگر (l <= 1)
                        بشکن;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                اگر (t < USEC_PER_MINUTE && b > 0) {
                        signed نویسه j = 0;

                        برای (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        برای (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        اگر (j > 0) {
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
                اگر (!done) {
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

        بازگشت buf;
}

ثابت نویسه* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        اگر (dst && isempty(tzname[1]))
                dst = false;

        بازگشت empty_to_null(tzname[dst]);
}

صحیح parse_gmtoff(ثابت نویسه *t, long *ret) {
        أكد(t);

        ساختار tm tm;
        ثابت نویسه *k = strptime(t, "%z", &tm);
        اگر (k && *k == '\0') {
                /* Success! */
                اگر (ret)
                        *ret = tm.tm_gmtoff;
                بازگشت 0;
        }

#ifdef __GLIBC__
        بازگشت -EINVAL;
#وگرنه
        صحیح r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        انتخاب (*t) {
        حالت '+':
                positive = true;
                بشکن;
        حالت '-':
                positive = false;
                بشکن;
        پیشفرض:
                بازگشت -EINVAL;
        }

        t++;
        r = undecchar(*t);
        اگر (r < 0)
                بازگشت r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        اگر (r < 0)
                بازگشت r;
        u += r * USEC_PER_HOUR;

        t++;
        اگر (*t == '\0') /* 2 digits case */
                برو finalize;

        اگر (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        اگر (r < 0)
                بازگشت r;
        اگر (r >= 6) /* refuse minutes equal to or larger than 60 */
                بازگشت -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        اگر (r < 0)
                بازگشت r;

        u += r * USEC_PER_MINUTE;

        t++;
        اگر (*t != '\0')
                بازگشت -EINVAL;

finalize:
        اگر (u > USEC_PER_DAY) /* refuse larger than one day */
                بازگشت -EINVAL;

        اگر (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        بازگشت 0;
#endif
}

ایستا صحیح parse_timestamp_impl(
                ثابت نویسه *t,
                size_t max_len,
                bool utc,
                صحیح isdst,
                long gmtoff,
                usec_t *ret) {

        ایستا ثابت ساختار {
                ثابت نویسه *name;
                ثابت صحیح nr;
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

        _cleanup_free_ نویسه *t_alloc = پوچ;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        صحیح r, weekday = -1;
        unsigned fractional = 0;
        ثابت نویسه *k;
        ساختار tm tm, copy;

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

        اگر (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                اگر (max_len == 0) /* Can't be the only field */
                        بازگشت -EINVAL;

                t_alloc = strndup(t, max_len);
                اگر (!t_alloc)
                        بازگشت -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        اگر (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                اگر ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        بازگشت -EINVAL;
        } وگرنه {
                اگر (gmtoff != 0)
                        بازگشت -EINVAL;
        }

        اگر (t[0] == '@' && !with_tz)
                بازگشت parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        اگر (!with_tz) {
                اگر (streq(t, "now"))
                        برو finish;

                اگر (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        اگر (r < 0)
                                بازگشت r;

                        برو finish;
                }

                اگر (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        اگر (r < 0)
                                بازگشت r;

                        برو finish;
                }

                اگر ((k = endswith(t, " ago"))) {
                        _cleanup_free_ نویسه *buf = پوچ;

                        buf = strndup(t, k - t);
                        اگر (!buf)
                                بازگشت -ENOMEM;

                        r = parse_sec(buf, &minus);
                        اگر (r < 0)
                                بازگشت r;

                        برو finish;
                }

                اگر ((k = endswith(t, " left"))) {
                        _cleanup_free_ نویسه *buf = پوچ;

                        buf = strndup(t, k - t);
                        اگر (!buf)
                                بازگشت -ENOMEM;

                        r = parse_sec(buf, &plus);
                        اگر (r < 0)
                                بازگشت r;

                        برو finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        اگر (r < 0)
                بازگشت r;

        tm.tm_isdst = isdst;

        اگر (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                برو from_tm;

        } وگرنه اگر (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                برو from_tm;

        } وگرنه اگر (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                برو from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                اگر (!k || *k != ' ')
                        ادامه;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                بشکن;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        برو parse_usec;
                وگرنه اگر (*k == 0)
                        برو from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        برو parse_usec;
                وگرنه اگر (*k == 0)
                        برو from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        برو parse_usec;
                وگرنه اگر (*k == 0)
                        برو from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        برو parse_usec;
                وگرنه اگر (*k == 0)
                        برو from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = 0;
                برو from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = 0;
                برو from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = 0;
                برو from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                برو from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                برو from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        برو parse_usec;
                وگرنه اگر (*k == 0)
                        برو from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = 0;
                برو from_tm;
        }

        بازگشت -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        اگر (r < 0)
                بازگشت -EINVAL;
        اگر (*k != '\0')
                بازگشت -EINVAL;

from_tm:
        أكد(plus == 0);
        أكد(minus == 0);

        اگر (weekday >= 0 && tm.tm_wday != weekday)
                بازگشت -EINVAL;

        اگر (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                اگر (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } وگرنه
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        اگر (r < 0)
                بازگشت r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        اگر (usec < minus)
                بازگشت -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        اگر (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                بازگشت -EINVAL;

        اگر (ret)
                *ret = usec;
        بازگشت 0;
}

صحیح parse_timestamp(ثابت نویسه *t, usec_t *ret) {
        long gmtoff;
        صحیح r;

        أكد(t);

        size_t t_len = strlen(t);
        اگر (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                اگر (r >= 0)
                        بازگشت r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        اگر (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                بازگشت parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ثابت نویسه *tz = strrchr(t, ' ');
        اگر (!tz)
                بازگشت parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        اگر (streq(tz, "UTC"))
                بازگشت parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        اگر (parse_gmtoff(tz, &gmtoff) >= 0)
                بازگشت parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        برای (صحیح j = 0; j <= 1; j++) {
                اگر (!streq_ptr(tz, get_tzname(j)))
                        ادامه;

                /* The specified timezone matches tzname[] of the local timezone. */
                بازگشت parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        اگر (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                اگر (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        بازگشت negative_errno();

                بازگشت parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        بازگشت parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

ایستا ثابت نویسه* extract_multiplier(ثابت نویسه *p, usec_t *ret) {
        ایستا ثابت ساختار {
                ثابت نویسه *suffix;
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
                ثابت نویسه *e = startswith(p, i->suffix);
                اگر (e) {
                        *ret = i->usec;
                        بازگشت e;
                }
        }

        بازگشت p;
}

صحیح parse_time(ثابت نویسه *t, usec_t *ret, usec_t default_unit) {
        ثابت نویسه *p, *s;

        أكد(t);
        أكد(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ پوچ);
        s = startswith(p, "infinity");
        اگر (s) {
                اگر (!in_charset(s, WHITESPACE))
                        بازگشت -EINVAL;

                اگر (ret)
                        *ret = USEC_INFINITY;
                بازگشت 0;
        }

        usec_t usec = 0;

        برای (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                نویسه *e;

                p = skip_leading_chars(p, /* bad= */ پوچ);
                اگر (*p == 0) {
                        اگر (!something)
                                بازگشت -EINVAL;

                        بشکن;
                }

                اگر (*p == '-') /* Don't allow "-0" */
                        بازگشت -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                اگر (errno > 0)
                        بازگشت -errno;
                اگر (l < 0)
                        بازگشت -ERANGE;

                اگر (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } وگرنه اگر (e == p)
                        بازگشت -EINVAL;
                وگرنه
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                اگر (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        بازگشت -EINVAL;

                p = s;

                اگر ((usec_t) l >= USEC_INFINITY / multiplier)
                        بازگشت -ERANGE;

                k = (usec_t) l * multiplier;
                اگر (k >= USEC_INFINITY - usec)
                        بازگشت -ERANGE;

                usec += k;

                something = true;

                اگر (*e == '.') {
                        usec_t m = multiplier / 10;
                        ثابت نویسه *b;

                        برای (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                اگر (k >= USEC_INFINITY - usec)
                                        بازگشت -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        اگر (b == e + 1)
                                بازگشت -EINVAL;
                }
        }

        اگر (ret)
                *ret = usec;
        بازگشت 0;
}

صحیح parse_sec(ثابت نویسه *t, usec_t *ret) {
        بازگشت parse_time(t, ret, USEC_PER_SEC);
}

صحیح parse_sec_fix_0(ثابت نویسه *t, usec_t *ret) {
        usec_t k;
        صحیح r;

        أكد(t);
        أكد(ret);

        r = parse_sec(t, &k);
        اگر (r < 0)
                بازگشت r;

        *ret = k == 0 ? USEC_INFINITY : k;
        بازگشت r;
}

صحیح parse_sec_def_infinity(ثابت نویسه *t, usec_t *ret) {
        أكد(t);
        أكد(ret);

        t += strspn(t, WHITESPACE);
        اگر (isempty(t)) {
                *ret = USEC_INFINITY;
                بازگشت 0;
        }
        بازگشت parse_sec(t, ret);
}

ایستا ثابت نویسه* extract_nsec_multiplier(ثابت نویسه *p, nsec_t *ret) {
        ایستا ثابت ساختار {
                ثابت نویسه *suffix;
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
                ثابت نویسه *e = startswith(p, i->suffix);
                اگر (e) {
                        *ret = i->nsec;
                        بازگشت e;
                }
        }

        بازگشت p;
}

صحیح parse_nsec(ثابت نویسه *t, nsec_t *ret) {
        ثابت نویسه *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        أكد(t);
        أكد(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        اگر (s) {
                s += strspn(s, WHITESPACE);
                اگر (*s != 0)
                        بازگشت -EINVAL;

                *ret = NSEC_INFINITY;
                بازگشت 0;
        }

        برای (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                نویسه *e;

                p += strspn(p, WHITESPACE);

                اگر (*p == 0) {
                        اگر (!something)
                                بازگشت -EINVAL;

                        بشکن;
                }

                اگر (*p == '-') /* Don't allow "-0" */
                        بازگشت -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                اگر (errno > 0)
                        بازگشت -errno;
                اگر (l < 0)
                        بازگشت -ERANGE;

                اگر (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } وگرنه اگر (e == p)
                        بازگشت -EINVAL;
                وگرنه
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                اگر (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        بازگشت -EINVAL;

                p = s;

                اگر ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        بازگشت -ERANGE;

                k = (nsec_t) l * multiplier;
                اگر (k >= NSEC_INFINITY - nsec)
                        بازگشت -ERANGE;

                nsec += k;

                something = true;

                اگر (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ثابت نویسه *b;

                        برای (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                اگر (k >= NSEC_INFINITY - nsec)
                                        بازگشت -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        اگر (b == e + 1)
                                بازگشت -EINVAL;
                }
        }

        *ret = nsec;

        بازگشت 0;
}

ایستا صحیح get_timezones_from_zone1970_tab(نویسه ***ret) {
        _cleanup_fclose_ FILE *f = پوچ;
        _cleanup_strv_free_ نویسه **zones = پوچ;
        صحیح r;

        أكد(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        اگر (!f)
                بازگشت -errno;

        برای (;;) {
                _cleanup_free_ نویسه *سطر = پوچ, *cc = پوچ, *co = پوچ, *tz = پوچ;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                ثابت نویسه *p = سطر;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_واژگان(&p, پوچ, 0, &cc, &co, &tz);
                اگر (r < 0)
                        ادامه;

                /* Lines that start with # are comments. */
                اگر (*cc == '#')
                        ادامه;

                اگر (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ادامه;

                r = strv_extend(&zones, tz);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret = TAKE_PTR(zones);
        بازگشت 0;
}

ایستا صحیح get_timezones_from_tzdata_zi(نویسه ***ret) {
        _cleanup_fclose_ FILE *f = پوچ;
        _cleanup_strv_free_ نویسه **zones = پوچ;
        صحیح r;

        أكد(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        اگر (!f)
                بازگشت -errno;

        برای (;;) {
                _cleanup_free_ نویسه *سطر = پوچ, *type = پوچ, *f1 = پوچ, *f2 = پوچ;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                ثابت نویسه *p = سطر;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_واژگان(&p, پوچ, 0, &type, &f1, &f2);
                اگر (r < 0)
                        ادامه;

                نویسه *tz;
                اگر (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                وگرنه اگر (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                وگرنه
                        /* Not a line we care about. */
                        ادامه;

                اگر (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ادامه;

                r = strv_extend(&zones, tz);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret = TAKE_PTR(zones);
        بازگشت 0;
}

صحیح get_timezones(نویسه ***ret) {
        _cleanup_strv_free_ نویسه **zones = پوچ;
        صحیح r;

        أكد(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        اگر (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                اگر (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        اگر (r < 0 && r != -ENOENT)
                بازگشت r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        اگر (r < 0)
                بازگشت r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        بازگشت 0;
}

صحیح verify_timezone(ثابت نویسه *name, صحیح log_level) {
        bool slash = false;
        ثابت نویسه *p, *t;
        _cleanup_close_ صحیح fd = -EBADF;
        نویسه buf[4];
        صحیح r;

        اگر (isempty(name))
                بازگشت -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        اگر (streq(name, "UTC"))
                بازگشت 0;

        اگر (name[0] == '/')
                بازگشت -EINVAL;

        برای (p = name; *p; p++) {
                اگر (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        بازگشت -EINVAL;

                اگر (*p == '/') {

                        اگر (slash)
                                بازگشت -EINVAL;

                        slash = true;
                } وگرنه
                        slash = false;
        }

        اگر (slash)
                بازگشت -EINVAL;

        اگر (p - name >= PATH_MAX)
                بازگشت -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        اگر (fd < 0)
                بازگشت log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        اگر (r < 0)
                بازگشت log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        اگر (r < 0)
                بازگشت log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        اگر (memcmp(buf, "TZif", 4) != 0)
                بازگشت log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        بازگشت 0;
}

تهی reset_timezonep(نویسه **p) {
        أكد(p);

        (تهی) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

نویسه* save_timezone(تهی) {
        ثابت نویسه *e = getenv("TZ");
        اگر (!e)
                بازگشت پوچ;

        نویسه *s = strdup(e);
        اگر (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        بازگشت s;
}

bool clock_supported(clockid_t clock) {
        ساختار timespec ts;

        انتخاب (clock) {

        حالت CLOCK_MONOTONIC:
        حالت CLOCK_REALTIME:
        حالت CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                بازگشت true;

        پیشفرض:
                /* For everything else, check properly */
                بازگشت clock_gettime(clock, &ts) >= 0;
        }
}

صحیح get_timezone(نویسه **ret) {
        _cleanup_free_ نویسه *t = پوچ;
        صحیح r;

        أكد(ret);

        r = readlink_malloc(etc_localtime(), &t);
        اگر (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                بازگشت strdup_to(ret, "UTC");
        اگر (r < 0)
                بازگشت r; /* Return EINVAL if not a symlink */

        ثابت نویسه *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        اگر (!e)
                بازگشت -EINVAL;
        اگر (!timezone_is_valid(e, LOG_DEBUG))
                بازگشت -EINVAL;

        بازگشت strdup_to(ret, e);
}

صحیح get_timezone_prefer_env(نویسه **ret) {
        أكد(ret);

        ثابت نویسه *e = getenv("TZ");
        اگر (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                بازگشت strdup_to(ret, e + 1);

        بازگشت get_timezone(ret);
}

ثابت نویسه* etc_localtime(تهی) {
        ایستا ثابت نویسه *cached = پوچ;

        اگر (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        بازگشت cached;
}

صحیح mktime_or_timegm_usec(
                ساختار tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        أكد(tm);

        اگر (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                بازگشت -ERANGE;
        اگر ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                بازگشت -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        اگر (t < 0) /* Refuse negative times and errors */
                بازگشت -ERANGE;
        اگر ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                بازگشت -ERANGE;

        اگر (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        بازگشت 0;
}

صحیح localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                ساختار tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        اگر (t > (usec_t) TIME_T_MAX)
                بازگشت -ERANGE;
        time_t sec = (time_t) t;

        ساختار tm buf = {};
        اگر (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                بازگشت -EINVAL;

        اگر (ret)
                *ret = buf;

        بازگشت 0;
}

uint64_t sysconf_clock_ticks_cached(تهی) {
        ایستا thread_local uint64_t hz = 0;

        اگر (hz != 0)
                بازگشت hz;

        long t = sysconf(_SC_CLK_TCK);
        أكد(t > 0);
        hz = (uint64_t) t;

        بازگشت hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        بازگشت DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        بازگشت DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        اگر (x == USEC_INFINITY)
                بازگشت USEC_INFINITY;
        اگر (map_clock_id(from) == map_clock_id(to))
                بازگشت x;

        a = now(from);
        b = now(to);

        اگر (x > a)
                /* x lies in the future */
                بازگشت usec_add(b, usec_sub_unsigned(x, a));
        وگرنه
                /* x lies in the past */
                بازگشت usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(تهی) {
        tzset();

        بازگشت timezone == 0 && daylight == 0;
}

صحیح usleep_safe(usec_t usec) {
        صحیح r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        اگر (usec == 0)
                بازگشت 0;

        اگر (usec == USEC_INFINITY)
                بازگشت RET_NERRNO(pause());

        ساختار timespec t;
        timespec_store(&t, usec);

        برای (;;) {
                ساختار timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                اگر (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        ادامه;
                }

                بازگشت r;
        }
}

صحیح time_change_fd(تهی) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        ایستا ثابت ساختار itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ صحیح fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        اگر (fd < 0)
                بازگشت -errno;

        اگر (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, پوچ) >= 0)
                بازگشت TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#اگر SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        اگر (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                ایستا ثابت ساختار itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                اگر (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, پوچ) >= 0)
                        بازگشت TAKE_FD(fd);
        }
#endif

        بازگشت -errno;
}

ایستا ثابت نویسه* ثابت timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ثابت نویسه *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        اگر (t >= 0)
                بازگشت t;
        اگر (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                بازگشت TIMESTAMP_US;
        اگر (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                بازگشت TIMESTAMP_US_UTC;
        بازگشت t;
}

صحیح parse_calendar_date_full(ثابت نویسه *s, bool allow_pre_epoch, usec_t *ret_usec, ساختار tm *ret_tm) {
        ساختار tm parsed_tm = {}, copy_tm;
        ثابت نویسه *k;
        صحیح r;

        أكد(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        اگر (!k || *k)
                بازگشت -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        اگر (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                اگر (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        بازگشت -EINVAL;
        } وگرنه {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                اگر (r < 0)
                        بازگشت r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        اگر (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                بازگشت -EINVAL;

        اگر (ret_usec)
                *ret_usec = usec;
        اگر (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        بازگشت 0;
}
