/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "escape.h"
#شامل "extract-word.h"
#شامل "log.h"
#شامل "string-util.h"
#شامل "utf8.h"

صحیح extract_first_word(ثابت نویسه **p, نویسه **ret, ثابت نویسه *separators, ExtractFlags flags) {
        _cleanup_free_ نویسه *s = پوچ;
        size_t sz = 0;
        نویسه quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        نویسه c;
        صحیح r;

        أكد(p);
        أكد(ret);
        أكد(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        اگر (!*p)
                برو finish;
        c = **p;

        اگر (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        اگر (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                اگر (!GREEDY_REALLOC(s, sz+1))
                        بازگشت -ENOMEM;

        برای (;; (*p)++, c = **p) {
                اگر (c == 0)
                        برو finish_force_terminate;
                وگرنه اگر (strchr(separators, c)) {
                        اگر (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                اگر (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                برو finish_force_next;
                        }
                } وگرنه {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        اگر (!GREEDY_REALLOC(s, sz+1))
                                بازگشت -ENOMEM;
                        بشکن;
                }
        }

        برای (;; (*p)++, c = **p) {
                اگر (backslash) {
                        اگر (!GREEDY_REALLOC(s, sz+7))
                                بازگشت -ENOMEM;

                        اگر (c == 0) {
                                اگر ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        برو finish_force_terminate;
                                }
                                اگر (flags & EXTRACT_RELAX)
                                        برو finish_force_terminate;
                                بازگشت -EINVAL;
                        }

                        اگر (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                اگر ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        أكد(r >= 1);

                                        (*p) += r - 1;

                                        اگر (eight_bit)
                                                s[sz++] = u;
                                        وگرنه
                                                sz += utf8_encode_unichar(s + sz, u);
                                } وگرنه اگر ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                وگرنه اگر (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } وگرنه
                                        بازگشت -EINVAL;
                        } وگرنه
                                s[sz++] = c;

                        backslash = false;

                } وگرنه اگر (quote != 0) {     /* inside either single or double quotes */
                        برای (;; (*p)++, c = **p) {
                                اگر (c == 0) {
                                        اگر (flags & EXTRACT_RELAX)
                                                برو finish_force_terminate;
                                        بازگشت -EINVAL;
                                } وگرنه اگر (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        اگر (flags & EXTRACT_UNQUOTE)
                                                بشکن;
                                } وگرنه اگر (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        بشکن;
                                }

                                اگر (!GREEDY_REALLOC(s, sz+2))
                                        بازگشت -ENOMEM;

                                s[sz++] = c;

                                اگر (quote == 0)
                                        بشکن;
                        }

                } وگرنه {
                        برای (;; (*p)++, c = **p) {
                                اگر (c == 0)
                                        برو finish_force_terminate;
                                وگرنه اگر (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        اگر (flags & EXTRACT_UNQUOTE)
                                                بشکن;
                                } وگرنه اگر (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        بشکن;
                                } وگرنه اگر (strchr(separators, c)) {
                                        اگر (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                اگر (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                برو finish_force_next;
                                        }
                                        اگر (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                برای (;; (*p)++, c = **p) {
                                                        اگر (c == 0)
                                                                برو finish_force_terminate;
                                                        اگر (!strchr(separators, c))
                                                                بشکن;
                                                }
                                        برو finish;

                                }

                                اگر (!GREEDY_REALLOC(s, sz+2))
                                        بازگشت -ENOMEM;

                                s[sz++] = c;

                                اگر (quote != 0)
                                        بشکن;
                        }
                }
        }

finish_force_terminate:
        *p = پوچ;
finish:
        اگر (!s) {
                *p = پوچ;
                *ret = پوچ;
                بازگشت 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        بازگشت 1;
}

صحیح extract_first_word_and_warn(
                ثابت نویسه **p,
                نویسه **ret,
                ثابت نویسه *separators,
                ExtractFlags flags,
                ثابت نویسه *unit,
                ثابت نویسه *filename,
                unsigned سطر,
                ثابت نویسه *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ثابت نویسه *save;
        صحیح r;

        أكد(p);
        أكد(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        اگر (r >= 0)
                بازگشت r;

        اگر (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                اگر (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, سطر, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        بازگشت r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                اگر (r == -EINVAL)
                        بازگشت log_syntax(unit, LOG_ERR, filename, سطر, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        بازگشت log_syntax(unit, LOG_ERR, filename, سطر, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(اندازه(شمارش ExtractFlags) <= اندازه(unsigned));

صحیح extract_many_واژگان_internal(ثابت نویسه **p, ثابت نویسه *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        صحیح r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        أكد(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        وقتی (va_arg(ap, نویسه**))
                n++;
        va_end(ap);

        اگر (n == 0)
                بازگشت 0;

        /* Read all words into a temporary array */
        نویسه **l = newa0(نویسه*, n);
        unsigned c;

        برای (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                اگر (r < 0) {
                        free_many_charp(l, c);
                        بازگشت r;
                }
                اگر (r == 0)
                        بشکن;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                نویسه **v = ASSERT_PTR(va_arg(ap, نویسه**));
                *v = *i;
        }
        va_end(ap);

        بازگشت c;
}
