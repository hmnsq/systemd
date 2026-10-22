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

تعريفنوع صحيح (*ادفع_env_func_t)(
                ثابت حرف *filename,
                unsigned سطر,
                ثابت حرف *مفتاح,
                حرف *قيمة,
                فراغ *userdata);

ساكن صحيح parse_env_file_internal(
                FILE *f,
                ثابت حرف *fname,
                ادفع_env_func_t ادفع,
                فراغ *userdata) {

        size_t n_مفتاح = 0, n_قيمة = 0, last_قيمة_whitespace = SIZE_MAX, last_مفتاح_whitespace = SIZE_MAX;
        _cleanup_free_ حرف *contents = عدم, *مفتاح = عدم, *قيمة = عدم;
        unsigned سطر = 1;
        صحيح r;

        تعداد {
                PRE_مفتاح,
                مفتاح_2,
                PRE_قيمة,
                قيمة_2,
                قيمة_ESCAPE,
                SINGLE_QUOTE_قيمة,
                DOUBLE_QUOTE_قيمة,
                DOUBLE_QUOTE_قيمة_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_مفتاح;

        أكد(f || fname);
        أكد(ادفع);

        إذا (f)
                r = read_full_stream(f, &contents, عدم);
        وإلا
                r = read_full_file(fname, &contents, عدم);
        إذا (r < 0)
                إرجاع r;

        لكل (حرف *p = contents; *p; p++) {
                حرف c = *p;

                بدل (state) {

                حالة PRE_مفتاح:
                        إذا (strchr(COMMENTS, c))
                                state = COMMENT;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = مفتاح_2;
                                last_مفتاح_whitespace = SIZE_MAX;

                                إذا (!GREEDY_REALLOC(مفتاح, n_مفتاح+2))
                                        إرجاع -ENOMEM;

                                مفتاح[n_مفتاح++] = c;
                        }
                        اكسر;

                حالة مفتاح_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_مفتاح;
                                سطر++;
                                n_مفتاح = 0;
                        } وإلا إذا (c == '=') {
                                state = PRE_قيمة;
                                last_قيمة_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        last_مفتاح_whitespace = SIZE_MAX;
                                وإلا إذا (last_مفتاح_whitespace == SIZE_MAX)
                                         last_مفتاح_whitespace = n_مفتاح;

                                إذا (!GREEDY_REALLOC(مفتاح, n_مفتاح+2))
                                        إرجاع -ENOMEM;

                                مفتاح[n_مفتاح++] = c;
                        }

                        اكسر;

                حالة PRE_قيمة:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_مفتاح;
                                سطر++;
                                مفتاح[n_مفتاح] = 0;

                                إذا (قيمة)
                                        قيمة[n_قيمة] = 0;

                                /* strip trailing whitespace from key */
                                إذا (last_مفتاح_whitespace != SIZE_MAX)
                                        مفتاح[last_مفتاح_whitespace] = 0;

                                r = ادفع(fname, سطر, مفتاح, قيمة, userdata);
                                إذا (r < 0)
                                        إرجاع r;

                                n_مفتاح = 0;
                                قيمة = عدم;
                                n_قيمة = 0;

                        } وإلا إذا (c == '\'')
                                state = SINGLE_QUOTE_قيمة;
                        وإلا إذا (c == '"')
                                state = DOUBLE_QUOTE_قيمة;
                        وإلا إذا (c == '\\')
                                state = قيمة_ESCAPE;
                        وإلا إذا (!strchr(WHITESPACE, c)) {
                                state = قيمة_2;

                                إذا (!GREEDY_REALLOC(قيمة, n_قيمة+2))
                                        إرجاع -ENOMEM;

                                قيمة[n_قيمة++] = c;
                        }

                        اكسر;

                حالة قيمة_2:
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_مفتاح;
                                سطر++;

                                مفتاح[n_مفتاح] = 0;

                                إذا (قيمة)
                                        قيمة[n_قيمة] = 0;

                                /* Chomp off trailing whitespace from value */
                                إذا (last_قيمة_whitespace != SIZE_MAX)
                                        قيمة[last_قيمة_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                إذا (last_مفتاح_whitespace != SIZE_MAX)
                                        مفتاح[last_مفتاح_whitespace] = 0;

                                r = ادفع(fname, سطر, مفتاح, قيمة, userdata);
                                إذا (r < 0)
                                        إرجاع r;

                                n_مفتاح = 0;
                                قيمة = عدم;
                                n_قيمة = 0;

                        } وإلا إذا (c == '\\') {
                                state = قيمة_ESCAPE;
                                last_قيمة_whitespace = SIZE_MAX;
                        } وإلا {
                                إذا (!strchr(WHITESPACE, c))
                                        last_قيمة_whitespace = SIZE_MAX;
                                وإلا إذا (last_قيمة_whitespace == SIZE_MAX)
                                        last_قيمة_whitespace = n_قيمة;

                                إذا (!GREEDY_REALLOC(قيمة, n_قيمة+2))
                                        إرجاع -ENOMEM;

                                قيمة[n_قيمة++] = c;
                        }

                        اكسر;

                حالة قيمة_ESCAPE:
                        state = قيمة_2;

                        إذا (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                إذا (!GREEDY_REALLOC(قيمة, n_قيمة+2))
                                        إرجاع -ENOMEM;

                                قيمة[n_قيمة++] = c;
                        }
                        اكسر;

                حالة SINGLE_QUOTE_قيمة:
                        إذا (c == '\'')
                                state = PRE_قيمة;
                        وإلا {
                                إذا (!GREEDY_REALLOC(قيمة, n_قيمة+2))
                                        إرجاع -ENOMEM;

                                قيمة[n_قيمة++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_قيمة:
                        إذا (c == '"')
                                state = PRE_قيمة;
                        وإلا إذا (c == '\\')
                                state = DOUBLE_QUOTE_قيمة_ESCAPE;
                        وإلا {
                                إذا (!GREEDY_REALLOC(قيمة, n_قيمة+2))
                                        إرجاع -ENOMEM;

                                قيمة[n_قيمة++] = c;
                        }

                        اكسر;

                حالة DOUBLE_QUOTE_قيمة_ESCAPE:
                        state = DOUBLE_QUOTE_قيمة;

                        إذا (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                إذا (!GREEDY_REALLOC(قيمة, n_قيمة+2))
                                        إرجاع -ENOMEM;
                                قيمة[n_قيمة++] = c;
                        } وإلا إذا (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                إذا (!GREEDY_REALLOC(قيمة, n_قيمة+3))
                                        إرجاع -ENOMEM;
                                قيمة[n_قيمة++] = '\\';
                                قيمة[n_قيمة++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        اكسر;

                حالة COMMENT:
                        إذا (c == '\\')
                                state = COMMENT_ESCAPE;
                        وإلا إذا (strchr(NEWLINE, c)) {
                                state = PRE_مفتاح;
                                سطر++;
                        }
                        اكسر;

                حالة COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        إذا (strchr(NEWLINE, c)) {
                                state = PRE_مفتاح;
                                سطر++;
                        } وإلا
                                state = COMMENT;
                        اكسر;
                }
        }

        إذا (IN_SET(state,
                   PRE_قيمة,
                   قيمة_2,
                   قيمة_ESCAPE,
                   SINGLE_QUOTE_قيمة,
                   DOUBLE_QUOTE_قيمة,
                   DOUBLE_QUOTE_قيمة_ESCAPE)) {

                مفتاح[n_مفتاح] = 0;

                إذا (قيمة)
                        قيمة[n_قيمة] = 0;

                إذا (state == قيمة_2)
                        إذا (last_قيمة_whitespace != SIZE_MAX)
                                قيمة[last_قيمة_whitespace] = 0;

                /* strip trailing whitespace from key */
                إذا (last_مفتاح_whitespace != SIZE_MAX)
                        مفتاح[last_مفتاح_whitespace] = 0;

                r = ادفع(fname, سطر, مفتاح, قيمة, userdata);
                إذا (r < 0)
                        إرجاع r;

                قيمة = عدم;
        }

        إرجاع 0;
}

ساكن صحيح check_utf8ness_and_warn(
                ثابت حرف *filename, unsigned سطر,
                ثابت حرف *مفتاح, حرف *قيمة) {

        أكد(مفتاح);

        إذا (!utf8_is_valid(مفتاح)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(مفتاح);
                إرجاع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), سطر, p);
        }

        إذا (قيمة && !utf8_is_valid(قيمة)) {
                _cleanup_free_ حرف *p = عدم;

                p = utf8_escape_invalid(قيمة);
                إرجاع log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), سطر, مفتاح, p);
        }

        إرجاع 0;
}

