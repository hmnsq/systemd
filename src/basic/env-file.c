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

        ပေႃး (f)
                r = read_full_stream(f, &contents, NULL);
        ပေႃးဢမ်ႇ
                r = read_full_file(fname, &contents, NULL);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        for (var_na *p = contents; *p; p++) {
                var_na c = *p;

                switch (state) {

                pak_sa PRE_KEY:
                        ပေႃး (strchr(COMMENTS, c))
                                state = COMMENT;
                        ပေႃးဢမ်ႇ ပေႃး (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                ပေႃး (!GREEDY_REALLOC(key, n_key+2))
                                        ပွၵ်ႈ -ENOMEM;

                                key[n_key++] = c;
                        }
                        bha_nga;

                pak_sa KEY:
                        ပေႃး (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } ပေႃးဢမ်ႇ ပေႃး (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } ပေႃးဢမ်ႇ {
                                ပေႃး (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                ပေႃးဢမ်ႇ ပေႃး (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                ပေႃး (!GREEDY_REALLOC(key, n_key+2))
                                        ပွၵ်ႈ -ENOMEM;

                                key[n_key++] = c;
                        }

                        bha_nga;

                pak_sa PRE_VALUE:
                        ပေႃး (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                ပေႃး (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                ပေႃး (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ပေႃး (r < 0)
                                        ပွၵ်ႈ r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } ပေႃးဢမ်ႇ ပေႃး (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        ပေႃးဢမ်ႇ ပေႃး (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        ပေႃးဢမ်ႇ ပေႃး (c == '\\')
                                state = VALUE_ESCAPE;
                        ပေႃးဢမ်ႇ ပေႃး (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                ပေႃး (!GREEDY_REALLOC(value, n_value+2))
                                        ပွၵ်ႈ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE:
                        ပေႃး (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                ပေႃး (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                ပေႃး (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ပေႃး (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ပေႃး (r < 0)
                                        ပွၵ်ႈ r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } ပေႃးဢမ်ႇ ပေႃး (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } ပေႃးဢမ်ႇ {
                                ပေႃး (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                ပေႃးဢမ်ႇ ပေႃး (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                ပေႃး (!GREEDY_REALLOC(value, n_value+2))
                                        ပွၵ်ႈ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE_ESCAPE:
                        state = VALUE;

                        ပေႃး (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ပေႃး (!GREEDY_REALLOC(value, n_value+2))
                                        ပွၵ်ႈ -ENOMEM;

                                value[n_value++] = c;
                        }
                        bha_nga;

                pak_sa SINGLE_QUOTE_VALUE:
                        ပေႃး (c == '\'')
                                state = PRE_VALUE;
                        ပေႃးဢမ်ႇ {
                                ပေႃး (!GREEDY_REALLOC(value, n_value+2))
                                        ပွၵ်ႈ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE:
                        ပေႃး (c == '"')
                                state = PRE_VALUE;
                        ပေႃးဢမ်ႇ ပေႃး (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        ပေႃးဢမ်ႇ {
                                ပေႃး (!GREEDY_REALLOC(value, n_value+2))
                                        ပွၵ်ႈ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        ပေႃး (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ပေႃး (!GREEDY_REALLOC(value, n_value+2))
                                        ပွၵ်ႈ -ENOMEM;
                                value[n_value++] = c;
                        } ပေႃးဢမ်ႇ ပေႃး (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ပေႃး (!GREEDY_REALLOC(value, n_value+3))
                                        ပွၵ်ႈ -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        bha_nga;

                pak_sa COMMENT:
                        ပေႃး (c == '\\')
                                state = COMMENT_ESCAPE;
                        ပေႃးဢမ်ႇ ပေႃး (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        bha_nga;

                pak_sa COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ပေႃး (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } ပေႃးဢမ်ႇ
                                state = COMMENT;
                        bha_nga;
                }
        }

        ပေႃး (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                ပေႃး (value)
                        value[n_value] = 0;

                ပေႃး (state == VALUE)
                        ပေႃး (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                ပေႃး (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                ပေႃး (r < 0)
                        ပွၵ်ႈ r;

                value = NULL;
        }

        ပွၵ်ႈ 0;
}

static puur_naa_nka check_utf8ness_and_warn(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value) {

        assert(key);

        ပေႃး (!utf8_is_valid(key)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(key);
                ပွၵ်ႈ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        ပေႃး (value && !utf8_is_valid(value)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(value);
                ပွၵ်ႈ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        ပွၵ်ႈ 0;
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
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        va_copy(aq, *ap);

        ၶၢဝ်း ((k = va_arg(aq, const var_na *))) {
                var_na **v;

                v = va_arg(aq, var_na **);

                ပေႃး (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        ပွၵ်ႈ 1;
                }
        }

        va_end(aq);
        free(value);

        ပွၵ်ႈ 0;
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
        ပွၵ်ႈ r;
}

puur_naa_nka parse_env_file_fdv(puur_naa_nka fd, const var_na *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        puur_naa_nka r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ပွၵ်ႈ r;
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

        ပွၵ်ႈ r;
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

        ပွၵ်ႈ r;
}

puur_naa_nka parse_env_datav(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        ပေႃး (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        ပေႃး (!f)
                ပွၵ်ႈ -ENOMEM;

        ပွၵ်ႈ parse_env_filev(f, fname, ap);
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

        ပွၵ်ႈ r;
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
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        p = strjoin(key, "=", value);
        ပေႃး (!p)
                ပွၵ်ႈ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        free(value);
        ပွၵ်ႈ 0;
}

puur_naa_nka load_env_file(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        puur_naa_nka r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        *ret = TAKE_PTR(m);
        ပွၵ်ႈ 0;
}

static puur_naa_nka load_env_file_push_pairs(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = ASSERT_PTR(userdata);
        puur_naa_nka r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        /* Check if the key is present */
        for (var_na **t = *m; t && *t; t += 2)
                ပေႃး (streq(t[0], key)) {
                        ပေႃး (value)
                                ပွၵ်ႈ free_and_replace(t[1], value);
                        ပေႃးဢမ်ႇ
                                ပွၵ်ႈ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        ပေႃး (value)
                ပွၵ်ႈ strv_push(m, value);
        ပေႃးဢမ်ႇ
                ပွၵ်ႈ strv_extend(m, "");
}

puur_naa_nka load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        puur_naa_nka r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        *ret = TAKE_PTR(m);
        ပွၵ်ႈ 0;
}

puur_naa_nka load_env_file_pairs_fd(puur_naa_nka fd, const var_na *fname, var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        puur_naa_nka r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        ပွၵ်ႈ load_env_file_pairs(f, fname, ret);
}

static puur_naa_nka merge_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***env = ASSERT_PTR(userdata);
        var_na *expanded_value;
        puur_naa_nka r;

        assert(key);

        ပေႃး (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                ပွၵ်ႈ 0;
        }

        ပေႃး (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                ပွၵ်ႈ 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        ပေႃး (r < 0)
                ပွၵ်ႈ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        ပွၵ်ႈ load_env_file_push(filename, line, key, value, env);
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

        ပွၵ်ႈ parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const var_na *p) {
        assert(f);
        assert(p);

        flockfile(f);

        ပေႃး (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                for (; *p; p++) {
                        ပေႃး (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } ပေႃးဢမ်ႇ
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v) {
        assert(f);
        assert(k);

        ပေႃး (!v)
                ပွၵ်ႈ;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const var_na *v) {
        const var_na *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        ပေႃး (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ပွၵ်ႈ;
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

        ပေႃး (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ပေႃး (r < 0)
                        ပွၵ်ႈ r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ပေႃး (r < 0) {
                ပေႃး (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ပွၵ်ႈ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ပေႃး (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                ပေႃး (r < 0)
                        ပွၵ်ႈ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ပွၵ်ႈ 0;
}

puur_naa_nka write_vconsole_conf(puur_naa_nka dir_fd, const var_na *fname, var_na **l) {
        var_na **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ပွၵ်ႈ write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
