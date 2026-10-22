/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "extract-word.h"
#ضمن "fileio.h"
#ضمن "initrd-util.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "proc-cmdline.h"
#ضمن "process-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "virt.h"

تعريفنوع تعداد ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

صحيح proc_cmdline_filter_pid1_حجوجي(حرف **argv, حرف ***ret) {
        ثابت بنية {
                ثابت حرف *name;
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
        ثابت حرف *short_options = "hDbsz:";

        _cleanup_strv_free_ حرف **filtered = عدم;
        صحيح state, r;

        أكد(argv);
        أكد(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        أكد(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                صحيح prev_state = state;
                ثابت حرف *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                إذا (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        استمر;

                إذا (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        إذا (r < 0)
                                أرجع r;
                        استمر;
                }

                إذا (a[1] == '-') {
                        إذا (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                إذا (r < 0)
                                        أرجع r;
                                اكسر;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ثابت حرف *q = startswith(a + 2, option->name);
                                إذا (!q || !IN_SET(q[0], '=', '\0'))
                                        استمر;

                                /* Found matching option, updating the state if necessary. */
                                إذا (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                اكسر;
                        }
                        استمر;
                }

                /* short option(s), e.g. -x or -xyz */
                طالما (a && *++a != '\0')
                        لكل (ثابت حرف *q = short_options; *q != '\0'; q++) {
                                إذا (*q != *a)
                                        استمر;

                                /* Found matching short option. */

                                إذا (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = عدم; /* Not necessary to parse remaining part. */
                                }
                                اكسر;
                        }
        }

        *ret = TAKE_PTR(filtered);
        أرجع 0;
}

صحيح proc_cmdline(حرف **ret) {
        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت حرف *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        إذا (e)
                أرجع strdup_to(ret, e);

        إذا (detect_container() > 0)
                أرجع pid_get_cmdline(1, SIZE_MAX, 0, ret);

        أرجع read_full_file("/proc/cmdline", ret, /* ret_size= */  عدم);
}

ساكن صحيح proc_cmdline_strv_internal(حرف ***ret, bool filter_pid1_حجوجي) {
        صحيح r;

        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت حرف *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        إذا (e)
                أرجع strv_split_full(ret, e, عدم, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        إذا (detect_container() > 0) {
                _cleanup_strv_free_ حرف **حجوجي = عدم;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &حجوجي);
                إذا (r < 0)
                        أرجع r;

                إذا (filter_pid1_حجوجي)
                        أرجع proc_cmdline_filter_pid1_حجوجي(حجوجي, ret);

                *ret = TAKE_PTR(حجوجي);
                أرجع 0;

        } وإلا {
                _cleanup_free_ حرف *s = عدم;

                r = read_full_file("/proc/cmdline", &s, عدم);
                إذا (r < 0)
                        أرجع r;

                أرجع strv_split_full(ret, s, عدم, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

صحيح proc_cmdline_strv(حرف ***ret) {
        أرجع proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

ساكن حرف *mangle_word(ثابت حرف *word, ProcCmdlineFlags flags) {
        حرف *c = (حرف*) startswith(word, "rd.");
        إذا (c) {
                /* Filter out arguments that are intended only for the initrd */

                إذا (!in_initrd())
                        أرجع عدم;

                إذا (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        أرجع c;

        } وإلا إذا (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                أرجع عدم;

        أرجع (حرف*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

ساكن صحيح proc_cmdline_parse_strv(حرف **حجوجي, proc_cmdline_parse_t parse_item, فراغ *data, ProcCmdlineFlags flags) {
        صحيح r;

        أكد(parse_item);

        STRV_FOREACH(word, حجوجي) {
                حرف *مبودي, *درج;

                مبودي = mangle_word(*word, flags);
                إذا (!مبودي)
                        استمر;

                درج = strchr(مبودي, '=');
                إذا (درج)
                        *(درج++) = '\0';

                r = parse_item(مبودي, درج, data);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح proc_cmdline_parse(proc_cmdline_parse_t parse_item, فراغ *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ حرف **حجوجي = عدم;
        صحيح r;

        أكد(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_درج_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&حجوجي, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع proc_cmdline_parse_strv(حجوجي, parse_item, data, flags);
}

ساكن bool relaxed_equal_char(حرف a, حرف b) {
        أرجع a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

حرف* proc_cmdline_مبودي_startswith(ثابت حرف *s, ثابت حرف *prefix) {
        أكد(s);
        أكد(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        لكل (; *prefix != 0; s++, prefix++)
                إذا (!relaxed_equal_char(*s, *prefix))
                        أرجع عدم;

        أرجع (حرف*) s;
}

bool proc_cmdline_مبودي_streq(ثابت حرف *x, ثابت حرف *y) {
        أكد(x);
        أكد(y);

        /* Much like streq(), but considers "-" and "_" the same */

        لكل (; *x != 0 || *y != 0; x++, y++)
                إذا (!relaxed_equal_char(*x, *y))
                        أرجع false;

        أرجع true;
}

ساكن صحيح cmdline_get_مبودي(حرف **حجوجي, ثابت حرف *مبودي, ProcCmdlineFlags flags, حرف **ret_درج) {
        _cleanup_free_ حرف *v = عدم;
        bool found = false;
        صحيح r;

        أكد(مبودي);

        STRV_FOREACH(p, حجوجي) {
                ثابت حرف *word;

                word = mangle_word(*p, flags);
                إذا (!word)
                        استمر;

                إذا (ret_درج) {
                        ثابت حرف *e;

                        e = proc_cmdline_مبودي_startswith(word, مبودي);
                        إذا (!e)
                                استمر;

                        إذا (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                إذا (r < 0)
                                        أرجع r;

                                found = true;

                        } وإلا إذا (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_درج_OPTIONAL))
                                found = true;

                } وإلا {
                        إذا (proc_cmdline_مبودي_streq(word, مبودي)) {
                                found = true;
                                اكسر; /* we found what we were looking for */
                        }
                }
        }

        إذا (ret_درج)
                *ret_درج = TAKE_PTR(v);

        أرجع found;
}

صحيح proc_cmdline_get_مبودي(ثابت حرف *مبودي, ProcCmdlineFlags flags, حرف **ret_درج) {
        _cleanup_strv_free_ حرف **حجوجي = عدم;
        صحيح r;

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
        أكد(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        إذا (isempty(مبودي))
                أرجع -EINVAL;

        إذا (FLAGS_SET(flags, PROC_CMDLINE_درج_OPTIONAL) && !ret_درج)
                أرجع -EINVAL;

        r = proc_cmdline_strv_internal(&حجوجي, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع cmdline_get_مبودي(حجوجي, مبودي, flags, ret_درج);
}

صحيح proc_cmdline_get_bool(ثابت حرف *مبودي, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        أكد(ret);

        r = proc_cmdline_get_مبودي(مبودي, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_درج_OPTIONAL, &v);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                أرجع 0;
        }

        إذا (v) { /* key with parameter passed */
                r = parse_boolean(v);
                إذا (r < 0)
                        أرجع r;
                *ret = r;
        } وإلا /* key without parameter passed */
                *ret = true;

        أرجع 1;
}

ساكن صحيح cmdline_get_مبودي_ap(ProcCmdlineFlags flags, حرف* ثابت* حجوجي, va_list ap) {
        صحيح r, ret = 0;

        لكل (;;) {
                حرف **v;
                ثابت حرف *k, *e;

                k = va_arg(ap, ثابت حرف*);
                إذا (!k)
                        اكسر;

                assert_se(v = va_arg(ap, حرف**));

                STRV_FOREACH(p, حجوجي) {
                        ثابت حرف *word;

                        word = mangle_word(*p, flags);
                        إذا (!word)
                                استمر;

                        e = proc_cmdline_مبودي_startswith(word, k);
                        إذا (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                إذا (r < 0)
                                        أرجع r;

                                ret++;
                        }
                }
        }

        أرجع ret;
}

صحيح proc_cmdline_get_مبودي_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ حرف **حجوجي = عدم;
        صحيح r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_درج_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&حجوجي);
        إذا (r < 0)
                أرجع r;

        va_start(ap, flags);
        r = cmdline_get_مبودي_ap(flags, حجوجي, ap);
        va_end(ap);

        أرجع r;
}

bool proc_cmdline_درج_missing(ثابت حرف *مبودي, ثابت حرف *درج) {
        أكد(مبودي);

        إذا (!درج) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", مبودي);
                أرجع true;
        }

        أرجع false;
}
