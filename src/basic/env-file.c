/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <sys/stat.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "env-file.h"
#شامل "env-util.h"
#شامل "escape.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "fs-util.h"
#شامل "label-util.h"
#شامل "log.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "tmpfile-util.h"
#شامل "utf8.h"

تعریفنوع صحیح (*افزودن_env_func_t)(
                ثابت نویسه *filename,
                unsigned سطر,
                ثابت نویسه *کلید,
                نویسه *مقدار,
                تهی *userdata);

ایستا صحیح parse_env_file_internal(
                FILE *f,
                ثابت نویسه *fname,
                افزودن_env_func_t افزودن,
                تهی *userdata) {

        size_t n_کلید = 0, n_مقدار = 0, last_مقدار_whitespace = SIZE_MAX, last_کلید_whitespace = SIZE_MAX;
        _cleanup_free_ نویسه *contents = پوچ, *کلید = پوچ, *مقدار = پوچ;
        unsigned سطر = 1;
        صحیح r;

        شمارش {
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

        اگر (f)
                r = read_full_stream(f, &contents, پوچ);
        وگرنه
                r = read_full_file(fname, &contents, پوچ);
        اگر (r < 0)
                بازگشت r;

        برای (نویسه *p = contents; *p; p++) {
                نویسه c = *p;

                انتخاب (state) {

                حالت PRE_کلید:
                        اگر (strchr(COMMENTS, c))
                                state = COMMENT;
                        وگرنه اگر (!strchr(WHITESPACE, c)) {
                                state = کلید_2;
                                last_کلید_whitespace = SIZE_MAX;

                                اگر (!GREEDY_REALLOC(کلید, n_کلید+2))
                                        بازگشت -ENOMEM;

                                کلید[n_کلید++] = c;
                        }
                        بشکن;

                حالت کلید_2:
                        اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                                n_کلید = 0;
                        } وگرنه اگر (c == '=') {
                                state = PRE_مقدار;
                                last_مقدار_whitespace = SIZE_MAX;
                        } وگرنه {
                                اگر (!strchr(WHITESPACE, c))
                                        last_کلید_whitespace = SIZE_MAX;
                                وگرنه اگر (last_کلید_whitespace == SIZE_MAX)
                                         last_کلید_whitespace = n_کلید;

                                اگر (!GREEDY_REALLOC(کلید, n_کلید+2))
                                        بازگشت -ENOMEM;

                                کلید[n_کلید++] = c;
                        }

                        بشکن;

                حالت PRE_مقدار:
                        اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                                کلید[n_کلید] = 0;

                                اگر (مقدار)
                                        مقدار[n_مقدار] = 0;

                                /* strip trailing whitespace from key */
                                اگر (last_کلید_whitespace != SIZE_MAX)
                                        کلید[last_کلید_whitespace] = 0;

                                r = افزودن(fname, سطر, کلید, مقدار, userdata);
                                اگر (r < 0)
                                        بازگشت r;

                                n_کلید = 0;
                                مقدار = پوچ;
                                n_مقدار = 0;

                        } وگرنه اگر (c == '\'')
                                state = SINGLE_QUOTE_مقدار;
                        وگرنه اگر (c == '"')
                                state = DOUBLE_QUOTE_مقدار;
                        وگرنه اگر (c == '\\')
                                state = مقدار_ESCAPE;
                        وگرنه اگر (!strchr(WHITESPACE, c)) {
                                state = مقدار_2;

                                اگر (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        بازگشت -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }

                        بشکن;

                حالت مقدار_2:
                        اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;

                                کلید[n_کلید] = 0;

                                اگر (مقدار)
                                        مقدار[n_مقدار] = 0;

                                /* Chomp off trailing whitespace from value */
                                اگر (last_مقدار_whitespace != SIZE_MAX)
                                        مقدار[last_مقدار_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                اگر (last_کلید_whitespace != SIZE_MAX)
                                        کلید[last_کلید_whitespace] = 0;

                                r = افزودن(fname, سطر, کلید, مقدار, userdata);
                                اگر (r < 0)
                                        بازگشت r;

                                n_کلید = 0;
                                مقدار = پوچ;
                                n_مقدار = 0;

                        } وگرنه اگر (c == '\\') {
                                state = مقدار_ESCAPE;
                                last_مقدار_whitespace = SIZE_MAX;
                        } وگرنه {
                                اگر (!strchr(WHITESPACE, c))
                                        last_مقدار_whitespace = SIZE_MAX;
                                وگرنه اگر (last_مقدار_whitespace == SIZE_MAX)
                                        last_مقدار_whitespace = n_مقدار;

                                اگر (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        بازگشت -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }

                        بشکن;

                حالت مقدار_ESCAPE:
                        state = مقدار_2;

                        اگر (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                اگر (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        بازگشت -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }
                        بشکن;

                حالت SINGLE_QUOTE_مقدار:
                        اگر (c == '\'')
                                state = PRE_مقدار;
                        وگرنه {
                                اگر (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        بازگشت -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }

                        بشکن;

                حالت DOUBLE_QUOTE_مقدار:
                        اگر (c == '"')
                                state = PRE_مقدار;
                        وگرنه اگر (c == '\\')
                                state = DOUBLE_QUOTE_مقدار_ESCAPE;
                        وگرنه {
                                اگر (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        بازگشت -ENOMEM;

                                مقدار[n_مقدار++] = c;
                        }

                        بشکن;

                حالت DOUBLE_QUOTE_مقدار_ESCAPE:
                        state = DOUBLE_QUOTE_مقدار;

                        اگر (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                اگر (!GREEDY_REALLOC(مقدار, n_مقدار+2))
                                        بازگشت -ENOMEM;
                                مقدار[n_مقدار++] = c;
                        } وگرنه اگر (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                اگر (!GREEDY_REALLOC(مقدار, n_مقدار+3))
                                        بازگشت -ENOMEM;
                                مقدار[n_مقدار++] = '\\';
                                مقدار[n_مقدار++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        بشکن;

                حالت COMMENT:
                        اگر (c == '\\')
                                state = COMMENT_ESCAPE;
                        وگرنه اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                        }
                        بشکن;

                حالت COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                        } وگرنه
                                state = COMMENT;
                        بشکن;
                }
        }

        اگر (IN_SET(state,
                   PRE_مقدار,
                   مقدار_2,
                   مقدار_ESCAPE,
                   SINGLE_QUOTE_مقدار,
                   DOUBLE_QUOTE_مقدار,
                   DOUBLE_QUOTE_مقدار_ESCAPE)) {

                کلید[n_کلید] = 0;

                اگر (مقدار)
                        مقدار[n_مقدار] = 0;

                اگر (state == مقدار_2)
                        اگر (last_مقدار_whitespace != SIZE_MAX)
                                مقدار[last_مقدار_whitespace] = 0;

                /* strip trailing whitespace from key */
                اگر (last_کلید_whitespace != SIZE_MAX)
                        کلید[last_کلید_whitespace] = 0;

                r = افزودن(fname, سطر, کلید, مقدار, userdata);
                اگر (r < 0)
                        بازگشت r;

                مقدار = پوچ;
        }

        بازگشت 0;
}

ایستا صحیح check_utf8ness_and_warn(
                ثابت نویسه *filename, unsigned سطر,
                ثابت نویسه *کلید, نویسه *مقدار) {

        أكد(کلید);

        اگر (!utf8_is_valid(کلید)) {
                _cleanup_free_ نویسه *p = پوچ;

                p = utf8_escape_invalid(کلید);
                بازگشت log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), سطر, p);
        }

        اگر (مقدار && !utf8_is_valid(مقدار)) {
                _cleanup_free_ نویسه *p = پوچ;

                p = utf8_escape_invalid(مقدار);
                بازگشت log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), سطر, کلید, p);
        }

        بازگشت 0;
}

