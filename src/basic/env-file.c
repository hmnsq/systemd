/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "env-file.h"
#অন্তর্ভাৱ "env-util.h"
#অন্তর্ভাৱ "escape.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "fs-util.h"
#অন্তর্ভাৱ "label-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "tmpfile-util.h"
#অন্তর্ভাৱ "utf8.h"

প্রকারনির্দেশ পূর্ণাঙ্ক (*ঠেলো_env_func_t)(
                ধ্রুৱ ৱর্ণ *filename,
                unsigned সাৰি,
                ধ্রুৱ ৱর্ণ *চাবি,
                ৱর্ণ *মান,
                শূন্য *userdata);

স্থির পূর্ণাঙ্ক parse_env_file_internal(
                FILE *f,
                ধ্রুৱ ৱর্ণ *fname,
                ঠেলো_env_func_t ঠেলো,
                শূন্য *userdata) {

        size_t n_চাবি = 0, n_মান = 0, last_মান_whitespace = SIZE_MAX, last_চাবি_whitespace = SIZE_MAX;
        _cleanup_free_ ৱর্ণ *contents = রিক্ত, *চাবি = রিক্ত, *মান = রিক্ত;
        unsigned সাৰি = 1;
        পূর্ণাঙ্ক r;

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
                r = read_full_stream(f, &contents, রিক্ত);
        অন্যথা
                r = read_full_file(fname, &contents, রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        বাবে (ৱর্ণ *p = contents; *p; p++) {
                ৱর্ণ c = *p;

                ৱিকল্প (state) {

                পক্ষ PRE_চাবি:
                        যদি (strchr(COMMENTS, c))
                                state = COMMENT;
                        অন্যথা যদি (!strchr(WHITESPACE, c)) {
                                state = চাবি_2;
                                last_চাবি_whitespace = SIZE_MAX;

                                যদি (!GREEDY_REALLOC(চাবি, n_চাবি+2))
                                        ঘূৰাই -ENOMEM;

                                চাবি[n_চাবি++] = c;
                        }
                        ভঙ্গ;

                পক্ষ চাবি_2:
                        যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সাৰি++;
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
                                        ঘূৰাই -ENOMEM;

                                চাবি[n_চাবি++] = c;
                        }

                        ভঙ্গ;

                পক্ষ PRE_মান:
                        যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সাৰি++;
                                চাবি[n_চাবি] = 0;

                                যদি (মান)
                                        মান[n_মান] = 0;

                                /* strip trailing whitespace from key */
                                যদি (last_চাবি_whitespace != SIZE_MAX)
                                        চাবি[last_চাবি_whitespace] = 0;

                                r = ঠেলো(fname, সাৰি, চাবি, মান, userdata);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                n_চাবি = 0;
                                মান = রিক্ত;
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
                                        ঘূৰাই -ENOMEM;

                                মান[n_মান++] = c;
                        }

                        ভঙ্গ;

                পক্ষ মান_2:
                        যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সাৰি++;

                                চাবি[n_চাবি] = 0;

                                যদি (মান)
                                        মান[n_মান] = 0;

                                /* Chomp off trailing whitespace from value */
                                যদি (last_মান_whitespace != SIZE_MAX)
                                        মান[last_মান_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                যদি (last_চাবি_whitespace != SIZE_MAX)
                                        চাবি[last_চাবি_whitespace] = 0;

                                r = ঠেলো(fname, সাৰি, চাবি, মান, userdata);
                                যদি (r < 0)
                                        ঘূৰাই r;

                                n_চাবি = 0;
                                মান = রিক্ত;
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
                                        ঘূৰাই -ENOMEM;

                                মান[n_মান++] = c;
                        }

                        ভঙ্গ;

                পক্ষ মান_ESCAPE:
                        state = মান_2;

                        যদি (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ঘূৰাই -ENOMEM;

                                মান[n_মান++] = c;
                        }
                        ভঙ্গ;

                পক্ষ SINGLE_QUOTE_মান:
                        যদি (c == '\'')
                                state = PRE_মান;
                        অন্যথা {
                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ঘূৰাই -ENOMEM;

                                মান[n_মান++] = c;
                        }

                        ভঙ্গ;

                পক্ষ DOUBLE_QUOTE_মান:
                        যদি (c == '"')
                                state = PRE_মান;
                        অন্যথা যদি (c == '\\')
                                state = DOUBLE_QUOTE_মান_ESCAPE;
                        অন্যথা {
                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ঘূৰাই -ENOMEM;

                                মান[n_মান++] = c;
                        }

                        ভঙ্গ;

                পক্ষ DOUBLE_QUOTE_মান_ESCAPE:
                        state = DOUBLE_QUOTE_মান;

                        যদি (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                যদি (!GREEDY_REALLOC(মান, n_মান+2))
                                        ঘূৰাই -ENOMEM;
                                মান[n_মান++] = c;
                        } অন্যথা যদি (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                যদি (!GREEDY_REALLOC(মান, n_মান+3))
                                        ঘূৰাই -ENOMEM;
                                মান[n_মান++] = '\\';
                                মান[n_মান++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        ভঙ্গ;

                পক্ষ COMMENT:
                        যদি (c == '\\')
                                state = COMMENT_ESCAPE;
                        অন্যথা যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সাৰি++;
                        }
                        ভঙ্গ;

                পক্ষ COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        যদি (strchr(NEWLINE, c)) {
                                state = PRE_চাবি;
                                সাৰি++;
                        } অন্যথা
                                state = COMMENT;
                        ভঙ্গ;
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

                r = ঠেলো(fname, সাৰি, চাবি, মান, userdata);
                যদি (r < 0)
                        ঘূৰাই r;

                মান = রিক্ত;
        }

        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক check_utf8ness_and_warn(
                ধ্রুৱ ৱর্ণ *filename, unsigned সাৰি,
                ধ্রুৱ ৱর্ণ *চাবি, ৱর্ণ *মান) {

        প্রতিজ্ঞা(চাবি);

        যদি (!utf8_is_valid(চাবি)) {
                _cleanup_free_ ৱর্ণ *p = রিক্ত;

                p = utf8_escape_invalid(চাবি);
                ঘূৰাই log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), সাৰি, p);
        }

        যদি (মান && !utf8_is_valid(মান)) {
                _cleanup_free_ ৱর্ণ *p = রিক্ত;

                p = utf8_escape_invalid(মান);
                ঘূৰাই log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), সাৰি, চাবি, p);
        }

        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক parse_env_file_ঠেলো(
                ধ্রুৱ ৱর্ণ *filename, unsigned সাৰি,
                ধ্রুৱ ৱর্ণ *চাবি, ৱর্ণ *মান,
                শূন্য *userdata) {

        ধ্রুৱ ৱর্ণ *k;
        va_list aq, *ap = userdata;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(চাবি);

        r = check_utf8ness_and_warn(filename, সাৰি, চাবি, মান);
        যদি (r < 0)
                ঘূৰাই r;

        va_copy(aq, *ap);

        যেতিয়ালৈ ((k = va_arg(aq, ধ্রুৱ ৱর্ণ *))) {
                ৱর্ণ **v;

                v = va_arg(aq, ৱর্ণ **);

                যদি (streq(চাবি, k)) {
                        va_end(aq);
                        free_and_replace(*v, মান);

                        ঘূৰাই 1;
                }
        }

        va_end(aq);
        মুক্ত(মান);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_env_filev(
                FILE *f,
                ধ্রুৱ ৱর্ণ *fname,
                va_list ap) {

        পূর্ণাঙ্ক r;
        va_list aq;

        প্রতিজ্ঞা(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ঠেলো, &aq);
        va_end(aq);
        ঘূৰাই r;
}

পূর্ণাঙ্ক parse_env_file_fdv(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        va_list aq;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        যদি (r < 0)
                ঘূৰাই r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ঠেলো, &aq);
        va_end(aq);
        ঘূৰাই r;
}

