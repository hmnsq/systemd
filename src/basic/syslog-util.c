/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <syslog.h>

#అన్తర్భావ "sd-id128.h"

#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "syslog-util.h"
#అన్తర్భావ "unit-name.h"

పూర్ణాంకం syslog_parse_ప్రాధాన్యత(స్థిరరాశి అక్షరం **p, పూర్ణాంకం *priority, bool with_facility) {
        పూర్ణాంకం a = 0, b = 0, c = 0;
        స్థిరరాశి అక్షరం *చివర;
        size_t k;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(*p);
        ప్రతిజ్ఞా(priority);

        ఒకవేళ ((*p)[0] != '<')
                తిరిగి 0;

        చివర = strchr(*p, '>');
        ఒకవేళ (!చివర)
                తిరిగి 0;

        k = చివర - *p;
        ప్రతిజ్ఞా(k > 0);

        ఒకవేళ (k == 2)
                c = undecchar((*p)[1]);
        లేకపోతే ఒకవేళ (k == 3) {
                b = undecchar((*p)[1]);
                c = undecchar((*p)[2]);
        } లేకపోతే ఒకవేళ (k == 4) {
                a = undecchar((*p)[1]);
                b = undecchar((*p)[2]);
                c = undecchar((*p)[3]);
        } లేకపోతే
                తిరిగి 0;

        ఒకవేళ (a < 0 || b < 0 || c < 0 ||
            (!with_facility && (a || b || c > 7)))
                తిరిగి 0;

        ఒకవేళ (with_facility)
                *priority = a*100 + b*10 + c;
        లేకపోతే
                *priority = (*priority & LOG_FACMASK) | c;

        *p += k + 1;
        తిరిగి 1;
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి log_facility_unshifted_పట్టిక[LOG_NFACILITIES] = {
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

DEFINE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(log_facility_unshifted, పూర్ణాంకం, LOG_FAC(~0));

bool log_facility_unshifted_is_valid(పూర్ణాంకం facility) {
        తిరిగి facility >= 0 && facility <= LOG_FAC(~0);
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి log_level_పట్టిక[] = {
        [LOG_EMERG]   = "emerg",
        [LOG_ALERT]   = "alert",
        [LOG_CRIT]    = "crit",
        [LOG_ERR]     = "err",
        [LOG_WARNING] = "warning",
        [LOG_NOTICE]  = "notice",
        [LOG_INFO]    = "info",
        [LOG_DEBUG]   = "debug",
};

DEFINE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(log_level, పూర్ణాంకం, LOG_DEBUG);

bool log_level_is_valid(పూర్ణాంకం level) {
        తిరిగి level >= 0 && level <= LOG_DEBUG;
}

/* The maximum size for a log namespace length. This is the file name size limit 255 minus the size of a
 * formatted machine ID minus a separator char */
#నిర్వచించు LOG_NAMESPACE_MAX (NAME_MAX - (SD_ID128_STRING_MAX - 1) - 1)

bool log_namespace_name_valid(స్థిరరాశి అక్షరం *s) {
        /* Let's make sure the namespace fits in a filename that is prefixed with the machine ID and a dot
         * (so that /var/log/journal/<machine-id>.<namespace> can be created based on it). Also make sure it
         * is suitable as unit instance name, and does not contain fishy characters. */

        /* Let's avoid globbing for now */
        ఒకవేళ (!string_is_సురక్షిత(s, STRING_FILENAME))
                తిరిగి false;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(s) > LOG_NAMESPACE_MAX)
                తిరిగి false;

        ఒకవేళ (!unit_instance_is_valid(s))
                తిరిగి false;

        తిరిగి true;
}