ایستا صحیح parse_env_file_افزودن(
                ثابت نویسه *filename, unsigned سطر,
                ثابت نویسه *کلید, نویسه *مقدار,
                تهی *userdata) {

        ثابت نویسه *k;
        va_list aq, *ap = userdata;
        صحیح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, مقدار);
        اگر (r < 0)
                بازگشت r;

        va_copy(aq, *ap);

        وقتی ((k = va_arg(aq, ثابت نویسه *))) {
                نویسه **v;

                v = va_arg(aq, نویسه **);

                اگر (streq(کلید, k)) {
                        va_end(aq);
                        free_and_replace(*v, مقدار);

                        بازگشت 1;
                }
        }

        va_end(aq);
        آزاد(مقدار);

        بازگشت 0;
}

صحیح parse_env_filev(
                FILE *f,
                ثابت نویسه *fname,
                va_list ap) {

        صحیح r;
        va_list aq;

        أكد(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_افزودن, &aq);
        va_end(aq);
        بازگشت r;
}

صحیح parse_env_file_fdv(صحیح fd, ثابت نویسه *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = پوچ;
        va_list aq;
        صحیح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        اگر (r < 0)
                بازگشت r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_افزودن, &aq);
        va_end(aq);
        بازگشت r;
}

صحیح parse_env_file_sentinel(
                FILE *f,
                ثابت نویسه *fname,
                ...) {

        va_list ap;
        صحیح r;

        أكد(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        بازگشت r;
}

صحیح parse_env_file_fd_sentinel(
                صحیح fd,
                ثابت نویسه *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحیح r;

        أكد(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        بازگشت r;
}

صحیح parse_env_datav(
                ثابت نویسه *data,
                size_t size,
                ثابت نویسه *fname, /* only used for logging */
                va_list ap) {

        أكد(data);

        اگر (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((تهی*) data, size, "r");
        اگر (!f)
                بازگشت -ENOMEM;

        بازگشت parse_env_filev(f, fname, ap);
}

صحیح parse_env_data_sentinel(
                ثابت نویسه *data,
                size_t size,
                ثابت نویسه *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحیح r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        بازگشت r;
}

ایستا صحیح load_env_file_افزودن(
                ثابت نویسه *filename, unsigned سطر,
                ثابت نویسه *کلید, نویسه *مقدار,
                تهی *userdata) {

        نویسه ***m = userdata;
        نویسه *p;
        صحیح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, مقدار);
        اگر (r < 0)
                بازگشت r;

        p = strjoin(کلید, "=", مقدار);
        اگر (!p)
                بازگشت -ENOMEM;

        r = strv_env_replace_consume(m, p);
        اگر (r < 0)
                بازگشت r;

        آزاد(مقدار);
        بازگشت 0;
}

صحیح load_env_file(FILE *f, ثابت نویسه *fname, نویسه ***ret) {
        _cleanup_strv_free_ نویسه **m = پوچ;
        صحیح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_افزودن, &m);
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(m);
        بازگشت 0;
}

ایستا صحیح load_env_file_افزودن_pairs(
                ثابت نویسه *filename, unsigned سطر,
                ثابت نویسه *کلید, نویسه *مقدار,
                تهی *userdata) {

        نویسه ***m = ASSERT_PTR(userdata);
        صحیح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, مقدار);
        اگر (r < 0)
                بازگشت r;

        /* Check if the key is present */
        برای (نویسه **t = *m; t && *t; t += 2)
                اگر (streq(t[0], کلید)) {
                        اگر (مقدار)
                                بازگشت free_and_replace(t[1], مقدار);
                        وگرنه
                                بازگشت free_and_strdup(t+1, "");
                }

        r = strv_extend(m, کلید);
        اگر (r < 0)
                بازگشت r;

        اگر (مقدار)
                بازگشت strv_افزودن(m, مقدار);
        وگرنه
                بازگشت strv_extend(m, "");
}

