/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "initrd-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "proc-cmdline.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "virt.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_filter_pid1_args(ଅକ୍ଷର **argv, ଅକ୍ଷର ***ret) {
        ଅଚଳ ସଂରଚନା {
                ଅଚଳ ଅକ୍ଷର *name;
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
        ଅଚଳ ଅକ୍ଷର *short_options = "hDbsz:";

        _cleanup_strv_free_ ଅକ୍ଷର **filtered = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା state, r;

        ପ୍ରତିଜ୍ଞା(argv);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        ପ୍ରତିଜ୍ଞା(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_state = state;
                ଅଚଳ ଅକ୍ଷର *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                ଯଦି (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        ଜାରି;

                ଯଦି (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଜାରି;
                }

                ଯଦି (a[1] == '-') {
                        ଯଦି (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                                ଭାଙ୍ଗ;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ଅଚଳ ଅକ୍ଷର *q = startswith(a + 2, option->name);
                                ଯଦି (!q || !IN_SET(q[0], '=', '\0'))
                                        ଜାରି;

                                /* Found matching option, updating the state if necessary. */
                                ଯଦି (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                ଭାଙ୍ଗ;
                        }
                        ଜାରି;
                }

                /* short option(s), e.g. -x or -xyz */
                ଯେତେବେଳେ (a && *++a != '\0')
                        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *q = short_options; *q != '\0'; q++) {
                                ଯଦି (*q != *a)
                                        ଜାରି;

                                /* Found matching short option. */

                                ଯଦି (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = ଖାଲି; /* Not necessary to parse remaining part. */
                                }
                                ଭାଙ୍ଗ;
                        }
        }

        *ret = TAKE_PTR(filtered);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline(ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ଅଚଳ ଅକ୍ଷର *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ଯଦି (e)
                ଫେରନ୍ତୁ strdup_to(ret, e);

        ଯଦି (detect_container() > 0)
                ଫେରନ୍ତୁ pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ଫେରନ୍ତୁ read_full_file("/proc/cmdline", ret, /* ret_size= */  ଖାଲି);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_strv_internal(ଅକ୍ଷର ***ret, bool filter_pid1_args) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ଅଚଳ ଅକ୍ଷର *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ଯଦି (e)
                ଫେରନ୍ତୁ strv_split_full(ret, e, ଖାଲି, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        ଯଦି (detect_container() > 0) {
                _cleanup_strv_free_ ଅକ୍ଷର **args = ଖାଲି;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (filter_pid1_args)
                        ଫେରନ୍ତୁ proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                ଫେରନ୍ତୁ 0;

        } ଅନ୍ୟଥା {
                _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;

                r = read_full_file("/proc/cmdline", &s, ଖାଲି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଫେରନ୍ତୁ strv_split_full(ret, s, ଖାଲି, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_strv(ଅକ୍ଷର ***ret) {
        ଫେରନ୍ତୁ proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

ସ୍ଥିର ଅକ୍ଷର *mangle_word(ଅଚଳ ଅକ୍ଷର *word, ProcCmdlineFlags flags) {
        ଅକ୍ଷର *c = (ଅକ୍ଷର*) startswith(word, "rd.");
        ଯଦି (c) {
                /* Filter out arguments that are intended only for the initrd */

                ଯଦି (!in_initrd())
                        ଫେରନ୍ତୁ ଖାଲି;

                ଯଦି (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        ଫେରନ୍ତୁ c;

        } ଅନ୍ୟଥା ଯଦି (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ (ଅକ୍ଷର*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_parse_strv(ଅକ୍ଷର **args, proc_cmdline_parse_t parse_item, ଶୂନ୍ୟ *data, ProcCmdlineFlags flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(parse_item);

        STRV_FOREACH(word, args) {
                ଅକ୍ଷର *ଚାବି, *ମୂଲ୍ୟ;

                ଚାବି = mangle_word(*word, flags);
                ଯଦି (!ଚାବି)
                        ଜାରି;

                ମୂଲ୍ୟ = strchr(ଚାବି, '=');
                ଯଦି (ମୂଲ୍ୟ)
                        *(ମୂଲ୍ୟ++) = '\0';

                r = parse_item(ଚାବି, ମୂଲ୍ୟ, data);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_parse(proc_cmdline_parse_t parse_item, ଶୂନ୍ୟ *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ ଅକ୍ଷର **args = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        ପ୍ରତିଜ୍ଞା(!(flags & (PROC_CMDLINE_ମୂଲ୍ୟ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ proc_cmdline_parse_strv(args, parse_item, data, flags);
}

ସ୍ଥିର bool relaxed_equal_char(ଅକ୍ଷର a, ଅକ୍ଷର b) {
        ଫେରନ୍ତୁ a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

ଅକ୍ଷର* proc_cmdline_ଚାବି_startswith(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *prefix) {
        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        ପାଇଁ (; *prefix != 0; s++, prefix++)
                ଯଦି (!relaxed_equal_char(*s, *prefix))
                        ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ (ଅକ୍ଷର*) s;
}

bool proc_cmdline_ଚାବି_streq(ଅଚଳ ଅକ୍ଷର *x, ଅଚଳ ଅକ୍ଷର *y) {
        ପ୍ରତିଜ୍ଞା(x);
        ପ୍ରତିଜ୍ଞା(y);

        /* Much like streq(), but considers "-" and "_" the same */

        ପାଇଁ (; *x != 0 || *y != 0; x++, y++)
                ଯଦି (!relaxed_equal_char(*x, *y))
                        ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdline_get_ଚାବି(ଅକ୍ଷର **args, ଅଚଳ ଅକ୍ଷର *ଚାବି, ProcCmdlineFlags flags, ଅକ୍ଷର **ret_ମୂଲ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;
        bool found = false;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ଚାବି);

        STRV_FOREACH(p, args) {
                ଅଚଳ ଅକ୍ଷର *word;

                word = mangle_word(*p, flags);
                ଯଦି (!word)
                        ଜାରି;

                ଯଦି (ret_ମୂଲ୍ୟ) {
                        ଅଚଳ ଅକ୍ଷର *e;

                        e = proc_cmdline_ଚାବି_startswith(word, ଚାବି);
                        ଯଦି (!e)
                                ଜାରି;

                        ଯଦି (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                found = true;

                        } ଅନ୍ୟଥା ଯଦି (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_ମୂଲ୍ୟ_OPTIONAL))
                                found = true;

                } ଅନ୍ୟଥା {
                        ଯଦି (proc_cmdline_ଚାବି_streq(word, ଚାବି)) {
                                found = true;
                                ଭାଙ୍ଗ; /* we found what we were looking for */
                        }
                }
        }

        ଯଦି (ret_ମୂଲ୍ୟ)
                *ret_ମୂଲ୍ୟ = TAKE_PTR(v);

        ଫେରନ୍ତୁ found;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_get_ଚାବି(ଅଚଳ ଅକ୍ଷର *ଚାବି, ProcCmdlineFlags flags, ଅକ୍ଷର **ret_ମୂଲ୍ୟ) {
        _cleanup_strv_free_ ଅକ୍ଷର **args = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        ଯଦି (isempty(ଚାବି))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (FLAGS_SET(flags, PROC_CMDLINE_ମୂଲ୍ୟ_OPTIONAL) && !ret_ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ cmdline_get_ଚାବି(args, ଚାବି, flags, ret_ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_get_bool(ଅଚଳ ଅକ୍ଷର *ଚାବି, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = proc_cmdline_get_ଚାବି(ଚାବି, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_ମୂଲ୍ୟ_OPTIONAL, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (v) { /* key with parameter passed */
                r = parse_boolean(v);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                *ret = r;
        } ଅନ୍ୟଥା /* key without parameter passed */
                *ret = true;

        ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmdline_get_ଚାବି_ap(ProcCmdlineFlags flags, ଅକ୍ଷର* ଅଚଳ* args, va_list ap) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ret = 0;

        ପାଇଁ (;;) {
                ଅକ୍ଷର **v;
                ଅଚଳ ଅକ୍ଷର *k, *e;

                k = va_arg(ap, ଅଚଳ ଅକ୍ଷର*);
                ଯଦି (!k)
                        ଭାଙ୍ଗ;

                assert_se(v = va_arg(ap, ଅକ୍ଷର**));

                STRV_FOREACH(p, args) {
                        ଅଚଳ ଅକ୍ଷର *word;

                        word = mangle_word(*p, flags);
                        ଯଦି (!word)
                                ଜାରି;

                        e = proc_cmdline_ଚାବି_startswith(word, k);
                        ଯଦି (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                ret++;
                        }
                }
        }

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_get_ଚାବି_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ ଅକ୍ଷର **args = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        ପ୍ରତିଜ୍ଞା(!(flags & (PROC_CMDLINE_ମୂଲ୍ୟ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        va_start(ap, flags);
        r = cmdline_get_ଚାବି_ap(flags, args, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

bool proc_cmdline_ମୂଲ୍ୟ_missing(ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପ୍ରତିଜ୍ଞା(ଚାବି);

        ଯଦି (!ମୂଲ୍ୟ) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", ଚାବି);
                ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
}
