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

قسمتعریف صحیح (*ڈالو_env_func_t)(
                ثابت حرف *filename,
                غیرعلامتی سطر,
                ثابت حرف *کلید,
                حرف *قدر,
                خالی *userdata);

ساکن صحیح parse_env_file_اندرونی(
                FILE *f,
                ثابت حرف *fname,
                ڈالو_env_func_t ڈالو,
                خالی *userdata) {

        size_t n_کلید = 0, n_قدر = 0, آخری_قدر_whitespace = SIZE_MAX, آخری_کلید_whitespace = SIZE_MAX;
        _cleanup_free_ حرف *contents = تہی, *کلید = تہی, *قدر = تہی;
        غیرعلامتی سطر = 1;
        صحیح r;

        شمار {
                PRE_کلید,
                کلید_2,
                PRE_قدر,
                قدر_2,
                قدر_ESCAPE,
                SINGLE_QUOTE_قدر,
                DOUBLE_QUOTE_قدر,
                DOUBLE_QUOTE_قدر_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_کلید;

        أكد(f || fname);
        أكد(ڈالو);

        اگر (f)
                r = read_بھرا_دھارا(f, &contents, تہی);
        ورنہ
                r = read_بھرا_file(fname, &contents, تہی);
        اگر (r < 0)
                واپس r;

        برائے (حرف *p = contents; *p; p++) {
                حرف c = *p;

                انتخاب (state) {

                حالت PRE_کلید:
                        اگر (strchr(COMMENTS, c))
                                state = COMMENT;
                        ورنہ اگر (!strchr(WHITESPACE, c)) {
                                state = کلید_2;
                                آخری_کلید_whitespace = SIZE_MAX;

                                اگر (!GREEDY_REALLOC(کلید, n_کلید+2))
                                        واپس -ENOMEM;

                                کلید[n_کلید++] = c;
                        }
                        توڑو;

                حالت کلید_2:
                        اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                                n_کلید = 0;
                        } ورنہ اگر (c == '=') {
                                state = PRE_قدر;
                                آخری_قدر_whitespace = SIZE_MAX;
                        } ورنہ {
                                اگر (!strchr(WHITESPACE, c))
                                        آخری_کلید_whitespace = SIZE_MAX;
                                ورنہ اگر (آخری_کلید_whitespace == SIZE_MAX)
                                         آخری_کلید_whitespace = n_کلید;

                                اگر (!GREEDY_REALLOC(کلید, n_کلید+2))
                                        واپس -ENOMEM;

                                کلید[n_کلید++] = c;
                        }

                        توڑو;

                حالت PRE_قدر:
                        اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                                کلید[n_کلید] = 0;

                                اگر (قدر)
                                        قدر[n_قدر] = 0;

                                /* strip trailing whitespace from key */
                                اگر (آخری_کلید_whitespace != SIZE_MAX)
                                        کلید[آخری_کلید_whitespace] = 0;

                                r = ڈالو(fname, سطر, کلید, قدر, userdata);
                                اگر (r < 0)
                                        واپس r;

                                n_کلید = 0;
                                قدر = تہی;
                                n_قدر = 0;

                        } ورنہ اگر (c == '\'')
                                state = SINGLE_QUOTE_قدر;
                        ورنہ اگر (c == '"')
                                state = DOUBLE_QUOTE_قدر;
                        ورنہ اگر (c == '\\')
                                state = قدر_ESCAPE;
                        ورنہ اگر (!strchr(WHITESPACE, c)) {
                                state = قدر_2;

                                اگر (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        واپس -ENOMEM;

                                قدر[n_قدر++] = c;
                        }

                        توڑو;

                حالت قدر_2:
                        اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;

                                کلید[n_کلید] = 0;

                                اگر (قدر)
                                        قدر[n_قدر] = 0;

                                /* Chomp off trailing whitespace from value */
                                اگر (آخری_قدر_whitespace != SIZE_MAX)
                                        قدر[آخری_قدر_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                اگر (آخری_کلید_whitespace != SIZE_MAX)
                                        کلید[آخری_کلید_whitespace] = 0;

                                r = ڈالو(fname, سطر, کلید, قدر, userdata);
                                اگر (r < 0)
                                        واپس r;

                                n_کلید = 0;
                                قدر = تہی;
                                n_قدر = 0;

                        } ورنہ اگر (c == '\\') {
                                state = قدر_ESCAPE;
                                آخری_قدر_whitespace = SIZE_MAX;
                        } ورنہ {
                                اگر (!strchr(WHITESPACE, c))
                                        آخری_قدر_whitespace = SIZE_MAX;
                                ورنہ اگر (آخری_قدر_whitespace == SIZE_MAX)
                                        آخری_قدر_whitespace = n_قدر;

                                اگر (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        واپس -ENOMEM;

                                قدر[n_قدر++] = c;
                        }

                        توڑو;

                حالت قدر_ESCAPE:
                        state = قدر_2;

                        اگر (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                اگر (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        واپس -ENOMEM;

                                قدر[n_قدر++] = c;
                        }
                        توڑو;

                حالت SINGLE_QUOTE_قدر:
                        اگر (c == '\'')
                                state = PRE_قدر;
                        ورنہ {
                                اگر (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        واپس -ENOMEM;

                                قدر[n_قدر++] = c;
                        }

                        توڑو;

                حالت DOUBLE_QUOTE_قدر:
                        اگر (c == '"')
                                state = PRE_قدر;
                        ورنہ اگر (c == '\\')
                                state = DOUBLE_QUOTE_قدر_ESCAPE;
                        ورنہ {
                                اگر (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        واپس -ENOMEM;

                                قدر[n_قدر++] = c;
                        }

                        توڑو;

                حالت DOUBLE_QUOTE_قدر_ESCAPE:
                        state = DOUBLE_QUOTE_قدر;

                        اگر (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                اگر (!GREEDY_REALLOC(قدر, n_قدر+2))
                                        واپس -ENOMEM;
                                قدر[n_قدر++] = c;
                        } ورنہ اگر (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                اگر (!GREEDY_REALLOC(قدر, n_قدر+3))
                                        واپس -ENOMEM;
                                قدر[n_قدر++] = '\\';
                                قدر[n_قدر++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        توڑو;

                حالت COMMENT:
                        اگر (c == '\\')
                                state = COMMENT_ESCAPE;
                        ورنہ اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                        }
                        توڑو;

                حالت COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        اگر (strchr(NEWLINE, c)) {
                                state = PRE_کلید;
                                سطر++;
                        } ورنہ
                                state = COMMENT;
                        توڑو;
                }
        }

        اگر (اندر_سیٹ(state,
                   PRE_قدر,
                   قدر_2,
                   قدر_ESCAPE,
                   SINGLE_QUOTE_قدر,
                   DOUBLE_QUOTE_قدر,
                   DOUBLE_QUOTE_قدر_ESCAPE)) {

                کلید[n_کلید] = 0;

                اگر (قدر)
                        قدر[n_قدر] = 0;

                اگر (state == قدر_2)
                        اگر (آخری_قدر_whitespace != SIZE_MAX)
                                قدر[آخری_قدر_whitespace] = 0;

                /* strip trailing whitespace from key */
                اگر (آخری_کلید_whitespace != SIZE_MAX)
                        کلید[آخری_کلید_whitespace] = 0;

                r = ڈالو(fname, سطر, کلید, قدر, userdata);
                اگر (r < 0)
                        واپس r;

                قدر = تہی;
        }

        واپس 0;
}

ساکن صحیح check_utf8ness_and_warn(
                ثابت حرف *filename, غیرعلامتی سطر,
                ثابت حرف *کلید, حرف *قدر) {

        أكد(کلید);

        اگر (!utf8_is_valid(کلید)) {
                _cleanup_free_ حرف *p = تہی;

                p = utf8_escape_invalid(کلید);
                واپس log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), سطر, p);
        }

        اگر (قدر && !utf8_is_valid(قدر)) {
                _cleanup_free_ حرف *p = تہی;

                p = utf8_escape_invalid(قدر);
                واپس log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), سطر, کلید, p);
        }

        واپس 0;
}

ساکن صحیح parse_env_file_ڈالو(
                ثابت حرف *filename, غیرعلامتی سطر,
                ثابت حرف *کلید, حرف *قدر,
                خالی *userdata) {

        ثابت حرف *k;
        va_list aq, *ap = userdata;
        صحیح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, قدر);
        اگر (r < 0)
                واپس r;

        va_copy(aq, *ap);

        جب ((k = va_arg(aq, ثابت حرف *))) {
                حرف **v;

                v = va_arg(aq, حرف **);

                اگر (streq(کلید, k)) {
                        va_end(aq);
                        free_and_replace(*v, قدر);

                        واپس 1;
                }
        }

        va_end(aq);
        آزاد(قدر);

        واپس 0;
}