صحیح load_env_file_pairs(FILE *f, ثابت نویسه *fname, نویسه ***ret) {
        _cleanup_strv_free_ نویسه **m = پوچ;
        صحیح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_internal(f, fname, load_env_file_افزودن_pairs, &m);
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(m);
        بازگشت 0;
}

صحیح load_env_file_pairs_fd(صحیح fd, ثابت نویسه *fname, نویسه ***ret) {
        _cleanup_fclose_ FILE *f = پوچ;
        صحیح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        اگر (r < 0)
                بازگشت r;

        بازگشت load_env_file_pairs(f, fname, ret);
}

ایستا صحیح merge_env_file_افزودن(
                ثابت نویسه *filename, unsigned سطر,
                ثابت نویسه *کلید, نویسه *مقدار,
                تهی *userdata) {

        نویسه ***env = ASSERT_PTR(userdata);
        نویسه *expanded_مقدار;
        صحیح r;

        أكد(کلید);

        اگر (!مقدار) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), سطر, کلید);
                بازگشت 0;
        }

        اگر (!env_name_is_valid(کلید)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), سطر, کلید);
                آزاد(مقدار);
                بازگشت 0;
        }

        r = replace_env(مقدار,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_مقدار);
        اگر (r < 0)
                بازگشت log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), سطر, مقدار);

        free_and_replace(مقدار, expanded_مقدار);

        log_debug("%s:%u: setting %s=%s", filename, سطر, کلید, مقدار);

        بازگشت load_env_file_افزودن(filename, سطر, کلید, مقدار, env);
}

صحیح merge_env_file(
                نویسه ***env,
                FILE *f,
                ثابت نویسه *fname) {

        أكد(env);
        أكد(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        بازگشت parse_env_file_internal(f, fname, merge_env_file_افزودن, env);
}

ایستا تهی env_file_fputs_escaped(FILE *f, ثابت نویسه *p) {
        أكد(f);
        أكد(p);

        flockfile(f);

        اگر (string_has_cc(p, پوچ) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                برای (; *p; p++) {
                        اگر (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } وگرنه
                fputs_unlocked(p, f);

        funlockfile(f);
}

تهی env_file_fputs_assignment(FILE *f, ثابت نویسه *k, ثابت نویسه *v) {
        أكد(f);
        أكد(k);

        اگر (!v)
                بازگشت;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

ایستا تهی write_env_var(FILE *f, ثابت نویسه *v) {
        ثابت نویسه *p;

        أكد(f);
        أكد(v);

        p = strchr(v, '=');
        اگر (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                بازگشت;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

صحیح write_env_file_label(صحیح dir_fd, ثابت نویسه *fname, نویسه **headers, نویسه **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(fname);

        اگر (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                اگر (r < 0)
                        بازگشت r;
        }

        _cleanup_fclose_ FILE *f = پوچ;
        _cleanup_free_ نویسه *p = پوچ;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        اگر (r < 0) {
                اگر (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (تهی) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                بازگشت r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        اگر (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ پوچ, /* created= */ true, label_context);
                اگر (r < 0)
                        بازگشت r;
        }

        r = fchmod_umask(fileno(f), 0644);
        اگر (r < 0)
                بازگشت r;

        STRV_FOREACH(i, headers) {
                أكد(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        اگر (r < 0)
                بازگشت r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        بازگشت 0;
}

صحیح write_vconsole_conf(صحیح dir_fd, ثابت نویسه *fname, نویسه **l) {
        نویسه **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        بازگشت write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ پوچ);
}
