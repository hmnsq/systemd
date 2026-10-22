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

typedef ຈຳນວນເຕັມ (*push_env_func_t)(
                const var_na *filename,
                unsigned line,
                const var_na *key,
                var_na *value,
                void *userdata);

static ຈຳນວນເຕັມ parse_env_file_internal(
                FILE *f,
                const var_na *fname,
                push_env_func_t push,
                void *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ var_na *contents = NULL, *key = NULL, *value = NULL;
        unsigned line = 1;
        ຈຳນວນເຕັມ r;

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

        ຖ້າ (f)
                r = read_full_stream(f, &contents, NULL);
        ບໍ່ດັ່ງນັ້ນ
                r = read_full_file(fname, &contents, NULL);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສຳລັບ (var_na *p = contents; *p; p++) {
                var_na c = *p;

                switch (state) {

                pak_sa PRE_KEY:
                        ຖ້າ (strchr(COMMENTS, c))
                                state = COMMENT;
                        ບໍ່ດັ່ງນັ້ນ ຖ້າ (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                ຖ້າ (!GREEDY_REALLOC(key, n_key+2))
                                        ສົ່ງຄືນ -ENOMEM;

                                key[n_key++] = c;
                        }
                        bha_nga;

                pak_sa KEY:
                        ຖ້າ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } ບໍ່ດັ່ງນັ້ນ {
                                ຖ້າ (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                ບໍ່ດັ່ງນັ້ນ ຖ້າ (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                ຖ້າ (!GREEDY_REALLOC(key, n_key+2))
                                        ສົ່ງຄືນ -ENOMEM;

                                key[n_key++] = c;
                        }

                        bha_nga;

                pak_sa PRE_VALUE:
                        ຖ້າ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                ຖ້າ (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                ຖ້າ (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ຖ້າ (r < 0)
                                        ສົ່ງຄືນ r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '\\')
                                state = VALUE_ESCAPE;
                        ບໍ່ດັ່ງນັ້ນ ຖ້າ (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                ຖ້າ (!GREEDY_REALLOC(value, n_value+2))
                                        ສົ່ງຄືນ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE:
                        ຖ້າ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                ຖ້າ (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                ຖ້າ (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ຖ້າ (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                ຖ້າ (r < 0)
                                        ສົ່ງຄືນ r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } ບໍ່ດັ່ງນັ້ນ {
                                ຖ້າ (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                ບໍ່ດັ່ງນັ້ນ ຖ້າ (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                ຖ້າ (!GREEDY_REALLOC(value, n_value+2))
                                        ສົ່ງຄືນ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE_ESCAPE:
                        state = VALUE;

                        ຖ້າ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ຖ້າ (!GREEDY_REALLOC(value, n_value+2))
                                        ສົ່ງຄືນ -ENOMEM;

                                value[n_value++] = c;
                        }
                        bha_nga;

                pak_sa SINGLE_QUOTE_VALUE:
                        ຖ້າ (c == '\'')
                                state = PRE_VALUE;
                        ບໍ່ດັ່ງນັ້ນ {
                                ຖ້າ (!GREEDY_REALLOC(value, n_value+2))
                                        ສົ່ງຄືນ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE:
                        ຖ້າ (c == '"')
                                state = PRE_VALUE;
                        ບໍ່ດັ່ງນັ້ນ ຖ້າ (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        ບໍ່ດັ່ງນັ້ນ {
                                ຖ້າ (!GREEDY_REALLOC(value, n_value+2))
                                        ສົ່ງຄືນ -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        ຖ້າ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ຖ້າ (!GREEDY_REALLOC(value, n_value+2))
                                        ສົ່ງຄືນ -ENOMEM;
                                value[n_value++] = c;
                        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ຖ້າ (!GREEDY_REALLOC(value, n_value+3))
                                        ສົ່ງຄືນ -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        bha_nga;

                pak_sa COMMENT:
                        ຖ້າ (c == '\\')
                                state = COMMENT_ESCAPE;
                        ບໍ່ດັ່ງນັ້ນ ຖ້າ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        bha_nga;

                pak_sa COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ຖ້າ (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } ບໍ່ດັ່ງນັ້ນ
                                state = COMMENT;
                        bha_nga;
                }
        }

        ຖ້າ (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                ຖ້າ (value)
                        value[n_value] = 0;

                ຖ້າ (state == VALUE)
                        ຖ້າ (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                ຖ້າ (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                value = NULL;
        }

        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ check_utf8ness_and_warn(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value) {

        assert(key);

        ຖ້າ (!utf8_is_valid(key)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(key);
                ສົ່ງຄືນ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        ຖ້າ (value && !utf8_is_valid(value)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(value);
                ສົ່ງຄືນ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ parse_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        const var_na *k;
        va_list aq, *ap = userdata;
        ຈຳນວນເຕັມ r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        va_copy(aq, *ap);

        ໃນຂະນະທີ່ ((k = va_arg(aq, const var_na *))) {
                var_na **v;

                v = va_arg(aq, var_na **);

                ຖ້າ (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        ສົ່ງຄືນ 1;
                }
        }

        va_end(aq);
        free(value);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ parse_env_filev(
                FILE *f,
                const var_na *fname,
                va_list ap) {

        ຈຳນວນເຕັມ r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ parse_env_file_fdv(ຈຳນວນເຕັມ fd, const var_na *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        ຈຳນວນເຕັມ r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ parse_env_file_sentinel(
                FILE *f,
                const var_na *fname,
                ...) {

        va_list ap;
        ຈຳນວນເຕັມ r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ parse_env_file_fd_sentinel(
                ຈຳນວນເຕັມ fd,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        ຈຳນວນເຕັມ r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ parse_env_datav(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        ຖ້າ (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        ຖ້າ (!f)
                ສົ່ງຄືນ -ENOMEM;

        ສົ່ງຄືນ parse_env_filev(f, fname, ap);
}

ຈຳນວນເຕັມ parse_env_data_sentinel(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        ຈຳນວນເຕັມ r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        ສົ່ງຄືນ r;
}

static ຈຳນວນເຕັມ load_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = userdata;
        var_na *p;
        ຈຳນວນເຕັມ r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        p = strjoin(key, "=", value);
        ຖ້າ (!p)
                ສົ່ງຄືນ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        free(value);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ load_env_file(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        ຈຳນວນເຕັມ r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        *ret = TAKE_PTR(m);
        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ load_env_file_push_pairs(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = ASSERT_PTR(userdata);
        ຈຳນວນເຕັມ r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        /* Check if the key is present */
        ສຳລັບ (var_na **t = *m; t && *t; t += 2)
                ຖ້າ (streq(t[0], key)) {
                        ຖ້າ (value)
                                ສົ່ງຄືນ free_and_replace(t[1], value);
                        ບໍ່ດັ່ງນັ້ນ
                                ສົ່ງຄືນ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (value)
                ສົ່ງຄືນ strv_push(m, value);
        ບໍ່ດັ່ງນັ້ນ
                ສົ່ງຄືນ strv_extend(m, "");
}

ຈຳນວນເຕັມ load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        ຈຳນວນເຕັມ r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        *ret = TAKE_PTR(m);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ load_env_file_pairs_fd(ຈຳນວນເຕັມ fd, const var_na *fname, var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        ຈຳນວນເຕັມ r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ load_env_file_pairs(f, fname, ret);
}

static ຈຳນວນເຕັມ merge_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***env = ASSERT_PTR(userdata);
        var_na *expanded_value;
        ຈຳນວນເຕັມ r;

        assert(key);

        ຖ້າ (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                ສົ່ງຄືນ 0;
        }

        ຖ້າ (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                ສົ່ງຄືນ 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        ສົ່ງຄືນ load_env_file_push(filename, line, key, value, env);
}

ຈຳນວນເຕັມ merge_env_file(
                var_na ***env,
                FILE *f,
                const var_na *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ສົ່ງຄືນ parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const var_na *p) {
        assert(f);
        assert(p);

        flockfile(f);

        ຖ້າ (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                ສຳລັບ (; *p; p++) {
                        ຖ້າ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } ບໍ່ດັ່ງນັ້ນ
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v) {
        assert(f);
        assert(k);

        ຖ້າ (!v)
                ສົ່ງຄືນ;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const var_na *v) {
        const var_na *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        ຖ້າ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ສົ່ງຄືນ;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

ຈຳນວນເຕັມ write_env_file_label(ຈຳນວນເຕັມ dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        ຈຳນວນເຕັມ r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        ຖ້າ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ຖ້າ (r < 0) {
                ຖ້າ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ສົ່ງຄືນ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ຖ້າ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ write_vconsole_conf(ຈຳນວນເຕັມ dir_fd, const var_na *fname, var_na **l) {
        var_na **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ສົ່ງຄືນ write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
