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

تعريفنوع صحيح (*تولق_env_func_t)(
                ثابت حرف *filename,
                unsigned باريس,
                ثابت حرف *کونچي,
                حرف *نيلاي,
                فراغ *userdata);

ساكن صحيح parse_env_file_internal(
                FILE *f,
                ثابت حرف *fname,
                تولق_env_func_t تولق,
                فراغ *userdata) {

        size_t n_کونچي = 0, n_نيلاي = 0, last_نيلاي_whitespace = SIZE_MAX, last_کونچي_whitespace = SIZE_MAX;
        _cleanup_free_ حرف *contents = عدم, *کونچي = عدم, *نيلاي = عدم;
        unsigned باريس = 1;
        صحيح r;

        تعداد {
                PRE_کونچي,
                کونچي_2,
                PRE_نيلاي,
                نيلاي_2,
                نيلاي_ESCAPE,
                SINGLE_QUOTE_نيلاي,
                DOUBLE_QUOTE_نيلاي,
                DOUBLE_QUOTE_نيلاي_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_کونچي;

        أكد(f || fname);
        أكد(تولق);

        إذا (f)
                r = read_full_stream(f, &contents, عدم);
        وإلا
                r = read_full_file(fname, &contents, عدم);
        إذا (r < 0)
                أرجع r;

        لكل (حرف *p = contents; *p; p++) {
                حرف c = *p;

                بدل (state) {

                حالة PRE_کونچي:
                        إذا (strchr(COMMENTS, c))
                                state = COMMENT;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = کونچي_2;
                                last_کونچي_whitespace = SIZE_MAX;

                                إذا (!GREEDY_REALLOC(کونچي, n_کونچي+2))
                                        أرجع -ENOMEM;

                                کونچي[n_کونچي++] = c;
                        }
                        اكسر;

                حالة کونچي_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کونچي;
                                باريس++;
                                n_کونچي = 0;
                        } وإلا إذا (c == '=') {
                                state = PRE_نيلاي;
                                last_نيلاي_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        last_کونچي_whitespace = SIZE_MAX;
                                وإلا إذا (last_کونچي_whitespace == SIZE_MAX)
                                         last_کونچي_whitespace = n_کونچي;

                                إذا (!GREEDY_REALLOC(کونچي, n_کونچي+2))
                                        أرجع -ENOMEM;

                                کونچي[n_کونچي++] = c;
                        }

                        اكسر;

                حالة PRE_نيلاي:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کونچي;
                                باريس++;
                                کونچي[n_کونچي] = 0;

                                إذا (نيلاي)
                                        نيلاي[n_نيلاي] = 0;

                                /* strip trailing whitespace from key */
                                إذا (last_کونچي_whitespace != SIZE_MAX)
                                        کونچي[last_کونچي_whitespace] = 0;

                                r = تولق(fname, باريس, کونچي, نيلاي, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_کونچي = 0;
                                نيلاي = عدم;
                                n_نيلاي = 0;

                        } وإلا إذا (c == '\'')
                                state = SINGLE_QUOTE_نيلاي;
                        وإلا إذا (c == '"')
                                state = DOUBLE_QUOTE_نيلاي;
                        وإلا إذا (c == '\\')
                                state = نيلاي_ESCAPE;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = نيلاي_2;

                                إذا (!GREEDY_REALLOC(نيلاي, n_نيلاي+2))
                                        أرجع -ENOMEM;

                                نيلاي[n_نيلاي++] = c;
                        }

                        اكسر;

                حالة نيلاي_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کونچي;
                                باريس++;

                                کونچي[n_کونچي] = 0;

                                إذا (نيلاي)
                                        نيلاي[n_نيلاي] = 0;

                                /* Chomp off trailing whitespace from value */
                                إذا (last_نيلاي_whitespace != SIZE_MAX)
                                        نيلاي[last_نيلاي_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                إذا (last_کونچي_whitespace != SIZE_MAX)
                                        کونچي[last_کونچي_whitespace] = 0;

                                r = تولق(fname, باريس, کونچي, نيلاي, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_کونچي = 0;
                                نيلاي = عدم;
                                n_نيلاي = 0;

                        } وإلا إذا (c == '\\') {
                                state = نيلاي_ESCAPE;
                                last_نيلاي_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        last_نيلاي_whitespace = SIZE_MAX;
                                وإلا إذا (last_نيلاي_whitespace == SIZE_MAX)
                                        last_نيلاي_whitespace = n_نيلاي;

                                إذا (!GREEDY_REALLOC(نيلاي, n_نيلاي+2))
                                        أرجع -ENOMEM;

                                نيلاي[n_نيلاي++] = c;
                        }

                        اكسر;

                حالة نيلاي_ESCAPE:
                        state = نيلاي_2;

                        إذا (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                إذا (!GREEDY_REALLOC(نيلاي, n_نيلاي+2))
                                        أرجع -ENOMEM;

                                نيلاي[n_نيلاي++] = c;
                        }
                        اكسر;

                حالة SINGLE_QUOTE_نيلاي:
                        إذا (c == '\'')
                                state = PRE_نيلاي;
                        وإلا {
                                إذا (!GREEDY_REALLOC(نيلاي, n_نيلاي+2))
                                        أرجع -ENOMEM;

                                نيلاي[n_نيلاي++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_نيلاي:
                        إذا (c == '"')
                                state = PRE_نيلاي;
                        وإلا إذا (c == '\\')
                                state = DOUBLE_QUOTE_نيلاي_ESCAPE;
                        وإلا {
                                إذا (!GREEDY_REALLOC(نيلاي, n_نيلاي+2))
                                        أرجع -ENOMEM;

                                نيلاي[n_نيلاي++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_نيلاي_ESCAPE:
                        state = DOUBLE_QUOTE_نيلاي;

                        إذا (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                إذا (!GREEDY_REALLOC(نيلاي, n_نيلاي+2))
                                        أرجع -ENOMEM;
                                نيلاي[n_نيلاي++] = c;
                        } وإلا إذا (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                إذا (!GREEDY_REALLOC(نيلاي, n_نيلاي+3))
                                        أرجع -ENOMEM;
                                نيلاي[n_نيلاي++] = '\\';
                                نيلاي[n_نيلاي++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        اكسر;

                حالة COMMENT:
                        إذا (c == '\\')
                                state = COMMENT_ESCAPE;
                        وإلا إذا (strchr(NEWLINE, c)) {
                                state = PRE_کونچي;
                                باريس++;
                        }
                        اكسر;

                حالة COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کونچي;
                                باريس++;
                        } وإلا
                                state = COMMENT;
                        اكسر;
                }
        }

        إذا (IN_SET(state,
                   PRE_نيلاي,
                   نيلاي_2,
                   نيلاي_ESCAPE,
                   SINGLE_QUOTE_نيلاي,
                   DOUBLE_QUOTE_نيلاي,
                   DOUBLE_QUOTE_نيلاي_ESCAPE)) {

                کونچي[n_کونچي] = 0;

                إذا (نيلاي)
                        نيلاي[n_نيلاي] = 0;

                إذا (state == نيلاي_2)
                        إذا (last_نيلاي_whitespace != SIZE_MAX)
                                نيلاي[last_نيلاي_whitespace] = 0;

                /* strip trailing whitespace from key */
                إذا (last_کونچي_whitespace != SIZE_MAX)
                        کونچي[last_کونچي_whitespace] = 0;

                r = تولق(fname, باريس, کونچي, نيلاي, userdata);
                إذا (r < 0)
                        أرجع r;

                نيلاي = عدم;
        }

        أرجع 0;
}

ساكن صحيح check_utf8ness_and_warn(
                ثابت حرف *filename, unsigned باريس,
                ثابت حرف *کونچي, حرف *نيلاي) {

        أكد(کونچي);

        إذا (!utf8_is_valid(کونچي)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(کونچي);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), باريس, p);
        }

        إذا (نيلاي && !utf8_is_valid(نيلاي)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(نيلاي);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), باريس, کونچي, p);
        }

        أرجع 0;
}

