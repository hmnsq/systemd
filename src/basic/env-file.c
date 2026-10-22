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

typedef целое (*push_env_func_t)(
                const char *filename,
                unsigned line,
                const char *key,
                char *value,
                void *userdata);

static целое parse_env_file_internal(
                FILE *f,
                const char *fname,
                push_env_func_t push,
                void *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ char *contents = NULL, *key = NULL, *value = NULL;
        unsigned line = 1;
        целое r;

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

        если (f)
                r = read_full_stream(f, &contents, NULL);
        иначе
                r = read_full_file(fname, &contents, NULL);
        если (r < 0)
                вернуть r;

        для (char *p = contents; *p; p++) {
                char c = *p;

                switch (state) {

                case PRE_KEY:
                        если (strchr(COMMENTS, c))
                                state = COMMENT;
                        иначе если (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                если (!GREEDY_REALLOC(key, n_key+2))
                                        вернуть -ENOMEM;

                                key[n_key++] = c;
                        }
                        break;

                case KEY:
                        если (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } иначе если (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } иначе {
                                если (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                иначе если (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                если (!GREEDY_REALLOC(key, n_key+2))
                                        вернуть -ENOMEM;

                                key[n_key++] = c;
                        }

                        break;

                case PRE_VALUE:
                        если (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                если (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                если (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                если (r < 0)
                                        вернуть r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } иначе если (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        иначе если (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        иначе если (c == '\\')
                                state = VALUE_ESCAPE;
                        иначе если (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                если (!GREEDY_REALLOC(value, n_value+2))
                                        вернуть -ENOMEM;

                                value[n_value++] = c;
                        }

                        break;

                case VALUE:
                        если (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                если (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                если (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                если (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                если (r < 0)
                                        вернуть r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } иначе если (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } иначе {
                                если (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                иначе если (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                если (!GREEDY_REALLOC(value, n_value+2))
                                        вернуть -ENOMEM;

                                value[n_value++] = c;
                        }

                        break;

                case VALUE_ESCAPE:
                        state = VALUE;

                        если (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                если (!GREEDY_REALLOC(value, n_value+2))
                                        вернуть -ENOMEM;

                                value[n_value++] = c;
                        }
                        break;

                case SINGLE_QUOTE_VALUE:
                        если (c == '\'')
                                state = PRE_VALUE;
                        иначе {
                                если (!GREEDY_REALLOC(value, n_value+2))
                                        вернуть -ENOMEM;

                                value[n_value++] = c;
                        }

                        break;

                case DOUBLE_QUOTE_VALUE:
                        если (c == '"')
                                state = PRE_VALUE;
                        иначе если (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        иначе {
                                если (!GREEDY_REALLOC(value, n_value+2))
                                        вернуть -ENOMEM;

                                value[n_value++] = c;
                        }

                        break;

                case DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        если (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                если (!GREEDY_REALLOC(value, n_value+2))
                                        вернуть -ENOMEM;
                                value[n_value++] = c;
                        } иначе если (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                если (!GREEDY_REALLOC(value, n_value+3))
                                        вернуть -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        break;

                case COMMENT:
                        если (c == '\\')
                                state = COMMENT_ESCAPE;
                        иначе если (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        break;

                case COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        если (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } иначе
                                state = COMMENT;
                        break;
                }
        }

        если (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                если (value)
                        value[n_value] = 0;

                если (state == VALUE)
                        если (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                если (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                если (r < 0)
                        вернуть r;

                value = NULL;
        }

        вернуть 0;
}

static целое check_utf8ness_and_warn(
                const char *filename, unsigned line,
                const char *key, char *value) {

        assert(key);

        если (!utf8_is_valid(key)) {
                _cleanup_free_ char *p = NULL;

                p = utf8_escape_invalid(key);
                вернуть log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        если (value && !utf8_is_valid(value)) {
                _cleanup_free_ char *p = NULL;

                p = utf8_escape_invalid(value);
                вернуть log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        вернуть 0;
}

static целое parse_env_file_push(
                const char *filename, unsigned line,
                const char *key, char *value,
                void *userdata) {

        const char *k;
        va_list aq, *ap = userdata;
        целое r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        если (r < 0)
                вернуть r;

        va_copy(aq, *ap);

        пока ((k = va_arg(aq, const char *))) {
                char **v;

                v = va_arg(aq, char **);

                если (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        вернуть 1;
                }
        }

        va_end(aq);
        free(value);

        вернуть 0;
}

целое parse_env_filev(
                FILE *f,
                const char *fname,
                va_list ap) {

        целое r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        вернуть r;
}

целое parse_env_file_fdv(целое fd, const char *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        целое r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        если (r < 0)
                вернуть r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        вернуть r;
}

целое parse_env_file_sentinel(
                FILE *f,
                const char *fname,
                ...) {

        va_list ap;
        целое r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        вернуть r;
}

целое parse_env_file_fd_sentinel(
                целое fd,
                const char *fname, /* only used for logging */
                ...) {

        va_list ap;
        целое r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        вернуть r;
}

целое parse_env_datav(
                const char *data,
                size_t size,
                const char *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        если (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        если (!f)
                вернуть -ENOMEM;

        вернуть parse_env_filev(f, fname, ap);
}

целое parse_env_data_sentinel(
                const char *data,
                size_t size,
                const char *fname, /* only used for logging */
                ...) {

        va_list ap;
        целое r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        вернуть r;
}

static целое load_env_file_push(
                const char *filename, unsigned line,
                const char *key, char *value,
                void *userdata) {

        char ***m = userdata;
        char *p;
        целое r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        если (r < 0)
                вернуть r;

        p = strjoin(key, "=", value);
        если (!p)
                вернуть -ENOMEM;

        r = strv_env_replace_consume(m, p);
        если (r < 0)
                вернуть r;

        free(value);
        вернуть 0;
}

целое load_env_file(FILE *f, const char *fname, char ***ret) {
        _cleanup_strv_free_ char **m = NULL;
        целое r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        если (r < 0)
                вернуть r;

        *ret = TAKE_PTR(m);
        вернуть 0;
}

static целое load_env_file_push_pairs(
                const char *filename, unsigned line,
                const char *key, char *value,
                void *userdata) {

        char ***m = ASSERT_PTR(userdata);
        целое r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        если (r < 0)
                вернуть r;

        /* Check if the key is present */
        для (char **t = *m; t && *t; t += 2)
                если (streq(t[0], key)) {
                        если (value)
                                вернуть free_and_replace(t[1], value);
                        иначе
                                вернуть free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        если (r < 0)
                вернуть r;

        если (value)
                вернуть strv_push(m, value);
        иначе
                вернуть strv_extend(m, "");
}

целое load_env_file_pairs(FILE *f, const char *fname, char ***ret) {
        _cleanup_strv_free_ char **m = NULL;
        целое r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        если (r < 0)
                вернуть r;

        *ret = TAKE_PTR(m);
        вернуть 0;
}

целое load_env_file_pairs_fd(целое fd, const char *fname, char ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        целое r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        если (r < 0)
                вернуть r;

        вернуть load_env_file_pairs(f, fname, ret);
}

static целое merge_env_file_push(
                const char *filename, unsigned line,
                const char *key, char *value,
                void *userdata) {

        char ***env = ASSERT_PTR(userdata);
        char *expanded_value;
        целое r;

        assert(key);

        если (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                вернуть 0;
        }

        если (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                вернуть 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        если (r < 0)
                вернуть log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        вернуть load_env_file_push(filename, line, key, value, env);
}

целое merge_env_file(
                char ***env,
                FILE *f,
                const char *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        вернуть parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const char *p) {
        assert(f);
        assert(p);

        flockfile(f);

        если (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                для (; *p; p++) {
                        если (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } иначе
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const char *k, const char *v) {
        assert(f);
        assert(k);

        если (!v)
                вернуть;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const char *v) {
        const char *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        если (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                вернуть;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

целое write_env_file_label(целое dir_fd, const char *fname, char **headers, char **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        целое r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        если (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                если (r < 0)
                        вернуть r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ char *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        если (r < 0) {
                если (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                вернуть r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        если (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                если (r < 0)
                        вернуть r;
        }

        r = fchmod_umask(fileno(f), 0644);
        если (r < 0)
                вернуть r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        если (r < 0)
                вернуть r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        вернуть 0;
}

целое write_vconsole_conf(целое dir_fd, const char *fname, char **l) {
        char **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        вернуть write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
