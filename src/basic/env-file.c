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

تعريفنوع صحيح (*ټېل_env_func_t)(
                ثابت حرف *filename,
                بینښه کرښه,
                ثابت حرف *کلی,
                حرف *ارزښت,
                فراغ *userdata);

ساكن صحيح parse_env_file_دننی(
                FILE *f,
                ثابت حرف *fname,
                ټېل_env_func_t ټېل,
                فراغ *userdata) {

        size_t n_کلی = 0, n_ارزښت = 0, وروستی_ارزښت_whitespace = SIZE_MAX, وروستی_کلی_whitespace = SIZE_MAX;
        _cleanup_free_ حرف *contents = عدم, *کلی = عدم, *ارزښت = عدم;
        بینښه کرښه = 1;
        صحيح r;

        تعداد {
                PRE_کلی,
                کلی_2,
                PRE_ارزښت,
                ارزښت_2,
                ارزښت_ESCAPE,
                SINGLE_QUOTE_ارزښت,
                DOUBLE_QUOTE_ارزښت,
                DOUBLE_QUOTE_ارزښت_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_کلی;

        أكد(f || fname);
        أكد(ټېل);

        إذا (f)
                r = read_ډک_بهیر(f, &contents, عدم);
        وإلا
                r = read_ډک_file(fname, &contents, عدم);
        إذا (r < 0)
                أرجع r;

        لكل (حرف *p = contents; *p; p++) {
                حرف c = *p;

                بدل (state) {

                حالة PRE_کلی:
                        إذا (strchr(COMMENTS, c))
                                state = COMMENT;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = کلی_2;
                                وروستی_کلی_whitespace = SIZE_MAX;

                                إذا (!GREEDY_REALLOC(کلی, n_کلی+2))
                                        أرجع -ENOMEM;

                                کلی[n_کلی++] = c;
                        }
                        اكسر;

                حالة کلی_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلی;
                                کرښه++;
                                n_کلی = 0;
                        } وإلا إذا (c == '=') {
                                state = PRE_ارزښت;
                                وروستی_ارزښت_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        وروستی_کلی_whitespace = SIZE_MAX;
                                وإلا إذا (وروستی_کلی_whitespace == SIZE_MAX)
                                         وروستی_کلی_whitespace = n_کلی;

                                إذا (!GREEDY_REALLOC(کلی, n_کلی+2))
                                        أرجع -ENOMEM;

                                کلی[n_کلی++] = c;
                        }

                        اكسر;

                حالة PRE_ارزښت:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلی;
                                کرښه++;
                                کلی[n_کلی] = 0;

                                إذا (ارزښت)
                                        ارزښت[n_ارزښت] = 0;

                                /* strip trailing whitespace from key */
                                إذا (وروستی_کلی_whitespace != SIZE_MAX)
                                        کلی[وروستی_کلی_whitespace] = 0;

                                r = ټېل(fname, کرښه, کلی, ارزښت, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_کلی = 0;
                                ارزښت = عدم;
                                n_ارزښت = 0;

                        } وإلا إذا (c == '\'')
                                state = SINGLE_QUOTE_ارزښت;
                        وإلا إذا (c == '"')
                                state = DOUBLE_QUOTE_ارزښت;
                        وإلا إذا (c == '\\')
                                state = ارزښت_ESCAPE;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = ارزښت_2;

                                إذا (!GREEDY_REALLOC(ارزښت, n_ارزښت+2))
                                        أرجع -ENOMEM;

                                ارزښت[n_ارزښت++] = c;
                        }

                        اكسر;

                حالة ارزښت_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلی;
                                کرښه++;

                                کلی[n_کلی] = 0;

                                إذا (ارزښت)
                                        ارزښت[n_ارزښت] = 0;

                                /* Chomp off trailing whitespace from value */
                                إذا (وروستی_ارزښت_whitespace != SIZE_MAX)
                                        ارزښت[وروستی_ارزښت_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                إذا (وروستی_کلی_whitespace != SIZE_MAX)
                                        کلی[وروستی_کلی_whitespace] = 0;

                                r = ټېل(fname, کرښه, کلی, ارزښت, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_کلی = 0;
                                ارزښت = عدم;
                                n_ارزښت = 0;

                        } وإلا إذا (c == '\\') {
                                state = ارزښت_ESCAPE;
                                وروستی_ارزښت_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        وروستی_ارزښت_whitespace = SIZE_MAX;
                                وإلا إذا (وروستی_ارزښت_whitespace == SIZE_MAX)
                                        وروستی_ارزښت_whitespace = n_ارزښت;

                                إذا (!GREEDY_REALLOC(ارزښت, n_ارزښت+2))
                                        أرجع -ENOMEM;

                                ارزښت[n_ارزښت++] = c;
                        }

                        اكسر;

                حالة ارزښت_ESCAPE:
                        state = ارزښت_2;

                        إذا (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                إذا (!GREEDY_REALLOC(ارزښت, n_ارزښت+2))
                                        أرجع -ENOMEM;

                                ارزښت[n_ارزښت++] = c;
                        }
                        اكسر;

                حالة SINGLE_QUOTE_ارزښت:
                        إذا (c == '\'')
                                state = PRE_ارزښت;
                        وإلا {
                                إذا (!GREEDY_REALLOC(ارزښت, n_ارزښت+2))
                                        أرجع -ENOMEM;

                                ارزښت[n_ارزښت++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_ارزښت:
                        إذا (c == '"')
                                state = PRE_ارزښت;
                        وإلا إذا (c == '\\')
                                state = DOUBLE_QUOTE_ارزښت_ESCAPE;
                        وإلا {
                                إذا (!GREEDY_REALLOC(ارزښت, n_ارزښت+2))
                                        أرجع -ENOMEM;

                                ارزښت[n_ارزښت++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_ارزښت_ESCAPE:
                        state = DOUBLE_QUOTE_ارزښت;

                        إذا (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                إذا (!GREEDY_REALLOC(ارزښت, n_ارزښت+2))
                                        أرجع -ENOMEM;
                                ارزښت[n_ارزښت++] = c;
                        } وإلا إذا (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                إذا (!GREEDY_REALLOC(ارزښت, n_ارزښت+3))
                                        أرجع -ENOMEM;
                                ارزښت[n_ارزښت++] = '\\';
                                ارزښت[n_ارزښت++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        اكسر;

                حالة COMMENT:
                        إذا (c == '\\')
                                state = COMMENT_ESCAPE;
                        وإلا إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلی;
                                کرښه++;
                        }
                        اكسر;

                حالة COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_کلی;
                                کرښه++;
                        } وإلا
                                state = COMMENT;
                        اكسر;
                }
        }

        إذا (دننه_سېټ(state,
                   PRE_ارزښت,
                   ارزښت_2,
                   ارزښت_ESCAPE,
                   SINGLE_QUOTE_ارزښت,
                   DOUBLE_QUOTE_ارزښت,
                   DOUBLE_QUOTE_ارزښت_ESCAPE)) {

                کلی[n_کلی] = 0;

                إذا (ارزښت)
                        ارزښت[n_ارزښت] = 0;

                إذا (state == ارزښت_2)
                        إذا (وروستی_ارزښت_whitespace != SIZE_MAX)
                                ارزښت[وروستی_ارزښت_whitespace] = 0;

                /* strip trailing whitespace from key */
                إذا (وروستی_کلی_whitespace != SIZE_MAX)
                        کلی[وروستی_کلی_whitespace] = 0;

                r = ټېل(fname, کرښه, کلی, ارزښت, userdata);
                إذا (r < 0)
                        أرجع r;

                ارزښت = عدم;
        }

        أرجع 0;
}

ساكن صحيح check_utf8ness_and_warn(
                ثابت حرف *filename, بینښه کرښه,
                ثابت حرف *کلی, حرف *ارزښت) {

        أكد(کلی);

        إذا (!utf8_is_valid(کلی)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(کلی);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), کرښه, p);
        }

        إذا (ارزښت && !utf8_is_valid(ارزښت)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(ارزښت);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), کرښه, کلی, p);
        }

        أرجع 0;
}

