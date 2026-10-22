/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/time_types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/timerfd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"          /* IWYU pragma: keep */
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

ਸਥਿਰ clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        ਚੋਣ (c) {

        ਹਾਲਤ CLOCK_BOOTTIME_ALARM:
                ਵਾਪਸ CLOCK_BOOTTIME;

        ਹਾਲਤ CLOCK_REALTIME_ALARM:
                ਵਾਪਸ CLOCK_REALTIME;

        ਮੂਲ:
                ਵਾਪਸ c;
        }
}

usec_t now(clockid_t clock_id) {
        ਬਣਤਰ timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_ਲੱਦੋ(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        ਪ੍ਰਤਿਜ੍ਞਾ(n > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(n < USEC_INFINITY);

        ਵਾਪਸ n;
}

nsec_t now_nsec(clockid_t clock_id) {
        ਬਣਤਰ timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_ਲੱਦੋ_nsec(&ts);

        ਪ੍ਰਤਿਜ੍ਞਾ(n > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(n < NSEC_INFINITY);

        ਵਾਪਸ n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        ਵਾਪਸ ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        ਵਾਪਸ ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_ਅਧਾਰ, usec_t ਮੰਜ਼ਿਲ_ਅਧਾਰ) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        ਜੇ (from >= from_ਅਧਾਰ) { /* In the future */
                usec_t delta = from - from_ਅਧਾਰ;

                ਜੇ (ਮੰਜ਼ਿਲ_ਅਧਾਰ >= USEC_INFINITY - delta) /* overflow? */
                        ਵਾਪਸ USEC_INFINITY;

                ਵਾਪਸ ਮੰਜ਼ਿਲ_ਅਧਾਰ + delta;

        } ਨਹੀਂਤਾਂ { /* In the past */
                usec_t delta = from_ਅਧਾਰ - from;

                ਜੇ (ਮੰਜ਼ਿਲ_ਅਧਾਰ <= delta) /* underflow? */
                        ਵਾਪਸ 0;

                ਵਾਪਸ ਮੰਜ਼ਿਲ_ਅਧਾਰ - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t ਮੰਜ਼ਿਲ_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        ਜੇ (map_clock_id(from_clock) == map_clock_id(ਮੰਜ਼ਿਲ_clock))
                ਵਾਪਸ from;

        /* Keep infinity as is */
        ਜੇ (from == USEC_INFINITY)
                ਵਾਪਸ from;

        ਵਾਪਸ map_clock_usec_raw(from, now(from_clock), now(ਮੰਜ਼ਿਲ_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (!timestamp_is_ਸੈੱਟ(u)) {
                ts->realtime = ts->monotonic = u;
                ਵਾਪਸ ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        ਵਾਪਸ ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (!timestamp_is_ਸੈੱਟ(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ਵਾਪਸ ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        ਵਾਪਸ ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ਵਾਪਸ ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        ਵਾਪਸ ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ਵਾਪਸ ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        ਵਾਪਸ ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ਵਾਪਸ ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        ਵਾਪਸ ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਚੋਣ (clock) {

        ਹਾਲਤ CLOCK_REALTIME:
        ਹਾਲਤ CLOCK_REALTIME_ALARM:
                ਵਾਪਸ ts->realtime;

        ਹਾਲਤ CLOCK_MONOTONIC:
                ਵਾਪਸ ts->monotonic;

        ਹਾਲਤ CLOCK_BOOTTIME:
        ਹਾਲਤ CLOCK_BOOTTIME_ALARM:
                ਵਾਪਸ ts->boottime;

        ਮੂਲ:
                ਵਾਪਸ USEC_INFINITY;
        }
}

usec_t timespec_ਲੱਦੋ(ਅਚਲ ਬਣਤਰ timespec *ts) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ਵਾਪਸ USEC_INFINITY;

        ਜੇ ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_ਪ੍ਰਤੀ_USEC)) / USEC_ਪ੍ਰਤੀ_SEC)
                ਵਾਪਸ USEC_INFINITY;

        ਵਾਪਸ
                (usec_t) ts->tv_sec * USEC_ਪ੍ਰਤੀ_SEC +
                (usec_t) ts->tv_nsec / NSEC_ਪ੍ਰਤੀ_USEC;
}

nsec_t timespec_ਲੱਦੋ_nsec(ਅਚਲ ਬਣਤਰ timespec *ts) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ਵਾਪਸ NSEC_INFINITY;

        ਜੇ ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_ਪ੍ਰਤੀ_SEC)
                ਵਾਪਸ NSEC_INFINITY;

        ਵਾਪਸ (nsec_t) ts->tv_sec * NSEC_ਪ੍ਰਤੀ_SEC + (nsec_t) ts->tv_nsec;
}

ਬਣਤਰ timespec *timespec_store(ਬਣਤਰ timespec *ts, usec_t u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (u == USEC_INFINITY ||
            u / USEC_ਪ੍ਰਤੀ_SEC >= ਸਮਾਂ_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ਵਾਪਸ ts;
        }

        ts->tv_sec = (time_t) (u / USEC_ਪ੍ਰਤੀ_SEC);
        ts->tv_nsec = (ਲੰਮਾ) ((u % USEC_ਪ੍ਰਤੀ_SEC) * NSEC_ਪ੍ਰਤੀ_USEC);

        ਵਾਪਸ ts;
}

ਬਣਤਰ timespec *timespec_store_nsec(ਬਣਤਰ timespec *ts, nsec_t n) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);

        ਜੇ (n == NSEC_INFINITY ||
            n / NSEC_ਪ੍ਰਤੀ_SEC >= ਸਮਾਂ_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ਵਾਪਸ ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_ਪ੍ਰਤੀ_SEC);
        ts->tv_nsec = (ਲੰਮਾ) (n % NSEC_ਪ੍ਰਤੀ_SEC);

        ਵਾਪਸ ts;
}