ساكن صحيح parse_env_file_ادفع(
                ثابت حرف *filename, unsigned سطر,
                ثابت حرف *مفتاح, حرف *قيمة,
                فراغ *userdata) {

        ثابت حرف *k;
        va_list aq, *ap = userdata;
        صحيح r;

        أكد(مفتاح);

        r = check_utf8ness_and_warn(filename, سطر, مفتاح, قيمة);
        إذا (r < 0)
                إرجاع r;

        va_copy(aq, *ap);

        بينما ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                إذا (streq(مفتاح, k)) {
                        va_end(aq);
                        free_and_replace(*v, قيمة);

                        إرجاع 1;
                }
        }

        va_end(aq);
        حرر(قيمة);

        إرجاع 0;
}

صحيح parse_env_filev(
                FILE *f,
                ثابت حرف *fname,
                va_list ap) {

        صحيح r;
        va_list aq;

        أكد(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ادفع, &aq);
        va_end(aq);
        إرجاع r;
}

صحيح parse_env_file_fdv(صحيح fd, ثابت حرف *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = عدم;
        va_list aq;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        إذا (r < 0)
                إرجاع r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ادفع, &aq);
        va_end(aq);
        إرجاع r;
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

        إرجاع r;
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

        إرجاع r;
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
                إرجاع -ENOMEM;

        إرجاع parse_env_filev(f, fname, ap);
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

        إرجاع r;
}

