/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "env-file.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "label-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "tmpfile-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*ತಳ್ಳು_env_func_t)(
                ಧ್ರುವ ವರ್ಣ *filename,
                unsigned ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ,
                ವರ್ಣ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_env_file_internal(
                FILE *f,
                ಧ್ರುವ ವರ್ಣ *fname,
                ತಳ್ಳು_env_func_t ತಳ್ಳು,
                ಶೂನ್ಯ *userdata) {

        size_t n_ಕೀಲಿ = 0, n_ಮೌಲ್ಯ = 0, last_ಮೌಲ್ಯ_whitespace = SIZE_MAX, last_ಕೀಲಿ_whitespace = SIZE_MAX;
        _cleanup_free_ ವರ್ಣ *contents = ರಿಕ್ತ, *ಕೀಲಿ = ರಿಕ್ತ, *ಮೌಲ್ಯ = ರಿಕ್ತ;
        unsigned ಸಾಲು = 1;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಗಣನಾ {
                PRE_ಕೀಲಿ,
                ಕೀಲಿ_2,
                PRE_ಮೌಲ್ಯ,
                ಮೌಲ್ಯ_2,
                ಮೌಲ್ಯ_ESCAPE,
                SINGLE_QUOTE_ಮೌಲ್ಯ,
                DOUBLE_QUOTE_ಮೌಲ್ಯ,
                DOUBLE_QUOTE_ಮೌಲ್ಯ_ESCAPE,
                COMMENT,
                COMMENT_ESCAPE
        } state = PRE_ಕೀಲಿ;

        ಪ್ರತಿಜ್ಞಾ(f || fname);
        ಪ್ರತಿಜ್ಞಾ(ತಳ್ಳು);

        ಯದಿ (f)
                r = read_full_stream(f, &contents, ರಿಕ್ತ);
        ಅನ್ಯಥಾ
                r = read_full_file(fname, &contents, ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (ವರ್ಣ *p = contents; *p; p++) {
                ವರ್ಣ c = *p;

                ವಿಕಲ್ಪ (state) {

                ಪಕ್ಷ PRE_ಕೀಲಿ:
                        ಯದಿ (strchr(COMMENTS, c))
                                state = COMMENT;
                        ಅನ್ಯಥಾ ಯದಿ (!strchr(WHITESPACE, c)) {
                                state = ಕೀಲಿ_2;
                                last_ಕೀಲಿ_whitespace = SIZE_MAX;

                                ಯದಿ (!GREEDY_REALLOC(ಕೀಲಿ, n_ಕೀಲಿ+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                ಕೀಲಿ[n_ಕೀಲಿ++] = c;
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ ಕೀಲಿ_2:
                        ಯದಿ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;
                                n_ಕೀಲಿ = 0;
                        } ಅನ್ಯಥಾ ಯದಿ (c == '=') {
                                state = PRE_ಮೌಲ್ಯ;
                                last_ಮೌಲ್ಯ_whitespace = SIZE_MAX;
                        } ಅನ್ಯಥಾ {
                                ಯದಿ (!strchr(WHITESPACE, c))
                                        last_ಕೀಲಿ_whitespace = SIZE_MAX;
                                ಅನ್ಯಥಾ ಯದಿ (last_ಕೀಲಿ_whitespace == SIZE_MAX)
                                         last_ಕೀಲಿ_whitespace = n_ಕೀಲಿ;

                                ಯದಿ (!GREEDY_REALLOC(ಕೀಲಿ, n_ಕೀಲಿ+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                ಕೀಲಿ[n_ಕೀಲಿ++] = c;
                        }

                        ಭಙ್ಗ;

                ಪಕ್ಷ PRE_ಮೌಲ್ಯ:
                        ಯದಿ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;
                                ಕೀಲಿ[n_ಕೀಲಿ] = 0;

                                ಯದಿ (ಮೌಲ್ಯ)
                                        ಮೌಲ್ಯ[n_ಮೌಲ್ಯ] = 0;

                                /* strip trailing whitespace from key */
                                ಯದಿ (last_ಕೀಲಿ_whitespace != SIZE_MAX)
                                        ಕೀಲಿ[last_ಕೀಲಿ_whitespace] = 0;

                                r = ತಳ್ಳು(fname, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ, userdata);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                n_ಕೀಲಿ = 0;
                                ಮೌಲ್ಯ = ರಿಕ್ತ;
                                n_ಮೌಲ್ಯ = 0;

                        } ಅನ್ಯಥಾ ಯದಿ (c == '\'')
                                state = SINGLE_QUOTE_ಮೌಲ್ಯ;
                        ಅನ್ಯಥಾ ಯದಿ (c == '"')
                                state = DOUBLE_QUOTE_ಮೌಲ್ಯ;
                        ಅನ್ಯಥಾ ಯದಿ (c == '\\')
                                state = ಮೌಲ್ಯ_ESCAPE;
                        ಅನ್ಯಥಾ ಯದಿ (!strchr(WHITESPACE, c)) {
                                state = ಮೌಲ್ಯ_2;

                                ಯದಿ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        ಭಙ್ಗ;

                ಪಕ್ಷ ಮೌಲ್ಯ_2:
                        ಯದಿ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;

                                ಕೀಲಿ[n_ಕೀಲಿ] = 0;

                                ಯದಿ (ಮೌಲ್ಯ)
                                        ಮೌಲ್ಯ[n_ಮೌಲ್ಯ] = 0;

                                /* Chomp off trailing whitespace from value */
                                ಯದಿ (last_ಮೌಲ್ಯ_whitespace != SIZE_MAX)
                                        ಮೌಲ್ಯ[last_ಮೌಲ್ಯ_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ಯದಿ (last_ಕೀಲಿ_whitespace != SIZE_MAX)
                                        ಕೀಲಿ[last_ಕೀಲಿ_whitespace] = 0;

                                r = ತಳ್ಳು(fname, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ, userdata);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                n_ಕೀಲಿ = 0;
                                ಮೌಲ್ಯ = ರಿಕ್ತ;
                                n_ಮೌಲ್ಯ = 0;

                        } ಅನ್ಯಥಾ ಯದಿ (c == '\\') {
                                state = ಮೌಲ್ಯ_ESCAPE;
                                last_ಮೌಲ್ಯ_whitespace = SIZE_MAX;
                        } ಅನ್ಯಥಾ {
                                ಯದಿ (!strchr(WHITESPACE, c))
                                        last_ಮೌಲ್ಯ_whitespace = SIZE_MAX;
                                ಅನ್ಯಥಾ ಯದಿ (last_ಮೌಲ್ಯ_whitespace == SIZE_MAX)
                                        last_ಮೌಲ್ಯ_whitespace = n_ಮೌಲ್ಯ;

                                ಯದಿ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        ಭಙ್ಗ;

                ಪಕ್ಷ ಮೌಲ್ಯ_ESCAPE:
                        state = ಮೌಲ್ಯ_2;

                        ಯದಿ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ಯದಿ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ SINGLE_QUOTE_ಮೌಲ್ಯ:
                        ಯದಿ (c == '\'')
                                state = PRE_ಮೌಲ್ಯ;
                        ಅನ್ಯಥಾ {
                                ಯದಿ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        ಭಙ್ಗ;

                ಪಕ್ಷ DOUBLE_QUOTE_ಮೌಲ್ಯ:
                        ಯದಿ (c == '"')
                                state = PRE_ಮೌಲ್ಯ;
                        ಅನ್ಯಥಾ ಯದಿ (c == '\\')
                                state = DOUBLE_QUOTE_ಮೌಲ್ಯ_ESCAPE;
                        ಅನ್ಯಥಾ {
                                ಯದಿ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        ಭಙ್ಗ;

                ಪಕ್ಷ DOUBLE_QUOTE_ಮೌಲ್ಯ_ESCAPE:
                        state = DOUBLE_QUOTE_ಮೌಲ್ಯ;

                        ಯದಿ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ಯದಿ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        } ಅನ್ಯಥಾ ಯದಿ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ಯದಿ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+3))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = '\\';
                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        ಭಙ್ಗ;

                ಪಕ್ಷ COMMENT:
                        ಯದಿ (c == '\\')
                                state = COMMENT_ESCAPE;
                        ಅನ್ಯಥಾ ಯದಿ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ಯದಿ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;
                        } ಅನ್ಯಥಾ
                                state = COMMENT;
                        ಭಙ್ಗ;
                }
        }

        ಯದಿ (IN_SET(state,
                   PRE_ಮೌಲ್ಯ,
                   ಮೌಲ್ಯ_2,
                   ಮೌಲ್ಯ_ESCAPE,
                   SINGLE_QUOTE_ಮೌಲ್ಯ,
                   DOUBLE_QUOTE_ಮೌಲ್ಯ,
                   DOUBLE_QUOTE_ಮೌಲ್ಯ_ESCAPE)) {

                ಕೀಲಿ[n_ಕೀಲಿ] = 0;

                ಯದಿ (ಮೌಲ್ಯ)
                        ಮೌಲ್ಯ[n_ಮೌಲ್ಯ] = 0;

                ಯದಿ (state == ಮೌಲ್ಯ_2)
                        ಯದಿ (last_ಮೌಲ್ಯ_whitespace != SIZE_MAX)
                                ಮೌಲ್ಯ[last_ಮೌಲ್ಯ_whitespace] = 0;

                /* strip trailing whitespace from key */
                ಯದಿ (last_ಕೀಲಿ_whitespace != SIZE_MAX)
                        ಕೀಲಿ[last_ಕೀಲಿ_whitespace] = 0;

                r = ತಳ್ಳು(fname, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ, userdata);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಮೌಲ್ಯ = ರಿಕ್ತ;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_utf8ness_and_warn(
                ಧ್ರುವ ವರ್ಣ *filename, unsigned ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ವರ್ಣ *ಮೌಲ್ಯ) {

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಯದಿ (!utf8_is_valid(ಕೀಲಿ)) {
                _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;

                p = utf8_escape_invalid(ಕೀಲಿ);
                ಪ್ರತ್ಯಾಗಮ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), ಸಾಲು, p);
        }

        ಯದಿ (ಮೌಲ್ಯ && !utf8_is_valid(ಮೌಲ್ಯ)) {
                _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;

                p = utf8_escape_invalid(ಮೌಲ್ಯ);
                ಪ್ರತ್ಯಾಗಮ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), ಸಾಲು, ಕೀಲಿ, p);
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_env_file_ತಳ್ಳು(
                ಧ್ರುವ ವರ್ಣ *filename, unsigned ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ವರ್ಣ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata) {

        ಧ್ರುವ ವರ್ಣ *k;
        va_list aq, *ap = userdata;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        r = check_utf8ness_and_warn(filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        va_copy(aq, *ap);

        ಯಾವತ್ ((k = va_arg(aq, ಧ್ರುವ ವರ್ಣ *))) {
                ವರ್ಣ **v;

                v = va_arg(aq, ವರ್ಣ **);

                ಯದಿ (streq(ಕೀಲಿ, k)) {
                        va_end(aq);
                        free_and_replace(*v, ಮೌಲ್ಯ);

                        ಪ್ರತ್ಯಾಗಮ 1;
                }
        }

        va_end(aq);
        ಮುಕ್ತ(ಮೌಲ್ಯ);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_env_filev(
                FILE *f,
                ಧ್ರುವ ವರ್ಣ *fname,
                va_list ap) {

        ಪೂರ್ಣಾಙ್ಕ r;
        va_list aq;

        ಪ್ರತಿಜ್ಞಾ(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ತಳ್ಳು, &aq);
        va_end(aq);
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ parse_env_file_fdv(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        va_list aq;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        va_copy(aq, ap);
        r = parse_env_file_internal(f, fname, parse_env_file_ತಳ್ಳು, &aq);
        va_end(aq);
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ parse_env_file_sentinel(
                FILE *f,
                ಧ್ರುವ ವರ್ಣ *fname,
                ...) {

        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ parse_env_file_fd_sentinel(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *fname, /* only used for logging */
                ...) {

        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ parse_env_datav(
                ಧ್ರುವ ವರ್ಣ *data,
                size_t size,
                ಧ್ರುವ ವರ್ಣ *fname, /* only used for logging */
                va_list ap) {

        ಪ್ರತಿಜ್ಞಾ(data);

        ಯದಿ (size == SIZE_MAX)
                size = strlen_ptr(data);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((ಶೂನ್ಯ*) data, size, "r");
        ಯದಿ (!f)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ parse_env_filev(f, fname, ap);
}

ಪೂರ್ಣಾಙ್ಕ parse_env_data_sentinel(
                ಧ್ರುವ ವರ್ಣ *data,
                size_t size,
                ಧ್ರುವ ವರ್ಣ *fname, /* only used for logging */
                ...) {

        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, fname);
        r = parse_env_datav(data, size, fname, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ load_env_file_ತಳ್ಳು(
                ಧ್ರುವ ವರ್ಣ *filename, unsigned ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ವರ್ಣ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata) {

        ವರ್ಣ ***m = userdata;
        ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        r = check_utf8ness_and_warn(filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        p = strjoin(ಕೀಲಿ, "=", ಮೌಲ್ಯ);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಮುಕ್ತ(ಮೌಲ್ಯ);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ load_env_file(FILE *f, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **m = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f || fname);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ತಳ್ಳು, &m);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(m);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ load_env_file_ತಳ್ಳು_pairs(
                ಧ್ರುವ ವರ್ಣ *filename, unsigned ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ವರ್ಣ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata) {

        ವರ್ಣ ***m = ASSERT_PTR(userdata);
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        r = check_utf8ness_and_warn(filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Check if the key is present */
        ಪ್ರತಿ (ವರ್ಣ **t = *m; t && *t; t += 2)
                ಯದಿ (streq(t[0], ಕೀಲಿ)) {
                        ಯದಿ (ಮೌಲ್ಯ)
                                ಪ್ರತ್ಯಾಗಮ free_and_replace(t[1], ಮೌಲ್ಯ);
                        ಅನ್ಯಥಾ
                                ಪ್ರತ್ಯಾಗಮ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, ಕೀಲಿ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ strv_ತಳ್ಳು(m, ಮೌಲ್ಯ);
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ strv_extend(m, "");
}

ಪೂರ್ಣಾಙ್ಕ load_env_file_pairs(FILE *f, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **m = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f || fname);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_env_file_internal(f, fname, load_env_file_ತಳ್ಳು_pairs, &m);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(m);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ load_env_file_pairs_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ ***ret) {
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ load_env_file_pairs(f, fname, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ merge_env_file_ತಳ್ಳು(
                ಧ್ರುವ ವರ್ಣ *filename, unsigned ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ವರ್ಣ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata) {

        ವರ್ಣ ***env = ASSERT_PTR(userdata);
        ವರ್ಣ *expanded_ಮೌಲ್ಯ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಯದಿ (!ಮೌಲ್ಯ) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), ಸಾಲು, ಕೀಲಿ);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (!env_name_is_valid(ಕೀಲಿ)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), ಸಾಲು, ಕೀಲಿ);
                ಮುಕ್ತ(ಮೌಲ್ಯ);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = replace_env(ಮೌಲ್ಯ,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), ಸಾಲು, ಮೌಲ್ಯ);

        free_and_replace(ಮೌಲ್ಯ, expanded_ಮೌಲ್ಯ);

        log_debug("%s:%u: setting %s=%s", filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ);

        ಪ್ರತ್ಯಾಗಮ load_env_file_ತಳ್ಳು(filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ, env);
}

ಪೂರ್ಣಾಙ್ಕ merge_env_file(
                ವರ್ಣ ***env,
                FILE *f,
                ಧ್ರುವ ವರ್ಣ *fname) {

        ಪ್ರತಿಜ್ಞಾ(env);
        ಪ್ರತಿಜ್ಞಾ(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ಪ್ರತ್ಯಾಗಮ parse_env_file_internal(f, fname, merge_env_file_ತಳ್ಳು, env);
}

ಸ್ಥಿರ ಶೂನ್ಯ env_file_fputs_escaped(FILE *f, ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(p);

        flockfile(f);

        ಯದಿ (string_has_cc(p, ರಿಕ್ತ) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                ಪ್ರತಿ (; *p; p++) {
                        ಯದಿ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } ಅನ್ಯಥಾ
                fputs_unlocked(p, f);

        funlockfile(f);
}

ಶೂನ್ಯ env_file_fputs_assignment(FILE *f, ಧ್ರುವ ವರ್ಣ *k, ಧ್ರುವ ವರ್ಣ *v) {
        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(k);

        ಯದಿ (!v)
                ಪ್ರತ್ಯಾಗಮ;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

ಸ್ಥಿರ ಶೂನ್ಯ write_env_var(FILE *f, ಧ್ರುವ ವರ್ಣ *v) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(v);

        p = strchr(v, '=');
        ಯದಿ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ಪ್ರತ್ಯಾಗಮ;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

ಪೂರ್ಣಾಙ್ಕ write_env_file_label(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ **headers, ವರ್ಣ **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(fname);

        ಯದಿ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ಯದಿ (r < 0) {
                ಯದಿ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL))
                        (ಶೂನ್ಯ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ಪ್ರತ್ಯಾಗಮ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ಯದಿ (FLAGS_SET(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ ರಿಕ್ತ, /* created= */ true, label_context);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        STRV_FOREACH(i, headers) {
                ಪ್ರತಿಜ್ಞಾ(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ write_vconsole_conf(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fname, ವರ್ಣ **l) {
        ವರ್ಣ **headers = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ಪ್ರತ್ಯಾಗಮ write_env_file_label(dir_fd, fname, headers, l, WRITE_ENV_FILE_LABEL, /* label_context= */ ರಿಕ್ತ);
}
