/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "env-file.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "label-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "tmpfile-util.h"
#অন্তর্ভাব "utf8.h"

প্রকারলেখ পূর্ণসংখ্যা (*ঠেলো_env_func_t)(
                ধ্রুবক অক্ষর *filename,
                unsigned সারি,
                ধ্রুবক অক্ষর *চাবি,
                অক্ষর *মান,
                শূন্য *userdata);

স্থির পূর্ণসংখ্যা parse_env_file_internal(
                FILE *f,
                ধ্রুবক অক্ষর *fname,
                ঠেলো_env_func_t ঠেলো,
                শূন্য *userdata) {

        size_t n_চাবি = 0, n_মান = 0, last_মান_whitespace = SIZE_MAX, last_চাবি_whitespace = SIZE_MAX;
        _cleanup_free_ অক্ষর *contents = ফাঁকা, *চাবি = ফাঁকা, *মান = ফাঁকা;
        unsigned সারি = 1;
        পূর্ণসংখ্যা r;

        গণনা {
                PRE_চাবি,
                চাবি_2,
                PRE_মান,
                মান_2,
                মান_ESCAPE,
                SINGLE_QUOTE_মান,
                DOUBLE_QUOTE_মান,
                DOUBLE_QUOTE_মান_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_চাবি;

        প্রতিজ্ঞা(f || fname);
        প্রতিজ্ঞা(ঠেলো);

        যদি (f)
                r = read_full_stream(f, &contents, ফাঁকা);
        অন্যথা
                r = read_full_file(fname, &contents, ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        জন্য (অক্ষর *p = contents; *p; p++) {
                অক্ষর c = *p;

                নির্বাচন (state) {

                ক্ষেত্র PRE_চাবি:
                        যদি (strchr(COMMENTS, c))
                                state = COMMENT;
                        অন্যথা যদি (!strchr(WHITESPACE, c)) {
                                state = চাবি_2;
                                last_চাবি_whitespace = SIZE_MAX;

                                যদি (!GREEDY_REALLOC(চাবি, n_চাবি+2))
                                        ফেরত -ENOMEM;

                                চাবি[n_চাবি++] = c;
                        }
                        বিরতি;

                ক্ষেত্র চাবি_2:
                        যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সারি++;
                                n_চাবি = 0;
                        } অন্যথা যদি (c == '=') {
                                state = PRE_মান;
                                last_মান_whitespace = SIZE_MAX;
                        } অন্যথা {
                                যদি (!strchr(WHITESPACE, c))
                                        last_চাবি_whitespace = SIZE_MAX;
                                অন্যথা যদি (last_চাবি_whitespace == SIZE_MAX)
                                         last_চাবি_whitespace = n_চাবি;

                                যদি (!GREEDY_REALLOC(চাবি, n_চাবি+2))
                                        ফেরত -ENOMEM;

                                চাবি[n_চাবি++] = c;
                        }

                        বিরতি;

                ক্ষেত্র PRE_মান:
                        যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সারি++;
                                চাবি[n_চাবি] = 0;

                                যদি (মান)
                                        মান[n_মান] = 0;

                                /* strip trailing whitespace from key */
                                যদি (last_চাবি_whitespace != SIZE_MAX)
                                        চাবি[last_চাবি_whitespace] = 0;

                                r = ঠেলো(fname, সারি, চাবি, মান, userdata);
                                যদি (r < 0)
                                        ফেরত r;

                                n_চাবি = 0;
                                মান = ফাঁকা;
                                n_মান = 0;

                        } অন্যথা যদি (c == '\'')
                                state = SINGLE_QUOTE_মান;
                        অন্যথা যদি (c == '"')
                                state = DOUBLE_QUOTE_মান;
                        অন্যথা যদি (c == '\\')
                                state = মান_ESCAPE;
                        অন্যথা যদি (!strchr(WHITESPACE, c)) {
                                state = মান_2;

                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ফেরত -ENOMEM;

                                মান[n_মান++] = c;
                        }

                        বিরতি;

                ক্ষেত্র মান_2:
                        যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সারি++;

                                চাবি[n_চাবি] = 0;

                                যদি (মান)
                                        মান[n_মান] = 0;

                                /* Chomp off trailing whitespace from value */
                                যদি (last_মান_whitespace != SIZE_MAX)
                                        মান[last_মান_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                যদি (last_চাবি_whitespace != SIZE_MAX)
                                        চাবি[last_চাবি_whitespace] = 0;

                                r = ঠেলো(fname, সারি, চাবি, মান, userdata);
                                যদি (r < 0)
                                        ফেরত r;

                                n_চাবি = 0;
                                মান = ফাঁকা;
                                n_মান = 0;

                        } অন্যথা যদি (c == '\\') {
                                state = মান_ESCAPE;
                                last_মান_whitespace = SIZE_MAX;
                        } অন্যথা {
                                যদি (!strchr(WHITESPACE, c))
                                        last_মান_whitespace = SIZE_MAX;
                                অন্যথা যদি (last_মান_whitespace == SIZE_MAX)
                                        last_মান_whitespace = n_মান;

                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ফেরত -ENOMEM;

                                মান[n_মান++] = c;
                        }

                        বিরতি;

                ক্ষেত্র মান_ESCAPE:
                        state = মান_2;

                        যদি (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ফেরত -ENOMEM;

                                মান[n_মান++] = c;
                        }
                        বিরতি;

                ক্ষেত্র SINGLE_QUOTE_মান:
                        যদি (c == '\'')
                                state = PRE_মান;
                        অন্যথা {
                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ফেরত -ENOMEM;

                                মান[n_মান++] = c;
                        }

                        বিরতি;

                ক্ষেত্র DOUBLE_QUOTE_মান:
                        যদি (c == '"')
                                state = PRE_মান;
                        অন্যথা যদি (c == '\\')
                                state = DOUBLE_QUOTE_মান_ESCAPE;
                        অন্যথা {
                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ফেরত -ENOMEM;

                                মান[n_মান++] = c;
                        }

                        বিরতি;

                ক্ষেত্র DOUBLE_QUOTE_মান_ESCAPE:
                        state = DOUBLE_QUOTE_মান;

                        যদি (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ফেরত -ENOMEM;
                                মান[n_মান++] = c;
                        } অন্যথা যদি (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                যদি (!GREEDY_REALLOC(মান, n_মান+3))
                                        ফেরত -ENOMEM;
                                মান[n_মান++] = '\\';
                                মান[n_মান++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        বিরতি;

                ক্ষেত্র COMMENT:
                        যদি (c == '\\')
                                state = COMMENT_ESCAPE;
                        অন্যথা যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সারি++;
                        }
                        বিরতি;

                ক্ষেত্র COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সারি++;
                        } অন্যথা
                                state = COMMENT;
                        বিরতি;
                }
        }

        যদি (IN_SET(state,
                   PRE_মান,
                   মান_2,
                   মান_ESCAPE,
                   SINGLE_QUOTE_মান,
                   DOUBLE_QUOTE_মান,
                   DOUBLE_QUOTE_মান_ESCAPE)) {

                চাবি[n_চাবি] = 0;

                যদি (মান)
                        মান[n_মান] = 0;

                যদি (state == মান_2)
                        যদি (last_মান_whitespace != SIZE_MAX)
                                মান[last_মান_whitespace] = 0;

                /* strip trailing whitespace from key */
                যদি (last_চাবি_whitespace != SIZE_MAX)
                        চাবি[last_চাবি_whitespace] = 0;

                r = ঠেলো(fname, সারি, চাবি, মান, userdata);
                যদি (r < 0)
                        ফেরত r;

                মান = ফাঁকা;
        }

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা check_utf8ness_and_warn(
                ধ্রুবক অক্ষর *filename, unsigned সারি,
                ধ্রুবক অক্ষর *চাবি, অক্ষর *মান) {

        প্রতিজ্ঞা(চাবি);

        যদি (!utf8_is_valid(চাবি)) {
                _cleanup_free_ অক্ষর *p = ফাঁকা;

                p = utf8_escape_invalid(চাবি);
                ফেরত log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), সারি, p);
        }

        যদি (মান && !utf8_is_valid(মান)) {
                _cleanup_free_ অক্ষর *p = ফাঁকা;

                p = utf8_escape_invalid(মান);
                ফেরত log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), সারি, চাবি, p);
        }

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা parse_env_file_ঠেলো(
                ধ্রুবক অক্ষর *filename, unsigned সারি,
                ধ্রুবক অক্ষর *চাবি, অক্ষর *মান,
                শূন্য *userdata) {

        ধ্রুবক অক্ষর *k;
        va_list aq, *ap = userdata;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(চাবি);

        r = check_utf8ness_and_warn(filename, সারি, চাবি, মান);
        যদি (r < 0)
                ফেরত r;

        va_copy(aq, *ap);

        যখন ((k = va_arg(aq, ধ্রুবক অক্ষর *))) {
                অক্ষর **v;

                v = va_arg(aq, অক্ষর **);

                যদি (streq(চাবি, k)) {
                        va_end(aq);
                        free_and_replace(*v, মান);

                        ফেরত 1;
                }
        }

        va_end(aq);
        মুক্ত(মান);

        ফেরত 0;
}