صحیح parse_env_filev(
                FILE *f,
                ثابت حرف *fname,
                va_list ap) {

        صحیح r;
        va_list aq;

        أكد(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_اندرونی(f, fname, parse_env_file_ڈالو, &aq);
        va_end(aq);
        واپس r;
}

صحیح parse_env_file_fdv(صحیح fd, ثابت حرف *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = تہی;
        va_list aq;
        صحیح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        اگر (r < 0)
                واپس r;

        va_copy(aq, ap);
        r = parse_env_file_اندرونی(f, fname, parse_env_file_ڈالو, &aq);
        va_end(aq);
        واپس r;
}

صحیح parse_env_file_sentinel(
                FILE *f,
                ثابت حرف *fname,
                ...) {

        va_list ap;
        صحیح r;

        أكد(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        واپس r;
}

صحیح parse_env_file_fd_sentinel(
                صحیح fd,
                ثابت حرف *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحیح r;

        أكد(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        واپس r;
}

صحیح parse_env_datav(
                ثابت حرف *ڈیٹا,
                size_t سائز,
                ثابت حرف *fname, /* only used for logging */
                va_list ap) {

        أكد(ڈیٹا);

        اگر (سائز == SIZE_MAX)
                سائز = strlen_ptr(ڈیٹا);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((خالی*) ڈیٹا, سائز, "r");
        اگر (!f)
                واپس -ENOMEM;

        واپس parse_env_filev(f, fname, ap);
}

صحیح parse_env_ڈیٹا_sentinel(
                ثابت حرف *ڈیٹا,
                size_t سائز,
                ثابت حرف *fname, /* only used for logging */
                ...) {

        va_list ap;
        صحیح r;

        va_start(ap, fname);
        r = parse_env_datav(ڈیٹا, سائز, fname, ap);
        va_end(ap);

        واپس r;
}

ساکن صحیح بھرو_env_file_ڈالو(
                ثابت حرف *filename, غیرعلامتی سطر,
                ثابت حرف *کلید, حرف *قدر,
                خالی *userdata) {

        حرف ***m = userdata;
        حرف *p;
        صحیح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, قدر);
        اگر (r < 0)
                واپس r;

        p = strjoin(کلید, "=", قدر);
        اگر (!p)
                واپس -ENOMEM;

        r = strv_env_replace_consume(m, p);
        اگر (r < 0)
                واپس r;

        آزاد(قدر);
        واپس 0;
}

صحیح بھرو_env_file(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = تہی;
        صحیح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_اندرونی(f, fname, بھرو_env_file_ڈالو, &m);
        اگر (r < 0)
                واپس r;

        *ret = TAKE_PTR(m);
        واپس 0;
}

ساکن صحیح بھرو_env_file_ڈالو_جوڑے(
                ثابت حرف *filename, غیرعلامتی سطر,
                ثابت حرف *کلید, حرف *قدر,
                خالی *userdata) {

        حرف ***m = ASSERT_PTR(userdata);
        صحیح r;

        أكد(کلید);

        r = check_utf8ness_and_warn(filename, سطر, کلید, قدر);
        اگر (r < 0)
                واپس r;

        /* Check if the key is present */
        برائے (حرف **t = *m; t && *t; t += 2)
                اگر (streq(t[0], کلید)) {
                        اگر (قدر)
                                واپس free_and_replace(t[1], قدر);
                        ورنہ
                                واپس free_and_strdup(t+1, "");
                }

        r = strv_extend(m, کلید);
        اگر (r < 0)
                واپس r;

        اگر (قدر)
                واپس strv_ڈالو(m, قدر);
        ورنہ
                واپس strv_extend(m, "");
}

صحیح بھرو_env_file_جوڑے(FILE *f, ثابت حرف *fname, حرف ***ret) {
        _cleanup_strv_free_ حرف **m = تہی;
        صحیح r;

        أكد(f || fname);
        أكد(ret);

        r = parse_env_file_اندرونی(f, fname, بھرو_env_file_ڈالو_جوڑے, &m);
        اگر (r < 0)
                واپس r;

        *ret = TAKE_PTR(m);
        واپس 0;
}

صحیح بھرو_env_file_جوڑے_fd(صحیح fd, ثابت حرف *fname, حرف ***ret) {
        _cleanup_fclose_ FILE *f = تہی;
        صحیح r;

        أكد(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        اگر (r < 0)
                واپس r;

        واپس بھرو_env_file_جوڑے(f, fname, ret);
}

ساکن صحیح ملاؤ_env_file_ڈالو(
                ثابت حرف *filename, غیرعلامتی سطر,
                ثابت حرف *کلید, حرف *قدر,
                خالی *userdata) {

        حرف ***env = ASSERT_PTR(userdata);
        حرف *expanded_قدر;
        صحیح r;

        أكد(کلید);

        اگر (!قدر) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), سطر, کلید);
                واپس 0;
        }

        اگر (!env_name_is_valid(کلید)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), سطر, کلید);
                آزاد(قدر);
                واپس 0;
        }

        r = replace_env(قدر,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_قدر);
        اگر (r < 0)
                واپس log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), سطر, قدر);

        free_and_replace(قدر, expanded_قدر);

        log_debug("%s:%u: setting %s=%s", filename, سطر, کلید, قدر);

        واپس بھرو_env_file_ڈالو(filename, سطر, کلید, قدر, env);
}

