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

تعريفنوع صحيح (*افزودن_env_func_t)(
                ثابت حرف *filename,
                بیعلامت سطر,
                ثابت حرف *کلید,
                حرف *مقدار,
                فراغ *userdata);

ساكن صحيح parse_env_file_داخلی(
                FILE *f,
                ثابت حرف *fname,
                افزودن_env_func_t افزودن,
                فراغ *userdata) {

        size_t n_کلید = 0, n_مقدار = 0, آخرین_مقدار_whitespace = SIZE_MAX, آخرین_کلید_whitespace = SIZE_MAX;
        _cleanup_free_ حرف *contents = عدم, *کلید = عدم, *مقدار = عدم;
        بیعلامت سطر = 1;
        صحيح r;

        تعداد {
                PRE_کلید,
                کلید_2,
                PRE_مقدار,
                مقدار_2,
                مقدار_ESCAPE,
                SINGLE_QUOTE_مقدار,
                DOUBLE_QUOTE_مقدار,
                DOUBLE_QUOTE_مقدار_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_کلید;

        أكد(f || fname);
        أكد(افزودن);

        إذا (f)
                r = read_پر_جریان(f, &contents, عدم);
        وإلا
                r = read_پر_file(fname, &contents, عدم);
        إذا (r < 0)
                أرجع r;

        لكل (حرف *p = contents; *p; p++) {
                حرف c = *p;

                بدل (state) {

                حالة PRE_کلید:
                        إذا (strchr(COMMENTS, c))
                                state = COMMENT;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = کلید_2;
                                آخرین_کلید_whitespace = SIZE_MAX;

                                إذا (!GREEDY_REALLOC(کلید, n_کلید+2))
                                        أرجع -ENOMEM;

                                کلید[n_کلید++] = c;
                        }
                        اكسر;

                حالة کلید_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                                n_کلید = 0;
                        } وإلا إذا (c == '=') {
                                state = PRE_مقدار;
                                آخرین_مقدار_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        آخرین_کلید_whitespace = SIZE_MAX;
                                وإلا إذا (آخرین_کلید_whitespace == SIZE_MAX)
                                         آخرین_کلید_whitespace = n_کلید;

                                إذا (!GREEDY_REALLOC(کلید, n_کلید+2))
                                        أرجع -ENOMEM;

                                کلید[n_کلید++] = c;
                        }

                        اكسر;

                حالة PRE_مقدار:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                                کلید[n_کلید] = 0;

                                إذا (مقدار)
                                        مقدار[n_مقدار] = 0;

                                /* strip trailing whitespace from key */
                                إذا (آخرین_کلید_whitespace != SIZE_MAX)
                                        کلید[آخرین_کلید_whitespace] = 0;

                                r = افزودن(fname, سطر, کلید, مقدار, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_کلید = 0;
                                مقدار = عدم;
                                n_مقدار = 0;

                        } وإلا إذا (c == '\'')
                                state = SINGLE_QUOTE_مقدار;
                        وإلا إذا (c == '"')
                                state = DOUBLE_QUOTE_مقدار;
                        وإلا إذا (c == '\\')
                                state = مقدار_ESCAPE;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = مقدار_2;

                                إذا (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        أرجع -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }

                        اكسر;

                حالة مقدار_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;

                                کلید[n_کلید] = 0;

                                إذا (مقدار)
                                        مقدار[n_مقدار] = 0;

                                /* Chomp off trailing whitespace from value */
                                إذا (آخرین_مقدار_whitespace != SIZE_MAX)
                                        مقدار[آخرین_مقدار_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                إذا (آخرین_کلید_whitespace != SIZE_MAX)
                                        کلید[آخرین_کلید_whitespace] = 0;

                                r = افزودن(fname, سطر, کلید, مقدار, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_کلید = 0;
                                مقدار = عدم;
                                n_مقدار = 0;

                        } وإلا إذا (c == '\\') {
                                state = مقدار_ESCAPE;
                                آخرین_مقدار_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        آخرین_مقدار_whitespace = SIZE_MAX;
                                وإلا إذا (آخرین_مقدار_whitespace == SIZE_MAX)
                                        آخرین_مقدار_whitespace = n_مقدار;

                                إذا (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        أرجع -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }

                        اكسر;

                حالة مقدار_ESCAPE:
                        state = مقدار_2;

                        إذا (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                إذا (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        أرجع -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }
                        اكسر;

                حالة SINGLE_QUOTE_مقدار:
                        إذا (c == '\'')
                                state = PRE_مقدار;
                        وإلا {
                                إذا (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        أرجع -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_مقدار:
                        إذا (c == '"')
                                state = PRE_مقدار;
                        وإلا إذا (c == '\\')
                                state = DOUBLE_QUOTE_مقدار_ESCAPE;
                        وإلا {
                                إذا (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        أرجع -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_مقدار_ESCAPE:
                        state = DOUBLE_QUOTE_مقدار;

                        إذا (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                إذا (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        أرجع -ENOMEM;
                                مقدار[n_مقدار++] = c;
                        } وإلا إذا (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                إذا (!GREEDY_REALLOC(مقدار, n_مقدار+3))
                                        أرجع -ENOMEM;
                                مقدار[n_مقدار++] = '\\';
                                مقدار[n_مقدار++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        اكسر;

                حالة COMMENT:
                        إذا (c == '\\')
                                state = COMMENT_ESCAPE;
                        وإلا إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                        }
                        اكسر;

                حالة COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                        } وإلا
                                state = COMMENT;
                        اكسر;
                }
        }

        إذا (درون_مجموعه(state,
                   PRE_مقدار,
                   مقدار_2,
                   مقدار_ESCAPE,
                   SINGLE_QUOTE_مقدار,
                   DOUBLE_QUOTE_مقدار,
                   DOUBLE_QUOTE_مقدار_ESCAPE)) {

                کلید[n_کلید] = 0;

                إذا (مقدار)
                        مقدار[n_مقدار] = 0;

                إذا (state == مقدار_2)
                        إذا (آخرین_مقدار_whitespace != SIZE_MAX)
                                مقدار[آخرین_مقدار_whitespace] = 0;

                /* strip trailing whitespace from key */
                إذا (آخرین_کلید_whitespace != SIZE_MAX)
                        کلید[آخرین_کلید_whitespace] = 0;

                r = افزودن(fname, سطر, کلید, مقدار, userdata);
                إذا (r < 0)
                        أرجع r;

                مقدار = عدم;
        }

        أرجع 0;
}

ساكن صحيح check_utf8ness_and_warn(
                ثابت حرف *filename, بیعلامت سطر,
                ثابت حرف *کلید, حرف *مقدار) {

        أكد(کلید);

        إذا (!utf8_is_valid(کلید)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(کلید);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), سطر, p);
        }

        إذا (مقدار && !utf8_is_valid(مقدار)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(مقدار);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), سطر, کلید, p);
        }

        أرجع 0;
}

ساكن صحيح parse_env_file_افزودن(
                ثابت حرف *filename, بیعلامت سطر,
                ثابت حرف *کلید, حرف *مقدار,
                فراغ *userdata) {

        ثابت حرف *k;
        va_list aq, *ap = userdata;
        صحيح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, مقدار);
        إذا (r < 0)
                أرجع r;

        va_copy(aq, *ap);

        طالما ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                إذا (streq(کلید, k)) {
                        va_end(aq);
                        free_and_replace(*v, مقدار);

                        أرجع 1;
                }
        }

        va_end(aq);
        حرر(مقدار);

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
        r = parse_env_file_داخلی(f, fname, parse_env_file_افزودن, &aq);
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
        r = parse_env_file_داخلی(f, fname, parse_env_file_افزودن, &aq);
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
                ثابت حرف *داده,
                size_t اندازه,
                ثابت حرف *fname, /* only used for logging */
                va_list ap) {

        أكد(داده);

        إذا (اندازه == SIZE_MAX)
                اندازه = strlen_ptr(داده);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((فراغ*) داده, اندازه, "r");
        إذا (!f)
                أرجع -ENOMEM;

        أرجع parse_env_filev(f, fname, ap);
}

صحيح parse_env_داده_sentinel(
                ثابت حرف *داده,
                size_t اندازه,
                ثابت حرف *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحيح r;

        va_start(ap, fname);
        r = parse_env_datav(داده, اندازه, fname, ap);
        va_end(ap);

        أرجع r;
}

ساكن صحيح بارگیری_env_file_افزودن(
                ثابت حرف *filename, بیعلامت سطر,
                ثابت حرف *کلید, حرف *مقدار,
                فراغ *userdata) {

        حرف ***m = userdata;
        حرف *p;
        صحيح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, مقدار);
        إذا (r < 0)
                أرجع r;

        p = strjoin(کلید, "=", مقدار);
        إذا (!p)
                أرجع -ENOMEM;

        r = strv_env_replace_consume(m, p);
        إذا (r < 0)
                أرجع r;

        حرر(مقدار);
        أرجع 0;
}

