/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <syslog.h>

#ಅನ್ತರ್ಭಾವ "sd-id128.h"

#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "syslog-util.h"
#ಅನ್ತರ್ಭಾವ "unit-name.h"

ಪೂರ್ಣಾಙ್ಕ syslog_parse_priority(ಧ್ರುವ ವರ್ಣ **p, ಪೂರ್ಣಾಙ್ಕ *priority, bool with_facility) {
        ಪೂರ್ಣಾಙ್ಕ a = 0, b = 0, c = 0;
        ಧ್ರುವ ವರ್ಣ *end;
        size_t k;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(*p);
        ಪ್ರತಿಜ್ಞಾ(priority);

        ಯದಿ ((*p)[0] != '<')
                ಪ್ರತ್ಯಾಗಮ 0;

        end = strchr(*p, '>');
        ಯದಿ (!end)
                ಪ್ರತ್ಯಾಗಮ 0;

        k = end - *p;
        ಪ್ರತಿಜ್ಞಾ(k > 0);

        ಯದಿ (k == 2)
                c = undecchar((*p)[1]);
        ಅನ್ಯಥಾ ಯದಿ (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } ಅನ್ಯಥಾ ಯದಿ (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (with_facility)
                *priority = a*100 + b*10 + c;
        ಅನ್ಯಥಾ
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ log_facility_unshifted_table[LOG_NFACILITIES] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ಪೂರ್ಣಾಙ್ಕ, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(ಪೂರ್ಣಾಙ್ಕ facility) {
        ಪ್ರತ್ಯಾಗಮ facility >= 0 && facility <= LOG_FAC(~0);
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ log_level_table[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, ಪೂರ್ಣಾಙ್ಕ, LOG_DEBUG);

bool log_level_is_valid(ಪೂರ್ಣಾಙ್ಕ level) {
        ಪ್ರತ್ಯಾಗಮ level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#define LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(ಧ್ರುವ ವರ್ಣ *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        ಯದಿ (!string_is_safe(s, STRING_FILENAME))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (strlen(s) > LOG_NAMESPACE_MAX)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!unit_instance_is_valid(s))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}
