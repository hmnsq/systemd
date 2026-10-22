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

ຈຳນວນເຕັມ proc_cmdline_filter_pid1_args(var_na **argv, var_na ***ret) {
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
        ຈຳນວນເຕັມ state, r;

        assert(argv);
        assert(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        assert(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                ຈຳນວນເຕັມ prev_state = state;
                const var_na *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                ຖ້າ (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        continue;

                ຖ້າ (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        ຖ້າ (r < 0)
                                ສົ່ງຄືນ r;
                        continue;
                }

                ຖ້າ (a[1] == '-') {
                        ຖ້າ (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                ຖ້າ (r < 0)
                                        ສົ່ງຄືນ r;
                                bha_nga;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                const var_na *q = startswith(a + 2, option->name);
                                ຖ້າ (!q || !IN_SET(q[0], '=', '\0'))
                                        continue;

                                /* Found matching option, updating the state if necessary. */
                                ຖ້າ (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                bha_nga;
                        }
                        continue;
                }

                /* short option(s), e.g. -x or -xyz */
                ໃນຂະນະທີ່ (a && *++a != '\0')
                        ສຳລັບ (const var_na *q = short_options; *q != '\0'; q++) {
                                ຖ້າ (*q != *a)
                                        continue;

                                /* Found matching short option. */

                                ຖ້າ (q[1] == ':') {
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
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ proc_cmdline(var_na **ret) {
        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const var_na *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ຖ້າ (e)
                ສົ່ງຄືນ strdup_to(ret, e);

        ຖ້າ (detect_container() > 0)
                ສົ່ງຄືນ pid_get_cmdline(1, SIZE_MAX, 0, ret);

        ສົ່ງຄືນ read_full_file("/proc/cmdline", ret, /* ret_size= */  NULL);
}

static ຈຳນວນເຕັມ proc_cmdline_strv_internal(var_na ***ret, bool filter_pid1_args) {
        ຈຳນວນເຕັມ r;

        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const var_na *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        ຖ້າ (e)
                ສົ່ງຄືນ strv_split_full(ret, e, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        ຖ້າ (detect_container() > 0) {
                _cleanup_strv_free_ var_na **args = NULL;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (filter_pid1_args)
                        ສົ່ງຄືນ proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                ສົ່ງຄືນ 0;

        } ບໍ່ດັ່ງນັ້ນ {
                _cleanup_free_ var_na *s = NULL;

                r = read_full_file("/proc/cmdline", &s, NULL);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ສົ່ງຄືນ strv_split_full(ret, s, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

ຈຳນວນເຕັມ proc_cmdline_strv(var_na ***ret) {
        ສົ່ງຄືນ proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

static var_na *mangle_word(const var_na *word, ProcCmdlineFlags flags) {
        var_na *c = (var_na*) startswith(word, "rd.");
        ຖ້າ (c) {
                /* Filter out arguments that are intended only for the initrd */

                ຖ້າ (!in_initrd())
                        ສົ່ງຄືນ NULL;

                ຖ້າ (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        ສົ່ງຄືນ c;

        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                ສົ່ງຄືນ NULL;

        ສົ່ງຄືນ (var_na*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

static ຈຳນວນເຕັມ proc_cmdline_parse_strv(var_na **args, proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        ຈຳນວນເຕັມ r;

        assert(parse_item);

        STRV_FOREACH(word, args) {
                var_na *key, *value;

                key = mangle_word(*word, flags);
                ຖ້າ (!key)
                        continue;

                value = strchr(key, '=');
                ຖ້າ (value)
                        *(value++) = '\0';

                r = parse_item(key, value, data);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ proc_cmdline_parse(proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ var_na **args = NULL;
        ຈຳນວນເຕັມ r;

        assert(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ proc_cmdline_parse_strv(args, parse_item, data, flags);
}

static bool relaxed_equal_char(var_na a, var_na b) {
        ສົ່ງຄືນ a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

var_na* proc_cmdline_key_startswith(const var_na *s, const var_na *prefix) {
        assert(s);
        assert(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        ສຳລັບ (; *prefix != 0; s++, prefix++)
                ຖ້າ (!relaxed_equal_char(*s, *prefix))
                        ສົ່ງຄືນ NULL;

        ສົ່ງຄືນ (var_na*) s;
}

bool proc_cmdline_key_streq(const var_na *x, const var_na *y) {
        assert(x);
        assert(y);

        /* Much like streq(), but considers "-" and "_" the same */

        ສຳລັບ (; *x != 0 || *y != 0; x++, y++)
                ຖ້າ (!relaxed_equal_char(*x, *y))
                        ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}

static ຈຳນວນເຕັມ cmdline_get_key(var_na **args, const var_na *key, ProcCmdlineFlags flags, var_na **ret_value) {
        _cleanup_free_ var_na *v = NULL;
        bool found = false;
        ຈຳນວນເຕັມ r;

        assert(key);

        STRV_FOREACH(p, args) {
                const var_na *word;

                word = mangle_word(*p, flags);
                ຖ້າ (!word)
                        continue;

                ຖ້າ (ret_value) {
                        const var_na *e;

                        e = proc_cmdline_key_startswith(word, key);
                        ຖ້າ (!e)
                                continue;

                        ຖ້າ (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                ຖ້າ (r < 0)
                                        ສົ່ງຄືນ r;

                                found = true;

                        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL))
                                found = true;

                } ບໍ່ດັ່ງນັ້ນ {
                        ຖ້າ (proc_cmdline_key_streq(word, key)) {
                                found = true;
                                bha_nga; /* we found what we were looking for */
                        }
                }
        }

        ຖ້າ (ret_value)
                *ret_value = TAKE_PTR(v);

        ສົ່ງຄືນ found;
}

ຈຳນວນເຕັມ proc_cmdline_get_key(const var_na *key, ProcCmdlineFlags flags, var_na **ret_value) {
        _cleanup_strv_free_ var_na **args = NULL;
        ຈຳນວນເຕັມ r;

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

        ຖ້າ (isempty(key))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL) && !ret_value)
                ສົ່ງຄືນ -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ cmdline_get_key(args, key, flags, ret_value);
}

ຈຳນວນເຕັມ proc_cmdline_get_bool(const var_na *key, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ var_na *v = NULL;
        ຈຳນວນເຕັມ r;

        assert(ret);

        r = proc_cmdline_get_key(key, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_VALUE_OPTIONAL, &v);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                ສົ່ງຄືນ 0;
        }

        ຖ້າ (v) { /* key with parameter passed */
                r = parse_boolean(v);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                *ret = r;
        } ບໍ່ດັ່ງນັ້ນ /* key without parameter passed */
                *ret = true;

        ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ cmdline_get_key_ap(ProcCmdlineFlags flags, var_na* const* args, va_list ap) {
        ຈຳນວນເຕັມ r, ret = 0;

        ສຳລັບ (;;) {
                var_na **v;
                const var_na *k, *e;

                k = va_arg(ap, const var_na*);
                ຖ້າ (!k)
                        bha_nga;

                assert_se(v = va_arg(ap, var_na**));

                STRV_FOREACH(p, args) {
                        const var_na *word;

                        word = mangle_word(*p, flags);
                        ຖ້າ (!word)
                                continue;

                        e = proc_cmdline_key_startswith(word, k);
                        ຖ້າ (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                ຖ້າ (r < 0)
                                        ສົ່ງຄືນ r;

                                ret++;
                        }
                }
        }

        ສົ່ງຄືນ ret;
}

ຈຳນວນເຕັມ proc_cmdline_get_key_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ var_na **args = NULL;
        ຈຳນວນເຕັມ r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        va_start(ap, flags);
        r = cmdline_get_key_ap(flags, args, ap);
        va_end(ap);

        ສົ່ງຄືນ r;
}

bool proc_cmdline_value_missing(const var_na *key, const var_na *value) {
        assert(key);

        ຖ້າ (!value) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", key);
                ສົ່ງຄືນ true;
        }

        ສົ່ງຄືນ false;
}
