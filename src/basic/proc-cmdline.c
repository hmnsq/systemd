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

typedef enum ArgType {
        no_argument,
        required_argument,
        optional_argument,
} ArgType;

ціле proc_cmdline_filter_pid1_args(char **argv, char ***ret) {
        const struct {
                const char *name;
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
        const char *short_options = "hDbsz:";

        _cleanup_strv_free_ char **filtered = NULL;
        ціле state, r;

        assert(argv);
        assert(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        assert(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                ціле prev_state = state;
                const char *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                якщо (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        continue;

                якщо (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        якщо (r < 0)
                                повернути r;
                        continue;
                }

                якщо (a[1] == '-') {
                        якщо (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                якщо (r < 0)
                                        повернути r;
                                break;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                const char *q = startswith(a + 2, option->name);
                                якщо (!q || !IN_SET(q[0], '=', '\0'))
                                        continue;

                                /* Found matching option, updating the state if necessary. */
                                якщо (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                break;
                        }
                        continue;
                }

                /* short option(s), e.g. -x or -xyz */
                поки (a && *++a != '\0')
                        для (const char *q = short_options; *q != '\0'; q++) {
                                якщо (*q != *a)
                                        continue;

                                /* Found matching short option. */

                                якщо (q[1] == ':') {
                                        /* An argument is required or optional, and remaining part
                                         * is handled as argument if exists. */
                                        state = a[1] != '\0' ? no_argument :
                                                q[2] == ':' ? optional_argument : required_argument;

                                        a = NULL; /* Not necessary to parse remaining part. */
                                }
                                break;
                        }
        }

        *ret = TAKE_PTR(filtered);
        повернути 0;
}

ціле proc_cmdline(char **ret) {
        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const char *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        якщо (e)
                повернути strdup_to(ret, e);

        якщо (detect_container() > 0)
                повернути pid_get_cmdline(1, SIZE_MAX, 0, ret);

        повернути read_full_file("/proc/cmdline", ret, /* ret_size= */  NULL);
}

static ціле proc_cmdline_strv_internal(char ***ret, bool filter_pid1_args) {
        ціле r;

        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const char *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        якщо (e)
                повернути strv_split_full(ret, e, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        якщо (detect_container() > 0) {
                _cleanup_strv_free_ char **args = NULL;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                якщо (r < 0)
                        повернути r;

                якщо (filter_pid1_args)
                        повернути proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                повернути 0;

        } інакше {
                _cleanup_free_ char *s = NULL;

                r = read_full_file("/proc/cmdline", &s, NULL);
                якщо (r < 0)
                        повернути r;

                повернути strv_split_full(ret, s, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

ціле proc_cmdline_strv(char ***ret) {
        повернути proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

static char *mangle_word(const char *word, ProcCmdlineFlags flags) {
        char *c = (char*) startswith(word, "rd.");
        якщо (c) {
                /* Filter out arguments that are intended only for the initrd */

                якщо (!in_initrd())
                        повернути NULL;

                якщо (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        повернути c;

        } інакше якщо (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                повернути NULL;

        повернути (char*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

static ціле proc_cmdline_parse_strv(char **args, proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        ціле r;

        assert(parse_item);

        STRV_FOREACH(word, args) {
                char *key, *value;

                key = mangle_word(*word, flags);
                якщо (!key)
                        continue;

                value = strchr(key, '=');
                якщо (value)
                        *(value++) = '\0';

                r = parse_item(key, value, data);
                якщо (r < 0)
                        повернути r;
        }

        повернути 0;
}

ціле proc_cmdline_parse(proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ char **args = NULL;
        ціле r;

        assert(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        якщо (r < 0)
                повернути r;

        повернути proc_cmdline_parse_strv(args, parse_item, data, flags);
}

static bool relaxed_equal_char(char a, char b) {
        повернути a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

char* proc_cmdline_key_startswith(const char *s, const char *prefix) {
        assert(s);
        assert(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        для (; *prefix != 0; s++, prefix++)
                якщо (!relaxed_equal_char(*s, *prefix))
                        повернути NULL;

        повернути (char*) s;
}

bool proc_cmdline_key_streq(const char *x, const char *y) {
        assert(x);
        assert(y);

        /* Much like streq(), but considers "-" and "_" the same */

        для (; *x != 0 || *y != 0; x++, y++)
                якщо (!relaxed_equal_char(*x, *y))
                        повернути false;

        повернути true;
}

static ціле cmdline_get_key(char **args, const char *key, ProcCmdlineFlags flags, char **ret_value) {
        _cleanup_free_ char *v = NULL;
        bool found = false;
        ціле r;

        assert(key);

        STRV_FOREACH(p, args) {
                const char *word;

                word = mangle_word(*p, flags);
                якщо (!word)
                        continue;

                якщо (ret_value) {
                        const char *e;

                        e = proc_cmdline_key_startswith(word, key);
                        якщо (!e)
                                continue;

                        якщо (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                якщо (r < 0)
                                        повернути r;

                                found = true;

                        } інакше якщо (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL))
                                found = true;

                } інакше {
                        якщо (proc_cmdline_key_streq(word, key)) {
                                found = true;
                                break; /* we found what we were looking for */
                        }
                }
        }

        якщо (ret_value)
                *ret_value = TAKE_PTR(v);

        повернути found;
}

ціле proc_cmdline_get_key(const char *key, ProcCmdlineFlags flags, char **ret_value) {
        _cleanup_strv_free_ char **args = NULL;
        ціле r;

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

        якщо (isempty(key))
                повернути -EINVAL;

        якщо (FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL) && !ret_value)
                повернути -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        якщо (r < 0)
                повернути r;

        повернути cmdline_get_key(args, key, flags, ret_value);
}

ціле proc_cmdline_get_bool(const char *key, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ char *v = NULL;
        ціле r;

        assert(ret);

        r = proc_cmdline_get_key(key, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_VALUE_OPTIONAL, &v);
        якщо (r < 0)
                повернути r;
        якщо (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                повернути 0;
        }

        якщо (v) { /* key with parameter passed */
                r = parse_boolean(v);
                якщо (r < 0)
                        повернути r;
                *ret = r;
        } інакше /* key without parameter passed */
                *ret = true;

        повернути 1;
}

static ціле cmdline_get_key_ap(ProcCmdlineFlags flags, char* const* args, va_list ap) {
        ціле r, ret = 0;

        для (;;) {
                char **v;
                const char *k, *e;

                k = va_arg(ap, const char*);
                якщо (!k)
                        break;

                assert_se(v = va_arg(ap, char**));

                STRV_FOREACH(p, args) {
                        const char *word;

                        word = mangle_word(*p, flags);
                        якщо (!word)
                                continue;

                        e = proc_cmdline_key_startswith(word, k);
                        якщо (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                якщо (r < 0)
                                        повернути r;

                                ret++;
                        }
                }
        }

        повернути ret;
}

ціле proc_cmdline_get_key_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ char **args = NULL;
        ціле r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        якщо (r < 0)
                повернути r;

        va_start(ap, flags);
        r = cmdline_get_key_ap(flags, args, ap);
        va_end(ap);

        повернути r;
}

bool proc_cmdline_value_missing(const char *key, const char *value) {
        assert(key);

        якщо (!value) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", key);
                повернути true;
        }

        повернути false;
}
