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
        no_دلیل,
        required_دلیل,
        optional_دلیل,
} ArgType;

صحيح proc_cmdline_filter_pid1_دلیلاں(حرف **argv, حرف ***ret) {
        ثابت بنية {
                ثابت حرف *name;
                ArgType has_arg;
        } options[] = {
                { "log-level",                required_دلیل },
                { "log-target",               required_دلیل },
                { "log-color",                optional_دلیل },
                { "log-location",             optional_دلیل },
                { "log-time",                 optional_دلیل },
                { "unit",                     required_دلیل },
                { "system",                   no_دلیل,      },
                { "user",                     no_دلیل,      },
                { "test",                     no_دلیل,      },
                { "no-pager",                 no_دلیل,      },
                { "help",                     no_دلیل,      },
                { "version",                  no_دلیل,      },
                { "introspect-cli",           no_دلیل,      },
                { "dump-configuration-items", no_دلیل,      },
                { "dump-bus-properties",      no_دلیل,      },
                { "bus-introspect",           required_دلیل },
                { "dump-core",                optional_دلیل },
                { "crash-chvt",               required_دلیل },
                { "crash-vt",                 required_دلیل },
                { "crash-shell",              optional_دلیل },
                { "crash-reboot",             optional_دلیل },
                { "crash-action",             required_دلیل },
                { "confirm-spawn",            optional_دلیل },
                { "show-status",              optional_دلیل },
                { "deserialize",              required_دلیل },
                { "switched-root",            no_دلیل,      },
                { "default-standard-output",  required_دلیل },
                { "default-standard-error",   required_دلیل },
                { "machine-id",               required_دلیل },
                { "service-watchdogs",        required_دلیل },
                { "exit-code",                required_دلیل },
                { "timeout",                  required_دلیل },
        };
        ثابت حرف *short_options = "hDbsz:";

        _cleanup_strv_free_ حرف **filtered = عدم;
        صحيح state, r;

        أكد(argv);
        أكد(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        أكد(!وچ_سیٹ(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_دلیل;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                صحيح prev_state = state;
                ثابت حرف *a = *p;

                /* Reset the state for the next step. */
                state = no_دلیل;

                إذا (prev_state == required_دلیل ||
                    (prev_state == optional_دلیل && a[0] != '-'))
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
                                إذا (!q || !وچ_سیٹ(q[0], '=', '\0'))
                                        استمر;

                                /* Found matching option, updating the state if necessary. */
                                إذا (q[0] == '\0' && option->has_arg == required_دلیل)
                                        state = required_دلیل;

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
                                        state = a[1] != '\0' ? no_دلیل :
                                                q[2] == ':' ? optional_دلیل : required_دلیل;

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

        أرجع read_بھریا_file("/proc/cmdline", ret, /* ret_size= */  عدم);
}

ساكن صحيح proc_cmdline_strv_اندرونی(حرف ***ret, bool filter_pid1_دلیلاں) {
        صحيح r;

        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت حرف *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        إذا (e)
                أرجع strv_ونڈو_بھریا(ret, e, عدم, کڈھو_UNQUOTE|کڈھو_RELAX|کڈھو_RETAIN_ESCAPE);

        إذا (detect_container() > 0) {
                _cleanup_strv_free_ حرف **دلیلاں = عدم;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &دلیلاں);
                إذا (r < 0)
                        أرجع r;

                إذا (filter_pid1_دلیلاں)
                        أرجع proc_cmdline_filter_pid1_دلیلاں(دلیلاں, ret);

                *ret = TAKE_PTR(دلیلاں);
                أرجع 0;

        } وإلا {
                _cleanup_free_ حرف *s = عدم;

                r = read_بھریا_file("/proc/cmdline", &s, عدم);
                إذا (r < 0)
                        أرجع r;

                أرجع strv_ونڈو_بھریا(ret, s, عدم, کڈھو_UNQUOTE|کڈھو_RELAX|کڈھو_RETAIN_ESCAPE);
        }
}

صحيح proc_cmdline_strv(حرف ***ret) {
        أرجع proc_cmdline_strv_اندرونی(ret, /* filter_pid1_args= */ false);
}

ساكن حرف *mangle_لفظ(ثابت حرف *لفظ, ProcCmdlineFlags flags) {
        حرف *c = (حرف*) startswith(لفظ, "rd.");
        إذا (c) {
                /* Filter out arguments that are intended only for the initrd */

                إذا (!وچ_initrd())
                        أرجع عدم;

                إذا (FLAGS_سیٹ(flags, PROC_CMDLINE_STRIP_RD_اگیتر))
                        أرجع c;

        } وإلا إذا (FLAGS_سیٹ(flags, PROC_CMDLINE_RD_STRICT) && وچ_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                أرجع عدم;

        أرجع (حرف*) لفظ;
}

#تعریف_کرو mangle_لفظ(لفظ, flags) const_generic(لفظ, mangle_لفظ(لفظ, flags))

ساكن صحيح proc_cmdline_parse_strv(حرف **دلیلاں, proc_cmdline_parse_t parse_شے, فراغ *انکڑے, ProcCmdlineFlags flags) {
        صحيح r;

        أكد(parse_شے);

        STRV_FOREACH(لفظ, دلیلاں) {
                حرف *کلید, *قدر;

                کلید = mangle_لفظ(*لفظ, flags);
                إذا (!کلید)
                        استمر;

                قدر = strchr(کلید, '=');
                إذا (قدر)
                        *(قدر++) = '\0';

                r = parse_شے(کلید, قدر, انکڑے);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح proc_cmdline_parse(proc_cmdline_parse_t parse_شے, فراغ *انکڑے, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ حرف **دلیلاں = عدم;
        صحيح r;

        أكد(parse_شے);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_قدر_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_اندرونی(&دلیلاں, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع proc_cmdline_parse_strv(دلیلاں, parse_شے, انکڑے, flags);
}

ساكن bool relaxed_equal_char(حرف a, حرف b) {
        أرجع a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

حرف* proc_cmdline_کلید_startswith(ثابت حرف *s, ثابت حرف *اگیتر) {
        أكد(s);
        أكد(اگیتر);

        /* Much like startswith(), but considers "-" and "_" the same */

        لكل (; *اگیتر != 0; s++, اگیتر++)
                إذا (!relaxed_equal_char(*s, *اگیتر))
                        أرجع عدم;

        أرجع (حرف*) s;
}

bool proc_cmdline_کلید_streq(ثابت حرف *x, ثابت حرف *y) {
        أكد(x);
        أكد(y);

        /* Much like streq(), but considers "-" and "_" the same */

        لكل (; *x != 0 || *y != 0; x++, y++)
                إذا (!relaxed_equal_char(*x, *y))
                        أرجع false;

        أرجع true;
}

ساكن صحيح cmdline_get_کلید(حرف **دلیلاں, ثابت حرف *کلید, ProcCmdlineFlags flags, حرف **ret_قدر) {
        _cleanup_free_ حرف *v = عدم;
        bool لبھیا = false;
        صحيح r;

        أكد(کلید);

        STRV_FOREACH(p, دلیلاں) {
                ثابت حرف *لفظ;

                لفظ = mangle_لفظ(*p, flags);
                إذا (!لفظ)
                        استمر;

                إذا (ret_قدر) {
                        ثابت حرف *e;

                        e = proc_cmdline_کلید_startswith(لفظ, کلید);
                        إذا (!e)
                                استمر;

                        إذا (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                إذا (r < 0)
                                        أرجع r;

                                لبھیا = true;

                        } وإلا إذا (*e == 0 && FLAGS_سیٹ(flags, PROC_CMDLINE_قدر_OPTIONAL))
                                لبھیا = true;

                } وإلا {
                        إذا (proc_cmdline_کلید_streq(لفظ, کلید)) {
                                لبھیا = true;
                                اكسر; /* we found what we were looking for */
                        }
                }
        }

        إذا (ret_قدر)
                *ret_قدر = TAKE_PTR(v);

        أرجع لبھیا;
}

صحيح proc_cmdline_get_کلید(ثابت حرف *کلید, ProcCmdlineFlags flags, حرف **ret_قدر) {
        _cleanup_strv_free_ حرف **دلیلاں = عدم;
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
        أكد(!FLAGS_سیٹ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        إذا (isempty(کلید))
                أرجع -EINVAL;

        إذا (FLAGS_سیٹ(flags, PROC_CMDLINE_قدر_OPTIONAL) && !ret_قدر)
                أرجع -EINVAL;

        r = proc_cmdline_strv_اندرونی(&دلیلاں, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع cmdline_get_کلید(دلیلاں, کلید, flags, ret_قدر);
}

صحيح proc_cmdline_get_bool(ثابت حرف *کلید, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        أكد(ret);

        r = proc_cmdline_get_کلید(کلید, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_قدر_OPTIONAL, &v);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) { /* key not specified at all */
                *ret = FLAGS_سیٹ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
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

ساكن صحيح cmdline_get_کلید_ap(ProcCmdlineFlags flags, حرف* ثابت* دلیلاں, va_list ap) {
        صحيح r, ret = 0;

        لكل (;;) {
                حرف **v;
                ثابت حرف *k, *e;

                k = va_arg(ap, ثابت حرف*);
                إذا (!k)
                        اكسر;

                assert_se(v = va_arg(ap, حرف**));

                STRV_FOREACH(p, دلیلاں) {
                        ثابت حرف *لفظ;

                        لفظ = mangle_لفظ(*p, flags);
                        إذا (!لفظ)
                                استمر;

                        e = proc_cmdline_کلید_startswith(لفظ, k);
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

صحيح proc_cmdline_get_کلید_many_اندرونی(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ حرف **دلیلاں = عدم;
        صحيح r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_قدر_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&دلیلاں);
        إذا (r < 0)
                أرجع r;

        va_start(ap, flags);
        r = cmdline_get_کلید_ap(flags, دلیلاں, ap);
        va_end(ap);

        أرجع r;
}

bool proc_cmdline_قدر_missing(ثابت حرف *کلید, ثابت حرف *قدر) {
        أكد(کلید);

        إذا (!قدر) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", کلید);
                أرجع true;
        }

        أرجع false;
}
