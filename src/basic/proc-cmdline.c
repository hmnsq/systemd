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

整数 proc_cmdline_filter_pid1_args(char **argv, char ***ret) {
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
        整数 state, r;

        assert(argv);
        assert(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        assert(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                整数 prev_state = state;
                const char *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                如果 (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        continue;

                如果 (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        如果 (r < 0)
                                返回 r;
                        continue;
                }

                如果 (a[1] == '-') {
                        如果 (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                如果 (r < 0)
                                        返回 r;
                                break;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                const char *q = startswith(a + 2, option->name);
                                如果 (!q || !IN_SET(q[0], '=', '\0'))
                                        continue;

                                /* Found matching option, updating the state if necessary. */
                                如果 (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                break;
                        }
                        continue;
                }

                /* short option(s), e.g. -x or -xyz */
                当 (a && *++a != '\0')
                        为 (const char *q = short_options; *q != '\0'; q++) {
                                如果 (*q != *a)
                                        continue;

                                /* Found matching short option. */

                                如果 (q[1] == ':') {
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
        返回 0;
}

整数 proc_cmdline(char **ret) {
        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const char *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        如果 (e)
                返回 strdup_to(ret, e);

        如果 (detect_container() > 0)
                返回 pid_get_cmdline(1, SIZE_MAX, 0, ret);

        返回 read_full_file("/proc/cmdline", ret, /* ret_size= */  NULL);
}

static 整数 proc_cmdline_strv_internal(char ***ret, bool filter_pid1_args) {
        整数 r;

        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const char *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        如果 (e)
                返回 strv_split_full(ret, e, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        如果 (detect_container() > 0) {
                _cleanup_strv_free_ char **args = NULL;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &args);
                如果 (r < 0)
                        返回 r;

                如果 (filter_pid1_args)
                        返回 proc_cmdline_filter_pid1_args(args, ret);

                *ret = TAKE_PTR(args);
                返回 0;

        } 否则 {
                _cleanup_free_ char *s = NULL;

                r = read_full_file("/proc/cmdline", &s, NULL);
                如果 (r < 0)
                        返回 r;

                返回 strv_split_full(ret, s, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

整数 proc_cmdline_strv(char ***ret) {
        返回 proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

static char *mangle_word(const char *word, ProcCmdlineFlags flags) {
        char *c = (char*) startswith(word, "rd.");
        如果 (c) {
                /* Filter out arguments that are intended only for the initrd */

                如果 (!in_initrd())
                        返回 NULL;

                如果 (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        返回 c;

        } 否则 如果 (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                返回 NULL;

        返回 (char*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

static 整数 proc_cmdline_parse_strv(char **args, proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        整数 r;

        assert(parse_item);

        STRV_FOREACH(word, args) {
                char *key, *value;

                key = mangle_word(*word, flags);
                如果 (!key)
                        continue;

                value = strchr(key, '=');
                如果 (value)
                        *(value++) = '\0';

                r = parse_item(key, value, data);
                如果 (r < 0)
                        返回 r;
        }

        返回 0;
}

整数 proc_cmdline_parse(proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ char **args = NULL;
        整数 r;

        assert(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        如果 (r < 0)
                返回 r;

        返回 proc_cmdline_parse_strv(args, parse_item, data, flags);
}

static bool relaxed_equal_char(char a, char b) {
        返回 a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

char* proc_cmdline_key_startswith(const char *s, const char *prefix) {
        assert(s);
        assert(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        为 (; *prefix != 0; s++, prefix++)
                如果 (!relaxed_equal_char(*s, *prefix))
                        返回 NULL;

        返回 (char*) s;
}

bool proc_cmdline_key_streq(const char *x, const char *y) {
        assert(x);
        assert(y);

        /* Much like streq(), but considers "-" and "_" the same */

        为 (; *x != 0 || *y != 0; x++, y++)
                如果 (!relaxed_equal_char(*x, *y))
                        返回 false;

        返回 true;
}

static 整数 cmdline_get_key(char **args, const char *key, ProcCmdlineFlags flags, char **ret_value) {
        _cleanup_free_ char *v = NULL;
        bool found = false;
        整数 r;

        assert(key);

        STRV_FOREACH(p, args) {
                const char *word;

                word = mangle_word(*p, flags);
                如果 (!word)
                        continue;

                如果 (ret_value) {
                        const char *e;

                        e = proc_cmdline_key_startswith(word, key);
                        如果 (!e)
                                continue;

                        如果 (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                如果 (r < 0)
                                        返回 r;

                                found = true;

                        } 否则 如果 (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL))
                                found = true;

                } 否则 {
                        如果 (proc_cmdline_key_streq(word, key)) {
                                found = true;
                                break; /* we found what we were looking for */
                        }
                }
        }

        如果 (ret_value)
                *ret_value = TAKE_PTR(v);

        返回 found;
}

整数 proc_cmdline_get_key(const char *key, ProcCmdlineFlags flags, char **ret_value) {
        _cleanup_strv_free_ char **args = NULL;
        整数 r;

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

        如果 (isempty(key))
                返回 -EINVAL;

        如果 (FLAGS_SET(flags, PROC_CMDLINE_VALUE_OPTIONAL) && !ret_value)
                返回 -EINVAL;

        r = proc_cmdline_strv_internal(&args, /* filter_pid1_args= */ true);
        如果 (r < 0)
                返回 r;

        返回 cmdline_get_key(args, key, flags, ret_value);
}

整数 proc_cmdline_get_bool(const char *key, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ char *v = NULL;
        整数 r;

        assert(ret);

        r = proc_cmdline_get_key(key, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_VALUE_OPTIONAL, &v);
        如果 (r < 0)
                返回 r;
        如果 (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                返回 0;
        }

        如果 (v) { /* key with parameter passed */
                r = parse_boolean(v);
                如果 (r < 0)
                        返回 r;
                *ret = r;
        } 否则 /* key without parameter passed */
                *ret = true;

        返回 1;
}

static 整数 cmdline_get_key_ap(ProcCmdlineFlags flags, char* const* args, va_list ap) {
        整数 r, ret = 0;

        为 (;;) {
                char **v;
                const char *k, *e;

                k = va_arg(ap, const char*);
                如果 (!k)
                        break;

                assert_se(v = va_arg(ap, char**));

                STRV_FOREACH(p, args) {
                        const char *word;

                        word = mangle_word(*p, flags);
                        如果 (!word)
                                continue;

                        e = proc_cmdline_key_startswith(word, k);
                        如果 (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                如果 (r < 0)
                                        返回 r;

                                ret++;
                        }
                }
        }

        返回 ret;
}

整数 proc_cmdline_get_key_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ char **args = NULL;
        整数 r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_VALUE_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&args);
        如果 (r < 0)
                返回 r;

        va_start(ap, flags);
        r = cmdline_get_key_ap(flags, args, ap);
        va_end(ap);

        返回 r;
}

bool proc_cmdline_value_missing(const char *key, const char *value) {
        assert(key);

        如果 (!value) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", key);
                返回 true;
        }

        返回 false;
}
