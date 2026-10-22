/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/time_types.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/timerfd.h>
#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "hexdecoct.h"          /* IWYU pragma: keep */
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "time-util.h"

സ്ഥിതം clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        തിരഞ്ഞെടുപ്പ് (c) {

        അവസ്ഥ CLOCK_BOOTTIME_ALARM:
                തിരികെ CLOCK_BOOTTIME;

        അവസ്ഥ CLOCK_REALTIME_ALARM:
                തിരികെ CLOCK_REALTIME;

        സ്ഥിരസ്ഥിതി:
                തിരികെ c;
        }
}

usec_t now(clockid_t clock_id) {
        ഘടന timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        പ്രതിജ്ഞാ(n > 0);
        പ്രതിജ്ഞാ(n < USEC_INFINITY);

        തിരികെ n;
}

nsec_t now_nsec(clockid_t clock_id) {
        ഘടന timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        പ്രതിജ്ഞാ(n > 0);
        പ്രതിജ്ഞാ(n < NSEC_INFINITY);

        തിരികെ n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        പ്രതിജ്ഞാ(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        തിരികെ ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        പ്രതിജ്ഞാ(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        തിരികെ ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        എങ്കിൽ (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                എങ്കിൽ (to_base >= USEC_INFINITY - delta) /* overflow? */
                        തിരികെ USEC_INFINITY;

                തിരികെ to_base + delta;

        } അല്ലെങ്കിൽ { /* In the past */
                usec_t delta = from_base - from;

                എങ്കിൽ (to_base <= delta) /* underflow? */
                        തിരികെ 0;

                തിരികെ to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        എങ്കിൽ (map_clock_id(from_clock) == map_clock_id(to_clock))
                തിരികെ from;

        /* Keep infinity as is */
        എങ്കിൽ (from == USEC_INFINITY)
                തിരികെ from;

        തിരികെ map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                തിരികെ ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        തിരികെ ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                തിരികെ ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        തിരികെ ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                തിരികെ ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        തിരികെ ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                തിരികെ ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        തിരികെ ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                തിരികെ ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        തിരികെ ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        പ്രതിജ്ഞാ(ts);

        തിരഞ്ഞെടുപ്പ് (clock) {

        അവസ്ഥ CLOCK_REALTIME:
        അവസ്ഥ CLOCK_REALTIME_ALARM:
                തിരികെ ts->realtime;

        അവസ്ഥ CLOCK_MONOTONIC:
                തിരികെ ts->monotonic;

        അവസ്ഥ CLOCK_BOOTTIME:
        അവസ്ഥ CLOCK_BOOTTIME_ALARM:
                തിരികെ ts->boottime;

        സ്ഥിരസ്ഥിതി:
                തിരികെ USEC_INFINITY;
        }
}

usec_t timespec_load(സ്ഥിരം ഘടന timespec *ts) {
        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                തിരികെ USEC_INFINITY;

        എങ്കിൽ ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                തിരികെ USEC_INFINITY;

        തിരികെ
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(സ്ഥിരം ഘടന timespec *ts) {
        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                തിരികെ NSEC_INFINITY;

        എങ്കിൽ ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                തിരികെ NSEC_INFINITY;

        തിരികെ (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

ഘടന timespec *timespec_store(ഘടന timespec *ts, usec_t u) {
        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                തിരികെ ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        തിരികെ ts;
}

ഘടന timespec *timespec_store_nsec(ഘടന timespec *ts, nsec_t n) {
        പ്രതിജ്ഞാ(ts);

        എങ്കിൽ (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                തിരികെ ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        തിരികെ ts;
}

ഘടന __kernel_timespec *kernel_timespec_store(ഘടന __kernel_timespec *ts, usec_t u) {
        പ്രതിജ്ഞാ(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        പ്രതിജ്ഞാ(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        തിരികെ ts;
}

usec_t timeval_load(സ്ഥിരം ഘടന timeval *tv) {
        പ്രതിജ്ഞാ(tv);

        എങ്കിൽ (tv->tv_sec < 0 || tv->tv_usec < 0)
                തിരികെ USEC_INFINITY;

        എങ്കിൽ ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                തിരികെ USEC_INFINITY;

        തിരികെ
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

ഘടന timeval *timeval_store(ഘടന timeval *tv, usec_t u) {
        പ്രതിജ്ഞാ(tv);

        എങ്കിൽ (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } അല്ലെങ്കിൽ {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        തിരികെ tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, പൂർണ്ണസംഖ്യ);

അക്ഷരം* format_timestamp_style(
                അക്ഷരം *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        ഘടന tm tm;
        bool utc, us;
        size_t n;

        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(style >= 0);
        പ്രതിജ്ഞാ(style < _TIMESTAMP_STYLE_MAX);

        എങ്കിൽ (!timestamp_is_set(t))
                തിരികെ ശൂന്യ; /* Timestamp is unset */

        എങ്കിൽ (style == TIMESTAMP_UNIX) {
                എങ്കിൽ (l < (size_t) (1 + 1 + 1))
                        തിരികെ ശൂന്യ; /* not enough space for even the shortest of forms */

                തിരികെ snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        എങ്കിൽ (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                തിരികെ ശൂന്യ; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        എങ്കിൽ (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                പ്രതിജ്ഞാ(l >= strlen(xxx[style]) + 1);
                തിരികെ strcpy(buf, xxx[style]);
        }

        എങ്കിൽ (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                തിരികെ ശൂന്യ;

        /* Start with the week day */
        സ്ഥിരം അക്ഷരം *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        പ്രതിജ്ഞാ(weekday);
        memcpy(buf, weekday, 4);

        എങ്കിൽ (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                എങ്കിൽ (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        തിരികെ ശൂന്യ; /* Doesn't fit */

                തിരികെ buf;
        }

        /* Add the main components */
        എങ്കിൽ (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                തിരികെ ശൂന്യ; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        എങ്കിൽ (us) {
                n = strlen(buf);
                എങ്കിൽ (n + 8 > l)
                        തിരികെ ശൂന്യ; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        എങ്കിൽ (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                എങ്കിൽ (n + 5 > l)
                        തിരികെ ശൂന്യ; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } അല്ലെങ്കിൽ എങ്കിൽ (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                എങ്കിൽ (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        എങ്കിൽ (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                തിരികെ ശൂന്യ; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } അല്ലെങ്കിൽ {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        തിരികെ buf;
}

അക്ഷരം* format_timestamp_relative_full(അക്ഷരം *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ഇടത്) {
        സ്ഥിരം അക്ഷരം *s;
        usec_t n, d;

        പ്രതിജ്ഞാ(buf);

        എങ്കിൽ (!timestamp_is_set(t))
                തിരികെ ശൂന്യ;

        n = now(clock);
        എങ്കിൽ (n > t) {
                d = n - t;
                s = " ago";
        } അല്ലെങ്കിൽ {
                d = t - n;
                s = implicit_ഇടത് ? "" : " left";
        }

        എങ്കിൽ (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (ശൂന്യം) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } അല്ലെങ്കിൽ എങ്കിൽ (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (ശൂന്യം) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } അല്ലെങ്കിൽ എങ്കിൽ (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (ശൂന്യം) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } അല്ലെങ്കിൽ എങ്കിൽ (d >= 2*USEC_PER_DAY)
                (ശൂന്യം) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        അല്ലെങ്കിൽ എങ്കിൽ (d >= 25*USEC_PER_HOUR)
                (ശൂന്യം) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        അല്ലെങ്കിൽ എങ്കിൽ (d >= 6*USEC_PER_HOUR)
                (ശൂന്യം) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        അല്ലെങ്കിൽ എങ്കിൽ (d >= USEC_PER_HOUR)
                (ശൂന്യം) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        അല്ലെങ്കിൽ എങ്കിൽ (d >= 5*USEC_PER_MINUTE)
                (ശൂന്യം) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        അല്ലെങ്കിൽ എങ്കിൽ (d >= USEC_PER_MINUTE)
                (ശൂന്യം) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        അല്ലെങ്കിൽ എങ്കിൽ (d >= USEC_PER_SEC)
                (ശൂന്യം) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        അല്ലെങ്കിൽ എങ്കിൽ (d >= USEC_PER_MSEC)
                (ശൂന്യം) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        അല്ലെങ്കിൽ എങ്കിൽ (d > 0)
                (ശൂന്യം) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        അല്ലെങ്കിൽ
                (ശൂന്യം) snprintf(buf, l, "now");

        buf[l-1] = 0;
        തിരികെ buf;
}

അക്ഷരം* format_timespan(അക്ഷരം *buf, size_t l, usec_t t, usec_t accuracy) {
        സ്ഥിതം സ്ഥിരം ഘടന {
                സ്ഥിരം അക്ഷരം *suffix;
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

        അക്ഷരം *p = ASSERT_PTR(buf);
        bool something = false;

        പ്രതിജ്ഞാ(l > 0);

        എങ്കിൽ (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                തിരികെ p;
        }

        എങ്കിൽ (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                തിരികെ p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                പൂർണ്ണസംഖ്യ k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                എങ്കിൽ (t <= 0)
                        നിർത്തുക;

                എങ്കിൽ (t < accuracy && something)
                        നിർത്തുക;

                എങ്കിൽ (t < i->usec)
                        തുടരുക;

                എങ്കിൽ (l <= 1)
                        നിർത്തുക;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                എങ്കിൽ (t < USEC_PER_MINUTE && b > 0) {
                        signed അക്ഷരം j = 0;

                        വേണ്ടി (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        വേണ്ടി (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        എങ്കിൽ (j > 0) {
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
                എങ്കിൽ (!done) {
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

        തിരികെ buf;
}

സ്ഥിരം അക്ഷരം* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        എങ്കിൽ (dst && isempty(tzname[1]))
                dst = false;

        തിരികെ empty_to_null(tzname[dst]);
}

പൂർണ്ണസംഖ്യ parse_gmtoff(സ്ഥിരം അക്ഷരം *t, long *ret) {
        പ്രതിജ്ഞാ(t);

        ഘടന tm tm;
        സ്ഥിരം അക്ഷരം *k = strptime(t, "%z", &tm);
        എങ്കിൽ (k && *k == '\0') {
                /* Success! */
                എങ്കിൽ (ret)
                        *ret = tm.tm_gmtoff;
                തിരികെ 0;
        }

#ifdef __GLIBC__
        തിരികെ -EINVAL;
#അല്ലെങ്കിൽ
        പൂർണ്ണസംഖ്യ r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        തിരഞ്ഞെടുപ്പ് (*t) {
        അവസ്ഥ '+':
                positive = true;
                നിർത്തുക;
        അവസ്ഥ '-':
                positive = false;
                നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EINVAL;
        }

        t++;
        r = undecchar(*t);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        u += r * USEC_PER_HOUR;

        t++;
        എങ്കിൽ (*t == '\0') /* 2 digits case */
                പോകുക finalize;

        എങ്കിൽ (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r >= 6) /* refuse minutes equal to or larger than 60 */
                തിരികെ -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        u += r * USEC_PER_MINUTE;

        t++;
        എങ്കിൽ (*t != '\0')
                തിരികെ -EINVAL;

finalize:
        എങ്കിൽ (u > USEC_PER_DAY) /* refuse larger than one day */
                തിരികെ -EINVAL;

        എങ്കിൽ (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        തിരികെ 0;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_timestamp_impl(
                സ്ഥിരം അക്ഷരം *t,
                size_t max_len,
                bool utc,
                പൂർണ്ണസംഖ്യ isdst,
                long gmtoff,
                usec_t *ret) {

        സ്ഥിതം സ്ഥിരം ഘടന {
                സ്ഥിരം അക്ഷരം *name;
                സ്ഥിരം പൂർണ്ണസംഖ്യ nr;
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

        _cleanup_free_ അക്ഷരം *t_alloc = ശൂന്യ;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        പൂർണ്ണസംഖ്യ r, weekday = -1;
        unsigned fractional = 0;
        സ്ഥിരം അക്ഷരം *k;
        ഘടന tm tm, copy;

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

        പ്രതിജ്ഞാ(t);

        എങ്കിൽ (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                എങ്കിൽ (max_len == 0) /* Can't be the only field */
                        തിരികെ -EINVAL;

                t_alloc = strndup(t, max_len);
                എങ്കിൽ (!t_alloc)
                        തിരികെ -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        എങ്കിൽ (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                എങ്കിൽ ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        തിരികെ -EINVAL;
        } അല്ലെങ്കിൽ {
                എങ്കിൽ (gmtoff != 0)
                        തിരികെ -EINVAL;
        }

        എങ്കിൽ (t[0] == '@' && !with_tz)
                തിരികെ parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        എങ്കിൽ (!with_tz) {
                എങ്കിൽ (streq(t, "now"))
                        പോകുക finish;

                എങ്കിൽ (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        പോകുക finish;
                }

                എങ്കിൽ (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        പോകുക finish;
                }

                എങ്കിൽ ((k = endswith(t, " ago"))) {
                        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;

                        buf = strndup(t, k - t);
                        എങ്കിൽ (!buf)
                                തിരികെ -ENOMEM;

                        r = parse_sec(buf, &minus);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        പോകുക finish;
                }

                എങ്കിൽ ((k = endswith(t, " left"))) {
                        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;

                        buf = strndup(t, k - t);
                        എങ്കിൽ (!buf)
                                തിരികെ -ENOMEM;

                        r = parse_sec(buf, &plus);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        പോകുക finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        tm.tm_isdst = isdst;

        എങ്കിൽ (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                പോകുക from_tm;

        } അല്ലെങ്കിൽ എങ്കിൽ (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                പോകുക from_tm;

        } അല്ലെങ്കിൽ എങ്കിൽ (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                പോകുക from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                എങ്കിൽ (!k || *k != ' ')
                        തുടരുക;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                നിർത്തുക;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        എങ്കിൽ (k) {
                എങ്കിൽ (*k == '.')
                        പോകുക parse_usec;
                അല്ലെങ്കിൽ എങ്കിൽ (*k == 0)
                        പോകുക from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        എങ്കിൽ (k) {
                എങ്കിൽ (*k == '.')
                        പോകുക parse_usec;
                അല്ലെങ്കിൽ എങ്കിൽ (*k == 0)
                        പോകുക from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        എങ്കിൽ (k) {
                എങ്കിൽ (*k == '.')
                        പോകുക parse_usec;
                അല്ലെങ്കിൽ എങ്കിൽ (*k == 0)
                        പോകുക from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        എങ്കിൽ (k) {
                എങ്കിൽ (*k == '.')
                        പോകുക parse_usec;
                അല്ലെങ്കിൽ എങ്കിൽ (*k == 0)
                        പോകുക from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        എങ്കിൽ (k && *k == 0) {
                tm.tm_sec = 0;
                പോകുക from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        എങ്കിൽ (k && *k == 0) {
                tm.tm_sec = 0;
                പോകുക from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        എങ്കിൽ (k && *k == 0) {
                tm.tm_sec = 0;
                പോകുക from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        എങ്കിൽ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                പോകുക from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        എങ്കിൽ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                പോകുക from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        എങ്കിൽ (k) {
                എങ്കിൽ (*k == '.')
                        പോകുക parse_usec;
                അല്ലെങ്കിൽ എങ്കിൽ (*k == 0)
                        പോകുക from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        എങ്കിൽ (k && *k == 0) {
                tm.tm_sec = 0;
                പോകുക from_tm;
        }

        തിരികെ -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        എങ്കിൽ (r < 0)
                തിരികെ -EINVAL;
        എങ്കിൽ (*k != '\0')
                തിരികെ -EINVAL;

from_tm:
        പ്രതിജ്ഞാ(plus == 0);
        പ്രതിജ്ഞാ(minus == 0);

        എങ്കിൽ (weekday >= 0 && tm.tm_wday != weekday)
                തിരികെ -EINVAL;

        എങ്കിൽ (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                എങ്കിൽ (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } അല്ലെങ്കിൽ
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        എങ്കിൽ (usec < minus)
                തിരികെ -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        എങ്കിൽ (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                തിരികെ -EINVAL;

        എങ്കിൽ (ret)
                *ret = usec;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_timestamp(സ്ഥിരം അക്ഷരം *t, usec_t *ret) {
        long gmtoff;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(t);

        size_t t_len = strlen(t);
        എങ്കിൽ (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                എങ്കിൽ (r >= 0)
                        തിരികെ r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        എങ്കിൽ (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                തിരികെ parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        സ്ഥിരം അക്ഷരം *tz = strrchr(t, ' ');
        എങ്കിൽ (!tz)
                തിരികെ parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        എങ്കിൽ (streq(tz, "UTC"))
                തിരികെ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        എങ്കിൽ (parse_gmtoff(tz, &gmtoff) >= 0)
                തിരികെ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j <= 1; j++) {
                എങ്കിൽ (!streq_ptr(tz, get_tzname(j)))
                        തുടരുക;

                /* The specified timezone matches tzname[] of the local timezone. */
                തിരികെ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        എങ്കിൽ (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                എങ്കിൽ (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        തിരികെ negative_errno();

                തിരികെ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        തിരികെ parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* extract_multiplier(സ്ഥിരം അക്ഷരം *p, usec_t *ret) {
        സ്ഥിതം സ്ഥിരം ഘടന {
                സ്ഥിരം അക്ഷരം *suffix;
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

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        FOREACH_ELEMENT(i, table) {
                സ്ഥിരം അക്ഷരം *e = startswith(p, i->suffix);
                എങ്കിൽ (e) {
                        *ret = i->usec;
                        തിരികെ e;
                }
        }

        തിരികെ p;
}

പൂർണ്ണസംഖ്യ parse_time(സ്ഥിരം അക്ഷരം *t, usec_t *ret, usec_t default_unit) {
        സ്ഥിരം അക്ഷരം *p, *s;

        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ ശൂന്യ);
        s = startswith(p, "infinity");
        എങ്കിൽ (s) {
                എങ്കിൽ (!in_charset(s, WHITESPACE))
                        തിരികെ -EINVAL;

                എങ്കിൽ (ret)
                        *ret = USEC_INFINITY;
                തിരികെ 0;
        }

        usec_t usec = 0;

        വേണ്ടി (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                അക്ഷരം *e;

                p = skip_leading_chars(p, /* bad= */ ശൂന്യ);
                എങ്കിൽ (*p == 0) {
                        എങ്കിൽ (!something)
                                തിരികെ -EINVAL;

                        നിർത്തുക;
                }

                എങ്കിൽ (*p == '-') /* Don't allow "-0" */
                        തിരികെ -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                എങ്കിൽ (errno > 0)
                        തിരികെ -errno;
                എങ്കിൽ (l < 0)
                        തിരികെ -ERANGE;

                എങ്കിൽ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } അല്ലെങ്കിൽ എങ്കിൽ (e == p)
                        തിരികെ -EINVAL;
                അല്ലെങ്കിൽ
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                എങ്കിൽ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        തിരികെ -EINVAL;

                p = s;

                എങ്കിൽ ((usec_t) l >= USEC_INFINITY / multiplier)
                        തിരികെ -ERANGE;

                k = (usec_t) l * multiplier;
                എങ്കിൽ (k >= USEC_INFINITY - usec)
                        തിരികെ -ERANGE;

                usec += k;

                something = true;

                എങ്കിൽ (*e == '.') {
                        usec_t m = multiplier / 10;
                        സ്ഥിരം അക്ഷരം *b;

                        വേണ്ടി (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                എങ്കിൽ (k >= USEC_INFINITY - usec)
                                        തിരികെ -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        എങ്കിൽ (b == e + 1)
                                തിരികെ -EINVAL;
                }
        }

        എങ്കിൽ (ret)
                *ret = usec;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_sec(സ്ഥിരം അക്ഷരം *t, usec_t *ret) {
        തിരികെ parse_time(t, ret, USEC_PER_SEC);
}

പൂർണ്ണസംഖ്യ parse_sec_fix_0(സ്ഥിരം അക്ഷരം *t, usec_t *ret) {
        usec_t k;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(ret);

        r = parse_sec(t, &k);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = k == 0 ? USEC_INFINITY : k;
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ parse_sec_def_infinity(സ്ഥിരം അക്ഷരം *t, usec_t *ret) {
        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(ret);

        t += strspn(t, WHITESPACE);
        എങ്കിൽ (isempty(t)) {
                *ret = USEC_INFINITY;
                തിരികെ 0;
        }
        തിരികെ parse_sec(t, ret);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* extract_nsec_multiplier(സ്ഥിരം അക്ഷരം *p, nsec_t *ret) {
        സ്ഥിതം സ്ഥിരം ഘടന {
                സ്ഥിരം അക്ഷരം *suffix;
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

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        FOREACH_ELEMENT(i, table) {
                സ്ഥിരം അക്ഷരം *e = startswith(p, i->suffix);
                എങ്കിൽ (e) {
                        *ret = i->nsec;
                        തിരികെ e;
                }
        }

        തിരികെ p;
}

പൂർണ്ണസംഖ്യ parse_nsec(സ്ഥിരം അക്ഷരം *t, nsec_t *ret) {
        സ്ഥിരം അക്ഷരം *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        എങ്കിൽ (s) {
                s += strspn(s, WHITESPACE);
                എങ്കിൽ (*s != 0)
                        തിരികെ -EINVAL;

                *ret = NSEC_INFINITY;
                തിരികെ 0;
        }

        വേണ്ടി (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                അക്ഷരം *e;

                p += strspn(p, WHITESPACE);

                എങ്കിൽ (*p == 0) {
                        എങ്കിൽ (!something)
                                തിരികെ -EINVAL;

                        നിർത്തുക;
                }

                എങ്കിൽ (*p == '-') /* Don't allow "-0" */
                        തിരികെ -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                എങ്കിൽ (errno > 0)
                        തിരികെ -errno;
                എങ്കിൽ (l < 0)
                        തിരികെ -ERANGE;

                എങ്കിൽ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } അല്ലെങ്കിൽ എങ്കിൽ (e == p)
                        തിരികെ -EINVAL;
                അല്ലെങ്കിൽ
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                എങ്കിൽ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        തിരികെ -EINVAL;

                p = s;

                എങ്കിൽ ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        തിരികെ -ERANGE;

                k = (nsec_t) l * multiplier;
                എങ്കിൽ (k >= NSEC_INFINITY - nsec)
                        തിരികെ -ERANGE;

                nsec += k;

                something = true;

                എങ്കിൽ (*e == '.') {
                        nsec_t m = multiplier / 10;
                        സ്ഥിരം അക്ഷരം *b;

                        വേണ്ടി (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                എങ്കിൽ (k >= NSEC_INFINITY - nsec)
                                        തിരികെ -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        എങ്കിൽ (b == e + 1)
                                തിരികെ -EINVAL;
                }
        }

        *ret = nsec;

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ get_timezones_from_zone1970_tab(അക്ഷരം ***ret) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_strv_free_ അക്ഷരം **zones = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        എങ്കിൽ (!f)
                തിരികെ -errno;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ, *cc = ശൂന്യ, *co = ശൂന്യ, *tz = ശൂന്യ;

                r = read_വരി(f, LONG_വരി_MAX, &വരി);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                സ്ഥിരം അക്ഷരം *p = വരി;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_വാക്കുകൾ(&p, ശൂന്യ, 0, &cc, &co, &tz);
                എങ്കിൽ (r < 0)
                        തുടരുക;

                /* Lines that start with # are comments. */
                എങ്കിൽ (*cc == '#')
                        തുടരുക;

                എങ്കിൽ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        തുടരുക;

                r = strv_extend(&zones, tz);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret = TAKE_PTR(zones);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ get_timezones_from_tzdata_zi(അക്ഷരം ***ret) {
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        _cleanup_strv_free_ അക്ഷരം **zones = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        എങ്കിൽ (!f)
                തിരികെ -errno;

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ, *type = ശൂന്യ, *f1 = ശൂന്യ, *f2 = ശൂന്യ;

                r = read_വരി(f, LONG_വരി_MAX, &വരി);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                സ്ഥിരം അക്ഷരം *p = വരി;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_വാക്കുകൾ(&p, ശൂന്യ, 0, &type, &f1, &f2);
                എങ്കിൽ (r < 0)
                        തുടരുക;

                അക്ഷരം *tz;
                എങ്കിൽ (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                അല്ലെങ്കിൽ എങ്കിൽ (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                അല്ലെങ്കിൽ
                        /* Not a line we care about. */
                        തുടരുക;

                എങ്കിൽ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        തുടരുക;

                r = strv_extend(&zones, tz);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret = TAKE_PTR(zones);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_timezones(അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **zones = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        എങ്കിൽ (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                എങ്കിൽ (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        എങ്കിൽ (r < 0 && r != -ENOENT)
                തിരികെ r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        എങ്കിൽ (r < 0)
                തിരികെ r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ verify_timezone(സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ log_level) {
        bool slash = false;
        സ്ഥിരം അക്ഷരം *p, *t;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        അക്ഷരം buf[4];
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (isempty(name))
                തിരികെ -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        എങ്കിൽ (streq(name, "UTC"))
                തിരികെ 0;

        എങ്കിൽ (name[0] == '/')
                തിരികെ -EINVAL;

        വേണ്ടി (p = name; *p; p++) {
                എങ്കിൽ (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        തിരികെ -EINVAL;

                എങ്കിൽ (*p == '/') {

                        എങ്കിൽ (slash)
                                തിരികെ -EINVAL;

                        slash = true;
                } അല്ലെങ്കിൽ
                        slash = false;
        }

        എങ്കിൽ (slash)
                തിരികെ -EINVAL;

        എങ്കിൽ (p - name >= PATH_MAX)
                തിരികെ -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        എങ്കിൽ (memcmp(buf, "TZif", 4) != 0)
                തിരികെ log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        തിരികെ 0;
}

ശൂന്യം reset_timezonep(അക്ഷരം **p) {
        പ്രതിജ്ഞാ(p);

        (ശൂന്യം) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

അക്ഷരം* save_timezone(ശൂന്യം) {
        സ്ഥിരം അക്ഷരം *e = getenv("TZ");
        എങ്കിൽ (!e)
                തിരികെ ശൂന്യ;

        അക്ഷരം *s = strdup(e);
        എങ്കിൽ (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        തിരികെ s;
}

bool clock_supported(clockid_t clock) {
        ഘടന timespec ts;

        തിരഞ്ഞെടുപ്പ് (clock) {

        അവസ്ഥ CLOCK_MONOTONIC:
        അവസ്ഥ CLOCK_REALTIME:
        അവസ്ഥ CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                തിരികെ true;

        സ്ഥിരസ്ഥിതി:
                /* For everything else, check properly */
                തിരികെ clock_gettime(clock, &ts) >= 0;
        }
}

പൂർണ്ണസംഖ്യ get_timezone(അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = readlink_malloc(etc_localtime(), &t);
        എങ്കിൽ (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                തിരികെ strdup_to(ret, "UTC");
        എങ്കിൽ (r < 0)
                തിരികെ r; /* Return EINVAL if not a symlink */

        സ്ഥിരം അക്ഷരം *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        എങ്കിൽ (!e)
                തിരികെ -EINVAL;
        എങ്കിൽ (!timezone_is_valid(e, LOG_DEBUG))
                തിരികെ -EINVAL;

        തിരികെ strdup_to(ret, e);
}

പൂർണ്ണസംഖ്യ get_timezone_prefer_env(അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(ret);

        സ്ഥിരം അക്ഷരം *e = getenv("TZ");
        എങ്കിൽ (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                തിരികെ strdup_to(ret, e + 1);

        തിരികെ get_timezone(ret);
}

സ്ഥിരം അക്ഷരം* etc_localtime(ശൂന്യം) {
        സ്ഥിതം സ്ഥിരം അക്ഷരം *cached = ശൂന്യ;

        എങ്കിൽ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        തിരികെ cached;
}

പൂർണ്ണസംഖ്യ mktime_or_timegm_usec(
                ഘടന tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        പ്രതിജ്ഞാ(tm);

        എങ്കിൽ (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                തിരികെ -ERANGE;
        എങ്കിൽ ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                തിരികെ -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        എങ്കിൽ (t < 0) /* Refuse negative times and errors */
                തിരികെ -ERANGE;
        എങ്കിൽ ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                തിരികെ -ERANGE;

        എങ്കിൽ (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                ഘടന tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        എങ്കിൽ (t > (usec_t) TIME_T_MAX)
                തിരികെ -ERANGE;
        time_t sec = (time_t) t;

        ഘടന tm buf = {};
        എങ്കിൽ (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                തിരികെ -EINVAL;

        എങ്കിൽ (ret)
                *ret = buf;

        തിരികെ 0;
}

uint64_t sysconf_clock_ticks_cached(ശൂന്യം) {
        സ്ഥിതം thread_local uint64_t hz = 0;

        എങ്കിൽ (hz != 0)
                തിരികെ hz;

        long t = sysconf(_SC_CLK_TCK);
        പ്രതിജ്ഞാ(t > 0);
        hz = (uint64_t) t;

        തിരികെ hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        തിരികെ DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        തിരികെ DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        എങ്കിൽ (x == USEC_INFINITY)
                തിരികെ USEC_INFINITY;
        എങ്കിൽ (map_clock_id(from) == map_clock_id(to))
                തിരികെ x;

        a = now(from);
        b = now(to);

        എങ്കിൽ (x > a)
                /* x lies in the future */
                തിരികെ usec_add(b, usec_sub_unsigned(x, a));
        അല്ലെങ്കിൽ
                /* x lies in the past */
                തിരികെ usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(ശൂന്യം) {
        tzset();

        തിരികെ timezone == 0 && daylight == 0;
}

പൂർണ്ണസംഖ്യ usleep_safe(usec_t usec) {
        പൂർണ്ണസംഖ്യ r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        എങ്കിൽ (usec == 0)
                തിരികെ 0;

        എങ്കിൽ (usec == USEC_INFINITY)
                തിരികെ RET_NERRNO(pause());

        ഘടന timespec t;
        timespec_store(&t, usec);

        വേണ്ടി (;;) {
                ഘടന timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                എങ്കിൽ (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        തുടരുക;
                }

                തിരികെ r;
        }
}

പൂർണ്ണസംഖ്യ time_change_fd(ശൂന്യം) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        സ്ഥിതം സ്ഥിരം ഘടന itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        എങ്കിൽ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, ശൂന്യ) >= 0)
                തിരികെ TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#എങ്കിൽ SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        എങ്കിൽ (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                സ്ഥിതം സ്ഥിരം ഘടന itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                എങ്കിൽ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, ശൂന്യ) >= 0)
                        തിരികെ TAKE_FD(fd);
        }
#endif

        തിരികെ -errno;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(സ്ഥിരം അക്ഷരം *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        എങ്കിൽ (t >= 0)
                തിരികെ t;
        എങ്കിൽ (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                തിരികെ TIMESTAMP_US;
        എങ്കിൽ (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                തിരികെ TIMESTAMP_US_UTC;
        തിരികെ t;
}

പൂർണ്ണസംഖ്യ parse_calendar_date_full(സ്ഥിരം അക്ഷരം *s, bool allow_pre_epoch, usec_t *ret_usec, ഘടന tm *ret_tm) {
        ഘടന tm parsed_tm = {}, copy_tm;
        സ്ഥിരം അക്ഷരം *k;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        എങ്കിൽ (!k || *k)
                തിരികെ -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        എങ്കിൽ (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                എങ്കിൽ (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        തിരികെ -EINVAL;
        } അല്ലെങ്കിൽ {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        എങ്കിൽ (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                തിരികെ -EINVAL;

        എങ്കിൽ (ret_usec)
                *ret_usec = usec;
        എങ്കിൽ (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        തിരികെ 0;
}
