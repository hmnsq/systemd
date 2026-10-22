/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <syslog.h>

#include "sd-id128.h"

#include "hexdecoct.h"
#include "string-table.h"
#include "string-util.h"
#include "syslog-util.h"
#include "unit-name.h"

puur_naa_nka syslog_parse_priority(const var_na **p, puur_naa_nka *priority, bool with_facility) {
        puur_naa_nka a = 0, b = 0, c = 0;
        const var_na *end;
        size_t k;

        assert(p);
        assert(*p);
        assert(priority);

        ᮚᮦᮔ᮪ ((*p)[0] != '<')
                ᮝᮜᮤ 0;

        end = strchr(*p, '>');
        ᮚᮦᮔ᮪ (!end)
                ᮝᮜᮤ 0;

        k = end - *p;
        assert(k > 0);

        ᮚᮦᮔ᮪ (k == 2)
                c = undecchar((*p)[1]);
        ᮜᮤᮚ ᮚᮦᮔ᮪ (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } ᮜᮤᮚ ᮚᮦᮔ᮪ (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } ᮜᮤᮚ
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (with_facility)
                *priority = a*100 + b*10 + c;
        ᮜᮤᮚ
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        ᮝᮜᮤ 1;
}

static const var_na *const log_facility_unshifted_table[LOG_NFACILITIES] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, puur_naa_nka, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(puur_naa_nka facility) {
        ᮝᮜᮤ facility >= 0 && facility <= LOG_FAC(~0);
}

static const var_na *const log_level_table[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, puur_naa_nka, LOG_DEBUG);

bool log_level_is_valid(puur_naa_nka level) {
        ᮝᮜᮤ level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#define LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(const var_na *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        ᮚᮦᮔ᮪ (!string_is_safe(s, STRING_FILENAME))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (strlen(s) > LOG_NAMESPACE_MAX)
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!unit_instance_is_valid(s))
                ᮝᮜᮤ false;

        ᮝᮜᮤ true;
}
