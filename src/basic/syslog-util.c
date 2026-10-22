/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <syslog.h>

#अन्तर्भाव "sd-id128.h"

#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "syslog-util.h"
#अन्तर्भाव "unit-name.h"

पूर्णांक syslog_parse_priority(स्थिरांक अक्षर **p, पूर्णांक *priority, bool with_facility) {
        पूर्णांक a = 0, b = 0, c = 0;
        स्थिरांक अक्षर *end;
        माप_प्रकार k;

        प्रतिज्ञा(p);
        प्रतिज्ञा(*p);
        प्रतिज्ञा(priority);

        जर ((*p)[0] != '<')
                परत 0;

        end = म_अक्षर(*p, '>');
        जर (!end)
                परत 0;

        k = end - *p;
        प्रतिज्ञा(k > 0);

        जर (k == 2)
                c = undecchar((*p)[1]);
        अन्यथा जर (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } अन्यथा जर (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } अन्यथा
                परत 0;

        जर (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                परत 0;

        जर (with_facility)
                *priority = a*100 + b*10 + c;
        अन्यथा
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        परत 1;
}

स्थिर स्थिरांक अक्षर *स्थिरांक log_facility_unshifted_table[LOG_NFACILITIES] = {
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
        परत facility >= 0 && facility <= LOG_FAC(~0);
}

स्थिर स्थिरांक अक्षर *स्थिरांक log_level_table[] = {
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
        परत level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#define LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(स्थिरांक अक्षर *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        जर (!string_is_safe(s, STRING_FILENAME))
                परत false;

        जर (म_माप(s) > LOG_NAMESPACE_MAX)
                परत false;

        जर (!unit_instance_is_valid(s))
                परत false;

        परत true;
}
