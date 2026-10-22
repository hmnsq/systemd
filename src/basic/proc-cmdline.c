/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "initrd-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "proc-cmdline.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "virt.h"

પ્રકારવ્યાખ્યા ગણના ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

પૂર્ણાંક proc_cmdline_filter_pid1_args(અક્ષર **argv, અક્ષર ***ret) {
        અચલ સંરચના {
                અચલ અક્ષર *name;
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
        અચલ અક્ષર *short_options = "hDbsz:";

        _cleanup_strv_free_ અક્ષર **filtered = ખાલી;
        પૂર્ણાંક state, r;

        પ્રતિજ્ઞા(argv);
        પ્રતિજ્ઞા(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        પ્રતિજ્ઞા(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                પૂર્ણાંક prev_state = state;
                અચલ અક્ષર *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                જો (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        ચાલુ;

                જો (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        જો (r < 0)
                                પાછા r;
                        ચાલુ;
                }

                જો (a[1] == '-') {
                        જો (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                જો (r < 0)
                                        પાછા r;
                                તોડો;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                અચલ અક્ષર *q = startswith(a + 2, option->name);
                                જો (!q || !IN_SET(q[0], '=', '\0'))
                                        ચાલુ;

                                /* Found matching option, updating the state if necessary. */
                                જો (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                તોડો;
                        }
                        ચાલુ;
                }

                /* short option(s), e.g. -x or -xyz */
                જ્યારે (a && *++a != '\0')
                        માટે (અચલ અક્ષર *q = short_options; *q != '\0'; q++) {
                                જો (*q != *a)
                                        ચાલુ;

                                /* Found matching short option. */

                                જો (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = ખાલી; /* Not necessary to parse remaining part. */
                                }
                                તોડો;
                        }
        }

        *ret = TAKE_PTR(filtered);
        પાછા 0;
}

પૂર્ણાંક proc_cmdline(અક્ષર **ret) {
        પ્રતિજ્ઞા(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        અચલ અક્ષર *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        જો (e)
                પાછા strdup_to(ret, e);

        જો (detect_container() > 0)
                પાછા pid_get_cmdline(1, SIZE_MAX, 0, ret);

        પાછા read_full_file("/proc/cmdline", ret, /* ret_size= */  ખાલી);
}

સ્થિર પૂર્ણાંક proc_cmdline_strv_internal(અક્ષર ***ret, bool filter_pid1_args) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        અચલ અક્ષર *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        જો (e)
                પાછા strv_split_full(ret, e, ખાલી, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        જો (detect_container() > 0) {
                _cleanup_strv_free_ અક્ષર **args = ખાલી;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                જો (r < 0)
                        પાછા r;

                જો (filter_pid1_args)
                        પાછા proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                પાછા 0;

        } નહીંતર {
                _cleanup_free_ અક્ષર *s = ખાલી;

                r = read_full_file("/proc/cmdline", &s, ખાલી);
                જો (r < 0)
                        પાછા r;

                પાછા strv_split_full(ret, s, ખાલી, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

પૂર્ણાંક proc_cmdline_strv(અક્ષર ***ret) {
        પાછા proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

સ્થિર અક્ષર *mangle_word(અચલ અક્ષર *word, ProcCmdlineFlags flags) {
        અક્ષર *c = (અક્ષર*) startswith(word, "rd.");
        જો (c) {
                /* Filter out arguments that are intended only for the initrd */

                જો (!in_initrd())
                        પાછા ખાલી;

                જો (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        પાછા c;

        } નહીંતર જો (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                પાછા ખાલી;

        પાછા (અક્ષર*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

સ્થિર પૂર્ણાંક proc_cmdline_parse_strv(અક્ષર **args, proc_cmdline_parse_t parse_item, શૂન્ય *data, ProcCmdlineFlags flags) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(parse_item);

        STRV_FOREACH(word, args) {
                અક્ષર *ચાવી, *કિંમત;

                ચાવી = mangle_word(*word, flags);
                જો (!ચાવી)
                        ચાલુ;

                કિંમત = strchr(ચાવી, '=');
                જો (કિંમત)
                        *(કિંમત++) = '\0';

                r = parse_item(ચાવી, કિંમત, data);
                જો (r < 0)
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક proc_cmdline_parse(proc_cmdline_parse_t parse_item, શૂન્ય *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ અક્ષર **args = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        પ્રતિજ્ઞા(!(flags & (PROC_CMDLINE_કિંમત_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        જો (r < 0)
                પાછા r;

        પાછા proc_cmdline_parse_strv(args, parse_item, data, flags);
}

સ્થિર bool relaxed_equal_char(અક્ષર a, અક્ષર b) {
        પાછા a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

અક્ષર* proc_cmdline_ચાવી_startswith(અચલ અક્ષર *s, અચલ અક્ષર *prefix) {
        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        માટે (; *prefix != 0; s++, prefix++)
                જો (!relaxed_equal_char(*s, *prefix))
                        પાછા ખાલી;

        પાછા (અક્ષર*) s;
}

bool proc_cmdline_ચાવી_streq(અચલ અક્ષર *x, અચલ અક્ષર *y) {
        પ્રતિજ્ઞા(x);
        પ્રતિજ્ઞા(y);

        /* Much like streq(), but considers "-" and "_" the same */

        માટે (; *x != 0 || *y != 0; x++, y++)
                જો (!relaxed_equal_char(*x, *y))
                        પાછા false;

        પાછા true;
}

સ્થિર પૂર્ણાંક cmdline_get_ચાવી(અક્ષર **args, અચલ અક્ષર *ચાવી, ProcCmdlineFlags flags, અક્ષર **ret_કિંમત) {
        _cleanup_free_ અક્ષર *v = ખાલી;
        bool found = false;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ચાવી);

        STRV_FOREACH(p, args) {
                અચલ અક્ષર *word;

                word = mangle_word(*p, flags);
                જો (!word)
                        ચાલુ;

                જો (ret_કિંમત) {
                        અચલ અક્ષર *e;

                        e = proc_cmdline_ચાવી_startswith(word, ચાવી);
                        જો (!e)
                                ચાલુ;

                        જો (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                જો (r < 0)
                                        પાછા r;

                                found = true;

                        } નહીંતર જો (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_કિંમત_OPTIONAL))
                                found = true;

                } નહીંતર {
                        જો (proc_cmdline_ચાવી_streq(word, ચાવી)) {
                                found = true;
                                તોડો; /* we found what we were looking for */
                        }
                }
        }

        જો (ret_કિંમત)
                *ret_કિંમત = TAKE_PTR(v);

        પાછા found;
}

પૂર્ણાંક proc_cmdline_get_ચાવી(અચલ અક્ષર *ચાવી, ProcCmdlineFlags flags, અક્ષર **ret_કિંમત) {
        _cleanup_strv_free_ અક્ષર **args = ખાલી;
        પૂર્ણાંક r;

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
        પ્રતિજ્ઞા(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        જો (isempty(ચાવી))
                પાછા -EINVAL;

        જો (FLAGS_SET(flags, PROC_CMDLINE_કિંમત_OPTIONAL) && !ret_કિંમત)
                પાછા -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        જો (r < 0)
                પાછા r;

        પાછા cmdline_get_ચાવી(args, ચાવી, flags, ret_કિંમત);
}

પૂર્ણાંક proc_cmdline_get_bool(અચલ અક્ષર *ચાવી, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ અક્ષર *v = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = proc_cmdline_get_ચાવી(ચાવી, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_કિંમત_OPTIONAL, &v);
        જો (r < 0)
                પાછા r;
        જો (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                પાછા 0;
        }

        જો (v) { /* key with parameter passed */
                r = parse_boolean(v);
                જો (r < 0)
                        પાછા r;
                *ret = r;
        } નહીંતર /* key without parameter passed */
                *ret = true;

        પાછા 1;
}

સ્થિર પૂર્ણાંક cmdline_get_ચાવી_ap(ProcCmdlineFlags flags, અક્ષર* અચલ* args, va_list ap) {
        પૂર્ણાંક r, ret = 0;

        માટે (;;) {
                અક્ષર **v;
                અચલ અક્ષર *k, *e;

                k = va_arg(ap, અચલ અક્ષર*);
                જો (!k)
                        તોડો;

                assert_se(v = va_arg(ap, અક્ષર**));

                STRV_FOREACH(p, args) {
                        અચલ અક્ષર *word;

                        word = mangle_word(*p, flags);
                        જો (!word)
                                ચાલુ;

                        e = proc_cmdline_ચાવી_startswith(word, k);
                        જો (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                જો (r < 0)
                                        પાછા r;

                                ret++;
                        }
                }
        }

        પાછા ret;
}

પૂર્ણાંક proc_cmdline_get_ચાવી_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ અક્ષર **args = ખાલી;
        પૂર્ણાંક r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        પ્રતિજ્ઞા(!(flags & (PROC_CMDLINE_કિંમત_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        જો (r < 0)
                પાછા r;

        va_start(ap, flags);
        r = cmdline_get_ચાવી_ap(flags, args, ap);
        va_end(ap);

        પાછા r;
}

bool proc_cmdline_કિંમત_missing(અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        પ્રતિજ્ઞા(ચાવી);

        જો (!કિંમત) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", ચાવી);
                પાછા true;
        }

        પાછા false;
}
