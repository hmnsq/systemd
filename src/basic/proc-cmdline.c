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

صحيح proc_cmdline_filter_pid1_دلیلونه(حرف **argv, حرف ***ret) {
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
        أكد(!دننه_سېټ(short_options[0], '-', '+', ':'));

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
                                إذا (!q || !دننه_سېټ(q[0], '=', '\0'))
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

        أرجع read_ډک_file("/proc/cmdline", ret, /* ret_size= */  عدم);
}

ساكن صحيح proc_cmdline_strv_دننی(حرف ***ret, bool filter_pid1_دلیلونه) {
        صحيح r;

        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت حرف *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        إذا (e)
                أرجع strv_وېشل_ډک(ret, e, عدم, ایستل_UNQUOTE|ایستل_RELAX|ایستل_RETAIN_ESCAPE);

        إذا (detect_container() > 0) {
                _cleanup_strv_free_ حرف **دلیلونه = عدم;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &دلیلونه);
                إذا (r < 0)
                        أرجع r;

                إذا (filter_pid1_دلیلونه)
                        أرجع proc_cmdline_filter_pid1_دلیلونه(دلیلونه, ret);

                *ret = TAKE_PTR(دلیلونه);
                أرجع 0;

        } وإلا {
                _cleanup_free_ حرف *s = عدم;

                r = read_ډک_file("/proc/cmdline", &s, عدم);
                إذا (r < 0)
                        أرجع r;

                أرجع strv_وېشل_ډک(ret, s, عدم, ایستل_UNQUOTE|ایستل_RELAX|ایستل_RETAIN_ESCAPE);
        }
}

صحيح proc_cmdline_strv(حرف ***ret) {
        أرجع proc_cmdline_strv_دننی(ret, /* filter_pid1_args= */ false);
}

