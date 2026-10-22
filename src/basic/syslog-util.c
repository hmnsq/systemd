/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <syslog.h>

#અન્તર્ભાવ "sd-id128.h"

#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "syslog-util.h"
#અન્તર્ભાવ "unit-name.h"

પૂર્ણાંક syslog_parse_priority(અચલ અક્ષર **p, પૂર્ણાંક *priority, bool with_facility) {
        પૂર્ણાંક a = 0, b = 0, c = 0;
        અચલ અક્ષર *end;
        size_t k;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(*p);
        પ્રતિજ્ઞા(priority);

        જો ((*p)[0] != '<')
                પાછા 0;

        end = strchr(*p, '>');
        જો (!end)
                પાછા 0;

        k = end - *p;
        પ્રતિજ્ઞા(k > 0);

        જો (k == 2)
                c = undecchar((*p)[1]);
        નહીંતર જો (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } નહીંતર જો (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } નહીંતર
                પાછા 0;

        જો (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                પાછા 0;

        જો (with_facility)
                *priority = a*100 + b*10 + c;
        નહીંતર
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        પાછા 1;
}

સ્થિર અચલ અક્ષર *અચલ log_facility_unshifted_table[LOG_NFACILITIES] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, પૂર્ણાંક, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(પૂર્ણાંક facility) {
        પાછા facility >= 0 && facility <= LOG_FAC(~0);
}

સ્થિર અચલ અક્ષર *અચલ log_level_table[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, પૂર્ણાંક, LOG_DEBUG);

bool log_level_is_valid(પૂર્ણાંક level) {
        પાછા level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#define LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(અચલ અક્ષર *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        જો (!string_is_safe(s, STRING_FILENAME))
                પાછા false;

        જો (strlen(s) > LOG_NAMESPACE_MAX)
                પાછા false;

        જો (!unit_instance_is_valid(s))
                પાછા false;

        પાછા true;
}
