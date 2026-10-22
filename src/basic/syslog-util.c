/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <syslog.h>

#அந்தர்பாவ "sd-id128.h"

#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "syslog-util.h"
#அந்தர்பாவ "unit-name.h"

முழு syslog_parse_priority(மாறிலி எழுத்து **p, முழு *priority, bool with_facility) {
        முழு a = 0, b = 0, c = 0;
        மாறிலி எழுத்து *end;
        size_t k;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(*p);
        ப்ரதிஜ்ஞா(priority);

        எனில் ((*p)[0] != '<')
                திரும்பு 0;

        end = strchr(*p, '>');
        எனில் (!end)
                திரும்பு 0;

        k = end - *p;
        ப்ரதிஜ்ஞா(k > 0);

        எனில் (k == 2)
                c = undecchar((*p)[1]);
        இல்லையெனில் எனில் (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } இல்லையெனில் எனில் (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } இல்லையெனில்
                திரும்பு 0;

        எனில் (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                திரும்பு 0;

        எனில் (with_facility)
                *priority = a*100 + b*10 + c;
        இல்லையெனில்
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        திரும்பு 1;
}

நிலையான மாறிலி எழுத்து *மாறிலி log_facility_unshifted_table[LOG_NFACILITIES] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, முழு, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(முழு facility) {
        திரும்பு facility >= 0 && facility <= LOG_FAC(~0);
}

நிலையான மாறிலி எழுத்து *மாறிலி log_level_table[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, முழு, LOG_DEBUG);

bool log_level_is_valid(முழு level) {
        திரும்பு level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#define LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(மாறிலி எழுத்து *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        எனில் (!string_is_safe(s, STRING_FILENAME))
                திரும்பு false;

        எனில் (strlen(s) > LOG_NAMESPACE_MAX)
                திரும்பு false;

        எனில் (!unit_instance_is_valid(s))
                திரும்பு false;

        திரும்பு true;
}