পূর্ণসংখ্যা parse_env_filev(
                FILE *f,
                ধ্রুবক অক্ষর *fname,
                va_list ap) {

        পূর্ণসংখ্যা r;
        va_list aq;

        প্রতিজ্ঞা(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ঠেলো, &aq);
        va_end(aq);
        ফেরত r;
}

পূর্ণসংখ্যা parse_env_file_fdv(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        va_list aq;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        যদি (r < 0)
                ফেরত r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ঠেলো, &aq);
        va_end(aq);
        ফেরত r;
}

পূর্ণসংখ্যা parse_env_file_sentinel(
                FILE *f,
                ধ্রুবক অক্ষর *fname,
                ...) {

        va_list ap;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ফেরত r;
}

পূর্ণসংখ্যা parse_env_file_fd_sentinel(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *fname, /* only used for logging */
                ...) {

        va_list ap;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ফেরত r;
}

পূর্ণসংখ্যা parse_env_datav(
                ধ্রুবক অক্ষর *তথ্য,
                size_t আকার_2,
                ধ্রুবক অক্ষর *fname, /* only used for logging */
                va_list ap) {

        প্রতিজ্ঞা(তথ্য);

        যদি (আকার_2 == SIZE_MAX)
                আকার_2 = strlen_ptr(তথ্য);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((শূন্য*) তথ্য, আকার_2, "r");
        যদি (!f)
                ফেরত -ENOMEM;

        ফেরত parse_env_filev(f, fname, ap);
}

