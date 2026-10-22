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

تعریفنوع شمارش ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

صحیح proc_cmdline_filter_pid1_args(نویسه **argv, نویسه ***ret) {
        ثابت ساختار {
                ثابت نویسه *name;
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
        ثابت نویسه *short_options = "hDbsz:";

        _cleanup_strv_free_ نویسه **filtered = پوچ;
        صحیح state, r;

        أكد(argv);
        أكد(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        أكد(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                صحیح prev_state = state;
                ثابت نویسه *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                اگر (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        ادامه;

                اگر (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        اگر (r < 0)
                                بازگشت r;
                        ادامه;
                }

                اگر (a[1] == '-') {
                        اگر (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                اگر (r < 0)
                                        بازگشت r;
                                بشکن;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ثابت نویسه *q = startswith(a + 2, option->name);
                                اگر (!q || !IN_SET(q[0], '=', '\0'))
                                        ادامه;

                                /* Found matching option, updating the state if necessary. */
                                اگر (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                بشکن;
                        }
                        ادامه;
                }

                /* short option(s), e.g. -x or -xyz */
                وقتی (a && *++a != '\0')
                        برای (ثابت نویسه *q = short_options; *q != '\0'; q++) {
                                اگر (*q != *a)
                                        ادامه;

                                /* Found matching short option. */

                                اگر (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = پوچ; /* Not necessary to parse remaining part. */
                                }
                                بشکن;
                        }
        }

        *ret = TAKE_PTR(filtered);
        بازگشت 0;
}

صحیح proc_cmdline(نویسه **ret) {
        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت نویسه *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        اگر (e)
                بازگشت strdup_to(ret, e);

        اگر (detect_container() > 0)
                بازگشت pid_get_cmdline(1, SIZE_MAX, 0, ret);

        بازگشت read_full_file("/proc/cmdline", ret, /* ret_size= */  پوچ);
}

ایستا صحیح proc_cmdline_strv_internal(نویسه ***ret, bool filter_pid1_args) {
        صحیح r;

        أكد(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ثابت نویسه *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        اگر (e)
                بازگشت strv_split_full(ret, e, پوچ, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        اگر (detect_container() > 0) {
                _cleanup_strv_free_ نویسه **args = پوچ;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                اگر (r < 0)
                        بازگشت r;

                اگر (filter_pid1_args)
                        بازگشت proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                بازگشت 0;

        } وگرنه {
                _cleanup_free_ نویسه *s = پوچ;

                r = read_full_file("/proc/cmdline", &s, پوچ);
                اگر (r < 0)
                        بازگشت r;

                بازگشت strv_split_full(ret, s, پوچ, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

صحیح proc_cmdline_strv(نویسه ***ret) {
        بازگشت proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

ایستا نویسه *mangle_word(ثابت نویسه *word, ProcCmdlineFlags flags) {
        نویسه *c = (نویسه*) startswith(word, "rd.");
        اگر (c) {
                /* Filter out arguments that are intended only for the initrd */

                اگر (!in_initrd())
                        بازگشت پوچ;

                اگر (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        بازگشت c;

        } وگرنه اگر (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                بازگشت پوچ;

        بازگشت (نویسه*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

ایستا صحیح proc_cmdline_parse_strv(نویسه **args, proc_cmdline_parse_t parse_item, تهی *data, ProcCmdlineFlags flags) {
        صحیح r;

        أكد(parse_item);

        STRV_FOREACH(word, args) {
                نویسه *کلید, *مقدار;

                کلید = mangle_word(*word, flags);
                اگر (!کلید)
                        ادامه;

                مقدار = strchr(کلید, '=');
                اگر (مقدار)
                        *(مقدار++) = '\0';

                r = parse_item(کلید, مقدار, data);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت 0;
}

صحیح proc_cmdline_parse(proc_cmdline_parse_t parse_item, تهی *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ نویسه **args = پوچ;
        صحیح r;

        أكد(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_مقدار_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        اگر (r < 0)
                بازگشت r;

        بازگشت proc_cmdline_parse_strv(args, parse_item, data, flags);
}

ایستا bool relaxed_equal_char(نویسه a, نویسه b) {
        بازگشت a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

نویسه* proc_cmdline_کلید_startswith(ثابت نویسه *s, ثابت نویسه *prefix) {
        أكد(s);
        أكد(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        برای (; *prefix != 0; s++, prefix++)
                اگر (!relaxed_equal_char(*s, *prefix))
                        بازگشت پوچ;

        بازگشت (نویسه*) s;
}

bool proc_cmdline_کلید_streq(ثابت نویسه *x, ثابت نویسه *y) {
        أكد(x);
        أكد(y);

        /* Much like streq(), but considers "-" and "_" the same */

        برای (; *x != 0 || *y != 0; x++, y++)
                اگر (!relaxed_equal_char(*x, *y))
                        بازگشت false;

        بازگشت true;
}

ایستا صحیح cmdline_get_کلید(نویسه **args, ثابت نویسه *کلید, ProcCmdlineFlags flags, نویسه **ret_مقدار) {
        _cleanup_free_ نویسه *v = پوچ;
        bool found = false;
        صحیح r;

        أكد(کلید);

        STRV_FOREACH(p, args) {
                ثابت نویسه *word;

                word = mangle_word(*p, flags);
                اگر (!word)
                        ادامه;

                اگر (ret_مقدار) {
                        ثابت نویسه *e;

                        e = proc_cmdline_کلید_startswith(word, کلید);
                        اگر (!e)
                                ادامه;

                        اگر (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                اگر (r < 0)
                                        بازگشت r;

                                found = true;

                        } وگرنه اگر (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_مقدار_OPTIONAL))
                                found = true;

                } وگرنه {
                        اگر (proc_cmdline_کلید_streq(word, کلید)) {
                                found = true;
                                بشکن; /* we found what we were looking for */
                        }
                }
        }

        اگر (ret_مقدار)
                *ret_مقدار = TAKE_PTR(v);

        بازگشت found;
}

صحیح proc_cmdline_get_کلید(ثابت نویسه *کلید, ProcCmdlineFlags flags, نویسه **ret_مقدار) {
        _cleanup_strv_free_ نویسه **args = پوچ;
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
        أكد(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        اگر (isempty(کلید))
                بازگشت -EINVAL;

        اگر (FLAGS_SET(flags, PROC_CMDLINE_مقدار_OPTIONAL) && !ret_مقدار)
                بازگشت -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        اگر (r < 0)
                بازگشت r;

        بازگشت cmdline_get_کلید(args, کلید, flags, ret_مقدار);
}

صحیح proc_cmdline_get_bool(ثابت نویسه *کلید, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ نویسه *v = پوچ;
        صحیح r;

        أكد(ret);

        r = proc_cmdline_get_کلید(کلید, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_مقدار_OPTIONAL, &v);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                بازگشت 0;
        }

        اگر (v) { /* key with parameter passed */
                r = parse_boolean(v);
                اگر (r < 0)
                        بازگشت r;
                *ret = r;
        } وگرنه /* key without parameter passed */
                *ret = true;

        بازگشت 1;
}

ایستا صحیح cmdline_get_کلید_ap(ProcCmdlineFlags flags, نویسه* ثابت* args, va_list ap) {
        صحیح r, ret = 0;

        برای (;;) {
                نویسه **v;
                ثابت نویسه *k, *e;

                k = va_arg(ap, ثابت نویسه*);
                اگر (!k)
                        بشکن;

                assert_se(v = va_arg(ap, نویسه**));

                STRV_FOREACH(p, args) {
                        ثابت نویسه *word;

                        word = mangle_word(*p, flags);
                        اگر (!word)
                                ادامه;

                        e = proc_cmdline_کلید_startswith(word, k);
                        اگر (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                اگر (r < 0)
                                        بازگشت r;

                                ret++;
                        }
                }
        }

        بازگشت ret;
}

صحیح proc_cmdline_get_کلید_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ نویسه **args = پوچ;
        صحیح r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        أكد(!(flags & (PROC_CMDLINE_مقدار_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        اگر (r < 0)
                بازگشت r;

        va_start(ap, flags);
        r = cmdline_get_کلید_ap(flags, args, ap);
        va_end(ap);

        بازگشت r;
}

bool proc_cmdline_مقدار_missing(ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        أكد(کلید);

        اگر (!مقدار) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", کلید);
                بازگشت true;
        }

        بازگشت false;
}
