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

typedef ចំនួនគត់ (*push_env_func_t)(
                const var_na *filename,
                unsigned line,
                const var_na *key,
                var_na *value,
                void *userdata);

static ចំនួនគត់ parse_env_file_internal(
                FILE *f,
                const var_na *fname,
                push_env_func_t push,
                void *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ var_na *contents = NULL, *key = NULL, *value = NULL;
        unsigned line = 1;
        ចំនួនគត់ r;

        ga_nanaa {
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

        ប្រសិនបើ (f)
                r = read_full_stream(f, &contents, NULL);
        បើមិនដូច្នេះ
                r = read_full_file(fname, &contents, NULL);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        សម្រាប់ (var_na *p = contents; *p; p++) {
                var_na c = *p;

                switch (state) {

                pak_sa PRE_KEY:
                        ប្រសិនបើ (strchr(COMMENTS, c))
                                state = COMMENT;
                        បើមិនដូច្នេះ ប្រសិនបើ (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                ប្រសិនបើ (!GREEDY_REALLOC(key, n_key+2))
                                        ត្រឡប់ -ENOMEM;

                                key[n_key++] = c;
                        }
                        bha_nga;

                pak_sa KEY:
                        ប្រសិនបើ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } បើមិនដូច្នេះ ប្រសិនបើ (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } បើមិនដូច្នេះ {
                                ប្រសិនបើ (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                បើមិនដូច្នេះ ប្រសិនបើ (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                ប្រសិនបើ (!GREEDY_REALLOC(key, n_key+2))
                                        ត្រឡប់ -ENOMEM;

                                key[n_key++] = c;
                        }

                        bha_nga;

                pak_sa PRE_VALUE:
                        ប្រសិនបើ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                ប្រសិនបើ (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                ប្រសិនបើ (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ប្រសិនបើ (r < 0)
                                        ត្រឡប់ r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } បើមិនដូច្នេះ ប្រសិនបើ (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        បើមិនដូច្នេះ ប្រសិនបើ (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        បើមិនដូច្នេះ ប្រសិនបើ (c == '\\')
                                state = VALUE_ESCAPE;
                        បើមិនដូច្នេះ ប្រសិនបើ (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                ប្រសិនបើ (!GREEDY_REALLOC(value, n_value+2))
                                        ត្រឡប់ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE:
                        ប្រសិនបើ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                ប្រសិនបើ (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                ប្រសិនបើ (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ប្រសិនបើ (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ប្រសិនបើ (r < 0)
                                        ត្រឡប់ r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } បើមិនដូច្នេះ ប្រសិនបើ (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } បើមិនដូច្នេះ {
                                ប្រសិនបើ (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                បើមិនដូច្នេះ ប្រសិនបើ (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                ប្រសិនបើ (!GREEDY_REALLOC(value, n_value+2))
                                        ត្រឡប់ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE_ESCAPE:
                        state = VALUE;

                        ប្រសិនបើ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ប្រសិនបើ (!GREEDY_REALLOC(value, n_value+2))
                                        ត្រឡប់ -ENOMEM;

                                value[n_value++] = c;
                        }
                        bha_nga;

                pak_sa SINGLE_QUOTE_VALUE:
                        ប្រសិនបើ (c == '\'')
                                state = PRE_VALUE;
                        បើមិនដូច្នេះ {
                                ប្រសិនបើ (!GREEDY_REALLOC(value, n_value+2))
                                        ត្រឡប់ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE:
                        ប្រសិនបើ (c == '"')
                                state = PRE_VALUE;
                        បើមិនដូច្នេះ ប្រសិនបើ (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        បើមិនដូច្នេះ {
                                ប្រសិនបើ (!GREEDY_REALLOC(value, n_value+2))
                                        ត្រឡប់ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        ប្រសិនបើ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ប្រសិនបើ (!GREEDY_REALLOC(value, n_value+2))
                                        ត្រឡប់ -ENOMEM;
                                value[n_value++] = c;
                        } បើមិនដូច្នេះ ប្រសិនបើ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ប្រសិនបើ (!GREEDY_REALLOC(value, n_value+3))
                                        ត្រឡប់ -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        bha_nga;

                pak_sa COMMENT:
                        ប្រសិនបើ (c == '\\')
                                state = COMMENT_ESCAPE;
                        បើមិនដូច្នេះ ប្រសិនបើ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        bha_nga;

                pak_sa COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ប្រសិនបើ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } បើមិនដូច្នេះ
                                state = COMMENT;
                        bha_nga;
                }
        }

        ប្រសិនបើ (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                ប្រសិនបើ (value)
                        value[n_value] = 0;

                ប្រសិនបើ (state == VALUE)
                        ប្រសិនបើ (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                ប្រសិនបើ (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                value = NULL;
        }

        ត្រឡប់ 0;
}

static ចំនួនគត់ check_utf8ness_and_warn(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value) {

        assert(key);

        ប្រសិនបើ (!utf8_is_valid(key)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(key);
                ត្រឡប់ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        ប្រសិនបើ (value && !utf8_is_valid(value)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(value);
                ត្រឡប់ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        ត្រឡប់ 0;
}

static ចំនួនគត់ parse_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        const var_na *k;
        va_list aq, *ap = userdata;
        ចំនួនគត់ r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        va_copy(aq, *ap);

        ខណៈពេល ((k = va_arg(aq, const var_na *))) {
                var_na **v;

                v = va_arg(aq, var_na **);

                ប្រសិនបើ (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        ត្រឡប់ 1;
                }
        }

        va_end(aq);
        free(value);

        ត្រឡប់ 0;
}

ចំនួនគត់ parse_env_filev(
                FILE *f,
                const var_na *fname,
                va_list ap) {

        ចំនួនគត់ r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ត្រឡប់ r;
}

ចំនួនគត់ parse_env_file_fdv(ចំនួនគត់ fd, const var_na *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        ចំនួនគត់ r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ត្រឡប់ r;
}

ចំនួនគត់ parse_env_file_sentinel(
                FILE *f,
                const var_na *fname,
                ...) {

        va_list ap;
        ចំនួនគត់ r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ត្រឡប់ r;
}

ចំនួនគត់ parse_env_file_fd_sentinel(
                ចំនួនគត់ fd,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        ចំនួនគត់ r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ត្រឡប់ r;
}

ចំនួនគត់ parse_env_datav(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        ប្រសិនបើ (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        ប្រសិនបើ (!f)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ parse_env_filev(f, fname, ap);
}

ចំនួនគត់ parse_env_data_sentinel(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        ចំនួនគត់ r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        ត្រឡប់ r;
}

static ចំនួនគត់ load_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = userdata;
        var_na *p;
        ចំនួនគត់ r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        p = strjoin(key, "=", value);
        ប្រសិនបើ (!p)
                ត្រឡប់ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        free(value);
        ត្រឡប់ 0;
}

ចំនួនគត់ load_env_file(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        ចំនួនគត់ r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *ret = TAKE_PTR(m);
        ត្រឡប់ 0;
}

static ចំនួនគត់ load_env_file_push_pairs(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = ASSERT_PTR(userdata);
        ចំនួនគត់ r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* Check if the key is present */
        សម្រាប់ (var_na **t = *m; t && *t; t += 2)
                ប្រសិនបើ (streq(t[0], key)) {
                        ប្រសិនបើ (value)
                                ត្រឡប់ free_and_replace(t[1], value);
                        បើមិនដូច្នេះ
                                ត្រឡប់ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (value)
                ត្រឡប់ strv_push(m, value);
        បើមិនដូច្នេះ
                ត្រឡប់ strv_extend(m, "");
}

ចំនួនគត់ load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        ចំនួនគត់ r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *ret = TAKE_PTR(m);
        ត្រឡប់ 0;
}

ចំនួនគត់ load_env_file_pairs_fd(ចំនួនគត់ fd, const var_na *fname, var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        ចំនួនគត់ r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ load_env_file_pairs(f, fname, ret);
}

static ចំនួនគត់ merge_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***env = ASSERT_PTR(userdata);
        var_na *expanded_value;
        ចំនួនគត់ r;

        assert(key);

        ប្រសិនបើ (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                ត្រឡប់ 0;
        }

        ប្រសិនបើ (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                ត្រឡប់ 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        ត្រឡប់ load_env_file_push(filename, line, key, value, env);
}

ចំនួនគត់ merge_env_file(
                var_na ***env,
                FILE *f,
                const var_na *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ត្រឡប់ parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const var_na *p) {
        assert(f);
        assert(p);

        flockfile(f);

        ប្រសិនបើ (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                សម្រាប់ (; *p; p++) {
                        ប្រសិនបើ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } បើមិនដូច្នេះ
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v) {
        assert(f);
        assert(k);

        ប្រសិនបើ (!v)
                ត្រឡប់;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const var_na *v) {
        const var_na *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        ប្រសិនបើ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ត្រឡប់;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

ចំនួនគត់ write_env_file_label(ចំនួនគត់ dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        ចំនួនគត់ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        ប្រសិនបើ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ប្រសិនបើ (r < 0) {
                ប្រសិនបើ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ត្រឡប់ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ប្រសិនបើ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ត្រឡប់ 0;
}

ចំនួនគត់ write_vconsole_conf(ចំនួនគត់ dir_fd, const var_na *fname, var_na **l) {
        var_na **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ត្រឡប់ write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
