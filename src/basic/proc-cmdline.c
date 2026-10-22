/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdlib.h>

#समावेश "alloc-util.h"
#समावेश "extract-word.h"
#समावेश "fileio.h"
#समावेश "initrd-util.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "proc-cmdline.h"
#समावेश "process-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "virt.h"

प्रकार क्रमागत ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

पूर्णांक proc_cmdline_filter_pid1_args(अक्षर **argv, अक्षर ***ret) {
        स्थिर काष्ठा {
                स्थिर अक्षर *name;
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
        स्थिर अक्षर *short_options = "hDbsz:";

        _cleanup_strv_free_ अक्षर **filtered = शून्य;
        पूर्णांक state, r;

        निश्चित(argv);
        निश्चित(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        निश्चित(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                पूर्णांक पूर्व_state = state;
                स्थिर अक्षर *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                अगर (पूर्व_state == required_argument ||
                    (पूर्व_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        जारी;

                अगर (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        अगर (r < 0)
                                वापस r;
                        जारी;
                }

                अगर (a[1] == '-') {
                        अगर (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                अगर (r < 0)
                                        वापस r;
                                अवरोध;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                स्थिर अक्षर *q = startswith(a + 2, option->name);
                                अगर (!q || !IN_SET(q[0], '=', '\0'))
                                        जारी;

                                /* Found matching option, updating the state if necessary. */
                                अगर (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                अवरोध;
                        }
                        जारी;
                }

                /* short option(s), e.g. -x or -xyz */
                जबतक (a && *++a != '\0')
                        क्रम (स्थिर अक्षर *q = short_options; *q != '\0'; q++) {
                                अगर (*q != *a)
                                        जारी;

                                /* Found matching short option. */

                                अगर (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = शून्य; /* Not necessary to parse remaining part. */
                                }
                                अवरोध;
                        }
        }

        *ret = TAKE_PTR(filtered);
        वापस 0;
}

पूर्णांक proc_cmdline(अक्षर **ret) {
        निश्चित(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        स्थिर अक्षर *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        अगर (e)
                वापस strdup_to(ret, e);

        अगर (detect_container() > 0)
                वापस pid_get_cmdline(1, SIZE_MAX, 0, ret);

        वापस read_full_file("/proc/cmdline", ret, /* ret_size= */  शून्य);
}

अटल पूर्णांक proc_cmdline_strv_internal(अक्षर ***ret, bool filter_pid1_args) {
        पूर्णांक r;

        निश्चित(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        स्थिर अक्षर *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        अगर (e)
                वापस strv_split_full(ret, e, शून्य, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        अगर (detect_container() > 0) {
                _cleanup_strv_free_ अक्षर **args = शून्य;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                अगर (r < 0)
                        वापस r;

                अगर (filter_pid1_args)
                        वापस proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                वापस 0;

        } अन्यथा {
                _cleanup_free_ अक्षर *s = शून्य;

                r = read_full_file("/proc/cmdline", &s, शून्य);
                अगर (r < 0)
                        वापस r;

                वापस strv_split_full(ret, s, शून्य, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

पूर्णांक proc_cmdline_strv(अक्षर ***ret) {
        वापस proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

अटल अक्षर *mangle_word(स्थिर अक्षर *word, ProcCmdlineFlags flags) {
        अक्षर *c = (अक्षर*) startswith(word, "rd.");
        अगर (c) {
                /* Filter out arguments that are intended only for the initrd */

                अगर (!in_initrd())
                        वापस शून्य;

                अगर (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        वापस c;

        } अन्यथा अगर (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                वापस शून्य;

        वापस (अक्षर*) word;
}

#घोषणा mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

अटल पूर्णांक proc_cmdline_parse_strv(अक्षर **args, proc_cmdline_parse_t parse_item, व्योम *तथ्य, ProcCmdlineFlags flags) {
        पूर्णांक r;

        निश्चित(parse_item);

        STRV_FOREACH(word, args) {
                अक्षर *कुंजी, *मूल्य;

                कुंजी = mangle_word(*word, flags);
                अगर (!कुंजी)
                        जारी;

                मूल्य = म_अक्षर(कुंजी, '=');
                अगर (मूल्य)
                        *(मूल्य++) = '\0';

                r = parse_item(कुंजी, मूल्य, तथ्य);
                अगर (r < 0)
                        वापस r;
        }

        वापस 0;
}

पूर्णांक proc_cmdline_parse(proc_cmdline_parse_t parse_item, व्योम *तथ्य, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ अक्षर **args = शून्य;
        पूर्णांक r;

        निश्चित(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        निश्चित(!(flags & (PROC_CMDLINE_मूल्य_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        अगर (r < 0)
                वापस r;

        वापस proc_cmdline_parse_strv(args, parse_item, तथ्य, flags);
}

अटल bool relaxed_equal_char(अक्षर a, अक्षर b) {
        वापस a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

अक्षर* proc_cmdline_कुंजी_startswith(स्थिर अक्षर *s, स्थिर अक्षर *prefix) {
        निश्चित(s);
        निश्चित(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        क्रम (; *prefix != 0; s++, prefix++)
                अगर (!relaxed_equal_char(*s, *prefix))
                        वापस शून्य;

        वापस (अक्षर*) s;
}

bool proc_cmdline_कुंजी_streq(स्थिर अक्षर *x, स्थिर अक्षर *y) {
        निश्चित(x);
        निश्चित(y);

        /* Much like streq(), but considers "-" and "_" the same */

        क्रम (; *x != 0 || *y != 0; x++, y++)
                अगर (!relaxed_equal_char(*x, *y))
                        वापस false;

        वापस true;
}

अटल पूर्णांक cmdline_get_कुंजी(अक्षर **args, स्थिर अक्षर *कुंजी, ProcCmdlineFlags flags, अक्षर **ret_मूल्य) {
        _cleanup_free_ अक्षर *v = शून्य;
        bool found = false;
        पूर्णांक r;

        निश्चित(कुंजी);

        STRV_FOREACH(p, args) {
                स्थिर अक्षर *word;

                word = mangle_word(*p, flags);
                अगर (!word)
                        जारी;

                अगर (ret_मूल्य) {
                        स्थिर अक्षर *e;

                        e = proc_cmdline_कुंजी_startswith(word, कुंजी);
                        अगर (!e)
                                जारी;

                        अगर (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                अगर (r < 0)
                                        वापस r;

                                found = true;

                        } अन्यथा अगर (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_मूल्य_OPTIONAL))
                                found = true;

                } अन्यथा {
                        अगर (proc_cmdline_कुंजी_streq(word, कुंजी)) {
                                found = true;
                                अवरोध; /* we found what we were looking for */
                        }
                }
        }

        अगर (ret_मूल्य)
                *ret_मूल्य = TAKE_PTR(v);

        वापस found;
}

पूर्णांक proc_cmdline_get_कुंजी(स्थिर अक्षर *कुंजी, ProcCmdlineFlags flags, अक्षर **ret_मूल्य) {
        _cleanup_strv_free_ अक्षर **args = शून्य;
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
        निश्चित(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        अगर (isempty(कुंजी))
                वापस -EINVAL;

        अगर (FLAGS_SET(flags, PROC_CMDLINE_मूल्य_OPTIONAL) && !ret_मूल्य)
                वापस -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        अगर (r < 0)
                वापस r;

        वापस cmdline_get_कुंजी(args, कुंजी, flags, ret_मूल्य);
}

पूर्णांक proc_cmdline_get_bool(स्थिर अक्षर *कुंजी, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ अक्षर *v = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = proc_cmdline_get_कुंजी(कुंजी, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_मूल्य_OPTIONAL, &v);
        अगर (r < 0)
                वापस r;
        अगर (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                वापस 0;
        }

        अगर (v) { /* key with parameter passed */
                r = parse_boolean(v);
                अगर (r < 0)
                        वापस r;
                *ret = r;
        } अन्यथा /* key without parameter passed */
                *ret = true;

        वापस 1;
}

अटल पूर्णांक cmdline_get_कुंजी_ap(ProcCmdlineFlags flags, अक्षर* स्थिर* args, बहु_सूची ap) {
        पूर्णांक r, ret = 0;

        क्रम (;;) {
                अक्षर **v;
                स्थिर अक्षर *k, *e;

                k = बहु_तर्क(ap, स्थिर अक्षर*);
                अगर (!k)
                        अवरोध;

                assert_se(v = बहु_तर्क(ap, अक्षर**));

                STRV_FOREACH(p, args) {
                        स्थिर अक्षर *word;

                        word = mangle_word(*p, flags);
                        अगर (!word)
                                जारी;

                        e = proc_cmdline_कुंजी_startswith(word, k);
                        अगर (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                अगर (r < 0)
                                        वापस r;

                                ret++;
                        }
                }
        }

        वापस ret;
}

पूर्णांक proc_cmdline_get_कुंजी_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ अक्षर **args = शून्य;
        पूर्णांक r;
        बहु_सूची ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        निश्चित(!(flags & (PROC_CMDLINE_मूल्य_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        अगर (r < 0)
                वापस r;

        बहु_शुरू(ap, flags);
        r = cmdline_get_कुंजी_ap(flags, args, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

bool proc_cmdline_मूल्य_missing(स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        निश्चित(कुंजी);

        अगर (!मूल्य) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", कुंजी);
                वापस true;
        }

        वापस false;
}
