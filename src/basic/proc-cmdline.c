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
        अचर संरचना {
                अचर वर्ण *name;
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
        अचर वर्ण *short_options = "hDbsz:";

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
                अचर वर्ण *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                यदि (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        जारी;

                यदि (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        यदि (r < 0)
                                फर्काऊ r;
                        जारी;
                }

                यदि (a[1] == '-') {
                        यदि (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                यदि (r < 0)
                                        फर्काऊ r;
                                रोक;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                अचर वर्ण *q = startswith(a + 2, option->name);
                                यदि (!q || !IN_SET(q[0], '=', '\0'))
                                        जारी;

                                /* Found matching option, updating the state if necessary. */
                                यदि (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                रोक;
                        }
                        जारी;
                }

                /* short option(s), e.g. -x or -xyz */
                जबसम्म (a && *++a != '\0')
                        लागि (अचर वर्ण *q = short_options; *q != '\0'; q++) {
                                यदि (*q != *a)
                                        जारी;

                                /* Found matching short option. */

                                यदि (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = रिक्त; /* Not necessary to parse remaining part. */
                                }
                                रोक;
                        }
        }

        *ret = TAKE_PTR(filtered);
        फर्काऊ 0;
}

पूर्णाङ्क proc_cmdline(वर्ण **ret) {
        प्रतिज्ञा(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        अचर वर्ण *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        यदि (e)
                फर्काऊ strdup_to(ret, e);

        यदि (detect_container() > 0)
                फर्काऊ pid_get_cmdline(1, SIZE_MAX, 0, ret);

        फर्काऊ read_full_file("/proc/cmdline", ret, /* ret_size= */  रिक्त);
}

स्थिर पूर्णाङ्क proc_cmdline_strv_internal(वर्ण ***ret, bool filter_pid1_args) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        अचर वर्ण *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        यदि (e)
                फर्काऊ strv_split_full(ret, e, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        यदि (detect_container() > 0) {
                _cleanup_strv_free_ वर्ण **args = रिक्त;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (filter_pid1_args)
                        फर्काऊ proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                फर्काऊ 0;

        } अन्यथा {
                _cleanup_free_ वर्ण *s = रिक्त;

                r = read_full_file("/proc/cmdline", &s, रिक्त);
                यदि (r < 0)
                        फर्काऊ r;

                फर्काऊ strv_split_full(ret, s, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

पूर्णाङ्क proc_cmdline_strv(वर्ण ***ret) {
        फर्काऊ proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

स्थिर वर्ण *mangle_word(अचर वर्ण *word, ProcCmdlineFlags flags) {
        वर्ण *c = (वर्ण*) startswith(word, "rd.");
        यदि (c) {
                /* Filter out arguments that are intended only for the initrd */

                यदि (!in_initrd())
                        फर्काऊ रिक्त;

                यदि (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        फर्काऊ c;

        } अन्यथा यदि (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                फर्काऊ रिक्त;

        फर्काऊ (वर्ण*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

स्थिर पूर्णाङ्क proc_cmdline_parse_strv(वर्ण **args, proc_cmdline_parse_t parse_item, शून्य *data, ProcCmdlineFlags flags) {
        पूर्णाङ्क r;

        प्रतिज्ञा(parse_item);

        STRV_FOREACH(word, args) {
                वर्ण *साँचो, *मान;

                साँचो = mangle_word(*word, flags);
                यदि (!साँचो)
                        जारी;

                मान = म_अक्षर(साँचो, '=');
                यदि (मान)
                        *(मान++) = '\0';

                r = parse_item(साँचो, मान, data);
                यदि (r < 0)
                        फर्काऊ r;
        }

        फर्काऊ 0;
}

पूर्णाङ्क proc_cmdline_parse(proc_cmdline_parse_t parse_item, शून्य *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ वर्ण **args = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        प्रतिज्ञा(!(flags & (PROC_CMDLINE_मान_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ proc_cmdline_parse_strv(args, parse_item, data, flags);
}

स्थिर bool relaxed_equal_char(वर्ण a, वर्ण b) {
        फर्काऊ a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

वर्ण* proc_cmdline_साँचो_startswith(अचर वर्ण *s, अचर वर्ण *prefix) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        लागि (; *prefix != 0; s++, prefix++)
                यदि (!relaxed_equal_char(*s, *prefix))
                        फर्काऊ रिक्त;

        फर्काऊ (वर्ण*) s;
}

bool proc_cmdline_साँचो_streq(अचर वर्ण *x, अचर वर्ण *y) {
        प्रतिज्ञा(x);
        प्रतिज्ञा(y);

        /* Much like streq(), but considers "-" and "_" the same */

        लागि (; *x != 0 || *y != 0; x++, y++)
                यदि (!relaxed_equal_char(*x, *y))
                        फर्काऊ false;

        फर्काऊ true;
}

स्थिर पूर्णाङ्क cmdline_get_साँचो(वर्ण **args, अचर वर्ण *साँचो, ProcCmdlineFlags flags, वर्ण **ret_मान) {
        _cleanup_free_ वर्ण *v = रिक्त;
        bool found = false;
        पूर्णाङ्क r;

        प्रतिज्ञा(साँचो);

        STRV_FOREACH(p, args) {
                अचर वर्ण *word;

                word = mangle_word(*p, flags);
                यदि (!word)
                        जारी;

                यदि (ret_मान) {
                        अचर वर्ण *e;

                        e = proc_cmdline_साँचो_startswith(word, साँचो);
                        यदि (!e)
                                जारी;

                        यदि (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                यदि (r < 0)
                                        फर्काऊ r;

                                found = true;

                        } अन्यथा यदि (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_मान_OPTIONAL))
                                found = true;

                } अन्यथा {
                        यदि (proc_cmdline_साँचो_streq(word, साँचो)) {
                                found = true;
                                रोक; /* we found what we were looking for */
                        }
                }
        }

        यदि (ret_मान)
                *ret_मान = TAKE_PTR(v);

        फर्काऊ found;
}

पूर्णाङ्क proc_cmdline_get_साँचो(अचर वर्ण *साँचो, ProcCmdlineFlags flags, वर्ण **ret_मान) {
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

        यदि (isempty(साँचो))
                फर्काऊ -EINVAL;

        यदि (FLAGS_SET(flags, PROC_CMDLINE_मान_OPTIONAL) && !ret_मान)
                फर्काऊ -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ cmdline_get_साँचो(args, साँचो, flags, ret_मान);
}

पूर्णाङ्क proc_cmdline_get_bool(अचर वर्ण *साँचो, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ वर्ण *v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = proc_cmdline_get_साँचो(साँचो, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_मान_OPTIONAL, &v);
        यदि (r < 0)
                फर्काऊ r;
        यदि (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                फर्काऊ 0;
        }

        यदि (v) { /* key with parameter passed */
                r = parse_boolean(v);
                यदि (r < 0)
                        फर्काऊ r;
                *ret = r;
        } अन्यथा /* key without parameter passed */
                *ret = true;

        फर्काऊ 1;
}

स्थिर पूर्णाङ्क cmdline_get_साँचो_ap(ProcCmdlineFlags flags, वर्ण* अचर* args, बहु_सूची ap) {
        पूर्णाङ्क r, ret = 0;

        लागि (;;) {
                वर्ण **v;
                अचर वर्ण *k, *e;

                k = बहु_तर्क(ap, अचर वर्ण*);
                यदि (!k)
                        रोक;

                assert_se(v = बहु_तर्क(ap, वर्ण**));

                STRV_FOREACH(p, args) {
                        अचर वर्ण *word;

                        word = mangle_word(*p, flags);
                        यदि (!word)
                                जारी;

                        e = proc_cmdline_साँचो_startswith(word, k);
                        यदि (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                यदि (r < 0)
                                        फर्काऊ r;

                                ret++;
                        }
                }
        }

        फर्काऊ ret;
}

पूर्णाङ्क proc_cmdline_get_साँचो_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ वर्ण **args = रिक्त;
        पूर्णाङ्क r;
        बहु_सूची ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        प्रतिज्ञा(!(flags & (PROC_CMDLINE_मान_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        यदि (r < 0)
                फर्काऊ r;

        बहु_शुरू(ap, flags);
        r = cmdline_get_साँचो_ap(flags, args, ap);
        बहु_पूर्ण(ap);

        फर्काऊ r;
}

bool proc_cmdline_मान_missing(अचर वर्ण *साँचो, अचर वर्ण *मान) {
        प्रतिज्ञा(साँचो);

        यदि (!मान) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", साँचो);
                फर्काऊ true;
        }

        फर्काऊ false;
}
