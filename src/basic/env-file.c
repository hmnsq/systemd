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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*ತಳ್ಳು_env_func_t)(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                ಚಿಹ್ನೆರಹಿತ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                ಅಕ್ಷರ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_env_file_ಆಂತರಿಕ(
                FILE *f,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname,
                ತಳ್ಳು_env_func_t ತಳ್ಳು,
                ಶೂನ್ಯ *userdata) {

        size_t n_ಕೀಲಿ = 0, n_ಮೌಲ್ಯ = 0, ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace = SIZE_MAX, ಕೊನೆಯ_ಕೀಲಿ_whitespace = SIZE_MAX;
        _cleanup_free_ ಅಕ್ಷರ *contents = ಖಾಲಿ, *ಕೀಲಿ = ಖಾಲಿ, *ಮೌಲ್ಯ = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ ಸಾಲು = 1;
        ಪೂರ್ಣಾಂಕ r;

        ಎಣಿಕೆ {
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

        ಆದರೆ (f)
                r = read_ತುಂಬಿದ_ಹರಿವು(f, &contents, ಖಾಲಿ);
        ಇಲ್ಲದಿದ್ದರೆ
                r = read_ತುಂಬಿದ_file(fname, &contents, ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (ಅಕ್ಷರ *p = contents; *p; p++) {
                ಅಕ್ಷರ c = *p;

                ಆಯ್ಕೆ (state) {

                ಸ್ಥಿತಿ PRE_ಕೀಲಿ:
                        ಆದರೆ (strchr(COMMENTS, c))
                                state = COMMENT;
                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strchr(WHITESPACE, c)) {
                                state = ಕೀಲಿ_2;
                                ಕೊನೆಯ_ಕೀಲಿ_whitespace = SIZE_MAX;

                                ಆದರೆ (!GREEDY_REALLOC(ಕೀಲಿ, n_ಕೀಲಿ+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಕೀಲಿ[n_ಕೀಲಿ++] = c;
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ ಕೀಲಿ_2:
                        ಆದರೆ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;
                                n_ಕೀಲಿ = 0;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '=') {
                                state = PRE_ಮೌಲ್ಯ;
                                ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace = SIZE_MAX;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ (!strchr(WHITESPACE, c))
                                        ಕೊನೆಯ_ಕೀಲಿ_whitespace = SIZE_MAX;
                                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೊನೆಯ_ಕೀಲಿ_whitespace == SIZE_MAX)
                                         ಕೊನೆಯ_ಕೀಲಿ_whitespace = n_ಕೀಲಿ;

                                ಆದರೆ (!GREEDY_REALLOC(ಕೀಲಿ, n_ಕೀಲಿ+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಕೀಲಿ[n_ಕೀಲಿ++] = c;
                        }

                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ PRE_ಮೌಲ್ಯ:
                        ಆದರೆ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;
                                ಕೀಲಿ[n_ಕೀಲಿ] = 0;

                                ಆದರೆ (ಮೌಲ್ಯ)
                                        ಮೌಲ್ಯ[n_ಮೌಲ್ಯ] = 0;

                                /* strip trailing whitespace from key */
                                ಆದರೆ (ಕೊನೆಯ_ಕೀಲಿ_whitespace != SIZE_MAX)
                                        ಕೀಲಿ[ಕೊನೆಯ_ಕೀಲಿ_whitespace] = 0;

                                r = ತಳ್ಳು(fname, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ, userdata);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                n_ಕೀಲಿ = 0;
                                ಮೌಲ್ಯ = ಖಾಲಿ;
                                n_ಮೌಲ್ಯ = 0;

                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\'')
                                state = SINGLE_QUOTE_ಮೌಲ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '"')
                                state = DOUBLE_QUOTE_ಮೌಲ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\\')
                                state = ಮೌಲ್ಯ_ESCAPE;
                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!strchr(WHITESPACE, c)) {
                                state = ಮೌಲ್ಯ_2;

                                ಆದರೆ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ ಮೌಲ್ಯ_2:
                        ಆದರೆ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;

                                ಕೀಲಿ[n_ಕೀಲಿ] = 0;

                                ಆದರೆ (ಮೌಲ್ಯ)
                                        ಮೌಲ್ಯ[n_ಮೌಲ್ಯ] = 0;

                                /* Chomp off trailing whitespace from value */
                                ಆದರೆ (ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace != SIZE_MAX)
                                        ಮೌಲ್ಯ[ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace] = 0;

                                /* strip trailing whitespace from key */
                                ಆದರೆ (ಕೊನೆಯ_ಕೀಲಿ_whitespace != SIZE_MAX)
                                        ಕೀಲಿ[ಕೊನೆಯ_ಕೀಲಿ_whitespace] = 0;

                                r = ತಳ್ಳು(fname, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ, userdata);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                n_ಕೀಲಿ = 0;
                                ಮೌಲ್ಯ = ಖಾಲಿ;
                                n_ಮೌಲ್ಯ = 0;

                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\\') {
                                state = ಮೌಲ್ಯ_ESCAPE;
                                ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace = SIZE_MAX;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ (!strchr(WHITESPACE, c))
                                        ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace = SIZE_MAX;
                                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace == SIZE_MAX)
                                        ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace = n_ಮೌಲ್ಯ;

                                ಆದರೆ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ ಮೌಲ್ಯ_ESCAPE:
                        state = ಮೌಲ್ಯ_2;

                        ಆದರೆ (!strchr(NEWLINE, c)) {
                                /* Escaped newlines we eat up entirely */
                                ಆದರೆ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ SINGLE_QUOTE_ಮೌಲ್ಯ:
                        ಆದರೆ (c == '\'')
                                state = PRE_ಮೌಲ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DOUBLE_QUOTE_ಮೌಲ್ಯ:
                        ಆದರೆ (c == '"')
                                state = PRE_ಮೌಲ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\\')
                                state = DOUBLE_QUOTE_ಮೌಲ್ಯ_ESCAPE;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಆದರೆ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DOUBLE_QUOTE_ಮೌಲ್ಯ_ESCAPE:
                        state = DOUBLE_QUOTE_ಮೌಲ್ಯ;

                        ಆದರೆ (strchr(SHELL_NEED_ESCAPE, c)) {
                                /* If this is a char that needs escaping, just unescape it. */
                                ಆದರೆ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;
                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c != '\n') {
                                /* If other char than what needs escaping, keep the "\" in place, like the
                                 * real shell does. */
                                ಆದರೆ (!GREEDY_REALLOC(ಮೌಲ್ಯ, n_ಮೌಲ್ಯ+3))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;
                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = '\\';
                                ಮೌಲ್ಯ[n_ಮೌಲ್ಯ++] = c;
                        }

                        /* Escaped newlines (aka "continuation lines") are eaten up entirely */
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ COMMENT:
                        ಆದರೆ (c == '\\')
                                state = COMMENT_ESCAPE;
                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ COMMENT_ESCAPE:
                        log_debug("The line which doesn't begin with \";\" or \"#\", but follows a comment" \
                                  " line trailing with escape is now treated as a non comment line since v254.");
                        ಆದರೆ (strchr(NEWLINE, c)) {
                                state = PRE_ಕೀಲಿ;
                                ಸಾಲು++;
                        } ಇಲ್ಲದಿದ್ದರೆ
                                state = COMMENT;
                        ನಿಲ್ಲಿಸು;
                }
        }

        ಆದರೆ (ಒಳ_ಗಣ(state,
                   PRE_ಮೌಲ್ಯ,
                   ಮೌಲ್ಯ_2,
                   ಮೌಲ್ಯ_ESCAPE,
                   SINGLE_QUOTE_ಮೌಲ್ಯ,
                   DOUBLE_QUOTE_ಮೌಲ್ಯ,
                   DOUBLE_QUOTE_ಮೌಲ್ಯ_ESCAPE)) {

                ಕೀಲಿ[n_ಕೀಲಿ] = 0;

                ಆದರೆ (ಮೌಲ್ಯ)
                        ಮೌಲ್ಯ[n_ಮೌಲ್ಯ] = 0;

                ಆದರೆ (state == ಮೌಲ್ಯ_2)
                        ಆದರೆ (ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace != SIZE_MAX)
                                ಮೌಲ್ಯ[ಕೊನೆಯ_ಮೌಲ್ಯ_whitespace] = 0;

                /* strip trailing whitespace from key */
                ಆದರೆ (ಕೊನೆಯ_ಕೀಲಿ_whitespace != SIZE_MAX)
                        ಕೀಲಿ[ಕೊನೆಯ_ಕೀಲಿ_whitespace] = 0;

                r = ತಳ್ಳು(fname, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ, userdata);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಮೌಲ್ಯ = ಖಾಲಿ;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_utf8ness_and_warn(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಚಿಹ್ನೆರಹಿತ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಅಕ್ಷರ *ಮೌಲ್ಯ) {

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಆದರೆ (!utf8_is_valid(ಕೀಲಿ)) {
                _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;

                p = utf8_escape_invalid(ಕೀಲಿ);
                ಹಿಂತಿರುಗಿ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 in key '%s', ignoring.",
                                       strna(filename), ಸಾಲು, p);
        }

        ಆದರೆ (ಮೌಲ್ಯ && !utf8_is_valid(ಮೌಲ್ಯ)) {
                _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;

                p = utf8_escape_invalid(ಮೌಲ್ಯ);
                ಹಿಂತಿರುಗಿ log_error_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "%s:%u: invalid UTF-8 value for key %s: '%s', ignoring.",
                                       strna(filename), ಸಾಲು, ಕೀಲಿ, p);
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_env_file_ತಳ್ಳು(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಚಿಹ್ನೆರಹಿತ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಅಕ್ಷರ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata) {

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k;
        va_list aq, *ap = userdata;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        r = check_utf8ness_and_warn(filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        va_copy(aq, *ap);

        ಆಗ ((k = va_arg(aq, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *))) {
                ಅಕ್ಷರ **v;

                v = va_arg(aq, ಅಕ್ಷರ **);

                ಆದರೆ (streq(ಕೀಲಿ, k)) {
                        va_end(aq);
                        free_and_replace(*v, ಮೌಲ್ಯ);

                        ಹಿಂತಿರುಗಿ 1;
                }
        }

        va_end(aq);
        ಬಿಡುಗಡೆ(ಮೌಲ್ಯ);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_env_filev(
                FILE *f,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname,
                va_list ap) {

        ಪೂರ್ಣಾಂಕ r;
        va_list aq;

        ಪ್ರತಿಜ್ಞಾ(f || fname);

        va_copy(aq, ap);
        r = parse_env_file_ಆಂತರಿಕ(f, fname, parse_env_file_ತಳ್ಳು, &aq);
        va_end(aq);
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ parse_env_file_fdv(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, va_list ap) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        va_list aq;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        va_copy(aq, ap);
        r = parse_env_file_ಆಂತರಿಕ(f, fname, parse_env_file_ತಳ್ಳು, &aq);
        va_end(aq);
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ parse_env_file_sentinel(
                FILE *f,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname,
                ...) {

        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(f || fname);

        va_start(ap, fname);
        r = parse_env_filev(f, fname, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ parse_env_file_fd_sentinel(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, /* only used for logging */
                ...) {

        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        va_start(ap, fname);
        r = parse_env_file_fdv(fd, fname, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ parse_env_datav(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                size_t ಗಾತ್ರ_2,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, /* only used for logging */
                va_list ap) {

        ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ);

        ಆದರೆ (ಗಾತ್ರ_2 == SIZE_MAX)
                ಗಾತ್ರ_2 = strlen_ptr(ದತ್ತಾಂಶ);

        _cleanup_fclose_ FILE *f = fmemopen_unlocked((ಶೂನ್ಯ*) ದತ್ತಾಂಶ, ಗಾತ್ರ_2, "r");
        ಆದರೆ (!f)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ parse_env_filev(f, fname, ap);
}

ಪೂರ್ಣಾಂಕ parse_env_ದತ್ತಾಂಶ_sentinel(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ,
                size_t ಗಾತ್ರ_2,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, /* only used for logging */
                ...) {

        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, fname);
        r = parse_env_datav(ದತ್ತಾಂಶ, ಗಾತ್ರ_2, fname, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತುಂಬು_env_file_ತಳ್ಳು(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಚಿಹ್ನೆರಹಿತ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಅಕ್ಷರ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata) {

        ಅಕ್ಷರ ***m = userdata;
        ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        r = check_utf8ness_and_warn(filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        p = strjoin(ಕೀಲಿ, "=", ಮೌಲ್ಯ);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        r = strv_env_replace_consume(m, p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಬಿಡುಗಡೆ(ಮೌಲ್ಯ);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ತುಂಬು_env_file(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **m = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(f || fname);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_env_file_ಆಂತರಿಕ(f, fname, ತುಂಬು_env_file_ತಳ್ಳು, &m);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(m);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ತುಂಬು_env_file_ತಳ್ಳು_ಜೋಡಿಗಳು(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಚಿಹ್ನೆರಹಿತ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಅಕ್ಷರ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata) {

        ಅಕ್ಷರ ***m = ASSERT_PTR(userdata);
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        r = check_utf8ness_and_warn(filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Check if the key is present */
        ಗಾಗಿ (ಅಕ್ಷರ **t = *m; t && *t; t += 2)
                ಆದರೆ (streq(t[0], ಕೀಲಿ)) {
                        ಆದರೆ (ಮೌಲ್ಯ)
                                ಹಿಂತಿರುಗಿ free_and_replace(t[1], ಮೌಲ್ಯ);
                        ಇಲ್ಲದಿದ್ದರೆ
                                ಹಿಂತಿರುಗಿ free_and_strdup(t+1, "");
                }

        r = strv_extend(m, ಕೀಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ strv_ತಳ್ಳು(m, ಮೌಲ್ಯ);
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ strv_extend(m, "");
}

ಪೂರ್ಣಾಂಕ ತುಂಬು_env_file_ಜೋಡಿಗಳು(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **m = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(f || fname);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_env_file_ಆಂತರಿಕ(f, fname, ತುಂಬು_env_file_ತಳ್ಳು_ಜೋಡಿಗಳು, &m);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(m);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ತುಂಬು_env_file_ಜೋಡಿಗಳು_fd(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ ***ret) {
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = fdopen_independent(fd, "re", &f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ತುಂಬು_env_file_ಜೋಡಿಗಳು(f, fname, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ವಿಲೀನ_env_file_ತಳ್ಳು(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಚಿಹ್ನೆರಹಿತ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಅಕ್ಷರ *ಮೌಲ್ಯ,
                ಶೂನ್ಯ *userdata) {

        ಅಕ್ಷರ ***env = ASSERT_PTR(userdata);
        ಅಕ್ಷರ *expanded_ಮೌಲ್ಯ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        ಆದರೆ (!ಮೌಲ್ಯ) {
                log_error("%s:%u: invalid syntax (around \"%s\"), ignoring.", strna(filename), ಸಾಲು, ಕೀಲಿ);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (!env_name_is_valid(ಕೀಲಿ)) {
                log_error("%s:%u: invalid variable name \"%s\", ignoring.", strna(filename), ಸಾಲು, ಕೀಲಿ);
                ಬಿಡುಗಡೆ(ಮೌಲ್ಯ);
                ಹಿಂತಿರುಗಿ 0;
        }

        r = replace_env(ಮೌಲ್ಯ,
                        *env,
                        REPLACE_ENV_USE_ENVIRONMENT|REPLACE_ENV_ALLOW_BRACELESS|REPLACE_ENV_ALLOW_EXTENDED,
                        &expanded_ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "%s:%u: Failed to expand variable '%s': %m", strna(filename), ಸಾಲು, ಮೌಲ್ಯ);

        free_and_replace(ಮೌಲ್ಯ, expanded_ಮೌಲ್ಯ);

        log_debug("%s:%u: setting %s=%s", filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ);

        ಹಿಂತಿರುಗಿ ತುಂಬು_env_file_ತಳ್ಳು(filename, ಸಾಲು, ಕೀಲಿ, ಮೌಲ್ಯ, env);
}

ಪೂರ್ಣಾಂಕ ವಿಲೀನ_env_file(
                ಅಕ್ಷರ ***env,
                FILE *f,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname) {

        ಪ್ರತಿಜ್ಞಾ(env);
        ಪ್ರತಿಜ್ಞಾ(f || fname);

        /* NOTE: this function supports braceful and braceless variable expansions,
         * plus "extended" substitutions, unlike other exported parsing functions.
         */

        ಹಿಂತಿರುಗಿ parse_env_file_ಆಂತರಿಕ(f, fname, ವಿಲೀನ_env_file_ತಳ್ಳು, env);
}

ಸ್ಥಿರ ಶೂನ್ಯ env_file_fputs_escaped(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(p);

        flockfile(f);

        ಆದರೆ (string_has_cc(p, ಖಾಲಿ) || chars_intersect(p, WHITESPACE SHELL_NEED_QUOTES)) {
                fputc_unlocked('"', f);

                ಗಾಗಿ (; *p; p++) {
                        ಆದರೆ (strchr(SHELL_NEED_ESCAPE, *p))
                                fputc_unlocked('\\', f);

                        fputc_unlocked(*p, f);
                }

                fputc_unlocked('"', f);
        } ಇಲ್ಲದಿದ್ದರೆ
                fputs_unlocked(p, f);

        funlockfile(f);
}

ಶೂನ್ಯ env_file_fputs_assignment(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v) {
        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(k);

        ಆದರೆ (!v)
                ಹಿಂತಿರುಗಿ;

        fputs(k, f);
        env_file_fputs_escaped(f, v);
        fputc('\n', f);
}

ಸ್ಥಿರ ಶೂನ್ಯ write_env_var(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(f);
        ಪ್ರತಿಜ್ಞಾ(v);

        p = strchr(v, '=');
        ಆದರೆ (!p) {
                /* Fallback */
                fputs_unlocked(v, f);
                fputc_unlocked('\n', f);
                ಹಿಂತಿರುಗಿ;
        }

        p++;
        fwrite_unlocked(v, 1, p-v, f);

        env_file_fputs_escaped(f, p);

        fputc_unlocked('\n', f);
}

ಪೂರ್ಣಾಂಕ write_env_file_label(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ **ಶೀರ್ಷಿಕೆಗಳು, ಅಕ್ಷರ **l, WriteEnvFileFlags flags, LabelContext *label_context) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(fname);

        ಆದರೆ (FLAGS_ಗಣ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_pre(dir_fd, fname, S_IFREG, label_context);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        r = fopen_tmpfile_linkable_at(dir_fd, fname, O_WRONLY|O_CLOEXEC, &p, &f);
        ಆದರೆ (r < 0) {
                ಆದರೆ (FLAGS_ಗಣ(flags, WRITE_ENV_FILE_LABEL))
                        (ಶೂನ್ಯ) label_ops_post(dir_fd, fname, /* created= */ false, label_context);
                ಹಿಂತಿರುಗಿ r;
        }
        CLEANUP_TMPFILE_AT(dir_fd, p);

        ಆದರೆ (FLAGS_ಗಣ(flags, WRITE_ENV_FILE_LABEL)) {
                r = label_ops_post(fileno(f), /* path= */ ಖಾಲಿ, /* created= */ true, label_context);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        r = fchmod_umask(fileno(f), 0644);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        STRV_FOREACH(i, ಶೀರ್ಷಿಕೆಗಳು) {
                ಪ್ರತಿಜ್ಞಾ(isempty(*i) || startswith(*i, "#"));
                fputs_unlocked(*i, f);
                fputc_unlocked('\n', f);
        }

        STRV_FOREACH(i, l)
                write_env_var(f, *i);

        r = flink_tmpfile_at(f, dir_fd, p, fname, LINK_TMPFILE_REPLACE|LINK_TMPFILE_SYNC);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        p = mfree(p); /* disarm CLEANUP_TMPFILE_AT() */

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ write_vconsole_conf(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಅಕ್ಷರ **l) {
        ಅಕ್ಷರ **ಶೀರ್ಷಿಕೆಗಳು = STRV_MAKE(
                "# Written by systemd-localed(8) or systemd-firstboot(1), read by systemd-localed",
                "# and systemd-vconsole-setup(8). Use localectl(1) to update this file.");

        ಹಿಂತಿರುಗಿ write_env_file_label(dir_fd, fname, ಶೀರ್ಷಿಕೆಗಳು, l, WRITE_ENV_FILE_LABEL, /* label_context= */ ಖಾಲಿ);
}
