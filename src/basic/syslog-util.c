/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <syslog.h>

#অন্তর্ভাব "sd-id128.h"

#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "syslog-util.h"
#অন্তর্ভাব "unit-name.h"

পূর্ণসংখ্যা syslog_parse_priority(ধ্রুবক অক্ষর **p, পূর্ণসংখ্যা *priority, bool with_facility) {
        পূর্ণসংখ্যা a = 0, b = 0, c = 0;
        ধ্রুবক অক্ষর *end;
        size_t k;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(*p);
        প্রতিজ্ঞা(priority);

        যদি ((*p)[0] != '<')
                ফেরত 0;

        end = strchr(*p, '>');
        যদি (!end)
                ফেরত 0;

        k = end - *p;
        প্রতিজ্ঞা(k > 0);

        যদি (k == 2)
                c = undecchar((*p)[1]);
        অন্যথা যদি (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } অন্যথা যদি (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } অন্যথা
                ফেরত 0;

        যদি (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                ফেরত 0;

        যদি (with_facility)
                *priority = a*100 + b*10 + c;
        অন্যথা
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        ফেরত 1;
}

স্থির ধ্রুবক অক্ষর *ধ্রুবক log_facility_unshifted_table[LOG_NFACILITIES] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, পূর্ণসংখ্যা, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(পূর্ণসংখ্যা facility) {
        ফেরত facility >= 0 && facility <= LOG_FAC(~0);
}

স্থির ধ্রুবক অক্ষর *ধ্রুবক log_level_table[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, পূর্ণসংখ্যা, LOG_DEBUG);

bool log_level_is_valid(পূর্ণসংখ্যা level) {
        ফেরত level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#define LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(ধ্রুবক অক্ষর *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        যদি (!string_is_safe(s, STRING_FILENAME))
                ফেরত false;

        যদি (strlen(s) > LOG_NAMESPACE_MAX)
                ফেরত false;

        যদি (!unit_instance_is_valid(s))
                ফেরত false;

        ফেরত true;
}