ਬਣਤਰ __kernel_timespec *kernel_timespec_store(ਬਣਤਰ __kernel_timespec *ts, usec_t u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        ਪ੍ਰਤਿਜ੍ਞਾ(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_ਪ੍ਰਤੀ_SEC;
        ts->tv_nsec = (u % USEC_ਪ੍ਰਤੀ_SEC) * NSEC_ਪ੍ਰਤੀ_USEC;

        ਵਾਪਸ ts;
}

usec_t timeval_ਲੱਦੋ(ਅਚਲ ਬਣਤਰ timeval *tv) {
        ਪ੍ਰਤਿਜ੍ਞਾ(tv);

        ਜੇ (tv->tv_sec < 0 || tv->tv_usec < 0)
                ਵਾਪਸ USEC_INFINITY;

        ਜੇ ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_ਪ੍ਰਤੀ_SEC)
                ਵਾਪਸ USEC_INFINITY;

        ਵਾਪਸ
                (usec_t) tv->tv_sec * USEC_ਪ੍ਰਤੀ_SEC +
                (usec_t) tv->tv_usec;
}

ਬਣਤਰ timeval *timeval_store(ਬਣਤਰ timeval *tv, usec_t u) {
        ਪ੍ਰਤਿਜ੍ਞਾ(tv);

        ਜੇ (u == USEC_INFINITY ||
            u / USEC_ਪ੍ਰਤੀ_SEC > ਸਮਾਂ_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } ਨਹੀਂਤਾਂ {
                tv->tv_sec = (time_t) (u / USEC_ਪ੍ਰਤੀ_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_ਪ੍ਰਤੀ_SEC);
        }

        ਵਾਪਸ tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ weekday_ਸਾਰਣੀ[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(weekday, ਪੂਰਨ);

ਅੱਖਰ* format_timestamp_style(
                ਅੱਖਰ *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        ਬਣਤਰ tm tm;
        bool utc, us;
        size_t n;

        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(style >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(style < _TIMESTAMP_STYLE_MAX);

        ਜੇ (!timestamp_is_ਸੈੱਟ(t))
                ਵਾਪਸ ਨਿਰਾ; /* Timestamp is unset */

        ਜੇ (style == TIMESTAMP_UNIX) {
                ਜੇ (l < (size_t) (1 + 1 + 1))
                        ਵਾਪਸ ਨਿਰਾ; /* not enough space for even the shortest of forms */

                ਵਾਪਸ snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_ਪ੍ਰਤੀ_SEC);  /* round down μs → s */
        }

        utc = ਵਿੱਚ_ਸੈੱਟ(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = ਵਿੱਚ_ਸੈੱਟ(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        ਜੇ (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                ਵਾਪਸ ਨਿਰਾ; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        ਜੇ (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                ਪ੍ਰਤਿਜ੍ਞਾ(l >= ਲੜੀ_ਲੰਬਾਈ(xxx[style]) + 1);
                ਵਾਪਸ strcpy(buf, xxx[style]);
        }

        ਜੇ (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                ਵਾਪਸ ਨਿਰਾ;

        /* Start with the week day */
        ਅਚਲ ਅੱਖਰ *weekday = weekday_ਮੰਜ਼ਿਲ_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        ਪ੍ਰਤਿਜ੍ਞਾ(weekday);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, weekday, 4);

        ਜੇ (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                ਜੇ (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        ਵਾਪਸ ਨਿਰਾ; /* Doesn't fit */

                ਵਾਪਸ buf;
        }

        /* Add the main components */
        ਜੇ (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                ਵਾਪਸ ਨਿਰਾ; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        ਜੇ (us) {
                n = ਲੜੀ_ਲੰਬਾਈ(buf);
                ਜੇ (n + 8 > l)
                        ਵਾਪਸ ਨਿਰਾ; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_ਪ੍ਰਤੀ_SEC);
        }

        /* Append the timezone */
        n = ਲੜੀ_ਲੰਬਾਈ(buf);
        ਜੇ (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                ਜੇ (n + 5 > l)
                        ਵਾਪਸ ਨਿਰਾ; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } ਨਹੀਂਤਾਂ ਜੇ (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = ਲੜੀ_ਲੰਬਾਈ(tm.tm_zone);
                ਜੇ (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        ਜੇ (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                ਵਾਪਸ ਨਿਰਾ; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } ਨਹੀਂਤਾਂ {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        ਵਾਪਸ buf;
}

ਅੱਖਰ* format_timestamp_relative_ਭਰਿਆ(ਅੱਖਰ *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ਖੱਬਾ) {
        ਅਚਲ ਅੱਖਰ *s;
        usec_t n, d;

        ਪ੍ਰਤਿਜ੍ਞਾ(buf);

        ਜੇ (!timestamp_is_ਸੈੱਟ(t))
                ਵਾਪਸ ਨਿਰਾ;

        n = now(clock);
        ਜੇ (n > t) {
                d = n - t;
                s = " ago";
        } ਨਹੀਂਤਾਂ {
                d = t - n;
                s = implicit_ਖੱਬਾ ? "" : " left";
        }

        ਜੇ (d >= USEC_ਪ੍ਰਤੀ_YEAR) {
                usec_t years = d / USEC_ਪ੍ਰਤੀ_YEAR;
                usec_t months = (d % USEC_ਪ੍ਰਤੀ_YEAR) / USEC_ਪ੍ਰਤੀ_MONTH;

                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } ਨਹੀਂਤਾਂ ਜੇ (d >= USEC_ਪ੍ਰਤੀ_MONTH) {
                usec_t months = d / USEC_ਪ੍ਰਤੀ_MONTH;
                usec_t days = (d % USEC_ਪ੍ਰਤੀ_MONTH) / USEC_ਪ੍ਰਤੀ_DAY;

                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } ਨਹੀਂਤਾਂ ਜੇ (d >= USEC_ਪ੍ਰਤੀ_WEEK) {
                usec_t weeks = d / USEC_ਪ੍ਰਤੀ_WEEK;
                usec_t days = (d % USEC_ਪ੍ਰਤੀ_WEEK) / USEC_ਪ੍ਰਤੀ_DAY;

                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } ਨਹੀਂਤਾਂ ਜੇ (d >= 2*USEC_ਪ੍ਰਤੀ_DAY)
                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT " days%s", d / USEC_ਪ੍ਰਤੀ_DAY,s);
        ਨਹੀਂਤਾਂ ਜੇ (d >= 25*USEC_ਪ੍ਰਤੀ_HOUR)
                (ਖਾਲੀ) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_ਪ੍ਰਤੀ_DAY) / USEC_ਪ੍ਰਤੀ_HOUR, s);
        ਨਹੀਂਤਾਂ ਜੇ (d >= 6*USEC_ਪ੍ਰਤੀ_HOUR)
                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_ਪ੍ਰਤੀ_HOUR, s);
        ਨਹੀਂਤਾਂ ਜੇ (d >= USEC_ਪ੍ਰਤੀ_HOUR)
                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_ਪ੍ਰਤੀ_HOUR,
                                (d % USEC_ਪ੍ਰਤੀ_HOUR) / USEC_ਪ੍ਰਤੀ_MINUTE, s);
        ਨਹੀਂਤਾਂ ਜੇ (d >= 5*USEC_ਪ੍ਰਤੀ_MINUTE)
                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_ਪ੍ਰਤੀ_MINUTE, s);
        ਨਹੀਂਤਾਂ ਜੇ (d >= USEC_ਪ੍ਰਤੀ_MINUTE)
                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_ਪ੍ਰਤੀ_MINUTE,
                                (d % USEC_ਪ੍ਰਤੀ_MINUTE) / USEC_ਪ੍ਰਤੀ_SEC, s);
        ਨਹੀਂਤਾਂ ਜੇ (d >= USEC_ਪ੍ਰਤੀ_SEC)
                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_ਪ੍ਰਤੀ_SEC, s);
        ਨਹੀਂਤਾਂ ਜੇ (d >= USEC_ਪ੍ਰਤੀ_MSEC)
                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_ਪ੍ਰਤੀ_MSEC, s);
        ਨਹੀਂਤਾਂ ਜੇ (d > 0)
                (ਖਾਲੀ) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        ਨਹੀਂਤਾਂ
                (ਖਾਲੀ) snprintf(buf, l, "now");

        buf[l-1] = 0;
        ਵਾਪਸ buf;
}

