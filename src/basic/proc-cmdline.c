/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdlib.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "initrd-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "proc-cmdline.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "virt.h"

প্রকারনির্দেশ গণনা ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

পূর্ণাঙ্ক proc_cmdline_filter_pid1_args(ৱর্ণ **argv, ৱর্ণ ***ret) {
        ধ্রুৱ সংরচনা {
                ধ্রুৱ ৱর্ণ *name;
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
        ধ্রুৱ ৱর্ণ *short_options = "hDbsz:";

        _cleanup_strv_free_ ৱর্ণ **filtered = রিক্ত;
        পূর্ণাঙ্ক state, r;

        প্রতিজ্ঞা(argv);
        প্রতিজ্ঞা(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        প্রতিজ্ঞা(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                পূর্ণাঙ্ক prev_state = state;
                ধ্রুৱ ৱর্ণ *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                যদি (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        অনুৱর্ত;

                যদি (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        যদি (r < 0)
                                ঘূৰাই r;
                        অনুৱর্ত;
                }

                যদি (a[1] == '-') {
                        যদি (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                যদি (r < 0)
                                        ঘূৰাই r;
                                ভঙ্গ;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ধ্রুৱ ৱর্ণ *q = startswith(a + 2, option->name);
                                যদি (!q || !IN_SET(q[0], '=', '\0'))
                                        অনুৱর্ত;

                                /* Found matching option, updating the state if necessary. */
                                যদি (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                ভঙ্গ;
                        }
                        অনুৱর্ত;
                }

                /* short option(s), e.g. -x or -xyz */
                যেতিয়ালৈ (a && *++a != '\0')
                        বাবে (ধ্রুৱ ৱর্ণ *q = short_options; *q != '\0'; q++) {
                                যদি (*q != *a)
                                        অনুৱর্ত;

                                /* Found matching short option. */

                                যদি (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = রিক্ত; /* Not necessary to parse remaining part. */
                                }
                                ভঙ্গ;
                        }
        }

        *ret = TAKE_PTR(filtered);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক proc_cmdline(ৱর্ণ **ret) {
        প্রতিজ্ঞা(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ধ্রুৱ ৱর্ণ *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        যদি (e)
                ঘূৰাই strdup_to(ret, e);

        যদি (detect_container() > 0)
                ঘূৰাই pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ঘূৰাই read_full_file("/proc/cmdline", ret, /* ret_size= */  রিক্ত);
}

স্থির পূর্ণাঙ্ক proc_cmdline_strv_internal(ৱর্ণ ***ret, bool filter_pid1_args) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ধ্রুৱ ৱর্ণ *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        যদি (e)
                ঘূৰাই strv_split_full(ret, e, রিক্ত, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        যদি (detect_container() > 0) {
                _cleanup_strv_free_ ৱর্ণ **args = রিক্ত;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (filter_pid1_args)
                        ঘূৰাই proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                ঘূৰাই 0;

        } অন্যথা {
                _cleanup_free_ ৱর্ণ *s = রিক্ত;

                r = read_full_file("/proc/cmdline", &s, রিক্ত);
                যদি (r < 0)
                        ঘূৰাই r;

                ঘূৰাই strv_split_full(ret, s, রিক্ত, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

পূর্ণাঙ্ক proc_cmdline_strv(ৱর্ণ ***ret) {
        ঘূৰাই proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

স্থির ৱর্ণ *mangle_word(ধ্রুৱ ৱর্ণ *word, ProcCmdlineFlags flags) {
        ৱর্ণ *c = (ৱর্ণ*) startswith(word, "rd.");
        যদি (c) {
                /* Filter out arguments that are intended only for the initrd */

                যদি (!in_initrd())
                        ঘূৰাই রিক্ত;

                যদি (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        ঘূৰাই c;

        } অন্যথা যদি (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ঘূৰাই রিক্ত;

        ঘূৰাই (ৱর্ণ*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

স্থির পূর্ণাঙ্ক proc_cmdline_parse_strv(ৱর্ণ **args, proc_cmdline_parse_t parse_item, শূন্য *data, ProcCmdlineFlags flags) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(parse_item);

        STRV_FOREACH(word, args) {
                ৱর্ণ *চাবি, *মান;

                চাবি = mangle_word(*word, flags);
                যদি (!চাবি)
                        অনুৱর্ত;

                মান = strchr(চাবি, '=');
                যদি (মান)
                        *(মান++) = '\0';

                r = parse_item(চাবি, মান, data);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক proc_cmdline_parse(proc_cmdline_parse_t parse_item, শূন্য *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ ৱর্ণ **args = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        প্রতিজ্ঞা(!(flags & (PROC_CMDLINE_মান_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই proc_cmdline_parse_strv(args, parse_item, data, flags);
}

স্থির bool relaxed_equal_char(ৱর্ণ a, ৱর্ণ b) {
        ঘূৰাই a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

ৱর্ণ* proc_cmdline_চাবি_startswith(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *prefix) {
        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        বাবে (; *prefix != 0; s++, prefix++)
                যদি (!relaxed_equal_char(*s, *prefix))
                        ঘূৰাই রিক্ত;

        ঘূৰাই (ৱর্ণ*) s;
}

bool proc_cmdline_চাবি_streq(ধ্রুৱ ৱর্ণ *x, ধ্রুৱ ৱর্ণ *y) {
        প্রতিজ্ঞা(x);
        প্রতিজ্ঞা(y);

        /* Much like streq(), but considers "-" and "_" the same */

        বাবে (; *x != 0 || *y != 0; x++, y++)
                যদি (!relaxed_equal_char(*x, *y))
                        ঘূৰাই false;

        ঘূৰাই true;
}

স্থির পূর্ণাঙ্ক cmdline_get_চাবি(ৱর্ণ **args, ধ্রুৱ ৱর্ণ *চাবি, ProcCmdlineFlags flags, ৱর্ণ **ret_মান) {
        _cleanup_free_ ৱর্ণ *v = রিক্ত;
        bool found = false;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(চাবি);

        STRV_FOREACH(p, args) {
                ধ্রুৱ ৱর্ণ *word;

                word = mangle_word(*p, flags);
                যদি (!word)
                        অনুৱর্ত;

                যদি (ret_মান) {
                        ধ্রুৱ ৱর্ণ *e;

                        e = proc_cmdline_চাবি_startswith(word, চাবি);
                        যদি (!e)
                                অনুৱর্ত;

                        যদি (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                found = true;

                        } অন্যথা যদি (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_মান_OPTIONAL))
                                found = true;

                } অন্যথা {
                        যদি (proc_cmdline_চাবি_streq(word, চাবি)) {
                                found = true;
                                ভঙ্গ; /* we found what we were looking for */
                        }
                }
        }

        যদি (ret_মান)
                *ret_মান = TAKE_PTR(v);

        ঘূৰাই found;
}

পূর্ণাঙ্ক proc_cmdline_get_চাবি(ধ্রুৱ ৱর্ণ *চাবি, ProcCmdlineFlags flags, ৱর্ণ **ret_মান) {
        _cleanup_strv_free_ ৱর্ণ **args = রিক্ত;
        পূর্ণাঙ্ক r;

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
                ঘূৰাই -EINVAL;

        যদি (FLAGS_SET(flags, PROC_CMDLINE_মান_OPTIONAL) && !ret_মান)
                ঘূৰাই -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই cmdline_get_চাবি(args, চাবি, flags, ret_মান);
}

পূর্ণাঙ্ক proc_cmdline_get_bool(ধ্রুৱ ৱর্ণ *চাবি, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ ৱর্ণ *v = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = proc_cmdline_get_চাবি(চাবি, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_মান_OPTIONAL, &v);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ঘূৰাই 0;
        }

        যদি (v) { /* key with parameter passed */
                r = parse_boolean(v);
                যদি (r < 0)
                        ঘূৰাই r;
                *ret = r;
        } অন্যথা /* key without parameter passed */
                *ret = true;

        ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক cmdline_get_চাবি_ap(ProcCmdlineFlags flags, ৱর্ণ* ধ্রুৱ* args, va_list ap) {
        পূর্ণাঙ্ক r, ret = 0;

        বাবে (;;) {
                ৱর্ণ **v;
                ধ্রুৱ ৱর্ণ *k, *e;

                k = va_arg(ap, ধ্রুৱ ৱর্ণ*);
                যদি (!k)
                        ভঙ্গ;

                assert_se(v = va_arg(ap, ৱর্ণ**));

                STRV_FOREACH(p, args) {
                        ধ্রুৱ ৱর্ণ *word;

                        word = mangle_word(*p, flags);
                        যদি (!word)
                                অনুৱর্ত;

                        e = proc_cmdline_চাবি_startswith(word, k);
                        যদি (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                ret++;
                        }
                }
        }

        ঘূৰাই ret;
}

পূর্ণাঙ্ক proc_cmdline_get_চাবি_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ ৱর্ণ **args = রিক্ত;
        পূর্ণাঙ্ক r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        প্রতিজ্ঞা(!(flags & (PROC_CMDLINE_মান_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        যদি (r < 0)
                ঘূৰাই r;

        va_start(ap, flags);
        r = cmdline_get_চাবি_ap(flags, args, ap);
        va_end(ap);

        ঘূৰাই r;
}

bool proc_cmdline_মান_missing(ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান) {
        প্রতিজ্ঞা(চাবি);

        যদি (!মান) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", চাবি);
                ঘূৰাই true;
        }

        ঘূৰাই false;
}
