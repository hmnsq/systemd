/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "env-file.h"
#include "env-util.h"
#include "escape.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "label-util.h"
#include "log.h"
#include "string-util.h"
#include "strv.h"
#include "tmpfile-util.h"
#include "utf8.h"

typedef цео (*push_env_func_t)(
                const char *filename,
                unsigned line,
                const char *key,
                char *value,
                void *userdata);

static цео parse_env_file_internal(
                FILE *f,
                const char *fname,
                push_env_func_t push,
                void *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ char *contents = NULL, *key = NULL, *value = NULL;
        unsigned line = 1;
        цео r;

        enum {
                PRE_KEY,
                KEY,
                PRE_VALUE,
                VALUE,
                VALUE_ESCAPE,
                SINGLE_QUOTE_VALUE,
                DOUBLE_QUOTE_VALUE,
                DOUBLE_QUOTE_VALUE_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_KEY;

        assert(f || fname);
        assert(push);

        ако (f)
                r = read_full_stream(f, &contents, NULL);
        инако
                r = read_full_file(fname, &contents, NULL);
        ако (r < 0)
                врати r;

        за (char *p = contents; *p; p++) {
                char c = *p;

                switch (state) {

                case PRE_KEY:
                        ако (strchr(COMMENTS, c))
                                state = COMMENT;
                        инако ако (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                ако (!GREEDY_REALLOC(key, n_key+2))
                                        врати -ENOMEM;

                                key[n_key++] = c;
                        }
                        break;

                case KEY:
                        ако (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } инако ако (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } инако {
                                ако (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                инако ако (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                ако (!GREEDY_REALLOC(key, n_key+2))
                                        врати -ENOMEM;

                                key[n_key++] = c;
                        }

                        break;

                case PRE_VALUE:
                        ако (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                ако (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                ако (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ако (r < 0)
                                        врати r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } инако ако (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        инако ако (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        инако ако (c == '\\')
                                state = VALUE_ESCAPE;
                        инако ако (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                ако (!GREEDY_REALLOC(value, n_value+2))
                                        врати -ENOMEM;

                                value[n_value++] = c;
                        }

                        break;

                case VALUE:
                        ако (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                ако (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                ако (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ако (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ако (r < 0)
                                        врати r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } инако ако (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } инако {
                                ако (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                инако ако (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                ако (!GREEDY_REALLOC(value, n_value+2))
                                        врати -ENOMEM;

                                value[n_value++] = c;
                        }

                        break;

                case VALUE_ESCAPE:
                        state = VALUE;

                        ако (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ако (!GREEDY_REALLOC(value, n_value+2))
                                        врати -ENOMEM;

                                value[n_value++] = c;
                        }
                        break;

                case SINGLE_QUOTE_VALUE:
                        ако (c == '\'')
                                state = PRE_VALUE;
                        инако {
                                ако (!GREEDY_REALLOC(value, n_value+2))
                                        врати -ENOMEM;

                                value[n_value++] = c;
                        }

                        break;

                case DOUBLE_QUOTE_VALUE:
                        ако (c == '"')
                                state = PRE_VALUE;
                        инако ако (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        инако {
                                ако (!GREEDY_REALLOC(value, n_value+2))
                                        врати -ENOMEM;

                                value[n_value++] = c;
                        }

                        break;

                case DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        ако (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ако (!GREEDY_REALLOC(value, n_value+2))
                                        врати -ENOMEM;
                                value[n_value++] = c;
                        } инако ако (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ако (!GREEDY_REALLOC(value, n_value+3))
                                        врати -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        break;

                case COMMENT:
                        ако (c == '\\')
                                state = COMMENT_ESCAPE;
                        инако ако (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        break;

                case COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ако (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } инако
                                state = COMMENT;
                        break;
                }
        }

        ако (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                ако (value)
                        value[n_value] = 0;

                ако (state == VALUE)
                        ако (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                ако (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                ако (r < 0)
                        врати r;

                value = NULL;
        }

        врати 0;
}

static цео check_utf8ness_and_warn(
                const char *filename, unsigned line,
                const char *key, char *value) {

        assert(key);

        ако (!utf8_is_valid(key)) {
                _cleanup_free_ char *p = NULL;

                p = utf8_escape_invalid(key);
                врати log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        ако (value && !utf8_is_valid(value)) {
                _cleanup_free_ char *p = NULL;

                p = utf8_escape_invalid(value);
                врати log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        врати 0;
}

static цео parse_env_file_push(
                const char *filename, unsigned line,
                const char *key, char *value,
                void *userdata) {

        const char *k;
        va_list aq, *ap = userdata;
        цео r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ако (r < 0)
                врати r;

        va_copy(aq, *ap);

        док ((k = va_arg(aq, const char *))) {
                char **v;

                v = va_arg(aq, char **);

                ако (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        врати 1;
                }
        }

        va_end(aq);
        free(value);

        врати 0;
}

цео parse_env_filev(
                FILE *f,
                const char *fname,
                va_list ap) {

        цео r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        врати r;
}

цео parse_env_file_fdv(цео fd, const char *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        цео r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ако (r < 0)
                врати r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        врати r;
}

цео parse_env_file_sentinel(
                FILE *f,
                const char *fname,
                ...) {

        va_list ap;
        цео r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        врати r;
}

цео parse_env_file_fd_sentinel(
                цео fd,
                const char *fname, /* only used for logging */
                ...) {

        va_list ap;
        цео r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        врати r;
}

цео parse_env_datav(
                const char *data,
                size_t size,
                const char *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        ако (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        ако (!f)
                врати -ENOMEM;

        врати parse_env_filev(f, fname, ap);
}

цео parse_env_data_sentinel(
                const char *data,
                size_t size,
                const char *fname, /* only used for logging */
                ...) {

        va_list ap;
        цео r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        врати r;
}

static цео load_env_file_push(
                const char *filename, unsigned line,
                const char *key, char *value,
                void *userdata) {

        char ***m = userdata;
        char *p;
        цео r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ако (r < 0)
                врати r;

        p = strjoin(key, "=", value);
        ако (!p)
                врати -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ако (r < 0)
                врати r;

        free(value);
        врати 0;
}

цео load_env_file(FILE *f, const char *fname, char ***ret) {
        _cleanup_strv_free_ char **m = NULL;
        цео r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        ако (r < 0)
                врати r;

        *ret = TAKE_PTR(m);
        врати 0;
}

static цео load_env_file_push_pairs(
                const char *filename, unsigned line,
                const char *key, char *value,
                void *userdata) {

        char ***m = ASSERT_PTR(userdata);
        цео r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ако (r < 0)
                врати r;

        /* Check if the key is present */
        за (char **t = *m; t && *t; t += 2)
                ако (streq(t[0], key)) {
                        ако (value)
                                врати free_and_replace(t[1], value);
                        инако
                                врати free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        ако (r < 0)
                врати r;

        ако (value)
                врати strv_push(m, value);
        инако
                врати strv_extend(m, "");
}

цео load_env_file_pairs(FILE *f, const char *fname, char ***ret) {
        _cleanup_strv_free_ char **m = NULL;
        цео r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        ако (r < 0)
                врати r;

        *ret = TAKE_PTR(m);
        врати 0;
}

цео load_env_file_pairs_fd(цео fd, const char *fname, char ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        цео r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ако (r < 0)
                врати r;

        врати load_env_file_pairs(f, fname, ret);
}

static цео merge_env_file_push(
                const char *filename, unsigned line,
                const char *key, char *value,
                void *userdata) {

        char ***env = ASSERT_PTR(userdata);
        char *expanded_value;
        цео r;

        assert(key);

        ако (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                врати 0;
        }

        ако (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                врати 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        ако (r < 0)
                врати log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        врати load_env_file_push(filename, line, key, value, env);
}

цео merge_env_file(
                char ***env,
                FILE *f,
                const char *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        врати parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const char *p) {
        assert(f);
        assert(p);

        flockfile(f);

        ако (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                за (; *p; p++) {
                        ако (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } инако
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const char *k, const char *v) {
        assert(f);
        assert(k);

        ако (!v)
                врати;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const char *v) {
        const char *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        ако (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                врати;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

цео write_env_file_label(цео dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        цео r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        ако (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ако (r < 0)
                        врати r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ char *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ако (r < 0) {
                ако (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                врати r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ако (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                ако (r < 0)
                        врати r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ако (r < 0)
                врати r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ако (r < 0)
                врати r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        врати 0;
}

цео write_vconsole_conf(цео dir_fd, const char *fname, char **l) {
        char **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        врати write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
