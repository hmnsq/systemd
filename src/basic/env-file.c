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

typedef puur_naa_nka (*push_env_func_t)(
                const var_na *filename,
                unsigned line,
                const var_na *key,
                var_na *value,
                void *userdata);

static puur_naa_nka parse_env_file_internal(
                FILE *f,
                const var_na *fname,
                push_env_func_t push,
                void *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ var_na *contents = NULL, *key = NULL, *value = NULL;
        unsigned line = 1;
        puur_naa_nka r;

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

        ᯀᯮᯝ (f)
                r = read_full_stream(f, &contents, NULL);
        ᯀᯮᯝᯂᯪ
                r = read_full_file(fname, &contents, NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        for (var_na *p = contents; *p; p++) {
                var_na c = *p;

                switch (state) {

                pak_sa PRE_KEY:
                        ᯀᯮᯝ (strchr(COMMENTS, c))
                                state = COMMENT;
                        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                ᯀᯮᯝ (!GREEDY_REALLOC(key, n_key+2))
                                        ᯀᯮᯞ -ENOMEM;

                                key[n_key++] = c;
                        }
                        bha_nga;

                pak_sa KEY:
                        ᯀᯮᯝ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } ᯀᯮᯝᯂᯪ {
                                ᯀᯮᯝ (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                ᯀᯮᯝᯂᯪ ᯀᯮᯝ (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                ᯀᯮᯝ (!GREEDY_REALLOC(key, n_key+2))
                                        ᯀᯮᯞ -ENOMEM;

                                key[n_key++] = c;
                        }

                        bha_nga;

                pak_sa PRE_VALUE:
                        ᯀᯮᯝ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                ᯀᯮᯝ (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                ᯀᯮᯝ (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ᯀᯮᯝ (r < 0)
                                        ᯀᯮᯞ r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '\\')
                                state = VALUE_ESCAPE;
                        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                ᯀᯮᯝ (!GREEDY_REALLOC(value, n_value+2))
                                        ᯀᯮᯞ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE:
                        ᯀᯮᯝ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                ᯀᯮᯝ (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                ᯀᯮᯝ (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ᯀᯮᯝ (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ᯀᯮᯝ (r < 0)
                                        ᯀᯮᯞ r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } ᯀᯮᯝᯂᯪ {
                                ᯀᯮᯝ (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                ᯀᯮᯝᯂᯪ ᯀᯮᯝ (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                ᯀᯮᯝ (!GREEDY_REALLOC(value, n_value+2))
                                        ᯀᯮᯞ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE_ESCAPE:
                        state = VALUE;

                        ᯀᯮᯝ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ᯀᯮᯝ (!GREEDY_REALLOC(value, n_value+2))
                                        ᯀᯮᯞ -ENOMEM;

                                value[n_value++] = c;
                        }
                        bha_nga;

                pak_sa SINGLE_QUOTE_VALUE:
                        ᯀᯮᯝ (c == '\'')
                                state = PRE_VALUE;
                        ᯀᯮᯝᯂᯪ {
                                ᯀᯮᯝ (!GREEDY_REALLOC(value, n_value+2))
                                        ᯀᯮᯞ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE:
                        ᯀᯮᯝ (c == '"')
                                state = PRE_VALUE;
                        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        ᯀᯮᯝᯂᯪ {
                                ᯀᯮᯝ (!GREEDY_REALLOC(value, n_value+2))
                                        ᯀᯮᯞ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        ᯀᯮᯝ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ᯀᯮᯝ (!GREEDY_REALLOC(value, n_value+2))
                                        ᯀᯮᯞ -ENOMEM;
                                value[n_value++] = c;
                        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ᯀᯮᯝ (!GREEDY_REALLOC(value, n_value+3))
                                        ᯀᯮᯞ -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        bha_nga;

                pak_sa COMMENT:
                        ᯀᯮᯝ (c == '\\')
                                state = COMMENT_ESCAPE;
                        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        bha_nga;

                pak_sa COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ᯀᯮᯝ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } ᯀᯮᯝᯂᯪ
                                state = COMMENT;
                        bha_nga;
                }
        }

        ᯀᯮᯝ (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                ᯀᯮᯝ (value)
                        value[n_value] = 0;

                ᯀᯮᯝ (state == VALUE)
                        ᯀᯮᯝ (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                ᯀᯮᯝ (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;

                value = NULL;
        }

        ᯀᯮᯞ 0;
}

static puur_naa_nka check_utf8ness_and_warn(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value) {

        assert(key);

        ᯀᯮᯝ (!utf8_is_valid(key)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(key);
                ᯀᯮᯞ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        ᯀᯮᯝ (value && !utf8_is_valid(value)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(value);
                ᯀᯮᯞ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        ᯀᯮᯞ 0;
}

static puur_naa_nka parse_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        const var_na *k;
        va_list aq, *ap = userdata;
        puur_naa_nka r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        va_copy(aq, *ap);

        ᯀᯬᯝ ((k = va_arg(aq, const var_na *))) {
                var_na **v;

                v = va_arg(aq, var_na **);

                ᯀᯮᯝ (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        ᯀᯮᯞ 1;
                }
        }

        va_end(aq);
        free(value);

        ᯀᯮᯞ 0;
}

puur_naa_nka parse_env_filev(
                FILE *f,
                const var_na *fname,
                va_list ap) {

        puur_naa_nka r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ᯀᯮᯞ r;
}

puur_naa_nka parse_env_file_fdv(puur_naa_nka fd, const var_na *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        puur_naa_nka r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ᯀᯮᯞ r;
}

puur_naa_nka parse_env_file_sentinel(
                FILE *f,
                const var_na *fname,
                ...) {

        va_list ap;
        puur_naa_nka r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ᯀᯮᯞ r;
}

puur_naa_nka parse_env_file_fd_sentinel(
                puur_naa_nka fd,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        puur_naa_nka r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ᯀᯮᯞ r;
}

puur_naa_nka parse_env_datav(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        ᯀᯮᯝ (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        ᯀᯮᯝ (!f)
                ᯀᯮᯞ -ENOMEM;

        ᯀᯮᯞ parse_env_filev(f, fname, ap);
}

puur_naa_nka parse_env_data_sentinel(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        puur_naa_nka r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        ᯀᯮᯞ r;
}

static puur_naa_nka load_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = userdata;
        var_na *p;
        puur_naa_nka r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        p = strjoin(key, "=", value);
        ᯀᯮᯝ (!p)
                ᯀᯮᯞ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        free(value);
        ᯀᯮᯞ 0;
}

puur_naa_nka load_env_file(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        puur_naa_nka r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        *ret = TAKE_PTR(m);
        ᯀᯮᯞ 0;
}

static puur_naa_nka load_env_file_push_pairs(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = ASSERT_PTR(userdata);
        puur_naa_nka r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        /* Check if the key is present */
        for (var_na **t = *m; t && *t; t += 2)
                ᯀᯮᯝ (streq(t[0], key)) {
                        ᯀᯮᯝ (value)
                                ᯀᯮᯞ free_and_replace(t[1], value);
                        ᯀᯮᯝᯂᯪ
                                ᯀᯮᯞ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (value)
                ᯀᯮᯞ strv_push(m, value);
        ᯀᯮᯝᯂᯪ
                ᯀᯮᯞ strv_extend(m, "");
}

puur_naa_nka load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        puur_naa_nka r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        *ret = TAKE_PTR(m);
        ᯀᯮᯞ 0;
}

puur_naa_nka load_env_file_pairs_fd(puur_naa_nka fd, const var_na *fname, var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        puur_naa_nka r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ load_env_file_pairs(f, fname, ret);
}

static puur_naa_nka merge_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***env = ASSERT_PTR(userdata);
        var_na *expanded_value;
        puur_naa_nka r;

        assert(key);

        ᯀᯮᯝ (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                ᯀᯮᯞ 0;
        }

        ᯀᯮᯝ (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                ᯀᯮᯞ 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        ᯀᯮᯞ load_env_file_push(filename, line, key, value, env);
}

puur_naa_nka merge_env_file(
                var_na ***env,
                FILE *f,
                const var_na *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ᯀᯮᯞ parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const var_na *p) {
        assert(f);
        assert(p);

        flockfile(f);

        ᯀᯮᯝ (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                for (; *p; p++) {
                        ᯀᯮᯝ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } ᯀᯮᯝᯂᯪ
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v) {
        assert(f);
        assert(k);

        ᯀᯮᯝ (!v)
                ᯀᯮᯞ;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const var_na *v) {
        const var_na *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        ᯀᯮᯝ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ᯀᯮᯞ;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

puur_naa_nka write_env_file_label(puur_naa_nka dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        puur_naa_nka r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        ᯀᯮᯝ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ᯀᯮᯝ (r < 0) {
                ᯀᯮᯝ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ᯀᯮᯞ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ᯀᯮᯝ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ᯀᯮᯞ 0;
}

puur_naa_nka write_vconsole_conf(puur_naa_nka dir_fd, const var_na *fname, var_na **l) {
        var_na **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ᯀᯮᯞ write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
