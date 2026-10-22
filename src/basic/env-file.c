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

        𐡄𐡍 (f)
                r = read_full_stream(f, &contents, عدم);
        وإلا
                r = read_full_file(fname, &contents, عدم);
        𐡄𐡍 (r < 0)
                أرجع r;

        لكل (حرف *p = contents; *p; p++) {
                حرف c = *p;

                بدل (state) {

                حالة PRE_KEY:
                        𐡄𐡍 (strchr(COMMENTS, c))
                                state = COMMENT;
                        وإلا 𐡄𐡍 (!strchr(WHITESPACE, c)) {
                                state = KEY;
                                last_key_whitespace = SIZE_MAX;

                                𐡄𐡍 (!GREEDY_REALLOC(key, n_key+2))
                                        أرجع -ENOMEM;

                                key[n_key++] = c;
                        }
                        اكسر;

                حالة KEY:
                        𐡄𐡍 (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                n_key = 0;
                        } وإلا 𐡄𐡍 (c == '=') {
                                state = PRE_VALUE;
                                last_value_whitespace = SIZE_MAX;
                        } وإلا {
                                𐡄𐡍 (!strchr(WHITESPACE, c))
                                        last_key_whitespace = SIZE_MAX;
                                وإلا 𐡄𐡍 (last_key_whitespace == SIZE_MAX)
                                         last_key_whitespace = n_key;

                                𐡄𐡍 (!GREEDY_REALLOC(key, n_key+2))
                                        أرجع -ENOMEM;

                                key[n_key++] = c;
                        }

                        اكسر;

                حالة PRE_VALUE:
                        𐡄𐡍 (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                                key[n_key] = 0;

                                𐡄𐡍 (value)
                                        value[n_value] = 0;

                                /* strip trailing whitespace from key */
                                𐡄𐡍 (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                𐡄𐡍 (r < 0)
                                        أرجع r;

                                n_key = 0;
                                value = عدم;
                                n_value = 0;

                        } وإلا 𐡄𐡍 (c == '\'')
                                state = SINGLE_QUOTE_VALUE;
                        وإلا 𐡄𐡍 (c == '"')
                                state = DOUBLE_QUOTE_VALUE;
                        وإلا 𐡄𐡍 (c == '\\')
                                state = VALUE_ESCAPE;
                        وإلا 𐡄𐡍 (!strchr(WHITESPACE, c)) {
                                state = VALUE;

                                𐡄𐡍 (!GREEDY_REALLOC(value, n_value+2))
                                        أرجع -ENOMEM;

                                value[n_value++] = c;
                        }

                        اكسر;

                حالة VALUE:
                        𐡄𐡍 (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;

                                key[n_key] = 0;

                                𐡄𐡍 (value)
                                        value[n_value] = 0;

                                /* Chomp off trailing whitespace from value */
                                𐡄𐡍 (last_value_whitespace != SIZE_MAX)
                                        value[last_value_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                𐡄𐡍 (last_key_whitespace != SIZE_MAX)
                                        key[last_key_whitespace] = 0;

                                r = push(fname, line, key, value, userdata);
                                𐡄𐡍 (r < 0)
                                        أرجع r;

                                n_key = 0;
                                value = عدم;
                                n_value = 0;

                        } وإلا 𐡄𐡍 (c == '\\') {
                                state = VALUE_ESCAPE;
                                last_value_whitespace = SIZE_MAX;
                        } وإلا {
                                𐡄𐡍 (!strchr(WHITESPACE, c))
                                        last_value_whitespace = SIZE_MAX;
                                وإلا 𐡄𐡍 (last_value_whitespace == SIZE_MAX)
                                        last_value_whitespace = n_value;

                                𐡄𐡍 (!GREEDY_REALLOC(value, n_value+2))
                                        أرجع -ENOMEM;

                                value[n_value++] = c;
                        }

                        اكسر;

                حالة VALUE_ESCAPE:
                        state = VALUE;

                        𐡄𐡍 (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                𐡄𐡍 (!GREEDY_REALLOC(value, n_value+2))
                                        أرجع -ENOMEM;

                                value[n_value++] = c;
                        }
                        اكسر;

                حالة SINGLE_QUOTE_VALUE:
                        𐡄𐡍 (c == '\'')
                                state = PRE_VALUE;
                        وإلا {
                                𐡄𐡍 (!GREEDY_REALLOC(value, n_value+2))
                                        أرجع -ENOMEM;

                                value[n_value++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_VALUE:
                        𐡄𐡍 (c == '"')
                                state = PRE_VALUE;
                        وإلا 𐡄𐡍 (c == '\\')
                                state = DOUBLE_QUOTE_VALUE_ESCAPE;
                        وإلا {
                                𐡄𐡍 (!GREEDY_REALLOC(value, n_value+2))
                                        أرجع -ENOMEM;

                                value[n_value++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_VALUE_ESCAPE:
                        state = DOUBLE_QUOTE_VALUE;

                        𐡄𐡍 (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                𐡄𐡍 (!GREEDY_REALLOC(value, n_value+2))
                                        أرجع -ENOMEM;
                                value[n_value++] = c;
                        } وإلا 𐡄𐡍 (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                𐡄𐡍 (!GREEDY_REALLOC(value, n_value+3))
                                        أرجع -ENOMEM;
                                value[n_value++] = '\\';
                                value[n_value++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        اكسر;

                حالة COMMENT:
                        𐡄𐡍 (c == '\\')
                                state = COMMENT_ESCAPE;
                        وإلا 𐡄𐡍 (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        }
                        اكسر;

                حالة COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        𐡄𐡍 (strchr(NEWLINE, c)) {
                                state = PRE_KEY;
                                line++;
                        } وإلا
                                state = COMMENT;
                        اكسر;
                }
        }

        𐡄𐡍 (IN_SET(state,
                   PRE_VALUE,
                   VALUE,
                   VALUE_ESCAPE,
                   SINGLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE,
                   DOUBLE_QUOTE_VALUE_ESCAPE)) {

                key[n_key] = 0;

                𐡄𐡍 (value)
                        value[n_value] = 0;

                𐡄𐡍 (state == VALUE)
                        𐡄𐡍 (last_value_whitespace != SIZE_MAX)
                                value[last_value_whitespace] = 0;

                /* strip trailing whitespace from key */
                𐡄𐡍 (last_key_whitespace != SIZE_MAX)
                        key[last_key_whitespace] = 0;

                r = push(fname, line, key, value, userdata);
                𐡄𐡍 (r < 0)
                        أرجع r;

                value = عدم;
        }

        أرجع 0;
}

ساكن صحيح check_utf8ness_and_warn(
                ثابت حرف *filename, unsigned line,
                ثابت حرف *key, حرف *value) {

        أكد(key);

        𐡄𐡍 (!utf8_is_valid(key)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(key);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), line, p);
        }

        𐡄𐡍 (value && !utf8_is_valid(value)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(value);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), line, key, p);
        }

        أرجع 0;
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
        𐡄𐡍 (r < 0)
                أرجع r;

        va_copy(aq, *ap);

        طالما ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                𐡄𐡍 (streq(key, k)) {
                        va_end(aq);
                        free_and_replace(*v, value);

                        أرجع 1;
                }
        }

        va_end(aq);
        حرر(value);

        أرجع 0;
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
        أرجع r;
}

صحيح parse_env_file_fdv(صحيح fd, ثابت حرف *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = عدم;
        va_list aq;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        𐡄𐡍 (r < 0)
                أرجع r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_push, &aq);
        va_end(aq);
        أرجع r;
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

        أرجع r;
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

        أرجع r;
}

صحيح parse_env_datav(
                ثابت حرف *data,
                size_t size,
                ثابت حرف *fname, /* only used for logging */
                va_list ap) {

        أكد(data);

        𐡄𐡍 (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((فراغ*) data, size, "r");
        𐡄𐡍 (!f)
                أرجع -ENOMEM;

        أرجع parse_env_filev(f, fname, ap);
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

        أرجع r;
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
        𐡄𐡍 (r < 0)
                أرجع r;

        p = strjoin(key, "=", value);
        𐡄𐡍 (!p)
                أرجع -ENOMEM;

        r = strv_env_replace_consume(m, p);
        𐡄𐡍 (r < 0)
                أرجع r;

        حرر(value);
        أرجع 0;
}

صحيح load_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push, &m);
        𐡄𐡍 (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

ساكن صحيح load_env_file_push_pairs(
                ثابت حرف *filename, unsigned line,
                ثابت حرف *key, حرف *value,
                فراغ *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحيح r;

        أكد(key);

        r = check_utf8ness_and_warn(filename, line, key, value);
        𐡄𐡍 (r < 0)
                أرجع r;

        /* Check if the key is present */
        لكل (حرف **t = *m; t && *t; t += 2)
                𐡄𐡍 (streq(t[0], key)) {
                        𐡄𐡍 (value)
                                أرجع free_and_replace(t[1], value);
                        وإلا
                                أرجع free_and_strdup(t+1, "");
                }

        r = strv_extend(m, key);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (value)
                أرجع strv_push(m, value);
        وإلا
                أرجع strv_extend(m, "");
}

صحيح load_env_file_pairs(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_push_pairs, &m);
        𐡄𐡍 (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

صحيح load_env_file_pairs_fd(صحيح fd, ثابت حرف *fname, حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع load_env_file_pairs(f, fname, ret);
}

ساكن صحيح merge_env_file_push(
                ثابت حرف *filename, unsigned line,
                ثابت حرف *key, حرف *value,
                فراغ *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_value;
        صحيح r;

        أكد(key);

        𐡄𐡍 (!value) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), line, key);
                أرجع 0;
        }

        𐡄𐡍 (!env_name_is_valid(key)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), line, key);
                حرر(value);
                أرجع 0;
        }

        r = replace_env(value,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_value);
        𐡄𐡍 (r < 0)
                أرجع log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), line, value);

        free_and_replace(value, expanded_value);

        log_debug("%s:%u: setting %s=%s", filename, line, key, value);

        أرجع load_env_file_push(filename, line, key, value, env);
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

        أرجع parse_env_file_internal(f, fname, merge_env_file_push, env);
}

