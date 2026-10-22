/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <linux/time_types.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/timerfd.h>
#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "hexdecoct.h"          /* IWYU pragma: keep */
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "time-util.h"

સ્થિર clockid_t map_clock_id(clockid_t c) {

        /* Some more exotic archs (s390, ppc, …) lack the "ALARM" flavour of the clocks. Thus,
         * clock_gettime() will fail for them. Since they are essentially the same as their non-ALARM
         * pendants (their only difference is when timers are set on them), let's just map them
         * accordingly. This way, we can get the correct time even on those archs. */

        પસંદગી (c) {

        સ્થિતિ CLOCK_BOOTTIME_ALARM:
                પાછા CLOCK_BOOTTIME;

        સ્થિતિ CLOCK_REALTIME_ALARM:
                પાછા CLOCK_REALTIME;

        મૂળભૂત:
                પાછા c;
        }
}

usec_t now(clockid_t clock_id) {
        સંરચના timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        usec_t n = timespec_load(&ts);

        /* We use both 0 and USEC_INFINITY as niche values. If the current time collides with either, things are
         * really weird and really broken. Let's not allow this to go through, it would break too many of our
         * assumptions in code. */
        પ્રતિજ્ઞા(n > 0);
        પ્રતિજ્ઞા(n < USEC_INFINITY);

        પાછા n;
}

nsec_t now_nsec(clockid_t clock_id) {
        સંરચના timespec ts;

        assert_se(clock_gettime(map_clock_id(clock_id), &ts) == 0);

        nsec_t n = timespec_load_nsec(&ts);

        પ્રતિજ્ઞા(n > 0);
        પ્રતિજ્ઞા(n < NSEC_INFINITY);

        પાછા n;
}

dual_timestamp* dual_timestamp_now(dual_timestamp *ts) {
        પ્રતિજ્ઞા(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);

        પાછા ts;
}

triple_timestamp* triple_timestamp_now(triple_timestamp *ts) {
        પ્રતિજ્ઞા(ts);

        ts->realtime = now(CLOCK_REALTIME);
        ts->monotonic = now(CLOCK_MONOTONIC);
        ts->boottime = now(CLOCK_BOOTTIME);

        પાછા ts;
}

usec_t map_clock_usec_raw(usec_t from, usec_t from_base, usec_t to_base) {

        /* Maps the time 'from' between two clocks, based on a common reference point where the first clock
         * is at 'from_base' and the second clock at 'to_base'. Basically calculates:
         *
         *         from - from_base + to_base
         *
         * But takes care of overflows/underflows and avoids signed operations. */

        જો (from >= from_base) { /* In the future */
                usec_t delta = from - from_base;

                જો (to_base >= USEC_INFINITY - delta) /* overflow? */
                        પાછા USEC_INFINITY;

                પાછા to_base + delta;

        } નહીંતર { /* In the past */
                usec_t delta = from_base - from;

                જો (to_base <= delta) /* underflow? */
                        પાછા 0;

                પાછા to_base - delta;
        }
}

usec_t map_clock_usec(usec_t from, clockid_t from_clock, clockid_t to_clock) {

        /* Try to avoid any inaccuracy needlessly added in case we convert from effectively the same clock
         * onto itself */
        જો (map_clock_id(from_clock) == map_clock_id(to_clock))
                પાછા from;

        /* Keep infinity as is */
        જો (from == USEC_INFINITY)
                પાછા from;

        પાછા map_clock_usec_raw(from, now(from_clock), now(to_clock));
}

dual_timestamp* dual_timestamp_from_realtime(dual_timestamp *ts, usec_t u) {
        પ્રતિજ્ઞા(ts);

        જો (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = u;
                પાછા ts;
        }

        ts->realtime = u;
        ts->monotonic = map_clock_usec(u, CLOCK_REALTIME, CLOCK_MONOTONIC);
        પાછા ts;
}

triple_timestamp* triple_timestamp_from_realtime(triple_timestamp *ts, usec_t u) {
        usec_t nowr;

        પ્રતિજ્ઞા(ts);

        જો (!timestamp_is_set(u)) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                પાછા ts;
        }

        nowr = now(CLOCK_REALTIME);

        ts->realtime = u;
        ts->monotonic = map_clock_usec_raw(u, nowr, now(CLOCK_MONOTONIC));
        ts->boottime = map_clock_usec_raw(u, nowr, now(CLOCK_BOOTTIME));

        પાછા ts;
}

triple_timestamp* triple_timestamp_from_boottime(triple_timestamp *ts, usec_t u) {
        usec_t nowb;

        પ્રતિજ્ઞા(ts);

        જો (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = ts->boottime = u;
                પાછા ts;
        }

        nowb = now(CLOCK_BOOTTIME);

        ts->boottime = u;
        ts->monotonic = map_clock_usec_raw(u, nowb, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowb, now(CLOCK_REALTIME));

        પાછા ts;
}

dual_timestamp* dual_timestamp_from_monotonic(dual_timestamp *ts, usec_t u) {
        પ્રતિજ્ઞા(ts);

        જો (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                પાછા ts;
        }

        ts->monotonic = u;
        ts->realtime = map_clock_usec(u, CLOCK_MONOTONIC, CLOCK_REALTIME);
        પાછા ts;
}

dual_timestamp* dual_timestamp_from_boottime(dual_timestamp *ts, usec_t u) {
        usec_t nowm;

        પ્રતિજ્ઞા(ts);

        જો (u == USEC_INFINITY) {
                ts->realtime = ts->monotonic = USEC_INFINITY;
                પાછા ts;
        }

        nowm = now(CLOCK_BOOTTIME);
        ts->monotonic = map_clock_usec_raw(u, nowm, now(CLOCK_MONOTONIC));
        ts->realtime = map_clock_usec_raw(u, nowm, now(CLOCK_REALTIME));
        પાછા ts;
}

usec_t triple_timestamp_by_clock(triple_timestamp *ts, clockid_t clock) {
        પ્રતિજ્ઞા(ts);

        પસંદગી (clock) {

        સ્થિતિ CLOCK_REALTIME:
        સ્થિતિ CLOCK_REALTIME_ALARM:
                પાછા ts->realtime;

        સ્થિતિ CLOCK_MONOTONIC:
                પાછા ts->monotonic;

        સ્થિતિ CLOCK_BOOTTIME:
        સ્થિતિ CLOCK_BOOTTIME_ALARM:
                પાછા ts->boottime;

        મૂળભૂત:
                પાછા USEC_INFINITY;
        }
}

usec_t timespec_load(અચલ સંરચના timespec *ts) {
        પ્રતિજ્ઞા(ts);

        જો (ts->tv_sec < 0 || ts->tv_nsec < 0)
                પાછા USEC_INFINITY;

        જો ((usec_t) ts->tv_sec > (UINT64_MAX - (ts->tv_nsec / NSEC_PER_USEC)) / USEC_PER_SEC)
                પાછા USEC_INFINITY;

        પાછા
                (usec_t) ts->tv_sec * USEC_PER_SEC +
                (usec_t) ts->tv_nsec / NSEC_PER_USEC;
}

