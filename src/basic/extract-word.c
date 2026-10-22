/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಪೂರ್ಣಾಂಕ ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, ಅಕ್ಷರ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಹೊರತೆಗೆ_Flags flags) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        size_t sz = 0;
        ಅಕ್ಷರ quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        ಅಕ್ಷರ c;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, ಹೊರತೆಗೆ_KEEP_QUOTE | ಹೊರತೆಗೆ_UNQUOTE));

        /* Bail early if called after last value or with no input */
        ಆದರೆ (!*p)
                ಹೋಗು ಮುಕ್ತಾಯ;
        c = **p;

        ಆದರೆ (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        ಆದರೆ (flags & ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS)
                ಆದರೆ (!GREEDY_REALLOC(s, sz+1))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (;; (*p)++, c = **p) {
                ಆದರೆ (c == 0)
                        ಹೋಗು ಮುಕ್ತಾಯ_force_terminate;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strchr(separators, c)) {
                        ಆದರೆ (flags & ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS) {
                                ಆದರೆ (!(flags & ಹೊರತೆಗೆ_RETAIN_SEPARATORS))
                                        (*p)++;
                                ಹೋಗು ಮುಕ್ತಾಯ_force_ಮುಂದಿನ;
                        }
                } ಇಲ್ಲದಿದ್ದರೆ {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        ಆದರೆ (!GREEDY_REALLOC(s, sz+1))
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                        ನಿಲ್ಲಿಸು;
                }
        }

        ಗಾಗಿ (;; (*p)++, c = **p) {
                ಆದರೆ (backslash) {
                        ಆದರೆ (!GREEDY_REALLOC(s, sz+7))
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        ಆದರೆ (c == 0) {
                                ಆದರೆ ((flags & ಹೊರತೆಗೆ_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & ಹೊರತೆಗೆ_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        ಹೋಗು ಮುಕ್ತಾಯ_force_terminate;
                                }
                                ಆದರೆ (flags & ಹೊರತೆಗೆ_RELAX)
                                        ಹೋಗು ಮುಕ್ತಾಯ_force_terminate;
                                ಹಿಂತಿರುಗಿ -EINVAL;
                        }

                        ಆದರೆ (flags & (ಹೊರತೆಗೆ_CUNESCAPE|ಹೊರತೆಗೆ_UNESCAPE_SEPARATORS|ಹೊರತೆಗೆ_UNESCAPE_RELAX)) {
                                bool eight_ಬಿಟ್ = false;
                                char32_t u;

                                ಆದರೆ ((flags & ಹೊರತೆಗೆ_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_ಬಿಟ್, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        ಪ್ರತಿಜ್ಞಾ(r >= 1);

                                        (*p) += r - 1;

                                        ಆದರೆ (eight_ಬಿಟ್)
                                                s[sz++] = u;
                                        ಇಲ್ಲದಿದ್ದರೆ
                                                sz += utf8_encode_unichar(s + sz, u);
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((flags & ಹೊರತೆಗೆ_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (flags & ಹೊರತೆಗೆ_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } ಇಲ್ಲದಿದ್ದರೆ
                                        ಹಿಂತಿರುಗಿ -EINVAL;
                        } ಇಲ್ಲದಿದ್ದರೆ
                                s[sz++] = c;

                        backslash = false;

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (quote != 0) {     /* inside either single or double quotes */
                        ಗಾಗಿ (;; (*p)++, c = **p) {
                                ಆದರೆ (c == 0) {
                                        ಆದರೆ (flags & ಹೊರತೆಗೆ_RELAX)
                                                ಹೋಗು ಮುಕ್ತಾಯ_force_terminate;
                                        ಹಿಂತಿರುಗಿ -EINVAL;
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        ಆದರೆ (flags & ಹೊರತೆಗೆ_UNQUOTE)
                                                ನಿಲ್ಲಿಸು;
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\\' && !(flags & ಹೊರತೆಗೆ_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ನಿಲ್ಲಿಸು;
                                }

                                ಆದರೆ (!GREEDY_REALLOC(s, sz+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                s[sz++] = c;

                                ಆದರೆ (quote == 0)
                                        ನಿಲ್ಲಿಸು;
                        }

                } ಇಲ್ಲದಿದ್ದರೆ {
                        ಗಾಗಿ (;; (*p)++, c = **p) {
                                ಆದರೆ (c == 0)
                                        ಹೋಗು ಮುಕ್ತಾಯ_force_terminate;
                                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(c, '\'', '"') && (flags & (ಹೊರತೆಗೆ_KEEP_QUOTE | ಹೊರತೆಗೆ_UNQUOTE))) {
                                        quote = c;
                                        ಆದರೆ (flags & ಹೊರತೆಗೆ_UNQUOTE)
                                                ನಿಲ್ಲಿಸು;
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\\' && !(flags & ಹೊರತೆಗೆ_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ನಿಲ್ಲಿಸು;
                                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (strchr(separators, c)) {
                                        ಆದರೆ (flags & ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS) {
                                                ಆದರೆ (!(flags & ಹೊರತೆಗೆ_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                ಹೋಗು ಮುಕ್ತಾಯ_force_ಮುಂದಿನ;
                                        }
                                        ಆದರೆ (!(flags & ಹೊರತೆಗೆ_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                ಗಾಗಿ (;; (*p)++, c = **p) {
                                                        ಆದರೆ (c == 0)
                                                                ಹೋಗು ಮುಕ್ತಾಯ_force_terminate;
                                                        ಆದರೆ (!strchr(separators, c))
                                                                ನಿಲ್ಲಿಸು;
                                                }
                                        ಹೋಗು ಮುಕ್ತಾಯ;

                                }

                                ಆದರೆ (!GREEDY_REALLOC(s, sz+2))
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                s[sz++] = c;

                                ಆದರೆ (quote != 0)
                                        ನಿಲ್ಲಿಸು;
                        }
                }
        }

ಮುಕ್ತಾಯ_force_terminate:
        *p = ಖಾಲಿ;
ಮುಕ್ತಾಯ:
        ಆದರೆ (!s) {
                *p = ಖಾಲಿ;
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

ಮುಕ್ತಾಯ_force_ಮುಂದಿನ:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ_and_warn(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p,
                ಅಕ್ಷರ **ret,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators,
                ಹೊರತೆಗೆ_Flags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename,
                ಚಿಹ್ನೆರಹಿತ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *save;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        save = *p;
        r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(p, ret, separators, flags);
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (r == -EINVAL && !(flags & ಹೊರತೆಗೆ_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(p, ret, separators, flags|ಹೊರತೆಗೆ_UNESCAPE_RELAX);
                ಆದರೆ (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, ಸಾಲು, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        ಹಿಂತಿರುಗಿ r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                ಆದರೆ (r == -EINVAL)
                        ಹಿಂತಿರುಗಿ log_syntax(unit, LOG_ERR, filename, ಸಾಲು, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        ಹಿಂತಿರುಗಿ log_syntax(unit, LOG_ERR, filename, ಸಾಲು, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(ಗಾತ್ರ(ಎಣಿಕೆ ಹೊರತೆಗೆ_Flags) <= ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ));

ಪೂರ್ಣಾಂಕ ಹೊರತೆಗೆ_many_ಪದಗಳು_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಚಿಹ್ನೆರಹಿತ flags, ...) {
        va_list ap;
        ಚಿಹ್ನೆರಹಿತ n = 0;
        ಪೂರ್ಣಾಂಕ r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        ಪ್ರತಿಜ್ಞಾ(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        ಆಗ (va_arg(ap, ಅಕ್ಷರ**))
                n++;
        va_end(ap);

        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ 0;

        /* Read all words into a temporary array */
        ಅಕ್ಷರ **l = newa0(ಅಕ್ಷರ*, n);
        ಚಿಹ್ನೆರಹಿತ c;

        ಗಾಗಿ (c = 0; c < n; c++) {
                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(p, &l[c], separators, flags);
                ಆದರೆ (r < 0) {
                        free_many_charp(l, c);
                        ಹಿಂತಿರುಗಿ r;
                }
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                ಅಕ್ಷರ **v = ASSERT_PTR(va_arg(ap, ಅಕ್ಷರ**));
                *v = *i;
        }
        va_end(ap);

        ಹಿಂತಿರುಗಿ c;
}
