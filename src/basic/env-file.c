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

تعريفنوع صحيح (*ڌڪو_env_func_t)(
                ثابت حرف *filename,
                بنانشان سٽ,
                ثابت حرف *ڪنجي,
                حرف *قدر,
                فراغ *userdata);

ساكن صحيح parse_env_file_اندروني(
                FILE *f,
                ثابت حرف *fname,
                ڌڪو_env_func_t ڌڪو,
                فراغ *userdata) {

        size_t n_ڪنجي = 0, n_قدر = 0, آخري_قدر_whitespace = SIZE_MAX, آخري_ڪنجي_whitespace = SIZE_MAX;
        _cleanup_free_ حرف *contents = عدم, *ڪنجي = عدم, *قدر = عدم;
        بنانشان سٽ = 1;
        صحيح r;

        تعداد {
                PRE_ڪنجي,
                ڪنجي_2,
                PRE_قدر,
                قدر_2,
                قدر_ESCAPE,
                SINGLE_QUOTE_قدر,
                DOUBLE_QUOTE_قدر,
                DOUBLE_QUOTE_قدر_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_ڪنجي;

        أكد(f || fname);
        أكد(ڌڪو);

        إذا (f)
                r = read_ڀريل_وهڪرو(f, &contents, عدم);
        وإلا
                r = read_ڀريل_file(fname, &contents, عدم);
        إذا (r < 0)
                أرجع r;

        لكل (حرف *p = contents; *p; p++) {
                حرف c = *p;

                بدل (state) {

                حالة PRE_ڪنجي:
                        إذا (strchr(COMMENTS, c))
                                state = COMMENT;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = ڪنجي_2;
                                آخري_ڪنجي_whitespace = SIZE_MAX;

                                إذا (!GREEDY_REALLOC(ڪنجي, n_ڪنجي+2))
                                        أرجع -ENOMEM;

                                ڪنجي[n_ڪنجي++] = c;
                        }
                        اكسر;

                حالة ڪنجي_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_ڪنجي;
                                سٽ++;
                                n_ڪنجي = 0;
                        } وإلا إذا (c == '=') {
                                state = PRE_قدر;
                                آخري_قدر_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        آخري_ڪنجي_whitespace = SIZE_MAX;
                                وإلا إذا (آخري_ڪنجي_whitespace == SIZE_MAX)
                                         آخري_ڪنجي_whitespace = n_ڪنجي;

                                إذا (!GREEDY_REALLOC(ڪنجي, n_ڪنجي+2))
                                        أرجع -ENOMEM;

                                ڪنجي[n_ڪنجي++] = c;
                        }

                        اكسر;

                حالة PRE_قدر:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_ڪنجي;
                                سٽ++;
                                ڪنجي[n_ڪنجي] = 0;

                                إذا (قدر)
                                        قدر[n_قدر] = 0;

                                /* strip trailing whitespace from key */
                                إذا (آخري_ڪنجي_whitespace != SIZE_MAX)
                                        ڪنجي[آخري_ڪنجي_whitespace] = 0;

                                r = ڌڪو(fname, سٽ, ڪنجي, قدر, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_ڪنجي = 0;
                                قدر = عدم;
                                n_قدر = 0;

                        } وإلا إذا (c == '\'')
                                state = SINGLE_QUOTE_قدر;
                        وإلا إذا (c == '"')
                                state = DOUBLE_QUOTE_قدر;
                        وإلا إذا (c == '\\')
                                state = قدر_ESCAPE;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = قدر_2;

                                إذا (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        أرجع -ENOMEM;

                                قدر[n_قدر++] = c;
                        }

                        اكسر;

                حالة قدر_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_ڪنجي;
                                سٽ++;

                                ڪنجي[n_ڪنجي] = 0;

                                إذا (قدر)
                                        قدر[n_قدر] = 0;

                                /* Chomp off trailing whitespace from value */
                                إذا (آخري_قدر_whitespace != SIZE_MAX)
                                        قدر[آخري_قدر_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                إذا (آخري_ڪنجي_whitespace != SIZE_MAX)
                                        ڪنجي[آخري_ڪنجي_whitespace] = 0;

                                r = ڌڪو(fname, سٽ, ڪنجي, قدر, userdata);
                                إذا (r < 0)
                                        أرجع r;

                                n_ڪنجي = 0;
                                قدر = عدم;
                                n_قدر = 0;

                        } وإلا إذا (c == '\\') {
                                state = قدر_ESCAPE;
                                آخري_قدر_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        آخري_قدر_whitespace = SIZE_MAX;
                                وإلا إذا (آخري_قدر_whitespace == SIZE_MAX)
                                        آخري_قدر_whitespace = n_قدر;

                                إذا (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        أرجع -ENOMEM;

                                قدر[n_قدر++] = c;
                        }

                        اكسر;

                حالة قدر_ESCAPE:
                        state = قدر_2;

                        إذا (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                إذا (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        أرجع -ENOMEM;

                                قدر[n_قدر++] = c;
                        }
                        اكسر;

                حالة SINGLE_QUOTE_قدر:
                        إذا (c == '\'')
                                state = PRE_قدر;
                        وإلا {
                                إذا (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        أرجع -ENOMEM;

                                قدر[n_قدر++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_قدر:
                        إذا (c == '"')
                                state = PRE_قدر;
                        وإلا إذا (c == '\\')
                                state = DOUBLE_QUOTE_قدر_ESCAPE;
                        وإلا {
                                إذا (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        أرجع -ENOMEM;

                                قدر[n_قدر++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_قدر_ESCAPE:
                        state = DOUBLE_QUOTE_قدر;

                        إذا (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                إذا (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        أرجع -ENOMEM;
                                قدر[n_قدر++] = c;
                        } وإلا إذا (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                إذا (!GREEDY_REALLOC(قدر, n_قدر+3))
                                        أرجع -ENOMEM;
                                قدر[n_قدر++] = '\\';
                                قدر[n_قدر++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        اكسر;

                حالة COMMENT:
                        إذا (c == '\\')
                                state = COMMENT_ESCAPE;
                        وإلا إذا (strchr(NEWLINE, c)) {
                                state = PRE_ڪنجي;
                                سٽ++;
                        }
                        اكسر;

                حالة COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_ڪنجي;
                                سٽ++;
                        } وإلا
                                state = COMMENT;
                        اكسر;
                }
        }

        إذا (اندريون_سيٽ(state,
                   PRE_قدر,
                   قدر_2,
                   قدر_ESCAPE,
                   SINGLE_QUOTE_قدر,
                   DOUBLE_QUOTE_قدر,
                   DOUBLE_QUOTE_قدر_ESCAPE)) {

                ڪنجي[n_ڪنجي] = 0;

                إذا (قدر)
                        قدر[n_قدر] = 0;

                إذا (state == قدر_2)
                        إذا (آخري_قدر_whitespace != SIZE_MAX)
                                قدر[آخري_قدر_whitespace] = 0;

                /* strip trailing whitespace from key */
                إذا (آخري_ڪنجي_whitespace != SIZE_MAX)
                        ڪنجي[آخري_ڪنجي_whitespace] = 0;

                r = ڌڪو(fname, سٽ, ڪنجي, قدر, userdata);
                إذا (r < 0)
                        أرجع r;

                قدر = عدم;
        }

        أرجع 0;
}

ساكن صحيح check_utf8ness_and_warn(
                ثابت حرف *filename, بنانشان سٽ,
                ثابت حرف *ڪنجي, حرف *قدر) {

        أكد(ڪنجي);

        إذا (!utf8_is_valid(ڪنجي)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(ڪنجي);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), سٽ, p);
        }

        إذا (قدر && !utf8_is_valid(قدر)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(قدر);
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), سٽ, ڪنجي, p);
        }

        أرجع 0;
}

ساكن صحيح parse_env_file_ڌڪو(
                ثابت حرف *filename, بنانشان سٽ,
                ثابت حرف *ڪنجي, حرف *قدر,
                فراغ *userdata) {

        ثابت حرف *k;
        va_list aq, *ap = userdata;
        صحيح r;

        أكد(ڪنجي);

        r = check_utf8ness_and_warn(filename, سٽ, ڪنجي, قدر);
        إذا (r < 0)
                أرجع r;

        va_copy(aq, *ap);

        طالما ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                إذا (streq(ڪنجي, k)) {
                        va_end(aq);
                        free_and_replace(*v, قدر);

                        أرجع 1;
                }
        }

        va_end(aq);
        حرر(قدر);

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
        r = parse_env_file_اندروني(f, fname, parse_env_file_ڌڪو, &aq);
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
        r = parse_env_file_اندروني(f, fname, parse_env_file_ڌڪو, &aq);
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
                size_t سائيز,
                ثابت حرف *fname, /* only used for logging */
                va_list ap) {

        أكد(معلومات);

        إذا (سائيز == SIZE_MAX)
                سائيز = strlen_ptr(معلومات);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((فراغ*) معلومات, سائيز, "r");
        إذا (!f)
                أرجع -ENOMEM;

        أرجع parse_env_filev(f, fname, ap);
}

صحيح parse_env_معلومات_sentinel(
                ثابت حرف *معلومات,
                size_t سائيز,
                ثابت حرف *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحيح r;

        va_start(ap, fname);
        r = parse_env_datav(معلومات, سائيز, fname, ap);
        va_end(ap);

        أرجع r;
}

ساكن صحيح ڀريو_env_file_ڌڪو(
                ثابت حرف *filename, بنانشان سٽ,
                ثابت حرف *ڪنجي, حرف *قدر,
                فراغ *userdata) {

        حرف ***m = userdata;
        حرف *p;
        صحيح r;

        أكد(ڪنجي);

        r = check_utf8ness_and_warn(filename, سٽ, ڪنجي, قدر);
        إذا (r < 0)
                أرجع r;

        p = strjoin(ڪنجي, "=", قدر);
        إذا (!p)
                أرجع -ENOMEM;

        r = strv_env_replace_consume(m, p);
        إذا (r < 0)
                أرجع r;

        حرر(قدر);
        أرجع 0;
}

صحيح ڀريو_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_اندروني(f, fname, ڀريو_env_file_ڌڪو, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

ساكن صحيح ڀريو_env_file_ڌڪو_جوڙا(
                ثابت حرف *filename, بنانشان سٽ,
                ثابت حرف *ڪنجي, حرف *قدر,
                فراغ *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحيح r;

        أكد(ڪنجي);

        r = check_utf8ness_and_warn(filename, سٽ, ڪنجي, قدر);
        إذا (r < 0)
                أرجع r;

        /* Check if the key is present */
        لكل (حرف **t = *m; t && *t; t += 2)
                إذا (streq(t[0], ڪنجي)) {
                        إذا (قدر)
                                أرجع free_and_replace(t[1], قدر);
                        وإلا
                                أرجع free_and_strdup(t+1, "");
                }

        r = strv_extend(m, ڪنجي);
        إذا (r < 0)
                أرجع r;

        إذا (قدر)
                أرجع strv_ڌڪو(m, قدر);
        وإلا
                أرجع strv_extend(m, "");
}

صحيح ڀريو_env_file_جوڙا(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_اندروني(f, fname, ڀريو_env_file_ڌڪو_جوڙا, &m);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(m);
        أرجع 0;
}

صحيح ڀريو_env_file_جوڙا_fd(صحيح fd, ثابت حرف *fname, حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        إذا (r < 0)
                أرجع r;

        أرجع ڀريو_env_file_جوڙا(f, fname, ret);
}

ساكن صحيح ملايو_env_file_ڌڪو(
                ثابت حرف *filename, بنانشان سٽ,
                ثابت حرف *ڪنجي, حرف *قدر,
                فراغ *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_قدر;
        صحيح r;

        أكد(ڪنجي);

        إذا (!قدر) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), سٽ, ڪنجي);
                أرجع 0;
        }

        إذا (!env_name_is_valid(ڪنجي)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), سٽ, ڪنجي);
                حرر(قدر);
                أرجع 0;
        }

        r = replace_env(قدر,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_قدر);
        إذا (r < 0)
                أرجع log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), سٽ, قدر);

        free_and_replace(قدر, expanded_قدر);

        log_debug("%s:%u: setting %s=%s", filename, سٽ, ڪنجي, قدر);

        أرجع ڀريو_env_file_ڌڪو(filename, سٽ, ڪنجي, قدر, env);
}

صحيح ملايو_env_file(
                حرف ***env,
                FILE *f,
                ثابت حرف *fname) {

        أكد(env);
        أكد(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        أرجع parse_env_file_اندروني(f, fname, ملايو_env_file_ڌڪو, env);
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

صحيح write_env_file_label(صحيح dir_fd, ثابت حرف *fname, حرف **سرا, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(fname);

        إذا (FLAGS_سيٽ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                إذا (r < 0)
                        أرجع r;
        }

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        إذا (r < 0) {
                إذا (FLAGS_سيٽ(flags, WRITE_ENV_FILE_LABEL))
                        (فراغ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                أرجع r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        إذا (FLAGS_سيٽ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ عدم, /* created= */ true, label_context);
                إذا (r < 0)
                        أرجع r;
        }

        r = fchmod_umask(fileno(f), 0644);
        إذا (r < 0)
                أرجع r;

        STRV_FOREACH(i, سرا) {
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
        حرف **سرا = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        أرجع write_env_file_label(dir_fd, fname, سرا, l, WRITE_ENV_FILE_LABEL, /* label_context= */ عدم);
}