nsec_t timespec_load_nsec(અચલ સંરચના timespec *ts) {
        પ્રતિજ્ઞા(ts);

        જો (ts->tv_sec < 0 || ts->tv_nsec < 0)
                પાછા NSEC_INFINITY;

        જો ((nsec_t) ts->tv_sec >= (UINT64_MAX - ts->tv_nsec) / NSEC_PER_SEC)
                પાછા NSEC_INFINITY;

        પાછા (nsec_t) ts->tv_sec * NSEC_PER_SEC + (nsec_t) ts->tv_nsec;
}

સંરચના timespec *timespec_store(સંરચના timespec *ts, usec_t u) {
        પ્રતિજ્ઞા(ts);

        જો (u == USEC_INFINITY ||
            u / USEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                પાછા ts;
        }

        ts->tv_sec = (time_t) (u / USEC_PER_SEC);
        ts->tv_nsec = (long) ((u % USEC_PER_SEC) * NSEC_PER_USEC);

        પાછા ts;
}

સંરચના timespec *timespec_store_nsec(સંરચના timespec *ts, nsec_t n) {
        પ્રતિજ્ઞા(ts);

        જો (n == NSEC_INFINITY ||
            n / NSEC_PER_SEC >= TIME_T_MAX) {
                ts->tv_sec = (time_t) -1;
                ts->tv_nsec = -1L;
                પાછા ts;
        }

        ts->tv_sec = (time_t) (n / NSEC_PER_SEC);
        ts->tv_nsec = (long) (n % NSEC_PER_SEC);

        પાછા ts;
}

સંરચના __kernel_timespec *kernel_timespec_store(સંરચના __kernel_timespec *ts, usec_t u) {
        પ્રતિજ્ઞા(ts);
        /* struct __kernel_timespec has no encoding for "no timeout": io_uring expects a NULL pointer for
         * that, so callers have to resolve USEC_INFINITY themselves. */
        પ્રતિજ્ઞા(u != USEC_INFINITY);

        ts->tv_sec  = u / USEC_PER_SEC;
        ts->tv_nsec = (u % USEC_PER_SEC) * NSEC_PER_USEC;

        પાછા ts;
}

usec_t timeval_load(અચલ સંરચના timeval *tv) {
        પ્રતિજ્ઞા(tv);

        જો (tv->tv_sec < 0 || tv->tv_usec < 0)
                પાછા USEC_INFINITY;

        જો ((usec_t) tv->tv_sec > (UINT64_MAX - tv->tv_usec) / USEC_PER_SEC)
                પાછા USEC_INFINITY;

        પાછા
                (usec_t) tv->tv_sec * USEC_PER_SEC +
                (usec_t) tv->tv_usec;
}

સંરચના timeval *timeval_store(સંરચના timeval *tv, usec_t u) {
        પ્રતિજ્ઞા(tv);

        જો (u == USEC_INFINITY ||
            u / USEC_PER_SEC > TIME_T_MAX) {
                tv->tv_sec = (time_t) -1;
                tv->tv_usec = (suseconds_t) -1;
        } નહીંતર {
                tv->tv_sec = (time_t) (u / USEC_PER_SEC);
                tv->tv_usec = (suseconds_t) (u % USEC_PER_SEC);
        }

        પાછા tv;
}

/* Returns the abbreviated English weekday name for wd in Mon=0 … Sun=6 order.
 * We use non-localized (English) form so that timestamps can be parsed with
 * parse_timestamp() and always read the same regardless of locale. */
સ્થિર અચલ અક્ષર *અચલ weekday_table[_WEEKDAY_MAX] = {
        [WEEKDAY_MON] = "Mon",
        [WEEKDAY_TUE] = "Tue",
        [WEEKDAY_WED] = "Wed",
        [WEEKDAY_THU] = "Thu",
        [WEEKDAY_FRI] = "Fri",
        [WEEKDAY_SAT] = "Sat",
        [WEEKDAY_SUN] = "Sun",
};

DEFINE_STRING_TABLE_LOOKUP_TO_STRING(weekday, પૂર્ણાંક);

અક્ષર* format_timestamp_style(
                અક્ષર *buf,
                size_t l,
                usec_t t,
                TimestampStyle style) {
        સંરચના tm tm;
        bool utc, us;
        size_t n;

        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(style >= 0);
        પ્રતિજ્ઞા(style < _TIMESTAMP_STYLE_MAX);

        જો (!timestamp_is_set(t))
                પાછા ખાલી; /* Timestamp is unset */

        જો (style == TIMESTAMP_UNIX) {
                જો (l < (size_t) (1 + 1 + 1))
                        પાછા ખાલી; /* not enough space for even the shortest of forms */

                પાછા snprintf_ok(buf, l, "@" USEC_FMT, t / USEC_PER_SEC);  /* round down μs → s */
        }

        utc = IN_SET(style, TIMESTAMP_UTC, TIMESTAMP_US_UTC, TIMESTAMP_DATE);
        us = IN_SET(style, TIMESTAMP_US, TIMESTAMP_US_UTC);

        જો (l < (size_t) (3 +                   /* week day */
                          1 + 10 +              /* space and date */
                          style == TIMESTAMP_DATE ? 0 :
                          (1 + 8 +              /* space and time */
                           (us ? 1 + 6 : 0) +   /* "." and microsecond part */
                           1 + (utc ? 3 : 1)) + /* space and shortest possible zone */
                          1))
                પાછા ખાલી; /* Not enough space even for the shortest form. */

        /* Let's not format times with years > 9999 */
        જો (t > USEC_TIMESTAMP_FORMATTABLE_MAX) {
                સ્થિર અચલ અક્ષર* અચલ xxx[_TIMESTAMP_STYLE_MAX] = {
                        [TIMESTAMP_PRETTY] = "--- XXXX-XX-XX XX:XX:XX",
                        [TIMESTAMP_US]     = "--- XXXX-XX-XX XX:XX:XX.XXXXXX",
                        [TIMESTAMP_UTC]    = "--- XXXX-XX-XX XX:XX:XX UTC",
                        [TIMESTAMP_US_UTC] = "--- XXXX-XX-XX XX:XX:XX.XXXXXX UTC",
                        [TIMESTAMP_DATE]   = "--- XXXX-XX-XX",
                };

                પ્રતિજ્ઞા(l >= strlen(xxx[style]) + 1);
                પાછા strcpy(buf, xxx[style]);
        }

        જો (localtime_or_gmtime_usec(t, utc, &tm) < 0)
                પાછા ખાલી;

        /* Start with the week day */
        અચલ અક્ષર *weekday = weekday_to_string(tm.tm_wday == 0 ? 6 : tm.tm_wday - 1);
        પ્રતિજ્ઞા(weekday);
        memcpy(buf, weekday, 4);

        જો (style == TIMESTAMP_DATE) {
                /* Special format string if only date should be shown. */
                જો (strftime(buf + 3, l - 3, " %Y-%m-%d", &tm) <= 0)
                        પાછા ખાલી; /* Doesn't fit */

                પાછા buf;
        }

        /* Add the main components */
        જો (strftime(buf + 3, l - 3, " %Y-%m-%d %H:%M:%S", &tm) <= 0)
                પાછા ખાલી; /* Doesn't fit */

        /* Append the microseconds part, if that's requested */
        જો (us) {
                n = strlen(buf);
                જો (n + 8 > l)
                        પાછા ખાલી; /* Microseconds part doesn't fit. */

                sprintf(buf + n, ".%06"PRI_USEC, t % USEC_PER_SEC);
        }

        /* Append the timezone */
        n = strlen(buf);
        જો (utc) {
                /* If this is UTC then let's explicitly use the "UTC" string here, because gmtime_r()
                 * normally uses the obsolete "GMT" instead. */
                જો (n + 5 > l)
                        પાછા ખાલી; /* "UTC" doesn't fit. */

                strcpy(buf + n, " UTC");

        } નહીંતર જો (!isempty(tm.tm_zone)) {
                size_t tn;

                /* An explicit timezone is specified, let's use it, if it fits */
                tn = strlen(tm.tm_zone);
                જો (n + 1 + tn + 1 > l) {
                        /* The full time zone does not fit in. Yuck. */

                        જો (n + 1 + _POSIX_TZNAME_MAX + 1 > l)
                                પાછા ખાલી; /* Not even enough space for the POSIX minimum (of 6)? In that
                                              * case, complain that it doesn't fit. */

                        /* So the time zone doesn't fit in fully, but the caller passed enough space for the
                         * POSIX minimum time zone length. In this case suppress the timezone entirely, in
                         * order not to dump an overly long, hard to read string on the user. This should be
                         * safe, because the user will assume the local timezone anyway if none is shown. And
                         * so does parse_timestamp(). */
                } નહીંતર {
                        buf[n++] = ' ';
                        strcpy(buf + n, tm.tm_zone);
                }
        }

        પાછા buf;
}