পূর্ণসংখ্যা parse_env_তথ্য_sentinel(
                ধ্রুবক অক্ষর *তথ্য,
                size_t আকার_2,
                ধ্রুবক অক্ষর *fname, /* only used for logging */
                ...) {

        va_list ap;
        পূর্ণসংখ্যা r;

        va_start(ap, fname);
        r = parse_env_datav(তথ্য, আকার_2, fname, ap);
        va_end(ap);

        ফেরত r;
}

স্থির পূর্ণসংখ্যা load_env_file_ঠেলো(
                ধ্রুবক অক্ষর *filename, unsigned সারি,
                ধ্রুবক অক্ষর *চাবি, অক্ষর *মান,
                শূন্য *userdata) {

        অক্ষর ***m = userdata;
        অক্ষর *p;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(চাবি);

        r = check_utf8ness_and_warn(filename, সারি, চাবি, মান);
        যদি (r < 0)
                ফেরত r;

        p = strjoin(চাবি, "=", মান);
        যদি (!p)
                ফেরত -ENOMEM;

        r = strv_env_replace_consume(m, p);
        যদি (r < 0)
                ফেরত r;

        মুক্ত(মান);
        ফেরত 0;
}

পূর্ণসংখ্যা load_env_file(FILE *f, ধ্রুবক অক্ষর *fname, অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **m = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f || fname);
        প্রতিজ্ঞা(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ঠেলো, &m);
        যদি (r < 0)
                ফেরত r;

        *ret = TAKE_PTR(m);
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা load_env_file_ঠেলো_pairs(
                ধ্রুবক অক্ষর *filename, unsigned সারি,
                ধ্রুবক অক্ষর *চাবি, অক্ষর *মান,
                শূন্য *userdata) {

        অক্ষর ***m = ASSERT_PTR(userdata);
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(চাবি);

        r = check_utf8ness_and_warn(filename, সারি, চাবি, মান);
        যদি (r < 0)
                ফেরত r;

        /* Check if the key is present */
        জন্য (অক্ষর **t = *m; t && *t; t += 2)
                যদি (streq(t[0], চাবি)) {
                        যদি (মান)
                                ফেরত free_and_replace(t[1], মান);
                        অন্যথা
                                ফেরত free_and_strdup(t+1, "");
                }

        r = strv_extend(m, চাবি);
        যদি (r < 0)
                ফেরত r;

        যদি (মান)
                ফেরত strv_ঠেলো(m, মান);
        অন্যথা
                ফেরত strv_extend(m, "");
}

