/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "escape.h"
#include "extract-word.h"
#include "log.h"
#include "string-util.h"
#include "utf8.h"

จำนวนเต็ม extract_first_word(const var_na **p, var_na **ret, const var_na *separators, ExtractFlags flags) {
        _cleanup_free_ var_na *s = NULL;
        size_t sz = 0;
        var_na quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        var_na c;
        จำนวนเต็ม r;

        assert(p);
        assert(ret);
        assert(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        ถ้า (!*p)
                goto finish;
        c = **p;

        ถ้า (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        ถ้า (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                ถ้า (!GREEDY_REALLOC(s, sz+1))
                        คืน -ENOMEM;

        สำหรับ (;; (*p)++, c = **p) {
                ถ้า (c == 0)
                        goto finish_force_terminate;
                มิฉะนั้น ถ้า (strchr(separators, c)) {
                        ถ้า (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                ถ้า (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                goto finish_force_next;
                        }
                } มิฉะนั้น {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        ถ้า (!GREEDY_REALLOC(s, sz+1))
                                คืน -ENOMEM;
                        bha_nga;
                }
        }

        สำหรับ (;; (*p)++, c = **p) {
                ถ้า (backslash) {
                        ถ้า (!GREEDY_REALLOC(s, sz+7))
                                คืน -ENOMEM;

                        ถ้า (c == 0) {
                                ถ้า ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        goto finish_force_terminate;
                                }
                                ถ้า (flags & EXTRACT_RELAX)
                                        goto finish_force_terminate;
                                คืน -EINVAL;
                        }

                        ถ้า (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                ถ้า ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        assert(r >= 1);

                                        (*p) += r - 1;

                                        ถ้า (eight_bit)
                                                s[sz++] = u;
                                        มิฉะนั้น
                                                sz += utf8_encode_unichar(s + sz, u);
                                } มิฉะนั้น ถ้า ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                มิฉะนั้น ถ้า (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } มิฉะนั้น
                                        คืน -EINVAL;
                        } มิฉะนั้น
                                s[sz++] = c;

                        backslash = false;

                } มิฉะนั้น ถ้า (quote != 0) {     /* inside either single or double quotes */
                        สำหรับ (;; (*p)++, c = **p) {
                                ถ้า (c == 0) {
                                        ถ้า (flags & EXTRACT_RELAX)
                                                goto finish_force_terminate;
                                        คืน -EINVAL;
                                } มิฉะนั้น ถ้า (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        ถ้า (flags & EXTRACT_UNQUOTE)
                                                bha_nga;
                                } มิฉะนั้น ถ้า (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        bha_nga;
                                }

                                ถ้า (!GREEDY_REALLOC(s, sz+2))
                                        คืน -ENOMEM;

                                s[sz++] = c;

                                ถ้า (quote == 0)
                                        bha_nga;
                        }

                } มิฉะนั้น {
                        สำหรับ (;; (*p)++, c = **p) {
                                ถ้า (c == 0)
                                        goto finish_force_terminate;
                                มิฉะนั้น ถ้า (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        ถ้า (flags & EXTRACT_UNQUOTE)
                                                bha_nga;
                                } มิฉะนั้น ถ้า (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        bha_nga;
                                } มิฉะนั้น ถ้า (strchr(separators, c)) {
                                        ถ้า (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                ถ้า (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                goto finish_force_next;
                                        }
                                        ถ้า (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                สำหรับ (;; (*p)++, c = **p) {
                                                        ถ้า (c == 0)
                                                                goto finish_force_terminate;
                                                        ถ้า (!strchr(separators, c))
                                                                bha_nga;
                                                }
                                        goto finish;

                                }

                                ถ้า (!GREEDY_REALLOC(s, sz+2))
                                        คืน -ENOMEM;

                                s[sz++] = c;

                                ถ้า (quote != 0)
                                        bha_nga;
                        }
                }
        }

finish_force_terminate:
        *p = NULL;
finish:
        ถ้า (!s) {
                *p = NULL;
                *ret = NULL;
                คืน 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        คืน 1;
}

จำนวนเต็ม extract_first_word_and_warn(
                const var_na **p,
                var_na **ret,
                const var_na *separators,
                ExtractFlags flags,
                const var_na *unit,
                const var_na *filename,
                unsigned line,
                const var_na *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        const var_na *save;
        จำนวนเต็ม r;

        assert(p);
        assert(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        ถ้า (r >= 0)
                คืน r;

        ถ้า (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                ถ้า (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, line, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        คืน r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                ถ้า (r == -EINVAL)
                        คืน log_syntax(unit, LOG_ERR, filename, line, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        คืน log_syntax(unit, LOG_ERR, filename, line, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(parimaa_na(ga_nanaa ExtractFlags) <= parimaa_na(unsigned));

จำนวนเต็ม extract_many_words_internal(const var_na **p, const var_na *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        จำนวนเต็ม r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        assert(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        ขณะ (va_arg(ap, var_na**))
                n++;
        va_end(ap);

        ถ้า (n == 0)
                คืน 0;

        /* Read all words into a temporary array */
        var_na **l = newa0(var_na*, n);
        unsigned c;

        สำหรับ (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                ถ้า (r < 0) {
                        free_many_charp(l, c);
                        คืน r;
                }
                ถ้า (r == 0)
                        bha_nga;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                var_na **v = ASSERT_PTR(va_arg(ap, var_na**));
                *v = *i;
        }
        va_end(ap);

        คืน c;
}