ساكن صحيح load_env_file_ادفع(
                ثابت حرف *filename, unsigned سطر,
                ثابت حرف *مفتاح, حرف *قيمة,
                فراغ *userdata) {

        حرف ***m = userdata;
        حرف *p;
        صحيح r;

        أكد(مفتاح);

        r = check_utf8ness_and_warn(filename, سطر, مفتاح, قيمة);
        إذا (r < 0)
                إرجاع r;

        p = strjoin(مفتاح, "=", قيمة);
        إذا (!p)
                إرجاع -ENOMEM;

        r = strv_env_replace_consume(m, p);
        إذا (r < 0)
                إرجاع r;

        حرر(قيمة);
        إرجاع 0;
}

صحيح load_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ادفع, &m);
        إذا (r < 0)
                إرجاع r;

        *ret = TAKE_PTR(m);
        إرجاع 0;
}

ساكن صحيح load_env_file_ادفع_pairs(
                ثابت حرف *filename, unsigned سطر,
                ثابت حرف *مفتاح, حرف *قيمة,
                فراغ *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحيح r;

        أكد(مفتاح);

        r = check_utf8ness_and_warn(filename, سطر, مفتاح, قيمة);
        إذا (r < 0)
                إرجاع r;

        /* Check if the key is present */
        لكل (حرف **t = *m; t && *t; t += 2)
                إذا (streq(t[0], مفتاح)) {
                        إذا (قيمة)
                                إرجاع free_and_replace(t[1], قيمة);
                        وإلا
                                إرجاع free_and_strdup(t+1, "");
                }

        r = strv_extend(m, مفتاح);
        إذا (r < 0)
                إرجاع r;

        إذا (قيمة)
                إرجاع strv_ادفع(m, قيمة);
        وإلا
                إرجاع strv_extend(m, "");
}

صحيح load_env_file_pairs(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = عدم;
        صحيح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ادفع_pairs, &m);
        إذا (r < 0)
                إرجاع r;

        *ret = TAKE_PTR(m);
        إرجاع 0;
}

صحيح load_env_file_pairs_fd(صحيح fd, ثابت حرف *fname, حرف ***ret) {
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        إذا (r < 0)
                إرجاع r;

        إرجاع load_env_file_pairs(f, fname, ret);
}

ساكن صحيح merge_env_file_ادفع(
                ثابت حرف *filename, unsigned سطر,
                ثابت حرف *مفتاح, حرف *قيمة,
                فراغ *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_قيمة;
        صحيح r;

        أكد(مفتاح);

        إذا (!قيمة) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), سطر, مفتاح);
                إرجاع 0;
        }

        إذا (!env_name_is_valid(مفتاح)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), سطر, مفتاح);
                حرر(قيمة);
                إرجاع 0;
        }

        r = replace_env(قيمة,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_قيمة);
        إذا (r < 0)
                إرجاع log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), سطر, قيمة);

        free_and_replace(قيمة, expanded_قيمة);

        log_debug("%s:%u: setting %s=%s", filename, سطر, مفتاح, قيمة);

        إرجاع load_env_file_ادفع(filename, سطر, مفتاح, قيمة, env);
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

        إرجاع parse_env_file_internal(f, fname, merge_env_file_ادفع, env);
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
                إرجاع;

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
                إرجاع;
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
                        إرجاع r;
        }

        _cleanup_fclose_ FILE *f = عدم;
        _cleanup_free_ حرف *p = عدم;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        إذا (r < 0) {
                إذا (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (فراغ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                إرجاع r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        إذا (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ عدم, /* created= */ true, label_context);
                إذا (r < 0)
                        إرجاع r;
        }

        r = fchmod_umask(fileno(f), 0644);
        إذا (r < 0)
                إرجاع r;

        STRV_FOREACH(i, headers) {
                أكد(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        إذا (r < 0)
                إرجاع r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        إرجاع 0;
}

صحيح write_vconsole_conf(صحيح dir_fd, ثابت حرف *fname, حرف **l) {
        حرف **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        إرجاع write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ عدم);
}
