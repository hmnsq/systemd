/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <syslog.h>

#അന്തര്ഭാവ "sd-id128.h"

#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "syslog-util.h"
#അന്തര്ഭാവ "unit-name.h"

പൂർണ്ണസംഖ്യ syslog_parse_priority(സ്ഥിരം അക്ഷരം **p, പൂർണ്ണസംഖ്യ *priority, bool with_facility) {
        പൂർണ്ണസംഖ്യ a = 0, b = 0, c = 0;
        സ്ഥിരം അക്ഷരം *end;
        size_t k;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(*p);
        പ്രതിജ്ഞാ(priority);

        എങ്കിൽ ((*p)[0] != '<')
                തിരികെ 0;

        end = strchr(*p, '>');
        എങ്കിൽ (!end)
                തിരികെ 0;

        k = end - *p;
        പ്രതിജ്ഞാ(k > 0);

        എങ്കിൽ (k == 2)
                c = undecchar((*p)[1]);
        അല്ലെങ്കിൽ എങ്കിൽ (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } അല്ലെങ്കിൽ എങ്കിൽ (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } അല്ലെങ്കിൽ
                തിരികെ 0;

        എങ്കിൽ (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                തിരികെ 0;

        എങ്കിൽ (with_facility)
                *priority = a*100 + b*10 + c;
        അല്ലെങ്കിൽ
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം log_facility_unshifted_table[LOG_NFACILITIES] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, പൂർണ്ണസംഖ്യ, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(പൂർണ്ണസംഖ്യ facility) {
        തിരികെ facility >= 0 && facility <= LOG_FAC(~0);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം log_level_table[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, പൂർണ്ണസംഖ്യ, LOG_DEBUG);

bool log_level_is_valid(പൂർണ്ണസംഖ്യ level) {
        തിരികെ level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#define LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(സ്ഥിരം അക്ഷരം *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        എങ്കിൽ (!string_is_safe(s, STRING_FILENAME))
                തിരികെ false;

        എങ്കിൽ (strlen(s) > LOG_NAMESPACE_MAX)
                തിരികെ false;

        എങ്കിൽ (!unit_instance_is_valid(s))
                തിരികെ false;

        തിരികെ true;
}
