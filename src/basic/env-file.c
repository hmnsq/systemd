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

تعريفنوع صحيح (*تور_env_func_t)(
                ثابت حرف *filename,
                unsigned لي,
                ثابت حرف *مبودي,
                حرف *درج,
                فراغ *userdata);

ساكن صحيح parse_env_file_internal(
                FILE *f,
                ثابت حرف *fname,
                تور_env_func_t تور,
                فراغ *userdata) {

        size_t n_مبودي = 0, n_درج = 0, last_درج_whitespace = SIZE_MAX, last_مبودي_whitespace = SIZE_MAX;
        _cleanup_free_ حرف *contents = عدم, *مبودي = عدم, *درج = عدم;
        unsigned لي = 1;
        صحيح r;

        تعداد {
                PRE_مبودي,
                مبودي_2,
                PRE_درج,
                درج_2,
                درج_ESCAPE,
                SINGLE_QUOTE_درج,
                DOUBLE_QUOTE_درج,
                DOUBLE_QUOTE_درج_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_مبودي;

        أكد(f || fname);
        أكد(تور);

        إذا (f)
                r = read_full_stream(f, &contents, عدم);
        وإلا
                r = read_full_file(fname, &contents, عدم);
        إذا (r < 0)
                أرجع r;

        لكل (حرف *p = contents; *p; p++) {
                حرف c = *p;

                بدل (state) {

                حالة PRE_مبودي:
                        إذا (strchr(COMMENTS, c))
                                state = COMMENT;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = مبودي_2;
                                last_مبودي_whitespace = SIZE_MAX;

                                إذا (!GREEDY_REALLOC(مبودي, n_مبودي+2))
                                        أرجع -ENOMEM;

                                مبودي[n_مبودي++] = c;
                        }
                        اكسر;

                حالة مبودي_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_مبودي;
                                لي++;
                                n_مبودي = 0;
                        } وإلا إذا (c == '=') {
                                state = PRE_درج;
                                last_درج_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        last_مبودي_whitespace = SIZE_MAX;
                                وإلا إذا (last_مبودي_whitespace == SIZE_MAX)
                                         last_مبودي_whitespace = n_مبودي;

                                إذا (!GREEDY_REALLOC(مبودي, n_مبودي+2))
                                        أرجع -ENOMEM;

                                مبودي[n_مبودي++] = c;
                        }

                        اكسر;

                حالة PRE_درج:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_مبودي;
                                لي++;
                                مبودي[n_مبودي] = 0;

                                إذا (درج)
                                        درج[n_درج] = 0;

                                /* strip trailing whitespace from key */
                                إذا (last_مبودي_whitespace != SIZE_MAX)
                                        مبودي[last_مبودي_whitespace] = 0;

                                r = تور(fname, لي, مبودي, درج, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_مبودي = 0;
                                درج = عدم;
                                n_درج = 0;

                        } وإلا إذا (c == '\'')
                                state = SINGLE_QUOTE_درج;
                        وإلا إذا (c == '"')
                                state = DOUBLE_QUOTE_درج;
                        وإلا إذا (c == '\\')
                                state = درج_ESCAPE;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = درج_2;

                                إذا (!GREEDY_REALLOC(درج, n_درج+2))
                                        أرجع -ENOMEM;

                                درج[n_درج++] = c;
                        }

                        اكسر;

                حالة درج_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_مبودي;
                                لي++;

                                مبودي[n_مبودي] = 0;

                                إذا (درج)
                                        درج[n_درج] = 0;

                                /* Chomp off trailing whitespace from value */
                                إذا (last_درج_whitespace != SIZE_MAX)
                                        درج[last_درج_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                إذا (last_مبودي_whitespace != SIZE_MAX)
                                        مبودي[last_مبودي_whitespace] = 0;

                                r = تور(fname, لي, مبودي, درج, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_مبودي = 0;
                                درج = عدم;
                                n_درج = 0;

                        } وإلا إذا (c == '\\') {
                                state = درج_ESCAPE;
                                last_درج_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        last_درج_whitespace = SIZE_MAX;
                                وإلا إذا (last_درج_whitespace == SIZE_MAX)
                                        last_درج_whitespace = n_درج;

                                إذا (!GREEDY_REALLOC(درج, n_درج+2))
                                        أرجع -ENOMEM;

                                درج[n_درج++] = c;
                        }

                        اكسر;

                حالة درج_ESCAPE:
                        state = درج_2;

                        إذا (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                إذا (!GREEDY_REALLOC(درج, n_درج+2))
                                        أرجع -ENOMEM;

                                درج[n_درج++] = c;
                        }
                        اكسر;

                حالة SINGLE_QUOTE_درج:
                        إذا (c == '\'')
                                state = PRE_درج;
                        وإلا {
                                إذا (!GREEDY_REALLOC(درج, n_درج+2))
                                        أرجع -ENOMEM;

                                درج[n_درج++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_درج:
                        إذا (c == '"')
                                state = PRE_درج;
                        وإلا إذا (c == '\\')
                                state = DOUBLE_QUOTE_درج_ESCAPE;
                        وإلا {
                                إذا (!GREEDY_REALLOC(درج, n_درج+2))
                                        أرجع -ENOMEM;

                                درج[n_درج++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_درج_ESCAPE:
                        state = DOUBLE_QUOTE_درج;

                        إذا (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                إذا (!GREEDY_REALLOC(درج, n_درج+2))
                                        أرجع -ENOMEM;
                                درج[n_درج++] = c;
                        } وإلا إذا (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                إذا (!GREEDY_REALLOC(درج, n_درج+3))
                                        أرجع -ENOMEM;
                                درج[n_درج++] = '\\';
                                درج[n_درج++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        اكسر;

                حالة COMMENT:
                        إذا (c == '\\')
                                state = COMMENT_ESCAPE;
                        وإلا إذا (strchr(NEWLINE, c)) {
                                state = PRE_مبودي;
                                لي++;
                        }
                        اكسر;

                حالة COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_مبودي;
                                لي++;
                        } وإلا
                                state = COMMENT;
                        اكسر;
                }
        }

        إذا (IN_SET(state,
                   PRE_درج,
                   درج_2,
                   درج_ESCAPE,
                   SINGLE_QUOTE_درج,
                   DOUBLE_QUOTE_درج,
                   DOUBLE_QUOTE_درج_ESCAPE)) {

                مبودي[n_مبودي] = 0;

                إذا (درج)
                        درج[n_درج] = 0;

                إذا (state == درج_2)
                        إذا (last_درج_whitespace != SIZE_MAX)
                                درج[last_درج_whitespace] = 0;

                /* strip trailing whitespace from key */
                إذا (last_مبودي_whitespace != SIZE_MAX)
                        مبودي[last_مبودي_whitespace] = 0;

                r = تور(fname, لي, مبودي, درج, userdata);
                إذا (r < 0)
                        أرجع r;

                درج = عدم;
        }

        أرجع 0;
}

ساكن صحيح check_utf8ness_and_warn(
                ثابت حرف *filename, unsigned لي,
                ثابت حرف *مبودي, حرف *درج) {

        أكد(مبودي);

        إذا (!utf8_is_valid(مبودي)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(مبودي);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), لي, p);
        }

        إذا (درج && !utf8_is_valid(درج)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(درج);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), لي, مبودي, p);
        }

        أرجع 0;
}

