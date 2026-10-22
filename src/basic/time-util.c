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

ساکن clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        انتخاب (c) {

        حالت CLOCK_BOOTTIME_ALARM:
                واپس CLOCK_BOOTTIME;

        حالت CLOCK_REALTIME_ALARM:
                واپس CLOCK_REALTIME;

        طےشدہ:
                واپس c;
        }
}

usec_t now(clockid_t clock_id) {
        ساخت timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_بھرو(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        أكد(n > 0);
        أكد(n < USEC_INFINITY);

        واپس n;
}

nsec_t now_nsec(clockid_t clock_id) {
        ساخت timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_بھرو_nsec(&ts);

        أكد(n > 0);
        أكد(n < NSEC_INFINITY);

        واپس n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        أكد(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        واپس ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        أكد(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        واپس ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_بنیاد, usec_t مقصد_بنیاد) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        اگر (from >= from_بنیاد) { /* In the future */
                usec_t delta = from - from_بنیاد;

                اگر (مقصد_بنیاد >= USEC_INFINITY - delta) /* overflow? */
                        واپس USEC_INFINITY;

                واپس مقصد_بنیاد + delta;

        } ورنہ { /* In the past */
                usec_t delta = from_بنیاد - from;

                اگر (مقصد_بنیاد <= delta) /* underflow? */
                        واپس 0;

                واپس مقصد_بنیاد - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t مقصد_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        اگر (map_clock_id(from_clock) == map_clock_id(مقصد_clock))
                واپس from;

        /* Keep infinity as is */
        اگر (from == USEC_INFINITY)
                واپس from;

        واپس map_clock_usec_raw(from, now(from_clock), now(مقصد_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        أكد(ts);

        اگر (!timestamp_is_سیٹ(u)) {
                ts->realtime = ts->monotonic = u;
                واپس ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        واپس ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        أكد(ts);

        اگر (!timestamp_is_سیٹ(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                واپس ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        واپس ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        أكد(ts);

        اگر (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                واپس ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        واپس ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        أكد(ts);

        اگر (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                واپس ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        واپس ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        أكد(ts);

        اگر (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                واپس ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        واپس ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        أكد(ts);

        انتخاب (clock) {

        حالت CLOCK_REALTIME:
        حالت CLOCK_REALTIME_ALARM:
                واپس ts->realtime;

        حالت CLOCK_MONOTONIC:
                واپس ts->monotonic;

        حالت CLOCK_BOOTTIME:
        حالت CLOCK_BOOTTIME_ALARM:
                واپس ts->boottime;

        طےشدہ:
                واپس USEC_INFINITY;
        }
}

usec_t timespec_بھرو(ثابت ساخت timespec *ts) {
        أكد(ts);

        اگر (ts->tv_sec < 0 || ts->tv_nsec < 0)
                واپس USEC_INFINITY;

        اگر ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_فی_USEC)) / USEC_فی_SEC)
                واپس USEC_INFINITY;

        واپس
                (usec_t) ts->tv_sec * USEC_فی_SEC +
                (usec_t) ts->tv_nsec / NSEC_فی_USEC;
}

nsec_t timespec_بھرو_nsec(ثابت ساخت timespec *ts) {
        أكد(ts);

        اگر (ts->tv_sec < 0 || ts->tv_nsec < 0)
                واپس NSEC_INFINITY;

        اگر ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_فی_SEC)
                واپس NSEC_INFINITY;

        واپس (nsec_t) ts->tv_sec * NSEC_فی_SEC + (nsec_t) ts->tv_nsec;
}

ساخت timespec *timespec_store(ساخت timespec *ts, usec_t u) {
        أكد(ts);

        اگر (u == USEC_INFINITY ||
            u / USEC_فی_SEC >= وقت_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                واپس ts;
        }

        ts->tv_sec = (time_t) (u / USEC_فی_SEC);
        ts->tv_nsec = (طویل) ((u % USEC_فی_SEC) * NSEC_فی_USEC);

        واپس ts;
}

ساخت timespec *timespec_store_nsec(ساخت timespec *ts, nsec_t n) {
        أكد(ts);

        اگر (n == NSEC_INFINITY ||
            n / NSEC_فی_SEC >= وقت_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                واپس ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_فی_SEC);
        ts->tv_nsec = (طویل) (n % NSEC_فی_SEC);

        واپس ts;
}

ساخت __kernel_timespec *kernel_timespec_store(ساخت __kernel_timespec *ts, usec_t u) {
        أكد(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        أكد(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_فی_SEC;
        ts->tv_nsec = (u % USEC_فی_SEC) * NSEC_فی_USEC;

        واپس ts;
}

usec_t timeval_بھرو(ثابت ساخت timeval *tv) {
        أكد(tv);

        اگر (tv->tv_sec < 0 || tv->tv_usec < 0)
                واپس USEC_INFINITY;

        اگر ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_فی_SEC)
                واپس USEC_INFINITY;

        واپس
                (usec_t) tv->tv_sec * USEC_فی_SEC +
                (usec_t) tv->tv_usec;
}

ساخت timeval *timeval_store(ساخت timeval *tv, usec_t u) {
        أكد(tv);

        اگر (u == USEC_INFINITY ||
            u / USEC_فی_SEC > وقت_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } ورنہ {
                tv->tv_sec = (time_t) (u / USEC_فی_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_فی_SEC);
        }

        واپس tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
ساکن ثابت حرف *ثابت weekday_جدول[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_جدول_LOOKUP_مقصد_STRING(weekday, صحیح);

حرف* format_timestamp_style(
                حرف *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        ساخت tm tm;
        bool utc, us;
        size_t n;

        أكد(buf);
        أكد(style >= 0);
        أكد(style < _TIMESTAMP_STYLE_MAX);

        اگر (!timestamp_is_سیٹ(t))
                واپس تہی; /* Timestamp is unset */

        اگر (style == TIMESTAMP_UNIX) {
                اگر (l < (size_t) (1 + 1 + 1))
                        واپس تہی; /* not enough space for even the shortest of forms */

                واپس snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_فی_SEC);  /* round down μs → s */
        }

        utc = اندر_سیٹ(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = اندر_سیٹ(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        اگر (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                واپس تہی; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        اگر (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                ساکن ثابت حرف* ثابت xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                أكد(l >= لڑی_لمبائی(xxx[style]) + 1);
                واپس strcpy(buf, xxx[style]);
        }

        اگر (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                واپس تہی;

        /* Start with the week day */
        ثابت حرف *weekday = weekday_مقصد_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        أكد(weekday);
        حافظہ_نقل(buf, weekday, 4);

        اگر (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                اگر (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        واپس تہی; /* Doesn't fit */

                واپس buf;
        }

        /* Add the main components */
        اگر (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                واپس تہی; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        اگر (us) {
                n = لڑی_لمبائی(buf);
                اگر (n + 8 > l)
                        واپس تہی; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_فی_SEC);
        }

        /* Append the timezone */
        n = لڑی_لمبائی(buf);
        اگر (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                اگر (n + 5 > l)
                        واپس تہی; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } ورنہ اگر (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = لڑی_لمبائی(tm.tm_zone);
                اگر (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        اگر (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                واپس تہی; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } ورنہ {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        واپس buf;
}

حرف* format_timestamp_relative_بھرا(حرف *buf, size_t l, usec_t t, clockid_t clock, bool implicit_بایاں) {
        ثابت حرف *s;
        usec_t n, d;

        أكد(buf);

        اگر (!timestamp_is_سیٹ(t))
                واپس تہی;

        n = now(clock);
        اگر (n > t) {
                d = n - t;
                s = " ago";
        } ورنہ {
                d = t - n;
                s = implicit_بایاں ? "" : " left";
        }

        اگر (d >= USEC_فی_YEAR) {
                usec_t years = d / USEC_فی_YEAR;
                usec_t months = (d % USEC_فی_YEAR) / USEC_فی_MONTH;

                (خالی) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } ورنہ اگر (d >= USEC_فی_MONTH) {
                usec_t months = d / USEC_فی_MONTH;
                usec_t days = (d % USEC_فی_MONTH) / USEC_فی_DAY;

                (خالی) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } ورنہ اگر (d >= USEC_فی_WEEK) {
                usec_t weeks = d / USEC_فی_WEEK;
                usec_t days = (d % USEC_فی_WEEK) / USEC_فی_DAY;

                (خالی) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } ورنہ اگر (d >= 2*USEC_فی_DAY)
                (خالی) snprintf(buf, l, USEC_FMT " days%s", d / USEC_فی_DAY,s);
        ورنہ اگر (d >= 25*USEC_فی_HOUR)
                (خالی) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_فی_DAY) / USEC_فی_HOUR, s);
        ورنہ اگر (d >= 6*USEC_فی_HOUR)
                (خالی) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_فی_HOUR, s);
        ورنہ اگر (d >= USEC_فی_HOUR)
                (خالی) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_فی_HOUR,
                                (d % USEC_فی_HOUR) / USEC_فی_MINUTE, s);
        ورنہ اگر (d >= 5*USEC_فی_MINUTE)
                (خالی) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_فی_MINUTE, s);
        ورنہ اگر (d >= USEC_فی_MINUTE)
                (خالی) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_فی_MINUTE,
                                (d % USEC_فی_MINUTE) / USEC_فی_SEC, s);
        ورنہ اگر (d >= USEC_فی_SEC)
                (خالی) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_فی_SEC, s);
        ورنہ اگر (d >= USEC_فی_MSEC)
                (خالی) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_فی_MSEC, s);
        ورنہ اگر (d > 0)
                (خالی) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        ورنہ
                (خالی) snprintf(buf, l, "now");

        buf[l-1] = 0;
        واپس buf;
}

حرف* format_timespan(حرف *buf, size_t l, usec_t t, usec_t accuracy) {
        ساکن ثابت ساخت {
                ثابت حرف *suffix;
                usec_t usec;
        } جدول[] = {
                { "y",     USEC_فی_YEAR   },
                { "month", USEC_فی_MONTH  },
                { "w",     USEC_فی_WEEK   },
                { "d",     USEC_فی_DAY    },
                { "h",     USEC_فی_HOUR   },
                { "min",   USEC_فی_MINUTE },
                { "s",     USEC_فی_SEC    },
                { "ms",    USEC_فی_MSEC   },
                { "us",    1               },
        };

        حرف *p = ASSERT_PTR(buf);
        bool something = false;

        أكد(l > 0);

        اگر (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                واپس p;
        }

        اگر (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                واپس p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, جدول) {
                صحیح k = 0;
                size_t n;
                bool مکمل = false;
                usec_t a, b;

                اگر (t <= 0)
                        توڑو;

                اگر (t < accuracy && something)
                        توڑو;

                اگر (t < i->usec)
                        جاری;

                اگر (l <= 1)
                        توڑو;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                اگر (t < USEC_فی_MINUTE && b > 0) {
                        علامتی حرف j = 0;

                        برائے (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        برائے (usec_t cc = accuracy; cc > 1; cc /= 10) {
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
                                مکمل = true;
                        }
                }

                /* No? Then let's show it normally */
                اگر (!مکمل) {
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

        واپس buf;
}

ثابت حرف* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        اگر (dst && isempty(tzname[1]))
                dst = false;

        واپس خالی_خانے_مقصد_null(tzname[dst]);
}

صحیح parse_gmtoff(ثابت حرف *t, طویل *ret) {
        أكد(t);

        ساخت tm tm;
        ثابت حرف *k = strptime(t, "%z", &tm);
        اگر (k && *k == '\0') {
                /* Success! */
                اگر (ret)
                        *ret = tm.tm_gmtoff;
                واپس 0;
        }

#اگر_تعریف_شدہ __GLIBC__
        واپس -EINVAL;
#ورنہ
        صحیح r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool مثبت;
        انتخاب (*t) {
        حالت '+':
                مثبت = true;
                توڑو;
        حالت '-':
                مثبت = false;
                توڑو;
        طےشدہ:
                واپس -EINVAL;
        }

        t++;
        r = undecchar(*t);
        اگر (r < 0)
                واپس r;

        usec_t u = r * 10 * USEC_فی_HOUR;

        t++;
        r = undecchar(*t);
        اگر (r < 0)
                واپس r;
        u += r * USEC_فی_HOUR;

        t++;
        اگر (*t == '\0') /* 2 digits case */
                جاؤ finalize;

        اگر (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        اگر (r < 0)
                واپس r;
        اگر (r >= 6) /* refuse minutes equal to or larger than 60 */
                واپس -EINVAL;

        u += r * 10 * USEC_فی_MINUTE;

        t++;
        r = undecchar(*t);
        اگر (r < 0)
                واپس r;

        u += r * USEC_فی_MINUTE;

        t++;
        اگر (*t != '\0')
                واپس -EINVAL;

finalize:
        اگر (u > USEC_فی_DAY) /* refuse larger than one day */
                واپس -EINVAL;

        اگر (ret) {
                طویل gmtoff = u / USEC_فی_SEC;
                *ret = مثبت ? gmtoff : -gmtoff;
        }

        واپس 0;
#اگر_ختم
}

ساکن صحیح parse_timestamp_impl(
                ثابت حرف *t,
                size_t max_len,
                bool utc,
                صحیح isdst,
                طویل gmtoff,
                usec_t *ret) {

        ساکن ثابت ساخت {
                ثابت حرف *name;
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

        _cleanup_free_ حرف *t_alloc = تہی;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        صحیح r, weekday = -1;
        غیرعلامتی fractional = 0;
        ثابت حرف *k;
        ساخت tm tm, copy;

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
                        واپس -EINVAL;

                t_alloc = strndup(t, max_len);
                اگر (!t_alloc)
                        واپس -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        اگر (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                اگر ((usec_t) labs(gmtoff) * USEC_فی_SEC > USEC_فی_DAY)
                        واپس -EINVAL;
        } ورنہ {
                اگر (gmtoff != 0)
                        واپس -EINVAL;
        }

        اگر (t[0] == '@' && !with_tz)
                واپس parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        اگر (!with_tz) {
                اگر (streq(t, "now"))
                        جاؤ اختتام;

                اگر (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        اگر (r < 0)
                                واپس r;

                        جاؤ اختتام;
                }

                اگر (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        اگر (r < 0)
                                واپس r;

                        جاؤ اختتام;
                }

                اگر ((k = endswith(t, " ago"))) {
                        _cleanup_free_ حرف *buf = تہی;

                        buf = strndup(t, k - t);
                        اگر (!buf)
                                واپس -ENOMEM;

                        r = parse_sec(buf, &minus);
                        اگر (r < 0)
                                واپس r;

                        جاؤ اختتام;
                }

                اگر ((k = endswith(t, " left"))) {
                        _cleanup_free_ حرف *buf = تہی;

                        buf = strndup(t, k - t);
                        اگر (!buf)
                                واپس -ENOMEM;

                        r = parse_sec(buf, &plus);
                        اگر (r < 0)
                                واپس r;

                        جاؤ اختتام;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        اگر (r < 0)
                واپس r;

        tm.tm_isdst = isdst;

        اگر (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                جاؤ from_tm;

        } ورنہ اگر (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                جاؤ from_tm;

        } ورنہ اگر (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                جاؤ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                اگر (!k || *k != ' ')
                        جاری;

#اگر_تعریف_شدہ __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#اگر_ختم
                t = k + 1;
                توڑو;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        جاؤ parse_usec;
                ورنہ اگر (*k == 0)
                        جاؤ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        جاؤ parse_usec;
                ورنہ اگر (*k == 0)
                        جاؤ from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        جاؤ parse_usec;
                ورنہ اگر (*k == 0)
                        جاؤ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        جاؤ parse_usec;
                ورنہ اگر (*k == 0)
                        جاؤ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = 0;
                جاؤ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = 0;
                جاؤ from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = 0;
                جاؤ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                جاؤ from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                جاؤ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        اگر (k) {
                اگر (*k == '.')
                        جاؤ parse_usec;
                ورنہ اگر (*k == 0)
                        جاؤ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        اگر (k && *k == 0) {
                tm.tm_sec = 0;
                جاؤ from_tm;
        }

        واپس -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        اگر (r < 0)
                واپس -EINVAL;
        اگر (*k != '\0')
                واپس -EINVAL;

from_tm:
        أكد(plus == 0);
        أكد(minus == 0);

        اگر (weekday >= 0 && tm.tm_wday != weekday)
                واپس -EINVAL;

        اگر (gmtoff < 0) {
                plus = -gmtoff * USEC_فی_SEC;

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
                        minus = USEC_فی_DAY;
                }
        } ورنہ
                minus = gmtoff * USEC_فی_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        اگر (r < 0)
                واپس r;

        usec = usec_جوڑو(usec, fractional);

اختتام:
        usec = usec_جوڑو(usec, plus);

        اگر (usec < minus)
                واپس -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        اگر (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                واپس -EINVAL;

        اگر (ret)
                *ret = usec;
        واپس 0;
}

صحیح parse_timestamp(ثابت حرف *t, usec_t *ret) {
        طویل gmtoff;
        صحیح r;

        أكد(t);

        size_t t_len = لڑی_لمبائی(t);
        اگر (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                اگر (r >= 0)
                        واپس r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        اگر (t_len > 7 && اندر_سیٹ(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                واپس parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ثابت حرف *tz = strrchr(t, ' ');
        اگر (!tz)
                واپس parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        اگر (streq(tz, "UTC"))
                واپس parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        اگر (parse_gmtoff(tz, &gmtoff) >= 0)
                واپس parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        برائے (صحیح j = 0; j <= 1; j++) {
                اگر (!streq_ptr(tz, get_tzname(j)))
                        جاری;

                /* The specified timezone matches tzname[] of the local timezone. */
                واپس parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        اگر (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                اگر (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        واپس negative_errno();

                واپس parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        واپس parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

ساکن ثابت حرف* نکالو_multiplier(ثابت حرف *p, usec_t *ret) {
        ساکن ثابت ساخت {
                ثابت حرف *suffix;
                usec_t usec;
        } جدول[] = {
                { "seconds", USEC_فی_SEC    },
                { "second",  USEC_فی_SEC    },
                { "sec",     USEC_فی_SEC    },
                { "s",       USEC_فی_SEC    },
                { "minutes", USEC_فی_MINUTE },
                { "minute",  USEC_فی_MINUTE },
                { "min",     USEC_فی_MINUTE },
                { "months",  USEC_فی_MONTH  },
                { "month",   USEC_فی_MONTH  },
                { "M",       USEC_فی_MONTH  },
                { "msec",    USEC_فی_MSEC   },
                { "ms",      USEC_فی_MSEC   },
                { "m",       USEC_فی_MINUTE },
                { "hours",   USEC_فی_HOUR   },
                { "hour",    USEC_فی_HOUR   },
                { "hr",      USEC_فی_HOUR   },
                { "h",       USEC_فی_HOUR   },
                { "days",    USEC_فی_DAY    },
                { "day",     USEC_فی_DAY    },
                { "d",       USEC_فی_DAY    },
                { "weeks",   USEC_فی_WEEK   },
                { "week",    USEC_فی_WEEK   },
                { "w",       USEC_فی_WEEK   },
                { "years",   USEC_فی_YEAR   },
                { "year",    USEC_فی_YEAR   },
                { "y",       USEC_فی_YEAR   },
                { "usec",    1ULL            },
                { "us",      1ULL            },
                { "μs",      1ULL            }, /* U+03bc (aka GREEK SMALL LETTER MU) */
                { "µs",      1ULL            }, /* U+b5 (aka MICRO SIGN) */
        };

        أكد(p);
        أكد(ret);

        FOREACH_ELEMENT(i, جدول) {
                ثابت حرف *e = startswith(p, i->suffix);
                اگر (e) {
                        *ret = i->usec;
                        واپس e;
                }
        }

        واپس p;
}

صحیح parse_وقت(ثابت حرف *t, usec_t *ret, usec_t default_unit) {
        ثابت حرف *p, *s;

        أكد(t);
        أكد(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ تہی);
        s = startswith(p, "infinity");
        اگر (s) {
                اگر (!اندر_charset(s, WHITESPACE))
                        واپس -EINVAL;

                اگر (ret)
                        *ret = USEC_INFINITY;
                واپس 0;
        }

        usec_t usec = 0;

        برائے (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                طویل طویل l;
                حرف *e;

                p = skip_leading_chars(p, /* bad= */ تہی);
                اگر (*p == 0) {
                        اگر (!something)
                                واپس -EINVAL;

                        توڑو;
                }

                اگر (*p == '-') /* Don't allow "-0" */
                        واپس -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                اگر (errno > 0)
                        واپس -errno;
                اگر (l < 0)
                        واپس -ERANGE;

                اگر (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } ورنہ اگر (e == p)
                        واپس -EINVAL;
                ورنہ
                        p = e;

                s = نکالو_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                اگر (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        واپس -EINVAL;

                p = s;

                اگر ((usec_t) l >= USEC_INFINITY / multiplier)
                        واپس -ERANGE;

                k = (usec_t) l * multiplier;
                اگر (k >= USEC_INFINITY - usec)
                        واپس -ERANGE;

                usec += k;

                something = true;

                اگر (*e == '.') {
                        usec_t m = multiplier / 10;
                        ثابت حرف *b;

                        برائے (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                اگر (k >= USEC_INFINITY - usec)
                                        واپس -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        اگر (b == e + 1)
                                واپس -EINVAL;
                }
        }

        اگر (ret)
                *ret = usec;
        واپس 0;
}

صحیح parse_sec(ثابت حرف *t, usec_t *ret) {
        واپس parse_وقت(t, ret, USEC_فی_SEC);
}

صحیح parse_sec_fix_0(ثابت حرف *t, usec_t *ret) {
        usec_t k;
        صحیح r;

        أكد(t);
        أكد(ret);

        r = parse_sec(t, &k);
        اگر (r < 0)
                واپس r;

        *ret = k == 0 ? USEC_INFINITY : k;
        واپس r;
}

صحیح parse_sec_def_infinity(ثابت حرف *t, usec_t *ret) {
        أكد(t);
        أكد(ret);

        t += strspn(t, WHITESPACE);
        اگر (isempty(t)) {
                *ret = USEC_INFINITY;
                واپس 0;
        }
        واپس parse_sec(t, ret);
}

ساکن ثابت حرف* نکالو_nsec_multiplier(ثابت حرف *p, nsec_t *ret) {
        ساکن ثابت ساخت {
                ثابت حرف *suffix;
                nsec_t nsec;
        } جدول[] = {
                { "seconds", NSEC_فی_SEC    },
                { "second",  NSEC_فی_SEC    },
                { "sec",     NSEC_فی_SEC    },
                { "s",       NSEC_فی_SEC    },
                { "minutes", NSEC_فی_MINUTE },
                { "minute",  NSEC_فی_MINUTE },
                { "min",     NSEC_فی_MINUTE },
                { "months",  NSEC_فی_MONTH  },
                { "month",   NSEC_فی_MONTH  },
                { "M",       NSEC_فی_MONTH  },
                { "msec",    NSEC_فی_MSEC   },
                { "ms",      NSEC_فی_MSEC   },
                { "m",       NSEC_فی_MINUTE },
                { "hours",   NSEC_فی_HOUR   },
                { "hour",    NSEC_فی_HOUR   },
                { "hr",      NSEC_فی_HOUR   },
                { "h",       NSEC_فی_HOUR   },
                { "days",    NSEC_فی_DAY    },
                { "day",     NSEC_فی_DAY    },
                { "d",       NSEC_فی_DAY    },
                { "weeks",   NSEC_فی_WEEK   },
                { "week",    NSEC_فی_WEEK   },
                { "w",       NSEC_فی_WEEK   },
                { "years",   NSEC_فی_YEAR   },
                { "year",    NSEC_فی_YEAR   },
                { "y",       NSEC_فی_YEAR   },
                { "usec",    NSEC_فی_USEC   },
                { "us",      NSEC_فی_USEC   },
                { "μs",      NSEC_فی_USEC   }, /* U+03bc (aka GREEK LETTER MU) */
                { "µs",      NSEC_فی_USEC   }, /* U+b5 (aka MICRO SIGN) */
                { "nsec",    1ULL            },
                { "ns",      1ULL            },
                { "",        1ULL            }, /* default is nsec */
        };

        أكد(p);
        أكد(ret);

        FOREACH_ELEMENT(i, جدول) {
                ثابت حرف *e = startswith(p, i->suffix);
                اگر (e) {
                        *ret = i->nsec;
                        واپس e;
                }
        }

        واپس p;
}

صحیح parse_nsec(ثابت حرف *t, nsec_t *ret) {
        ثابت حرف *p, *s;
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
                        واپس -EINVAL;

                *ret = NSEC_INFINITY;
                واپس 0;
        }

        برائے (;;) {
                nsec_t multiplier = 1, k;
                طویل طویل l;
                حرف *e;

                p += strspn(p, WHITESPACE);

                اگر (*p == 0) {
                        اگر (!something)
                                واپس -EINVAL;

                        توڑو;
                }

                اگر (*p == '-') /* Don't allow "-0" */
                        واپس -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                اگر (errno > 0)
                        واپس -errno;
                اگر (l < 0)
                        واپس -ERANGE;

                اگر (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } ورنہ اگر (e == p)
                        واپس -EINVAL;
                ورنہ
                        p = e;

                s = نکالو_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                اگر (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        واپس -EINVAL;

                p = s;

                اگر ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        واپس -ERANGE;

                k = (nsec_t) l * multiplier;
                اگر (k >= NSEC_INFINITY - nsec)
                        واپس -ERANGE;

                nsec += k;

                something = true;

                اگر (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ثابت حرف *b;

                        برائے (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                اگر (k >= NSEC_INFINITY - nsec)
                                        واپس -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        اگر (b == e + 1)
                                واپس -EINVAL;
                }
        }

        *ret = nsec;

        واپس 0;
}

ساکن صحیح get_timezones_from_zone1970_tab(حرف ***ret) {
        _cleanup_fclose_ FILE *f = تہی;
        _cleanup_strv_free_ حرف **zones = تہی;
        صحیح r;

        أكد(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        اگر (!f)
                واپس -errno;

        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی, *cc = تہی, *co = تہی, *tz = تہی;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                ثابت حرف *p = سطر;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = نکالو_many_الفاظ(&p, تہی, 0, &cc, &co, &tz);
                اگر (r < 0)
                        جاری;

                /* Lines that start with # are comments. */
                اگر (*cc == '#')
                        جاری;

                اگر (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        جاری;

                r = strv_extend(&zones, tz);
                اگر (r < 0)
                        واپس r;
        }

        *ret = TAKE_PTR(zones);
        واپس 0;
}

ساکن صحیح get_timezones_from_tzdata_zi(حرف ***ret) {
        _cleanup_fclose_ FILE *f = تہی;
        _cleanup_strv_free_ حرف **zones = تہی;
        صحیح r;

        أكد(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        اگر (!f)
                واپس -errno;

        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی, *type = تہی, *f1 = تہی, *f2 = تہی;

                r = read_سطر(f, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                ثابت حرف *p = سطر;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = نکالو_many_الفاظ(&p, تہی, 0, &type, &f1, &f2);
                اگر (r < 0)
                        جاری;

                حرف *tz;
                اگر (اندر_سیٹ(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                ورنہ اگر (اندر_سیٹ(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                ورنہ
                        /* Not a line we care about. */
                        جاری;

                اگر (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        جاری;

                r = strv_extend(&zones, tz);
                اگر (r < 0)
                        واپس r;
        }

        *ret = TAKE_PTR(zones);
        واپس 0;
}

صحیح get_timezones(حرف ***ret) {
        _cleanup_strv_free_ حرف **zones = تہی;
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
                واپس r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        اگر (r < 0)
                واپس r;

        strv_ترتیب_دو_uniq(zones);

        *ret = TAKE_PTR(zones);
        واپس 0;
}

صحیح verify_timezone(ثابت حرف *name, صحیح log_level) {
        bool slash = false;
        ثابت حرف *p, *t;
        _cleanup_close_ صحیح fd = -EBADF;
        حرف buf[4];
        صحیح r;

        اگر (isempty(name))
                واپس -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        اگر (streq(name, "UTC"))
                واپس 0;

        اگر (name[0] == '/')
                واپس -EINVAL;

        برائے (p = name; *p; p++) {
                اگر (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !اندر_سیٹ(*p, '-', '_', '+', '/'))
                        واپس -EINVAL;

                اگر (*p == '/') {

                        اگر (slash)
                                واپس -EINVAL;

                        slash = true;
                } ورنہ
                        slash = false;
        }

        اگر (slash)
                واپس -EINVAL;

        اگر (p - name >= PATH_MAX)
                واپس -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        اگر (fd < 0)
                واپس log_بھرا_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        اگر (r < 0)
                واپس log_بھرا_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        اگر (r < 0)
                واپس log_بھرا_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        اگر (memcmp(buf, "TZif", 4) != 0)
                واپس log_بھرا_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        واپس 0;
}

خالی reset_timezonep(حرف **p) {
        أكد(p);

        (خالی) سیٹ_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

حرف* save_timezone(خالی) {
        ثابت حرف *e = getenv("TZ");
        اگر (!e)
                واپس تہی;

        حرف *s = strdup(e);
        اگر (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        واپس s;
}

bool clock_supported(clockid_t clock) {
        ساخت timespec ts;

        انتخاب (clock) {

        حالت CLOCK_MONOTONIC:
        حالت CLOCK_REALTIME:
        حالت CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                واپس true;

        طےشدہ:
                /* For everything else, check properly */
                واپس clock_gettime(clock, &ts) >= 0;
        }
}

صحیح get_timezone(حرف **ret) {
        _cleanup_free_ حرف *t = تہی;
        صحیح r;

        أكد(ret);

        r = readlink_malloc(etc_localtime(), &t);
        اگر (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                واپس strdup_مقصد(ret, "UTC");
        اگر (r < 0)
                واپس r; /* Return EINVAL if not a symlink */

        ثابت حرف *e = راستہ_STARTSWITH_سیٹ(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        اگر (!e)
                واپس -EINVAL;
        اگر (!timezone_is_valid(e, LOG_DEBUG))
                واپس -EINVAL;

        واپس strdup_مقصد(ret, e);
}

صحیح get_timezone_prefer_env(حرف **ret) {
        أكد(ret);

        ثابت حرف *e = getenv("TZ");
        اگر (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                واپس strdup_مقصد(ret, e + 1);

        واپس get_timezone(ret);
}

ثابت حرف* etc_localtime(خالی) {
        ساکن ثابت حرف *cached = تہی;

        اگر (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        واپس cached;
}

صحیح mktime_or_timegm_usec(
                ساخت tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        أكد(tm);

        اگر (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                واپس -ERANGE;
        اگر ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_فی_YEAR, (usec_t) وقت_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                واپس -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        اگر (t < 0) /* Refuse negative times and errors */
                واپس -ERANGE;
        اگر ((usec_t) t >= USEC_INFINITY / USEC_فی_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                واپس -ERANGE;

        اگر (ret)
                *ret = (usec_t) t * USEC_فی_SEC;
        واپس 0;
}

صحیح localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                ساخت tm *ret) {

        t /= USEC_فی_SEC; /* Round down */
        اگر (t > (usec_t) وقت_T_MAX)
                واپس -ERANGE;
        time_t sec = (time_t) t;

        ساخت tm buf = {};
        اگر (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                واپس -EINVAL;

        اگر (ret)
                *ret = buf;

        واپس 0;
}

uint64_t sysconf_clock_ticks_cached(خالی) {
        ساکن thread_local uint64_t hz = 0;

        اگر (hz != 0)
                واپس hz;

        طویل t = sysconf(_SC_CLK_TCK);
        أكد(t > 0);
        hz = (uint64_t) t;

        واپس hz;
}

uint32_t usec_مقصد_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        واپس DIV_باری_اوپر(u, USEC_فی_SEC / hz);
}

usec_t jiffies_مقصد_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        واپس DIV_باری_اوپر(j * USEC_فی_SEC, hz);
}

usec_t usec_سرکاؤ_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        اگر (x == USEC_INFINITY)
                واپس USEC_INFINITY;
        اگر (map_clock_id(from) == map_clock_id(to))
                واپس x;

        a = now(from);
        b = now(to);

        اگر (x > a)
                /* x lies in the future */
                واپس usec_جوڑو(b, usec_sub_unsigned(x, a));
        ورنہ
                /* x lies in the past */
                واپس usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool اندر_utc_timezone(خالی) {
        tzset();

        واپس timezone == 0 && daylight == 0;
}

صحیح usleep_محفوظ(usec_t usec) {
        صحیح r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        اگر (usec == 0)
                واپس 0;

        اگر (usec == USEC_INFINITY)
                واپس RET_NERRNO(pause());

        ساخت timespec t;
        timespec_store(&t, usec);

        برائے (;;) {
                ساخت timespec بقایا;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &بقایا);
                اگر (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = بقایا;
                        جاری;
                }

                واپس r;
        }
}

صحیح وقت_change_fd(خالی) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        ساکن ثابت ساخت itimerspec its = {
                .it_value.tv_sec = وقت_T_MAX,
        };

        _cleanup_close_ صحیح fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        اگر (fd < 0)
                واپس -errno;

        اگر (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, تہی) >= 0)
                واپس TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#اگر SIZEOF_وقت_T == 8 && ULONG_MAX < UINT64_MAX
        اگر (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                ساکن ثابت ساخت itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                اگر (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, تہی) >= 0)
                        واپس TAKE_FD(fd);
        }
#اگر_ختم

        واپس -errno;
}

ساکن ثابت حرف* ثابت timestamp_style_جدول[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_جدول_LOOKUP_مقصد_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ثابت حرف *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_جدول_lookup_from_string(timestamp_style_جدول, ELEMENTSOF(timestamp_style_جدول), s);
        اگر (t >= 0)
                واپس t;
        اگر (STRPTR_اندر_سیٹ(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                واپس TIMESTAMP_US;
        اگر (STRPTR_اندر_سیٹ(s, "µs+utc", "μs+utc"))
                واپس TIMESTAMP_US_UTC;
        واپس t;
}

صحیح parse_calendar_date_بھرا(ثابت حرف *s, bool allow_pre_epoch, usec_t *ret_usec, ساخت tm *ret_tm) {
        ساخت tm parsed_tm = {}, copy_tm;
        ثابت حرف *k;
        صحیح r;

        أكد(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        اگر (!k || *k)
                واپس -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        اگر (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                اگر (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        واپس -EINVAL;
        } ورنہ {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                اگر (r < 0)
                        واپس r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        اگر (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                واپس -EINVAL;

        اگر (ret_usec)
                *ret_usec = usec;
        اگر (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        واپس 0;
}