ساكن صحيح parse_env_file_ټېل(
                ثابت حرف *filename, بینښه کرښه,
                ثابت حرف *کلی, حرف *ارزښت,
                فراغ *userdata) {

        ثابت حرف *k;
        va_list aq, *ap = userdata;
        صحيح r;

        أكد(کلی);

        r = check_utf8ness_and_warn(filename, کرښه, کلی, ارزښت);
        إذا (r < 0)
                أرجع r;

        va_copy(aq, *ap);

        طالما ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                إذا (streq(کلی, k)) {
                        va_end(aq);
                        free_and_replace(*v, ارزښت);

                        أرجع 1;
                }
        }

        va_end(aq);
        حرر(ارزښت);

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
        r = parse_env_file_دننی(f, fname, parse_env_file_ټېل, &aq);
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
        r = parse_env_file_دننی(f, fname, parse_env_file_ټېل, &aq);
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
                ثابت حرف *معلومات,
                size_t اندازه,
                ثابت حرف *fname, /* only used for logging */
                va_list ap) {

        أكد(معلومات);

        إذا (اندازه == SIZE_MAX)
                اندازه = strlen_ptr(معلومات);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((فراغ*) معلومات, اندازه, "r");
        إذا (!f)
                أرجع -ENOMEM;

        أرجع parse_env_filev(f, fname, ap);
}