صحیح ملاؤ_env_file(
                حرف ***env,
                FILE *f,
                ثابت حرف *fname) {

        أكد(env);
        أكد(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        واپس parse_env_file_اندرونی(f, fname, ملاؤ_env_file_ڈالو, env);
}

ساکن خالی env_file_fputs_escaped(FILE *f, ثابت حرف *p) {
        أكد(f);
        أكد(p);

        flockfile(f);

        اگر (string_has_cc(p, تہی) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                برائے (; *p; p++) {
                        اگر (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } ورنہ
                fputs_unlocked(p, f);

        funlockfile(f);
}

خالی env_file_fputs_assignment(FILE *f, ثابت حرف *k, ثابت حرف *v) {
        أكد(f);
        أكد(k);

        اگر (!v)
                واپس;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

ساکن خالی write_env_var(FILE *f, ثابت حرف *v) {
        ثابت حرف *p;

        أكد(f);
        أكد(v);

        p = strchr(v, '=');
        اگر (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                واپس;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

صحیح write_env_file_label(صحیح dir_fd, ثابت حرف *fname, حرف **سرخیاں, حرف **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(fname);

        اگر (FLAGS_سیٹ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                اگر (r < 0)
                        واپس r;
        }

        _cleanup_fclose_ FILE *f = تہی;
        _cleanup_free_ حرف *p = تہی;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        اگر (r < 0) {
                اگر (FLAGS_سیٹ(flags, WRITE_ENV_FILE_LABEL))
                        (خالی) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                واپس r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        اگر (FLAGS_سیٹ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ تہی, /* created= */ true, label_context);
                اگر (r < 0)
                        واپس r;
        }

        r = fchmod_umask(fileno(f), 0644);
        اگر (r < 0)
                واپس r;

        STRV_FOREACH(i, سرخیاں) {
                أكد(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        اگر (r < 0)
                واپس r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        واپس 0;
}

صحیح write_vconsole_conf(صحیح dir_fd, ثابت حرف *fname, حرف **l) {
        حرف **سرخیاں = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        واپس write_env_file_label(dir_fd, fname, سرخیاں, l, WRITE_ENV_FILE_LABEL, /* label_context= */ تہی);
}