ساكن صحيح parse_env_file_تولق(
                ثابت حرف *filename, unsigned باريس,
                ثابت حرف *کونچي, حرف *نيلاي,
                فراغ *userdata) {

        ثابت حرف *k;
        va_list aq, *ap = userdata;
        صحيح r;

        أكد(کونچي);

        r = check_utf8ness_and_warn(filename, باريس, کونچي, نيلاي);
        إذا (r < 0)
                أرجع r;

        va_copy(aq, *ap);

        طالما ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                إذا (streq(کونچي, k)) {
                        va_end(aq);
                        free_and_replace(*v, نيلاي);

                        أرجع 1;
                }
        }

        va_end(aq);
        حرر(نيلاي);

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
        r = parse_env_file_internal(f, fname, parse_env_file_تولق, &aq);
        va_end(aq);
        أرجع r;
}

صحيح parse_env_file_fdv(صحيح fd, ثابت حرف *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = عدم;
        va_list aq;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        إذا (r < 0)
                أرجع r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_تولق, &aq);
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

        إذا (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((فراغ*) data, size, "r");
        إذا (!f)
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

ساكن صحيح load_env_file_تولق(
                ثابت حرف *filename, unsigned باريس,
                ثابت حرف *کونچي, حرف *نيلاي,
                فراغ *userdata) {

        حرف ***m = userdata;
        حرف *p;
        صحيح r;

        أكد(کونچي);

        r = check_utf8ness_and_warn(filename, باريس, کونچي, نيلاي);
        إذا (r < 0)
                أرجع r;

        p = strjoin(کونچي, "=", نيلاي);
        إذا (!p)
                أرجع -ENOMEM;

        r = strv_env_replace_consume(m, p);
        إذا (r < 0)
                أرجع r;

        حرر(نيلاي);
        أرجع 0;
}

صحيح load_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_تولق, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

ساكن صحيح load_env_file_تولق_pairs(
                ثابت حرف *filename, unsigned باريس,
                ثابت حرف *کونچي, حرف *نيلاي,
                فراغ *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحيح r;

        أكد(کونچي);

        r = check_utf8ness_and_warn(filename, باريس, کونچي, نيلاي);
        إذا (r < 0)
                أرجع r;

        /* Check if the key is present */
        لكل (حرف **t = *m; t && *t; t += 2)
                إذا (streq(t[0], کونچي)) {
                        إذا (نيلاي)
                                أرجع free_and_replace(t[1], نيلاي);
                        وإلا
                                أرجع free_and_strdup(t+1, "");
                }

        r = strv_extend(m, کونچي);
        إذا (r < 0)
                أرجع r;

        إذا (نيلاي)
                أرجع strv_تولق(m, نيلاي);
        وإلا
                أرجع strv_extend(m, "");
}

صحيح load_env_file_pairs(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_تولق_pairs, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

صحيح load_env_file_pairs_fd(صحيح fd, ثابت حرف *fname, حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        إذا (r < 0)
                أرجع r;

        أرجع load_env_file_pairs(f, fname, ret);
}

ساكن صحيح merge_env_file_تولق(
                ثابت حرف *filename, unsigned باريس,
                ثابت حرف *کونچي, حرف *نيلاي,
                فراغ *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_نيلاي;
        صحيح r;

        أكد(کونچي);

        إذا (!نيلاي) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), باريس, کونچي);
                أرجع 0;
        }

        إذا (!env_name_is_valid(کونچي)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), باريس, کونچي);
                حرر(نيلاي);
                أرجع 0;
        }

        r = replace_env(نيلاي,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_نيلاي);
        إذا (r < 0)
                أرجع log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), باريس, نيلاي);

        free_and_replace(نيلاي, expanded_نيلاي);

        log_debug("%s:%u: setting %s=%s", filename, باريس, کونچي, نيلاي);

        أرجع load_env_file_تولق(filename, باريس, کونچي, نيلاي, env);
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

        أرجع parse_env_file_internal(f, fname, merge_env_file_تولق, env);
}

ساكن فراغ env_file_fputs_escaped(FILE *f, ثابت حرف *p) {
        أكد(f);
        أكد(p);

        flockfile(f);

        إذا (string_has_cc(p, عدم) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                لكل (; *p; p++) {
                        إذا (strchr(SHELL_NEED_ESCAPE, *p))
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

        إذا (!v)
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
        إذا (!p) {
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

        إذا (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                إذا (r < 0)
                        أرجع r;
        }

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        إذا (r < 0) {
                إذا (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (فراغ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                أرجع r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        إذا (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ عدم, /* created= */ true, label_context);
                إذا (r < 0)
                        أرجع r;
        }

        r = fchmod_umask(fileno(f), 0644);
        إذا (r < 0)
                أرجع r;

        STRV_FOREACH(i, headers) {
                أكد(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        إذا (r < 0)
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