ساكن فراغ env_file_fputs_escaped(FILE *f, ثابت حرف *p) {
        أكد(f);
        أكد(p);

        flockfile(f);

        𐡄𐡍 (string_has_cc(p, عدم) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                لكل (; *p; p++) {
                        𐡄𐡍 (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } وإلا
                fputs_unlocked(p, f);

        funlockfile(f);
}

فراغ env_file_fputs_assignment(FILE *f, ثابت حرف *k, ثابت حرف *v) {
        أكد(f);
        أكد(k);

        𐡄𐡍 (!v)
                أرجع;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

ساكن فراغ write_env_var(FILE *f, ثابت حرف *v) {
        ثابت حرف *p;

        أكد(f);
        أكد(v);

        p = strchr(v, '=');
        𐡄𐡍 (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                أرجع;
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

        𐡄𐡍 (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        𐡄𐡍 (r < 0) {
                𐡄𐡍 (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (فراغ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                أرجع r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        𐡄𐡍 (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ عدم, /* created= */ true, label_context);
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        r = fchmod_umask(fileno(f), 0644);
        𐡄𐡍 (r < 0)
                أرجع r;

        STRV_FOREACH(i, headers) {
                أكد(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        𐡄𐡍 (r < 0)
                أرجع r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        أرجع 0;
}

صحيح write_vconsole_conf(صحيح dir_fd, ثابت حرف *fname, حرف **l) {
        حرف **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        أرجع write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ عدم);
}
