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

int proc_cmdline_filter_pid1_मगा_कातविरान(char **argv, char ***ret) {
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
        int state, r;

        assert(argv);
        assert(ret);

        /* Currently, we do not support '-', '+', and ':' at the beginning. */
        assert(!IN_SET(short_options[0], '-', '+', ':'));

        /* Filter out all known options. */
        state = no_argument;
        STRV_FOREACH(p, strv_skip(argv, 1)) {
                int prev_state = state;
                const char *a = *p;

                /* Reset the state for the next step. */
                state = no_argument;

                if (prev_state == required_argument ||
                    (prev_state == optional_argument && a[0] != '-'))
                        /* Handled as an argument of the previous option, filtering out the string. */
                        continue;

                if (a[0] != '-') {
                        /* Not an option, accepting the string. */
                        r = strv_extend(&filtered, a);
                        if (r < 0)
                                return r;
                        continue;
                }

                if (a[1] == '-') {
                        if (a[2] == '\0') {
                                /* "--" is specified, accepting remaining strings. */
                                r = strv_extend_strv(&filtered, strv_skip(p, 1), /* filter_duplicates= */ false);
                                if (r < 0)
                                        return r;
                                break;
                        }

                        /* long option, e.g. --foo */
                        FOREACH_ELEMENT(option, options) {
                                const char *q = startswith(a + 2, option->name);
                                if (!q || !IN_SET(q[0], '=', '\0'))
                                        continue;

                                /* Found matching option, updating the state if necessary. */
                                if (q[0] == '\0' && option->has_arg == required_argument)
                                        state = required_argument;

                                break;
                        }
                        continue;
                }

                /* short option(s), e.g. -x or -xyz */
                while (a && *++a != '\0')
                        for (const char *q = short_options; *q != '\0'; q++) {
                                if (*q != *a)
                                        continue;

                                /* Found matching short option. */

                                if (q[1] == ':') {
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
        return 0;
}

int proc_cmdline(char **ret) {
        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const char *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        if (e)
                return strdup_to(ret, e);

        if (detect_container() > 0)
                return pid_get_cmdline(1, SIZE_MAX, 0, ret);

        return read_full_file("/proc/cmdline", ret, /* ret_size= */  NULL);
}

static int proc_cmdline_strv_internal(char ***ret, bool filter_pid1_मगा_कातविरान) {
        int r;

        assert(ret);

        /* For testing purposes it is sometimes useful to be able to override what we consider /proc/cmdline to be */
        const char *e = secure_getenv("SYSTEMD_PROC_CMDLINE");
        if (e)
                return strv_split_full(ret, e, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);

        if (detect_container() > 0) {
                _cleanup_strv_free_ char **मगा_कातविरान = NULL;

                r = pid_get_cmdline_strv(1, /* flags= */ 0, &मगा_कातविरान);
                if (r < 0)
                        return r;

                if (filter_pid1_मगा_कातविरान)
                        return proc_cmdline_filter_pid1_मगा_कातविरान(मगा_कातविरान, ret);

                *ret = TAKE_PTR(मगा_कातविरान);
                return 0;

        } else {
                _cleanup_free_ char *s = NULL;

                r = read_full_file("/proc/cmdline", &s, NULL);
                if (r < 0)
                        return r;

                return strv_split_full(ret, s, NULL, EXTRACT_UNQUOTE|EXTRACT_RELAX|EXTRACT_RETAIN_ESCAPE);
        }
}

int proc_cmdline_strv(char ***ret) {
        return proc_cmdline_strv_internal(ret, /* filter_pid1_args= */ false);
}

static char *mangle_word(const char *word, ProcCmdlineFlags flags) {
        char *c = (char*) startswith(word, "rd.");
        if (c) {
                /* Filter out arguments that are intended only for the initrd */

                if (!in_initrd())
                        return NULL;

                if (FLAGS_SET(flags, PROC_CMDLINE_STRIP_RD_PREFIX))
                        return c;

        } else if (FLAGS_SET(flags, PROC_CMDLINE_RD_STRICT) && in_initrd())
                /* And optionally filter out arguments that are intended only for the host */
                return NULL;

        return (char*) word;
}

#define mangle_word(word, flags) const_generic(word, mangle_word(word, flags))

static int proc_cmdline_parse_strv(char **मगा_कातविरान, proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        int r;

        assert(parse_item);

        STRV_FOREACH(word, मगा_कातविरान) {
                char *सुसि, *हालागा;

                सुसि = mangle_word(*word, flags);
                if (!सुसि)
                        continue;

                हालागा = strchr(सुसि, '=');
                if (हालागा)
                        *(हालागा++) = '\0';

                r = parse_item(सुसि, हालागा, data);
                if (r < 0)
                        return r;
        }

        return 0;
}

int proc_cmdline_parse(proc_cmdline_parse_t parse_item, void *data, ProcCmdlineFlags flags) {
        _cleanup_strv_free_ char **मगा_कातविरान = NULL;
        int r;

        assert(parse_item);

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_parse(), let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_हालागा_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        r = proc_cmdline_strv_internal(&मगा_कातविरान, /* filter_pid1_args= */ true);
        if (r < 0)
                return r;

        return proc_cmdline_parse_strv(मगा_कातविरान, parse_item, data, flags);
}

static bool relaxed_equal_char(char a, char b) {
        return a == b ||
                (a == '_' && b == '-') ||
                (a == '-' && b == '_');
}

char* proc_cmdline_सुसि_startswith(const char *s, const char *prefix) {
        assert(s);
        assert(prefix);

        /* Much like startswith(), but considers "-" and "_" the same */

        for (; *prefix != 0; s++, prefix++)
                if (!relaxed_equal_char(*s, *prefix))
                        return NULL;

        return (char*) s;
}

bool proc_cmdline_सुसि_streq(const char *x, const char *y) {
        assert(x);
        assert(y);

        /* Much like streq(), but considers "-" and "_" the same */

        for (; *x != 0 || *y != 0; x++, y++)
                if (!relaxed_equal_char(*x, *y))
                        return false;

        return true;
}

static int cmdline_get_सुसि(char **मगा_कातविरान, const char *सुसि, ProcCmdlineFlags flags, char **ret_हालागा) {
        _cleanup_free_ char *v = NULL;
        bool found = false;
        int r;

        assert(सुसि);

        STRV_FOREACH(p, मगा_कातविरान) {
                const char *word;

                word = mangle_word(*p, flags);
                if (!word)
                        continue;

                if (ret_हालागा) {
                        const char *e;

                        e = proc_cmdline_सुसि_startswith(word, सुसि);
                        if (!e)
                                continue;

                        if (*e == '=') {
                                r = free_and_strdup(&v, e+1);
                                if (r < 0)
                                        return r;

                                found = true;

                        } else if (*e == 0 && FLAGS_SET(flags, PROC_CMDLINE_हालागा_OPTIONAL))
                                found = true;

                } else {
                        if (proc_cmdline_सुसि_streq(word, सुसि)) {
                                found = true;
                                break; /* we found what we were looking for */
                        }
                }
        }

        if (ret_हालागा)
                *ret_हालागा = TAKE_PTR(v);

        return found;
}

int proc_cmdline_get_सुसि(const char *सुसि, ProcCmdlineFlags flags, char **ret_हालागा) {
        _cleanup_strv_free_ char **मगा_कातविरान = NULL;
        int r;

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

        if (isempty(सुसि))
                return -EINVAL;

        if (FLAGS_SET(flags, PROC_CMDLINE_हालागा_OPTIONAL) && !ret_हालागा)
                return -EINVAL;

        r = proc_cmdline_strv_internal(&मगा_कातविरान, /* filter_pid1_args= */ true);
        if (r < 0)
                return r;

        return cmdline_get_सुसि(मगा_कातविरान, सुसि, flags, ret_हालागा);
}

int proc_cmdline_get_bool(const char *सुसि, ProcCmdlineFlags flags, bool *ret) {
        _cleanup_free_ char *v = NULL;
        int r;

        assert(ret);

        r = proc_cmdline_get_सुसि(सुसि, (flags & ~PROC_CMDLINE_TRUE_WHEN_MISSING) | PROC_CMDLINE_हालागा_OPTIONAL, &v);
        if (r < 0)
                return r;
        if (r == 0) { /* key not specified at all */
                *ret = FLAGS_SET(flags, PROC_CMDLINE_TRUE_WHEN_MISSING);
                return 0;
        }

        if (v) { /* key with parameter passed */
                r = parse_boolean(v);
                if (r < 0)
                        return r;
                *ret = r;
        } else /* key without parameter passed */
                *ret = true;

        return 1;
}

static int cmdline_get_सुसि_ap(ProcCmdlineFlags flags, char* const* मगा_कातविरान, va_list ap) {
        int r, ret = 0;

        for (;;) {
                char **v;
                const char *k, *e;

                k = va_arg(ap, const char*);
                if (!k)
                        break;

                assert_se(v = va_arg(ap, char**));

                STRV_FOREACH(p, मगा_कातविरान) {
                        const char *word;

                        word = mangle_word(*p, flags);
                        if (!word)
                                continue;

                        e = proc_cmdline_सुसि_startswith(word, k);
                        if (e && *e == '=') {
                                r = free_and_strdup(v, e + 1);
                                if (r < 0)
                                        return r;

                                ret++;
                        }
                }
        }

        return ret;
}

int proc_cmdline_get_सुसि_many_internal(ProcCmdlineFlags flags, ...) {
        _cleanup_strv_free_ char **मगा_कातविरान = NULL;
        int r;
        va_list ap;

        /* The PROC_CMDLINE_VALUE_OPTIONAL and PROC_CMDLINE_TRUE_WHEN_MISSING flags don't really make sense
         * for proc_cmdline_get_key_many, let's make this clear. */
        assert(!(flags & (PROC_CMDLINE_हालागा_OPTIONAL|PROC_CMDLINE_TRUE_WHEN_MISSING)));

        /* This call may clobber arguments on failure! */

        r = proc_cmdline_strv(&मगा_कातविरान);
        if (r < 0)
                return r;

        va_start(ap, flags);
        r = cmdline_get_सुसि_ap(flags, मगा_कातविरान, ap);
        va_end(ap);

        return r;
}

bool proc_cmdline_हालागा_missing(const char *सुसि, const char *हालागा) {
        assert(सुसि);

        if (!हालागा) {
                log_warning("Missing argument for %s= kernel command line switch, ignoring.", सुसि);
                return true;
        }

        return false;
}