পূর্ণাঙ্ক parse_env_file_sentinel(
                FILE *f,
                ধ্রুৱ ৱর্ণ *fname,
                ...) {

        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ঘূৰাই r;
}

পূর্ণাঙ্ক parse_env_file_fd_sentinel(
                পূর্ণাঙ্ক fd,
                ধ্রুৱ ৱর্ণ *fname, /* only used for logging */
                ...) {

        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ঘূৰাই r;
}

পূর্ণাঙ্ক parse_env_datav(
                ধ্রুৱ ৱর্ণ *data,
                size_t size,
                ধ্রুৱ ৱর্ণ *fname, /* only used for logging */
                va_list ap) {

        প্রতিজ্ঞা(data);

        যদি (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((শূন্য*) data, size, "r");
        যদি (!f)
                ঘূৰাই -ENOMEM;

        ঘূৰাই parse_env_filev(f, fname, ap);
}

পূর্ণাঙ্ক parse_env_data_sentinel(
                ধ্রুৱ ৱর্ণ *data,
                size_t size,
                ধ্রুৱ ৱর্ণ *fname, /* only used for logging */
                ...) {

        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        ঘূৰাই r;
}

স্থির পূর্ণাঙ্ক load_env_file_ঠেলো(
                ধ্রুৱ ৱর্ণ *filename, unsigned সাৰি,
                ধ্রুৱ ৱর্ণ *চাবি, ৱর্ণ *মান,
                শূন্য *userdata) {

        ৱর্ণ ***m = userdata;
        ৱর্ণ *p;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(চাবি);

        r = check_utf8ness_and_warn(filename, সাৰি, চাবি, মান);
        যদি (r < 0)
                ঘূৰাই r;

        p = strjoin(চাবি, "=", মান);
        যদি (!p)
                ঘূৰাই -ENOMEM;

        r = strv_env_replace_consume(m, p);
        যদি (r < 0)
                ঘূৰাই r;

        মুক্ত(মান);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক load_env_file(FILE *f, ধ্রুৱ ৱর্ণ *fname, ৱর্ণ ***ret) {
        _cleanup_strv_free_ ৱর্ণ **m = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f || fname);
        প্রতিজ্ঞা(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ঠেলো, &m);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = TAKE_PTR(m);
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক load_env_file_ঠেলো_pairs(
                ধ্রুৱ ৱর্ণ *filename, unsigned সাৰি,
                ধ্রুৱ ৱর্ণ *চাবি, ৱর্ণ *মান,
                শূন্য *userdata) {

        ৱর্ণ ***m = ASSERT_PTR(userdata);
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(চাবি);

        r = check_utf8ness_and_warn(filename, সাৰি, চাবি, মান);
        যদি (r < 0)
                ঘূৰাই r;

        /* Check if the key is present */
        বাবে (ৱর্ণ **t = *m; t && *t; t += 2)
                যদি (streq(t[0], চাবি)) {
                        যদি (মান)
                                ঘূৰাই free_and_replace(t[1], মান);
                        অন্যথা
                                ঘূৰাই free_and_strdup(t+1, "");
                }

        r = strv_extend(m, চাবি);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (মান)
                ঘূৰাই strv_ঠেলো(m, মান);
        অন্যথা
                ঘূৰাই strv_extend(m, "");
}

পূর্ণাঙ্ক load_env_file_pairs(FILE *f, ধ্রুৱ ৱর্ণ *fname, ৱর্ণ ***ret) {
        _cleanup_strv_free_ ৱর্ণ **m = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f || fname);
        প্রতিজ্ঞা(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ঠেলো_pairs, &m);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = TAKE_PTR(m);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক load_env_file_pairs_fd(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *fname, ৱর্ণ ***ret) {
        _cleanup_fclose_ FILE *f = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই load_env_file_pairs(f, fname, ret);
}

স্থির পূর্ণাঙ্ক merge_env_file_ঠেলো(
                ধ্রুৱ ৱর্ণ *filename, unsigned সাৰি,
                ধ্রুৱ ৱর্ণ *চাবি, ৱর্ণ *মান,
                শূন্য *userdata) {

        ৱর্ণ ***env = ASSERT_PTR(userdata);
        ৱর্ণ *expanded_মান;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(চাবি);

        যদি (!মান) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), সাৰি, চাবি);
                ঘূৰাই 0;
        }

        যদি (!env_name_is_valid(চাবি)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), সাৰি, চাবি);
                মুক্ত(মান);
                ঘূৰাই 0;
        }

        r = replace_env(মান,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_মান);
        যদি (r < 0)
                ঘূৰাই log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), সাৰি, মান);

        free_and_replace(মান, expanded_মান);

        log_debug("%s:%u: setting %s=%s", filename, সাৰি, চাবি, মান);

        ঘূৰাই load_env_file_ঠেলো(filename, সাৰি, চাবি, মান, env);
}

