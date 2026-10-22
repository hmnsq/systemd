/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "escape.h"
#include "extract-word.h"
#include "log.h"
#include "string-util.h"
#include "utf8.h"

số_nguyên extract_first_word(const char **p, char **ret, const char *separators, ExtractFlags flags) {
        _cleanup_free_ char *s = NULL;
        size_t sz = 0;
        char quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        char c;
        số_nguyên r;

        assert(p);
        assert(ret);
        assert(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        nếu (!*p)
                goto finish;
        c = **p;

        nếu (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        nếu (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                nếu (!GREEDY_REALLOC(s, sz+1))
                        trả_về -ENOMEM;

        cho (;; (*p)++, c = **p) {
                nếu (c == 0)
                        goto finish_force_terminate;
                không_thì nếu (strchr(separators, c)) {
                        nếu (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                nếu (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                goto finish_force_next;
                        }
                } không_thì {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        nếu (!GREEDY_REALLOC(s, sz+1))
                                trả_về -ENOMEM;
                        break;
                }
        }

        cho (;; (*p)++, c = **p) {
                nếu (backslash) {
                        nếu (!GREEDY_REALLOC(s, sz+7))
                                trả_về -ENOMEM;

                        nếu (c == 0) {
                                nếu ((flags & EXTRACT_UNESCAPE_RELAX) &&
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
                                nếu (flags & EXTRACT_RELAX)
                                        goto finish_force_terminate;
                                trả_về -EINVAL;
                        }

                        nếu (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                nếu ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        assert(r >= 1);

                                        (*p) += r - 1;

                                        nếu (eight_bit)
                                                s[sz++] = u;
                                        không_thì
                                                sz += utf8_encode_unichar(s + sz, u);
                                } không_thì nếu ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                không_thì nếu (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } không_thì
                                        trả_về -EINVAL;
                        } không_thì
                                s[sz++] = c;

                        backslash = false;

                } không_thì nếu (quote != 0) {     /* inside either single or double quotes */
                        cho (;; (*p)++, c = **p) {
                                nếu (c == 0) {
                                        nếu (flags & EXTRACT_RELAX)
                                                goto finish_force_terminate;
                                        trả_về -EINVAL;
                                } không_thì nếu (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        nếu (flags & EXTRACT_UNQUOTE)
                                                break;
                                } không_thì nếu (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        break;
                                }

                                nếu (!GREEDY_REALLOC(s, sz+2))
                                        trả_về -ENOMEM;

                                s[sz++] = c;

                                nếu (quote == 0)
                                        break;
                        }

                } không_thì {
                        cho (;; (*p)++, c = **p) {
                                nếu (c == 0)
                                        goto finish_force_terminate;
                                không_thì nếu (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        nếu (flags & EXTRACT_UNQUOTE)
                                                break;
                                } không_thì nếu (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        break;
                                } không_thì nếu (strchr(separators, c)) {
                                        nếu (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                nếu (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                goto finish_force_next;
                                        }
                                        nếu (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                cho (;; (*p)++, c = **p) {
                                                        nếu (c == 0)
                                                                goto finish_force_terminate;
                                                        nếu (!strchr(separators, c))
                                                                break;
                                                }
                                        goto finish;

                                }

                                nếu (!GREEDY_REALLOC(s, sz+2))
                                        trả_về -ENOMEM;

                                s[sz++] = c;

                                nếu (quote != 0)
                                        break;
                        }
                }
        }

finish_force_terminate:
        *p = NULL;
finish:
        nếu (!s) {
                *p = NULL;
                *ret = NULL;
                trả_về 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        trả_về 1;
}

số_nguyên extract_first_word_and_warn(
                const char **p,
                char **ret,
                const char *separators,
                ExtractFlags flags,
                const char *unit,
                const char *filename,
                unsigned line,
                const char *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        const char *save;
        số_nguyên r;

        assert(p);
        assert(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        nếu (r >= 0)
                trả_về r;

        nếu (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                nếu (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, line, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        trả_về r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                nếu (r == -EINVAL)
                        trả_về log_syntax(unit, LOG_ERR, filename, line, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        trả_về log_syntax(unit, LOG_ERR, filename, line, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(sizeof(enum ExtractFlags) <= sizeof(unsigned));

số_nguyên extract_many_words_internal(const char **p, const char *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        số_nguyên r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        assert(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        trong_khi (va_arg(ap, char**))
                n++;
        va_end(ap);

        nếu (n == 0)
                trả_về 0;

        /* Read all words into a temporary array */
        char **l = newa0(char*, n);
        unsigned c;

        cho (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                nếu (r < 0) {
                        free_many_charp(l, c);
                        trả_về r;
                }
                nếu (r == 0)
                        break;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                char **v = ASSERT_PTR(va_arg(ap, char**));
                *v = *i;
        }
        va_end(ap);

        trả_về c;
}
