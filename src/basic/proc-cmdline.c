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
        no_آرگومان,
        required_آرگومان,
        optional_آرگومان,
} ArgType;

صحيح proc_cmdline_filter_pid1_آرگومانها(حرف **argv, حرف ***ret) {
        ثابت بنية {
                ثابت حرف *name;
                ArgType has_arg;
        } options[] = {
                { "log-level",                required_آرگومان },
                { "log-target",               required_آرگومان },
                { "log-color",                optional_آرگومان },
                { "log-location",             optional_آرگومان },
                { "log-time",                 optional_آرگومان },
                { "unit",                     required_آرگومان },
                { "system",                   no_آرگومان,      },
                { "user",                     no_آرگومان,      },
                { "test",                     no_آرگومان,      },
                { "no-pager",                 no_آرگومان,      },
                { "help",                     no_آرگومان,      },
                { "version",                  no_آرگومان,      },
                { "introspect-cli",           no_آرگومان,      },
                { "dump-configuration-items", no_آرگومان,      },
                { "dump-bus-properties",      no_آرگومان,      },
                { "bus-introspect",           required_آرگومان },
                { "dump-core",                optional_آرگومان },
                { "crash-chvt",               required_آرگومان },
                { "crash-vt",                 required_آرگومان },
                { "crash-shell",              optional_آرگومان },
                { "crash-reboot",             optional_آرگومان },
                { "crash-action",             required_آرگومان },
                { "confirm-spawn",            optional_آرگومان },
                { "show-status",              optional_آرگومان },
                { "deserialize",              required_آرگومان },
                { "switched-root",            no_آرگومان,      },
                { "default-standard-output",  required_آرگومان },
                { "default-standard-error",   required_آرگومان },
                { "machine-id",               required_آرگومان },
                { "service-watchdogs",        required_آرگومان },
                { "exit-code",                required_آرگومان },
                { "timeout",                  required_آرگومان },
        };
        ثابت حرف *short_options = "hDbsz:";

        _cleanup_strv_free_ حرف **filtered = عدم;
        صحيح state, r;

        أكد(argv);
        أكد(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        أكد(!درون_مجموعه(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_آرگومان;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                صحيح prev_state = state;
                ثابت حرف *a = *p;

                /* Reset the state for the next step. */
                state = no_آرگومان;

                إذا (prev_state == required_آرگومان ||
                    (prev_state == optional_آرگومان && a[0] != '-'))
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
                                إذا (!q || !درون_مجموعه(q[0], '=', '\0'))
                                        استمر;

                                /* Found matching option, updating the state if necessary. */
                                إذا (q[0] == '\0' && option->has_arg == required_آرگومان)
                                        state = required_آرگومان;

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
                                        state = a[1] != '\0' ? no_آرگومان :
                                                q[2] == ':' ? optional_آرگومان : required_آرگومان;

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
                أرجع strdup_مقصد(ret, e);

        إذا (detect_container() > 0)
                أرجع pid_get_cmdline(1, SIZE_MAX, 0, ret);

        أرجع read_پر_file("/proc/cmdline", ret, /* ret_size= */  عدم);
}

ساكن صحيح proc_cmdline_strv_داخلی(حرف ***ret, bool filter_pid1_آرگومانها) {
        صحيح r;

        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت حرف *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        إذا (e)
                أرجع strv_تقسیم_پر(ret, e, عدم, استخراج_UNQUOTE|استخراج_RELAX|استخراج_RETAIN_ESCAPE);

        إذا (detect_container() > 0) {
                _cleanup_strv_free_ حرف **آرگومانها = عدم;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &آرگومانها);
                إذا (r < 0)
                        أرجع r;

                إذا (filter_pid1_آرگومانها)
                        أرجع proc_cmdline_filter_pid1_آرگومانها(آرگومانها, ret);

                *ret = TAKE_PTR(آرگومانها);
                أرجع 0;

        } وإلا {
                _cleanup_free_ حرف *s = عدم;

                r = read_پر_file("/proc/cmdline", &s, عدم);
                إذا (r < 0)
                        أرجع r;

                أرجع strv_تقسیم_پر(ret, s, عدم, استخراج_UNQUOTE|استخراج_RELAX|استخراج_RETAIN_ESCAPE);
        }
}

صحيح proc_cmdline_strv(حرف ***ret) {
        أرجع proc_cmdline_strv_داخلی(ret, /* filter_pid1_args= */ false);
}

ساكن حرف *mangle_کلمه(ثابت حرف *کلمه, ProcCmdlineFlags flags) {
        حرف *c = (حرف*) startswith(کلمه, "rd.");
        إذا (c) {
                /* Filter out arguments that are intended only for the initrd */

                إذا (!درون_initrd())
                        أرجع عدم;

                إذا (FLAGS_مجموعه(flags, PROC_CMDLINE_STRIP_RD_پیشوند))
                        أرجع c;

        } وإلا إذا (FLAGS_مجموعه(flags, PROC_CMDLINE_RD_STRICT) && درون_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                أرجع عدم;

        أرجع (حرف*) کلمه;
}

#تعریف_کن mangle_کلمه(کلمه, flags) const_generic(کلمه, mangle_کلمه(کلمه, flags))

ساكن صحيح proc_cmdline_parse_strv(حرف **آرگومانها, proc_cmdline_parse_t parse_قلم, فراغ *داده, ProcCmdlineFlags flags) {
        صحيح r;

        أكد(parse_قلم);

        STRV_FOREACH(کلمه, آرگومانها) {
                حرف *کلید, *مقدار;

                کلید = mangle_کلمه(*کلمه, flags);
                إذا (!کلید)
                        استمر;

                مقدار = strchr(کلید, '=');
                إذا (مقدار)
                        *(مقدار++) = '\0';

                r = parse_قلم(کلید, مقدار, داده);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح proc_cmdline_parse(proc_cmdline_parse_t parse_قلم, فراغ *داده, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ حرف **آرگومانها = عدم;
        صحيح r;

        أكد(parse_قلم);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_مقدار_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_داخلی(&آرگومانها, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع proc_cmdline_parse_strv(آرگومانها, parse_قلم, داده, flags);
}

ساكن bool relaxed_equal_char(حرف a, حرف b) {
        أرجع a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

حرف* proc_cmdline_کلید_startswith(ثابت حرف *s, ثابت حرف *پیشوند) {
        أكد(s);
        أكد(پیشوند);

        /* Much like startswith(), but considers "-" and "_" the same */

        لكل (; *پیشوند != 0; s++, پیشوند++)
                إذا (!relaxed_equal_char(*s, *پیشوند))
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

ساكن صحيح cmdline_get_کلید(حرف **آرگومانها, ثابت حرف *کلید, ProcCmdlineFlags flags, حرف **ret_مقدار) {
        _cleanup_free_ حرف *v = عدم;
        bool یافت = false;
        صحيح r;

        أكد(کلید);

        STRV_FOREACH(p, آرگومانها) {
                ثابت حرف *کلمه;

                کلمه = mangle_کلمه(*p, flags);
                إذا (!کلمه)
                        استمر;

                إذا (ret_مقدار) {
                        ثابت حرف *e;

                        e = proc_cmdline_کلید_startswith(کلمه, کلید);
                        إذا (!e)
                                استمر;

                        إذا (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                إذا (r < 0)
                                        أرجع r;

                                یافت = true;

                        } وإلا إذا (*e == 0 && FLAGS_مجموعه(flags, PROC_CMDLINE_مقدار_OPTIONAL))
                                یافت = true;

                } وإلا {
                        إذا (proc_cmdline_کلید_streq(کلمه, کلید)) {
                                یافت = true;
                                اكسر; /* we found what we were looking for */
                        }
                }
        }

        إذا (ret_مقدار)
                *ret_مقدار = TAKE_PTR(v);

        أرجع یافت;
}

صحيح proc_cmdline_get_کلید(ثابت حرف *کلید, ProcCmdlineFlags flags, حرف **ret_مقدار) {
        _cleanup_strv_free_ حرف **آرگومانها = عدم;
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
        أكد(!FLAGS_مجموعه(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        إذا (isempty(کلید))
                أرجع -EINVAL;

        إذا (FLAGS_مجموعه(flags, PROC_CMDLINE_مقدار_OPTIONAL) && !ret_مقدار)
                أرجع -EINVAL;

        r = proc_cmdline_strv_داخلی(&آرگومانها, /* filter_pid1_args= */ true);
        إذا (r < 0)
                أرجع r;

        أرجع cmdline_get_کلید(آرگومانها, کلید, flags, ret_مقدار);
}

صحيح proc_cmdline_get_bool(ثابت حرف *کلید, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        أكد(ret);

        r = proc_cmdline_get_کلید(کلید, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_مقدار_OPTIONAL, &v);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) { /* key not specified at all */
                *ret = FLAGS_مجموعه(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
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

ساكن صحيح cmdline_get_کلید_ap(ProcCmdlineFlags flags, حرف* ثابت* آرگومانها, va_list ap) {
        صحيح r, ret = 0;

        لكل (;;) {
                حرف **v;
                ثابت حرف *k, *e;

                k = va_arg(ap, ثابت حرف*);
                إذا (!k)
                        اكسر;

                assert_se(v = va_arg(ap, حرف**));

                STRV_FOREACH(p, آرگومانها) {
                        ثابت حرف *کلمه;

                        کلمه = mangle_کلمه(*p, flags);
                        إذا (!کلمه)
                                استمر;

                        e = proc_cmdline_کلید_startswith(کلمه, k);
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

صحيح proc_cmdline_get_کلید_many_داخلی(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ حرف **آرگومانها = عدم;
        صحيح r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_مقدار_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&آرگومانها);
        إذا (r < 0)
                أرجع r;

        va_start(ap, flags);
        r = cmdline_get_کلید_ap(flags, آرگومانها, ap);
        va_end(ap);

        أرجع r;
}

bool proc_cmdline_مقدار_missing(ثابت حرف *کلید, ثابت حرف *مقدار) {
        أكد(کلید);

        إذا (!مقدار) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", کلید);
                أرجع true;
        }

        أرجع false;
}
