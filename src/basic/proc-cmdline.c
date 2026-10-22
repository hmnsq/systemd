/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "initrd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc-cmdline.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "virt.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ArgType {
        no_ਦਲੀਲ,
        required_ਦਲੀਲ,
        optional_ਦਲੀਲ,
} ArgType;

ਪੂਰਨ proc_cmdline_filter_pid1_ਦਲੀਲਾਂ(ਅੱਖਰ **argv, ਅੱਖਰ ***ret) {
        ਅਚਲ ਬਣਤਰ {
                ਅਚਲ ਅੱਖਰ *name;
                ArgType has_arg;
        } options[] = {
                { "log-level",                required_ਦਲੀਲ },
                { "log-target",               required_ਦਲੀਲ },
                { "log-color",                optional_ਦਲੀਲ },
                { "log-location",             optional_ਦਲੀਲ },
                { "log-time",                 optional_ਦਲੀਲ },
                { "unit",                     required_ਦਲੀਲ },
                { "system",                   no_ਦਲੀਲ,      },
                { "user",                     no_ਦਲੀਲ,      },
                { "test",                     no_ਦਲੀਲ,      },
                { "no-pager",                 no_ਦਲੀਲ,      },
                { "help",                     no_ਦਲੀਲ,      },
                { "version",                  no_ਦਲੀਲ,      },
                { "introspect-cli",           no_ਦਲੀਲ,      },
                { "dump-configuration-items", no_ਦਲੀਲ,      },
                { "dump-bus-properties",      no_ਦਲੀਲ,      },
                { "bus-introspect",           required_ਦਲੀਲ },
                { "dump-core",                optional_ਦਲੀਲ },
                { "crash-chvt",               required_ਦਲੀਲ },
                { "crash-vt",                 required_ਦਲੀਲ },
                { "crash-shell",              optional_ਦਲੀਲ },
                { "crash-reboot",             optional_ਦਲੀਲ },
                { "crash-action",             required_ਦਲੀਲ },
                { "confirm-spawn",            optional_ਦਲੀਲ },
                { "show-status",              optional_ਦਲੀਲ },
                { "deserialize",              required_ਦਲੀਲ },
                { "switched-root",            no_ਦਲੀਲ,      },
                { "default-standard-output",  required_ਦਲੀਲ },
                { "default-standard-error",   required_ਦਲੀਲ },
                { "machine-id",               required_ਦਲੀਲ },
                { "service-watchdogs",        required_ਦਲੀਲ },
                { "exit-code",                required_ਦਲੀਲ },
                { "timeout",                  required_ਦਲੀਲ },
        };
        ਅਚਲ ਅੱਖਰ *short_options = "hDbsz:";

        _cleanup_strv_free_ ਅੱਖਰ **filtered = ਨਿਰਾ;
        ਪੂਰਨ state, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(argv);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        ਪ੍ਰਤਿਜ੍ਞਾ(!ਵਿੱਚ_ਸੈੱਟ(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_ਦਲੀਲ;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                ਪੂਰਨ prev_state = state;
                ਅਚਲ ਅੱਖਰ *a = *p;

                /* Reset the state for the next step. */
                state = no_ਦਲੀਲ;

                ਜੇ (prev_state == required_ਦਲੀਲ ||
                    (prev_state == optional_ਦਲੀਲ && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        ਜਾਰੀ;

                ਜੇ (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜਾਰੀ;
                }

                ਜੇ (a[1] == '-') {
                        ਜੇ (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                                ਰੋਕੋ;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                ਅਚਲ ਅੱਖਰ *q = startswith(a + 2, option->name);
                                ਜੇ (!q || !ਵਿੱਚ_ਸੈੱਟ(q[0], '=', '\0'))
                                        ਜਾਰੀ;

                                /* Found matching option, updating the state if necessary. */
                                ਜੇ (q[0] == '\0' && option->has_arg == required_ਦਲੀਲ)
                                        state = required_ਦਲੀਲ;

                                ਰੋਕੋ;
                        }
                        ਜਾਰੀ;
                }

                /* short option(s), e.g. -x or -xyz */
                ਜਦੋਂ (a && *++a != '\0')
                        ਲਈ (ਅਚਲ ਅੱਖਰ *q = short_options; *q != '\0'; q++) {
                                ਜੇ (*q != *a)
                                        ਜਾਰੀ;

                                /* Found matching short option. */

                                ਜੇ (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_ਦਲੀਲ :
                                                q[2] == ':' ? optional_ਦਲੀਲ : required_ਦਲੀਲ;

                                        a = ਨਿਰਾ; /* Not necessary to parse remaining part. */
                                }
                                ਰੋਕੋ;
                        }
        }

        *ret = TAKE_PTR(filtered);
        ਵਾਪਸ 0;
}

ਪੂਰਨ proc_cmdline(ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ਅਚਲ ਅੱਖਰ *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ਜੇ (e)
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, e);

        ਜੇ (detect_container() > 0)
                ਵਾਪਸ pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ਵਾਪਸ read_ਭਰਿਆ_file("/proc/cmdline", ret, /* ret_size= */  ਨਿਰਾ);
}

ਸਥਿਰ ਪੂਰਨ proc_cmdline_strv_ਅੰਦਰੂਨੀ(ਅੱਖਰ ***ret, bool filter_pid1_ਦਲੀਲਾਂ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        ਅਚਲ ਅੱਖਰ *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ਜੇ (e)
                ਵਾਪਸ strv_ਵੰਡੋ_ਭਰਿਆ(ret, e, ਨਿਰਾ, ਕੱਢੋ_UNQUOTE|ਕੱਢੋ_RELAX|ਕੱਢੋ_RETAIN_ESCAPE);

        ਜੇ (detect_container() > 0) {
                _cleanup_strv_free_ ਅੱਖਰ **ਦਲੀਲਾਂ = ਨਿਰਾ;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &ਦਲੀਲਾਂ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (filter_pid1_ਦਲੀਲਾਂ)
                        ਵਾਪਸ proc_cmdline_filter_pid1_ਦਲੀਲਾਂ(ਦਲੀਲਾਂ, ret);

                *ret = TAKE_PTR(ਦਲੀਲਾਂ);
                ਵਾਪਸ 0;

        } ਨਹੀਂਤਾਂ {
                _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;

                r = read_ਭਰਿਆ_file("/proc/cmdline", &s, ਨਿਰਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਵਾਪਸ strv_ਵੰਡੋ_ਭਰਿਆ(ret, s, ਨਿਰਾ, ਕੱਢੋ_UNQUOTE|ਕੱਢੋ_RELAX|ਕੱਢੋ_RETAIN_ESCAPE);
        }
}

ਪੂਰਨ proc_cmdline_strv(ਅੱਖਰ ***ret) {
        ਵਾਪਸ proc_cmdline_strv_ਅੰਦਰੂਨੀ(ret, /* filter_pid1_args= */ false);
}

ਸਥਿਰ ਅੱਖਰ *mangle_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ *ਸ਼ਬਦ, ProcCmdlineFlags flags) {
        ਅੱਖਰ *c = (ਅੱਖਰ*) startswith(ਸ਼ਬਦ, "rd.");
        ਜੇ (c) {
                /* Filter out arguments that are intended only for the initrd */

                ਜੇ (!ਵਿੱਚ_initrd())
                        ਵਾਪਸ ਨਿਰਾ;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, PROC_CMDLINE_STRIP_RD_ਅਗੇਤਰ))
                        ਵਾਪਸ c;

        } ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(flags, PROC_CMDLINE_RD_STRICT) && ਵਿੱਚ_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ (ਅੱਖਰ*) ਸ਼ਬਦ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mangle_ਸ਼ਬਦ(ਸ਼ਬਦ, flags) const_generic(ਸ਼ਬਦ, mangle_ਸ਼ਬਦ(ਸ਼ਬਦ, flags))

ਸਥਿਰ ਪੂਰਨ proc_cmdline_parse_strv(ਅੱਖਰ **ਦਲੀਲਾਂ, proc_cmdline_parse_t parse_ਵਸਤੂ, ਖਾਲੀ *ਅੰਕੜੇ, ProcCmdlineFlags flags) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(parse_ਵਸਤੂ);

        STRV_FOREACH(ਸ਼ਬਦ, ਦਲੀਲਾਂ) {
                ਅੱਖਰ *ਕੁੰਜੀ, *ਮੁੱਲ;

                ਕੁੰਜੀ = mangle_ਸ਼ਬਦ(*ਸ਼ਬਦ, flags);
                ਜੇ (!ਕੁੰਜੀ)
                        ਜਾਰੀ;

                ਮੁੱਲ = strchr(ਕੁੰਜੀ, '=');
                ਜੇ (ਮੁੱਲ)
                        *(ਮੁੱਲ++) = '\0';

                r = parse_ਵਸਤੂ(ਕੁੰਜੀ, ਮੁੱਲ, ਅੰਕੜੇ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ proc_cmdline_parse(proc_cmdline_parse_t parse_ਵਸਤੂ, ਖਾਲੀ *ਅੰਕੜੇ, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ ਅੱਖਰ **ਦਲੀਲਾਂ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(parse_ਵਸਤੂ);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        ਪ੍ਰਤਿਜ੍ਞਾ(!(flags & (PROC_CMDLINE_ਮੁੱਲ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_ਅੰਦਰੂਨੀ(&ਦਲੀਲਾਂ, /* filter_pid1_args= */ true);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ proc_cmdline_parse_strv(ਦਲੀਲਾਂ, parse_ਵਸਤੂ, ਅੰਕੜੇ, flags);
}

ਸਥਿਰ bool relaxed_equal_char(ਅੱਖਰ a, ਅੱਖਰ b) {
        ਵਾਪਸ a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

ਅੱਖਰ* proc_cmdline_ਕੁੰਜੀ_startswith(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

        /* Much like startswith(), but considers "-" and "_" the same */

        ਲਈ (; *ਅਗੇਤਰ != 0; s++, ਅਗੇਤਰ++)
                ਜੇ (!relaxed_equal_char(*s, *ਅਗੇਤਰ))
                        ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ (ਅੱਖਰ*) s;
}

bool proc_cmdline_ਕੁੰਜੀ_streq(ਅਚਲ ਅੱਖਰ *x, ਅਚਲ ਅੱਖਰ *y) {
        ਪ੍ਰਤਿਜ੍ਞਾ(x);
        ਪ੍ਰਤਿਜ੍ਞਾ(y);

        /* Much like streq(), but considers "-" and "_" the same */

        ਲਈ (; *x != 0 || *y != 0; x++, y++)
                ਜੇ (!relaxed_equal_char(*x, *y))
                        ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਸਥਿਰ ਪੂਰਨ cmdline_get_ਕੁੰਜੀ(ਅੱਖਰ **ਦਲੀਲਾਂ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ProcCmdlineFlags flags, ਅੱਖਰ **ret_ਮੁੱਲ) {
        _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;
        bool ਮਿਲਿਆ = false;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        STRV_FOREACH(p, ਦਲੀਲਾਂ) {
                ਅਚਲ ਅੱਖਰ *ਸ਼ਬਦ;

                ਸ਼ਬਦ = mangle_ਸ਼ਬਦ(*p, flags);
                ਜੇ (!ਸ਼ਬਦ)
                        ਜਾਰੀ;

                ਜੇ (ret_ਮੁੱਲ) {
                        ਅਚਲ ਅੱਖਰ *e;

                        e = proc_cmdline_ਕੁੰਜੀ_startswith(ਸ਼ਬਦ, ਕੁੰਜੀ);
                        ਜੇ (!e)
                                ਜਾਰੀ;

                        ਜੇ (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ਮਿਲਿਆ = true;

                        } ਨਹੀਂਤਾਂ ਜੇ (*e == 0 && FLAGS_ਸੈੱਟ(flags, PROC_CMDLINE_ਮੁੱਲ_OPTIONAL))
                                ਮਿਲਿਆ = true;

                } ਨਹੀਂਤਾਂ {
                        ਜੇ (proc_cmdline_ਕੁੰਜੀ_streq(ਸ਼ਬਦ, ਕੁੰਜੀ)) {
                                ਮਿਲਿਆ = true;
                                ਰੋਕੋ; /* we found what we were looking for */
                        }
                }
        }

        ਜੇ (ret_ਮੁੱਲ)
                *ret_ਮੁੱਲ = TAKE_PTR(v);

        ਵਾਪਸ ਮਿਲਿਆ;
}

ਪੂਰਨ proc_cmdline_get_ਕੁੰਜੀ(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ProcCmdlineFlags flags, ਅੱਖਰ **ret_ਮੁੱਲ) {
        _cleanup_strv_free_ ਅੱਖਰ **ਦਲੀਲਾਂ = ਨਿਰਾ;
        ਪੂਰਨ r;

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
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        ਜੇ (isempty(ਕੁੰਜੀ))
                ਵਾਪਸ -EINVAL;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, PROC_CMDLINE_ਮੁੱਲ_OPTIONAL) && !ret_ਮੁੱਲ)
                ਵਾਪਸ -EINVAL;

        r = proc_cmdline_strv_ਅੰਦਰੂਨੀ(&ਦਲੀਲਾਂ, /* filter_pid1_args= */ true);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ cmdline_get_ਕੁੰਜੀ(ਦਲੀਲਾਂ, ਕੁੰਜੀ, flags, ret_ਮੁੱਲ);
}

ਪੂਰਨ proc_cmdline_get_bool(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = proc_cmdline_get_ਕੁੰਜੀ(ਕੁੰਜੀ, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_ਮੁੱਲ_OPTIONAL, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0) { /* key not specified at all */
                *ret = FLAGS_ਸੈੱਟ(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ਵਾਪਸ 0;
        }

        ਜੇ (v) { /* key with parameter passed */
                r = parse_boolean(v);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                *ret = r;
        } ਨਹੀਂਤਾਂ /* key without parameter passed */
                *ret = true;

        ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ cmdline_get_ਕੁੰਜੀ_ap(ProcCmdlineFlags flags, ਅੱਖਰ* ਅਚਲ* ਦਲੀਲਾਂ, va_list ap) {
        ਪੂਰਨ r, ret = 0;

        ਲਈ (;;) {
                ਅੱਖਰ **v;
                ਅਚਲ ਅੱਖਰ *k, *e;

                k = va_arg(ap, ਅਚਲ ਅੱਖਰ*);
                ਜੇ (!k)
                        ਰੋਕੋ;

                assert_se(v = va_arg(ap, ਅੱਖਰ**));

                STRV_FOREACH(p, ਦਲੀਲਾਂ) {
                        ਅਚਲ ਅੱਖਰ *ਸ਼ਬਦ;

                        ਸ਼ਬਦ = mangle_ਸ਼ਬਦ(*p, flags);
                        ਜੇ (!ਸ਼ਬਦ)
                                ਜਾਰੀ;

                        e = proc_cmdline_ਕੁੰਜੀ_startswith(ਸ਼ਬਦ, k);
                        ਜੇ (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ret++;
                        }
                }
        }

        ਵਾਪਸ ret;
}

ਪੂਰਨ proc_cmdline_get_ਕੁੰਜੀ_many_ਅੰਦਰੂਨੀ(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ ਅੱਖਰ **ਦਲੀਲਾਂ = ਨਿਰਾ;
        ਪੂਰਨ r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        ਪ੍ਰਤਿਜ੍ਞਾ(!(flags & (PROC_CMDLINE_ਮੁੱਲ_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&ਦਲੀਲਾਂ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        va_start(ap, flags);
        r = cmdline_get_ਕੁੰਜੀ_ap(flags, ਦਲੀਲਾਂ, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

bool proc_cmdline_ਮੁੱਲ_missing(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        ਜੇ (!ਮੁੱਲ) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", ਕੁੰਜੀ);
                ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
}
