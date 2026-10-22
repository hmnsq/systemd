/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>

#شامل "alloc-util.h"
#شامل "extract-word.h"
#شامل "fileio.h"
#شامل "initrd-util.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "proc-cmdline.h"
#شامل "process-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "virt.h"

قسمتعریف شمار ArgType {
        no_دلیل,
        required_دلیل,
        optional_دلیل,
} ArgType;

صحیح proc_cmdline_filter_pid1_دلائل(حرف **argv, حرف ***ret) {
        ثابت ساخت {
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

        _cleanup_strv_free_ حرف **filtered = تہی;
        صحیح state, r;

        أكد(argv);
        أكد(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        أكد(!اندر_سیٹ(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_دلیل;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                صحیح پچھلا_state = state;
                ثابت حرف *a = *p;

                /* Reset the state for the next step. */
                state = no_دلیل;

                اگر (پچھلا_state == required_دلیل ||
                    (پچھلا_state == optional_دلیل && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        جاری;

                اگر (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        اگر (r < 0)
                                واپس r;
                        جاری;
                }

                اگر (a[1] == '-') {
                        اگر (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                اگر (r < 0)
                                        واپس r;
                                توڑو;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ثابت حرف *q = startswith(a + 2, option->name);
                                اگر (!q || !اندر_سیٹ(q[0], '=', '\0'))
                                        جاری;

                                /* Found matching option, updating the state if necessary. */
                                اگر (q[0] == '\0' && option->has_arg == required_دلیل)
                                        state = required_دلیل;

                                توڑو;
                        }
                        جاری;
                }

                /* short option(s), e.g. -x or -xyz */
                جب (a && *++a != '\0')
                        برائے (ثابت حرف *q = short_options; *q != '\0'; q++) {
                                اگر (*q != *a)
                                        جاری;

                                /* Found matching short option. */

                                اگر (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_دلیل :
                                                q[2] == ':' ? optional_دلیل : required_دلیل;

                                        a = تہی; /* Not necessary to parse remaining part. */
                                }
                                توڑو;
                        }
        }

        *ret = TAKE_PTR(filtered);
        واپس 0;
}

صحیح proc_cmdline(حرف **ret) {
        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت حرف *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        اگر (e)
                واپس strdup_مقصد(ret, e);

        اگر (detect_container() > 0)
                واپس pid_get_cmdline(1, SIZE_MAX, 0, ret);

        واپس read_بھرا_file("/proc/cmdline", ret, /* ret_size= */  تہی);
}

ساکن صحیح proc_cmdline_strv_اندرونی(حرف ***ret, bool filter_pid1_دلائل) {
        صحیح r;

        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت حرف *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        اگر (e)
                واپس strv_بانٹو_بھرا(ret, e, تہی, نکالو_UNQUOTE|نکالو_RELAX|نکالو_RETAIN_ESCAPE);

        اگر (detect_container() > 0) {
                _cleanup_strv_free_ حرف **دلائل = تہی;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &دلائل);
                اگر (r < 0)
                        واپس r;

                اگر (filter_pid1_دلائل)
                        واپس proc_cmdline_filter_pid1_دلائل(دلائل, ret);

                *ret = TAKE_PTR(دلائل);
                واپس 0;

        } ورنہ {
                _cleanup_free_ حرف *s = تہی;

                r = read_بھرا_file("/proc/cmdline", &s, تہی);
                اگر (r < 0)
                        واپس r;

                واپس strv_بانٹو_بھرا(ret, s, تہی, نکالو_UNQUOTE|نکالو_RELAX|نکالو_RETAIN_ESCAPE);
        }
}

صحیح proc_cmdline_strv(حرف ***ret) {
        واپس proc_cmdline_strv_اندرونی(ret, /* filter_pid1_args= */ false);
}

ساکن حرف *mangle_لفظ(ثابت حرف *لفظ, ProcCmdlineFlags flags) {
        حرف *c = (حرف*) startswith(لفظ, "rd.");
        اگر (c) {
                /* Filter out arguments that are intended only for the initrd */

                اگر (!اندر_initrd())
                        واپس تہی;

                اگر (FLAGS_سیٹ(flags, PROC_CMDLINE_STRIP_RD_سابقہ))
                        واپس c;

        } ورنہ اگر (FLAGS_سیٹ(flags, PROC_CMDLINE_RD_STRICT) && اندر_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                واپس تہی;

        واپس (حرف*) لفظ;
}

#تعریف_کرو mangle_لفظ(لفظ, flags) const_generic(لفظ, mangle_لفظ(لفظ, flags))

ساکن صحیح proc_cmdline_parse_strv(حرف **دلائل, proc_cmdline_parse_t parse_شے, خالی *ڈیٹا, ProcCmdlineFlags flags) {
        صحیح r;

        أكد(parse_شے);

        STRV_FOREACH(لفظ, دلائل) {
                حرف *کلید, *قدر;

                کلید = mangle_لفظ(*لفظ, flags);
                اگر (!کلید)
                        جاری;

                قدر = strchr(کلید, '=');
                اگر (قدر)
                        *(قدر++) = '\0';

                r = parse_شے(کلید, قدر, ڈیٹا);
                اگر (r < 0)
                        واپس r;
        }

        واپس 0;
}

صحیح proc_cmdline_parse(proc_cmdline_parse_t parse_شے, خالی *ڈیٹا, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ حرف **دلائل = تہی;
        صحیح r;

        أكد(parse_شے);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_قدر_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_اندرونی(&دلائل, /* filter_pid1_args= */ true);
        اگر (r < 0)
                واپس r;

        واپس proc_cmdline_parse_strv(دلائل, parse_شے, ڈیٹا, flags);
}

ساکن bool relaxed_equal_char(حرف a, حرف b) {
        واپس a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

حرف* proc_cmdline_کلید_startswith(ثابت حرف *s, ثابت حرف *سابقہ) {
        أكد(s);
        أكد(سابقہ);

        /* Much like startswith(), but considers "-" and "_" the same */

        برائے (; *سابقہ != 0; s++, سابقہ++)
                اگر (!relaxed_equal_char(*s, *سابقہ))
                        واپس تہی;

        واپس (حرف*) s;
}

bool proc_cmdline_کلید_streq(ثابت حرف *x, ثابت حرف *y) {
        أكد(x);
        أكد(y);

        /* Much like streq(), but considers "-" and "_" the same */

        برائے (; *x != 0 || *y != 0; x++, y++)
                اگر (!relaxed_equal_char(*x, *y))
                        واپس false;

        واپس true;
}

ساکن صحیح cmdline_get_کلید(حرف **دلائل, ثابت حرف *کلید, ProcCmdlineFlags flags, حرف **ret_قدر) {
        _cleanup_free_ حرف *v = تہی;
        bool ملا = false;
        صحیح r;

        أكد(کلید);

        STRV_FOREACH(p, دلائل) {
                ثابت حرف *لفظ;

                لفظ = mangle_لفظ(*p, flags);
                اگر (!لفظ)
                        جاری;

                اگر (ret_قدر) {
                        ثابت حرف *e;

                        e = proc_cmdline_کلید_startswith(لفظ, کلید);
                        اگر (!e)
                                جاری;

                        اگر (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                اگر (r < 0)
                                        واپس r;

                                ملا = true;

                        } ورنہ اگر (*e == 0 && FLAGS_سیٹ(flags, PROC_CMDLINE_قدر_OPTIONAL))
                                ملا = true;

                } ورنہ {
                        اگر (proc_cmdline_کلید_streq(لفظ, کلید)) {
                                ملا = true;
                                توڑو; /* we found what we were looking for */
                        }
                }
        }

        اگر (ret_قدر)
                *ret_قدر = TAKE_PTR(v);

        واپس ملا;
}

صحیح proc_cmdline_get_کلید(ثابت حرف *کلید, ProcCmdlineFlags flags, حرف **ret_قدر) {
        _cleanup_strv_free_ حرف **دلائل = تہی;
        صحیح r;

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

        اگر (isempty(کلید))
                واپس -EINVAL;

        اگر (FLAGS_سیٹ(flags, PROC_CMDLINE_قدر_OPTIONAL) && !ret_قدر)
                واپس -EINVAL;

        r = proc_cmdline_strv_اندرونی(&دلائل, /* filter_pid1_args= */ true);
        اگر (r < 0)
                واپس r;

        واپس cmdline_get_کلید(دلائل, کلید, flags, ret_قدر);
}

صحیح proc_cmdline_get_bool(ثابت حرف *کلید, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ حرف *v = تہی;
        صحیح r;

        أكد(ret);

        r = proc_cmdline_get_کلید(کلید, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_قدر_OPTIONAL, &v);
        اگر (r < 0)
                واپس r;
        اگر (r == 0) { /* key not specified at all */
                *ret = FLAGS_سیٹ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                واپس 0;
        }

        اگر (v) { /* key with parameter passed */
                r = parse_boolean(v);
                اگر (r < 0)
                        واپس r;
                *ret = r;
        } ورنہ /* key without parameter passed */
                *ret = true;

        واپس 1;
}

ساکن صحیح cmdline_get_کلید_ap(ProcCmdlineFlags flags, حرف* ثابت* دلائل, va_list ap) {
        صحیح r, ret = 0;

        برائے (;;) {
                حرف **v;
                ثابت حرف *k, *e;

                k = va_arg(ap, ثابت حرف*);
                اگر (!k)
                        توڑو;

                assert_se(v = va_arg(ap, حرف**));

                STRV_FOREACH(p, دلائل) {
                        ثابت حرف *لفظ;

                        لفظ = mangle_لفظ(*p, flags);
                        اگر (!لفظ)
                                جاری;

                        e = proc_cmdline_کلید_startswith(لفظ, k);
                        اگر (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                اگر (r < 0)
                                        واپس r;

                                ret++;
                        }
                }
        }

        واپس ret;
}

صحیح proc_cmdline_get_کلید_many_اندرونی(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ حرف **دلائل = تہی;
        صحیح r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_قدر_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&دلائل);
        اگر (r < 0)
                واپس r;

        va_start(ap, flags);
        r = cmdline_get_کلید_ap(flags, دلائل, ap);
        va_end(ap);

        واپس r;
}

bool proc_cmdline_قدر_missing(ثابت حرف *کلید, ثابت حرف *قدر) {
        أكد(کلید);

        اگر (!قدر) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", کلید);
                واپس true;
        }

        واپس false;
}