અક્ષર* format_timestamp_relative_full(અક્ષર *buf, size_t l, usec_t t, clockid_t clock, bool implicit_ડાબું) {
        અચલ અક્ષર *s;
        usec_t n, d;

        પ્રતિજ્ઞા(buf);

        જો (!timestamp_is_set(t))
                પાછા ખાલી;

        n = now(clock);
        જો (n > t) {
                d = n - t;
                s = " ago";
        } નહીંતર {
                d = t - n;
                s = implicit_ડાબું ? "" : " left";
        }

        જો (d >= USEC_PER_YEAR) {
                usec_t years = d / USEC_PER_YEAR;
                usec_t months = (d % USEC_PER_YEAR) / USEC_PER_MONTH;

                (શૂન્ય) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                years,
                                years == 1 ? "year" : "years",
                                months,
                                months == 1 ? "month" : "months",
                                s);
        } નહીંતર જો (d >= USEC_PER_MONTH) {
                usec_t months = d / USEC_PER_MONTH;
                usec_t days = (d % USEC_PER_MONTH) / USEC_PER_DAY;

                (શૂન્ય) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                months,
                                months == 1 ? "month" : "months",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } નહીંતર જો (d >= USEC_PER_WEEK) {
                usec_t weeks = d / USEC_PER_WEEK;
                usec_t days = (d % USEC_PER_WEEK) / USEC_PER_DAY;

                (શૂન્ય) snprintf(buf, l, USEC_FMT " %s " USEC_FMT " %s%s",
                                weeks,
                                weeks == 1 ? "week" : "weeks",
                                days,
                                days == 1 ? "day" : "days",
                                s);
        } નહીંતર જો (d >= 2*USEC_PER_DAY)
                (શૂન્ય) snprintf(buf, l, USEC_FMT " days%s", d / USEC_PER_DAY,s);
        નહીંતર જો (d >= 25*USEC_PER_HOUR)
                (શૂન્ય) snprintf(buf, l, "1 day " USEC_FMT "h%s",
                                (d - USEC_PER_DAY) / USEC_PER_HOUR, s);
        નહીંતર જો (d >= 6*USEC_PER_HOUR)
                (શૂન્ય) snprintf(buf, l, USEC_FMT "h%s",
                                d / USEC_PER_HOUR, s);
        નહીંતર જો (d >= USEC_PER_HOUR)
                (શૂન્ય) snprintf(buf, l, USEC_FMT "h " USEC_FMT "min%s",
                                d / USEC_PER_HOUR,
                                (d % USEC_PER_HOUR) / USEC_PER_MINUTE, s);
        નહીંતર જો (d >= 5*USEC_PER_MINUTE)
                (શૂન્ય) snprintf(buf, l, USEC_FMT "min%s",
                                d / USEC_PER_MINUTE, s);
        નહીંતર જો (d >= USEC_PER_MINUTE)
                (શૂન્ય) snprintf(buf, l, USEC_FMT "min " USEC_FMT "s%s",
                                d / USEC_PER_MINUTE,
                                (d % USEC_PER_MINUTE) / USEC_PER_SEC, s);
        નહીંતર જો (d >= USEC_PER_SEC)
                (શૂન્ય) snprintf(buf, l, USEC_FMT "s%s",
                                d / USEC_PER_SEC, s);
        નહીંતર જો (d >= USEC_PER_MSEC)
                (શૂન્ય) snprintf(buf, l, USEC_FMT "ms%s",
                                d / USEC_PER_MSEC, s);
        નહીંતર જો (d > 0)
                (શૂન્ય) snprintf(buf, l, USEC_FMT"us%s",
                                d, s);
        નહીંતર
                (શૂન્ય) snprintf(buf, l, "now");

        buf[l-1] = 0;
        પાછા buf;
}

