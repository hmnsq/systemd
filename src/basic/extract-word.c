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
        אם (!*p)
                اذهب finish;
        c = **p;

        אם (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        אם (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                אם (!GREEDY_REALLOC(s, sz+1))
                        החזר -ENOMEM;

        עבור (;; (*p)++, c = **p) {
                אם (c == 0)
                        اذهب finish_force_terminate;
                אחרת אם (strchr(separators, c)) {
                        אם (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                אם (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                اذهب finish_force_next;
                        }
                } אחרת {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        אם (!GREEDY_REALLOC(s, sz+1))
                                החזר -ENOMEM;
                        اكسر;
                }
        }

        עבור (;; (*p)++, c = **p) {
                אם (backslash) {
                        אם (!GREEDY_REALLOC(s, sz+7))
                                החזר -ENOMEM;

                        אם (c == 0) {
                                אם ((flags & EXTRACT_UNESCAPE_RELAX) &&
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
                                אם (flags & EXTRACT_RELAX)
                                        اذهب finish_force_terminate;
                                החזר -EINVAL;
                        }

                        אם (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                אם ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        أكد(r >= 1);

                                        (*p) += r - 1;

                                        אם (eight_bit)
                                                s[sz++] = u;
                                        אחרת
                                                sz += utf8_encode_unichar(s + sz, u);
                                } אחרת אם ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                אחרת אם (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } אחרת
                                        החזר -EINVAL;
                        } אחרת
                                s[sz++] = c;

                        backslash = false;

                } אחרת אם (quote != 0) {     /* inside either single or double quotes */
                        עבור (;; (*p)++, c = **p) {
                                אם (c == 0) {
                                        אם (flags & EXTRACT_RELAX)
                                                اذهب finish_force_terminate;
                                        החזר -EINVAL;
                                } אחרת אם (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        אם (flags & EXTRACT_UNQUOTE)
                                                اكسر;
                                } אחרת אם (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        اكسر;
                                }

                                אם (!GREEDY_REALLOC(s, sz+2))
                                        החזר -ENOMEM;

                                s[sz++] = c;

                                אם (quote == 0)
                                        اكسر;
                        }

                } אחרת {
                        עבור (;; (*p)++, c = **p) {
                                אם (c == 0)
                                        اذهب finish_force_terminate;
                                אחרת אם (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        אם (flags & EXTRACT_UNQUOTE)
                                                اكسر;
                                } אחרת אם (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        اكسر;
                                } אחרת אם (strchr(separators, c)) {
                                        אם (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                אם (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                اذهب finish_force_next;
                                        }
                                        אם (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                עבור (;; (*p)++, c = **p) {
                                                        אם (c == 0)
                                                                اذهب finish_force_terminate;
                                                        אם (!strchr(separators, c))
                                                                اكسر;
                                                }
                                        اذهب finish;

                                }

                                אם (!GREEDY_REALLOC(s, sz+2))
                                        החזר -ENOMEM;

                                s[sz++] = c;

                                אם (quote != 0)
                                        اكسر;
                        }
                }
        }

finish_force_terminate:
        *p = عدم;
finish:
        אם (!s) {
                *p = عدم;
                *ret = عدم;
                החזר 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        החזר 1;
}

صحيح extract_first_word_and_warn(
                ثابت حرف **p,
                حرف **ret,
                ثابت حرف *separators,
                ExtractFlags flags,
                ثابت حرف *unit,
                ثابت حرف *filename,
                unsigned line,
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
        אם (r >= 0)
                החזר r;

        אם (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                אם (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, line, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        החזר r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                אם (r == -EINVAL)
                        החזר log_syntax(unit, LOG_ERR, filename, line, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        החזר log_syntax(unit, LOG_ERR, filename, line, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(حجم(تعداد ExtractFlags) <= حجم(unsigned));

صحيح extract_many_words_internal(ثابت حرف **p, ثابت حرف *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        صحيح r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        أكد(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        כלעוד (va_arg(ap, حرف**))
                n++;
        va_end(ap);

        אם (n == 0)
                החזר 0;

        /* Read all words into a temporary array */
        حرف **l = newa0(حرف*, n);
        unsigned c;

        עבור (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                אם (r < 0) {
                        free_many_charp(l, c);
                        החזר r;
                }
                אם (r == 0)
                        اكسر;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                حرف **v = ASSERT_PTR(va_arg(ap, حرف**));
                *v = *i;
        }
        va_end(ap);

        החזר c;
}
