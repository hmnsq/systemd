/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "initrd-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "proc-cmdline.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "virt.h"

రకనిర్వచనం గణన ArgType {
        no_వాదం,
        required_వాదం,
        optional_వాదం,
} ArgType;

పూర్ణాంకం proc_cmdline_filter_pid1_వాదనలు(అక్షరం **argv, అక్షరం ***ret) {
        స్థిరరాశి నిర్మాణం {
                స్థిరరాశి అక్షరం *name;
                ArgType has_arg;
        } options[] = {
                { "log-level",                required_వాదం },
                { "log-target",               required_వాదం },
                { "log-color",                optional_వాదం },
                { "log-location",             optional_వాదం },
                { "log-time",                 optional_వాదం },
                { "unit",                     required_వాదం },
                { "system",                   no_వాదం,      },
                { "user",                     no_వాదం,      },
                { "test",                     no_వాదం,      },
                { "no-pager",                 no_వాదం,      },
                { "help",                     no_వాదం,      },
                { "version",                  no_వాదం,      },
                { "introspect-cli",           no_వాదం,      },
                { "dump-configuration-items", no_వాదం,      },
                { "dump-bus-properties",      no_వాదం,      },
                { "bus-introspect",           required_వాదం },
                { "dump-core",                optional_వాదం },
                { "crash-chvt",               required_వాదం },
                { "crash-vt",                 required_వాదం },
                { "crash-shell",              optional_వాదం },
                { "crash-reboot",             optional_వాదం },
                { "crash-action",             required_వాదం },
                { "confirm-spawn",            optional_వాదం },
                { "show-status",              optional_వాదం },
                { "deserialize",              required_వాదం },
                { "switched-root",            no_వాదం,      },
                { "default-standard-output",  required_వాదం },
                { "default-standard-error",   required_వాదం },
                { "machine-id",               required_వాదం },
                { "service-watchdogs",        required_వాదం },
                { "exit-code",                required_వాదం },
                { "timeout",                  required_వాదం },
        };
        స్థిరరాశి అక్షరం *short_options = "hDbsz:";

        _cleanup_strv_free_ అక్షరం **filtered = ఖాళీ;
        పూర్ణాంకం state, r;

        ప్రతిజ్ఞా(argv);
        ప్రతిజ్ఞా(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        ప్రతిజ్ఞా(!లోపల_సమితి(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_వాదం;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                పూర్ణాంకం prev_state = state;
                స్థిరరాశి అక్షరం *a = *p;

                /* Reset the state for the next step. */
                state = no_వాదం;

                ఒకవేళ (prev_state == required_వాదం ||
                    (prev_state == optional_వాదం && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        కొనసాగు;

                ఒకవేళ (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        కొనసాగు;
                }

                ఒకవేళ (a[1] == '-') {
                        ఒకవేళ (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                                ఆపు;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                స్థిరరాశి అక్షరం *q = startswith(a + 2, option->name);
                                ఒకవేళ (!q || !లోపల_సమితి(q[0], '=', '\0'))
                                        కొనసాగు;

                                /* Found matching option, updating the state if necessary. */
                                ఒకవేళ (q[0] == '\0' && option->has_arg == required_వాదం)
                                        state = required_వాదం;

                                ఆపు;
                        }
                        కొనసాగు;
                }

                /* short option(s), e.g. -x or -xyz */
                అయితే (a && *++a != '\0')
                        కొరకు (స్థిరరాశి అక్షరం *q = short_options; *q != '\0'; q++) {
                                ఒకవేళ (*q != *a)
                                        కొనసాగు;

                                /* Found matching short option. */

                                ఒకవేళ (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_వాదం :
                                                q[2] == ':' ? optional_వాదం : required_వాదం;

                                        a = ఖాళీ; /* Not necessary to parse remaining part. */
                                }
                                ఆపు;
                        }
        }

        *ret = TAKE_PTR(filtered);
        తిరిగి 0;
}

పూర్ణాంకం proc_cmdline(అక్షరం **ret) {
        ప్రతిజ్ఞా(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        స్థిరరాశి అక్షరం *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ఒకవేళ (e)
                తిరిగి strdup_గమ్యం(ret, e);

        ఒకవేళ (detect_container() > 0)
                తిరిగి pid_get_cmdline(1, SIZE_MAX, 0, ret);

        తిరిగి read_నిండిన_file("/proc/cmdline", ret, /* ret_size= */  ఖాళీ);
}

స్థిర పూర్ణాంకం proc_cmdline_strv_అంతర్గత(అక్షరం ***ret, bool filter_pid1_వాదనలు) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        స్థిరరాశి అక్షరం *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ఒకవేళ (e)
                తిరిగి strv_విభజించు_నిండిన(ret, e, ఖాళీ, వెలికితీయి_UNQUOTE|వెలికితీయి_RELAX|వెలికితీయి_RETAIN_ESCAPE);

        ఒకవేళ (detect_container() > 0) {
                _cleanup_strv_free_ అక్షరం **వాదనలు = ఖాళీ;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &వాదనలు);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (filter_pid1_వాదనలు)
                        తిరిగి proc_cmdline_filter_pid1_వాదనలు(వాదనలు, ret);

                *ret = TAKE_PTR(వాదనలు);
                తిరిగి 0;

        } లేకపోతే {
                _cleanup_free_ అక్షరం *s = ఖాళీ;

                r = read_నిండిన_file("/proc/cmdline", &s, ఖాళీ);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                తిరిగి strv_విభజించు_నిండిన(ret, s, ఖాళీ, వెలికితీయి_UNQUOTE|వెలికితీయి_RELAX|వెలికితీయి_RETAIN_ESCAPE);
        }
}

పూర్ణాంకం proc_cmdline_strv(అక్షరం ***ret) {
        తిరిగి proc_cmdline_strv_అంతర్గత(ret, /* filter_pid1_args= */ false);
}

స్థిర అక్షరం *mangle_మాట(స్థిరరాశి అక్షరం *మాట, ProcCmdlineFlags flags) {
        అక్షరం *c = (అక్షరం*) startswith(మాట, "rd.");
        ఒకవేళ (c) {
                /* Filter out arguments that are intended only for the initrd */

                ఒకవేళ (!లోపల_initrd())
                        తిరిగి ఖాళీ;

                ఒకవేళ (FLAGS_సమితి(flags, PROC_CMDLINE_STRIP_RD_ఉపసర్గ))
                        తిరిగి c;

        } లేకపోతే ఒకవేళ (FLAGS_సమితి(flags, PROC_CMDLINE_RD_STRICT) && లోపల_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                తిరిగి ఖాళీ;

        తిరిగి (అక్షరం*) మాట;
}

#నిర్వచించు mangle_మాట(మాట, flags) const_generic(మాట, mangle_మాట(మాట, flags))

స్థిర పూర్ణాంకం proc_cmdline_parse_strv(అక్షరం **వాదనలు, proc_cmdline_parse_t parse_అంశం, శూన్య *దత్తాంశం, ProcCmdlineFlags flags) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(parse_అంశం);

        STRV_FOREACH(మాట, వాదనలు) {
                అక్షరం *కీలకం, *విలువ;

                కీలకం = mangle_మాట(*మాట, flags);
                ఒకవేళ (!కీలకం)
                        కొనసాగు;

                విలువ = strchr(కీలకం, '=');
                ఒకవేళ (విలువ)
                        *(విలువ++) = '\0';

                r = parse_అంశం(కీలకం, విలువ, దత్తాంశం);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం proc_cmdline_parse(proc_cmdline_parse_t parse_అంశం, శూన్య *దత్తాంశం, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ అక్షరం **వాదనలు = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(parse_అంశం);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        ప్రతిజ్ఞా(!(flags & (PROC_CMDLINE_విలువ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_అంతర్గత(&వాదనలు, /* filter_pid1_args= */ true);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి proc_cmdline_parse_strv(వాదనలు, parse_అంశం, దత్తాంశం, flags);
}

స్థిర bool relaxed_equal_char(అక్షరం a, అక్షరం b) {
        తిరిగి a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

అక్షరం* proc_cmdline_కీలకం_startswith(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *ఉపసర్గ) {
        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ఉపసర్గ);

        /* Much like startswith(), but considers "-" and "_" the same */

        కొరకు (; *ఉపసర్గ != 0; s++, ఉపసర్గ++)
                ఒకవేళ (!relaxed_equal_char(*s, *ఉపసర్గ))
                        తిరిగి ఖాళీ;

        తిరిగి (అక్షరం*) s;
}

bool proc_cmdline_కీలకం_streq(స్థిరరాశి అక్షరం *x, స్థిరరాశి అక్షరం *y) {
        ప్రతిజ్ఞా(x);
        ప్రతిజ్ఞా(y);

        /* Much like streq(), but considers "-" and "_" the same */

        కొరకు (; *x != 0 || *y != 0; x++, y++)
                ఒకవేళ (!relaxed_equal_char(*x, *y))
                        తిరిగి false;

        తిరిగి true;
}

స్థిర పూర్ణాంకం cmdline_get_కీలకం(అక్షరం **వాదనలు, స్థిరరాశి అక్షరం *కీలకం, ProcCmdlineFlags flags, అక్షరం **ret_విలువ) {
        _cleanup_free_ అక్షరం *v = ఖాళీ;
        bool దొరికింది = false;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(కీలకం);

        STRV_FOREACH(p, వాదనలు) {
                స్థిరరాశి అక్షరం *మాట;

                మాట = mangle_మాట(*p, flags);
                ఒకవేళ (!మాట)
                        కొనసాగు;

                ఒకవేళ (ret_విలువ) {
                        స్థిరరాశి అక్షరం *e;

                        e = proc_cmdline_కీలకం_startswith(మాట, కీలకం);
                        ఒకవేళ (!e)
                                కొనసాగు;

                        ఒకవేళ (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                దొరికింది = true;

                        } లేకపోతే ఒకవేళ (*e == 0 && FLAGS_సమితి(flags, PROC_CMDLINE_విలువ_OPTIONAL))
                                దొరికింది = true;

                } లేకపోతే {
                        ఒకవేళ (proc_cmdline_కీలకం_streq(మాట, కీలకం)) {
                                దొరికింది = true;
                                ఆపు; /* we found what we were looking for */
                        }
                }
        }

        ఒకవేళ (ret_విలువ)
                *ret_విలువ = TAKE_PTR(v);

        తిరిగి దొరికింది;
}

పూర్ణాంకం proc_cmdline_get_కీలకం(స్థిరరాశి అక్షరం *కీలకం, ProcCmdlineFlags flags, అక్షరం **ret_విలువ) {
        _cleanup_strv_free_ అక్షరం **వాదనలు = ఖాళీ;
        పూర్ణాంకం r;

        /* Looks for a specific key on the kernel command line. Supports three modes:
         *
         * a) The "ret_value" parameter is used. In this case a parameter beginning with the "key" string followed by
         *    "=" is searched for, and the value following it is returned in "ret_value".
         *
         * b) as above, but the PROC_CMDLINE_VALUE_OPTIONAL flag is set. In this case if the key is found as a separate
         *    word (i.e. not followed by "=" but instead by whitespace or the end of the command line), then this is
         *    also accepted, and "value" is returned as NULL.
         *
         * c) The "ret_value" parameter is NULL. In this case a search for the exact "key" parameter is performed.
         *
         * In all three cases, > 0 is returned if the key is found, 0 if not. */

        /* PROC_CMDLINE_TRUE_WHEN_MISSING doesn't really make sense for proc_cmdline_get_key(). */
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        ఒకవేళ (isempty(కీలకం))
                తిరిగి -EINVAL;

        ఒకవేళ (FLAGS_సమితి(flags, PROC_CMDLINE_విలువ_OPTIONAL) && !ret_విలువ)
                తిరిగి -EINVAL;

        r = proc_cmdline_strv_అంతర్గత(&వాదనలు, /* filter_pid1_args= */ true);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి cmdline_get_కీలకం(వాదనలు, కీలకం, flags, ret_విలువ);
}

పూర్ణాంకం proc_cmdline_get_bool(స్థిరరాశి అక్షరం *కీలకం, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ అక్షరం *v = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = proc_cmdline_get_కీలకం(కీలకం, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_విలువ_OPTIONAL, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0) { /* key not specified at all */
                *ret = FLAGS_సమితి(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                తిరిగి 0;
        }

        ఒకవేళ (v) { /* key with parameter passed */
                r = parse_boolean(v);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                *ret = r;
        } లేకపోతే /* key without parameter passed */
                *ret = true;

        తిరిగి 1;
}

స్థిర పూర్ణాంకం cmdline_get_కీలకం_ap(ProcCmdlineFlags flags, అక్షరం* స్థిరరాశి* వాదనలు, va_list ap) {
        పూర్ణాంకం r, ret = 0;

        కొరకు (;;) {
                అక్షరం **v;
                స్థిరరాశి అక్షరం *k, *e;

                k = va_arg(ap, స్థిరరాశి అక్షరం*);
                ఒకవేళ (!k)
                        ఆపు;

                assert_se(v = va_arg(ap, అక్షరం**));

                STRV_FOREACH(p, వాదనలు) {
                        స్థిరరాశి అక్షరం *మాట;

                        మాట = mangle_మాట(*p, flags);
                        ఒకవేళ (!మాట)
                                కొనసాగు;

                        e = proc_cmdline_కీలకం_startswith(మాట, k);
                        ఒకవేళ (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                ret++;
                        }
                }
        }

        తిరిగి ret;
}

పూర్ణాంకం proc_cmdline_get_కీలకం_many_అంతర్గత(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ అక్షరం **వాదనలు = ఖాళీ;
        పూర్ణాంకం r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        ప్రతిజ్ఞా(!(flags & (PROC_CMDLINE_విలువ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&వాదనలు);
        ఒకవేళ (r < 0)
                తిరిగి r;

        va_start(ap, flags);
        r = cmdline_get_కీలకం_ap(flags, వాదనలు, ap);
        va_end(ap);

        తిరిగి r;
}

bool proc_cmdline_విలువ_missing(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        ప్రతిజ్ఞా(కీలకం);

        ఒకవేళ (!విలువ) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", కీలకం);
                తిరిగి true;
        }

        తిరిగి false;
}
