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

typedef จำนวนเต็ม (*push_env_func_t)(
                const var_na *filename,
                unsigned line,
                const var_na *key,
                var_na *value,
                void *userdata);

static จำนวนเต็ม parse_env_file_internal(
                FILE *f,
                const var_na *fname,
                push_env_func_t push,
                void *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ var_na *contents = NULL, *key = NULL, *value = NULL;
        unsigned line = 1;
        จำนวนเต็ม r;

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

        ถ้า (f)
                r = read_full_stream(f, &contents, NULL);
        มิฉะนั้น
                r = read_full_file(fname, &contents, NULL);
        ถ้า (r < 0)
                คืน r;

        สำหรับ (var_na *p = contents; *p; p++) {
                var_na c = *p;

                switch (state) {

                pak_sa PRE_KEY:
                        ถ้า (strchr(COMMENTS, c))
                                state = COMMENT;
                        มิฉะนั้น ถ้า (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                ถ้า (!GREEDY_REALLOC(key, n_key+2))
                                        คืน -ENOMEM;

                                key[n_key++] = c;
                        }
                        bha_nga;

                pak_sa KEY:
                        ถ้า (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } มิฉะนั้น ถ้า (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } มิฉะนั้น {
                                ถ้า (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                มิฉะนั้น ถ้า (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                ถ้า (!GREEDY_REALLOC(key, n_key+2))
                                        คืน -ENOMEM;

                                key[n_key++] = c;
                        }

                        bha_nga;

                pak_sa PRE_VALUE:
                        ถ้า (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                ถ้า (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                ถ้า (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ถ้า (r < 0)
                                        คืน r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } มิฉะนั้น ถ้า (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        มิฉะนั้น ถ้า (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        มิฉะนั้น ถ้า (c == '\\')
                                state = VALUE_ESCAPE;
                        มิฉะนั้น ถ้า (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                ถ้า (!GREEDY_REALLOC(value, n_value+2))
                                        คืน -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE:
                        ถ้า (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                ถ้า (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                ถ้า (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ถ้า (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ถ้า (r < 0)
                                        คืน r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } มิฉะนั้น ถ้า (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } มิฉะนั้น {
                                ถ้า (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                มิฉะนั้น ถ้า (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                ถ้า (!GREEDY_REALLOC(value, n_value+2))
                                        คืน -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE_ESCAPE:
                        state = VALUE;

                        ถ้า (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ถ้า (!GREEDY_REALLOC(value, n_value+2))
                                        คืน -ENOMEM;

                                value[n_value++] = c;
                        }
                        bha_nga;

                pak_sa SINGLE_QUOTE_VALUE:
                        ถ้า (c == '\'')
                                state = PRE_VALUE;
                        มิฉะนั้น {
                                ถ้า (!GREEDY_REALLOC(value, n_value+2))
                                        คืน -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE:
                        ถ้า (c == '"')
                                state = PRE_VALUE;
                        มิฉะนั้น ถ้า (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        มิฉะนั้น {
                                ถ้า (!GREEDY_REALLOC(value, n_value+2))
                                        คืน -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        ถ้า (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ถ้า (!GREEDY_REALLOC(value, n_value+2))
                                        คืน -ENOMEM;
                                value[n_value++] = c;
                        } มิฉะนั้น ถ้า (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ถ้า (!GREEDY_REALLOC(value, n_value+3))
                                        คืน -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        bha_nga;

                pak_sa COMMENT:
                        ถ้า (c == '\\')
                                state = COMMENT_ESCAPE;
                        มิฉะนั้น ถ้า (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        bha_nga;

                pak_sa COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ถ้า (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } มิฉะนั้น
                                state = COMMENT;
                        bha_nga;
                }
        }

        ถ้า (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                ถ้า (value)
                        value[n_value] = 0;

                ถ้า (state == VALUE)
                        ถ้า (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                ถ้า (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                ถ้า (r < 0)
                        คืน r;

                value = NULL;
        }

        คืน 0;
}

static จำนวนเต็ม check_utf8ness_and_warn(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value) {

        assert(key);

        ถ้า (!utf8_is_valid(key)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(key);
                คืน log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        ถ้า (value && !utf8_is_valid(value)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(value);
                คืน log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        คืน 0;
}

static จำนวนเต็ม parse_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        const var_na *k;
        va_list aq, *ap = userdata;
        จำนวนเต็ม r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ถ้า (r < 0)
                คืน r;

        va_copy(aq, *ap);

        ขณะ ((k = va_arg(aq, const var_na *))) {
                var_na **v;

                v = va_arg(aq, var_na **);

                ถ้า (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        คืน 1;
                }
        }

        va_end(aq);
        free(value);

        คืน 0;
}

จำนวนเต็ม parse_env_filev(
                FILE *f,
                const var_na *fname,
                va_list ap) {

        จำนวนเต็ม r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        คืน r;
}

จำนวนเต็ม parse_env_file_fdv(จำนวนเต็ม fd, const var_na *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ถ้า (r < 0)
                คืน r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        คืน r;
}

จำนวนเต็ม parse_env_file_sentinel(
                FILE *f,
                const var_na *fname,
                ...) {

        va_list ap;
        จำนวนเต็ม r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        คืน r;
}

จำนวนเต็ม parse_env_file_fd_sentinel(
                จำนวนเต็ม fd,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        จำนวนเต็ม r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        คืน r;
}

จำนวนเต็ม parse_env_datav(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        ถ้า (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        ถ้า (!f)
                คืน -ENOMEM;

        คืน parse_env_filev(f, fname, ap);
}

จำนวนเต็ม parse_env_data_sentinel(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        จำนวนเต็ม r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        คืน r;
}

static จำนวนเต็ม load_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = userdata;
        var_na *p;
        จำนวนเต็ม r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ถ้า (r < 0)
                คืน r;

        p = strjoin(key, "=", value);
        ถ้า (!p)
                คืน -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ถ้า (r < 0)
                คืน r;

        free(value);
        คืน 0;
}

จำนวนเต็ม load_env_file(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        จำนวนเต็ม r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        ถ้า (r < 0)
                คืน r;

        *ret = TAKE_PTR(m);
        คืน 0;
}

static จำนวนเต็ม load_env_file_push_pairs(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = ASSERT_PTR(userdata);
        จำนวนเต็ม r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ถ้า (r < 0)
                คืน r;

        /* Check if the key is present */
        สำหรับ (var_na **t = *m; t && *t; t += 2)
                ถ้า (streq(t[0], key)) {
                        ถ้า (value)
                                คืน free_and_replace(t[1], value);
                        มิฉะนั้น
                                คืน free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        ถ้า (r < 0)
                คืน r;

        ถ้า (value)
                คืน strv_push(m, value);
        มิฉะนั้น
                คืน strv_extend(m, "");
}

จำนวนเต็ม load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        จำนวนเต็ม r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        ถ้า (r < 0)
                คืน r;

        *ret = TAKE_PTR(m);
        คืน 0;
}

จำนวนเต็ม load_env_file_pairs_fd(จำนวนเต็ม fd, const var_na *fname, var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ถ้า (r < 0)
                คืน r;

        คืน load_env_file_pairs(f, fname, ret);
}

static จำนวนเต็ม merge_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***env = ASSERT_PTR(userdata);
        var_na *expanded_value;
        จำนวนเต็ม r;

        assert(key);

        ถ้า (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                คืน 0;
        }

        ถ้า (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                คืน 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        ถ้า (r < 0)
                คืน log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        คืน load_env_file_push(filename, line, key, value, env);
}

จำนวนเต็ม merge_env_file(
                var_na ***env,
                FILE *f,
                const var_na *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        คืน parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const var_na *p) {
        assert(f);
        assert(p);

        flockfile(f);

        ถ้า (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                สำหรับ (; *p; p++) {
                        ถ้า (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } มิฉะนั้น
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v) {
        assert(f);
        assert(k);

        ถ้า (!v)
                คืน;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const var_na *v) {
        const var_na *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        ถ้า (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                คืน;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

จำนวนเต็ม write_env_file_label(จำนวนเต็ม dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        จำนวนเต็ม r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        ถ้า (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ถ้า (r < 0)
                        คืน r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ถ้า (r < 0) {
                ถ้า (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                คืน r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ถ้า (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                ถ้า (r < 0)
                        คืน r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ถ้า (r < 0)
                คืน r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ถ้า (r < 0)
                คืน r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        คืน 0;
}

จำนวนเต็ม write_vconsole_conf(จำนวนเต็ม dir_fd, const var_na *fname, var_na **l) {
        var_na **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        คืน write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
