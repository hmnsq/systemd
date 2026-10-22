/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <syslog.h>

#ضمن "sd-id128.h"

#ضمن "hexdecoct.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "syslog-util.h"
#ضمن "unit-name.h"

صحيح syslog_parse_اولویت(ثابت حرف **p, صحيح *priority, bool with_facility) {
        صحيح a = 0, b = 0, c = 0;
        ثابت حرف *انتها;
        size_t k;

        أكد(p);
        أكد(*p);
        أكد(priority);

        إذا ((*p)[0] != '<')
                أرجع 0;

        انتها = strchr(*p, '>');
        إذا (!انتها)
                أرجع 0;

        k = انتها - *p;
        أكد(k > 0);

        إذا (k == 2)
                c = undecchar((*p)[1]);
        وإلا إذا (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } وإلا إذا (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } وإلا
                أرجع 0;

        إذا (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                أرجع 0;

        إذا (with_facility)
                *priority = a*100 + b*10 + c;
        وإلا
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        أرجع 1;
}

ساكن ثابت حرف *ثابت log_facility_unshifted_جدول[LOG_NFACILITIES] = {
        [LOG_FAC(LOG_KERN)]     = "kern",
        [LOG_FAC(LOG_USER)]     = "user",
        [LOG_FAC(LOG_MAIL)]     = "mail",
        [LOG_FAC(LOG_DAEMON)]   = "daemon",
        [LOG_FAC(LOG_AUTH)]     = "auth",
        [LOG_FAC(LOG_SYSLOG)]   = "syslog",
        [LOG_FAC(LOG_LPR)]      = "lpr",
        [LOG_FAC(LOG_NEWS)]     = "news",
        [LOG_FAC(LOG_UUCP)]     = "uucp",
        [LOG_FAC(LOG_CRON)]     = "cron",
        [LOG_FAC(LOG_AUTHPRIV)] = "authpriv",
        [LOG_FAC(LOG_FTP)]      = "ftp",
        [LOG_FAC(LOG_LOCAL0)]   = "local0",
        [LOG_FAC(LOG_LOCAL1)]   = "local1",
        [LOG_FAC(LOG_LOCAL2)]   = "local2",
        [LOG_FAC(LOG_LOCAL3)]   = "local3",
        [LOG_FAC(LOG_LOCAL4)]   = "local4",
        [LOG_FAC(LOG_LOCAL5)]   = "local5",
        [LOG_FAC(LOG_LOCAL6)]   = "local6",
        [LOG_FAC(LOG_LOCAL7)]   = "local7",
};

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(log_facility_unshifted, صحيح, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(صحيح facility) {
        أرجع facility >= 0 && facility <= LOG_FAC(~0);
}

ساكن ثابت حرف *ثابت log_level_جدول[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(log_level, صحيح, LOG_DEBUG);

bool log_level_is_valid(صحيح level) {
        أرجع level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#تعریف_کن LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(ثابت حرف *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        إذا (!string_is_امن(s, STRING_FILENAME))
                أرجع false;

        إذا (طول_رشته(s) > LOG_NAMESPACE_MAX)
                أرجع false;

        إذا (!unit_instance_is_valid(s))
                أرجع false;

        أرجع true;
}