পূর্ণসংখ্যা load_env_file_pairs(FILE *f, ধ্রুবক অক্ষর *fname, অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **m = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f || fname);
        প্রতিজ্ঞা(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ঠেলো_pairs, &m);
        যদি (r < 0)
                ফেরত r;

        *ret = TAKE_PTR(m);
        ফেরত 0;
}

পূর্ণসংখ্যা load_env_file_pairs_fd(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *fname, অক্ষর ***ret) {
        _cleanup_fclose_ FILE *f = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        যদি (r < 0)
                ফেরত r;

        ফেরত load_env_file_pairs(f, fname, ret);
}

স্থির পূর্ণসংখ্যা merge_env_file_ঠেলো(
                ধ্রুবক অক্ষর *filename, unsigned সারি,
                ধ্রুবক অক্ষর *চাবি, অক্ষর *মান,
                শূন্য *userdata) {

        অক্ষর ***env = ASSERT_PTR(userdata);
        অক্ষর *expanded_মান;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(চাবি);

        যদি (!মান) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), সারি, চাবি);
                ফেরত 0;
        }

        যদি (!env_name_is_valid(চাবি)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), সারি, চাবি);
                মুক্ত(মান);
                ফেরত 0;
        }

        r = replace_env(মান,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_মান);
        যদি (r < 0)
                ফেরত log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), সারি, মান);

        free_and_replace(মান, expanded_মান);

        log_debug("%s:%u: setting %s=%s", filename, সারি, চাবি, মান);

        ফেরত load_env_file_ঠেলো(filename, সারি, চাবি, মান, env);
}

পূর্ণসংখ্যা merge_env_file(
                অক্ষর ***env,
                FILE *f,
                ধ্রুবক অক্ষর *fname) {

        প্রতিজ্ঞা(env);
        প্রতিজ্ঞা(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ফেরত parse_env_file_internal(f, fname, merge_env_file_ঠেলো, env);
}

স্থির শূন্য env_file_fputs_escaped(FILE *f, ধ্রুবক অক্ষর *p) {
        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(p);

        flockfile(f);

        যদি (string_has_cc(p, ফাঁকা) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                জন্য (; *p; p++) {
                        যদি (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } অন্যথা
                fputs_unlocked(p, f);

        funlockfile(f);
}

শূন্য env_file_fputs_assignment(FILE *f, ধ্রুবক অক্ষর *k, ধ্রুবক অক্ষর *v) {
        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(k);

        যদি (!v)
                ফেরত;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

স্থির শূন্য write_env_var(FILE *f, ধ্রুবক অক্ষর *v) {
        ধ্রুবক অক্ষর *p;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(v);

        p = strchr(v, '=');
        যদি (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ফেরত;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

পূর্ণসংখ্যা write_env_file_label(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fname, অক্ষর **headers, অক্ষর **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(fname);

        যদি (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                যদি (r < 0)
                        ফেরত r;
        }

        _cleanup_fclose_ FILE *f = ফাঁকা;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        যদি (r < 0) {
                যদি (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (শূন্য) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ফেরত r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        যদি (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ ফাঁকা, /* created= */ true, label_context);
                যদি (r < 0)
                        ফেরত r;
        }

        r = fchmod_umask(fileno(f), 0644);
        যদি (r < 0)
                ফেরত r;

        STRV_FOREACH(i, headers) {
                প্রতিজ্ঞা(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        যদি (r < 0)
                ফেরত r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ফেরত 0;
}

পূর্ণসংখ্যা write_vconsole_conf(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fname, অক্ষর **l) {
        অক্ষর **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ফেরত write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ ফাঁকা);
}
