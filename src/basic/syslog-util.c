/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <syslog.h>

#ಅನ್ತರ್ಭಾವ "sd-id128.h"

#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "syslog-util.h"
#ಅನ್ತರ್ಭಾವ "unit-name.h"

ಪೂರ್ಣಾಂಕ syslog_parse_ಆದ್ಯತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, ಪೂರ್ಣಾಂಕ *priority, bool with_facility) {
        ಪೂರ್ಣಾಂಕ a = 0, b = 0, c = 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ;
        size_t k;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(*p);
        ಪ್ರತಿಜ್ಞಾ(priority);

        ಆದರೆ ((*p)[0] != '<')
                ಹಿಂತಿರುಗಿ 0;

        ಕೊನೆ = strchr(*p, '>');
        ಆದರೆ (!ಕೊನೆ)
                ಹಿಂತಿರುಗಿ 0;

        k = ಕೊನೆ - *p;
        ಪ್ರತಿಜ್ಞಾ(k > 0);

        ಆದರೆ (k == 2)
                c = undecchar((*p)[1]);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (with_facility)
                *priority = a*100 + b*10 + c;
        ಇಲ್ಲದಿದ್ದರೆ
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ log_facility_unshifted_ಕೋಷ್ಟಕ[LOG_NFACILITIES] = {
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

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(log_facility_unshifted, ಪೂರ್ಣಾಂಕ, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(ಪೂರ್ಣಾಂಕ facility) {
        ಹಿಂತಿರುಗಿ facility >= 0 && facility <= LOG_FAC(~0);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ log_level_ಕೋಷ್ಟಕ[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(log_level, ಪೂರ್ಣಾಂಕ, LOG_DEBUG);

bool log_level_is_valid(ಪೂರ್ಣಾಂಕ level) {
        ಹಿಂತಿರುಗಿ level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        ಆದರೆ (!string_is_ಸುರಕ್ಷಿತ(s, STRING_FILENAME))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s) > LOG_NAMESPACE_MAX)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!unit_instance_is_valid(s))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}
