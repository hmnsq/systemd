/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <syslog.h>

#شامل "sd-id128.h"

#شامل "hexdecoct.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "syslog-util.h"
#شامل "unit-name.h"

صحیح syslog_parse_ترجیح(ثابت حرف **p, صحیح *priority, bool with_facility) {
        صحیح a = 0, b = 0, c = 0;
        ثابت حرف *انتہا;
        size_t k;

        أكد(p);
        أكد(*p);
        أكد(priority);

        اگر ((*p)[0] != '<')
                واپس 0;

        انتہا = strchr(*p, '>');
        اگر (!انتہا)
                واپس 0;

        k = انتہا - *p;
        أكد(k > 0);

        اگر (k == 2)
                c = undecchar((*p)[1]);
        ورنہ اگر (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } ورنہ اگر (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } ورنہ
                واپس 0;

        اگر (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                واپس 0;

        اگر (with_facility)
                *priority = a*100 + b*10 + c;
        ورنہ
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        واپس 1;
}

ساکن ثابت حرف *ثابت log_facility_unshifted_جدول[LOG_NFACILITIES] = {
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

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(log_facility_unshifted, صحیح, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(صحیح facility) {
        واپس facility >= 0 && facility <= LOG_FAC(~0);
}

ساکن ثابت حرف *ثابت log_level_جدول[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_جدول_LOOKUP_WITH_FALLBACK(log_level, صحیح, LOG_DEBUG);

bool log_level_is_valid(صحیح level) {
        واپس level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#تعریف_کرو LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(ثابت حرف *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        اگر (!string_is_محفوظ(s, STRING_FILENAME))
                واپس false;

        اگر (لڑی_لمبائی(s) > LOG_NAMESPACE_MAX)
                واپس false;

        اگر (!unit_instance_is_valid(s))
                واپس false;

        واپس true;
}
