/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಪೂರ್ಣಾಙ್ಕ extract_first_word(ಧ್ರುವ ವರ್ಣ **p, ವರ್ಣ **ret, ಧ್ರುವ ವರ್ಣ *separators, ExtractFlags flags) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        size_t sz = 0;
        ವರ್ಣ quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        ವರ್ಣ c;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        ಯದಿ (!*p)
                ಗಚ್ಛ finish;
        c = **p;

        ಯದಿ (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        ಯದಿ (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                ಯದಿ (!GREEDY_REALLOC(s, sz+1))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (;; (*p)++, c = **p) {
                ಯದಿ (c == 0)
                        ಗಚ್ಛ finish_force_terminate;
                ಅನ್ಯಥಾ ಯದಿ (strchr(separators, c)) {
                        ಯದಿ (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                ಯದಿ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                ಗಚ್ಛ finish_force_next;
                        }
                } ಅನ್ಯಥಾ {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        ಯದಿ (!GREEDY_REALLOC(s, sz+1))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        ಭಙ್ಗ;
                }
        }

        ಪ್ರತಿ (;; (*p)++, c = **p) {
                ಯದಿ (backslash) {
                        ಯದಿ (!GREEDY_REALLOC(s, sz+7))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        ಯದಿ (c == 0) {
                                ಯದಿ ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        ಗಚ್ಛ finish_force_terminate;
                                }
                                ಯದಿ (flags & EXTRACT_RELAX)
                                        ಗಚ್ಛ finish_force_terminate;
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;
                        }

                        ಯದಿ (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                ಯದಿ ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        ಪ್ರತಿಜ್ಞಾ(r >= 1);

                                        (*p) += r - 1;

                                        ಯದಿ (eight_bit)
                                                s[sz++] = u;
                                        ಅನ್ಯಥಾ
                                                sz += utf8_encode_unichar(s + sz, u);
                                } ಅನ್ಯಥಾ ಯದಿ ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                ಅನ್ಯಥಾ ಯದಿ (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } ಅನ್ಯಥಾ
                                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                        } ಅನ್ಯಥಾ
                                s[sz++] = c;

                        backslash = false;

                } ಅನ್ಯಥಾ ಯದಿ (quote != 0) {     /* inside either single or double quotes */
                        ಪ್ರತಿ (;; (*p)++, c = **p) {
                                ಯದಿ (c == 0) {
                                        ಯದಿ (flags & EXTRACT_RELAX)
                                                ಗಚ್ಛ finish_force_terminate;
                                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                                } ಅನ್ಯಥಾ ಯದಿ (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        ಯದಿ (flags & EXTRACT_UNQUOTE)
                                                ಭಙ್ಗ;
                                } ಅನ್ಯಥಾ ಯದಿ (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ಭಙ್ಗ;
                                }

                                ಯದಿ (!GREEDY_REALLOC(s, sz+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                s[sz++] = c;

                                ಯದಿ (quote == 0)
                                        ಭಙ್ಗ;
                        }

                } ಅನ್ಯಥಾ {
                        ಪ್ರತಿ (;; (*p)++, c = **p) {
                                ಯದಿ (c == 0)
                                        ಗಚ್ಛ finish_force_terminate;
                                ಅನ್ಯಥಾ ಯದಿ (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        ಯದಿ (flags & EXTRACT_UNQUOTE)
                                                ಭಙ್ಗ;
                                } ಅನ್ಯಥಾ ಯದಿ (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ಭಙ್ಗ;
                                } ಅನ್ಯಥಾ ಯದಿ (strchr(separators, c)) {
                                        ಯದಿ (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                ಯದಿ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                ಗಚ್ಛ finish_force_next;
                                        }
                                        ಯದಿ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                ಪ್ರತಿ (;; (*p)++, c = **p) {
                                                        ಯದಿ (c == 0)
                                                                ಗಚ್ಛ finish_force_terminate;
                                                        ಯದಿ (!strchr(separators, c))
                                                                ಭಙ್ಗ;
                                                }
                                        ಗಚ್ಛ finish;

                                }

                                ಯದಿ (!GREEDY_REALLOC(s, sz+2))
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                s[sz++] = c;

                                ಯದಿ (quote != 0)
                                        ಭಙ್ಗ;
                        }
                }
        }

finish_force_terminate:
        *p = ರಿಕ್ತ;
finish:
        ಯದಿ (!s) {
                *p = ರಿಕ್ತ;
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ extract_first_word_and_warn(
                ಧ್ರುವ ವರ್ಣ **p,
                ವರ್ಣ **ret,
                ಧ್ರುವ ವರ್ಣ *separators,
                ExtractFlags flags,
                ಧ್ರುವ ವರ್ಣ *unit,
                ಧ್ರುವ ವರ್ಣ *filename,
                unsigned ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ಧ್ರುವ ವರ್ಣ *save;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                ಯದಿ (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, ಸಾಲು, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        ಪ್ರತ್ಯಾಗಮ r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                ಯದಿ (r == -EINVAL)
                        ಪ್ರತ್ಯಾಗಮ log_syntax(unit, LOG_ERR, filename, ಸಾಲು, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        ಪ್ರತ್ಯಾಗಮ log_syntax(unit, LOG_ERR, filename, ಸಾಲು, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(ಪರಿಮಾಣ(ಗಣನಾ ExtractFlags) <= ಪರಿಮಾಣ(unsigned));

ಪೂರ್ಣಾಙ್ಕ extract_many_ಪದಗಳು_internal(ಧ್ರುವ ವರ್ಣ **p, ಧ್ರುವ ವರ್ಣ *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        ಪ್ರತಿಜ್ಞಾ(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        ಯಾವತ್ (va_arg(ap, ವರ್ಣ**))
                n++;
        va_end(ap);

        ಯದಿ (n == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* Read all words into a temporary array */
        ವರ್ಣ **l = newa0(ವರ್ಣ*, n);
        unsigned c;

        ಪ್ರತಿ (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                ಯದಿ (r < 0) {
                        free_many_charp(l, c);
                        ಪ್ರತ್ಯಾಗಮ r;
                }
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                ವರ್ಣ **v = ASSERT_PTR(va_arg(ap, ವರ್ಣ**));
                *v = *i;
        }
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ c;
}
