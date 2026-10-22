/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "initrd-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "proc-cmdline.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "virt.h"

প্রকারলেখ গণনা ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

পূর্ণসংখ্যা proc_cmdline_filter_pid1_args(অক্ষর **argv, অক্ষর ***ret) {
        ধ্রুবক কাঠামো {
                ধ্রুবক অক্ষর *name;
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
        ধ্রুবক অক্ষর *short_options = "hDbsz:";

        _cleanup_strv_free_ অক্ষর **filtered = ফাঁকা;
        পূর্ণসংখ্যা state, r;

        প্রতিজ্ঞা(argv);
        প্রতিজ্ঞা(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        প্রতিজ্ঞা(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                পূর্ণসংখ্যা পূর্ব_state = state;
                ধ্রুবক অক্ষর *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                যদি (পূর্ব_state == required_argument ||
                    (পূর্ব_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        চালু;

                যদি (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        যদি (r < 0)
                                ফেরত r;
                        চালু;
                }

                যদি (a[1] == '-') {
                        যদি (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                যদি (r < 0)
                                        ফেরত r;
                                বিরতি;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ধ্রুবক অক্ষর *q = startswith(a + 2, option->name);
                                যদি (!q || !IN_SET(q[0], '=', '\0'))
                                        চালু;

                                /* Found matching option, updating the state if necessary. */
                                যদি (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                বিরতি;
                        }
                        চালু;
                }

                /* short option(s), e.g. -x or -xyz */
                যখন (a && *++a != '\0')
                        জন্য (ধ্রুবক অক্ষর *q = short_options; *q != '\0'; q++) {
                                যদি (*q != *a)
                                        চালু;

                                /* Found matching short option. */

                                যদি (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = ফাঁকা; /* Not necessary to parse remaining part. */
                                }
                                বিরতি;
                        }
        }

        *ret = TAKE_PTR(filtered);
        ফেরত 0;
}

পূর্ণসংখ্যা proc_cmdline(অক্ষর **ret) {
        প্রতিজ্ঞা(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ধ্রুবক অক্ষর *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        যদি (e)
                ফেরত strdup_to(ret, e);

        যদি (detect_container() > 0)
                ফেরত pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ফেরত read_full_file("/proc/cmdline", ret, /* ret_size= */  ফাঁকা);
}

স্থির পূর্ণসংখ্যা proc_cmdline_strv_internal(অক্ষর ***ret, bool filter_pid1_args) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ধ্রুবক অক্ষর *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        যদি (e)
                ফেরত strv_split_full(ret, e, ফাঁকা, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        যদি (detect_container() > 0) {
                _cleanup_strv_free_ অক্ষর **args = ফাঁকা;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                যদি (r < 0)
                        ফেরত r;

                যদি (filter_pid1_args)
                        ফেরত proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                ফেরত 0;

        } অন্যথা {
                _cleanup_free_ অক্ষর *s = ফাঁকা;

                r = read_full_file("/proc/cmdline", &s, ফাঁকা);
                যদি (r < 0)
                        ফেরত r;

                ফেরত strv_split_full(ret, s, ফাঁকা, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

পূর্ণসংখ্যা proc_cmdline_strv(অক্ষর ***ret) {
        ফেরত proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

স্থির অক্ষর *mangle_word(ধ্রুবক অক্ষর *word, ProcCmdlineFlags flags) {
        অক্ষর *c = (অক্ষর*) startswith(word, "rd.");
        যদি (c) {
                /* Filter out arguments that are intended only for the initrd */

                যদি (!in_initrd())
                        ফেরত ফাঁকা;

                যদি (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        ফেরত c;

        } অন্যথা যদি (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ফেরত ফাঁকা;

        ফেরত (অক্ষর*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

স্থির পূর্ণসংখ্যা proc_cmdline_parse_strv(অক্ষর **args, proc_cmdline_parse_t parse_item, শূন্য *তথ্য, ProcCmdlineFlags flags) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(parse_item);

        STRV_FOREACH(word, args) {
                অক্ষর *চাবি, *মান;

                চাবি = mangle_word(*word, flags);
                যদি (!চাবি)
                        চালু;

                মান = strchr(চাবি, '=');
                যদি (মান)
                        *(মান++) = '\0';

                r = parse_item(চাবি, মান, তথ্য);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা proc_cmdline_parse(proc_cmdline_parse_t parse_item, শূন্য *তথ্য, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ অক্ষর **args = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        প্রতিজ্ঞা(!(flags & (PROC_CMDLINE_মান_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        যদি (r < 0)
                ফেরত r;

        ফেরত proc_cmdline_parse_strv(args, parse_item, তথ্য, flags);
}

স্থির bool relaxed_equal_char(অক্ষর a, অক্ষর b) {
        ফেরত a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

অক্ষর* proc_cmdline_চাবি_startswith(ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *prefix) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        জন্য (; *prefix != 0; s++, prefix++)
                যদি (!relaxed_equal_char(*s, *prefix))
                        ফেরত ফাঁকা;

        ফেরত (অক্ষর*) s;
}

bool proc_cmdline_চাবি_streq(ধ্রুবক অক্ষর *x, ধ্রুবক অক্ষর *y) {
        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(y);

        /* Much like streq(), but considers "-" and "_" the same */

        জন্য (; *x != 0 || *y != 0; x++, y++)
                যদি (!relaxed_equal_char(*x, *y))
                        ফেরত false;

        ফেরত true;
}

স্থির পূর্ণসংখ্যা cmdline_get_চাবি(অক্ষর **args, ধ্রুবক অক্ষর *চাবি, ProcCmdlineFlags flags, অক্ষর **ret_মান) {
        _cleanup_free_ অক্ষর *v = ফাঁকা;
        bool found = false;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(চাবি);

        STRV_FOREACH(p, args) {
                ধ্রুবক অক্ষর *word;

                word = mangle_word(*p, flags);
                যদি (!word)
                        চালু;

                যদি (ret_মান) {
                        ধ্রুবক অক্ষর *e;

                        e = proc_cmdline_চাবি_startswith(word, চাবি);
                        যদি (!e)
                                চালু;

                        যদি (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                যদি (r < 0)
                                        ফেরত r;

                                found = true;

                        } অন্যথা যদি (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_মান_OPTIONAL))
                                found = true;

                } অন্যথা {
                        যদি (proc_cmdline_চাবি_streq(word, চাবি)) {
                                found = true;
                                বিরতি; /* we found what we were looking for */
                        }
                }
        }

        যদি (ret_মান)
                *ret_মান = TAKE_PTR(v);

        ফেরত found;
}

পূর্ণসংখ্যা proc_cmdline_get_চাবি(ধ্রুবক অক্ষর *চাবি, ProcCmdlineFlags flags, অক্ষর **ret_মান) {
        _cleanup_strv_free_ অক্ষর **args = ফাঁকা;
        পূর্ণসংখ্যা r;

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
        প্রতিজ্ঞা(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        যদি (isempty(চাবি))
                ফেরত -EINVAL;

        যদি (FLAGS_SET(flags, PROC_CMDLINE_মান_OPTIONAL) && !ret_মান)
                ফেরত -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        যদি (r < 0)
                ফেরত r;

        ফেরত cmdline_get_চাবি(args, চাবি, flags, ret_মান);
}

পূর্ণসংখ্যা proc_cmdline_get_bool(ধ্রুবক অক্ষর *চাবি, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ অক্ষর *v = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = proc_cmdline_get_চাবি(চাবি, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_মান_OPTIONAL, &v);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ফেরত 0;
        }

        যদি (v) { /* key with parameter passed */
                r = parse_boolean(v);
                যদি (r < 0)
                        ফেরত r;
                *ret = r;
        } অন্যথা /* key without parameter passed */
                *ret = true;

        ফেরত 1;
}

স্থির পূর্ণসংখ্যা cmdline_get_চাবি_ap(ProcCmdlineFlags flags, অক্ষর* ধ্রুবক* args, va_list ap) {
        পূর্ণসংখ্যা r, ret = 0;

        জন্য (;;) {
                অক্ষর **v;
                ধ্রুবক অক্ষর *k, *e;

                k = va_arg(ap, ধ্রুবক অক্ষর*);
                যদি (!k)
                        বিরতি;

                assert_se(v = va_arg(ap, অক্ষর**));

                STRV_FOREACH(p, args) {
                        ধ্রুবক অক্ষর *word;

                        word = mangle_word(*p, flags);
                        যদি (!word)
                                চালু;

                        e = proc_cmdline_চাবি_startswith(word, k);
                        যদি (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                যদি (r < 0)
                                        ফেরত r;

                                ret++;
                        }
                }
        }

        ফেরত ret;
}

পূর্ণসংখ্যা proc_cmdline_get_চাবি_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ অক্ষর **args = ফাঁকা;
        পূর্ণসংখ্যা r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        প্রতিজ্ঞা(!(flags & (PROC_CMDLINE_মান_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        যদি (r < 0)
                ফেরত r;

        va_start(ap, flags);
        r = cmdline_get_চাবি_ap(flags, args, ap);
        va_end(ap);

        ফেরত r;
}

bool proc_cmdline_মান_missing(ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান) {
        প্রতিজ্ঞা(চাবি);

        যদি (!মান) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", চাবি);
                ফেরত true;
        }

        ফেরত false;
}
