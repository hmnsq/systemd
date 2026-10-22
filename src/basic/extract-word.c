/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "escape.h"
#شامل "extract-word.h"
#شامل "log.h"
#شامل "string-util.h"
#شامل "utf8.h"

صحیح نکالو_پہلا_لفظ(ثابت حرف **p, حرف **ret, ثابت حرف *separators, نکالو_Flags flags) {
        _cleanup_free_ حرف *s = تہی;
        size_t sz = 0;
        حرف quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        حرف c;
        صحیح r;

        أكد(p);
        أكد(ret);
        أكد(!FLAGS_سیٹ(flags, نکالو_KEEP_QUOTE | نکالو_UNQUOTE));

        /* Bail early if called after last value or with no input */
        اگر (!*p)
                جاؤ اختتام;
        c = **p;

        اگر (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        اگر (flags & نکالو_DONT_COALESCE_SEPARATORS)
                اگر (!GREEDY_REALLOC(s, sz+1))
                        واپس -ENOMEM;

        برائے (;; (*p)++, c = **p) {
                اگر (c == 0)
                        جاؤ اختتام_force_terminate;
                ورنہ اگر (strchr(separators, c)) {
                        اگر (flags & نکالو_DONT_COALESCE_SEPARATORS) {
                                اگر (!(flags & نکالو_RETAIN_SEPARATORS))
                                        (*p)++;
                                جاؤ اختتام_force_اگلا;
                        }
                } ورنہ {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        اگر (!GREEDY_REALLOC(s, sz+1))
                                واپس -ENOMEM;
                        توڑو;
                }
        }

        برائے (;; (*p)++, c = **p) {
                اگر (backslash) {
                        اگر (!GREEDY_REALLOC(s, sz+7))
                                واپس -ENOMEM;

                        اگر (c == 0) {
                                اگر ((flags & نکالو_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & نکالو_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        جاؤ اختتام_force_terminate;
                                }
                                اگر (flags & نکالو_RELAX)
                                        جاؤ اختتام_force_terminate;
                                واپس -EINVAL;
                        }

                        اگر (flags & (نکالو_CUNESCAPE|نکالو_UNESCAPE_SEPARATORS|نکالو_UNESCAPE_RELAX)) {
                                bool eight_ثنائی_ہندسہ = false;
                                char32_t u;

                                اگر ((flags & نکالو_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_ثنائی_ہندسہ, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        أكد(r >= 1);

                                        (*p) += r - 1;

                                        اگر (eight_ثنائی_ہندسہ)
                                                s[sz++] = u;
                                        ورنہ
                                                sz += utf8_encode_unichar(s + sz, u);
                                } ورنہ اگر ((flags & نکالو_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                ورنہ اگر (flags & نکالو_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } ورنہ
                                        واپس -EINVAL;
                        } ورنہ
                                s[sz++] = c;

                        backslash = false;

                } ورنہ اگر (quote != 0) {     /* inside either single or double quotes */
                        برائے (;; (*p)++, c = **p) {
                                اگر (c == 0) {
                                        اگر (flags & نکالو_RELAX)
                                                جاؤ اختتام_force_terminate;
                                        واپس -EINVAL;
                                } ورنہ اگر (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        اگر (flags & نکالو_UNQUOTE)
                                                توڑو;
                                } ورنہ اگر (c == '\\' && !(flags & نکالو_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        توڑو;
                                }

                                اگر (!GREEDY_REALLOC(s, sz+2))
                                        واپس -ENOMEM;

                                s[sz++] = c;

                                اگر (quote == 0)
                                        توڑو;
                        }

                } ورنہ {
                        برائے (;; (*p)++, c = **p) {
                                اگر (c == 0)
                                        جاؤ اختتام_force_terminate;
                                ورنہ اگر (اندر_سیٹ(c, '\'', '"') && (flags & (نکالو_KEEP_QUOTE | نکالو_UNQUOTE))) {
                                        quote = c;
                                        اگر (flags & نکالو_UNQUOTE)
                                                توڑو;
                                } ورنہ اگر (c == '\\' && !(flags & نکالو_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        توڑو;
                                } ورنہ اگر (strchr(separators, c)) {
                                        اگر (flags & نکالو_DONT_COALESCE_SEPARATORS) {
                                                اگر (!(flags & نکالو_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                جاؤ اختتام_force_اگلا;
                                        }
                                        اگر (!(flags & نکالو_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                برائے (;; (*p)++, c = **p) {
                                                        اگر (c == 0)
                                                                جاؤ اختتام_force_terminate;
                                                        اگر (!strchr(separators, c))
                                                                توڑو;
                                                }
                                        جاؤ اختتام;

                                }

                                اگر (!GREEDY_REALLOC(s, sz+2))
                                        واپس -ENOMEM;

                                s[sz++] = c;

                                اگر (quote != 0)
                                        توڑو;
                        }
                }
        }

اختتام_force_terminate:
        *p = تہی;
اختتام:
        اگر (!s) {
                *p = تہی;
                *ret = تہی;
                واپس 0;
        }

اختتام_force_اگلا:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        واپس 1;
}

صحیح نکالو_پہلا_لفظ_and_warn(
                ثابت حرف **p,
                حرف **ret,
                ثابت حرف *separators,
                نکالو_Flags flags,
                ثابت حرف *unit,
                ثابت حرف *filename,
                غیرعلامتی سطر,
                ثابت حرف *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ثابت حرف *save;
        صحیح r;

        أكد(p);
        أكد(ret);

        save = *p;
        r = نکالو_پہلا_لفظ(p, ret, separators, flags);
        اگر (r >= 0)
                واپس r;

        اگر (r == -EINVAL && !(flags & نکالو_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = نکالو_پہلا_لفظ(p, ret, separators, flags|نکالو_UNESCAPE_RELAX);
                اگر (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, سطر, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        واپس r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                اگر (r == -EINVAL)
                        واپس log_syntax(unit, LOG_ERR, filename, سطر, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        واپس log_syntax(unit, LOG_ERR, filename, سطر, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(حجم(شمار نکالو_Flags) <= حجم(غیرعلامتی));

صحیح نکالو_many_الفاظ_اندرونی(ثابت حرف **p, ثابت حرف *separators, غیرعلامتی flags, ...) {
        va_list ap;
        غیرعلامتی n = 0;
        صحیح r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        أكد(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        جب (va_arg(ap, حرف**))
                n++;
        va_end(ap);

        اگر (n == 0)
                واپس 0;

        /* Read all words into a temporary array */
        حرف **l = newa0(حرف*, n);
        غیرعلامتی c;

        برائے (c = 0; c < n; c++) {
                r = نکالو_پہلا_لفظ(p, &l[c], separators, flags);
                اگر (r < 0) {
                        free_many_charp(l, c);
                        واپس r;
                }
                اگر (r == 0)
                        توڑو;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                حرف **v = ASSERT_PTR(va_arg(ap, حرف**));
                *v = *i;
        }
        va_end(ap);

        واپس c;
}
