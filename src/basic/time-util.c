/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/time_types.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/timerfd.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"          /* IWYU pragma: keep */
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

ಸ್ಥಿರ clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        ಆಯ್ಕೆ (c) {

        ಸ್ಥಿತಿ CLOCK_BOOTTIME_ALARM:
                ಹಿಂತಿರುಗಿ CLOCK_BOOTTIME;

        ಸ್ಥಿತಿ CLOCK_REALTIME_ALARM:
                ಹಿಂತಿರುಗಿ CLOCK_REALTIME;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ c;
        }
}

usec_t now(clockid_t clock_id) {
        ರಚನೆ timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_ತುಂಬು(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        ಪ್ರತಿಜ್ಞಾ(n > 0);
        ಪ್ರತಿಜ್ಞಾ(n < USEC_INFINITY);

        ಹಿಂತಿರುಗಿ n;
}

nsec_t now_nsec(clockid_t clock_id) {
        ರಚನೆ timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_ತುಂಬು_nsec(&ts);

        ಪ್ರತಿಜ್ಞಾ(n > 0);
        ಪ್ರತಿಜ್ಞಾ(n < NSEC_INFINITY);

        ಹಿಂತಿರುಗಿ n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        ಹಿಂತಿರುಗಿ ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        ಹಿಂತಿರುಗಿ ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_ತಳ, usec_t ಗಮ್ಯ_ತಳ) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        ಆದರೆ (from >= from_ತಳ) { /* In the future */
                usec_t delta = from - from_ತಳ;

                ಆದರೆ (ಗಮ್ಯ_ತಳ >= USEC_INFINITY - delta) /* overflow? */
                        ಹಿಂತಿರುಗಿ USEC_INFINITY;

                ಹಿಂತಿರುಗಿ ಗಮ್ಯ_ತಳ + delta;

        } ಇಲ್ಲದಿದ್ದರೆ { /* In the past */
                usec_t delta = from_ತಳ - from;

                ಆದರೆ (ಗಮ್ಯ_ತಳ <= delta) /* underflow? */
                        ಹಿಂತಿರುಗಿ 0;

                ಹಿಂತಿರುಗಿ ಗಮ್ಯ_ತಳ - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t ಗಮ್ಯ_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        ಆದರೆ (map_clock_id(from_clock) == map_clock_id(ಗಮ್ಯ_clock))
                ಹಿಂತಿರುಗಿ from;

        /* Keep infinity as is */
        ಆದರೆ (from == USEC_INFINITY)
                ಹಿಂತಿರುಗಿ from;

        ಹಿಂತಿರುಗಿ map_clock_usec_raw(from, now(from_clock), now(ಗಮ್ಯ_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (!timestamp_is_ಗಣ(u)) {
                ts->realtime = ts->monotonic = u;
                ಹಿಂತಿರುಗಿ ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        ಹಿಂತಿರುಗಿ ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (!timestamp_is_ಗಣ(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ಹಿಂತಿರುಗಿ ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        ಹಿಂತಿರುಗಿ ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                ಹಿಂತಿರುಗಿ ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        ಹಿಂತಿರುಗಿ ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ಹಿಂತಿರುಗಿ ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        ಹಿಂತಿರುಗಿ ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                ಹಿಂತಿರುಗಿ ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        ಹಿಂತಿರುಗಿ ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆಯ್ಕೆ (clock) {

        ಸ್ಥಿತಿ CLOCK_REALTIME:
        ಸ್ಥಿತಿ CLOCK_REALTIME_ALARM:
                ಹಿಂತಿರುಗಿ ts->realtime;

        ಸ್ಥಿತಿ CLOCK_MONOTONIC:
                ಹಿಂತಿರುಗಿ ts->monotonic;

        ಸ್ಥಿತಿ CLOCK_BOOTTIME:
        ಸ್ಥಿತಿ CLOCK_BOOTTIME_ALARM:
                ಹಿಂತಿರುಗಿ ts->boottime;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ USEC_INFINITY;
        }
}

usec_t timespec_ತುಂಬು(ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ಹಿಂತಿರುಗಿ USEC_INFINITY;

        ಆದರೆ ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_ಪ್ರತಿ_USEC)) / USEC_ಪ್ರತಿ_SEC)
                ಹಿಂತಿರುಗಿ USEC_INFINITY;

        ಹಿಂತಿರುಗಿ
                (usec_t) ts->tv_sec * USEC_ಪ್ರತಿ_SEC +
                (usec_t) ts->tv_nsec / NSEC_ಪ್ರತಿ_USEC;
}

nsec_t timespec_ತುಂಬು_nsec(ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (ts->tv_sec < 0 || ts->tv_nsec < 0)
                ಹಿಂತಿರುಗಿ NSEC_INFINITY;

        ಆದರೆ ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_ಪ್ರತಿ_SEC)
                ಹಿಂತಿರುಗಿ NSEC_INFINITY;

        ಹಿಂತಿರುಗಿ (nsec_t) ts->tv_sec * NSEC_ಪ್ರತಿ_SEC + (nsec_t) ts->tv_nsec;
}

ರಚನೆ timespec *timespec_store(ರಚನೆ timespec *ts, usec_t u) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (u == USEC_INFINITY ||
            u / USEC_ಪ್ರತಿ_SEC >= ಸಮಯ_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ಹಿಂತಿರುಗಿ ts;
        }

        ts->tv_sec = (time_t) (u / USEC_ಪ್ರತಿ_SEC);
        ts->tv_nsec = (ದೀರ್ಘ) ((u % USEC_ಪ್ರತಿ_SEC) * NSEC_ಪ್ರತಿ_USEC);

        ಹಿಂತಿರುಗಿ ts;
}

ರಚನೆ timespec *timespec_store_nsec(ರಚನೆ timespec *ts, nsec_t n) {
        ಪ್ರತಿಜ್ಞಾ(ts);

        ಆದರೆ (n == NSEC_INFINITY ||
            n / NSEC_ಪ್ರತಿ_SEC >= ಸಮಯ_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                ಹಿಂತಿರುಗಿ ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_ಪ್ರತಿ_SEC);
        ts->tv_nsec = (ದೀರ್ಘ) (n % NSEC_ಪ್ರತಿ_SEC);

        ಹಿಂತಿರುಗಿ ts;
}

ರಚನೆ __kernel_timespec *kernel_timespec_store(ರಚನೆ __kernel_timespec *ts, usec_t u) {
        ಪ್ರತಿಜ್ಞಾ(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        ಪ್ರತಿಜ್ಞಾ(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_ಪ್ರತಿ_SEC;
        ts->tv_nsec = (u % USEC_ಪ್ರತಿ_SEC) * NSEC_ಪ್ರತಿ_USEC;

        ಹಿಂತಿರುಗಿ ts;
}

usec_t timeval_ತುಂಬು(ಸ್ಥಿರಾಂಕ ರಚನೆ timeval *tv) {
        ಪ್ರತಿಜ್ಞಾ(tv);

        ಆದರೆ (tv->tv_sec < 0 || tv->tv_usec < 0)
                ಹಿಂತಿರುಗಿ USEC_INFINITY;

        ಆದರೆ ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_ಪ್ರತಿ_SEC)
                ಹಿಂತಿರುಗಿ USEC_INFINITY;

        ಹಿಂತಿರುಗಿ
                (usec_t) tv->tv_sec * USEC_ಪ್ರತಿ_SEC +
                (usec_t) tv->tv_usec;
}

ರಚನೆ timeval *timeval_store(ರಚನೆ timeval *tv, usec_t u) {
        ಪ್ರತಿಜ್ಞಾ(tv);

        ಆದರೆ (u == USEC_INFINITY ||
            u / USEC_ಪ್ರತಿ_SEC > ಸಮಯ_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } ಇಲ್ಲದಿದ್ದರೆ {
                tv->tv_sec = (time_t) (u / USEC_ಪ್ರತಿ_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_ಪ್ರತಿ_SEC);
        }

        ಹಿಂತಿರುಗಿ tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ weekday_ಕೋಷ್ಟಕ[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(weekday, ಪೂರ್ಣಾಂಕ);

ಅಕ್ಷರ* format_timestamp_style(
                ಅಕ್ಷರ *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        ರಚನೆ tm tm;
        bool utc, us;
        size_t n;

        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(style >= 0);
        ಪ್ರತಿಜ್ಞಾ(style < _TIMESTAMP_STYLE_MAX);

        ಆದರೆ (!timestamp_is_ಗಣ(t))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* Timestamp is unset */

        ಆದರೆ (style == TIMESTAMP_UNIX) {
                ಆದರೆ (l < (size_t) (1 + 1 + 1))
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* not enough space for even the shortest of forms */

                ಹಿಂತಿರುಗಿ snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_ಪ್ರತಿ_SEC);  /* round down μs → s */
        }

        utc = ಒಳ_ಗಣ(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = ಒಳ_ಗಣ(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        ಆದರೆ (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        ಆದರೆ (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                ಪ್ರತಿಜ್ಞಾ(l >= ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(xxx[style]) + 1);
                ಹಿಂತಿರುಗಿ strcpy(buf, xxx[style]);
        }

        ಆದರೆ (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        /* Start with the week day */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *weekday = weekday_ಗಮ್ಯ_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        ಪ್ರತಿಜ್ಞಾ(weekday);
        ಸ್ಮರಣೆ_ನಕಲು(buf, weekday, 4);

        ಆದರೆ (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                ಆದರೆ (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* Doesn't fit */

                ಹಿಂತಿರುಗಿ buf;
        }

        /* Add the main components */
        ಆದರೆ (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        ಆದರೆ (us) {
                n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
                ಆದರೆ (n + 8 > l)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_ಪ್ರತಿ_SEC);
        }

        /* Append the timezone */
        n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
        ಆದರೆ (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                ಆದರೆ (n + 5 > l)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tm.tm_zone);
                ಆದರೆ (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        ಆದರೆ (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                ಹಿಂತಿರುಗಿ ಖಾಲಿ; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } ಇಲ್ಲದಿದ್ದರೆ {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        ಹಿಂತಿರುಗಿ buf;
}

ಅಕ್ಷರ* format_timestamp_relative_ತುಂಬಿದ(ಅಕ್ಷರ *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ಎಡ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
        usec_t n, d;

        ಪ್ರತಿಜ್ಞಾ(buf);

        ಆದರೆ (!timestamp_is_ಗಣ(t))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        n = now(clock);
        ಆದರೆ (n > t) {
                d = n - t;
                s = " ago";
        } ಇಲ್ಲದಿದ್ದರೆ {
                d = t - n;
                s = implicit_ಎಡ ? "" : " left";
        }

        ಆದರೆ (d >= USEC_ಪ್ರತಿ_YEAR) {
                usec_t years = d / USEC_ಪ್ರತಿ_YEAR;
                usec_t months = (d % USEC_ಪ್ರತಿ_YEAR) / USEC_ಪ್ರತಿ_MONTH;

                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= USEC_ಪ್ರತಿ_MONTH) {
                usec_t months = d / USEC_ಪ್ರತಿ_MONTH;
                usec_t days = (d % USEC_ಪ್ರತಿ_MONTH) / USEC_ಪ್ರತಿ_DAY;

                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= USEC_ಪ್ರತಿ_WEEK) {
                usec_t weeks = d / USEC_ಪ್ರತಿ_WEEK;
                usec_t days = (d % USEC_ಪ್ರತಿ_WEEK) / USEC_ಪ್ರತಿ_DAY;

                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= 2*USEC_ಪ್ರತಿ_DAY)
                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT " days%s", d / USEC_ಪ್ರತಿ_DAY,s);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= 25*USEC_ಪ್ರತಿ_HOUR)
                (ಶೂನ್ಯ) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_ಪ್ರತಿ_DAY) / USEC_ಪ್ರತಿ_HOUR, s);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= 6*USEC_ಪ್ರತಿ_HOUR)
                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_ಪ್ರತಿ_HOUR, s);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= USEC_ಪ್ರತಿ_HOUR)
                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_ಪ್ರತಿ_HOUR,
                                (d % USEC_ಪ್ರತಿ_HOUR) / USEC_ಪ್ರತಿ_MINUTE, s);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= 5*USEC_ಪ್ರತಿ_MINUTE)
                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_ಪ್ರತಿ_MINUTE, s);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= USEC_ಪ್ರತಿ_MINUTE)
                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_ಪ್ರತಿ_MINUTE,
                                (d % USEC_ಪ್ರತಿ_MINUTE) / USEC_ಪ್ರತಿ_SEC, s);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= USEC_ಪ್ರತಿ_SEC)
                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_ಪ್ರತಿ_SEC, s);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d >= USEC_ಪ್ರತಿ_MSEC)
                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_ಪ್ರತಿ_MSEC, s);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (d > 0)
                (ಶೂನ್ಯ) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        ಇಲ್ಲದಿದ್ದರೆ
                (ಶೂನ್ಯ) snprintf(buf, l, "now");

        buf[l-1] = 0;
        ಹಿಂತಿರುಗಿ buf;
}

