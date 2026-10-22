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

typedef පූර්ණ_සංඛ්‍යා (*push_env_func_t)(
                const var_na *filename,
                unsigned line,
                const var_na *key,
                var_na *value,
                void *userdata);

static පූර්ණ_සංඛ්‍යා parse_env_file_internal(
                FILE *f,
                const var_na *fname,
                push_env_func_t push,
                void *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ var_na *contents = NULL, *key = NULL, *value = NULL;
        unsigned line = 1;
        පූර්ණ_සංඛ්‍යා r;

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

        නම් (f)
                r = read_full_stream(f, &contents, NULL);
        නැතහොත්
                r = read_full_file(fname, &contents, NULL);
        නම් (r < 0)
                ආපසු r;

        සඳහා (var_na *p = contents; *p; p++) {
                var_na c = *p;

                switch (state) {

                pak_sa PRE_KEY:
                        නම් (strchr(COMMENTS, c))
                                state = COMMENT;
                        නැතහොත් නම් (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                නම් (!GREEDY_REALLOC(key, n_key+2))
                                        ආපසු -ENOMEM;

                                key[n_key++] = c;
                        }
                        bha_nga;

                pak_sa KEY:
                        නම් (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } නැතහොත් නම් (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } නැතහොත් {
                                නම් (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                නැතහොත් නම් (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                නම් (!GREEDY_REALLOC(key, n_key+2))
                                        ආපසු -ENOMEM;

                                key[n_key++] = c;
                        }

                        bha_nga;

                pak_sa PRE_VALUE:
                        නම් (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                නම් (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                නම් (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                නම් (r < 0)
                                        ආපසු r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } නැතහොත් නම් (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        නැතහොත් නම් (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        නැතහොත් නම් (c == '\\')
                                state = VALUE_ESCAPE;
                        නැතහොත් නම් (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                නම් (!GREEDY_REALLOC(value, n_value+2))
                                        ආපසු -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE:
                        නම් (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                නම් (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                නම් (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                නම් (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                නම් (r < 0)
                                        ආපසු r;

                                n_key = 0;
                                value = NULL;
                                n_value = 0;

                        } නැතහොත් නම් (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } නැතහොත් {
                                නම් (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                නැතහොත් නම් (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                නම් (!GREEDY_REALLOC(value, n_value+2))
                                        ආපසු -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa VALUE_ESCAPE:
                        state = VALUE;

                        නම් (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                නම් (!GREEDY_REALLOC(value, n_value+2))
                                        ආපසු -ENOMEM;

                                value[n_value++] = c;
                        }
                        bha_nga;

                pak_sa SINGLE_QUOTE_VALUE:
                        නම් (c == '\'')
                                state = PRE_VALUE;
                        නැතහොත් {
                                නම් (!GREEDY_REALLOC(value, n_value+2))
                                        ආපසු -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE:
                        නම් (c == '"')
                                state = PRE_VALUE;
                        නැතහොත් නම් (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        නැතහොත් {
                                නම් (!GREEDY_REALLOC(value, n_value+2))
                                        ආපසු -ENOMEM;

                                value[n_value++] = c;
                        }

                        bha_nga;

                pak_sa DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        නම් (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                නම් (!GREEDY_REALLOC(value, n_value+2))
                                        ආපසු -ENOMEM;
                                value[n_value++] = c;
                        } නැතහොත් නම් (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                නම් (!GREEDY_REALLOC(value, n_value+3))
                                        ආපසු -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        bha_nga;

                pak_sa COMMENT:
                        නම් (c == '\\')
                                state = COMMENT_ESCAPE;
                        නැතහොත් නම් (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        bha_nga;

                pak_sa COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        නම් (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } නැතහොත්
                                state = COMMENT;
                        bha_nga;
                }
        }

        නම් (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                නම් (value)
                        value[n_value] = 0;

                නම් (state == VALUE)
                        නම් (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                නම් (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                නම් (r < 0)
                        ආපසු r;

                value = NULL;
        }

        ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා check_utf8ness_and_warn(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value) {

        assert(key);

        නම් (!utf8_is_valid(key)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(key);
                ආපසු log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        නම් (value && !utf8_is_valid(value)) {
                _cleanup_free_ var_na *p = NULL;

                p = utf8_escape_invalid(value);
                ආපසු log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා parse_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        const var_na *k;
        va_list aq, *ap = userdata;
        පූර්ණ_සංඛ්‍යා r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        නම් (r < 0)
                ආපසු r;

        va_copy(aq, *ap);

        අතරතුර ((k = va_arg(aq, const var_na *))) {
                var_na **v;

                v = va_arg(aq, var_na **);

                නම් (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        ආපසු 1;
                }
        }

        va_end(aq);
        free(value);

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_env_filev(
                FILE *f,
                const var_na *fname,
                va_list ap) {

        පූර්ණ_සංඛ්‍යා r;
        va_list aq;

        assert(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ආපසු r;
}

පූර්ණ_සංඛ්‍යා parse_env_file_fdv(පූර්ණ_සංඛ්‍යා fd, const var_na *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = NULL;
        va_list aq;
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        නම් (r < 0)
                ආපසු r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        ආපසු r;
}

පූර්ණ_සංඛ්‍යා parse_env_file_sentinel(
                FILE *f,
                const var_na *fname,
                ...) {

        va_list ap;
        පූර්ණ_සංඛ්‍යා r;

        assert(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ආපසු r;
}

පූර්ණ_සංඛ්‍යා parse_env_file_fd_sentinel(
                පූර්ණ_සංඛ්‍යා fd,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ආපසු r;
}

පූර්ණ_සංඛ්‍යා parse_env_datav(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                va_list ap) {

        assert(data);

        නම් (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((void*) data, size, "r");
        නම් (!f)
                ආපසු -ENOMEM;

        ආපසු parse_env_filev(f, fname, ap);
}

පූර්ණ_සංඛ්‍යා parse_env_data_sentinel(
                const var_na *data,
                size_t size,
                const var_na *fname, /* only used for logging */
                ...) {

        va_list ap;
        පූර්ණ_සංඛ්‍යා r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        ආපසු r;
}

static පූර්ණ_සංඛ්‍යා load_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = userdata;
        var_na *p;
        පූර්ණ_සංඛ්‍යා r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        නම් (r < 0)
                ආපසු r;

        p = strjoin(key, "=", value);
        නම් (!p)
                ආපසු -ENOMEM;

        r = strv_env_replace_consume(m, p);
        නම් (r < 0)
                ආපසු r;

        free(value);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා load_env_file(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        නම් (r < 0)
                ආපසු r;

        *ret = TAKE_PTR(m);
        ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා load_env_file_push_pairs(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***m = ASSERT_PTR(userdata);
        පූර්ණ_සංඛ්‍යා r;

        assert(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        නම් (r < 0)
                ආපසු r;

        /* Check if the key is present */
        සඳහා (var_na **t = *m; t && *t; t += 2)
                නම් (streq(t[0], key)) {
                        නම් (value)
                                ආපසු free_and_replace(t[1], value);
                        නැතහොත්
                                ආපසු free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        නම් (r < 0)
                ආපසු r;

        නම් (value)
                ආපසු strv_push(m, value);
        නැතහොත්
                ආපසු strv_extend(m, "");
}

පූර්ණ_සංඛ්‍යා load_env_file_pairs(FILE *f, const var_na *fname, var_na ***ret) {
        _cleanup_strv_free_ var_na **m = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(f || fname);
        assert(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        නම් (r < 0)
                ආපසු r;

        *ret = TAKE_PTR(m);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා load_env_file_pairs_fd(පූර්ණ_සංඛ්‍යා fd, const var_na *fname, var_na ***ret) {
        _cleanup_fclose_ FILE *f = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        නම් (r < 0)
                ආපසු r;

        ආපසු load_env_file_pairs(f, fname, ret);
}

static පූර්ණ_සංඛ්‍යා merge_env_file_push(
                const var_na *filename, unsigned line,
                const var_na *key, var_na *value,
                void *userdata) {

        var_na ***env = ASSERT_PTR(userdata);
        var_na *expanded_value;
        පූර්ණ_සංඛ්‍යා r;

        assert(key);

        නම් (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                ආපසු 0;
        }

        නම් (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                free(value);
                ආපසු 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        නම් (r < 0)
                ආපසු log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        ආපසු load_env_file_push(filename, line, key, value, env);
}

පූර්ණ_සංඛ්‍යා merge_env_file(
                var_na ***env,
                FILE *f,
                const var_na *fname) {

        assert(env);
        assert(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ආපසු parse_env_file_internal(f, fname, merge_env_file_push, env);
}

static void env_file_fputs_escaped(FILE *f, const var_na *p) {
        assert(f);
        assert(p);

        flockfile(f);

        නම් (string_has_cc(p, NULL) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                සඳහා (; *p; p++) {
                        නම් (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } නැතහොත්
                fputs_unlocked(p, f);

        funlockfile(f);
}

void env_file_fputs_assignment(FILE *f, const var_na *k, const var_na *v) {
        assert(f);
        assert(k);

        නම් (!v)
                ආපසු;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

static void write_env_var(FILE *f, const var_na *v) {
        const var_na *p;

        assert(f);
        assert(v);

        p = strchr(v, '=');
        නම් (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ආපසු;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

පූර්ණ_සංඛ්‍යා write_env_file_label(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fname, var_na **headers, var_na **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        පූර්ණ_සංඛ්‍යා r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(fname);

        නම් (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                නම් (r < 0)
                        ආපසු r;
        }

        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ var_na *p = NULL;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        නම් (r < 0) {
                නම් (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (void) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ආපසු r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        නම් (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ NULL, /* created= */ true, label_context);
                නම් (r < 0)
                        ආපසු r;
        }

        r = fchmod_umask(fileno(f), 0644);
        නම් (r < 0)
                ආපසු r;

        STRV_FOREACH(i, headers) {
                assert(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        නම් (r < 0)
                ආපසු r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා write_vconsole_conf(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fname, var_na **l) {
        var_na **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ආපසු write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ NULL);
}
