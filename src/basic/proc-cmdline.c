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
        no_دليل,
        required_دليل,
        optional_دليل,
} ArgType;

صحيح proc_cmdline_filter_pid1_دليلون(حرف **argv, حرف ***ret) {
        ثابت بنية {
                ثابت حرف *name;
                ArgType has_arg;
        } options[] = {
                { "log-level",                required_دليل },
                { "log-target",               required_دليل },
                { "log-color",                optional_دليل },
                { "log-location",             optional_دليل },
                { "log-time",                 optional_دليل },
                { "unit",                     required_دليل },
                { "system",                   no_دليل,      },
                { "user",                     no_دليل,      },
                { "test",                     no_دليل,      },
                { "no-pager",                 no_دليل,      },
                { "help",                     no_دليل,      },
                { "version",                  no_دليل,      },
                { "introspect-cli",           no_دليل,      },
                { "dump-configuration-items", no_دليل,      },
                { "dump-bus-properties",      no_دليل,      },
                { "bus-introspect",           required_دليل },
                { "dump-core",                optional_دليل },
                { "crash-chvt",               required_دليل },
                { "crash-vt",                 required_دليل },
                { "crash-shell",              optional_دليل },
                { "crash-reboot",             optional_دليل },
                { "crash-action",             required_دليل },
                { "confirm-spawn",            optional_دليل },
                { "show-status",              optional_دليل },
                { "deserialize",              required_دليل },
                { "switched-root",            no_دليل,      },
                { "default-standard-output",  required_دليل },
                { "default-standard-error",   required_دليل },
                { "machine-id",               required_دليل },
                { "service-watchdogs",        required_دليل },
                { "exit-code",                required_دليل },
                { "timeout",                  required_دليل },
        };
        ثابت حرف *short_options = "hDbsz:";

        _cleanup_strv_free_ حرف **filtered = عدم;
        صحيح state, r;

        أكد(argv);
        أكد(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        أكد(!اندريون_سيٽ(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_دليل;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                صحيح prev_state = state;
                ثابت حرف *a = *p;

                /* Reset the state for the next step. */
                state = no_دليل;

                إذا (prev_state == required_دليل ||
                    (prev_state == optional_دليل && a[0] != '-'))
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
                                إذا (!q || !اندريون_سيٽ(q[0], '=', '\0'))
                                        استمر;

                                /* Found matching option, updating the state if necessary. */
                                إذا (q[0] == '\0' && option->has_arg == required_دليل)
                                        state = required_دليل;

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
                                        state = a[1] != '\0' ? no_دليل :
                                                q[2] == ':' ? optional_دليل : required_دليل;

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
                أرجع strdup_منزل(ret, e);

        إذا (detect_container() > 0)
                أرجع pid_get_cmdline(1, SIZE_MAX, 0, ret);

        أرجع read_ڀريل_file("/proc/cmdline", ret, /* ret_size= */  عدم);
}

ساكن صحيح proc_cmdline_strv_اندروني(حرف ***ret, bool filter_pid1_دليلون) {
        صحيح r;

        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت حرف *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        إذا (e)
                أرجع strv_ورهايو_ڀريل(ret, e, عدم, ڪڍو_UNQUOTE|ڪڍو_RELAX|ڪڍو_RETAIN_ESCAPE);

        إذا (detect_container() > 0) {
                _cleanup_strv_free_ حرف **دليلون = عدم;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &دليلون);
                إذا (r < 0)
                        أرجع r;

                إذا (filter_pid1_دليلون)
                        أرجع proc_cmdline_filter_pid1_دليلون(دليلون, ret);

                *ret = TAKE_PTR(دليلون);
                أرجع 0;

        } وإلا {
                _cleanup_free_ حرف *s = عدم;

                r = read_ڀريل_file("/proc/cmdline", &s, عدم);
                إذا (r < 0)
                        أرجع r;

                أرجع strv_ورهايو_ڀريل(ret, s, عدم, ڪڍو_UNQUOTE|ڪڍو_RELAX|ڪڍو_RETAIN_ESCAPE);
        }
}

صحيح proc_cmdline_strv(حرف ***ret) {
        أرجع proc_cmdline_strv_اندروني(ret, /* filter_pid1_args= */ false);
}

ساكن حرف *mangle_لفظ(ثابت حرف *لفظ, ProcCmdlineFlags flags) {
        حرف *c = (حرف*) startswith(لفظ, "rd.");
        إذا (c) {
                /* Filter out arguments that are intended only for the initrd */

                إذا (!اندريون_initrd())
                        أرجع عدم;

                إذا (FLAGS_سيٽ(flags, PROC_CMDLINE_STRIP_RD_اڳياڙي))
                        أرجع c;

        } وإلا إذا (FLAGS_سيٽ(flags, PROC_CMDLINE_RD_STRICT) && اندريون_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                أرجع عدم;

        أرجع (حرف*) لفظ;
}

#وصف_ڪريو mangle_لفظ(لفظ, flags) const_generic(لفظ, mangle_لفظ(لفظ, flags))

ساكن صحيح proc_cmdline_parse_strv(حرف **دليلون, proc_cmdline_parse_t parse_شي, فراغ *معلومات, ProcCmdlineFlags flags) {
        صحيح r;

        أكد(parse_شي);

        STRV_FOREACH(لفظ, دليلون) {
                حرف *ڪنجي, *قدر;

                ڪنجي = mangle_لفظ(*لفظ, flags);
                إذا (!ڪنجي)
                        استمر;

                قدر = strchr(ڪنجي, '=');
                إذا (قدر)
                        *(قدر++) = '\0';

                r = parse_شي(ڪنجي, قدر, معلومات);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح proc_cmdline_parse(proc_cmdline_parse_t parse_شي, فراغ *معلومات, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ حرف **دليلون = عدم;
        صحيح r;

        أكد(parse_شي);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_قدر_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_اندروني(&دليلون, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع proc_cmdline_parse_strv(دليلون, parse_شي, معلومات, flags);
}

ساكن bool relaxed_equal_char(حرف a, حرف b) {
        أرجع a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

حرف* proc_cmdline_ڪنجي_startswith(ثابت حرف *s, ثابت حرف *اڳياڙي) {
        أكد(s);
        أكد(اڳياڙي);

        /* Much like startswith(), but considers "-" and "_" the same */

        لكل (; *اڳياڙي != 0; s++, اڳياڙي++)
                إذا (!relaxed_equal_char(*s, *اڳياڙي))
                        أرجع عدم;

        أرجع (حرف*) s;
}

bool proc_cmdline_ڪنجي_streq(ثابت حرف *x, ثابت حرف *y) {
        أكد(x);
        أكد(y);

        /* Much like streq(), but considers "-" and "_" the same */

        لكل (; *x != 0 || *y != 0; x++, y++)
                إذا (!relaxed_equal_char(*x, *y))
                        أرجع false;

        أرجع true;
}

ساكن صحيح cmdline_get_ڪنجي(حرف **دليلون, ثابت حرف *ڪنجي, ProcCmdlineFlags flags, حرف **ret_قدر) {
        _cleanup_free_ حرف *v = عدم;
        bool مليو = false;
        صحيح r;

        أكد(ڪنجي);

        STRV_FOREACH(p, دليلون) {
                ثابت حرف *لفظ;

                لفظ = mangle_لفظ(*p, flags);
                إذا (!لفظ)
                        استمر;

                إذا (ret_قدر) {
                        ثابت حرف *e;

                        e = proc_cmdline_ڪنجي_startswith(لفظ, ڪنجي);
                        إذا (!e)
                                استمر;

                        إذا (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                إذا (r < 0)
                                        أرجع r;

                                مليو = true;

                        } وإلا إذا (*e == 0 && FLAGS_سيٽ(flags, PROC_CMDLINE_قدر_OPTIONAL))
                                مليو = true;

                } وإلا {
                        إذا (proc_cmdline_ڪنجي_streq(لفظ, ڪنجي)) {
                                مليو = true;
                                اكسر; /* we found what we were looking for */
                        }
                }
        }

        إذا (ret_قدر)
                *ret_قدر = TAKE_PTR(v);

        أرجع مليو;
}

صحيح proc_cmdline_get_ڪنجي(ثابت حرف *ڪنجي, ProcCmdlineFlags flags, حرف **ret_قدر) {
        _cleanup_strv_free_ حرف **دليلون = عدم;
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
        أكد(!FLAGS_سيٽ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        إذا (isempty(ڪنجي))
                أرجع -EINVAL;

        إذا (FLAGS_سيٽ(flags, PROC_CMDLINE_قدر_OPTIONAL) && !ret_قدر)
                أرجع -EINVAL;

        r = proc_cmdline_strv_اندروني(&دليلون, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع cmdline_get_ڪنجي(دليلون, ڪنجي, flags, ret_قدر);
}

صحيح proc_cmdline_get_bool(ثابت حرف *ڪنجي, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        أكد(ret);

        r = proc_cmdline_get_ڪنجي(ڪنجي, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_قدر_OPTIONAL, &v);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) { /* key not specified at all */
                *ret = FLAGS_سيٽ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
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

ساكن صحيح cmdline_get_ڪنجي_ap(ProcCmdlineFlags flags, حرف* ثابت* دليلون, va_list ap) {
        صحيح r, ret = 0;

        لكل (;;) {
                حرف **v;
                ثابت حرف *k, *e;

                k = va_arg(ap, ثابت حرف*);
                إذا (!k)
                        اكسر;

                assert_se(v = va_arg(ap, حرف**));

                STRV_FOREACH(p, دليلون) {
                        ثابت حرف *لفظ;

                        لفظ = mangle_لفظ(*p, flags);
                        إذا (!لفظ)
                                استمر;

                        e = proc_cmdline_ڪنجي_startswith(لفظ, k);
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

صحيح proc_cmdline_get_ڪنجي_many_اندروني(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ حرف **دليلون = عدم;
        صحيح r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_قدر_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&دليلون);
        إذا (r < 0)
                أرجع r;

        va_start(ap, flags);
        r = cmdline_get_ڪنجي_ap(flags, دليلون, ap);
        va_end(ap);

        أرجع r;
}

bool proc_cmdline_قدر_missing(ثابت حرف *ڪنجي, ثابت حرف *قدر) {
        أكد(ڪنجي);

        إذا (!قدر) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", ڪنجي);
                أرجع true;
        }

        أرجع false;
}