ಅಕ್ಷರ* format_timespan(ಅಕ್ಷರ *buf, size_t l, usec_t t, usec_t accuracy) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix;
                usec_t usec;
        } ಕೋಷ್ಟಕ[] = {
                { "y",     USEC_ಪ್ರತಿ_YEAR   },
                { "month", USEC_ಪ್ರತಿ_MONTH  },
                { "w",     USEC_ಪ್ರತಿ_WEEK   },
                { "d",     USEC_ಪ್ರತಿ_DAY    },
                { "h",     USEC_ಪ್ರತಿ_HOUR   },
                { "min",   USEC_ಪ್ರತಿ_MINUTE },
                { "s",     USEC_ಪ್ರತಿ_SEC    },
                { "ms",    USEC_ಪ್ರತಿ_MSEC   },
                { "us",    1               },
        };

        ಅಕ್ಷರ *p = ASSERT_PTR(buf);
        bool something = false;

        ಪ್ರತಿಜ್ಞಾ(l > 0);

        ಆದರೆ (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                ಹಿಂತಿರುಗಿ p;
        }

        ಆದರೆ (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                ಹಿಂತಿರುಗಿ p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, ಕೋಷ್ಟಕ) {
                ಪೂರ್ಣಾಂಕ k = 0;
                size_t n;
                bool ಮುಗಿದ = false;
                usec_t a, b;

                ಆದರೆ (t <= 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (t < accuracy && something)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (t < i->usec)
                        ಮುಂದುವರಿ;

                ಆದರೆ (l <= 1)
                        ನಿಲ್ಲಿಸು;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                ಆದರೆ (t < USEC_ಪ್ರತಿ_MINUTE && b > 0) {
                        ಚಿಹ್ನೆಯುಳ್ಳ ಅಕ್ಷರ j = 0;

                        ಗಾಗಿ (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        ಗಾಗಿ (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        ಆದರೆ (j > 0) {
                                k = snprintf(p, l,
                                             "%s"USEC_FMT".%0*"PRI_USEC"%s",
                                             p > buf ? " " : "",
                                             a,
                                             j,
                                             b,
                                             i->suffix);

                                t = 0;
                                ಮುಗಿದ = true;
                        }
                }

                /* No? Then let's show it normally */
                ಆದರೆ (!ಮುಗಿದ) {
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

        ಹಿಂತಿರುಗಿ buf;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        ಆದರೆ (dst && isempty(tzname[1]))
                dst = false;

        ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_ಗಮ್ಯ_null(tzname[dst]);
}

ಪೂರ್ಣಾಂಕ parse_gmtoff(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, ದೀರ್ಘ *ret) {
        ಪ್ರತಿಜ್ಞಾ(t);

        ರಚನೆ tm tm;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k = strptime(t, "%z", &tm);
        ಆದರೆ (k && *k == '\0') {
                /* Success! */
                ಆದರೆ (ret)
                        *ret = tm.tm_gmtoff;
                ಹಿಂತಿರುಗಿ 0;
        }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GLIBC__
        ಹಿಂತಿರುಗಿ -EINVAL;
#ಇಲ್ಲದಿದ್ದರೆ
        ಪೂರ್ಣಾಂಕ r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool ಧನಾತ್ಮಕ;
        ಆಯ್ಕೆ (*t) {
        ಸ್ಥಿತಿ '+':
                ಧನಾತ್ಮಕ = true;
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ '-':
                ಧನಾತ್ಮಕ = false;
                ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EINVAL;
        }

        t++;
        r = undecchar(*t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        usec_t u = r * 10 * USEC_ಪ್ರತಿ_HOUR;

        t++;
        r = undecchar(*t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        u += r * USEC_ಪ್ರತಿ_HOUR;

        t++;
        ಆದರೆ (*t == '\0') /* 2 digits case */
                ಹೋಗು finalize;

        ಆದರೆ (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r >= 6) /* refuse minutes equal to or larger than 60 */
                ಹಿಂತಿರುಗಿ -EINVAL;

        u += r * 10 * USEC_ಪ್ರತಿ_MINUTE;

        t++;
        r = undecchar(*t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        u += r * USEC_ಪ್ರತಿ_MINUTE;

        t++;
        ಆದರೆ (*t != '\0')
                ಹಿಂತಿರುಗಿ -EINVAL;

finalize:
        ಆದರೆ (u > USEC_ಪ್ರತಿ_DAY) /* refuse larger than one day */
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret) {
                ದೀರ್ಘ gmtoff = u / USEC_ಪ್ರತಿ_SEC;
                *ret = ಧನಾತ್ಮಕ ? gmtoff : -gmtoff;
        }

        ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_timestamp_impl(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t,
                size_t max_len,
                bool utc,
                ಪೂರ್ಣಾಂಕ isdst,
                ದೀರ್ಘ gmtoff,
                usec_t *ret) {

        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
                ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nr;
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

        _cleanup_free_ ಅಕ್ಷರ *t_alloc = ಖಾಲಿ;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        ಪೂರ್ಣಾಂಕ r, weekday = -1;
        ಚಿಹ್ನೆರಹಿತ fractional = 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k;
        ರಚನೆ tm tm, copy;

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

        ಪ್ರತಿಜ್ಞಾ(t);

        ಆದರೆ (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                ಆದರೆ (max_len == 0) /* Can't be the only field */
                        ಹಿಂತಿರುಗಿ -EINVAL;

                t_alloc = strndup(t, max_len);
                ಆದರೆ (!t_alloc)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        ಆದರೆ (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                ಆದರೆ ((usec_t) labs(gmtoff) * USEC_ಪ್ರತಿ_SEC > USEC_ಪ್ರತಿ_DAY)
                        ಹಿಂತಿರುಗಿ -EINVAL;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (gmtoff != 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಆದರೆ (t[0] == '@' && !with_tz)
                ಹಿಂತಿರುಗಿ parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        ಆದರೆ (!with_tz) {
                ಆದರೆ (streq(t, "now"))
                        ಹೋಗು ಮುಕ್ತಾಯ;

                ಆದರೆ (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ಹೋಗು ಮುಕ್ತಾಯ;
                }

                ಆದರೆ (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ಹೋಗು ಮುಕ್ತಾಯ;
                }

                ಆದರೆ ((k = endswith(t, " ago"))) {
                        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;

                        buf = strndup(t, k - t);
                        ಆದರೆ (!buf)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        r = parse_sec(buf, &minus);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ಹೋಗು ಮುಕ್ತಾಯ;
                }

                ಆದರೆ ((k = endswith(t, " left"))) {
                        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;

                        buf = strndup(t, k - t);
                        ಆದರೆ (!buf)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        r = parse_sec(buf, &plus);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ಹೋಗು ಮುಕ್ತಾಯ;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        tm.tm_isdst = isdst;

        ಆದರೆ (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ಹೋಗು from_tm;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ಹೋಗು from_tm;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ಹೋಗು from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                ಆದರೆ (!k || *k != ' ')
                        ಮುಂದುವರಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#ಆದರೆ_ಅಂತ್ಯ
                t = k + 1;
                ನಿಲ್ಲಿಸು;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        ಆದರೆ (k) {
                ಆದರೆ (*k == '.')
                        ಹೋಗು parse_usec;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*k == 0)
                        ಹೋಗು from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        ಆದರೆ (k) {
                ಆದರೆ (*k == '.')
                        ಹೋಗು parse_usec;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*k == 0)
                        ಹೋಗು from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        ಆದರೆ (k) {
                ಆದರೆ (*k == '.')
                        ಹೋಗು parse_usec;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*k == 0)
                        ಹೋಗು from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        ಆದರೆ (k) {
                ಆದರೆ (*k == '.')
                        ಹೋಗು parse_usec;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*k == 0)
                        ಹೋಗು from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        ಆದರೆ (k && *k == 0) {
                tm.tm_sec = 0;
                ಹೋಗು from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        ಆದರೆ (k && *k == 0) {
                tm.tm_sec = 0;
                ಹೋಗು from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        ಆದರೆ (k && *k == 0) {
                tm.tm_sec = 0;
                ಹೋಗು from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        ಆದರೆ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ಹೋಗು from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        ಆದರೆ (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                ಹೋಗು from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        ಆದರೆ (k) {
                ಆದರೆ (*k == '.')
                        ಹೋಗು parse_usec;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*k == 0)
                        ಹೋಗು from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        ಆದರೆ (k && *k == 0) {
                tm.tm_sec = 0;
                ಹೋಗು from_tm;
        }

        ಹಿಂತಿರುಗಿ -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (*k != '\0')
                ಹಿಂತಿರುಗಿ -EINVAL;

from_tm:
        ಪ್ರತಿಜ್ಞಾ(plus == 0);
        ಪ್ರತಿಜ್ಞಾ(minus == 0);

        ಆದರೆ (weekday >= 0 && tm.tm_wday != weekday)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (gmtoff < 0) {
                plus = -gmtoff * USEC_ಪ್ರತಿ_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                ಆದರೆ (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_ಪ್ರತಿ_DAY;
                }
        } ಇಲ್ಲದಿದ್ದರೆ
                minus = gmtoff * USEC_ಪ್ರತಿ_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        usec = usec_ಕೂಡಿಸು(usec, fractional);

ಮುಕ್ತಾಯ:
        usec = usec_ಕೂಡಿಸು(usec, plus);

        ಆದರೆ (usec < minus)
                ಹಿಂತಿರುಗಿ -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        ಆದರೆ (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret)
                *ret = usec;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_timestamp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret) {
        ದೀರ್ಘ gmtoff;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);

        size_t t_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(t);
        ಆದರೆ (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        ಆದರೆ (t_len > 7 && ಒಳ_ಗಣ(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                ಹಿಂತಿರುಗಿ parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tz = strrchr(t, ' ');
        ಆದರೆ (!tz)
                ಹಿಂತಿರುಗಿ parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        ಆದರೆ (streq(tz, "UTC"))
                ಹಿಂತಿರುಗಿ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        ಆದರೆ (parse_gmtoff(tz, &gmtoff) >= 0)
                ಹಿಂತಿರುಗಿ parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j <= 1; j++) {
                ಆದರೆ (!streq_ptr(tz, get_tzname(j)))
                        ಮುಂದುವರಿ;

                /* The specified timezone matches tzname[] of the local timezone. */
                ಹಿಂತಿರುಗಿ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        ಆದರೆ (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                ಆದರೆ (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        ಹಿಂತಿರುಗಿ negative_errno();

                ಹಿಂತಿರುಗಿ parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        ಹಿಂತಿರುಗಿ parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಹೊರತೆಗೆ_multiplier(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, usec_t *ret) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix;
                usec_t usec;
        } ಕೋಷ್ಟಕ[] = {
                { "seconds", USEC_ಪ್ರತಿ_SEC    },
                { "second",  USEC_ಪ್ರತಿ_SEC    },
                { "sec",     USEC_ಪ್ರತಿ_SEC    },
                { "s",       USEC_ಪ್ರತಿ_SEC    },
                { "minutes", USEC_ಪ್ರತಿ_MINUTE },
                { "minute",  USEC_ಪ್ರತಿ_MINUTE },
                { "min",     USEC_ಪ್ರತಿ_MINUTE },
                { "months",  USEC_ಪ್ರತಿ_MONTH  },
                { "month",   USEC_ಪ್ರತಿ_MONTH  },
                { "M",       USEC_ಪ್ರತಿ_MONTH  },
                { "msec",    USEC_ಪ್ರತಿ_MSEC   },
                { "ms",      USEC_ಪ್ರತಿ_MSEC   },
                { "m",       USEC_ಪ್ರತಿ_MINUTE },
                { "hours",   USEC_ಪ್ರತಿ_HOUR   },
                { "hour",    USEC_ಪ್ರತಿ_HOUR   },
                { "hr",      USEC_ಪ್ರತಿ_HOUR   },
                { "h",       USEC_ಪ್ರತಿ_HOUR   },
                { "days",    USEC_ಪ್ರತಿ_DAY    },
                { "day",     USEC_ಪ್ರತಿ_DAY    },
                { "d",       USEC_ಪ್ರತಿ_DAY    },
                { "weeks",   USEC_ಪ್ರತಿ_WEEK   },
                { "week",    USEC_ಪ್ರತಿ_WEEK   },
                { "w",       USEC_ಪ್ರತಿ_WEEK   },
                { "years",   USEC_ಪ್ರತಿ_YEAR   },
                { "year",    USEC_ಪ್ರತಿ_YEAR   },
                { "y",       USEC_ಪ್ರತಿ_YEAR   },
                { "usec",    1ULL            },
                { "us",      1ULL            },
                { "μs",      1ULL            }, /* U+03bc (aka GREEK SMALL LETTER MU) */
                { "µs",      1ULL            }, /* U+b5 (aka MICRO SIGN) */
        };

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        FOREACH_ELEMENT(i, ಕೋಷ್ಟಕ) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = startswith(p, i->suffix);
                ಆದರೆ (e) {
                        *ret = i->usec;
                        ಹಿಂತಿರುಗಿ e;
                }
        }

        ಹಿಂತಿರುಗಿ p;
}

ಪೂರ್ಣಾಂಕ parse_ಸಮಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret, usec_t default_unit) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *s;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ ಖಾಲಿ);
        s = startswith(p, "infinity");
        ಆದರೆ (s) {
                ಆದರೆ (!ಒಳ_charset(s, WHITESPACE))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (ret)
                        *ret = USEC_INFINITY;
                ಹಿಂತಿರುಗಿ 0;
        }

        usec_t usec = 0;

        ಗಾಗಿ (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                ದೀರ್ಘ ದೀರ್ಘ l;
                ಅಕ್ಷರ *e;

                p = skip_leading_chars(p, /* bad= */ ಖಾಲಿ);
                ಆದರೆ (*p == 0) {
                        ಆದರೆ (!something)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (*p == '-') /* Don't allow "-0" */
                        ಹಿಂತಿರುಗಿ -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ಆದರೆ (errno > 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (l < 0)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                ಆದರೆ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e == p)
                        ಹಿಂತಿರುಗಿ -EINVAL;
                ಇಲ್ಲದಿದ್ದರೆ
                        p = e;

                s = ಹೊರತೆಗೆ_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ಆದರೆ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ಹಿಂತಿರುಗಿ -EINVAL;

                p = s;

                ಆದರೆ ((usec_t) l >= USEC_INFINITY / multiplier)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                k = (usec_t) l * multiplier;
                ಆದರೆ (k >= USEC_INFINITY - usec)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                usec += k;

                something = true;

                ಆದರೆ (*e == '.') {
                        usec_t m = multiplier / 10;
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b;

                        ಗಾಗಿ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                ಆದರೆ (k >= USEC_INFINITY - usec)
                                        ಹಿಂತಿರುಗಿ -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ಆದರೆ (b == e + 1)
                                ಹಿಂತಿರುಗಿ -EINVAL;
                }
        }

        ಆದರೆ (ret)
                *ret = usec;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_sec(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret) {
        ಹಿಂತಿರುಗಿ parse_ಸಮಯ(t, ret, USEC_ಪ್ರತಿ_SEC);
}

ಪೂರ್ಣಾಂಕ parse_sec_fix_0(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret) {
        usec_t k;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_sec(t, &k);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = k == 0 ? USEC_INFINITY : k;
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ parse_sec_def_infinity(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, usec_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(ret);

        t += strspn(t, WHITESPACE);
        ಆದರೆ (isempty(t)) {
                *ret = USEC_INFINITY;
                ಹಿಂತಿರುಗಿ 0;
        }
        ಹಿಂತಿರುಗಿ parse_sec(t, ret);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಹೊರತೆಗೆ_nsec_multiplier(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, nsec_t *ret) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix;
                nsec_t nsec;
        } ಕೋಷ್ಟಕ[] = {
                { "seconds", NSEC_ಪ್ರತಿ_SEC    },
                { "second",  NSEC_ಪ್ರತಿ_SEC    },
                { "sec",     NSEC_ಪ್ರತಿ_SEC    },
                { "s",       NSEC_ಪ್ರತಿ_SEC    },
                { "minutes", NSEC_ಪ್ರತಿ_MINUTE },
                { "minute",  NSEC_ಪ್ರತಿ_MINUTE },
                { "min",     NSEC_ಪ್ರತಿ_MINUTE },
                { "months",  NSEC_ಪ್ರತಿ_MONTH  },
                { "month",   NSEC_ಪ್ರತಿ_MONTH  },
                { "M",       NSEC_ಪ್ರತಿ_MONTH  },
                { "msec",    NSEC_ಪ್ರತಿ_MSEC   },
                { "ms",      NSEC_ಪ್ರತಿ_MSEC   },
                { "m",       NSEC_ಪ್ರತಿ_MINUTE },
                { "hours",   NSEC_ಪ್ರತಿ_HOUR   },
                { "hour",    NSEC_ಪ್ರತಿ_HOUR   },
                { "hr",      NSEC_ಪ್ರತಿ_HOUR   },
                { "h",       NSEC_ಪ್ರತಿ_HOUR   },
                { "days",    NSEC_ಪ್ರತಿ_DAY    },
                { "day",     NSEC_ಪ್ರತಿ_DAY    },
                { "d",       NSEC_ಪ್ರತಿ_DAY    },
                { "weeks",   NSEC_ಪ್ರತಿ_WEEK   },
                { "week",    NSEC_ಪ್ರತಿ_WEEK   },
                { "w",       NSEC_ಪ್ರತಿ_WEEK   },
                { "years",   NSEC_ಪ್ರತಿ_YEAR   },
                { "year",    NSEC_ಪ್ರತಿ_YEAR   },
                { "y",       NSEC_ಪ್ರತಿ_YEAR   },
                { "usec",    NSEC_ಪ್ರತಿ_USEC   },
                { "us",      NSEC_ಪ್ರತಿ_USEC   },
                { "μs",      NSEC_ಪ್ರತಿ_USEC   }, /* U+03bc (aka GREEK LETTER MU) */
                { "µs",      NSEC_ಪ್ರತಿ_USEC   }, /* U+b5 (aka MICRO SIGN) */
                { "nsec",    1ULL            },
                { "ns",      1ULL            },
                { "",        1ULL            }, /* default is nsec */
        };

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        FOREACH_ELEMENT(i, ಕೋಷ್ಟಕ) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = startswith(p, i->suffix);
                ಆದರೆ (e) {
                        *ret = i->nsec;
                        ಹಿಂತಿರುಗಿ e;
                }
        }

        ಹಿಂತಿರುಗಿ p;
}

ಪೂರ್ಣಾಂಕ parse_nsec(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, nsec_t *ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        ಆದರೆ (s) {
                s += strspn(s, WHITESPACE);
                ಆದರೆ (*s != 0)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                *ret = NSEC_INFINITY;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಗಾಗಿ (;;) {
                nsec_t multiplier = 1, k;
                ದೀರ್ಘ ದೀರ್ಘ l;
                ಅಕ್ಷರ *e;

                p += strspn(p, WHITESPACE);

                ಆದರೆ (*p == 0) {
                        ಆದರೆ (!something)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (*p == '-') /* Don't allow "-0" */
                        ಹಿಂತಿರುಗಿ -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                ಆದರೆ (errno > 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (l < 0)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                ಆದರೆ (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (e == p)
                        ಹಿಂತಿರುಗಿ -EINVAL;
                ಇಲ್ಲದಿದ್ದರೆ
                        p = e;

                s = ಹೊರತೆಗೆ_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                ಆದರೆ (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        ಹಿಂತಿರುಗಿ -EINVAL;

                p = s;

                ಆದರೆ ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                k = (nsec_t) l * multiplier;
                ಆದರೆ (k >= NSEC_INFINITY - nsec)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                nsec += k;

                something = true;

                ಆದರೆ (*e == '.') {
                        nsec_t m = multiplier / 10;
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b;

                        ಗಾಗಿ (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                ಆದರೆ (k >= NSEC_INFINITY - nsec)
                                        ಹಿಂತಿರುಗಿ -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        ಆದರೆ (b == e + 1)
                                ಹಿಂತಿರುಗಿ -EINVAL;
                }
        }

        *ret = nsec;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_timezones_from_zone1970_tab(ಅಕ್ಷರ ***ret) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_strv_free_ ಅಕ್ಷರ **zones = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ, *cc = ಖಾಲಿ, *co = ಖಾಲಿ, *tz = ಖಾಲಿ;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಸಾಲು;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = ಹೊರತೆಗೆ_many_ಪದಗಳು(&p, ಖಾಲಿ, 0, &cc, &co, &tz);
                ಆದರೆ (r < 0)
                        ಮುಂದುವರಿ;

                /* Lines that start with # are comments. */
                ಆದರೆ (*cc == '#')
                        ಮುಂದುವರಿ;

                ಆದರೆ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ಮುಂದುವರಿ;

                r = strv_extend(&zones, tz);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret = TAKE_PTR(zones);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_timezones_from_tzdata_zi(ಅಕ್ಷರ ***ret) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_strv_free_ ಅಕ್ಷರ **zones = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -errno;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ, *type = ಖಾಲಿ, *f1 = ಖಾಲಿ, *f2 = ಖಾಲಿ;

                r = read_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಸಾಲು;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = ಹೊರತೆಗೆ_many_ಪದಗಳು(&p, ಖಾಲಿ, 0, &type, &f1, &f2);
                ಆದರೆ (r < 0)
                        ಮುಂದುವರಿ;

                ಅಕ್ಷರ *tz;
                ಆದರೆ (ಒಳ_ಗಣ(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                ಇಲ್ಲದಿದ್ದರೆ
                        /* Not a line we care about. */
                        ಮುಂದುವರಿ;

                ಆದರೆ (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ಮುಂದುವರಿ;

                r = strv_extend(&zones, tz);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret = TAKE_PTR(zones);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_timezones(ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **zones = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        ಆದರೆ (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                ಆದರೆ (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        ಆದರೆ (r < 0 && r != -ENOENT)
                ಹಿಂತಿರುಗಿ r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        strv_ವಿಂಗಡಿಸು_uniq(zones);

        *ret = TAKE_PTR(zones);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ verify_timezone(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ log_level) {
        bool slash = false;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *t;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಅಕ್ಷರ buf[4];
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (isempty(name))
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        ಆದರೆ (streq(name, "UTC"))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (name[0] == '/')
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಗಾಗಿ (p = name; *p; p++) {
                ಆದರೆ (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !ಒಳ_ಗಣ(*p, '-', '_', '+', '/'))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (*p == '/') {

                        ಆದರೆ (slash)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        slash = true;
                } ಇಲ್ಲದಿದ್ದರೆ
                        slash = false;
        }

        ಆದರೆ (slash)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (p - name >= PATH_MAX)
                ಹಿಂತಿರುಗಿ -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        ಆದರೆ (memcmp(buf, "TZif", 4) != 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ reset_timezonep(ಅಕ್ಷರ **p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        (ಶೂನ್ಯ) ಗಣ_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

ಅಕ್ಷರ* save_timezone(ಶೂನ್ಯ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = getenv("TZ");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಅಕ್ಷರ *s = strdup(e);
        ಆದರೆ (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        ಹಿಂತಿರುಗಿ s;
}

bool clock_supported(clockid_t clock) {
        ರಚನೆ timespec ts;

        ಆಯ್ಕೆ (clock) {

        ಸ್ಥಿತಿ CLOCK_MONOTONIC:
        ಸ್ಥಿತಿ CLOCK_REALTIME:
        ಸ್ಥಿತಿ CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                ಹಿಂತಿರುಗಿ true;

        ಪೂರ್ವನಿಯೋಜಿತ:
                /* For everything else, check properly */
                ಹಿಂತಿರುಗಿ clock_gettime(clock, &ts) >= 0;
        }
}

ಪೂರ್ಣಾಂಕ get_timezone(ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = readlink_malloc(etc_localtime(), &t);
        ಆದರೆ (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, "UTC");
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r; /* Return EINVAL if not a symlink */

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = ಮಾರ್ಗ_STARTSWITH_ಗಣ(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (!timezone_is_valid(e, LOG_DEBUG))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, e);
}

ಪೂರ್ಣಾಂಕ get_timezone_prefer_env(ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = getenv("TZ");
        ಆದರೆ (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, e + 1);

        ಹಿಂತಿರುಗಿ get_timezone(ret);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* etc_localtime(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cached = ಖಾಲಿ;

        ಆದರೆ (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        ಹಿಂತಿರುಗಿ cached;
}

ಪೂರ್ಣಾಂಕ mktime_or_timegm_usec(
                ರಚನೆ tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        ಪ್ರತಿಜ್ಞಾ(tm);

        ಆದರೆ (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                ಹಿಂತಿರುಗಿ -ERANGE;
        ಆದರೆ ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_ಪ್ರತಿ_YEAR, (usec_t) ಸಮಯ_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                ಹಿಂತಿರುಗಿ -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        ಆದರೆ (t < 0) /* Refuse negative times and errors */
                ಹಿಂತಿರುಗಿ -ERANGE;
        ಆದರೆ ((usec_t) t >= USEC_INFINITY / USEC_ಪ್ರತಿ_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (ret)
                *ret = (usec_t) t * USEC_ಪ್ರತಿ_SEC;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                ರಚನೆ tm *ret) {

        t /= USEC_ಪ್ರತಿ_SEC; /* Round down */
        ಆದರೆ (t > (usec_t) ಸಮಯ_T_MAX)
                ಹಿಂತಿರುಗಿ -ERANGE;
        time_t sec = (time_t) t;

        ರಚನೆ tm buf = {};
        ಆದರೆ (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret)
                *ret = buf;

        ಹಿಂತಿರುಗಿ 0;
}

uint64_t sysconf_clock_ticks_cached(ಶೂನ್ಯ) {
        ಸ್ಥಿರ thread_local uint64_t hz = 0;

        ಆದರೆ (hz != 0)
                ಹಿಂತಿರುಗಿ hz;

        ದೀರ್ಘ t = sysconf(_SC_CLK_TCK);
        ಪ್ರತಿಜ್ಞಾ(t > 0);
        hz = (uint64_t) t;

        ಹಿಂತಿರುಗಿ hz;
}

uint32_t usec_ಗಮ್ಯ_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ಹಿಂತಿರುಗಿ DIV_ಸುತ್ತು_ಮೇಲೆ(u, USEC_ಪ್ರತಿ_SEC / hz);
}

usec_t jiffies_ಗಮ್ಯ_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        ಹಿಂತಿರುಗಿ DIV_ಸುತ್ತು_ಮೇಲೆ(j * USEC_ಪ್ರತಿ_SEC, hz);
}

usec_t usec_ಸರಿಸು_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        ಆದರೆ (x == USEC_INFINITY)
                ಹಿಂತಿರುಗಿ USEC_INFINITY;
        ಆದರೆ (map_clock_id(from) == map_clock_id(to))
                ಹಿಂತಿರುಗಿ x;

        a = now(from);
        b = now(to);

        ಆದರೆ (x > a)
                /* x lies in the future */
                ಹಿಂತಿರುಗಿ usec_ಕೂಡಿಸು(b, usec_sub_unsigned(x, a));
        ಇಲ್ಲದಿದ್ದರೆ
                /* x lies in the past */
                ಹಿಂತಿರುಗಿ usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool ಒಳ_utc_timezone(ಶೂನ್ಯ) {
        tzset();

        ಹಿಂತಿರುಗಿ timezone == 0 && daylight == 0;
}

ಪೂರ್ಣಾಂಕ usleep_ಸುರಕ್ಷಿತ(usec_t usec) {
        ಪೂರ್ಣಾಂಕ r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        ಆದರೆ (usec == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (usec == USEC_INFINITY)
                ಹಿಂತಿರುಗಿ RET_NERRNO(pause());

        ರಚನೆ timespec t;
        timespec_store(&t, usec);

        ಗಾಗಿ (;;) {
                ರಚನೆ timespec ಉಳಿದ;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &ಉಳಿದ);
                ಆದರೆ (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = ಉಳಿದ;
                        ಮುಂದುವರಿ;
                }

                ಹಿಂತಿರುಗಿ r;
        }
}

ಪೂರ್ಣಾಂಕ ಸಮಯ_change_fd(ಶೂನ್ಯ) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ itimerspec its = {
                .it_value.tv_sec = ಸಮಯ_T_MAX,
        };

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, ಖಾಲಿ) >= 0)
                ಹಿಂತಿರುಗಿ TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#ಆದರೆ SIZEOF_ಸಮಯ_T == 8 && ULONG_MAX < UINT64_MAX
        ಆದರೆ (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                ಆದರೆ (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, ಖಾಲಿ) >= 0)
                        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಹಿಂತಿರುಗಿ -errno;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ timestamp_style_ಕೋಷ್ಟಕ[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_ಕೋಷ್ಟಕ_lookup_from_string(timestamp_style_ಕೋಷ್ಟಕ, ELEMENTSOF(timestamp_style_ಕೋಷ್ಟಕ), s);
        ಆದರೆ (t >= 0)
                ಹಿಂತಿರುಗಿ t;
        ಆದರೆ (STRPTR_ಒಳ_ಗಣ(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                ಹಿಂತಿರುಗಿ TIMESTAMP_US;
        ಆದರೆ (STRPTR_ಒಳ_ಗಣ(s, "µs+utc", "μs+utc"))
                ಹಿಂತಿರುಗಿ TIMESTAMP_US_UTC;
        ಹಿಂತಿರುಗಿ t;
}

ಪೂರ್ಣಾಂಕ parse_calendar_date_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, bool allow_pre_epoch, usec_t *ret_usec, ರಚನೆ tm *ret_tm) {
        ರಚನೆ tm parsed_tm = {}, copy_tm;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        ಆದರೆ (!k || *k)
                ಹಿಂತಿರುಗಿ -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        ಆದರೆ (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                ಆದರೆ (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        ಹಿಂತಿರುಗಿ -EINVAL;
        } ಇಲ್ಲದಿದ್ದರೆ {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        ಆದರೆ (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret_usec)
                *ret_usec = usec;
        ಆದರೆ (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        ಹಿಂತಿರುಗಿ 0;
}