صحيح parse_env_معلومات_sentinel(
                ثابت حرف *معلومات,
                size_t اندازه,
                ثابت حرف *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحيح r;

        va_start(ap, fname);
        r = parse_env_datav(معلومات, اندازه, fname, ap);
        va_end(ap);

        أرجع r;
}

ساكن صحيح بارول_env_file_ټېل(
                ثابت حرف *filename, بینښه کرښه,
                ثابت حرف *کلی, حرف *ارزښت,
                فراغ *userdata) {

        حرف ***m = userdata;
        حرف *p;
        صحيح r;

        أكد(کلی);

        r = check_utf8ness_and_warn(filename, کرښه, کلی, ارزښت);
        إذا (r < 0)
                أرجع r;

        p = strjoin(کلی, "=", ارزښت);
        إذا (!p)
                أرجع -ENOMEM;

        r = strv_env_replace_consume(m, p);
        إذا (r < 0)
                أرجع r;

        حرر(ارزښت);
        أرجع 0;
}

صحيح بارول_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_دننی(f, fname, بارول_env_file_ټېل, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

ساكن صحيح بارول_env_file_ټېل_جوړې(
                ثابت حرف *filename, بینښه کرښه,
                ثابت حرف *کلی, حرف *ارزښت,
                فراغ *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحيح r;

        أكد(کلی);

        r = check_utf8ness_and_warn(filename, کرښه, کلی, ارزښت);
        إذا (r < 0)
                أرجع r;

        /* Check if the key is present */
        لكل (حرف **t = *m; t && *t; t += 2)
                إذا (streq(t[0], کلی)) {
                        إذا (ارزښت)
                                أرجع free_and_replace(t[1], ارزښت);
                        وإلا
                                أرجع free_and_strdup(t+1, "");
                }

        r = strv_extend(m, کلی);
        إذا (r < 0)
                أرجع r;

        إذا (ارزښت)
                أرجع strv_ټېل(m, ارزښت);
        وإلا
                أرجع strv_extend(m, "");
}

صحيح بارول_env_file_جوړې(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_دننی(f, fname, بارول_env_file_ټېل_جوړې, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

صحيح بارول_env_file_جوړې_fd(صحيح fd, ثابت حرف *fname, حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        إذا (r < 0)
                أرجع r;

        أرجع بارول_env_file_جوړې(f, fname, ret);
}

ساكن صحيح یوځای_env_file_ټېل(
                ثابت حرف *filename, بینښه کرښه,
                ثابت حرف *کلی, حرف *ارزښت,
                فراغ *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_ارزښت;
        صحيح r;

        أكد(کلی);

        إذا (!ارزښت) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), کرښه, کلی);
                أرجع 0;
        }

        إذا (!env_name_is_valid(کلی)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), کرښه, کلی);
                حرر(ارزښت);
                أرجع 0;
        }

        r = replace_env(ارزښت,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_ارزښت);
        إذا (r < 0)
                أرجع log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), کرښه, ارزښت);

        free_and_replace(ارزښت, expanded_ارزښت);

        log_debug("%s:%u: setting %s=%s", filename, کرښه, کلی, ارزښت);

        أرجع بارول_env_file_ټېل(filename, کرښه, کلی, ارزښت, env);
}

صحيح یوځای_env_file(
                حرف ***env,
                FILE *f,
                ثابت حرف *fname) {

        أكد(env);
        أكد(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        أرجع parse_env_file_دننی(f, fname, یوځای_env_file_ټېل, env);
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

صحيح write_env_file_label(صحيح dir_fd, ثابت حرف *fname, حرف **سرلیکونه, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(fname);

        إذا (FLAGS_سېټ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                إذا (r < 0)
                        أرجع r;
        }

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        إذا (r < 0) {
                إذا (FLAGS_سېټ(flags, WRITE_ENV_FILE_LABEL))
                        (فراغ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                أرجع r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        إذا (FLAGS_سېټ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ عدم, /* created= */ true, label_context);
                إذا (r < 0)
                        أرجع r;
        }

        r = fchmod_umask(fileno(f), 0644);
        إذا (r < 0)
                أرجع r;

        STRV_FOREACH(i, سرلیکونه) {
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
        حرف **سرلیکونه = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        أرجع write_env_file_label(dir_fd, fname, سرلیکونه, l, WRITE_ENV_FILE_LABEL, /* label_context= */ عدم);
}
