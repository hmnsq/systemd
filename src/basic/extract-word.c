/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "escape.h"
#include "extract-word.h"
#include "log.h"
#include "string-util.h"
#include "utf8.h"

整数 extract_first_word(const char **p, char **ret, const char *separators, ExtractFlags flags) {
        _cleanup_free_ char *s = NULL;
        size_t sz = 0;
        char quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        char c;
        整数 r;

        assert(p);
        assert(ret);
        assert(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        如果 (!*p)
                goto finish;
        c = **p;

        如果 (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        如果 (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                如果 (!GREEDY_REALLOC(s, sz+1))
                        返回 -ENOMEM;

        为 (;; (*p)++, c = **p) {
                如果 (c == 0)
                        goto finish_force_terminate;
                否则 如果 (strchr(separators, c)) {
                        如果 (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                如果 (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                goto finish_force_next;
                        }
                } 否则 {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        如果 (!GREEDY_REALLOC(s, sz+1))
                                返回 -ENOMEM;
                        break;
                }
        }

        为 (;; (*p)++, c = **p) {
                如果 (backslash) {
                        如果 (!GREEDY_REALLOC(s, sz+7))
                                返回 -ENOMEM;

                        如果 (c == 0) {
                                如果 ((flags & EXTRACT_UNESCAPE_RELAX) &&
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
                                如果 (flags & EXTRACT_RELAX)
                                        goto finish_force_terminate;
                                返回 -EINVAL;
                        }

                        如果 (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                如果 ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        assert(r >= 1);

                                        (*p) += r - 1;

                                        如果 (eight_bit)
                                                s[sz++] = u;
                                        否则
                                                sz += utf8_encode_unichar(s + sz, u);
                                } 否则 如果 ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                否则 如果 (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } 否则
                                        返回 -EINVAL;
                        } 否则
                                s[sz++] = c;

                        backslash = false;

                } 否则 如果 (quote != 0) {     /* inside either single or double quotes */
                        为 (;; (*p)++, c = **p) {
                                如果 (c == 0) {
                                        如果 (flags & EXTRACT_RELAX)
                                                goto finish_force_terminate;
                                        返回 -EINVAL;
                                } 否则 如果 (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        如果 (flags & EXTRACT_UNQUOTE)
                                                break;
                                } 否则 如果 (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        break;
                                }

                                如果 (!GREEDY_REALLOC(s, sz+2))
                                        返回 -ENOMEM;

                                s[sz++] = c;

                                如果 (quote == 0)
                                        break;
                        }

                } 否则 {
                        为 (;; (*p)++, c = **p) {
                                如果 (c == 0)
                                        goto finish_force_terminate;
                                否则 如果 (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        如果 (flags & EXTRACT_UNQUOTE)
                                                break;
                                } 否则 如果 (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        break;
                                } 否则 如果 (strchr(separators, c)) {
                                        如果 (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                如果 (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                goto finish_force_next;
                                        }
                                        如果 (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                为 (;; (*p)++, c = **p) {
                                                        如果 (c == 0)
                                                                goto finish_force_terminate;
                                                        如果 (!strchr(separators, c))
                                                                break;
                                                }
                                        goto finish;

                                }

                                如果 (!GREEDY_REALLOC(s, sz+2))
                                        返回 -ENOMEM;

                                s[sz++] = c;

                                如果 (quote != 0)
                                        break;
                        }
                }
        }

finish_force_terminate:
        *p = NULL;
finish:
        如果 (!s) {
                *p = NULL;
                *ret = NULL;
                返回 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        返回 1;
}

整数 extract_first_word_and_warn(
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
        整数 r;

        assert(p);
        assert(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        如果 (r >= 0)
                返回 r;

        如果 (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                如果 (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, line, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        返回 r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                如果 (r == -EINVAL)
                        返回 log_syntax(unit, LOG_ERR, filename, line, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        返回 log_syntax(unit, LOG_ERR, filename, line, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(sizeof(enum ExtractFlags) <= sizeof(unsigned));

整数 extract_many_words_internal(const char **p, const char *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        整数 r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        assert(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        当 (va_arg(ap, char**))
                n++;
        va_end(ap);

        如果 (n == 0)
                返回 0;

        /* Read all words into a temporary array */
        char **l = newa0(char*, n);
        unsigned c;

        为 (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                如果 (r < 0) {
                        free_many_charp(l, c);
                        返回 r;
                }
                如果 (r == 0)
                        break;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                char **v = ASSERT_PTR(va_arg(ap, char**));
                *v = *i;
        }
        va_end(ap);

        返回 c;
}
