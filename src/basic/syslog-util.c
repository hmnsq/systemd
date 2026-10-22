/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <syslog.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "sd-id128.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "syslog-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unit-name.h"

ਪੂਰਨ syslog_parse_ਤਰਜੀਹ(ਅਚਲ ਅੱਖਰ **p, ਪੂਰਨ *priority, bool with_facility) {
        ਪੂਰਨ a = 0, b = 0, c = 0;
        ਅਚਲ ਅੱਖਰ *ਸਿਰਾ;
        size_t k;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(*p);
        ਪ੍ਰਤਿਜ੍ਞਾ(priority);

        ਜੇ ((*p)[0] != '<')
                ਵਾਪਸ 0;

        ਸਿਰਾ = strchr(*p, '>');
        ਜੇ (!ਸਿਰਾ)
                ਵਾਪਸ 0;

        k = ਸਿਰਾ - *p;
        ਪ੍ਰਤਿਜ੍ਞਾ(k > 0);

        ਜੇ (k == 2)
                c = undecchar((*p)[1]);
        ਨਹੀਂਤਾਂ ਜੇ (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } ਨਹੀਂਤਾਂ ਜੇ (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } ਨਹੀਂਤਾਂ
                ਵਾਪਸ 0;

        ਜੇ (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                ਵਾਪਸ 0;

        ਜੇ (with_facility)
                *priority = a*100 + b*10 + c;
        ਨਹੀਂਤਾਂ
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ log_facility_unshifted_ਸਾਰਣੀ[LOG_NFACILITIES] = {
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

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ਪੂਰਨ, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(ਪੂਰਨ facility) {
        ਵਾਪਸ facility >= 0 && facility <= LOG_FAC(~0);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ log_level_ਸਾਰਣੀ[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(log_level, ਪੂਰਨ, LOG_DEBUG);

bool log_level_is_valid(ਪੂਰਨ level) {
        ਵਾਪਸ level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(ਅਚਲ ਅੱਖਰ *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        ਜੇ (!string_is_ਸੁਰੱਖਿਅਤ(s, STRING_FILENAME))
                ਵਾਪਸ false;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(s) > LOG_NAMESPACE_MAX)
                ਵਾਪਸ false;

        ਜੇ (!unit_instance_is_valid(s))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}
