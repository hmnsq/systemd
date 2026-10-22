/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "env-file.h"
#ضمن "env-util.h"
#ضمن "escape.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "fs-util.h"
#ضمن "label-util.h"
#ضمن "log.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "tmpfile-util.h"
#ضمن "utf8.h"

تعريفنوع صحيح (*push_env_func_t)(
                ثابت حرف *filename,
                unsigned line,
                ثابت حرف *key,
                حرف *value,
                فراغ *userdata);

ساكن صحيح parse_env_file_internal(
                FILE *f,
                ثابت حرف *fname,
                push_env_func_t push,
                فراغ *userdata) {

        size_t n_key = 0, n_value = 0, last_value_whitespace = SIZE_MAX, last_key_whitespace = SIZE_MAX;
        _cleanup_free_ حرف *contents = عدم, *key = عدم, *value = عدم;
        unsigned line = 1;
        صحيح r;

        تعداد {
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

        أكد(f || fname);
        أكد(push);

        אם (f)
                r = read_full_stream(f, &contents, عدم);
        אחרת
                r = read_full_file(fname, &contents, عدم);
        אם (r < 0)
                החזר r;

        עבור (حرف *p = contents; *p; p++) {
                حرف c = *p;

                بدل (state) {

                حالة PRE_KEY:
                        אם (strchr(COMMENTS, c))
                                state = COMMENT;
                        אחרת אם (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                אם (!GREEDY_REALLOC(key, n_key+2))
                                        החזר -ENOMEM;

                                key[n_key++] = c;
                        }
                        اكسر;

                حالة KEY:
                        אם (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } אחרת אם (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } אחרת {
                                אם (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                אחרת אם (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                אם (!GREEDY_REALLOC(key, n_key+2))
                                        החזר -ENOMEM;

                                key[n_key++] = c;
                        }

                        اكسر;

                حالة PRE_VALUE:
                        אם (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                אם (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                אם (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                אם (r < 0)
                                        החזר r;

                                n_key = 0;
                                value = عدم;
                                n_value = 0;

                        } אחרת אם (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        אחרת אם (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        אחרת אם (c == '\\')
                                state = VALUE_ESCAPE;
                        אחרת אם (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                אם (!GREEDY_REALLOC(value, n_value+2))
                                        החזר -ENOMEM;

                                value[n_value++] = c;
                        }

                        اكسر;

                حالة VALUE:
                        אם (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                אם (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                אם (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                אם (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                אם (r < 0)
                                        החזר r;

                                n_key = 0;
                                value = عدم;
                                n_value = 0;

                        } אחרת אם (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } אחרת {
                                אם (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                אחרת אם (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                אם (!GREEDY_REALLOC(value, n_value+2))
                                        החזר -ENOMEM;

                                value[n_value++] = c;
                        }

                        اكسر;

                حالة VALUE_ESCAPE:
                        state = VALUE;

                        אם (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                אם (!GREEDY_REALLOC(value, n_value+2))
                                        החזר -ENOMEM;

                                value[n_value++] = c;
                        }
                        اكسر;

                حالة SINGLE_QUOTE_VALUE:
                        אם (c == '\'')
                                state = PRE_VALUE;
                        אחרת {
                                אם (!GREEDY_REALLOC(value, n_value+2))
                                        החזר -ENOMEM;

                                value[n_value++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_VALUE:
                        אם (c == '"')
                                state = PRE_VALUE;
                        אחרת אם (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        אחרת {
                                אם (!GREEDY_REALLOC(value, n_value+2))
                                        החזר -ENOMEM;

                                value[n_value++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        אם (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                אם (!GREEDY_REALLOC(value, n_value+2))
                                        החזר -ENOMEM;
                                value[n_value++] = c;
                        } אחרת אם (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                אם (!GREEDY_REALLOC(value, n_value+3))
                                        החזר -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        اكسر;

                حالة COMMENT:
                        אם (c == '\\')
                                state = COMMENT_ESCAPE;
                        אחרת אם (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        اكسر;

                حالة COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        אם (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } אחרת
                                state = COMMENT;
                        اكسر;
                }
        }

        אם (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                אם (value)
                        value[n_value] = 0;

                אם (state == VALUE)
                        אם (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                אם (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                אם (r < 0)
                        החזר r;

                value = عدم;
        }

        החזר 0;
}

ساكن صحيح check_utf8ness_and_warn(
                ثابت حرف *filename, unsigned line,
                ثابت حرف *key, حرف *value) {

        أكد(key);

        אם (!utf8_is_valid(key)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(key);
                החזר log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        אם (value && !utf8_is_valid(value)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(value);
                החזר log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        החזר 0;
}

ساكن صحيح parse_env_file_push(
                ثابت حرف *filename, unsigned line,
                ثابت حرف *key, حرف *value,
                فراغ *userdata) {

        ثابت حرف *k;
        va_list aq, *ap = userdata;
        صحيح r;

        أكد(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        אם (r < 0)
                החזר r;

        va_copy(aq, *ap);

        כלעוד ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                אם (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        החזר 1;
                }
        }

        va_end(aq);
        حرر(value);

        החזר 0;
}

صحيح parse_env_filev(
                FILE *f,
                ثابت حرف *fname,
                va_list ap) {

        صحيح r;
        va_list aq;

        أكد(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        החזר r;
}

صحيح parse_env_file_fdv(صحيح fd, ثابت حرف *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = عدم;
        va_list aq;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        אם (r < 0)
                החזר r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        החזר r;
}

صحيح parse_env_file_sentinel(
                FILE *f,
                ثابت حرف *fname,
                ...) {

        va_list ap;
        صحيح r;

        أكد(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        החזר r;
}

صحيح parse_env_file_fd_sentinel(
                صحيح fd,
                ثابت حرف *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحيح r;

        أكد(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        החזר r;
}

صحيح parse_env_datav(
                ثابت حرف *data,
                size_t size,
                ثابت حرف *fname, /* only used for logging */
                va_list ap) {

        أكد(data);

        אם (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((فراغ*) data, size, "r");
        אם (!f)
                החזר -ENOMEM;

        החזר parse_env_filev(f, fname, ap);
}

صحيح parse_env_data_sentinel(
                ثابت حرف *data,
                size_t size,
                ثابت حرف *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحيح r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        החזר r;
}

ساكن صحيح load_env_file_push(
                ثابت حرف *filename, unsigned line,
                ثابت حرف *key, حرف *value,
                فراغ *userdata) {

        حرف ***m = userdata;
        حرف *p;
        صحيح r;

        أكد(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        אם (r < 0)
                החזר r;

        p = strjoin(key, "=", value);
        אם (!p)
                החזר -ENOMEM;

        r = strv_env_replace_consume(m, p);
        אם (r < 0)
                החזר r;

        حرر(value);
        החזר 0;
}

صحيح load_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        אם (r < 0)
                החזר r;

        *ret = TAKE_PTR(m);
        החזר 0;
}

ساكن صحيح load_env_file_push_pairs(
                ثابت حرف *filename, unsigned line,
                ثابت حرف *key, حرف *value,
                فراغ *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحيح r;

        أكد(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        אם (r < 0)
                החזר r;

        /* Check if the key is present */
        עבור (حرف **t = *m; t && *t; t += 2)
                אם (streq(t[0], key)) {
                        אם (value)
                                החזר free_and_replace(t[1], value);
                        אחרת
                                החזר free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        אם (r < 0)
                החזר r;

        אם (value)
                החזר strv_push(m, value);
        אחרת
                החזר strv_extend(m, "");
}

صحيح load_env_file_pairs(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        אם (r < 0)
                החזר r;

        *ret = TAKE_PTR(m);
        החזר 0;
}

صحيح load_env_file_pairs_fd(صحيح fd, ثابت حرف *fname, حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        אם (r < 0)
                החזר r;

        החזר load_env_file_pairs(f, fname, ret);
}

ساكن صحيح merge_env_file_push(
                ثابت حرف *filename, unsigned line,
                ثابت حرف *key, حرف *value,
                فراغ *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_value;
        صحيح r;

        أكد(key);

        אם (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                החזר 0;
        }

        אם (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                حرر(value);
                החזר 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        אם (r < 0)
                החזר log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        החזר load_env_file_push(filename, line, key, value, env);
}

صحيح merge_env_file(
                حرف ***env,
                FILE *f,
                ثابت حرف *fname) {

        أكد(env);
        أكد(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        החזר parse_env_file_internal(f, fname, merge_env_file_push, env);
}

ساكن فراغ env_file_fputs_escaped(FILE *f, ثابت حرف *p) {
        أكد(f);
        أكد(p);

        flockfile(f);

        אם (string_has_cc(p, عدم) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                עבור (; *p; p++) {
                        אם (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } אחרת
                fputs_unlocked(p, f);

        funlockfile(f);
}

فراغ env_file_fputs_assignment(FILE *f, ثابت حرف *k, ثابت حرف *v) {
        أكد(f);
        أكد(k);

        אם (!v)
                החזר;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

ساكن فراغ write_env_var(FILE *f, ثابت حرف *v) {
        ثابت حرف *p;

        أكد(f);
        أكد(v);

        p = strchr(v, '=');
        אם (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                החזר;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

صحيح write_env_file_label(صحيح dir_fd, ثابت حرف *fname, حرف **headers, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(fname);

        אם (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                אם (r < 0)
                        החזר r;
        }

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        אם (r < 0) {
                אם (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (فراغ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                החזר r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        אם (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ عدم, /* created= */ true, label_context);
                אם (r < 0)
                        החזר r;
        }

        r = fchmod_umask(fileno(f), 0644);
        אם (r < 0)
                החזר r;

        STRV_FOREACH(i, headers) {
                أكد(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        אם (r < 0)
                החזר r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        החזר 0;
}

صحيح write_vconsole_conf(صحيح dir_fd, ثابت حرف *fname, حرف **l) {
        حرف **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        החזר write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ عدم);
}
