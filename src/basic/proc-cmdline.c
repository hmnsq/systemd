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

प्रकारव्याख्या गणना ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

पूर्णांक proc_cmdline_filter_pid1_args(अक्षर **argv, अक्षर ***ret) {
        स्थिरांक संरचना {
                स्थिरांक अक्षर *name;
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
        स्थिरांक अक्षर *short_options = "hDbsz:";

        _cleanup_strv_free_ अक्षर **filtered = रिक्त;
        पूर्णांक state, r;

        प्रतिज्ञा(argv);
        प्रतिज्ञा(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        प्रतिज्ञा(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                पूर्णांक prev_state = state;
                स्थिरांक अक्षर *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                जर (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        सुरूठेवा;

                जर (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        जर (r < 0)
                                परत r;
                        सुरूठेवा;
                }

                जर (a[1] == '-') {
                        जर (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                जर (r < 0)
                                        परत r;
                                थांबा;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                स्थिरांक अक्षर *q = startswith(a + 2, option->name);
                                जर (!q || !IN_SET(q[0], '=', '\0'))
                                        सुरूठेवा;

                                /* Found matching option, updating the state if necessary. */
                                जर (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                थांबा;
                        }
                        सुरूठेवा;
                }

                /* short option(s), e.g. -x or -xyz */
                पर्यंत (a && *++a != '\0')
                        साठी (स्थिरांक अक्षर *q = short_options; *q != '\0'; q++) {
                                जर (*q != *a)
                                        सुरूठेवा;

                                /* Found matching short option. */

                                जर (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = रिक्त; /* Not necessary to parse remaining part. */
                                }
                                थांबा;
                        }
        }

        *ret = TAKE_PTR(filtered);
        परत 0;
}

पूर्णांक proc_cmdline(अक्षर **ret) {
        प्रतिज्ञा(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        स्थिरांक अक्षर *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        जर (e)
                परत strdup_to(ret, e);

        जर (detect_container() > 0)
                परत pid_get_cmdline(1, SIZE_MAX, 0, ret);

        परत read_full_file("/proc/cmdline", ret, /* ret_size= */  रिक्त);
}

स्थिर पूर्णांक proc_cmdline_strv_internal(अक्षर ***ret, bool filter_pid1_args) {
        पूर्णांक r;

        प्रतिज्ञा(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        स्थिरांक अक्षर *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        जर (e)
                परत strv_split_full(ret, e, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        जर (detect_container() > 0) {
                _cleanup_strv_free_ अक्षर **args = रिक्त;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                जर (r < 0)
                        परत r;

                जर (filter_pid1_args)
                        परत proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                परत 0;

        } अन्यथा {
                _cleanup_free_ अक्षर *s = रिक्त;

                r = read_full_file("/proc/cmdline", &s, रिक्त);
                जर (r < 0)
                        परत r;

                परत strv_split_full(ret, s, रिक्त, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

पूर्णांक proc_cmdline_strv(अक्षर ***ret) {
        परत proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

स्थिर अक्षर *mangle_word(स्थिरांक अक्षर *word, ProcCmdlineFlags flags) {
        अक्षर *c = (अक्षर*) startswith(word, "rd.");
        जर (c) {
                /* Filter out arguments that are intended only for the initrd */

                जर (!in_initrd())
                        परत रिक्त;

                जर (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        परत c;

        } अन्यथा जर (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                परत रिक्त;

        परत (अक्षर*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

स्थिर पूर्णांक proc_cmdline_parse_strv(अक्षर **args, proc_cmdline_parse_t parse_item, शून्य *data, ProcCmdlineFlags flags) {
        पूर्णांक r;

        प्रतिज्ञा(parse_item);

        STRV_FOREACH(word, args) {
                अक्षर *किल्ली, *मूल्य;

                किल्ली = mangle_word(*word, flags);
                जर (!किल्ली)
                        सुरूठेवा;

                मूल्य = म_अक्षर(किल्ली, '=');
                जर (मूल्य)
                        *(मूल्य++) = '\0';

                r = parse_item(किल्ली, मूल्य, data);
                जर (r < 0)
                        परत r;
        }

        परत 0;
}

पूर्णांक proc_cmdline_parse(proc_cmdline_parse_t parse_item, शून्य *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ अक्षर **args = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        प्रतिज्ञा(!(flags & (PROC_CMDLINE_मूल्य_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        जर (r < 0)
                परत r;

        परत proc_cmdline_parse_strv(args, parse_item, data, flags);
}

स्थिर bool relaxed_equal_char(अक्षर a, अक्षर b) {
        परत a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

अक्षर* proc_cmdline_किल्ली_startswith(स्थिरांक अक्षर *s, स्थिरांक अक्षर *prefix) {
        प्रतिज्ञा(s);
        प्रतिज्ञा(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        साठी (; *prefix != 0; s++, prefix++)
                जर (!relaxed_equal_char(*s, *prefix))
                        परत रिक्त;

        परत (अक्षर*) s;
}

bool proc_cmdline_किल्ली_streq(स्थिरांक अक्षर *x, स्थिरांक अक्षर *y) {
        प्रतिज्ञा(x);
        प्रतिज्ञा(y);

        /* Much like streq(), but considers "-" and "_" the same */

        साठी (; *x != 0 || *y != 0; x++, y++)
                जर (!relaxed_equal_char(*x, *y))
                        परत false;

        परत true;
}

स्थिर पूर्णांक cmdline_get_किल्ली(अक्षर **args, स्थिरांक अक्षर *किल्ली, ProcCmdlineFlags flags, अक्षर **ret_मूल्य) {
        _cleanup_free_ अक्षर *v = रिक्त;
        bool found = false;
        पूर्णांक r;

        प्रतिज्ञा(किल्ली);

        STRV_FOREACH(p, args) {
                स्थिरांक अक्षर *word;

                word = mangle_word(*p, flags);
                जर (!word)
                        सुरूठेवा;

                जर (ret_मूल्य) {
                        स्थिरांक अक्षर *e;

                        e = proc_cmdline_किल्ली_startswith(word, किल्ली);
                        जर (!e)
                                सुरूठेवा;

                        जर (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                जर (r < 0)
                                        परत r;

                                found = true;

                        } अन्यथा जर (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_मूल्य_OPTIONAL))
                                found = true;

                } अन्यथा {
                        जर (proc_cmdline_किल्ली_streq(word, किल्ली)) {
                                found = true;
                                थांबा; /* we found what we were looking for */
                        }
                }
        }

        जर (ret_मूल्य)
                *ret_मूल्य = TAKE_PTR(v);

        परत found;
}

पूर्णांक proc_cmdline_get_किल्ली(स्थिरांक अक्षर *किल्ली, ProcCmdlineFlags flags, अक्षर **ret_मूल्य) {
        _cleanup_strv_free_ अक्षर **args = रिक्त;
        पूर्णांक r;

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

        जर (isempty(किल्ली))
                परत -EINVAL;

        जर (FLAGS_SET(flags, PROC_CMDLINE_मूल्य_OPTIONAL) && !ret_मूल्य)
                परत -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        जर (r < 0)
                परत r;

        परत cmdline_get_किल्ली(args, किल्ली, flags, ret_मूल्य);
}

पूर्णांक proc_cmdline_get_bool(स्थिरांक अक्षर *किल्ली, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ अक्षर *v = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = proc_cmdline_get_किल्ली(किल्ली, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_मूल्य_OPTIONAL, &v);
        जर (r < 0)
                परत r;
        जर (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                परत 0;
        }

        जर (v) { /* key with parameter passed */
                r = parse_boolean(v);
                जर (r < 0)
                        परत r;
                *ret = r;
        } अन्यथा /* key without parameter passed */
                *ret = true;

        परत 1;
}

स्थिर पूर्णांक cmdline_get_किल्ली_ap(ProcCmdlineFlags flags, अक्षर* स्थिरांक* args, बहु_सूची ap) {
        पूर्णांक r, ret = 0;

        साठी (;;) {
                अक्षर **v;
                स्थिरांक अक्षर *k, *e;

                k = बहु_तर्क(ap, स्थिरांक अक्षर*);
                जर (!k)
                        थांबा;

                assert_se(v = बहु_तर्क(ap, अक्षर**));

                STRV_FOREACH(p, args) {
                        स्थिरांक अक्षर *word;

                        word = mangle_word(*p, flags);
                        जर (!word)
                                सुरूठेवा;

                        e = proc_cmdline_किल्ली_startswith(word, k);
                        जर (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                जर (r < 0)
                                        परत r;

                                ret++;
                        }
                }
        }

        परत ret;
}

पूर्णांक proc_cmdline_get_किल्ली_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ अक्षर **args = रिक्त;
        पूर्णांक r;
        बहु_सूची ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        प्रतिज्ञा(!(flags & (PROC_CMDLINE_मूल्य_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        जर (r < 0)
                परत r;

        बहु_शुरू(ap, flags);
        r = cmdline_get_किल्ली_ap(flags, args, ap);
        बहु_पूर्ण(ap);

        परत r;
}

bool proc_cmdline_मूल्य_missing(स्थिरांक अक्षर *किल्ली, स्थिरांक अक्षर *मूल्य) {
        प्रतिज्ञा(किल्ली);

        जर (!मूल्य) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", किल्ली);
                परत true;
        }

        परत false;
}
