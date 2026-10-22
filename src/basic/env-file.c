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

typedef ကိန်းပြည့် (*push_env_func_t)(
                const var_na *filename,
                unsigned line,
                const var_na *key,
                var_na *value,
                void *userdata);

static ကိန်းပြည့် parse_env_file_internal(
                FILE *f,
                const var_na *fname,
                push_env_func_t push,
                void *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ var_na *contents = NULL, *key = NULL, *value = NULL;
        unsigned line = 1;
        ကိန်းပြည့် r;

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

        အကယ်၍ (f)
                r = read_full_stream(f, &contents, NULL);
        မဟုတ်ပါက
                r = read_full_file(fname, &contents, NULL);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အတွက် (var_na *p = contents; *p; p++) {
                var_na c = *p;

                switch (state) {

                pak_sa PRE_KEY:
                        အကယ်၍ (strchr(COMMENTS, c))
                                state = COMMENT;
                        မဟုတ်ပါက အကယ်၍ (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                အကယ်၍ (!GREEDY_REALLOC(key, n_key+2))
                                        ပြန်ပေး -ENOMEM;

                                key[n_key++] = c;
                        }
                        bha_nga;

                pak_sa KEY:
                        အကယ်၍ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } မဟုတ်ပါက အကယ်၍ (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } မဟုတ်ပါက {
                                အကယ်၍ (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                မဟုတ်ပါက အကယ်၍ (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                အကယ်၍ (!GREEDY_REALLOC(key, n_key+2))
                                        ပြန်ပေး -ENOMEM;

                                key[n_key++] = c;
                        }

                        bha_nga;

                pak_sa PRE_VALUE:
                        အကယ်၍ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                အကယ်၍ (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                အကယ်၍ (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } မဟုတ်ပါက အကယ်၍ (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        မဟုတ်ပါက အကယ်၍ (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        မဟုတ်ပါက အကယ်၍ (c == '\\')
                                state = VALUE_ESCAPE;
                        မဟုတ်ပါက အကယ်၍ (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                အကယ်၍ (!GREEDY_REALLOC(value, n_value+2))
                                        ပြန်ပေး -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE:
                        အကယ်၍ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                အကယ်၍ (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                အကယ်၍ (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                အကယ်၍ (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } မဟုတ်ပါက အကယ်၍ (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } မဟုတ်ပါက {
                                အကယ်၍ (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                မဟုတ်ပါက အကယ်၍ (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                အကယ်၍ (!GREEDY_REALLOC(value, n_value+2))
                                        ပြန်ပေး -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE_ESCAPE:
                        state = VALUE;

                        အကယ်၍ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                အကယ်၍ (!GREEDY_REALLOC(value, n_value+2))
                                        ပြန်ပေး -ENOMEM;

                                value[n_value++] = c;
                        }
                        bha_nga;

                pak_sa SINGLE_QUOTE_VALUE:
                        အကယ်၍ (c == '\'')
                                state = PRE_VALUE;
                        မဟုတ်ပါက {
                                အကယ်၍ (!GREEDY_REALLOC(value, n_value+2))
                                        ပြန်ပေး -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE:
                        အကယ်၍ (c == '"')
                                state = PRE_VALUE;
                        မဟုတ်ပါက အကယ်၍ (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        မဟုတ်ပါက {
                                အကယ်၍ (!GREEDY_REALLOC(value, n_value+2))
                                        ပြန်ပေး -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        အကယ်၍ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                အကယ်၍ (!GREEDY_REALLOC(value, n_value+2))
                                        ပြန်ပေး -ENOMEM;
                                value[n_value++] = c;
                        } မဟုတ်ပါက အကယ်၍ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                အကယ်၍ (!GREEDY_REALLOC(value, n_value+3))
                                        ပြန်ပေး -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        bha_nga;

                pak_sa COMMENT:
                        အကယ်၍ (c == '\\')
                                state = COMMENT_ESCAPE;
                        မဟုတ်ပါက အကယ်၍ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        bha_nga;

                pak_sa COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        အကယ်၍ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } မဟုတ်ပါက
                                state = COMMENT;
                        bha_nga;
                }
        }

        အကယ်၍ (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                အကယ်၍ (value)
                        value[n_value] = 0;

                အကယ်၍ (state == VALUE)
                        အကယ်၍ (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                အကယ်၍ (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                value = NULL;
        }

        ပြန်ပေး 0;
}

static ကိန်းပြည့် check_utf8ness_and_warn(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value) {

        assert(key);

        အကယ်၍ (!utf8_is_valid(key)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(key);
                ပြန်ပေး log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        အကယ်၍ (value && !utf8_is_valid(value)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(value);
                ပြန်ပေး log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        ပြန်ပေး 0;
}

static ကိန်းပြည့် parse_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        const var_na *k;
        va_list aq, *ap = userdata;
        ကိန်းပြည့် r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        va_copy(aq, *ap);

        နေစဉ် ((k = va_arg(aq, const var_na *))) {
                var_na **v;

                v = va_arg(aq, var_na **);

                အကယ်၍ (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        ပြန်ပေး 1;
                }
        }

        va_end(aq);
        free(value);

        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_env_filev(
                FILE *f,
                const var_na *fname,
                va_list ap) {

        ကိန်းပြည့် r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ပြန်ပေး r;
}

ကိန်းပြည့် parse_env_file_fdv(ကိန်းပြည့် fd, const var_na *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        ကိန်းပြည့် r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ပြန်ပေး r;
}

ကိန်းပြည့် parse_env_file_sentinel(
                FILE *f,
                const var_na *fname,
                ...) {

        va_list ap;
        ကိန်းပြည့် r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ပြန်ပေး r;
}

ကိန်းပြည့် parse_env_file_fd_sentinel(
                ကိန်းပြည့် fd,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        ကိန်းပြည့် r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ပြန်ပေး r;
}

ကိန်းပြည့် parse_env_datav(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        အကယ်၍ (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        အကယ်၍ (!f)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး parse_env_filev(f, fname, ap);
}

ကိန်းပြည့် parse_env_data_sentinel(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        ကိန်းပြည့် r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        ပြန်ပေး r;
}

static ကိန်းပြည့် load_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = userdata;
        var_na *p;
        ကိန်းပြည့် r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        p = strjoin(key, "=", value);
        အကယ်၍ (!p)
                ပြန်ပေး -ENOMEM;

        r = strv_env_replace_consume(m, p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        free(value);
        ပြန်ပေး 0;
}

ကိန်းပြည့် load_env_file(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        ကိန်းပြည့် r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = TAKE_PTR(m);
        ပြန်ပေး 0;
}

static ကိန်းပြည့် load_env_file_push_pairs(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = ASSERT_PTR(userdata);
        ကိန်းပြည့် r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Check if the key is present */
        အတွက် (var_na **t = *m; t && *t; t += 2)
                အကယ်၍ (streq(t[0], key)) {
                        အကယ်၍ (value)
                                ပြန်ပေး free_and_replace(t[1], value);
                        မဟုတ်ပါက
                                ပြန်ပေး free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (value)
                ပြန်ပေး strv_push(m, value);
        မဟုတ်ပါက
                ပြန်ပေး strv_extend(m, "");
}

ကိန်းပြည့် load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        ကိန်းပြည့် r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = TAKE_PTR(m);
        ပြန်ပေး 0;
}

ကိန်းပြည့် load_env_file_pairs_fd(ကိန်းပြည့် fd, const var_na *fname, var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        ကိန်းပြည့် r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး load_env_file_pairs(f, fname, ret);
}

static ကိန်းပြည့် merge_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***env = ASSERT_PTR(userdata);
        var_na *expanded_value;
        ကိန်းပြည့် r;

        assert(key);

        အကယ်၍ (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                ပြန်ပေး 0;
        }

        အကယ်၍ (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                ပြန်ပေး 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        အကယ်၍ (r < 0)
                ပြန်ပေး log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        ပြန်ပေး load_env_file_push(filename, line, key, value, env);
}

ကိန်းပြည့် merge_env_file(
                var_na ***env,
                FILE *f,
                const var_na *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ပြန်ပေး parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const var_na *p) {
        assert(f);
        assert(p);

        flockfile(f);

        အကယ်၍ (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                အတွက် (; *p; p++) {
                        အကယ်၍ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } မဟုတ်ပါက
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v) {
        assert(f);
        assert(k);

        အကယ်၍ (!v)
                ပြန်ပေး;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const var_na *v) {
        const var_na *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        အကယ်၍ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ပြန်ပေး;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

ကိန်းပြည့် write_env_file_label(ကိန်းပြည့် dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        ကိန်းပြည့် r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        အကယ်၍ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        အကယ်၍ (r < 0) {
                အကယ်၍ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ပြန်ပေး r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        အကယ်၍ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        r = fchmod_umask(fileno(f), 0644);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ပြန်ပေး 0;
}

ကိန်းပြည့် write_vconsole_conf(ကိန်းပြည့် dir_fd, const var_na *fname, var_na **l) {
        var_na **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ပြန်ပေး write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
