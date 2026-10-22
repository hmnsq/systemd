/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "initrd-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "proc-cmdline.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "virt.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_filter_pid1_args(ವರ್ಣ **argv, ವರ್ಣ ***ret) {
        ಧ್ರುವ ಸಂರಚನಾ {
                ಧ್ರುವ ವರ್ಣ *name;
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
        ಧ್ರುವ ವರ್ಣ *short_options = "hDbsz:";

        _cleanup_strv_free_ ವರ್ಣ **filtered = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ state, r;

        ಪ್ರತಿಜ್ಞಾ(argv);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        ಪ್ರತಿಜ್ಞಾ(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                ಪೂರ್ಣಾಙ್ಕ prev_state = state;
                ಧ್ರುವ ವರ್ಣ *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                ಯದಿ (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        ಅನುವರ್ತ;

                ಯದಿ (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಅನುವರ್ತ;
                }

                ಯದಿ (a[1] == '-') {
                        ಯದಿ (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                                ಭಙ್ಗ;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ಧ್ರುವ ವರ್ಣ *q = startswith(a + 2, option->name);
                                ಯದಿ (!q || !IN_SET(q[0], '=', '\0'))
                                        ಅನುವರ್ತ;

                                /* Found matching option, updating the state if necessary. */
                                ಯದಿ (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                ಭಙ್ಗ;
                        }
                        ಅನುವರ್ತ;
                }

                /* short option(s), e.g. -x or -xyz */
                ಯಾವತ್ (a && *++a != '\0')
                        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *q = short_options; *q != '\0'; q++) {
                                ಯದಿ (*q != *a)
                                        ಅನುವರ್ತ;

                                /* Found matching short option. */

                                ಯದಿ (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = ರಿಕ್ತ; /* Not necessary to parse remaining part. */
                                }
                                ಭಙ್ಗ;
                        }
        }

        *ret = TAKE_PTR(filtered);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ proc_cmdline(ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ಧ್ರುವ ವರ್ಣ *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ಯದಿ (e)
                ಪ್ರತ್ಯಾಗಮ strdup_to(ret, e);

        ಯದಿ (detect_container() > 0)
                ಪ್ರತ್ಯಾಗಮ pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ಪ್ರತ್ಯಾಗಮ read_full_file("/proc/cmdline", ret, /* ret_size= */  ರಿಕ್ತ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proc_cmdline_strv_internal(ವರ್ಣ ***ret, bool filter_pid1_args) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ಧ್ರುವ ವರ್ಣ *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ಯದಿ (e)
                ಪ್ರತ್ಯಾಗಮ strv_split_full(ret, e, ರಿಕ್ತ, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        ಯದಿ (detect_container() > 0) {
                _cleanup_strv_free_ ವರ್ಣ **args = ರಿಕ್ತ;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (filter_pid1_args)
                        ಪ್ರತ್ಯಾಗಮ proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                ಪ್ರತ್ಯಾಗಮ 0;

        } ಅನ್ಯಥಾ {
                _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;

                r = read_full_file("/proc/cmdline", &s, ರಿಕ್ತ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತ್ಯಾಗಮ strv_split_full(ret, s, ರಿಕ್ತ, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_strv(ವರ್ಣ ***ret) {
        ಪ್ರತ್ಯಾಗಮ proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

ಸ್ಥಿರ ವರ್ಣ *mangle_word(ಧ್ರುವ ವರ್ಣ *word, ProcCmdlineFlags flags) {
        ವರ್ಣ *c = (ವರ್ಣ*) startswith(word, "rd.");
        ಯದಿ (c) {
                /* Filter out arguments that are intended only for the initrd */

                ಯದಿ (!in_initrd())
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                ಯದಿ (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        ಪ್ರತ್ಯಾಗಮ c;

        } ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ proc_cmdline_parse_strv(ವರ್ಣ **args, proc_cmdline_parse_t parse_item, ಶೂನ್ಯ *data, ProcCmdlineFlags flags) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(parse_item);

        STRV_FOREACH(word, args) {
                ವರ್ಣ *ಕೀಲಿ, *ಮೌಲ್ಯ;

                ಕೀಲಿ = mangle_word(*word, flags);
                ಯದಿ (!ಕೀಲಿ)
                        ಅನುವರ್ತ;

                ಮೌಲ್ಯ = strchr(ಕೀಲಿ, '=');
                ಯದಿ (ಮೌಲ್ಯ)
                        *(ಮೌಲ್ಯ++) = '\0';

                r = parse_item(ಕೀಲಿ, ಮೌಲ್ಯ, data);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_parse(proc_cmdline_parse_t parse_item, ಶೂನ್ಯ *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ ವರ್ಣ **args = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        ಪ್ರತಿಜ್ಞಾ(!(flags & (PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ proc_cmdline_parse_strv(args, parse_item, data, flags);
}

ಸ್ಥಿರ bool relaxed_equal_char(ವರ್ಣ a, ವರ್ಣ b) {
        ಪ್ರತ್ಯಾಗಮ a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

ವರ್ಣ* proc_cmdline_ಕೀಲಿ_startswith(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *prefix) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        ಪ್ರತಿ (; *prefix != 0; s++, prefix++)
                ಯದಿ (!relaxed_equal_char(*s, *prefix))
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ*) s;
}

bool proc_cmdline_ಕೀಲಿ_streq(ಧ್ರುವ ವರ್ಣ *x, ಧ್ರುವ ವರ್ಣ *y) {
        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(y);

        /* Much like streq(), but considers "-" and "_" the same */

        ಪ್ರತಿ (; *x != 0 || *y != 0; x++, y++)
                ಯದಿ (!relaxed_equal_char(*x, *y))
                        ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cmdline_get_ಕೀಲಿ(ವರ್ಣ **args, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ProcCmdlineFlags flags, ವರ್ಣ **ret_ಮೌಲ್ಯ) {
        _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;
        bool found = false;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        STRV_FOREACH(p, args) {
                ಧ್ರುವ ವರ್ಣ *word;

                word = mangle_word(*p, flags);
                ಯದಿ (!word)
                        ಅನುವರ್ತ;

                ಯದಿ (ret_ಮೌಲ್ಯ) {
                        ಧ್ರುವ ವರ್ಣ *e;

                        e = proc_cmdline_ಕೀಲಿ_startswith(word, ಕೀಲಿ);
                        ಯದಿ (!e)
                                ಅನುವರ್ತ;

                        ಯದಿ (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                found = true;

                        } ಅನ್ಯಥಾ ಯದಿ (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL))
                                found = true;

                } ಅನ್ಯಥಾ {
                        ಯದಿ (proc_cmdline_ಕೀಲಿ_streq(word, ಕೀಲಿ)) {
                                found = true;
                                ಭಙ್ಗ; /* we found what we were looking for */
                        }
                }
        }

        ಯದಿ (ret_ಮೌಲ್ಯ)
                *ret_ಮೌಲ್ಯ = TAKE_PTR(v);

        ಪ್ರತ್ಯಾಗಮ found;
}

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_get_ಕೀಲಿ(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ProcCmdlineFlags flags, ವರ್ಣ **ret_ಮೌಲ್ಯ) {
        _cleanup_strv_free_ ವರ್ಣ **args = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

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
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        ಯದಿ (isempty(ಕೀಲಿ))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (FLAGS_SET(flags, PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL) && !ret_ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ cmdline_get_ಕೀಲಿ(args, ಕೀಲಿ, flags, ret_ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_get_bool(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = proc_cmdline_get_ಕೀಲಿ(ಕೀಲಿ, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (v) { /* key with parameter passed */
                r = parse_boolean(v);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                *ret = r;
        } ಅನ್ಯಥಾ /* key without parameter passed */
                *ret = true;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cmdline_get_ಕೀಲಿ_ap(ProcCmdlineFlags flags, ವರ್ಣ* ಧ್ರುವ* args, va_list ap) {
        ಪೂರ್ಣಾಙ್ಕ r, ret = 0;

        ಪ್ರತಿ (;;) {
                ವರ್ಣ **v;
                ಧ್ರುವ ವರ್ಣ *k, *e;

                k = va_arg(ap, ಧ್ರುವ ವರ್ಣ*);
                ಯದಿ (!k)
                        ಭಙ್ಗ;

                assert_se(v = va_arg(ap, ವರ್ಣ**));

                STRV_FOREACH(p, args) {
                        ಧ್ರುವ ವರ್ಣ *word;

                        word = mangle_word(*p, flags);
                        ಯದಿ (!word)
                                ಅನುವರ್ತ;

                        e = proc_cmdline_ಕೀಲಿ_startswith(word, k);
                        ಯದಿ (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ret++;
                        }
                }
        }

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_get_ಕೀಲಿ_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ ವರ್ಣ **args = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        ಪ್ರತಿಜ್ಞಾ(!(flags & (PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        va_start(ap, flags);
        r = cmdline_get_ಕೀಲಿ_ap(flags, args, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

bool proc_cmdline_ಮೌಲ್ಯ_missing(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಯದಿ (!ಮೌಲ್ಯ) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", ಕೀಲಿ);
                ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
}
