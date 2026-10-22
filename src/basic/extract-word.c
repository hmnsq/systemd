/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "escape.h"
#ضمن "extract-word.h"
#ضمن "log.h"
#ضمن "string-util.h"
#ضمن "utf8.h"

صحيح extract_first_word(ثابت حرف **p, حرف **ret, ثابت حرف *separators, ExtractFlags flags) {
        _cleanup_free_ حرف *s = عدم;
        size_t sz = 0;
        حرف quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        حرف c;
        صحيح r;

        أكد(p);
        أكد(ret);
        أكد(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        إذا (!*p)
                اذهب finish;
        c = **p;

        إذا (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        إذا (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                إذا (!GREEDY_REALLOC(s, sz+1))
                        أرجع -ENOMEM;

        لكل (;; (*p)++, c = **p) {
                إذا (c == 0)
                        اذهب finish_force_terminate;
                وإلا إذا (strchr(separators, c)) {
                        إذا (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                إذا (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                اذهب finish_force_next;
                        }
                } وإلا {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        إذا (!GREEDY_REALLOC(s, sz+1))
                                أرجع -ENOMEM;
                        اكسر;
                }
        }

        لكل (;; (*p)++, c = **p) {
                إذا (backslash) {
                        إذا (!GREEDY_REALLOC(s, sz+7))
                                أرجع -ENOMEM;

                        إذا (c == 0) {
                                إذا ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        اذهب finish_force_terminate;
                                }
                                إذا (flags & EXTRACT_RELAX)
                                        اذهب finish_force_terminate;
                                أرجع -EINVAL;
                        }

                        إذا (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                إذا ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        أكد(r >= 1);

                                        (*p) += r - 1;

                                        إذا (eight_bit)
                                                s[sz++] = u;
                                        وإلا
                                                sz += utf8_encode_unichar(s + sz, u);
                                } وإلا إذا ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                وإلا إذا (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } وإلا
                                        أرجع -EINVAL;
                        } وإلا
                                s[sz++] = c;

                        backslash = false;

                } وإلا إذا (quote != 0) {     /* inside either single or double quotes */
                        لكل (;; (*p)++, c = **p) {
                                إذا (c == 0) {
                                        إذا (flags & EXTRACT_RELAX)
                                                اذهب finish_force_terminate;
                                        أرجع -EINVAL;
                                } وإلا إذا (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        إذا (flags & EXTRACT_UNQUOTE)
                                                اكسر;
                                } وإلا إذا (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        اكسر;
                                }

                                إذا (!GREEDY_REALLOC(s, sz+2))
                                        أرجع -ENOMEM;

                                s[sz++] = c;

                                إذا (quote == 0)
                                        اكسر;
                        }

                } وإلا {
                        لكل (;; (*p)++, c = **p) {
                                إذا (c == 0)
                                        اذهب finish_force_terminate;
                                وإلا إذا (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        إذا (flags & EXTRACT_UNQUOTE)
                                                اكسر;
                                } وإلا إذا (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        اكسر;
                                } وإلا إذا (strchr(separators, c)) {
                                        إذا (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                إذا (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                اذهب finish_force_next;
                                        }
                                        إذا (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                لكل (;; (*p)++, c = **p) {
                                                        إذا (c == 0)
                                                                اذهب finish_force_terminate;
                                                        إذا (!strchr(separators, c))
                                                                اكسر;
                                                }
                                        اذهب finish;

                                }

                                إذا (!GREEDY_REALLOC(s, sz+2))
                                        أرجع -ENOMEM;

                                s[sz++] = c;

                                إذا (quote != 0)
                                        اكسر;
                        }
                }
        }

finish_force_terminate:
        *p = عدم;
finish:
        إذا (!s) {
                *p = عدم;
                *ret = عدم;
                أرجع 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        أرجع 1;
}

صحيح extract_first_word_and_warn(
                ثابت حرف **p,
                حرف **ret,
                ثابت حرف *separators,
                ExtractFlags flags,
                ثابت حرف *unit,
                ثابت حرف *filename,
                unsigned قۇر,
                ثابت حرف *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ثابت حرف *save;
        صحيح r;

        أكد(p);
        أكد(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        إذا (r >= 0)
                أرجع r;

        إذا (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                إذا (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, قۇر, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        أرجع r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                إذا (r == -EINVAL)
                        أرجع log_syntax(unit, LOG_ERR, filename, قۇر, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        أرجع log_syntax(unit, LOG_ERR, filename, قۇر, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(حجم(تعداد ExtractFlags) <= حجم(unsigned));

صحيح extract_many_سۆزلەر_internal(ثابت حرف **p, ثابت حرف *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        صحيح r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        أكد(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        طالما (va_arg(ap, حرف**))
                n++;
        va_end(ap);

        إذا (n == 0)
                أرجع 0;

        /* Read all words into a temporary array */
        حرف **l = newa0(حرف*, n);
        unsigned c;

        لكل (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                إذا (r < 0) {
                        free_many_charp(l, c);
                        أرجع r;
                }
                إذا (r == 0)
                        اكسر;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                حرف **v = ASSERT_PTR(va_arg(ap, حرف**));
                *v = *i;
        }
        va_end(ap);

        أرجع c;
}
