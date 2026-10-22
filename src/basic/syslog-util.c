/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <syslog.h>

#समावेश "sd-id128.h"

#समावेश "hexdecoct.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "syslog-util.h"
#समावेश "unit-name.h"

पूर्णांक syslog_parse_priority(स्थिर अक्षर **p, पूर्णांक *priority, bool with_facility) {
        पूर्णांक a = 0, b = 0, c = 0;
        स्थिर अक्षर *end;
        माप_प्रकार k;

        निश्चित(p);
        निश्चित(*p);
        निश्चित(priority);

        अगर ((*p)[0] != '<')
                वापस 0;

        end = म_अक्षर(*p, '>');
        अगर (!end)
                वापस 0;

        k = end - *p;
        निश्चित(k > 0);

        अगर (k == 2)
                c = undecchar((*p)[1]);
        अन्यथा अगर (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } अन्यथा अगर (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } अन्यथा
                वापस 0;

        अगर (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                वापस 0;

        अगर (with_facility)
                *priority = a*100 + b*10 + c;
        अन्यथा
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        वापस 1;
}

अटल स्थिर अक्षर *स्थिर log_facility_unshifted_table[LOG_NFACILITIES] = {
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

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_facility_unshifted, पूर्णांक, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(पूर्णांक facility) {
        वापस facility >= 0 && facility <= LOG_FAC(~0);
}

अटल स्थिर अक्षर *स्थिर log_level_table[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(log_level, पूर्णांक, LOG_DEBUG);

bool log_level_is_valid(पूर्णांक level) {
        वापस level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#घोषणा LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(स्थिर अक्षर *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        अगर (!string_is_safe(s, STRING_FILENAME))
                वापस false;

        अगर (म_माप(s) > LOG_NAMESPACE_MAX)
                वापस false;

        अगर (!unit_instance_is_valid(s))
                वापस false;

        वापस true;
}
