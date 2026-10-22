/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "initrd-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "proc-cmdline.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "virt.h"

வகைவரை எண்ணிக்கை ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

முழு proc_cmdline_filter_pid1_args(எழுத்து **argv, எழுத்து ***ret) {
        மாறிலி கட்டமைப்பு {
                மாறிலி எழுத்து *name;
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
        மாறிலி எழுத்து *short_options = "hDbsz:";

        _cleanup_strv_free_ எழுத்து **filtered = வெற்று;
        முழு state, r;

        ப்ரதிஜ்ஞா(argv);
        ப்ரதிஜ்ஞா(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        ப்ரதிஜ்ஞா(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                முழு prev_state = state;
                மாறிலி எழுத்து *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                எனில் (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        தொடர்;

                எனில் (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        எனில் (r < 0)
                                திரும்பு r;
                        தொடர்;
                }

                எனில் (a[1] == '-') {
                        எனில் (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                எனில் (r < 0)
                                        திரும்பு r;
                                நிறுத்து;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                மாறிலி எழுத்து *q = startswith(a + 2, option->name);
                                எனில் (!q || !IN_SET(q[0], '=', '\0'))
                                        தொடர்;

                                /* Found matching option, updating the state if necessary. */
                                எனில் (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                நிறுத்து;
                        }
                        தொடர்;
                }

                /* short option(s), e.g. -x or -xyz */
                போது (a && *++a != '\0')
                        க்கு (மாறிலி எழுத்து *q = short_options; *q != '\0'; q++) {
                                எனில் (*q != *a)
                                        தொடர்;

                                /* Found matching short option. */

                                எனில் (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = வெற்று; /* Not necessary to parse remaining part. */
                                }
                                நிறுத்து;
                        }
        }

        *ret = TAKE_PTR(filtered);
        திரும்பு 0;
}

முழு proc_cmdline(எழுத்து **ret) {
        ப்ரதிஜ்ஞா(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        மாறிலி எழுத்து *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        எனில் (e)
                திரும்பு strdup_to(ret, e);

        எனில் (detect_container() > 0)
                திரும்பு pid_get_cmdline(1, SIZE_MAX, 0, ret);

        திரும்பு read_full_file("/proc/cmdline", ret, /* ret_size= */  வெற்று);
}

நிலையான முழு proc_cmdline_strv_internal(எழுத்து ***ret, bool filter_pid1_args) {
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        மாறிலி எழுத்து *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        எனில் (e)
                திரும்பு strv_split_full(ret, e, வெற்று, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        எனில் (detect_container() > 0) {
                _cleanup_strv_free_ எழுத்து **args = வெற்று;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (filter_pid1_args)
                        திரும்பு proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                திரும்பு 0;

        } இல்லையெனில் {
                _cleanup_free_ எழுத்து *s = வெற்று;

                r = read_full_file("/proc/cmdline", &s, வெற்று);
                எனில் (r < 0)
                        திரும்பு r;

                திரும்பு strv_split_full(ret, s, வெற்று, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

முழு proc_cmdline_strv(எழுத்து ***ret) {
        திரும்பு proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

நிலையான எழுத்து *mangle_word(மாறிலி எழுத்து *word, ProcCmdlineFlags flags) {
        எழுத்து *c = (எழுத்து*) startswith(word, "rd.");
        எனில் (c) {
                /* Filter out arguments that are intended only for the initrd */

                எனில் (!in_initrd())
                        திரும்பு வெற்று;

                எனில் (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        திரும்பு c;

        } இல்லையெனில் எனில் (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                திரும்பு வெற்று;

        திரும்பு (எழுத்து*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

நிலையான முழு proc_cmdline_parse_strv(எழுத்து **args, proc_cmdline_parse_t parse_item, வெறுமை *data, ProcCmdlineFlags flags) {
        முழு r;

        ப்ரதிஜ்ஞா(parse_item);

        STRV_FOREACH(word, args) {
                எழுத்து *சாவி, *மதிப்பு;

                சாவி = mangle_word(*word, flags);
                எனில் (!சாவி)
                        தொடர்;

                மதிப்பு = strchr(சாவி, '=');
                எனில் (மதிப்பு)
                        *(மதிப்பு++) = '\0';

                r = parse_item(சாவி, மதிப்பு, data);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

முழு proc_cmdline_parse(proc_cmdline_parse_t parse_item, வெறுமை *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ எழுத்து **args = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        ப்ரதிஜ்ஞா(!(flags & (PROC_CMDLINE_மதிப்பு_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு proc_cmdline_parse_strv(args, parse_item, data, flags);
}

நிலையான bool relaxed_equal_char(எழுத்து a, எழுத்து b) {
        திரும்பு a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

எழுத்து* proc_cmdline_சாவி_startswith(மாறிலி எழுத்து *s, மாறிலி எழுத்து *prefix) {
        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        க்கு (; *prefix != 0; s++, prefix++)
                எனில் (!relaxed_equal_char(*s, *prefix))
                        திரும்பு வெற்று;

        திரும்பு (எழுத்து*) s;
}

bool proc_cmdline_சாவி_streq(மாறிலி எழுத்து *x, மாறிலி எழுத்து *y) {
        ப்ரதிஜ்ஞா(x);
        ப்ரதிஜ்ஞா(y);

        /* Much like streq(), but considers "-" and "_" the same */

        க்கு (; *x != 0 || *y != 0; x++, y++)
                எனில் (!relaxed_equal_char(*x, *y))
                        திரும்பு false;

        திரும்பு true;
}

நிலையான முழு cmdline_get_சாவி(எழுத்து **args, மாறிலி எழுத்து *சாவி, ProcCmdlineFlags flags, எழுத்து **ret_மதிப்பு) {
        _cleanup_free_ எழுத்து *v = வெற்று;
        bool found = false;
        முழு r;

        ப்ரதிஜ்ஞா(சாவி);

        STRV_FOREACH(p, args) {
                மாறிலி எழுத்து *word;

                word = mangle_word(*p, flags);
                எனில் (!word)
                        தொடர்;

                எனில் (ret_மதிப்பு) {
                        மாறிலி எழுத்து *e;

                        e = proc_cmdline_சாவி_startswith(word, சாவி);
                        எனில் (!e)
                                தொடர்;

                        எனில் (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                எனில் (r < 0)
                                        திரும்பு r;

                                found = true;

                        } இல்லையெனில் எனில் (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_மதிப்பு_OPTIONAL))
                                found = true;

                } இல்லையெனில் {
                        எனில் (proc_cmdline_சாவி_streq(word, சாவி)) {
                                found = true;
                                நிறுத்து; /* we found what we were looking for */
                        }
                }
        }

        எனில் (ret_மதிப்பு)
                *ret_மதிப்பு = TAKE_PTR(v);

        திரும்பு found;
}

முழு proc_cmdline_get_சாவி(மாறிலி எழுத்து *சாவி, ProcCmdlineFlags flags, எழுத்து **ret_மதிப்பு) {
        _cleanup_strv_free_ எழுத்து **args = வெற்று;
        முழு r;

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
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        எனில் (isempty(சாவி))
                திரும்பு -EINVAL;

        எனில் (FLAGS_SET(flags, PROC_CMDLINE_மதிப்பு_OPTIONAL) && !ret_மதிப்பு)
                திரும்பு -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு cmdline_get_சாவி(args, சாவி, flags, ret_மதிப்பு);
}

முழு proc_cmdline_get_bool(மாறிலி எழுத்து *சாவி, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ எழுத்து *v = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = proc_cmdline_get_சாவி(சாவி, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_மதிப்பு_OPTIONAL, &v);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                திரும்பு 0;
        }

        எனில் (v) { /* key with parameter passed */
                r = parse_boolean(v);
                எனில் (r < 0)
                        திரும்பு r;
                *ret = r;
        } இல்லையெனில் /* key without parameter passed */
                *ret = true;

        திரும்பு 1;
}

நிலையான முழு cmdline_get_சாவி_ap(ProcCmdlineFlags flags, எழுத்து* மாறிலி* args, va_list ap) {
        முழு r, ret = 0;

        க்கு (;;) {
                எழுத்து **v;
                மாறிலி எழுத்து *k, *e;

                k = va_arg(ap, மாறிலி எழுத்து*);
                எனில் (!k)
                        நிறுத்து;

                assert_se(v = va_arg(ap, எழுத்து**));

                STRV_FOREACH(p, args) {
                        மாறிலி எழுத்து *word;

                        word = mangle_word(*p, flags);
                        எனில் (!word)
                                தொடர்;

                        e = proc_cmdline_சாவி_startswith(word, k);
                        எனில் (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                எனில் (r < 0)
                                        திரும்பு r;

                                ret++;
                        }
                }
        }

        திரும்பு ret;
}

முழு proc_cmdline_get_சாவி_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ எழுத்து **args = வெற்று;
        முழு r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        ப்ரதிஜ்ஞா(!(flags & (PROC_CMDLINE_மதிப்பு_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        எனில் (r < 0)
                திரும்பு r;

        va_start(ap, flags);
        r = cmdline_get_சாவி_ap(flags, args, ap);
        va_end(ap);

        திரும்பு r;
}

bool proc_cmdline_மதிப்பு_missing(மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        ப்ரதிஜ்ஞா(சாவி);

        எனில் (!மதிப்பு) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", சாவி);
                திரும்பு true;
        }

        திரும்பு false;
}
