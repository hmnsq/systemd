/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "initrd-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "proc-cmdline.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "virt.h"

തരനിർവചനം എണ്ണൽ ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

പൂർണ്ണസംഖ്യ proc_cmdline_filter_pid1_args(അക്ഷരം **argv, അക്ഷരം ***ret) {
        സ്ഥിരം ഘടന {
                സ്ഥിരം അക്ഷരം *name;
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
        സ്ഥിരം അക്ഷരം *short_options = "hDbsz:";

        _cleanup_strv_free_ അക്ഷരം **filtered = ശൂന്യ;
        പൂർണ്ണസംഖ്യ state, r;

        പ്രതിജ്ഞാ(argv);
        പ്രതിജ്ഞാ(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        പ്രതിജ്ഞാ(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                പൂർണ്ണസംഖ്യ prev_state = state;
                സ്ഥിരം അക്ഷരം *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                എങ്കിൽ (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        തുടരുക;

                എങ്കിൽ (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        തുടരുക;
                }

                എങ്കിൽ (a[1] == '-') {
                        എങ്കിൽ (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                                നിർത്തുക;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                സ്ഥിരം അക്ഷരം *q = startswith(a + 2, option->name);
                                എങ്കിൽ (!q || !IN_SET(q[0], '=', '\0'))
                                        തുടരുക;

                                /* Found matching option, updating the state if necessary. */
                                എങ്കിൽ (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                നിർത്തുക;
                        }
                        തുടരുക;
                }

                /* short option(s), e.g. -x or -xyz */
                ആകുമ്പോൾ (a && *++a != '\0')
                        വേണ്ടി (സ്ഥിരം അക്ഷരം *q = short_options; *q != '\0'; q++) {
                                എങ്കിൽ (*q != *a)
                                        തുടരുക;

                                /* Found matching short option. */

                                എങ്കിൽ (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = ശൂന്യ; /* Not necessary to parse remaining part. */
                                }
                                നിർത്തുക;
                        }
        }

        *ret = TAKE_PTR(filtered);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ proc_cmdline(അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        സ്ഥിരം അക്ഷരം *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        എങ്കിൽ (e)
                തിരികെ strdup_to(ret, e);

        എങ്കിൽ (detect_container() > 0)
                തിരികെ pid_get_cmdline(1, SIZE_MAX, 0, ret);

        തിരികെ read_full_file("/proc/cmdline", ret, /* ret_size= */  ശൂന്യ);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ proc_cmdline_strv_internal(അക്ഷരം ***ret, bool filter_pid1_args) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        സ്ഥിരം അക്ഷരം *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        എങ്കിൽ (e)
                തിരികെ strv_split_full(ret, e, ശൂന്യ, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        എങ്കിൽ (detect_container() > 0) {
                _cleanup_strv_free_ അക്ഷരം **args = ശൂന്യ;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (filter_pid1_args)
                        തിരികെ proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                തിരികെ 0;

        } അല്ലെങ്കിൽ {
                _cleanup_free_ അക്ഷരം *s = ശൂന്യ;

                r = read_full_file("/proc/cmdline", &s, ശൂന്യ);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                തിരികെ strv_split_full(ret, s, ശൂന്യ, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

പൂർണ്ണസംഖ്യ proc_cmdline_strv(അക്ഷരം ***ret) {
        തിരികെ proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

സ്ഥിതം അക്ഷരം *mangle_word(സ്ഥിരം അക്ഷരം *word, ProcCmdlineFlags flags) {
        അക്ഷരം *c = (അക്ഷരം*) startswith(word, "rd.");
        എങ്കിൽ (c) {
                /* Filter out arguments that are intended only for the initrd */

                എങ്കിൽ (!in_initrd())
                        തിരികെ ശൂന്യ;

                എങ്കിൽ (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        തിരികെ c;

        } അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                തിരികെ ശൂന്യ;

        തിരികെ (അക്ഷരം*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

സ്ഥിതം പൂർണ്ണസംഖ്യ proc_cmdline_parse_strv(അക്ഷരം **args, proc_cmdline_parse_t parse_item, ശൂന്യം *data, ProcCmdlineFlags flags) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(parse_item);

        STRV_FOREACH(word, args) {
                അക്ഷരം *താക്കോൽ, *മൂല്യം;

                താക്കോൽ = mangle_word(*word, flags);
                എങ്കിൽ (!താക്കോൽ)
                        തുടരുക;

                മൂല്യം = strchr(താക്കോൽ, '=');
                എങ്കിൽ (മൂല്യം)
                        *(മൂല്യം++) = '\0';

                r = parse_item(താക്കോൽ, മൂല്യം, data);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ proc_cmdline_parse(proc_cmdline_parse_t parse_item, ശൂന്യം *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ അക്ഷരം **args = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        പ്രതിജ്ഞാ(!(flags & (PROC_CMDLINE_മൂല്യം_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ proc_cmdline_parse_strv(args, parse_item, data, flags);
}

സ്ഥിതം bool relaxed_equal_char(അക്ഷരം a, അക്ഷരം b) {
        തിരികെ a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

അക്ഷരം* proc_cmdline_താക്കോൽ_startswith(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *prefix) {
        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        വേണ്ടി (; *prefix != 0; s++, prefix++)
                എങ്കിൽ (!relaxed_equal_char(*s, *prefix))
                        തിരികെ ശൂന്യ;

        തിരികെ (അക്ഷരം*) s;
}

bool proc_cmdline_താക്കോൽ_streq(സ്ഥിരം അക്ഷരം *x, സ്ഥിരം അക്ഷരം *y) {
        പ്രതിജ്ഞാ(x);
        പ്രതിജ്ഞാ(y);

        /* Much like streq(), but considers "-" and "_" the same */

        വേണ്ടി (; *x != 0 || *y != 0; x++, y++)
                എങ്കിൽ (!relaxed_equal_char(*x, *y))
                        തിരികെ false;

        തിരികെ true;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cmdline_get_താക്കോൽ(അക്ഷരം **args, സ്ഥിരം അക്ഷരം *താക്കോൽ, ProcCmdlineFlags flags, അക്ഷരം **ret_മൂല്യം) {
        _cleanup_free_ അക്ഷരം *v = ശൂന്യ;
        bool found = false;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(താക്കോൽ);

        STRV_FOREACH(p, args) {
                സ്ഥിരം അക്ഷരം *word;

                word = mangle_word(*p, flags);
                എങ്കിൽ (!word)
                        തുടരുക;

                എങ്കിൽ (ret_മൂല്യം) {
                        സ്ഥിരം അക്ഷരം *e;

                        e = proc_cmdline_താക്കോൽ_startswith(word, താക്കോൽ);
                        എങ്കിൽ (!e)
                                തുടരുക;

                        എങ്കിൽ (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                found = true;

                        } അല്ലെങ്കിൽ എങ്കിൽ (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_മൂല്യം_OPTIONAL))
                                found = true;

                } അല്ലെങ്കിൽ {
                        എങ്കിൽ (proc_cmdline_താക്കോൽ_streq(word, താക്കോൽ)) {
                                found = true;
                                നിർത്തുക; /* we found what we were looking for */
                        }
                }
        }

        എങ്കിൽ (ret_മൂല്യം)
                *ret_മൂല്യം = TAKE_PTR(v);

        തിരികെ found;
}

പൂർണ്ണസംഖ്യ proc_cmdline_get_താക്കോൽ(സ്ഥിരം അക്ഷരം *താക്കോൽ, ProcCmdlineFlags flags, അക്ഷരം **ret_മൂല്യം) {
        _cleanup_strv_free_ അക്ഷരം **args = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

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
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        എങ്കിൽ (isempty(താക്കോൽ))
                തിരികെ -EINVAL;

        എങ്കിൽ (FLAGS_SET(flags, PROC_CMDLINE_മൂല്യം_OPTIONAL) && !ret_മൂല്യം)
                തിരികെ -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ cmdline_get_താക്കോൽ(args, താക്കോൽ, flags, ret_മൂല്യം);
}

പൂർണ്ണസംഖ്യ proc_cmdline_get_bool(സ്ഥിരം അക്ഷരം *താക്കോൽ, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ അക്ഷരം *v = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = proc_cmdline_get_താക്കോൽ(താക്കോൽ, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_മൂല്യം_OPTIONAL, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                തിരികെ 0;
        }

        എങ്കിൽ (v) { /* key with parameter passed */
                r = parse_boolean(v);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                *ret = r;
        } അല്ലെങ്കിൽ /* key without parameter passed */
                *ret = true;

        തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ cmdline_get_താക്കോൽ_ap(ProcCmdlineFlags flags, അക്ഷരം* സ്ഥിരം* args, va_list ap) {
        പൂർണ്ണസംഖ്യ r, ret = 0;

        വേണ്ടി (;;) {
                അക്ഷരം **v;
                സ്ഥിരം അക്ഷരം *k, *e;

                k = va_arg(ap, സ്ഥിരം അക്ഷരം*);
                എങ്കിൽ (!k)
                        നിർത്തുക;

                assert_se(v = va_arg(ap, അക്ഷരം**));

                STRV_FOREACH(p, args) {
                        സ്ഥിരം അക്ഷരം *word;

                        word = mangle_word(*p, flags);
                        എങ്കിൽ (!word)
                                തുടരുക;

                        e = proc_cmdline_താക്കോൽ_startswith(word, k);
                        എങ്കിൽ (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                ret++;
                        }
                }
        }

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ proc_cmdline_get_താക്കോൽ_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ അക്ഷരം **args = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        പ്രതിജ്ഞാ(!(flags & (PROC_CMDLINE_മൂല്യം_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        va_start(ap, flags);
        r = cmdline_get_താക്കോൽ_ap(flags, args, ap);
        va_end(ap);

        തിരികെ r;
}

bool proc_cmdline_മൂല്യം_missing(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        പ്രതിജ്ഞാ(താക്കോൽ);

        എങ്കിൽ (!മൂല്യം) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", താക്കോൽ);
                തിരികെ true;
        }

        തിരികെ false;
}