ਅੱਖਰ* format_timespan(ਅੱਖਰ *buf, size_t l, usec_t t, usec_t accuracy) {
        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ *suffix;
                usec_t usec;
        } ਸਾਰਣੀ[] = {
                { "y",     USEC_ਪ੍ਰਤੀ_YEAR   },
                { "month", USEC_ਪ੍ਰਤੀ_MONTH  },
                { "w",     USEC_ਪ੍ਰਤੀ_WEEK   },
                { "d",     USEC_ਪ੍ਰਤੀ_DAY    },
                { "h",     USEC_ਪ੍ਰਤੀ_HOUR   },
                { "min",   USEC_ਪ੍ਰਤੀ_MINUTE },
                { "s",     USEC_ਪ੍ਰਤੀ_SEC    },
                { "ms",    USEC_ਪ੍ਰਤੀ_MSEC   },
                { "us",    1               },
        };

        ਅੱਖਰ *p = ASSERT_PTR(buf);
        bool something = false;

        ਪ੍ਰਤਿਜ੍ਞਾ(l > 0);

        ਜੇ (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                ਵਾਪਸ p;
        }

        ਜੇ (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                ਵਾਪਸ p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, ਸਾਰਣੀ) {
                ਪੂਰਨ k = 0;
                size_t n;
                bool ਪੂਰਾ = false;
                usec_t a, b;

                ਜੇ (t <= 0)
                        ਰੋਕੋ;

                ਜੇ (t < accuracy && something)
                        ਰੋਕੋ;

                ਜੇ (t < i->usec)
                        ਜਾਰੀ;

                ਜੇ (l <= 1)
                        ਰੋਕੋ;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                ਜੇ (t < USEC_ਪ੍ਰਤੀ_MINUTE && b > 0) {
                        ਨਿਸ਼ਾਨੀਵਾਲਾ ਅੱਖਰ j = 0;

                        ਲਈ (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        ਲਈ (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        ਜੇ (j > 0) {
                                k = snprintf(p, l,
                                             "%s"USEC_FMT".%0*"PRI_USEC"%s",
                                             p > buf ? " " : "",
                                             a,
                                             j,
                                             b,
                                             i->suffix);

                                t = 0;
                                ਪੂਰਾ = true;
                        }
                }

                /* No? Then let's show it normally */
                ਜੇ (!ਪੂਰਾ) {
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

        ਵਾਪਸ buf;
}

ਅਚਲ ਅੱਖਰ* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        ਜੇ (dst && isempty(tzname[1]))
                dst = false;

        ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_null(tzname[dst]);
}

ਪੂਰਨ parse_gmtoff(ਅਚਲ ਅੱਖਰ *t, ਲੰਮਾ *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(t);

        ਬਣਤਰ tm tm;
        ਅਚਲ ਅੱਖਰ *k = strptime(t, "%z", &tm);
        ਜੇ (k && *k == '\0') {
                /* Success! */
                ਜੇ (ret)
                        *ret = tm.tm_gmtoff;
                ਵਾਪਸ 0;
        }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GLIBC__
        ਵਾਪਸ -EINVAL;
#ਨਹੀਂਤਾਂ
        ਪੂਰਨ r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool ਸਕਾਰਾਤਮਕ;
        ਚੋਣ (*t) {
        ਹਾਲਤ '+':
                ਸਕਾਰਾਤਮਕ = true;
                ਰੋਕੋ;
        ਹਾਲਤ '-':
                ਸਕਾਰਾਤਮਕ = false;
                ਰੋਕੋ;
        ਮੂਲ:
                ਵਾਪਸ -EINVAL;
        }

        t++;
        r = undecchar(*t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        usec_t u = r * 10 * USEC_ਪ੍ਰਤੀ_HOUR;

        t++;
        r = undecchar(*t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        u += r * USEC_ਪ੍ਰਤੀ_HOUR;

        t++;
        ਜੇ (*t == '\0') /* 2 digits case */
                ਜਾਓ finalize;

        ਜੇ (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r >= 6) /* refuse minutes equal to or larger than 60 */
                ਵਾਪਸ -EINVAL;

        u += r * 10 * USEC_ਪ੍ਰਤੀ_MINUTE;

        t++;
        r = undecchar(*t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        u += r * USEC_ਪ੍ਰਤੀ_MINUTE;

        t++;
        ਜੇ (*t != '\0')
                ਵਾਪਸ -EINVAL;

finalize:
        ਜੇ (u > USEC_ਪ੍ਰਤੀ_DAY) /* refuse larger than one day */
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret) {
                ਲੰਮਾ gmtoff = u / USEC_ਪ੍ਰਤੀ_SEC;
                *ret = ਸਕਾਰਾਤਮਕ ? gmtoff : -gmtoff;
        }

        ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ parse_timestamp_impl(
                ਅਚਲ ਅੱਖਰ *t,
                size_t max_len,
                bool utc,
                ਪੂਰਨ isdst,
                ਲੰਮਾ gmtoff,
                usec_t *ret) {

        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ *name;
                ਅਚਲ ਪੂਰਨ nr;
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

        _cleanup_free_ ਅੱਖਰ *t_alloc = ਨਿਰਾ;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        ਪੂਰਨ r, weekday = -1;
        ਬਿਨਾਨਿਸ਼ਾਨੀ fractional = 0;
        ਅਚਲ ਅੱਖਰ *k;
        ਬਣਤਰ tm tm, copy;

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

        ਪ੍ਰਤਿਜ੍ਞਾ(t);

        ਜੇ (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                ਜੇ (max_len == 0) /* Can't be the only field */
                        ਵਾਪਸ -EINVAL;

                t_alloc = strndup(t, max_len);
                ਜੇ (!t_alloc)
                        ਵਾਪਸ -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        ਜੇ (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                ਜੇ ((usec_t) labs(gmtoff) * USEC_ਪ੍ਰਤੀ_SEC > USEC_ਪ੍ਰਤੀ_DAY)
                        ਵਾਪਸ -EINVAL;
        } ਨਹੀਂਤਾਂ {
                ਜੇ (gmtoff != 0)
                        ਵਾਪਸ -EINVAL;
        }

        ਜੇ (t[0] == '@' && !with_tz)
                ਵਾਪਸ parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        ਜੇ (!with_tz) {
                ਜੇ (streq(t, "now"))
                        ਜਾਓ ਅੰਤ;

                ਜੇ (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ਜਾਓ ਅੰਤ;
                }

                ਜੇ (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ਜਾਓ ਅੰਤ;
                }

                ਜੇ ((k = endswith(t, " ago"))) {
                        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;

                        buf = strndup(t, k - t);
                        ਜੇ (!buf)
                                ਵਾਪਸ -ENOMEM;

                        r = parse_sec(buf, &minus);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ਜਾਓ ਅੰਤ;
                }

                ਜੇ ((k = endswith(t, " left"))) {
                        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;

                        buf = strndup(t, k - t);
                        ਜੇ (!buf)
                                ਵਾਪਸ -ENOMEM;

                        r = parse_sec(buf, &plus);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ਜਾਓ ਅੰਤ;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        tm.tm_isdst = isdst;

        ਜੇ (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ਜਾਓ from_tm;

        } ਨਹੀਂਤਾਂ ਜੇ (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ਜਾਓ from_tm;

        } ਨਹੀਂਤਾਂ ਜੇ (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ਜਾਓ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                ਜੇ (!k || *k != ' ')
                        ਜਾਰੀ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#ਜੇ_ਅੰਤ
                t = k + 1;
                ਰੋਕੋ;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        ਜੇ (k) {
                ਜੇ (*k == '.')
                        ਜਾਓ parse_usec;
                ਨਹੀਂਤਾਂ ਜੇ (*k == 0)
                        ਜਾਓ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        ਜੇ (k) {
                ਜੇ (*k == '.')
                        ਜਾਓ parse_usec;
                ਨਹੀਂਤਾਂ ਜੇ (*k == 0)
                        ਜਾਓ from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        ਜੇ (k) {
                ਜੇ (*k == '.')
                        ਜਾਓ parse_usec;
                ਨਹੀਂਤਾਂ ਜੇ (*k == 0)
                        ਜਾਓ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        ਜੇ (k) {
                ਜੇ (*k == '.')
                        ਜਾਓ parse_usec;
                ਨਹੀਂਤਾਂ ਜੇ (*k == 0)
                        ਜਾਓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        ਜੇ (k && *k == 0) {
                tm.tm_sec = 0;
                ਜਾਓ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        ਜੇ (k && *k == 0) {
                tm.tm_sec = 0;
                ਜਾਓ from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        ਜੇ (k && *k == 0) {
                tm.tm_sec = 0;
                ਜਾਓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        ਜੇ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ਜਾਓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        ਜੇ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ਜਾਓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        ਜੇ (k) {
                ਜੇ (*k == '.')
                        ਜਾਓ parse_usec;
                ਨਹੀਂਤਾਂ ਜੇ (*k == 0)
                        ਜਾਓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        ਜੇ (k && *k == 0) {
                tm.tm_sec = 0;
                ਜਾਓ from_tm;
        }

        ਵਾਪਸ -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        ਜੇ (r < 0)
                ਵਾਪਸ -EINVAL;
        ਜੇ (*k != '\0')
                ਵਾਪਸ -EINVAL;

from_tm:
        ਪ੍ਰਤਿਜ੍ਞਾ(plus == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(minus == 0);

        ਜੇ (weekday >= 0 && tm.tm_wday != weekday)
                ਵਾਪਸ -EINVAL;

        ਜੇ (gmtoff < 0) {
                plus = -gmtoff * USEC_ਪ੍ਰਤੀ_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                ਜੇ (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_ਪ੍ਰਤੀ_DAY;
                }
        } ਨਹੀਂਤਾਂ
                minus = gmtoff * USEC_ਪ੍ਰਤੀ_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        usec = usec_ਜੋੜੋ(usec, fractional);

ਅੰਤ:
        usec = usec_ਜੋੜੋ(usec, plus);

        ਜੇ (usec < minus)
                ਵਾਪਸ -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        ਜੇ (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret)
                *ret = usec;
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_timestamp(ਅਚਲ ਅੱਖਰ *t, usec_t *ret) {
        ਲੰਮਾ gmtoff;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);

        size_t t_len = ਲੜੀ_ਲੰਬਾਈ(t);
        ਜੇ (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                ਜੇ (r >= 0)
                        ਵਾਪਸ r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        ਜੇ (t_len > 7 && ਵਿੱਚ_ਸੈੱਟ(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                ਵਾਪਸ parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ਅਚਲ ਅੱਖਰ *tz = strrchr(t, ' ');
        ਜੇ (!tz)
                ਵਾਪਸ parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        ਜੇ (streq(tz, "UTC"))
                ਵਾਪਸ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        ਜੇ (parse_gmtoff(tz, &gmtoff) >= 0)
                ਵਾਪਸ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        ਲਈ (ਪੂਰਨ j = 0; j <= 1; j++) {
                ਜੇ (!streq_ptr(tz, get_tzname(j)))
                        ਜਾਰੀ;

                /* The specified timezone matches tzname[] of the local timezone. */
                ਵਾਪਸ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        ਜੇ (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                ਜੇ (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        ਵਾਪਸ negative_errno();

                ਵਾਪਸ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        ਵਾਪਸ parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਕੱਢੋ_multiplier(ਅਚਲ ਅੱਖਰ *p, usec_t *ret) {
        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ *suffix;
                usec_t usec;
        } ਸਾਰਣੀ[] = {
                { "seconds", USEC_ਪ੍ਰਤੀ_SEC    },
                { "second",  USEC_ਪ੍ਰਤੀ_SEC    },
                { "sec",     USEC_ਪ੍ਰਤੀ_SEC    },
                { "s",       USEC_ਪ੍ਰਤੀ_SEC    },
                { "minutes", USEC_ਪ੍ਰਤੀ_MINUTE },
                { "minute",  USEC_ਪ੍ਰਤੀ_MINUTE },
                { "min",     USEC_ਪ੍ਰਤੀ_MINUTE },
                { "months",  USEC_ਪ੍ਰਤੀ_MONTH  },
                { "month",   USEC_ਪ੍ਰਤੀ_MONTH  },
                { "M",       USEC_ਪ੍ਰਤੀ_MONTH  },
                { "msec",    USEC_ਪ੍ਰਤੀ_MSEC   },
                { "ms",      USEC_ਪ੍ਰਤੀ_MSEC   },
                { "m",       USEC_ਪ੍ਰਤੀ_MINUTE },
                { "hours",   USEC_ਪ੍ਰਤੀ_HOUR   },
                { "hour",    USEC_ਪ੍ਰਤੀ_HOUR   },
                { "hr",      USEC_ਪ੍ਰਤੀ_HOUR   },
                { "h",       USEC_ਪ੍ਰਤੀ_HOUR   },
                { "days",    USEC_ਪ੍ਰਤੀ_DAY    },
                { "day",     USEC_ਪ੍ਰਤੀ_DAY    },
                { "d",       USEC_ਪ੍ਰਤੀ_DAY    },
                { "weeks",   USEC_ਪ੍ਰਤੀ_WEEK   },
                { "week",    USEC_ਪ੍ਰਤੀ_WEEK   },
                { "w",       USEC_ਪ੍ਰਤੀ_WEEK   },
                { "years",   USEC_ਪ੍ਰਤੀ_YEAR   },
                { "year",    USEC_ਪ੍ਰਤੀ_YEAR   },
                { "y",       USEC_ਪ੍ਰਤੀ_YEAR   },
                { "usec",    1ULL            },
                { "us",      1ULL            },
                { "μs",      1ULL            }, /* U+03bc (aka GREEK SMALL LETTER MU) */
                { "µs",      1ULL            }, /* U+b5 (aka MICRO SIGN) */
        };

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        FOREACH_ELEMENT(i, ਸਾਰਣੀ) {
                ਅਚਲ ਅੱਖਰ *e = startswith(p, i->suffix);
                ਜੇ (e) {
                        *ret = i->usec;
                        ਵਾਪਸ e;
                }
        }

        ਵਾਪਸ p;
}

ਪੂਰਨ parse_ਸਮਾਂ(ਅਚਲ ਅੱਖਰ *t, usec_t *ret, usec_t default_unit) {
        ਅਚਲ ਅੱਖਰ *p, *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ ਨਿਰਾ);
        s = startswith(p, "infinity");
        ਜੇ (s) {
                ਜੇ (!ਵਿੱਚ_charset(s, WHITESPACE))
                        ਵਾਪਸ -EINVAL;

                ਜੇ (ret)
                        *ret = USEC_INFINITY;
                ਵਾਪਸ 0;
        }

        usec_t usec = 0;

        ਲਈ (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                ਲੰਮਾ ਲੰਮਾ l;
                ਅੱਖਰ *e;

                p = skip_leading_chars(p, /* bad= */ ਨਿਰਾ);
                ਜੇ (*p == 0) {
                        ਜੇ (!something)
                                ਵਾਪਸ -EINVAL;

                        ਰੋਕੋ;
                }

                ਜੇ (*p == '-') /* Don't allow "-0" */
                        ਵਾਪਸ -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ਜੇ (errno > 0)
                        ਵਾਪਸ -errno;
                ਜੇ (l < 0)
                        ਵਾਪਸ -ERANGE;

                ਜੇ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } ਨਹੀਂਤਾਂ ਜੇ (e == p)
                        ਵਾਪਸ -EINVAL;
                ਨਹੀਂਤਾਂ
                        p = e;

                s = ਕੱਢੋ_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ਜੇ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ਵਾਪਸ -EINVAL;

                p = s;

                ਜੇ ((usec_t) l >= USEC_INFINITY / multiplier)
                        ਵਾਪਸ -ERANGE;

                k = (usec_t) l * multiplier;
                ਜੇ (k >= USEC_INFINITY - usec)
                        ਵਾਪਸ -ERANGE;

                usec += k;

                something = true;

                ਜੇ (*e == '.') {
                        usec_t m = multiplier / 10;
                        ਅਚਲ ਅੱਖਰ *b;

                        ਲਈ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                ਜੇ (k >= USEC_INFINITY - usec)
                                        ਵਾਪਸ -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ਜੇ (b == e + 1)
                                ਵਾਪਸ -EINVAL;
                }
        }

        ਜੇ (ret)
                *ret = usec;
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_sec(ਅਚਲ ਅੱਖਰ *t, usec_t *ret) {
        ਵਾਪਸ parse_ਸਮਾਂ(t, ret, USEC_ਪ੍ਰਤੀ_SEC);
}

ਪੂਰਨ parse_sec_fix_0(ਅਚਲ ਅੱਖਰ *t, usec_t *ret) {
        usec_t k;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = parse_sec(t, &k);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = k == 0 ? USEC_INFINITY : k;
        ਵਾਪਸ r;
}

ਪੂਰਨ parse_sec_def_infinity(ਅਚਲ ਅੱਖਰ *t, usec_t *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        t += strspn(t, WHITESPACE);
        ਜੇ (isempty(t)) {
                *ret = USEC_INFINITY;
                ਵਾਪਸ 0;
        }
        ਵਾਪਸ parse_sec(t, ret);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਕੱਢੋ_nsec_multiplier(ਅਚਲ ਅੱਖਰ *p, nsec_t *ret) {
        ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ *suffix;
                nsec_t nsec;
        } ਸਾਰਣੀ[] = {
                { "seconds", NSEC_ਪ੍ਰਤੀ_SEC    },
                { "second",  NSEC_ਪ੍ਰਤੀ_SEC    },
                { "sec",     NSEC_ਪ੍ਰਤੀ_SEC    },
                { "s",       NSEC_ਪ੍ਰਤੀ_SEC    },
                { "minutes", NSEC_ਪ੍ਰਤੀ_MINUTE },
                { "minute",  NSEC_ਪ੍ਰਤੀ_MINUTE },
                { "min",     NSEC_ਪ੍ਰਤੀ_MINUTE },
                { "months",  NSEC_ਪ੍ਰਤੀ_MONTH  },
                { "month",   NSEC_ਪ੍ਰਤੀ_MONTH  },
                { "M",       NSEC_ਪ੍ਰਤੀ_MONTH  },
                { "msec",    NSEC_ਪ੍ਰਤੀ_MSEC   },
                { "ms",      NSEC_ਪ੍ਰਤੀ_MSEC   },
                { "m",       NSEC_ਪ੍ਰਤੀ_MINUTE },
                { "hours",   NSEC_ਪ੍ਰਤੀ_HOUR   },
                { "hour",    NSEC_ਪ੍ਰਤੀ_HOUR   },
                { "hr",      NSEC_ਪ੍ਰਤੀ_HOUR   },
                { "h",       NSEC_ਪ੍ਰਤੀ_HOUR   },
                { "days",    NSEC_ਪ੍ਰਤੀ_DAY    },
                { "day",     NSEC_ਪ੍ਰਤੀ_DAY    },
                { "d",       NSEC_ਪ੍ਰਤੀ_DAY    },
                { "weeks",   NSEC_ਪ੍ਰਤੀ_WEEK   },
                { "week",    NSEC_ਪ੍ਰਤੀ_WEEK   },
                { "w",       NSEC_ਪ੍ਰਤੀ_WEEK   },
                { "years",   NSEC_ਪ੍ਰਤੀ_YEAR   },
                { "year",    NSEC_ਪ੍ਰਤੀ_YEAR   },
                { "y",       NSEC_ਪ੍ਰਤੀ_YEAR   },
                { "usec",    NSEC_ਪ੍ਰਤੀ_USEC   },
                { "us",      NSEC_ਪ੍ਰਤੀ_USEC   },
                { "μs",      NSEC_ਪ੍ਰਤੀ_USEC   }, /* U+03bc (aka GREEK LETTER MU) */
                { "µs",      NSEC_ਪ੍ਰਤੀ_USEC   }, /* U+b5 (aka MICRO SIGN) */
                { "nsec",    1ULL            },
                { "ns",      1ULL            },
                { "",        1ULL            }, /* default is nsec */
        };

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        FOREACH_ELEMENT(i, ਸਾਰਣੀ) {
                ਅਚਲ ਅੱਖਰ *e = startswith(p, i->suffix);
                ਜੇ (e) {
                        *ret = i->nsec;
                        ਵਾਪਸ e;
                }
        }

        ਵਾਪਸ p;
}

ਪੂਰਨ parse_nsec(ਅਚਲ ਅੱਖਰ *t, nsec_t *ret) {
        ਅਚਲ ਅੱਖਰ *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        ਜੇ (s) {
                s += strspn(s, WHITESPACE);
                ਜੇ (*s != 0)
                        ਵਾਪਸ -EINVAL;

                *ret = NSEC_INFINITY;
                ਵਾਪਸ 0;
        }

        ਲਈ (;;) {
                nsec_t multiplier = 1, k;
                ਲੰਮਾ ਲੰਮਾ l;
                ਅੱਖਰ *e;

                p += strspn(p, WHITESPACE);

                ਜੇ (*p == 0) {
                        ਜੇ (!something)
                                ਵਾਪਸ -EINVAL;

                        ਰੋਕੋ;
                }

                ਜੇ (*p == '-') /* Don't allow "-0" */
                        ਵਾਪਸ -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ਜੇ (errno > 0)
                        ਵਾਪਸ -errno;
                ਜੇ (l < 0)
                        ਵਾਪਸ -ERANGE;

                ਜੇ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } ਨਹੀਂਤਾਂ ਜੇ (e == p)
                        ਵਾਪਸ -EINVAL;
                ਨਹੀਂਤਾਂ
                        p = e;

                s = ਕੱਢੋ_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ਜੇ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ਵਾਪਸ -EINVAL;

                p = s;

                ਜੇ ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        ਵਾਪਸ -ERANGE;

                k = (nsec_t) l * multiplier;
                ਜੇ (k >= NSEC_INFINITY - nsec)
                        ਵਾਪਸ -ERANGE;

                nsec += k;

                something = true;

                ਜੇ (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ਅਚਲ ਅੱਖਰ *b;

                        ਲਈ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                ਜੇ (k >= NSEC_INFINITY - nsec)
                                        ਵਾਪਸ -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ਜੇ (b == e + 1)
                                ਵਾਪਸ -EINVAL;
                }
        }

        *ret = nsec;

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ get_timezones_from_zone1970_tab(ਅੱਖਰ ***ret) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_strv_free_ ਅੱਖਰ **zones = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ, *cc = ਨਿਰਾ, *co = ਨਿਰਾ, *tz = ਨਿਰਾ;

                r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਅਚਲ ਅੱਖਰ *p = ਸਤਰ;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = ਕੱਢੋ_many_ਸ਼ਬਦ(&p, ਨਿਰਾ, 0, &cc, &co, &tz);
                ਜੇ (r < 0)
                        ਜਾਰੀ;

                /* Lines that start with # are comments. */
                ਜੇ (*cc == '#')
                        ਜਾਰੀ;

                ਜੇ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ਜਾਰੀ;

                r = strv_extend(&zones, tz);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret = TAKE_PTR(zones);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ get_timezones_from_tzdata_zi(ਅੱਖਰ ***ret) {
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        _cleanup_strv_free_ ਅੱਖਰ **zones = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        ਜੇ (!f)
                ਵਾਪਸ -errno;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ, *type = ਨਿਰਾ, *f1 = ਨਿਰਾ, *f2 = ਨਿਰਾ;

                r = read_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਅਚਲ ਅੱਖਰ *p = ਸਤਰ;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = ਕੱਢੋ_many_ਸ਼ਬਦ(&p, ਨਿਰਾ, 0, &type, &f1, &f2);
                ਜੇ (r < 0)
                        ਜਾਰੀ;

                ਅੱਖਰ *tz;
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                ਨਹੀਂਤਾਂ
                        /* Not a line we care about. */
                        ਜਾਰੀ;

                ਜੇ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ਜਾਰੀ;

                r = strv_extend(&zones, tz);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret = TAKE_PTR(zones);
        ਵਾਪਸ 0;
}

ਪੂਰਨ get_timezones(ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **zones = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        ਜੇ (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                ਜੇ (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        ਜੇ (r < 0 && r != -ENOENT)
                ਵਾਪਸ r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        strv_ਛਾਂਟੋ_uniq(zones);

        *ret = TAKE_PTR(zones);
        ਵਾਪਸ 0;
}

ਪੂਰਨ verify_timezone(ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ log_level) {
        bool slash = false;
        ਅਚਲ ਅੱਖਰ *p, *t;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਅੱਖਰ buf[4];
        ਪੂਰਨ r;

        ਜੇ (isempty(name))
                ਵਾਪਸ -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        ਜੇ (streq(name, "UTC"))
                ਵਾਪਸ 0;

        ਜੇ (name[0] == '/')
                ਵਾਪਸ -EINVAL;

        ਲਈ (p = name; *p; p++) {
                ਜੇ (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !ਵਿੱਚ_ਸੈੱਟ(*p, '-', '_', '+', '/'))
                        ਵਾਪਸ -EINVAL;

                ਜੇ (*p == '/') {

                        ਜੇ (slash)
                                ਵਾਪਸ -EINVAL;

                        slash = true;
                } ਨਹੀਂਤਾਂ
                        slash = false;
        }

        ਜੇ (slash)
                ਵਾਪਸ -EINVAL;

        ਜੇ (p - name >= PATH_MAX)
                ਵਾਪਸ -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        ਜੇ (memcmp(buf, "TZif", 4) != 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        ਵਾਪਸ 0;
}

ਖਾਲੀ reset_timezonep(ਅੱਖਰ **p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        (ਖਾਲੀ) ਸੈੱਟ_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

ਅੱਖਰ* save_timezone(ਖਾਲੀ) {
        ਅਚਲ ਅੱਖਰ *e = getenv("TZ");
        ਜੇ (!e)
                ਵਾਪਸ ਨਿਰਾ;

        ਅੱਖਰ *s = strdup(e);
        ਜੇ (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        ਵਾਪਸ s;
}

bool clock_supported(clockid_t clock) {
        ਬਣਤਰ timespec ts;

        ਚੋਣ (clock) {

        ਹਾਲਤ CLOCK_MONOTONIC:
        ਹਾਲਤ CLOCK_REALTIME:
        ਹਾਲਤ CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                ਵਾਪਸ true;

        ਮੂਲ:
                /* For everything else, check properly */
                ਵਾਪਸ clock_gettime(clock, &ts) >= 0;
        }
}

ਪੂਰਨ get_timezone(ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = readlink_malloc(etc_localtime(), &t);
        ਜੇ (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, "UTC");
        ਜੇ (r < 0)
                ਵਾਪਸ r; /* Return EINVAL if not a symlink */

        ਅਚਲ ਅੱਖਰ *e = ਰਸਤਾ_STARTSWITH_ਸੈੱਟ(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        ਜੇ (!e)
                ਵਾਪਸ -EINVAL;
        ਜੇ (!timezone_is_valid(e, LOG_DEBUG))
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, e);
}

ਪੂਰਨ get_timezone_prefer_env(ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਅਚਲ ਅੱਖਰ *e = getenv("TZ");
        ਜੇ (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, e + 1);

        ਵਾਪਸ get_timezone(ret);
}

ਅਚਲ ਅੱਖਰ* etc_localtime(ਖਾਲੀ) {
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ *cached = ਨਿਰਾ;

        ਜੇ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        ਵਾਪਸ cached;
}

ਪੂਰਨ mktime_or_timegm_usec(
                ਬਣਤਰ tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        ਪ੍ਰਤਿਜ੍ਞਾ(tm);

        ਜੇ (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                ਵਾਪਸ -ERANGE;
        ਜੇ ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_ਪ੍ਰਤੀ_YEAR, (usec_t) ਸਮਾਂ_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                ਵਾਪਸ -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        ਜੇ (t < 0) /* Refuse negative times and errors */
                ਵਾਪਸ -ERANGE;
        ਜੇ ((usec_t) t >= USEC_INFINITY / USEC_ਪ੍ਰਤੀ_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                ਵਾਪਸ -ERANGE;

        ਜੇ (ret)
                *ret = (usec_t) t * USEC_ਪ੍ਰਤੀ_SEC;
        ਵਾਪਸ 0;
}

ਪੂਰਨ localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                ਬਣਤਰ tm *ret) {

        t /= USEC_ਪ੍ਰਤੀ_SEC; /* Round down */
        ਜੇ (t > (usec_t) ਸਮਾਂ_T_MAX)
                ਵਾਪਸ -ERANGE;
        time_t sec = (time_t) t;

        ਬਣਤਰ tm buf = {};
        ਜੇ (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret)
                *ret = buf;

        ਵਾਪਸ 0;
}

uint64_t sysconf_clock_ticks_cached(ਖਾਲੀ) {
        ਸਥਿਰ thread_local uint64_t hz = 0;

        ਜੇ (hz != 0)
                ਵਾਪਸ hz;

        ਲੰਮਾ t = sysconf(_SC_CLK_TCK);
        ਪ੍ਰਤਿਜ੍ਞਾ(t > 0);
        hz = (uint64_t) t;

        ਵਾਪਸ hz;
}

uint32_t usec_ਮੰਜ਼ਿਲ_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ਵਾਪਸ DIV_ਗੇੜਾ_ਉੱਪਰ(u, USEC_ਪ੍ਰਤੀ_SEC / hz);
}

usec_t jiffies_ਮੰਜ਼ਿਲ_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ਵਾਪਸ DIV_ਗੇੜਾ_ਉੱਪਰ(j * USEC_ਪ੍ਰਤੀ_SEC, hz);
}

usec_t usec_ਖਿਸਕਾਓ_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        ਜੇ (x == USEC_INFINITY)
                ਵਾਪਸ USEC_INFINITY;
        ਜੇ (map_clock_id(from) == map_clock_id(to))
                ਵਾਪਸ x;

        a = now(from);
        b = now(to);

        ਜੇ (x > a)
                /* x lies in the future */
                ਵਾਪਸ usec_ਜੋੜੋ(b, usec_sub_unsigned(x, a));
        ਨਹੀਂਤਾਂ
                /* x lies in the past */
                ਵਾਪਸ usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool ਵਿੱਚ_utc_timezone(ਖਾਲੀ) {
        tzset();

        ਵਾਪਸ timezone == 0 && daylight == 0;
}

ਪੂਰਨ usleep_ਸੁਰੱਖਿਅਤ(usec_t usec) {
        ਪੂਰਨ r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        ਜੇ (usec == 0)
                ਵਾਪਸ 0;

        ਜੇ (usec == USEC_INFINITY)
                ਵਾਪਸ RET_NERRNO(pause());

        ਬਣਤਰ timespec t;
        timespec_store(&t, usec);

        ਲਈ (;;) {
                ਬਣਤਰ timespec ਬਚਿਆ;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &ਬਚਿਆ);
                ਜੇ (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = ਬਚਿਆ;
                        ਜਾਰੀ;
                }

                ਵਾਪਸ r;
        }
}

ਪੂਰਨ ਸਮਾਂ_change_fd(ਖਾਲੀ) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        ਸਥਿਰ ਅਚਲ ਬਣਤਰ itimerspec its = {
                .it_value.tv_sec = ਸਮਾਂ_T_MAX,
        };

        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        ਜੇ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, ਨਿਰਾ) >= 0)
                ਵਾਪਸ TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#ਜੇ SIZEOF_ਸਮਾਂ_T == 8 && ULONG_MAX < UINT64_MAX
        ਜੇ (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                ਸਥਿਰ ਅਚਲ ਬਣਤਰ itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                ਜੇ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, ਨਿਰਾ) >= 0)
                        ਵਾਪਸ TAKE_FD(fd);
        }
#ਜੇ_ਅੰਤ

        ਵਾਪਸ -errno;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ timestamp_style_ਸਾਰਣੀ[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ਅਚਲ ਅੱਖਰ *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_ਸਾਰਣੀ_lookup_from_string(timestamp_style_ਸਾਰਣੀ, ELEMENTSOF(timestamp_style_ਸਾਰਣੀ), s);
        ਜੇ (t >= 0)
                ਵਾਪਸ t;
        ਜੇ (STRPTR_ਵਿੱਚ_ਸੈੱਟ(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                ਵਾਪਸ TIMESTAMP_US;
        ਜੇ (STRPTR_ਵਿੱਚ_ਸੈੱਟ(s, "µs+utc", "μs+utc"))
                ਵਾਪਸ TIMESTAMP_US_UTC;
        ਵਾਪਸ t;
}

ਪੂਰਨ parse_calendar_date_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, bool allow_pre_epoch, usec_t *ret_usec, ਬਣਤਰ tm *ret_tm) {
        ਬਣਤਰ tm parsed_tm = {}, copy_tm;
        ਅਚਲ ਅੱਖਰ *k;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        ਜੇ (!k || *k)
                ਵਾਪਸ -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        ਜੇ (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                ਜੇ (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        ਵਾਪਸ -EINVAL;
        } ਨਹੀਂਤਾਂ {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        ਜੇ (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret_usec)
                *ret_usec = usec;
        ਜੇ (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        ਵਾਪਸ 0;
}
