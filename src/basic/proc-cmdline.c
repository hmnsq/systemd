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
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

पूर्णाङ्क proc_cmdline_filter_pid1_args(वर्ण **argv, वर्ण ***ret) {
        ध्रुव संरचना {
                ध्रुव वर्ण *name;
                ArgType has_arg;
        } options[] = {
                { "log-level",                required_argument },
                { "log-target",               required_argument },
                { "log-color",                optional_argument },
                { "log-location",             optional_argument },
                { "log-time",                 optional_argument },
                { "unit",                     required_argument },
                { "system",                   no_argument,      },
                { "user",                     no_argument,      },
                { "test",                     no_argument,      },
                { "no-pager",                 no_argument,      },
                { "help",                     no_argument,      },
                { "version",                  no_argument,      },
                { "introspect-cli",           no_argument,      },
                { "dump-configuration-items", no_argument,      },
                { "dump-bus-properties",      no_argument,      },
                { "bus-introspect",           required_argument },
                { "dump-core",                optional_argument },
                { "crash-chvt",               required_argument },
                { "crash-vt",                 required_argument },
                { "crash-shell",              optional_argument },
                { "crash-reboot",             optional_argument },
                { "crash-action",             required_argument },
                { "confirm-spawn",            optional_argument },
                { "show-status",              optional_argument },
                { "deserialize",              required_argument },
                { "switched-root",            no_argument,      },
                { "default-standard-output",  required_argument },
                { "default-standard-error",   required_argument },
                { "machine-id",               required_argument },
                { "service-watchdogs",        required_argument },
                { "exit-code",                required_argument },
                { "timeout",                  required_argument },
        };
        ध्रुव वर्ण *short_options = "hDbsz:";

        _cleanup_strv_free_ वर्ण **filtered = रिक्त;
        पूर्णाङ्क state, r;

        प्रतिज्ञा(argv);
        प्रतिज्ञा(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        प्रतिज्ञा(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                पूर्णाङ्क prev_state = state;
                ध्रुव वर्ण *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                सचे (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        अनुवर्त;

                सचे (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        सचे (r < 0)
                                पटिनिवत्त r;
                        अनुवर्त;
                }

                सचे (a[1] == '-') {
                        सचे (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                                भङ्ग;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ध्रुव वर्ण *q = startswith(a + 2, option->name);
                                सचे (!q || !IN_SET(q[0], '=', '\0'))
                                        अनुवर्त;

                                /* Found matching option, updating the state if necessary. */
                                सचे (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                भङ्ग;
                        }
                        अनुवर्त;
                }

                /* short option(s), e.g. -x or -xyz */
                याव (a && *++a != '\0')
                        अत्थाय (ध्रुव वर्ण *q = short_options; *q != '\0'; q++) {
                                सचे (*q != *a)
                                        अनुवर्त;

                                /* Found matching short option. */

                                सचे (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = रिक्त; /* Not necessary to parse remaining part. */
                                }
                                भङ्ग;
                        }
        }

        *ret = TAKE_PTR(filtered);
        पटिनिवत्त 0;
}

पूर्णाङ्क proc_cmdline(वर्ण **ret) {
        प्रतिज्ञा(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ध्रुव वर्ण *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        सचे (e)
                पटिनिवत्त strdup_to(ret, e);

        सचे (detect_container() > 0)
                पटिनिवत्त pid_get_cmdline(1, SIZE_MAX, 0, ret);

        पटिनिवत्त read_full_file("/proc/cmdline", ret, /* ret_size= */  रिक्त);
}

स्थिर पूर्णाङ्क proc_cmdline_strv_internal(वर्ण ***ret, bool filter_pid1_args) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ध्रुव वर्ण *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        सचे (e)
                पटिनिवत्त strv_split_full(ret, e, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        सचे (detect_container() > 0) {
                _cleanup_strv_free_ वर्ण **args = रिक्त;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (filter_pid1_args)
                        पटिनिवत्त proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                पटिनिवत्त 0;

        } अञ्ञथा {
                _cleanup_free_ वर्ण *s = रिक्त;

                r = read_full_file("/proc/cmdline", &s, रिक्त);
                सचे (r < 0)
                        पटिनिवत्त r;

                पटिनिवत्त strv_split_full(ret, s, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

पूर्णाङ्क proc_cmdline_strv(वर्ण ***ret) {
        पटिनिवत्त proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

स्थिर वर्ण *mangle_word(ध्रुव वर्ण *word, ProcCmdlineFlags flags) {
        वर्ण *c = (वर्ण*) startswith(word, "rd.");
        सचे (c) {
                /* Filter out arguments that are intended only for the initrd */

                सचे (!in_initrd())
                        पटिनिवत्त रिक्त;

                सचे (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        पटिनिवत्त c;

        } अञ्ञथा सचे (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                पटिनिवत्त रिक्त;

        पटिनिवत्त (वर्ण*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

स्थिर पूर्णाङ्क proc_cmdline_parse_strv(वर्ण **args, proc_cmdline_parse_t parse_item, शून्य *data, ProcCmdlineFlags flags) {
        पूर्णाङ्क r;

        प्रतिज्ञा(parse_item);

        STRV_FOREACH(word, args) {
                वर्ण *कुञ्चिका, *मानम्;

                कुञ्चिका = mangle_word(*word, flags);
                सचे (!कुञ्चिका)
                        अनुवर्त;

                मानम् = strchr(कुञ्चिका, '=');
                सचे (मानम्)
                        *(मानम्++) = '\0';

                r = parse_item(कुञ्चिका, मानम्, data);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क proc_cmdline_parse(proc_cmdline_parse_t parse_item, शून्य *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ वर्ण **args = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        प्रतिज्ञा(!(flags & (PROC_CMDLINE_मानम्_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त proc_cmdline_parse_strv(args, parse_item, data, flags);
}

स्थिर bool relaxed_equal_char(वर्ण a, वर्ण b) {
        पटिनिवत्त a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

वर्ण* proc_cmdline_कुञ्चिका_startswith(ध्रुव वर्ण *s, ध्रुव वर्ण *prefix) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        अत्थाय (; *prefix != 0; s++, prefix++)
                सचे (!relaxed_equal_char(*s, *prefix))
                        पटिनिवत्त रिक्त;

        पटिनिवत्त (वर्ण*) s;
}

bool proc_cmdline_कुञ्चिका_streq(ध्रुव वर्ण *x, ध्रुव वर्ण *y) {
        प्रतिज्ञा(x);
        प्रतिज्ञा(y);

        /* Much like streq(), but considers "-" and "_" the same */

        अत्थाय (; *x != 0 || *y != 0; x++, y++)
                सचे (!relaxed_equal_char(*x, *y))
                        पटिनिवत्त false;

        पटिनिवत्त true;
}

स्थिर पूर्णाङ्क cmdline_get_कुञ्चिका(वर्ण **args, ध्रुव वर्ण *कुञ्चिका, ProcCmdlineFlags flags, वर्ण **ret_मानम्) {
        _cleanup_free_ वर्ण *v = रिक्त;
        bool found = false;
        पूर्णाङ्क r;

        प्रतिज्ञा(कुञ्चिका);

        STRV_FOREACH(p, args) {
                ध्रुव वर्ण *word;

                word = mangle_word(*p, flags);
                सचे (!word)
                        अनुवर्त;

                सचे (ret_मानम्) {
                        ध्रुव वर्ण *e;

                        e = proc_cmdline_कुञ्चिका_startswith(word, कुञ्चिका);
                        सचे (!e)
                                अनुवर्त;

                        सचे (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                सचे (r < 0)
                                        पटिनिवत्त r;

                                found = true;

                        } अञ्ञथा सचे (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_मानम्_OPTIONAL))
                                found = true;

                } अञ्ञथा {
                        सचे (proc_cmdline_कुञ्चिका_streq(word, कुञ्चिका)) {
                                found = true;
                                भङ्ग; /* we found what we were looking for */
                        }
                }
        }

        सचे (ret_मानम्)
                *ret_मानम् = TAKE_PTR(v);

        पटिनिवत्त found;
}

पूर्णाङ्क proc_cmdline_get_कुञ्चिका(ध्रुव वर्ण *कुञ्चिका, ProcCmdlineFlags flags, वर्ण **ret_मानम्) {
        _cleanup_strv_free_ वर्ण **args = रिक्त;
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
        प्रतिज्ञा(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        सचे (isempty(कुञ्चिका))
                पटिनिवत्त -EINVAL;

        सचे (FLAGS_SET(flags, PROC_CMDLINE_मानम्_OPTIONAL) && !ret_मानम्)
                पटिनिवत्त -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त cmdline_get_कुञ्चिका(args, कुञ्चिका, flags, ret_मानम्);
}

पूर्णाङ्क proc_cmdline_get_bool(ध्रुव वर्ण *कुञ्चिका, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ वर्ण *v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = proc_cmdline_get_कुञ्चिका(कुञ्चिका, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_मानम्_OPTIONAL, &v);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                पटिनिवत्त 0;
        }

        सचे (v) { /* key with parameter passed */
                r = parse_boolean(v);
                सचे (r < 0)
                        पटिनिवत्त r;
                *ret = r;
        } अञ्ञथा /* key without parameter passed */
                *ret = true;

        पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क cmdline_get_कुञ्चिका_ap(ProcCmdlineFlags flags, वर्ण* ध्रुव* args, va_list ap) {
        पूर्णाङ्क r, ret = 0;

        अत्थाय (;;) {
                वर्ण **v;
                ध्रुव वर्ण *k, *e;

                k = va_arg(ap, ध्रुव वर्ण*);
                सचे (!k)
                        भङ्ग;

                assert_se(v = va_arg(ap, वर्ण**));

                STRV_FOREACH(p, args) {
                        ध्रुव वर्ण *word;

                        word = mangle_word(*p, flags);
                        सचे (!word)
                                अनुवर्त;

                        e = proc_cmdline_कुञ्चिका_startswith(word, k);
                        सचे (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                सचे (r < 0)
                                        पटिनिवत्त r;

                                ret++;
                        }
                }
        }

        पटिनिवत्त ret;
}

पूर्णाङ्क proc_cmdline_get_कुञ्चिका_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ वर्ण **args = रिक्त;
        पूर्णाङ्क r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        प्रतिज्ञा(!(flags & (PROC_CMDLINE_मानम्_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        सचे (r < 0)
                पटिनिवत्त r;

        va_start(ap, flags);
        r = cmdline_get_कुञ्चिका_ap(flags, args, ap);
        va_end(ap);

        पटिनिवत्त r;
}

bool proc_cmdline_मानम्_missing(ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्) {
        प्रतिज्ञा(कुञ्चिका);

        सचे (!मानम्) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", कुञ्चिका);
                पटिनिवत्त true;
        }

        पटिनिवत्त false;
}
