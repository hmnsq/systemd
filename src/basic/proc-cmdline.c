/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>

#include "alloc-util.h"
#include "extract-word.h"
#include "fileio.h"
#include "initrd-util.h"
#include "log.h"
#include "parse-util.h"
#include "proc-cmdline.h"
#include "process-util.h"
#include "string-util.h"
#include "strv.h"
#include "virt.h"

typedef ga_nanaa ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

puur_naa_nka proc_cmdline_filter_pid1_args(var_na **argv, var_na ***ret) {
        const sa_nracanaa {
                const var_na *name;
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
        const var_na *short_options = "hDbsz:";

        _cleanup_strv_free_ var_na **filtered = NULL;
        puur_naa_nka state, r;

        assert(argv);
        assert(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        assert(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                puur_naa_nka prev_state = state;
                const var_na *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                ᨀᨘ (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        continue;

                ᨀᨘ (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        ᨀᨘ (r < 0)
                                ᨀᨘᨒ r;
                        continue;
                }

                ᨀᨘ (a[1] == '-') {
                        ᨀᨘ (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                ᨀᨘ (r < 0)
                                        ᨀᨘᨒ r;
                                bha_nga;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                const var_na *q = startswith(a + 2, option->name);
                                ᨀᨘ (!q || !IN_SET(q[0], '=', '\0'))
                                        continue;

                                /* Found matching option, updating the state if necessary. */
                                ᨀᨘ (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                bha_nga;
                        }
                        continue;
                }

                /* short option(s), e.g. -x or -xyz */
                ᨀᨛ (a && *++a != '\0')
                        for (const var_na *q = short_options; *q != '\0'; q++) {
                                ᨀᨘ (*q != *a)
                                        continue;

                                /* Found matching short option. */

                                ᨀᨘ (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = NULL; /* Not necessary to parse remaining part. */
                                }
                                bha_nga;
                        }
        }

        *ret = TAKE_PTR(filtered);
        ᨀᨘᨒ 0;
}

puur_naa_nka proc_cmdline(var_na **ret) {
        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const var_na *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ᨀᨘ (e)
                ᨀᨘᨒ strdup_to(ret, e);

        ᨀᨘ (detect_container() > 0)
                ᨀᨘᨒ pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ᨀᨘᨒ read_full_file("/proc/cmdline", ret, /* ret_size= */  NULL);
}

static puur_naa_nka proc_cmdline_strv_internal(var_na ***ret, bool filter_pid1_args) {
        puur_naa_nka r;

        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const var_na *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ᨀᨘ (e)
                ᨀᨘᨒ strv_split_full(ret, e, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        ᨀᨘ (detect_container() > 0) {
                _cleanup_strv_free_ var_na **args = NULL;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                ᨀᨘ (filter_pid1_args)
                        ᨀᨘᨒ proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                ᨀᨘᨒ 0;

        } ᨀᨘᨁ {
                _cleanup_free_ var_na *s = NULL;

                r = read_full_file("/proc/cmdline", &s, NULL);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;

                ᨀᨘᨒ strv_split_full(ret, s, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

puur_naa_nka proc_cmdline_strv(var_na ***ret) {
        ᨀᨘᨒ proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

static var_na *mangle_word(const var_na *word, ProcCmdlineFlags flags) {
        var_na *c = (var_na*) startswith(word, "rd.");
        ᨀᨘ (c) {
                /* Filter out arguments that are intended only for the initrd */

                ᨀᨘ (!in_initrd())
                        ᨀᨘᨒ NULL;

                ᨀᨘ (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        ᨀᨘᨒ c;

        } ᨀᨘᨁ ᨀᨘ (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ᨀᨘᨒ NULL;

        ᨀᨘᨒ (var_na*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

static puur_naa_nka proc_cmdline_parse_strv(var_na **args, proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        puur_naa_nka r;

        assert(parse_item);

        STRV_FOREACH(word, args) {
                var_na *key, *value;

                key = mangle_word(*word, flags);
                ᨀᨘ (!key)
                        continue;

                value = strchr(key, '=');
                ᨀᨘ (value)
                        *(value++) = '\0';

                r = parse_item(key, value, data);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
        }

        ᨀᨘᨒ 0;
}

puur_naa_nka proc_cmdline_parse(proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ var_na **args = NULL;
        puur_naa_nka r;

        assert(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ proc_cmdline_parse_strv(args, parse_item, data, flags);
}

static bool relaxed_equal_char(var_na a, var_na b) {
        ᨀᨘᨒ a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

var_na* proc_cmdline_key_startswith(const var_na *s, const var_na *prefix) {
        assert(s);
        assert(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        for (; *prefix != 0; s++, prefix++)
                ᨀᨘ (!relaxed_equal_char(*s, *prefix))
                        ᨀᨘᨒ NULL;

        ᨀᨘᨒ (var_na*) s;
}

bool proc_cmdline_key_streq(const var_na *x, const var_na *y) {
        assert(x);
        assert(y);

        /* Much like streq(), but considers "-" and "_" the same */

        for (; *x != 0 || *y != 0; x++, y++)
                ᨀᨘ (!relaxed_equal_char(*x, *y))
                        ᨀᨘᨒ false;

        ᨀᨘᨒ true;
}

static puur_naa_nka cmdline_get_key(var_na **args, const var_na *key, ProcCmdlineFlags flags, var_na **ret_value) {
        _cleanup_free_ var_na *v = NULL;
        bool found = false;
        puur_naa_nka r;

        assert(key);

        STRV_FOREACH(p, args) {
                const var_na *word;

                word = mangle_word(*p, flags);
                ᨀᨘ (!word)
                        continue;

                ᨀᨘ (ret_value) {
                        const var_na *e;

                        e = proc_cmdline_key_startswith(word, key);
                        ᨀᨘ (!e)
                                continue;

                        ᨀᨘ (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                ᨀᨘ (r < 0)
                                        ᨀᨘᨒ r;

                                found = true;

                        } ᨀᨘᨁ ᨀᨘ (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL))
                                found = true;

                } ᨀᨘᨁ {
                        ᨀᨘ (proc_cmdline_key_streq(word, key)) {
                                found = true;
                                bha_nga; /* we found what we were looking for */
                        }
                }
        }

        ᨀᨘ (ret_value)
                *ret_value = TAKE_PTR(v);

        ᨀᨘᨒ found;
}

puur_naa_nka proc_cmdline_get_key(const var_na *key, ProcCmdlineFlags flags, var_na **ret_value) {
        _cleanup_strv_free_ var_na **args = NULL;
        puur_naa_nka r;

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
        assert(!FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING));

        ᨀᨘ (isempty(key))
                ᨀᨘᨒ -EINVAL;

        ᨀᨘ (FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL) && !ret_value)
                ᨀᨘᨒ -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ cmdline_get_key(args, key, flags, ret_value);
}

puur_naa_nka proc_cmdline_get_bool(const var_na *key, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ var_na *v = NULL;
        puur_naa_nka r;

        assert(ret);

        r = proc_cmdline_get_key(key, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_VALUE_OPTIONAL, &v);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        ᨀᨘ (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ᨀᨘᨒ 0;
        }

        ᨀᨘ (v) { /* key with parameter passed */
                r = parse_boolean(v);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
                *ret = r;
        } ᨀᨘᨁ /* key without parameter passed */
                *ret = true;

        ᨀᨘᨒ 1;
}

static puur_naa_nka cmdline_get_key_ap(ProcCmdlineFlags flags, var_na* const* args, va_list ap) {
        puur_naa_nka r, ret = 0;

        for (;;) {
                var_na **v;
                const var_na *k, *e;

                k = va_arg(ap, const var_na*);
                ᨀᨘ (!k)
                        bha_nga;

                assert_se(v = va_arg(ap, var_na**));

                STRV_FOREACH(p, args) {
                        const var_na *word;

                        word = mangle_word(*p, flags);
                        ᨀᨘ (!word)
                                continue;

                        e = proc_cmdline_key_startswith(word, k);
                        ᨀᨘ (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                ᨀᨘ (r < 0)
                                        ᨀᨘᨒ r;

                                ret++;
                        }
                }
        }

        ᨀᨘᨒ ret;
}

puur_naa_nka proc_cmdline_get_key_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ var_na **args = NULL;
        puur_naa_nka r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        va_start(ap, flags);
        r = cmdline_get_key_ap(flags, args, ap);
        va_end(ap);

        ᨀᨘᨒ r;
}

bool proc_cmdline_value_missing(const var_na *key, const var_na *value) {
        assert(key);

        ᨀᨘ (!value) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", key);
                ᨀᨘᨒ true;
        }

        ᨀᨘᨒ false;
}