পূর্ণাঙ্ক merge_env_file(
                ৱর্ণ ***env,
                FILE *f,
                ধ্রুৱ ৱর্ণ *fname) {

        প্রতিজ্ঞা(env);
        প্রতিজ্ঞা(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ঘূৰাই parse_env_file_internal(f, fname, merge_env_file_ঠেলো, env);
}

স্থির শূন্য env_file_fputs_escaped(FILE *f, ধ্রুৱ ৱর্ণ *p) {
        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(p);

        flockfile(f);

        যদি (string_has_cc(p, রিক্ত) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                বাবে (; *p; p++) {
                        যদি (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } অন্যথা
                fputs_unlocked(p, f);

        funlockfile(f);
}

শূন্য env_file_fputs_assignment(FILE *f, ধ্রুৱ ৱর্ণ *k, ধ্রুৱ ৱর্ণ *v) {
        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(k);

        যদি (!v)
                ঘূৰাই;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

স্থির শূন্য write_env_var(FILE *f, ধ্রুৱ ৱর্ণ *v) {
        ধ্রুৱ ৱর্ণ *p;

        প্রতিজ্ঞা(f);
        প্রতিজ্ঞা(v);

        p = strchr(v, '=');
        যদি (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ঘূৰাই;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

পূর্ণাঙ্ক write_env_file_label(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *fname, ৱর্ণ **headers, ৱর্ণ **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(fname);

        যদি (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        _cleanup_fclose_ FILE *f = রিক্ত;
        _cleanup_free_ ৱর্ণ *p = রিক্ত;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        যদি (r < 0) {
                যদি (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (শূন্য) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ঘূৰাই r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        যদি (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ রিক্ত, /* created= */ true, label_context);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        r = fchmod_umask(fileno(f), 0644);
        যদি (r < 0)
                ঘূৰাই r;

        STRV_FOREACH(i, headers) {
                প্রতিজ্ঞা(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        যদি (r < 0)
                ঘূৰাই r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ঘূৰাই 0;
}

পূর্ণাঙ্ক write_vconsole_conf(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *fname, ৱর্ণ **l) {
        ৱর্ণ **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ঘূৰাই write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ রিক্ত);
}
