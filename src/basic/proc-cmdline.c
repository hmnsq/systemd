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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ArgType {
        no_ವಾದ,
        required_ವಾದ,
        optional_ವಾದ,
} ArgType;

ಪೂರ್ಣಾಂಕ proc_cmdline_filter_pid1_ವಾದಗಳು(ಅಕ್ಷರ **argv, ಅಕ್ಷರ ***ret) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
                ArgType has_arg;
        } options[] = {
                { "log-level",                required_ವಾದ },
                { "log-target",               required_ವಾದ },
                { "log-color",                optional_ವಾದ },
                { "log-location",             optional_ವಾದ },
                { "log-time",                 optional_ವಾದ },
                { "unit",                     required_ವಾದ },
                { "system",                   no_ವಾದ,      },
                { "user",                     no_ವಾದ,      },
                { "test",                     no_ವಾದ,      },
                { "no-pager",                 no_ವಾದ,      },
                { "help",                     no_ವಾದ,      },
                { "version",                  no_ವಾದ,      },
                { "introspect-cli",           no_ವಾದ,      },
                { "dump-configuration-items", no_ವಾದ,      },
                { "dump-bus-properties",      no_ವಾದ,      },
                { "bus-introspect",           required_ವಾದ },
                { "dump-core",                optional_ವಾದ },
                { "crash-chvt",               required_ವಾದ },
                { "crash-vt",                 required_ವಾದ },
                { "crash-shell",              optional_ವಾದ },
                { "crash-reboot",             optional_ವಾದ },
                { "crash-action",             required_ವಾದ },
                { "confirm-spawn",            optional_ವಾದ },
                { "show-status",              optional_ವಾದ },
                { "deserialize",              required_ವಾದ },
                { "switched-root",            no_ವಾದ,      },
                { "default-standard-output",  required_ವಾದ },
                { "default-standard-error",   required_ವಾದ },
                { "machine-id",               required_ವಾದ },
                { "service-watchdogs",        required_ವಾದ },
                { "exit-code",                required_ವಾದ },
                { "timeout",                  required_ವಾದ },
        };
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *short_options = "hDbsz:";

        _cleanup_strv_free_ ಅಕ್ಷರ **filtered = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ state, r;

        ಪ್ರತಿಜ್ಞಾ(argv);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        ಪ್ರತಿಜ್ಞಾ(!ಒಳ_ಗಣ(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_ವಾದ;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                ಪೂರ್ಣಾಂಕ prev_state = state;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a = *p;

                /* Reset the state for the next step. */
                state = no_ವಾದ;

                ಆದರೆ (prev_state == required_ವಾದ ||
                    (prev_state == optional_ವಾದ && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        ಮುಂದುವರಿ;

                ಆದರೆ (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಮುಂದುವರಿ;
                }

                ಆದರೆ (a[1] == '-') {
                        ಆದರೆ (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                                ನಿಲ್ಲಿಸು;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q = startswith(a + 2, option->name);
                                ಆದರೆ (!q || !ಒಳ_ಗಣ(q[0], '=', '\0'))
                                        ಮುಂದುವರಿ;

                                /* Found matching option, updating the state if necessary. */
                                ಆದರೆ (q[0] == '\0' && option->has_arg == required_ವಾದ)
                                        state = required_ವಾದ;

                                ನಿಲ್ಲಿಸು;
                        }
                        ಮುಂದುವರಿ;
                }

                /* short option(s), e.g. -x or -xyz */
                ಆಗ (a && *++a != '\0')
                        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q = short_options; *q != '\0'; q++) {
                                ಆದರೆ (*q != *a)
                                        ಮುಂದುವರಿ;

                                /* Found matching short option. */

                                ಆದರೆ (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_ವಾದ :
                                                q[2] == ':' ? optional_ವಾದ : required_ವಾದ;

                                        a = ಖಾಲಿ; /* Not necessary to parse remaining part. */
                                }
                                ನಿಲ್ಲಿಸು;
                        }
        }

        *ret = TAKE_PTR(filtered);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ proc_cmdline(ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ಆದರೆ (e)
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, e);

        ಆದರೆ (detect_container() > 0)
                ಹಿಂತಿರುಗಿ pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ಹಿಂತಿರುಗಿ read_ತುಂಬಿದ_file("/proc/cmdline", ret, /* ret_size= */  ಖಾಲಿ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proc_cmdline_strv_ಆಂತರಿಕ(ಅಕ್ಷರ ***ret, bool filter_pid1_ವಾದಗಳು) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ಆದರೆ (e)
                ಹಿಂತಿರುಗಿ strv_ವಿಭಜಿಸು_ತುಂಬಿದ(ret, e, ಖಾಲಿ, ಹೊರತೆಗೆ_UNQUOTE|ಹೊರತೆಗೆ_RELAX|ಹೊರತೆಗೆ_RETAIN_ESCAPE);

        ಆದರೆ (detect_container() > 0) {
                _cleanup_strv_free_ ಅಕ್ಷರ **ವಾದಗಳು = ಖಾಲಿ;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &ವಾದಗಳು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (filter_pid1_ವಾದಗಳು)
                        ಹಿಂತಿರುಗಿ proc_cmdline_filter_pid1_ವಾದಗಳು(ವಾದಗಳು, ret);

                *ret = TAKE_PTR(ವಾದಗಳು);
                ಹಿಂತಿರುಗಿ 0;

        } ಇಲ್ಲದಿದ್ದರೆ {
                _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;

                r = read_ತುಂಬಿದ_file("/proc/cmdline", &s, ಖಾಲಿ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಹಿಂತಿರುಗಿ strv_ವಿಭಜಿಸು_ತುಂಬಿದ(ret, s, ಖಾಲಿ, ಹೊರತೆಗೆ_UNQUOTE|ಹೊರತೆಗೆ_RELAX|ಹೊರತೆಗೆ_RETAIN_ESCAPE);
        }
}

ಪೂರ್ಣಾಂಕ proc_cmdline_strv(ಅಕ್ಷರ ***ret) {
        ಹಿಂತಿರುಗಿ proc_cmdline_strv_ಆಂತರಿಕ(ret, /* filter_pid1_args= */ false);
}

ಸ್ಥಿರ ಅಕ್ಷರ *mangle_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶಬ್ದ, ProcCmdlineFlags flags) {
        ಅಕ್ಷರ *c = (ಅಕ್ಷರ*) startswith(ಶಬ್ದ, "rd.");
        ಆದರೆ (c) {
                /* Filter out arguments that are intended only for the initrd */

                ಆದರೆ (!ಒಳ_initrd())
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                ಆದರೆ (FLAGS_ಗಣ(flags, PROC_CMDLINE_STRIP_RD_ಪೂರ್ವಪ್ರತ್ಯಯ))
                        ಹಿಂತಿರುಗಿ c;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(flags, PROC_CMDLINE_RD_STRICT) && ಒಳ_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) ಶಬ್ದ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು mangle_ಶಬ್ದ(ಶಬ್ದ, flags) const_generic(ಶಬ್ದ, mangle_ಶಬ್ದ(ಶಬ್ದ, flags))

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ proc_cmdline_parse_strv(ಅಕ್ಷರ **ವಾದಗಳು, proc_cmdline_parse_t parse_ಅಂಶ, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ProcCmdlineFlags flags) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(parse_ಅಂಶ);

        STRV_FOREACH(ಶಬ್ದ, ವಾದಗಳು) {
                ಅಕ್ಷರ *ಕೀಲಿ, *ಮೌಲ್ಯ;

                ಕೀಲಿ = mangle_ಶಬ್ದ(*ಶಬ್ದ, flags);
                ಆದರೆ (!ಕೀಲಿ)
                        ಮುಂದುವರಿ;

                ಮೌಲ್ಯ = strchr(ಕೀಲಿ, '=');
                ಆದರೆ (ಮೌಲ್ಯ)
                        *(ಮೌಲ್ಯ++) = '\0';

                r = parse_ಅಂಶ(ಕೀಲಿ, ಮೌಲ್ಯ, ದತ್ತಾಂಶ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ proc_cmdline_parse(proc_cmdline_parse_t parse_ಅಂಶ, ಶೂನ್ಯ *ದತ್ತಾಂಶ, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ ಅಕ್ಷರ **ವಾದಗಳು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(parse_ಅಂಶ);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        ಪ್ರತಿಜ್ಞಾ(!(flags & (PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_ಆಂತರಿಕ(&ವಾದಗಳು, /* filter_pid1_args= */ true);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ proc_cmdline_parse_strv(ವಾದಗಳು, parse_ಅಂಶ, ದತ್ತಾಂಶ, flags);
}

ಸ್ಥಿರ bool relaxed_equal_char(ಅಕ್ಷರ a, ಅಕ್ಷರ b) {
        ಹಿಂತಿರುಗಿ a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

ಅಕ್ಷರ* proc_cmdline_ಕೀಲಿ_startswith(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ) {
        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

        /* Much like startswith(), but considers "-" and "_" the same */

        ಗಾಗಿ (; *ಪೂರ್ವಪ್ರತ್ಯಯ != 0; s++, ಪೂರ್ವಪ್ರತ್ಯಯ++)
                ಆದರೆ (!relaxed_equal_char(*s, *ಪೂರ್ವಪ್ರತ್ಯಯ))
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*) s;
}

bool proc_cmdline_ಕೀಲಿ_streq(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *y) {
        ಪ್ರತಿಜ್ಞಾ(x);
        ಪ್ರತಿಜ್ಞಾ(y);

        /* Much like streq(), but considers "-" and "_" the same */

        ಗಾಗಿ (; *x != 0 || *y != 0; x++, y++)
                ಆದರೆ (!relaxed_equal_char(*x, *y))
                        ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cmdline_get_ಕೀಲಿ(ಅಕ್ಷರ **ವಾದಗಳು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ProcCmdlineFlags flags, ಅಕ್ಷರ **ret_ಮೌಲ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;
        bool ಸಿಕ್ಕಿತು = false;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        STRV_FOREACH(p, ವಾದಗಳು) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶಬ್ದ;

                ಶಬ್ದ = mangle_ಶಬ್ದ(*p, flags);
                ಆದರೆ (!ಶಬ್ದ)
                        ಮುಂದುವರಿ;

                ಆದರೆ (ret_ಮೌಲ್ಯ) {
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                        e = proc_cmdline_ಕೀಲಿ_startswith(ಶಬ್ದ, ಕೀಲಿ);
                        ಆದರೆ (!e)
                                ಮುಂದುವರಿ;

                        ಆದರೆ (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ಸಿಕ್ಕಿತು = true;

                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*e == 0 && FLAGS_ಗಣ(flags, PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL))
                                ಸಿಕ್ಕಿತು = true;

                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಆದರೆ (proc_cmdline_ಕೀಲಿ_streq(ಶಬ್ದ, ಕೀಲಿ)) {
                                ಸಿಕ್ಕಿತು = true;
                                ನಿಲ್ಲಿಸು; /* we found what we were looking for */
                        }
                }
        }

        ಆದರೆ (ret_ಮೌಲ್ಯ)
                *ret_ಮೌಲ್ಯ = TAKE_PTR(v);

        ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

ಪೂರ್ಣಾಂಕ proc_cmdline_get_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ProcCmdlineFlags flags, ಅಕ್ಷರ **ret_ಮೌಲ್ಯ) {
        _cleanup_strv_free_ ಅಕ್ಷರ **ವಾದಗಳು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

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
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        ಆದರೆ (isempty(ಕೀಲಿ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (FLAGS_ಗಣ(flags, PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL) && !ret_ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = proc_cmdline_strv_ಆಂತರಿಕ(&ವಾದಗಳು, /* filter_pid1_args= */ true);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ cmdline_get_ಕೀಲಿ(ವಾದಗಳು, ಕೀಲಿ, flags, ret_ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ proc_cmdline_get_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = proc_cmdline_get_ಕೀಲಿ(ಕೀಲಿ, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0) { /* key not specified at all */
                *ret = FLAGS_ಗಣ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (v) { /* key with parameter passed */
                r = parse_boolean(v);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                *ret = r;
        } ಇಲ್ಲದಿದ್ದರೆ /* key without parameter passed */
                *ret = true;

        ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cmdline_get_ಕೀಲಿ_ap(ProcCmdlineFlags flags, ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* ವಾದಗಳು, va_list ap) {
        ಪೂರ್ಣಾಂಕ r, ret = 0;

        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ **v;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k, *e;

                k = va_arg(ap, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*);
                ಆದರೆ (!k)
                        ನಿಲ್ಲಿಸು;

                assert_se(v = va_arg(ap, ಅಕ್ಷರ**));

                STRV_FOREACH(p, ವಾದಗಳು) {
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶಬ್ದ;

                        ಶಬ್ದ = mangle_ಶಬ್ದ(*p, flags);
                        ಆದರೆ (!ಶಬ್ದ)
                                ಮುಂದುವರಿ;

                        e = proc_cmdline_ಕೀಲಿ_startswith(ಶಬ್ದ, k);
                        ಆದರೆ (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                ret++;
                        }
                }
        }

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ proc_cmdline_get_ಕೀಲಿ_many_ಆಂತರಿಕ(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ ಅಕ್ಷರ **ವಾದಗಳು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        ಪ್ರತಿಜ್ಞಾ(!(flags & (PROC_CMDLINE_ಮೌಲ್ಯ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&ವಾದಗಳು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        va_start(ap, flags);
        r = cmdline_get_ಕೀಲಿ_ap(flags, ವಾದಗಳು, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

bool proc_cmdline_ಮೌಲ್ಯ_missing(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಆದರೆ (!ಮೌಲ್ಯ) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", ಕೀಲಿ);
                ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
}