ساكن صحيح parse_env_file_تور(
                ثابت حرف *filename, unsigned لي,
                ثابت حرف *مبودي, حرف *درج,
                فراغ *userdata) {

        ثابت حرف *k;
        va_list aq, *ap = userdata;
        صحيح r;

        أكد(مبودي);

        r = check_utf8ness_and_warn(filename, لي, مبودي, درج);
        إذا (r < 0)
                أرجع r;

        va_copy(aq, *ap);

        طالما ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                إذا (streq(مبودي, k)) {
                        va_end(aq);
                        free_and_replace(*v, درج);

                        أرجع 1;
                }
        }

        va_end(aq);
        حرر(درج);

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
        r = parse_env_file_internal(f, fname, parse_env_file_تور, &aq);
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
        r = parse_env_file_internal(f, fname, parse_env_file_تور, &aq);
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

ساكن صحيح load_env_file_تور(
                ثابت حرف *filename, unsigned لي,
                ثابت حرف *مبودي, حرف *درج,
                فراغ *userdata) {

        حرف ***m = userdata;
        حرف *p;
        صحيح r;

        أكد(مبودي);

        r = check_utf8ness_and_warn(filename, لي, مبودي, درج);
        إذا (r < 0)
                أرجع r;

        p = strjoin(مبودي, "=", درج);
        إذا (!p)
                أرجع -ENOMEM;

        r = strv_env_replace_consume(m, p);
        إذا (r < 0)
                أرجع r;

        حرر(درج);
        أرجع 0;
}

صحيح load_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_تور, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

ساكن صحيح load_env_file_تور_pairs(
                ثابت حرف *filename, unsigned لي,
                ثابت حرف *مبودي, حرف *درج,
                فراغ *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحيح r;

        أكد(مبودي);

        r = check_utf8ness_and_warn(filename, لي, مبودي, درج);
        إذا (r < 0)
                أرجع r;

        /* Check if the key is present */
        لكل (حرف **t = *m; t && *t; t += 2)
                إذا (streq(t[0], مبودي)) {
                        إذا (درج)
                                أرجع free_and_replace(t[1], درج);
                        وإلا
                                أرجع free_and_strdup(t+1, "");
                }

        r = strv_extend(m, مبودي);
        إذا (r < 0)
                أرجع r;

        إذا (درج)
                أرجع strv_تور(m, درج);
        وإلا
                أرجع strv_extend(m, "");
}

صحيح load_env_file_pairs(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_تور_pairs, &m);
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

ساكن صحيح merge_env_file_تور(
                ثابت حرف *filename, unsigned لي,
                ثابت حرف *مبودي, حرف *درج,
                فراغ *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_درج;
        صحيح r;

        أكد(مبودي);

        إذا (!درج) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), لي, مبودي);
                أرجع 0;
        }

        إذا (!env_name_is_valid(مبودي)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), لي, مبودي);
                حرر(درج);
                أرجع 0;
        }

        r = replace_env(درج,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_درج);
        إذا (r < 0)
                أرجع log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), لي, درج);

        free_and_replace(درج, expanded_درج);

        log_debug("%s:%u: setting %s=%s", filename, لي, مبودي, درج);

        أرجع load_env_file_تور(filename, لي, مبودي, درج, env);
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

        أرجع parse_env_file_internal(f, fname, merge_env_file_تور, env);
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
