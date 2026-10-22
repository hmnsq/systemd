/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "initrd-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "proc-cmdline.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "virt.h"

प्रकारनिर्देश गणना ArgType {
        no_तर्कः,
        required_तर्कः,
        optional_तर्कः,
} ArgType;

पूर्णाङ्क proc_cmdline_filter_pid1_तर्काः(वर्ण **argv, वर्ण ***ret) {
        ध्रुव संरचना {
                ध्रुव वर्ण *name;
                ArgType has_arg;
        } options[] = {
                { "log-level",                required_तर्कः },
                { "log-target",               required_तर्कः },
                { "log-color",                optional_तर्कः },
                { "log-location",             optional_तर्कः },
                { "log-time",                 optional_तर्कः },
                { "unit",                     required_तर्कः },
                { "system",                   no_तर्कः,      },
                { "user",                     no_तर्कः,      },
                { "test",                     no_तर्कः,      },
                { "no-pager",                 no_तर्कः,      },
                { "help",                     no_तर्कः,      },
                { "version",                  no_तर्कः,      },
                { "introspect-cli",           no_तर्कः,      },
                { "dump-configuration-items", no_तर्कः,      },
                { "dump-bus-properties",      no_तर्कः,      },
                { "bus-introspect",           required_तर्कः },
                { "dump-core",                optional_तर्कः },
                { "crash-chvt",               required_तर्कः },
                { "crash-vt",                 required_तर्कः },
                { "crash-shell",              optional_तर्कः },
                { "crash-reboot",             optional_तर्कः },
                { "crash-action",             required_तर्कः },
                { "confirm-spawn",            optional_तर्कः },
                { "show-status",              optional_तर्कः },
                { "deserialize",              required_तर्कः },
                { "switched-root",            no_तर्कः,      },
                { "default-standard-output",  required_तर्कः },
                { "default-standard-error",   required_तर्कः },
                { "machine-id",               required_तर्कः },
                { "service-watchdogs",        required_तर्कः },
                { "exit-code",                required_तर्कः },
                { "timeout",                  required_तर्कः },
        };
        ध्रुव वर्ण *short_options = "hDbsz:";

        _cleanup_strv_free_ वर्ण **filtered = रिक्त;
        पूर्णाङ्क state, r;

        प्रतिज्ञा(argv);
        प्रतिज्ञा(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        प्रतिज्ञा(!अन्तर्_समुच्चयः(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_तर्कः;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                पूर्णाङ्क पूर्व_state = state;
                ध्रुव वर्ण *a = *p;

                /* Reset the state for the next step. */
                state = no_तर्कः;

                यदि (पूर्व_state == required_तर्कः ||
                    (पूर्व_state == optional_तर्कः && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        अनुवर्त;

                यदि (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        यदि (r < 0)
                                प्रत्यागम r;
                        अनुवर्त;
                }

                यदि (a[1] == '-') {
                        यदि (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                यदि (r < 0)
                                        प्रत्यागम r;
                                भङ्ग;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ध्रुव वर्ण *q = startswith(a + 2, option->name);
                                यदि (!q || !अन्तर्_समुच्चयः(q[0], '=', '\0'))
                                        अनुवर्त;

                                /* Found matching option, updating the state if necessary. */
                                यदि (q[0] == '\0' && option->has_arg == required_तर्कः)
                                        state = required_तर्कः;

                                भङ्ग;
                        }
                        अनुवर्त;
                }

                /* short option(s), e.g. -x or -xyz */
                यावत् (a && *++a != '\0')
                        प्रति (ध्रुव वर्ण *q = short_options; *q != '\0'; q++) {
                                यदि (*q != *a)
                                        अनुवर्त;

                                /* Found matching short option. */

                                यदि (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_तर्कः :
                                                q[2] == ':' ? optional_तर्कः : required_तर्कः;

                                        a = रिक्त; /* Not necessary to parse remaining part. */
                                }
                                भङ्ग;
                        }
        }

        *ret = TAKE_PTR(filtered);
        प्रत्यागम 0;
}

पूर्णाङ्क proc_cmdline(वर्ण **ret) {
        प्रतिज्ञा(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ध्रुव वर्ण *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        यदि (e)
                प्रत्यागम strdup_गन्तव्यम्(ret, e);

        यदि (detect_container() > 0)
                प्रत्यागम pid_get_cmdline(1, SIZE_MAX, 0, ret);

        प्रत्यागम read_पूर्णम्_file("/proc/cmdline", ret, /* ret_size= */  रिक्त);
}

स्थिर पूर्णाङ्क proc_cmdline_strv_आन्तरिकम्(वर्ण ***ret, bool filter_pid1_तर्काः) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ध्रुव वर्ण *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        यदि (e)
                प्रत्यागम strv_विभज_पूर्णम्(ret, e, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        यदि (detect_container() > 0) {
                _cleanup_strv_free_ वर्ण **तर्काः = रिक्त;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &तर्काः);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (filter_pid1_तर्काः)
                        प्रत्यागम proc_cmdline_filter_pid1_तर्काः(तर्काः, ret);

                *ret = TAKE_PTR(तर्काः);
                प्रत्यागम 0;

        } अन्यथा {
                _cleanup_free_ वर्ण *s = रिक्त;

                r = read_पूर्णम्_file("/proc/cmdline", &s, रिक्त);
                यदि (r < 0)
                        प्रत्यागम r;

                प्रत्यागम strv_विभज_पूर्णम्(ret, s, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

पूर्णाङ्क proc_cmdline_strv(वर्ण ***ret) {
        प्रत्यागम proc_cmdline_strv_आन्तरिकम्(ret, /* filter_pid1_args= */ false);
}

स्थिर वर्ण *mangle_word(ध्रुव वर्ण *word, ProcCmdlineFlags flags) {
        वर्ण *c = (वर्ण*) startswith(word, "rd.");
        यदि (c) {
                /* Filter out arguments that are intended only for the initrd */

                यदि (!अन्तर्_initrd())
                        प्रत्यागम रिक्त;

                यदि (FLAGS_समुच्चयः(flags, PROC_CMDLINE_STRIP_RD_उपसर्गः))
                        प्रत्यागम c;

        } अन्यथा यदि (FLAGS_समुच्चयः(flags, PROC_CMDLINE_RD_STRICT) && अन्तर्_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                प्रत्यागम रिक्त;

        प्रत्यागम (वर्ण*) word;
}

#परिभाषय mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

स्थिर पूर्णाङ्क proc_cmdline_parse_strv(वर्ण **तर्काः, proc_cmdline_parse_t parse_item, शून्य *दत्त, ProcCmdlineFlags flags) {
        पूर्णाङ्क r;

        प्रतिज्ञा(parse_item);

        STRV_FOREACH(word, तर्काः) {
                वर्ण *कुञ्चिका, *मानम्;

                कुञ्चिका = mangle_word(*word, flags);
                यदि (!कुञ्चिका)
                        अनुवर्त;

                मानम् = strchr(कुञ्चिका, '=');
                यदि (मानम्)
                        *(मानम्++) = '\0';

                r = parse_item(कुञ्चिका, मानम्, दत्त);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क proc_cmdline_parse(proc_cmdline_parse_t parse_item, शून्य *दत्त, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ वर्ण **तर्काः = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        प्रतिज्ञा(!(flags & (PROC_CMDLINE_मानम्_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_आन्तरिकम्(&तर्काः, /* filter_pid1_args= */ true);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम proc_cmdline_parse_strv(तर्काः, parse_item, दत्त, flags);
}

स्थिर bool relaxed_equal_char(वर्ण a, वर्ण b) {
        प्रत्यागम a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

वर्ण* proc_cmdline_कुञ्चिका_startswith(ध्रुव वर्ण *s, ध्रुव वर्ण *उपसर्गः) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(उपसर्गः);

        /* Much like startswith(), but considers "-" and "_" the same */

        प्रति (; *उपसर्गः != 0; s++, उपसर्गः++)
                यदि (!relaxed_equal_char(*s, *उपसर्गः))
                        प्रत्यागम रिक्त;

        प्रत्यागम (वर्ण*) s;
}

bool proc_cmdline_कुञ्चिका_streq(ध्रुव वर्ण *x, ध्रुव वर्ण *y) {
        प्रतिज्ञा(x);
        प्रतिज्ञा(y);

        /* Much like streq(), but considers "-" and "_" the same */

        प्रति (; *x != 0 || *y != 0; x++, y++)
                यदि (!relaxed_equal_char(*x, *y))
                        प्रत्यागम false;

        प्रत्यागम true;
}

स्थिर पूर्णाङ्क cmdline_get_कुञ्चिका(वर्ण **तर्काः, ध्रुव वर्ण *कुञ्चिका, ProcCmdlineFlags flags, वर्ण **ret_मानम्) {
        _cleanup_free_ वर्ण *v = रिक्त;
        bool प्राप्तम् = false;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुञ्चिका);

        STRV_FOREACH(p, तर्काः) {
                ध्रुव वर्ण *word;

                word = mangle_word(*p, flags);
                यदि (!word)
                        अनुवर्त;

                यदि (ret_मानम्) {
                        ध्रुव वर्ण *e;

                        e = proc_cmdline_कुञ्चिका_startswith(word, कुञ्चिका);
                        यदि (!e)
                                अनुवर्त;

                        यदि (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                प्राप्तम् = true;

                        } अन्यथा यदि (*e == 0 && FLAGS_समुच्चयः(flags, PROC_CMDLINE_मानम्_OPTIONAL))
                                प्राप्तम् = true;

                } अन्यथा {
                        यदि (proc_cmdline_कुञ्चिका_streq(word, कुञ्चिका)) {
                                प्राप्तम् = true;
                                भङ्ग; /* we found what we were looking for */
                        }
                }
        }

        यदि (ret_मानम्)
                *ret_मानम् = TAKE_PTR(v);

        प्रत्यागम प्राप्तम्;
}

पूर्णाङ्क proc_cmdline_get_कुञ्चिका(ध्रुव वर्ण *कुञ्चिका, ProcCmdlineFlags flags, वर्ण **ret_मानम्) {
        _cleanup_strv_free_ वर्ण **तर्काः = रिक्त;
        पूर्णाङ्क r;

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
        प्रतिज्ञा(!FLAGS_समुच्चयः(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        यदि (isempty(कुञ्चिका))
                प्रत्यागम -EINVAL;

        यदि (FLAGS_समुच्चयः(flags, PROC_CMDLINE_मानम्_OPTIONAL) && !ret_मानम्)
                प्रत्यागम -EINVAL;

        r = proc_cmdline_strv_आन्तरिकम्(&तर्काः, /* filter_pid1_args= */ true);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम cmdline_get_कुञ्चिका(तर्काः, कुञ्चिका, flags, ret_मानम्);
}

पूर्णाङ्क proc_cmdline_get_bool(ध्रुव वर्ण *कुञ्चिका, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ वर्ण *v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = proc_cmdline_get_कुञ्चिका(कुञ्चिका, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_मानम्_OPTIONAL, &v);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0) { /* key not specified at all */
                *ret = FLAGS_समुच्चयः(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                प्रत्यागम 0;
        }

        यदि (v) { /* key with parameter passed */
                r = parse_boolean(v);
                यदि (r < 0)
                        प्रत्यागम r;
                *ret = r;
        } अन्यथा /* key without parameter passed */
                *ret = true;

        प्रत्यागम 1;
}

स्थिर पूर्णाङ्क cmdline_get_कुञ्चिका_ap(ProcCmdlineFlags flags, वर्ण* ध्रुव* तर्काः, va_list ap) {
        पूर्णाङ्क r, ret = 0;

        प्रति (;;) {
                वर्ण **v;
                ध्रुव वर्ण *k, *e;

                k = va_arg(ap, ध्रुव वर्ण*);
                यदि (!k)
                        भङ्ग;

                assert_se(v = va_arg(ap, वर्ण**));

                STRV_FOREACH(p, तर्काः) {
                        ध्रुव वर्ण *word;

                        word = mangle_word(*p, flags);
                        यदि (!word)
                                अनुवर्त;

                        e = proc_cmdline_कुञ्चिका_startswith(word, k);
                        यदि (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                ret++;
                        }
                }
        }

        प्रत्यागम ret;
}

पूर्णाङ्क proc_cmdline_get_कुञ्चिका_many_आन्तरिकम्(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ वर्ण **तर्काः = रिक्त;
        पूर्णाङ्क r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        प्रतिज्ञा(!(flags & (PROC_CMDLINE_मानम्_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&तर्काः);
        यदि (r < 0)
                प्रत्यागम r;

        va_start(ap, flags);
        r = cmdline_get_कुञ्चिका_ap(flags, तर्काः, ap);
        va_end(ap);

        प्रत्यागम r;
}

bool proc_cmdline_मानम्_missing(ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्) {
        प्रतिज्ञा(कुञ्चिका);

        यदि (!मानम्) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", कुञ्चिका);
                प्रत्यागम true;
        }

        प्रत्यागम false;
}