અક્ષર* format_timespan(અક્ષર *buf, size_t l, usec_t t, usec_t accuracy) {
        સ્થિર અચલ સંરચના {
                અચલ અક્ષર *suffix;
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

        અક્ષર *p = ASSERT_PTR(buf);
        bool something = false;

        પ્રતિજ્ઞા(l > 0);

        જો (t == USEC_INFINITY) {
                strncpy(p, "infinity", l-1);
                p[l-1] = 0;
                પાછા p;
        }

        જો (t <= 0) {
                strncpy(p, "0", l-1);
                p[l-1] = 0;
                પાછા p;
        }

        /* The result of this function can be parsed with parse_sec */

        FOREACH_ELEMENT(i, table) {
                પૂર્ણાંક k = 0;
                size_t n;
                bool done = false;
                usec_t a, b;

                જો (t <= 0)
                        તોડો;

                જો (t < accuracy && something)
                        તોડો;

                જો (t < i->usec)
                        ચાલુ;

                જો (l <= 1)
                        તોડો;

                a = t / i->usec;
                b = t % i->usec;

                /* Let's see if we should shows this in dot notation */
                જો (t < USEC_PER_MINUTE && b > 0) {
                        signed અક્ષર j = 0;

                        માટે (usec_t cc = i->usec; cc > 1; cc /= 10)
                                j++;

                        માટે (usec_t cc = accuracy; cc > 1; cc /= 10) {
                                b /= 10;
                                j--;
                        }

                        જો (j > 0) {
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
                જો (!done) {
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

        પાછા buf;
}

અચલ અક્ષર* get_tzname(bool dst) {
        /* musl leaves the DST timezone name unset if there is no DST, map this back to no DST */
        જો (dst && isempty(tzname[1]))
                dst = false;

        પાછા empty_to_null(tzname[dst]);
}

પૂર્ણાંક parse_gmtoff(અચલ અક્ષર *t, long *ret) {
        પ્રતિજ્ઞા(t);

        સંરચના tm tm;
        અચલ અક્ષર *k = strptime(t, "%z", &tm);
        જો (k && *k == '\0') {
                /* Success! */
                જો (ret)
                        *ret = tm.tm_gmtoff;
                પાછા 0;
        }

#ifdef __GLIBC__
        પાછા -EINVAL;
#નહીંતર
        પૂર્ણાંક r;

        /* musl v1.2.5 does not support %z specifier in strptime(). Since
         * https://github.com/kraj/musl/commit/fced99e93daeefb0192fd16304f978d4401d1d77
         * %z is supported, but it only supports strict RFC-822/ISO 8601 format, that is, 4 digits with sign
         * (e.g. +0900 or -1400), but does not support extended format: 2 digits or colon separated 4 digits
         * (e.g. +09 or -14:00). Let's add fallback logic to make it support the extended timezone spec. */

        bool positive;
        પસંદગી (*t) {
        સ્થિતિ '+':
                positive = true;
                તોડો;
        સ્થિતિ '-':
                positive = false;
                તોડો;
        મૂળભૂત:
                પાછા -EINVAL;
        }

        t++;
        r = undecchar(*t);
        જો (r < 0)
                પાછા r;

        usec_t u = r * 10 * USEC_PER_HOUR;

        t++;
        r = undecchar(*t);
        જો (r < 0)
                પાછા r;
        u += r * USEC_PER_HOUR;

        t++;
        જો (*t == '\0') /* 2 digits case */
                જાઓ finalize;

        જો (*t == ':') /* skip colon */
                t++;

        r = undecchar(*t);
        જો (r < 0)
                પાછા r;
        જો (r >= 6) /* refuse minutes equal to or larger than 60 */
                પાછા -EINVAL;

        u += r * 10 * USEC_PER_MINUTE;

        t++;
        r = undecchar(*t);
        જો (r < 0)
                પાછા r;

        u += r * USEC_PER_MINUTE;

        t++;
        જો (*t != '\0')
                પાછા -EINVAL;

finalize:
        જો (u > USEC_PER_DAY) /* refuse larger than one day */
                પાછા -EINVAL;

        જો (ret) {
                long gmtoff = u / USEC_PER_SEC;
                *ret = positive ? gmtoff : -gmtoff;
        }

        પાછા 0;
#endif
}

સ્થિર પૂર્ણાંક parse_timestamp_impl(
                અચલ અક્ષર *t,
                size_t max_len,
                bool utc,
                પૂર્ણાંક isdst,
                long gmtoff,
                usec_t *ret) {

        સ્થિર અચલ સંરચના {
                અચલ અક્ષર *name;
                અચલ પૂર્ણાંક nr;
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

        _cleanup_free_ અક્ષર *t_alloc = ખાલી;
        usec_t usec, plus = 0, minus = 0;
        bool with_tz = false;
        પૂર્ણાંક r, weekday = -1;
        unsigned fractional = 0;
        અચલ અક્ષર *k;
        સંરચના tm tm, copy;

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

        પ્રતિજ્ઞા(t);

        જો (max_len != SIZE_MAX) {
                /* If the input string contains timezone, then cut it here. */

                જો (max_len == 0) /* Can't be the only field */
                        પાછા -EINVAL;

                t_alloc = strndup(t, max_len);
                જો (!t_alloc)
                        પાછા -ENOMEM;

                t = t_alloc;
                with_tz = true;
        }

        જો (utc) {
                /* glibc accepts gmtoff more than 24 hours, but we refuse it. */
                જો ((usec_t) labs(gmtoff) * USEC_PER_SEC > USEC_PER_DAY)
                        પાછા -EINVAL;
        } નહીંતર {
                જો (gmtoff != 0)
                        પાછા -EINVAL;
        }

        જો (t[0] == '@' && !with_tz)
                પાછા parse_sec(t + 1, ret);

        usec = now(CLOCK_REALTIME);

        જો (!with_tz) {
                જો (streq(t, "now"))
                        જાઓ finish;

                જો (t[0] == '+') {
                        r = parse_sec(t+1, &plus);
                        જો (r < 0)
                                પાછા r;

                        જાઓ finish;
                }

                જો (t[0] == '-') {
                        r = parse_sec(t+1, &minus);
                        જો (r < 0)
                                પાછા r;

                        જાઓ finish;
                }

                જો ((k = endswith(t, " ago"))) {
                        _cleanup_free_ અક્ષર *buf = ખાલી;

                        buf = strndup(t, k - t);
                        જો (!buf)
                                પાછા -ENOMEM;

                        r = parse_sec(buf, &minus);
                        જો (r < 0)
                                પાછા r;

                        જાઓ finish;
                }

                જો ((k = endswith(t, " left"))) {
                        _cleanup_free_ અક્ષર *buf = ખાલી;

                        buf = strndup(t, k - t);
                        જો (!buf)
                                પાછા -ENOMEM;

                        r = parse_sec(buf, &plus);
                        જો (r < 0)
                                પાછા r;

                        જાઓ finish;
                }
        }

        r = localtime_or_gmtime_usec(usec, utc, &tm);
        જો (r < 0)
                પાછા r;

        tm.tm_isdst = isdst;

        જો (streq(t, "today")) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                જાઓ from_tm;

        } નહીંતર જો (streq(t, "yesterday")) {
                tm.tm_mday--;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                જાઓ from_tm;

        } નહીંતર જો (streq(t, "tomorrow")) {
                tm.tm_mday++;
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                જાઓ from_tm;
        }

        FOREACH_ELEMENT(day, day_nr) {
                k = startswith_no_case(t, day->name);
                જો (!k || *k != ' ')
                        ચાલુ;

#ifdef __GLIBC__
                /* musl does not set tm_wday field and set 0 unless it is explicitly requested by %w or so.
                 * In the below, let's only check tm_wday field only when built with glibc. */
                weekday = day->nr;
#endif
                t = k + 1;
                તોડો;
        }

        copy = tm;
        k = strptime(t, "%y-%m-%d %H:%M:%S", &tm);
        જો (k) {
                જો (*k == '.')
                        જાઓ parse_usec;
                નહીંતર જો (*k == 0)
                        જાઓ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M:%S", &tm);
        જો (k) {
                જો (*k == '.')
                        જાઓ parse_usec;
                નહીંતર જો (*k == 0)
                        જાઓ from_tm;
        }

        /* RFC3339 syntax */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M:%S", &tm);
        જો (k) {
                જો (*k == '.')
                        જાઓ parse_usec;
                નહીંતર જો (*k == 0)
                        જાઓ from_tm;
        }

        /* Support OUTPUT_SHORT and OUTPUT_SHORT_PRECISE formats */
        tm = copy;
        k = strptime(t, "%b %d %H:%M:%S", &tm);
        જો (k) {
                જો (*k == '.')
                        જાઓ parse_usec;
                નહીંતર જો (*k == 0)
                        જાઓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d %H:%M", &tm);
        જો (k && *k == 0) {
                tm.tm_sec = 0;
                જાઓ from_tm;
        }

        /* Our "canonical" RFC3339 syntax variant without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%d %H:%M", &tm);
        જો (k && *k == 0) {
                tm.tm_sec = 0;
                જાઓ from_tm;
        }

        /* RFC3339 syntax without seconds */
        tm = copy;
        k = strptime(t, "%Y-%m-%dT%H:%M", &tm);
        જો (k && *k == 0) {
                tm.tm_sec = 0;
                જાઓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%y-%m-%d", &tm);
        જો (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                જાઓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%Y-%m-%d", &tm);
        જો (k && *k == 0) {
                tm.tm_sec = tm.tm_min = tm.tm_hour = 0;
                જાઓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M:%S", &tm);
        જો (k) {
                જો (*k == '.')
                        જાઓ parse_usec;
                નહીંતર જો (*k == 0)
                        જાઓ from_tm;
        }

        tm = copy;
        k = strptime(t, "%H:%M", &tm);
        જો (k && *k == 0) {
                tm.tm_sec = 0;
                જાઓ from_tm;
        }

        પાછા -EINVAL;

parse_usec:
        k++;
        r = parse_fractional_part_u(&k, 6, &fractional);
        જો (r < 0)
                પાછા -EINVAL;
        જો (*k != '\0')
                પાછા -EINVAL;

from_tm:
        પ્રતિજ્ઞા(plus == 0);
        પ્રતિજ્ઞા(minus == 0);

        જો (weekday >= 0 && tm.tm_wday != weekday)
                પાછા -EINVAL;

        જો (gmtoff < 0) {
                plus = -gmtoff * USEC_PER_SEC;

                /* If gmtoff is negative, the string may be too old to be parsed as UTC.
                 * E.g. 1969-12-31 23:00:00 -06 == 1970-01-01 05:00:00 UTC
                 * We assumed that gmtoff is in the range of -24:00…+24:00, hence the only date we need to
                 * handle here is 1969-12-31. So, let's shift the date with one day, then subtract the shift
                 * later. */
                જો (tm.tm_year == 69 && tm.tm_mon == 11 && tm.tm_mday == 31) {
                        /* Thu 1970-01-01-00:00:00 */
                        tm.tm_year = 70;
                        tm.tm_mon = 0;
                        tm.tm_mday = 1;
                        tm.tm_wday = 4;
                        tm.tm_yday = 0;
                        minus = USEC_PER_DAY;
                }
        } નહીંતર
                minus = gmtoff * USEC_PER_SEC;

        r = mktime_or_timegm_usec(&tm, utc, &usec);
        જો (r < 0)
                પાછા r;

        usec = usec_add(usec, fractional);

finish:
        usec = usec_add(usec, plus);

        જો (usec < minus)
                પાછા -EINVAL;

        usec = usec_sub_unsigned(usec, minus);

        જો (usec > USEC_TIMESTAMP_FORMATTABLE_MAX)
                પાછા -EINVAL;

        જો (ret)
                *ret = usec;
        પાછા 0;
}

પૂર્ણાંક parse_timestamp(અચલ અક્ષર *t, usec_t *ret) {
        long gmtoff;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(t);

        size_t t_len = strlen(t);
        જો (t_len > 2 && t[t_len - 1] == 'Z') {
                /* Try to parse as RFC3339-style welded UTC: "1985-04-12T23:20:50.52Z" */
                r = parse_timestamp_impl(t, t_len - 1, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);
                જો (r >= 0)
                        પાછા r;
        }

        /* RFC3339-style welded offset: "1990-12-31T15:59:60-08:00" */
        જો (t_len > 7 && IN_SET(t[t_len - 6], '+', '-') && t[t_len - 7] != ' ' && parse_gmtoff(&t[t_len - 6], &gmtoff) >= 0)
                પાછા parse_timestamp_impl(t, t_len - 6, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        અચલ અક્ષર *tz = strrchr(t, ' ');
        જો (!tz)
                પાછા parse_timestamp_impl(t, /* max_len= */ SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        size_t max_len = tz - t;
        tz++;

        /* Shortcut, parse the string as UTC. */
        જો (streq(tz, "UTC"))
                પાછા parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, /* gmtoff= */ 0, ret);

        /* If the timezone is compatible with RFC-822/ISO 8601 (e.g. +06, or -03:00) then parse the string as
         * UTC and shift the result. Note, this must be earlier than the timezone check with tzname[], as
         * tzname[] may be in the same format. */
        જો (parse_gmtoff(tz, &gmtoff) >= 0)
                પાછા parse_timestamp_impl(t, max_len, /* utc= */ true, /* isdst= */ -1, gmtoff, ret);

        /* Check if the last word matches tzname[] of the local timezone. Note, this must be done earlier
         * than the check by timezone_is_valid() below, as some short timezone specifications have their own
         * timezone files (e.g. WET has its own timezone file, but JST does not), but using such files does
         * not follow the timezone change in the current area. */
        tzset();
        માટે (પૂર્ણાંક j = 0; j <= 1; j++) {
                જો (!streq_ptr(tz, get_tzname(j)))
                        ચાલુ;

                /* The specified timezone matches tzname[] of the local timezone. */
                પાછા parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ j, /* gmtoff= */ 0, ret);
        }

        /* If the last word is a valid timezone file (e.g. Asia/Tokyo), then save the current timezone, apply
         * the specified timezone, and parse the remaining string as a local time. */
        જો (timezone_is_valid(tz, LOG_DEBUG)) {
                SAVE_TIMEZONE;

                જો (setenv("TZ", tz, /* overwrite= */ true) < 0)
                        પાછા negative_errno();

                પાછા parse_timestamp_impl(t, max_len, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
        }

        /* Otherwise, assume that the last word is a part of the time and try to parse the whole string as a
         * local time. */
        પાછા parse_timestamp_impl(t, SIZE_MAX, /* utc= */ false, /* isdst= */ -1, /* gmtoff= */ 0, ret);
}

સ્થિર અચલ અક્ષર* extract_multiplier(અચલ અક્ષર *p, usec_t *ret) {
        સ્થિર અચલ સંરચના {
                અચલ અક્ષર *suffix;
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

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);

        FOREACH_ELEMENT(i, table) {
                અચલ અક્ષર *e = startswith(p, i->suffix);
                જો (e) {
                        *ret = i->usec;
                        પાછા e;
                }
        }

        પાછા p;
}

પૂર્ણાંક parse_time(અચલ અક્ષર *t, usec_t *ret, usec_t default_unit) {
        અચલ અક્ષર *p, *s;

        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(default_unit > 0);

        p = skip_leading_chars(t, /* bad= */ ખાલી);
        s = startswith(p, "infinity");
        જો (s) {
                જો (!in_charset(s, WHITESPACE))
                        પાછા -EINVAL;

                જો (ret)
                        *ret = USEC_INFINITY;
                પાછા 0;
        }

        usec_t usec = 0;

        માટે (bool something = false;;) {
                usec_t multiplier = default_unit, k;
                long long l;
                અક્ષર *e;

                p = skip_leading_chars(p, /* bad= */ ખાલી);
                જો (*p == 0) {
                        જો (!something)
                                પાછા -EINVAL;

                        તોડો;
                }

                જો (*p == '-') /* Don't allow "-0" */
                        પાછા -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                જો (errno > 0)
                        પાછા -errno;
                જો (l < 0)
                        પાછા -ERANGE;

                જો (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } નહીંતર જો (e == p)
                        પાછા -EINVAL;
                નહીંતર
                        p = e;

                s = extract_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                જો (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        પાછા -EINVAL;

                p = s;

                જો ((usec_t) l >= USEC_INFINITY / multiplier)
                        પાછા -ERANGE;

                k = (usec_t) l * multiplier;
                જો (k >= USEC_INFINITY - usec)
                        પાછા -ERANGE;

                usec += k;

                something = true;

                જો (*e == '.') {
                        usec_t m = multiplier / 10;
                        અચલ અક્ષર *b;

                        માટે (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (usec_t) (*b - '0') * m;
                                જો (k >= USEC_INFINITY - usec)
                                        પાછા -ERANGE;

                                usec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        જો (b == e + 1)
                                પાછા -EINVAL;
                }
        }

        જો (ret)
                *ret = usec;
        પાછા 0;
}

પૂર્ણાંક parse_sec(અચલ અક્ષર *t, usec_t *ret) {
        પાછા parse_time(t, ret, USEC_PER_SEC);
}

પૂર્ણાંક parse_sec_fix_0(અચલ અક્ષર *t, usec_t *ret) {
        usec_t k;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(ret);

        r = parse_sec(t, &k);
        જો (r < 0)
                પાછા r;

        *ret = k == 0 ? USEC_INFINITY : k;
        પાછા r;
}

પૂર્ણાંક parse_sec_def_infinity(અચલ અક્ષર *t, usec_t *ret) {
        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(ret);

        t += strspn(t, WHITESPACE);
        જો (isempty(t)) {
                *ret = USEC_INFINITY;
                પાછા 0;
        }
        પાછા parse_sec(t, ret);
}

સ્થિર અચલ અક્ષર* extract_nsec_multiplier(અચલ અક્ષર *p, nsec_t *ret) {
        સ્થિર અચલ સંરચના {
                અચલ અક્ષર *suffix;
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

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);

        FOREACH_ELEMENT(i, table) {
                અચલ અક્ષર *e = startswith(p, i->suffix);
                જો (e) {
                        *ret = i->nsec;
                        પાછા e;
                }
        }

        પાછા p;
}

પૂર્ણાંક parse_nsec(અચલ અક્ષર *t, nsec_t *ret) {
        અચલ અક્ષર *p, *s;
        nsec_t nsec = 0;
        bool something = false;

        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(ret);

        p = t;

        p += strspn(p, WHITESPACE);
        s = startswith(p, "infinity");
        જો (s) {
                s += strspn(s, WHITESPACE);
                જો (*s != 0)
                        પાછા -EINVAL;

                *ret = NSEC_INFINITY;
                પાછા 0;
        }

        માટે (;;) {
                nsec_t multiplier = 1, k;
                long long l;
                અક્ષર *e;

                p += strspn(p, WHITESPACE);

                જો (*p == 0) {
                        જો (!something)
                                પાછા -EINVAL;

                        તોડો;
                }

                જો (*p == '-') /* Don't allow "-0" */
                        પાછા -ERANGE;

                errno = 0;
                l = strtoll(p, &e, 10);
                જો (errno > 0)
                        પાછા -errno;
                જો (l < 0)
                        પાછા -ERANGE;

                જો (*e == '.') {
                        p = e + 1;
                        p += strspn(p, DIGITS);
                } નહીંતર જો (e == p)
                        પાછા -EINVAL;
                નહીંતર
                        p = e;

                s = extract_nsec_multiplier(p + strspn(p, WHITESPACE), &multiplier);
                જો (s == p && *s != '\0')
                        /* Don't allow '12.34.56', but accept '12.34 .56' or '12.34s.56' */
                        પાછા -EINVAL;

                p = s;

                જો ((nsec_t) l >= NSEC_INFINITY / multiplier)
                        પાછા -ERANGE;

                k = (nsec_t) l * multiplier;
                જો (k >= NSEC_INFINITY - nsec)
                        પાછા -ERANGE;

                nsec += k;

                something = true;

                જો (*e == '.') {
                        nsec_t m = multiplier / 10;
                        અચલ અક્ષર *b;

                        માટે (b = e + 1; *b >= '0' && *b <= '9'; b++, m /= 10) {
                                k = (nsec_t) (*b - '0') * m;
                                જો (k >= NSEC_INFINITY - nsec)
                                        પાછા -ERANGE;

                                nsec += k;
                        }

                        /* Don't allow "0.-0", "3.+1", "3. 1", "3.sec" or "3.hoge" */
                        જો (b == e + 1)
                                પાછા -EINVAL;
                }
        }

        *ret = nsec;

        પાછા 0;
}

સ્થિર પૂર્ણાંક get_timezones_from_zone1970_tab(અક્ષર ***ret) {
        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_strv_free_ અક્ષર **zones = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        f = fopen("/usr/share/zoneinfo/zone1970.tab", "re");
        જો (!f)
                પાછા -errno;

        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી, *cc = ખાલી, *co = ખાલી, *tz = ખાલી;

                r = read_લીટી(f, LONG_લીટી_MAX, &લીટી);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                અચલ અક્ષર *p = લીટી;

                /* Line format is:
                 * 'country codes' 'coordinates' 'timezone' 'comments' */
                r = extract_many_શબ્દો(&p, ખાલી, 0, &cc, &co, &tz);
                જો (r < 0)
                        ચાલુ;

                /* Lines that start with # are comments. */
                જો (*cc == '#')
                        ચાલુ;

                જો (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ચાલુ;

                r = strv_extend(&zones, tz);
                જો (r < 0)
                        પાછા r;
        }

        *ret = TAKE_PTR(zones);
        પાછા 0;
}

સ્થિર પૂર્ણાંક get_timezones_from_tzdata_zi(અક્ષર ***ret) {
        _cleanup_fclose_ FILE *f = ખાલી;
        _cleanup_strv_free_ અક્ષર **zones = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        f = fopen("/usr/share/zoneinfo/tzdata.zi", "re");
        જો (!f)
                પાછા -errno;

        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી, *type = ખાલી, *f1 = ખાલી, *f2 = ખાલી;

                r = read_લીટી(f, LONG_લીટી_MAX, &લીટી);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                અચલ અક્ષર *p = લીટી;

                /* The only lines we care about are Zone and Link lines.
                 * Zone line format is:
                 * 'Zone' 'timezone' ...
                 * Link line format is:
                 * 'Link' 'target' 'alias'
                 * See 'man zic' for more detail. */
                r = extract_many_શબ્દો(&p, ખાલી, 0, &type, &f1, &f2);
                જો (r < 0)
                        ચાલુ;

                અક્ષર *tz;
                જો (IN_SET(*type, 'Z', 'z'))
                        /* Zone lines have timezone in field 1. */
                        tz = f1;
                નહીંતર જો (IN_SET(*type, 'L', 'l'))
                        /* Link lines have timezone in field 2. */
                        tz = f2;
                નહીંતર
                        /* Not a line we care about. */
                        ચાલુ;

                જો (!timezone_is_valid(tz, LOG_DEBUG))
                        /* Don't list unusable timezones. */
                        ચાલુ;

                r = strv_extend(&zones, tz);
                જો (r < 0)
                        પાછા r;
        }

        *ret = TAKE_PTR(zones);
        પાછા 0;
}

પૂર્ણાંક get_timezones(અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **zones = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = get_timezones_from_tzdata_zi(&zones);
        જો (r == -ENOENT) {
                log_debug_errno(r, "Could not get timezone data from tzdata.zi, using zone1970.tab: %m");
                r = get_timezones_from_zone1970_tab(&zones);
                જો (r == -ENOENT)
                        log_debug_errno(r, "Could not get timezone data from zone1970.tab, using UTC: %m");
        }
        જો (r < 0 && r != -ENOENT)
                પાછા r;

        /* Always include UTC */
        r = strv_extend(&zones, "UTC");
        જો (r < 0)
                પાછા r;

        strv_sort_uniq(zones);

        *ret = TAKE_PTR(zones);
        પાછા 0;
}

પૂર્ણાંક verify_timezone(અચલ અક્ષર *name, પૂર્ણાંક log_level) {
        bool slash = false;
        અચલ અક્ષર *p, *t;
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        અક્ષર buf[4];
        પૂર્ણાંક r;

        જો (isempty(name))
                પાછા -EINVAL;

        /* Always accept "UTC" as valid timezone, since it's the fallback, even if user has no timezones installed. */
        જો (streq(name, "UTC"))
                પાછા 0;

        જો (name[0] == '/')
                પાછા -EINVAL;

        માટે (p = name; *p; p++) {
                જો (!ascii_isdigit(*p) &&
                    !ascii_isalpha(*p) &&
                    !IN_SET(*p, '-', '_', '+', '/'))
                        પાછા -EINVAL;

                જો (*p == '/') {

                        જો (slash)
                                પાછા -EINVAL;

                        slash = true;
                } નહીંતર
                        slash = false;
        }

        જો (slash)
                પાછા -EINVAL;

        જો (p - name >= PATH_MAX)
                પાછા -ENAMETOOLONG;

        t = strjoina("/usr/share/zoneinfo/", name);

        fd = open(t, O_RDONLY|O_CLOEXEC);
        જો (fd < 0)
                પાછા log_full_errno(log_level, errno, "Failed to open timezone file '%s': %m", t);

        r = fd_verify_regular(fd);
        જો (r < 0)
                પાછા log_full_errno(log_level, r, "Timezone file '%s' is not a regular file: %m", t);

        r = loop_read_exact(fd, buf, 4, false);
        જો (r < 0)
                પાછા log_full_errno(log_level, r, "Failed to read from timezone file '%s': %m", t);

        /* Magic from tzfile(5) */
        જો (memcmp(buf, "TZif", 4) != 0)
                પાછા log_full_errno(log_level, SYNTHETIC_ERRNO(EBADMSG),
                                      "Timezone file '%s' has wrong magic bytes", t);

        પાછા 0;
}

શૂન્ય reset_timezonep(અક્ષર **p) {
        પ્રતિજ્ઞા(p);

        (શૂન્ય) set_unset_env("TZ", *p, /* overwrite= */ true);
        tzset();
        *p = mfree(*p);
}

અક્ષર* save_timezone(શૂન્ય) {
        અચલ અક્ષર *e = getenv("TZ");
        જો (!e)
                પાછા ખાલી;

        અક્ષર *s = strdup(e);
        જો (!s)
                log_debug("Failed to save $TZ=%s, unsetting the environment variable.", e);

        પાછા s;
}

bool clock_supported(clockid_t clock) {
        સંરચના timespec ts;

        પસંદગી (clock) {

        સ્થિતિ CLOCK_MONOTONIC:
        સ્થિતિ CLOCK_REALTIME:
        સ્થિતિ CLOCK_BOOTTIME:
                /* These three are always available in our baseline, and work in timerfd, as of kernel 3.15 */
                પાછા true;

        મૂળભૂત:
                /* For everything else, check properly */
                પાછા clock_gettime(clock, &ts) >= 0;
        }
}

પૂર્ણાંક get_timezone(અક્ષર **ret) {
        _cleanup_free_ અક્ષર *t = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = readlink_malloc(etc_localtime(), &t);
        જો (r == -ENOENT)
                /* If the symlink does not exist, assume "UTC", like glibc does */
                પાછા strdup_to(ret, "UTC");
        જો (r < 0)
                પાછા r; /* Return EINVAL if not a symlink */

        અચલ અક્ષર *e = PATH_STARTSWITH_SET(t, "/usr/share/zoneinfo/", "../usr/share/zoneinfo/");
        જો (!e)
                પાછા -EINVAL;
        જો (!timezone_is_valid(e, LOG_DEBUG))
                પાછા -EINVAL;

        પાછા strdup_to(ret, e);
}

પૂર્ણાંક get_timezone_prefer_env(અક્ષર **ret) {
        પ્રતિજ્ઞા(ret);

        અચલ અક્ષર *e = getenv("TZ");
        જો (e && e[0] == ':' && timezone_is_valid(e + 1, LOG_DEBUG))
                પાછા strdup_to(ret, e + 1);

        પાછા get_timezone(ret);
}

અચલ અક્ષર* etc_localtime(શૂન્ય) {
        સ્થિર અચલ અક્ષર *cached = ખાલી;

        જો (!cached)
                cached = secure_getenv("SYSTEMD_ETC_LOCALTIME") ?: "/etc/localtime";

        પાછા cached;
}

પૂર્ણાંક mktime_or_timegm_usec(
                સંરચના tm *tm, /* input + normalized output */
                bool utc,
                usec_t *ret) {

        time_t t;

        પ્રતિજ્ઞા(tm);

        જો (tm->tm_year < 69) /* early check for negative (i.e. before 1970) time_t (Note that in some timezones the epoch is in the year 1969!) */
                પાછા -ERANGE;
        જો ((usec_t) tm->tm_year > CONST_MIN(USEC_INFINITY / USEC_PER_YEAR, (usec_t) TIME_T_MAX / (365U * 24U * 60U * 60U)) - 1900) /* early check for possible overrun of usec_t or time_t */
                પાછા -ERANGE;

        /* timegm()/mktime() is a bit weird to use, since it returns -1 in two cases: on error as well as a
         * valid time indicating one second before the UNIX epoch. Let's treat both cases the same here, and
         * return -ERANGE for anything negative, since usec_t is unsigned, and we can thus not express
         * negative times anyway. */

        t = utc ? timegm(tm) : mktime(tm);
        જો (t < 0) /* Refuse negative times and errors */
                પાછા -ERANGE;
        જો ((usec_t) t >= USEC_INFINITY / USEC_PER_SEC) /* Never return USEC_INFINITY by accident (or overflow) */
                પાછા -ERANGE;

        જો (ret)
                *ret = (usec_t) t * USEC_PER_SEC;
        પાછા 0;
}

પૂર્ણાંક localtime_or_gmtime_usec(
                usec_t t,
                bool utc,
                સંરચના tm *ret) {

        t /= USEC_PER_SEC; /* Round down */
        જો (t > (usec_t) TIME_T_MAX)
                પાછા -ERANGE;
        time_t sec = (time_t) t;

        સંરચના tm buf = {};
        જો (!(utc ? gmtime_r(&sec, &buf) : localtime_r(&sec, &buf)))
                પાછા -EINVAL;

        જો (ret)
                *ret = buf;

        પાછા 0;
}

uint64_t sysconf_clock_ticks_cached(શૂન્ય) {
        સ્થિર thread_local uint64_t hz = 0;

        જો (hz != 0)
                પાછા hz;

        long t = sysconf(_SC_CLK_TCK);
        પ્રતિજ્ઞા(t > 0);
        hz = (uint64_t) t;

        પાછા hz;
}

uint32_t usec_to_jiffies(usec_t u) {
        uint64_t hz = sysconf_clock_ticks_cached();
        પાછા DIV_ROUND_UP(u, USEC_PER_SEC / hz);
}

usec_t jiffies_to_usec(uint32_t j) {
        uint64_t hz = sysconf_clock_ticks_cached();
        પાછા DIV_ROUND_UP(j * USEC_PER_SEC, hz);
}

usec_t usec_shift_clock(usec_t x, clockid_t from, clockid_t to) {
        usec_t a, b;

        જો (x == USEC_INFINITY)
                પાછા USEC_INFINITY;
        જો (map_clock_id(from) == map_clock_id(to))
                પાછા x;

        a = now(from);
        b = now(to);

        જો (x > a)
                /* x lies in the future */
                પાછા usec_add(b, usec_sub_unsigned(x, a));
        નહીંતર
                /* x lies in the past */
                પાછા usec_sub_unsigned(b, usec_sub_unsigned(a, x));
}

bool in_utc_timezone(શૂન્ય) {
        tzset();

        પાછા timezone == 0 && daylight == 0;
}

પૂર્ણાંક usleep_safe(usec_t usec) {
        પૂર્ણાંક r;

        /* usleep() takes useconds_t that is (typically?) uint32_t. Also, usleep() may only support the
         * range [0, 1000000]. See usleep(3). Let's override usleep() with clock_nanosleep().
         *
         * ⚠️ Note we are not using plain nanosleep() here, since that operates on CLOCK_REALTIME, not
         *    CLOCK_MONOTONIC! */

        જો (usec == 0)
                પાછા 0;

        જો (usec == USEC_INFINITY)
                પાછા RET_NERRNO(pause());

        સંરચના timespec t;
        timespec_store(&t, usec);

        માટે (;;) {
                સંરચના timespec remaining;

                /* `clock_nanosleep()` does not use `errno`, but returns positive error codes. */
                r = -clock_nanosleep(CLOCK_MONOTONIC, /* flags= */ 0, &t, &remaining);
                જો (r == -EINTR) {
                        /* Interrupted. Continue sleeping for the remaining time. */
                        t = remaining;
                        ચાલુ;
                }

                પાછા r;
        }
}

પૂર્ણાંક time_change_fd(શૂન્ય) {

        /* We only care for the cancellation event, hence we set the timeout to the latest possible value. */
        સ્થિર અચલ સંરચના itimerspec its = {
                .it_value.tv_sec = TIME_T_MAX,
        };

        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        /* Uses TFD_TIMER_CANCEL_ON_SET to get notifications whenever CLOCK_REALTIME makes a jump relative to
         * CLOCK_MONOTONIC. */

        fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
        જો (fd < 0)
                પાછા -errno;

        જો (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its, ખાલી) >= 0)
                પાછા TAKE_FD(fd);

        /* So apparently there are systems where time_t is 64-bit, but the kernel actually doesn't support
         * 64-bit time_t. In that case configuring a timer to TIME_T_MAX will fail with EOPNOTSUPP or a
         * similar error. If that's the case let's try with INT32_MAX instead, maybe that works. It's a bit
         * of a black magic thing though, but what can we do?
         *
         * We don't want this code on x86-64, hence let's conditionalize this for systems with 64-bit time_t
         * but where "long" is shorter than 64-bit, i.e. 32-bit archs.
         *
         * See: https://github.com/systemd/systemd/issues/14362 */

#જો SIZEOF_TIME_T == 8 && ULONG_MAX < UINT64_MAX
        જો (ERRNO_IS_NOT_SUPPORTED(errno) || errno == EOVERFLOW) {
                સ્થિર અચલ સંરચના itimerspec its32 = {
                        .it_value.tv_sec = INT32_MAX,
                };

                જો (timerfd_settime(fd, TFD_TIMER_ABSTIME|TFD_TIMER_CANCEL_ON_SET, &its32, ખાલી) >= 0)
                        પાછા TAKE_FD(fd);
        }
#endif

        પાછા -errno;
}

સ્થિર અચલ અક્ષર* અચલ timestamp_style_table[_TIMESTAMP_STYLE_MAX] = {
        [TIMESTAMP_PRETTY] = "pretty",
        [TIMESTAMP_US]     = "us",
        [TIMESTAMP_UTC]    = "utc",
        [TIMESTAMP_US_UTC] = "us+utc",
        [TIMESTAMP_UNIX]   = "unix",
};

/* Use the macro for enum → string to allow for aliases */
DEFINE_STRING_TABLE_LOOKUP_TO_STRING(timestamp_style, TimestampStyle);

/* For the string → enum mapping we use the generic implementation, but also support two aliases */
TimestampStyle timestamp_style_from_string(અચલ અક્ષર *s) {
        TimestampStyle t;

        t = (TimestampStyle) string_table_lookup_from_string(timestamp_style_table, ELEMENTSOF(timestamp_style_table), s);
        જો (t >= 0)
                પાછા t;
        જો (STRPTR_IN_SET(s, "µs", "μs")) /* accept both µ symbols in unicode, i.e. micro symbol + Greek small letter mu. */
                પાછા TIMESTAMP_US;
        જો (STRPTR_IN_SET(s, "µs+utc", "μs+utc"))
                પાછા TIMESTAMP_US_UTC;
        પાછા t;
}

પૂર્ણાંક parse_calendar_date_full(અચલ અક્ષર *s, bool allow_pre_epoch, usec_t *ret_usec, સંરચના tm *ret_tm) {
        સંરચના tm parsed_tm = {}, copy_tm;
        અચલ અક્ષર *k;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        k = strptime(s, "%Y-%m-%d", &parsed_tm);
        જો (!k || *k)
                પાછા -EINVAL;

        copy_tm = parsed_tm;

        usec_t usec = USEC_INFINITY;

        જો (allow_pre_epoch) {
                /* For birth dates we use timegm() directly since we need to accept pre-epoch dates.
                 * timegm() returns (time_t) -1 both on error and for one second before the epoch.
                 * Initialize wday to -1 beforehand: if it remains -1 after the call, it's a genuine
                 * error; if timegm() changed it, the date was successfully normalized. */
                copy_tm.tm_wday = -1;
                જો (timegm(&copy_tm) == (time_t) -1 && copy_tm.tm_wday == -1)
                        પાછા -EINVAL;
        } નહીંતર {
                r = mktime_or_timegm_usec(&copy_tm, /* utc= */ true, &usec);
                જો (r < 0)
                        પાછા r;
        }

        /* Refuse non-normalized dates, e.g. Feb 30 */
        જો (copy_tm.tm_mday != parsed_tm.tm_mday ||
            copy_tm.tm_mon  != parsed_tm.tm_mon  ||
            copy_tm.tm_year != parsed_tm.tm_year)
                પાછા -EINVAL;

        જો (ret_usec)
                *ret_usec = usec;
        જો (ret_tm) {
                /* Reset to unset, then fill in only the date fields we parsed and validated */
                *ret_tm = BIRTH_DATE_UNSET;
                ret_tm->tm_mday = parsed_tm.tm_mday;
                ret_tm->tm_mon = parsed_tm.tm_mon;
                ret_tm->tm_year = parsed_tm.tm_year;
        }

        પાછા 0;
}
