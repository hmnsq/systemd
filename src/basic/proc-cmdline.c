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

ကိန်းပြည့် proc_cmdline_filter_pid1_args(var_na **argv, var_na ***ret) {
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
        ကိန်းပြည့် state, r;

        assert(argv);
        assert(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        assert(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                ကိန်းပြည့် prev_state = state;
                const var_na *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                အကယ်၍ (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        continue;

                အကယ်၍ (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;
                        continue;
                }

                အကယ်၍ (a[1] == '-') {
                        အကယ်၍ (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;
                                bha_nga;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                const var_na *q = startswith(a + 2, option->name);
                                အကယ်၍ (!q || !IN_SET(q[0], '=', '\0'))
                                        continue;

                                /* Found matching option, updating the state if necessary. */
                                အကယ်၍ (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                bha_nga;
                        }
                        continue;
                }

                /* short option(s), e.g. -x or -xyz */
                နေစဉ် (a && *++a != '\0')
                        အတွက် (const var_na *q = short_options; *q != '\0'; q++) {
                                အကယ်၍ (*q != *a)
                                        continue;

                                /* Found matching short option. */

                                အကယ်၍ (q[1] == ':') {
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
        ပြန်ပေး 0;
}

ကိန်းပြည့် proc_cmdline(var_na **ret) {
        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const var_na *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        အကယ်၍ (e)
                ပြန်ပေး strdup_to(ret, e);

        အကယ်၍ (detect_container() > 0)
                ပြန်ပေး pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ပြန်ပေး read_full_file("/proc/cmdline", ret, /* ret_size= */  NULL);
}

static ကိန်းပြည့် proc_cmdline_strv_internal(var_na ***ret, bool filter_pid1_args) {
        ကိန်းပြည့် r;

        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const var_na *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        အကယ်၍ (e)
                ပြန်ပေး strv_split_full(ret, e, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        အကယ်၍ (detect_container() > 0) {
                _cleanup_strv_free_ var_na **args = NULL;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (filter_pid1_args)
                        ပြန်ပေး proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                ပြန်ပေး 0;

        } မဟုတ်ပါက {
                _cleanup_free_ var_na *s = NULL;

                r = read_full_file("/proc/cmdline", &s, NULL);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                ပြန်ပေး strv_split_full(ret, s, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

ကိန်းပြည့် proc_cmdline_strv(var_na ***ret) {
        ပြန်ပေး proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

static var_na *mangle_word(const var_na *word, ProcCmdlineFlags flags) {
        var_na *c = (var_na*) startswith(word, "rd.");
        အကယ်၍ (c) {
                /* Filter out arguments that are intended only for the initrd */

                အကယ်၍ (!in_initrd())
                        ပြန်ပေး NULL;

                အကယ်၍ (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        ပြန်ပေး c;

        } မဟုတ်ပါက အကယ်၍ (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ပြန်ပေး NULL;

        ပြန်ပေး (var_na*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

static ကိန်းပြည့် proc_cmdline_parse_strv(var_na **args, proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        ကိန်းပြည့် r;

        assert(parse_item);

        STRV_FOREACH(word, args) {
                var_na *key, *value;

                key = mangle_word(*word, flags);
                အကယ်၍ (!key)
                        continue;

                value = strchr(key, '=');
                အကယ်၍ (value)
                        *(value++) = '\0';

                r = parse_item(key, value, data);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် proc_cmdline_parse(proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ var_na **args = NULL;
        ကိန်းပြည့် r;

        assert(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး proc_cmdline_parse_strv(args, parse_item, data, flags);
}

static bool relaxed_equal_char(var_na a, var_na b) {
        ပြန်ပေး a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

var_na* proc_cmdline_key_startswith(const var_na *s, const var_na *prefix) {
        assert(s);
        assert(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        အတွက် (; *prefix != 0; s++, prefix++)
                အကယ်၍ (!relaxed_equal_char(*s, *prefix))
                        ပြန်ပေး NULL;

        ပြန်ပေး (var_na*) s;
}

bool proc_cmdline_key_streq(const var_na *x, const var_na *y) {
        assert(x);
        assert(y);

        /* Much like streq(), but considers "-" and "_" the same */

        အတွက် (; *x != 0 || *y != 0; x++, y++)
                အကယ်၍ (!relaxed_equal_char(*x, *y))
                        ပြန်ပေး false;

        ပြန်ပေး true;
}

static ကိန်းပြည့် cmdline_get_key(var_na **args, const var_na *key, ProcCmdlineFlags flags, var_na **ret_value) {
        _cleanup_free_ var_na *v = NULL;
        bool found = false;
        ကိန်းပြည့် r;

        assert(key);

        STRV_FOREACH(p, args) {
                const var_na *word;

                word = mangle_word(*p, flags);
                အကယ်၍ (!word)
                        continue;

                အကယ်၍ (ret_value) {
                        const var_na *e;

                        e = proc_cmdline_key_startswith(word, key);
                        အကယ်၍ (!e)
                                continue;

                        အကယ်၍ (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                found = true;

                        } မဟုတ်ပါက အကယ်၍ (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL))
                                found = true;

                } မဟုတ်ပါက {
                        အကယ်၍ (proc_cmdline_key_streq(word, key)) {
                                found = true;
                                bha_nga; /* we found what we were looking for */
                        }
                }
        }

        အကယ်၍ (ret_value)
                *ret_value = TAKE_PTR(v);

        ပြန်ပေး found;
}

ကိန်းပြည့် proc_cmdline_get_key(const var_na *key, ProcCmdlineFlags flags, var_na **ret_value) {
        _cleanup_strv_free_ var_na **args = NULL;
        ကိန်းပြည့် r;

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

        အကယ်၍ (isempty(key))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL) && !ret_value)
                ပြန်ပေး -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး cmdline_get_key(args, key, flags, ret_value);
}

ကိန်းပြည့် proc_cmdline_get_bool(const var_na *key, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ var_na *v = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        r = proc_cmdline_get_key(key, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_VALUE_OPTIONAL, &v);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ပြန်ပေး 0;
        }

        အကယ်၍ (v) { /* key with parameter passed */
                r = parse_boolean(v);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                *ret = r;
        } မဟုတ်ပါက /* key without parameter passed */
                *ret = true;

        ပြန်ပေး 1;
}

static ကိန်းပြည့် cmdline_get_key_ap(ProcCmdlineFlags flags, var_na* const* args, va_list ap) {
        ကိန်းပြည့် r, ret = 0;

        အတွက် (;;) {
                var_na **v;
                const var_na *k, *e;

                k = va_arg(ap, const var_na*);
                အကယ်၍ (!k)
                        bha_nga;

                assert_se(v = va_arg(ap, var_na**));

                STRV_FOREACH(p, args) {
                        const var_na *word;

                        word = mangle_word(*p, flags);
                        အကယ်၍ (!word)
                                continue;

                        e = proc_cmdline_key_startswith(word, k);
                        အကယ်၍ (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                ret++;
                        }
                }
        }

        ပြန်ပေး ret;
}

ကိန်းပြည့် proc_cmdline_get_key_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ var_na **args = NULL;
        ကိန်းပြည့် r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        va_start(ap, flags);
        r = cmdline_get_key_ap(flags, args, ap);
        va_end(ap);

        ပြန်ပေး r;
}

bool proc_cmdline_value_missing(const var_na *key, const var_na *value) {
        assert(key);

        အကယ်၍ (!value) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", key);
                ပြန်ပေး true;
        }

        ပြန်ပေး false;
}