صحيح بارگیری_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_داخلی(f, fname, بارگیری_env_file_افزودن, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

ساكن صحيح بارگیری_env_file_افزودن_جفتها(
                ثابت حرف *filename, بیعلامت سطر,
                ثابت حرف *کلید, حرف *مقدار,
                فراغ *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحيح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, مقدار);
        إذا (r < 0)
                أرجع r;

        /* Check if the key is present */
        لكل (حرف **t = *m; t && *t; t += 2)
                إذا (streq(t[0], کلید)) {
                        إذا (مقدار)
                                أرجع free_and_replace(t[1], مقدار);
                        وإلا
                                أرجع free_and_strdup(t+1, "");
                }

        r = strv_extend(m, کلید);
        إذا (r < 0)
                أرجع r;

        إذا (مقدار)
                أرجع strv_افزودن(m, مقدار);
        وإلا
                أرجع strv_extend(m, "");
}

صحيح بارگیری_env_file_جفتها(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_داخلی(f, fname, بارگیری_env_file_افزودن_جفتها, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

صحيح بارگیری_env_file_جفتها_fd(صحيح fd, ثابت حرف *fname, حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        إذا (r < 0)
                أرجع r;

        أرجع بارگیری_env_file_جفتها(f, fname, ret);
}

ساكن صحيح ادغام_env_file_افزودن(
                ثابت حرف *filename, بیعلامت سطر,
                ثابت حرف *کلید, حرف *مقدار,
                فراغ *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_مقدار;
        صحيح r;

        أكد(کلید);

        إذا (!مقدار) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), سطر, کلید);
                أرجع 0;
        }

        إذا (!env_name_is_valid(کلید)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), سطر, کلید);
                حرر(مقدار);
                أرجع 0;
        }

        r = replace_env(مقدار,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_مقدار);
        إذا (r < 0)
                أرجع log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), سطر, مقدار);

        free_and_replace(مقدار, expanded_مقدار);

        log_debug("%s:%u: setting %s=%s", filename, سطر, کلید, مقدار);

        أرجع بارگیری_env_file_افزودن(filename, سطر, کلید, مقدار, env);
}

صحيح ادغام_env_file(
                حرف ***env,
                FILE *f,
                ثابت حرف *fname) {

        أكد(env);
        أكد(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        أرجع parse_env_file_داخلی(f, fname, ادغام_env_file_افزودن, env);
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

صحيح write_env_file_label(صحيح dir_fd, ثابت حرف *fname, حرف **سرآیندها, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(fname);

        إذا (FLAGS_مجموعه(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                إذا (r < 0)
                        أرجع r;
        }

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        إذا (r < 0) {
                إذا (FLAGS_مجموعه(flags, WRITE_ENV_FILE_LABEL))
                        (فراغ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                أرجع r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        إذا (FLAGS_مجموعه(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ عدم, /* created= */ true, label_context);
                إذا (r < 0)
                        أرجع r;
        }

        r = fchmod_umask(fileno(f), 0644);
        إذا (r < 0)
                أرجع r;

        STRV_FOREACH(i, سرآیندها) {
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
        حرف **سرآیندها = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        أرجع write_env_file_label(dir_fd, fname, سرآیندها, l, WRITE_ENV_FILE_LABEL, /* label_context= */ عدم);
}