ساكن حرف *mangle_کلمه(ثابت حرف *کلمه, ProcCmdlineFlags flags) {
        حرف *c = (حرف*) startswith(کلمه, "rd.");
        إذا (c) {
                /* Filter out arguments that are intended only for the initrd */

                إذا (!دننه_initrd())
                        أرجع عدم;

                إذا (FLAGS_سېټ(flags, PROC_CMDLINE_STRIP_RD_مختاړی))
                        أرجع c;

        } وإلا إذا (FLAGS_سېټ(flags, PROC_CMDLINE_RD_STRICT) && دننه_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                أرجع عدم;

        أرجع (حرف*) کلمه;
}

#تعریف_کړه mangle_کلمه(کلمه, flags) const_generic(کلمه, mangle_کلمه(کلمه, flags))

ساكن صحيح proc_cmdline_parse_strv(حرف **دلیلونه, proc_cmdline_parse_t parse_توکی, فراغ *معلومات, ProcCmdlineFlags flags) {
        صحيح r;

        أكد(parse_توکی);

        STRV_FOREACH(کلمه, دلیلونه) {
                حرف *کلی, *ارزښت;

                کلی = mangle_کلمه(*کلمه, flags);
                إذا (!کلی)
                        استمر;

                ارزښت = strchr(کلی, '=');
                إذا (ارزښت)
                        *(ارزښت++) = '\0';

                r = parse_توکی(کلی, ارزښت, معلومات);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح proc_cmdline_parse(proc_cmdline_parse_t parse_توکی, فراغ *معلومات, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ حرف **دلیلونه = عدم;
        صحيح r;

        أكد(parse_توکی);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_ارزښت_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_دننی(&دلیلونه, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع proc_cmdline_parse_strv(دلیلونه, parse_توکی, معلومات, flags);
}

ساكن bool relaxed_equal_char(حرف a, حرف b) {
        أرجع a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

حرف* proc_cmdline_کلی_startswith(ثابت حرف *s, ثابت حرف *مختاړی) {
        أكد(s);
        أكد(مختاړی);

        /* Much like startswith(), but considers "-" and "_" the same */

        لكل (; *مختاړی != 0; s++, مختاړی++)
                إذا (!relaxed_equal_char(*s, *مختاړی))
                        أرجع عدم;

        أرجع (حرف*) s;
}

bool proc_cmdline_کلی_streq(ثابت حرف *x, ثابت حرف *y) {
        أكد(x);
        أكد(y);

        /* Much like streq(), but considers "-" and "_" the same */

        لكل (; *x != 0 || *y != 0; x++, y++)
                إذا (!relaxed_equal_char(*x, *y))
                        أرجع false;

        أرجع true;
}

ساكن صحيح cmdline_get_کلی(حرف **دلیلونه, ثابت حرف *کلی, ProcCmdlineFlags flags, حرف **ret_ارزښت) {
        _cleanup_free_ حرف *v = عدم;
        bool وموندل = false;
        صحيح r;

        أكد(کلی);

        STRV_FOREACH(p, دلیلونه) {
                ثابت حرف *کلمه;

                کلمه = mangle_کلمه(*p, flags);
                إذا (!کلمه)
                        استمر;

                إذا (ret_ارزښت) {
                        ثابت حرف *e;

                        e = proc_cmdline_کلی_startswith(کلمه, کلی);
                        إذا (!e)
                                استمر;

                        إذا (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                إذا (r < 0)
                                        أرجع r;

                                وموندل = true;

                        } وإلا إذا (*e == 0 && FLAGS_سېټ(flags, PROC_CMDLINE_ارزښت_OPTIONAL))
                                وموندل = true;

                } وإلا {
                        إذا (proc_cmdline_کلی_streq(کلمه, کلی)) {
                                وموندل = true;
                                اكسر; /* we found what we were looking for */
                        }
                }
        }

        إذا (ret_ارزښت)
                *ret_ارزښت = TAKE_PTR(v);

        أرجع وموندل;
}

صحيح proc_cmdline_get_کلی(ثابت حرف *کلی, ProcCmdlineFlags flags, حرف **ret_ارزښت) {
        _cleanup_strv_free_ حرف **دلیلونه = عدم;
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
        أكد(!FLAGS_سېټ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        إذا (isempty(کلی))
                أرجع -EINVAL;

        إذا (FLAGS_سېټ(flags, PROC_CMDLINE_ارزښت_OPTIONAL) && !ret_ارزښت)
                أرجع -EINVAL;

        r = proc_cmdline_strv_دننی(&دلیلونه, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع cmdline_get_کلی(دلیلونه, کلی, flags, ret_ارزښت);
}

صحيح proc_cmdline_get_bool(ثابت حرف *کلی, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        أكد(ret);

        r = proc_cmdline_get_کلی(کلی, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_ارزښت_OPTIONAL, &v);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) { /* key not specified at all */
                *ret = FLAGS_سېټ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
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

ساكن صحيح cmdline_get_کلی_ap(ProcCmdlineFlags flags, حرف* ثابت* دلیلونه, va_list ap) {
        صحيح r, ret = 0;

        لكل (;;) {
                حرف **v;
                ثابت حرف *k, *e;

                k = va_arg(ap, ثابت حرف*);
                إذا (!k)
                        اكسر;

                assert_se(v = va_arg(ap, حرف**));

                STRV_FOREACH(p, دلیلونه) {
                        ثابت حرف *کلمه;

                        کلمه = mangle_کلمه(*p, flags);
                        إذا (!کلمه)
                                استمر;

                        e = proc_cmdline_کلی_startswith(کلمه, k);
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

صحيح proc_cmdline_get_کلی_many_دننی(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ حرف **دلیلونه = عدم;
        صحيح r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_ارزښت_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&دلیلونه);
        إذا (r < 0)
                أرجع r;

        va_start(ap, flags);
        r = cmdline_get_کلی_ap(flags, دلیلونه, ap);
        va_end(ap);

        أرجع r;
}

bool proc_cmdline_ارزښت_missing(ثابت حرف *کلی, ثابت حرف *ارزښت) {
        أكد(کلی);

        إذا (!ارزښت) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", کلی);
                أرجع true;
        }

        أرجع false;
}
