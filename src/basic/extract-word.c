/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "escape.h"
#include "extract-word.h"
#include "log.h"
#include "string-util.h"
#include "utf8.h"

정수 extract_first_word(const char **p, char **ret, const char *separators, ExtractFlags flags) {
        _cleanup_free_ char *s = NULL;
        size_t sz = 0;
        char quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        char c;
        정수 r;

        assert(p);
        assert(ret);
        assert(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        만약 (!*p)
                goto finish;
        c = **p;

        만약 (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        만약 (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                만약 (!GREEDY_REALLOC(s, sz+1))
                        반환 -ENOMEM;

        위해 (;; (*p)++, c = **p) {
                만약 (c == 0)
                        goto finish_force_terminate;
                아니면 만약 (strchr(separators, c)) {
                        만약 (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                만약 (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                goto finish_force_next;
                        }
                } 아니면 {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        만약 (!GREEDY_REALLOC(s, sz+1))
                                반환 -ENOMEM;
                        break;
                }
        }

        위해 (;; (*p)++, c = **p) {
                만약 (backslash) {
                        만약 (!GREEDY_REALLOC(s, sz+7))
                                반환 -ENOMEM;

                        만약 (c == 0) {
                                만약 ((flags & EXTRACT_UNESCAPE_RELAX) &&
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
                                만약 (flags & EXTRACT_RELAX)
                                        goto finish_force_terminate;
                                반환 -EINVAL;
                        }

                        만약 (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                만약 ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        assert(r >= 1);

                                        (*p) += r - 1;

                                        만약 (eight_bit)
                                                s[sz++] = u;
                                        아니면
                                                sz += utf8_encode_unichar(s + sz, u);
                                } 아니면 만약 ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                아니면 만약 (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } 아니면
                                        반환 -EINVAL;
                        } 아니면
                                s[sz++] = c;

                        backslash = false;

                } 아니면 만약 (quote != 0) {     /* inside either single or double quotes */
                        위해 (;; (*p)++, c = **p) {
                                만약 (c == 0) {
                                        만약 (flags & EXTRACT_RELAX)
                                                goto finish_force_terminate;
                                        반환 -EINVAL;
                                } 아니면 만약 (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        만약 (flags & EXTRACT_UNQUOTE)
                                                break;
                                } 아니면 만약 (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        break;
                                }

                                만약 (!GREEDY_REALLOC(s, sz+2))
                                        반환 -ENOMEM;

                                s[sz++] = c;

                                만약 (quote == 0)
                                        break;
                        }

                } 아니면 {
                        위해 (;; (*p)++, c = **p) {
                                만약 (c == 0)
                                        goto finish_force_terminate;
                                아니면 만약 (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        만약 (flags & EXTRACT_UNQUOTE)
                                                break;
                                } 아니면 만약 (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        break;
                                } 아니면 만약 (strchr(separators, c)) {
                                        만약 (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                만약 (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                goto finish_force_next;
                                        }
                                        만약 (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                위해 (;; (*p)++, c = **p) {
                                                        만약 (c == 0)
                                                                goto finish_force_terminate;
                                                        만약 (!strchr(separators, c))
                                                                break;
                                                }
                                        goto finish;

                                }

                                만약 (!GREEDY_REALLOC(s, sz+2))
                                        반환 -ENOMEM;

                                s[sz++] = c;

                                만약 (quote != 0)
                                        break;
                        }
                }
        }

finish_force_terminate:
        *p = NULL;
finish:
        만약 (!s) {
                *p = NULL;
                *ret = NULL;
                반환 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        반환 1;
}

정수 extract_first_word_and_warn(
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
        정수 r;

        assert(p);
        assert(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        만약 (r >= 0)
                반환 r;

        만약 (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                만약 (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, line, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        반환 r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                만약 (r == -EINVAL)
                        반환 log_syntax(unit, LOG_ERR, filename, line, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        반환 log_syntax(unit, LOG_ERR, filename, line, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(sizeof(enum ExtractFlags) <= sizeof(unsigned));

정수 extract_many_words_internal(const char **p, const char *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        정수 r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        assert(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        동안 (va_arg(ap, char**))
                n++;
        va_end(ap);

        만약 (n == 0)
                반환 0;

        /* Read all words into a temporary array */
        char **l = newa0(char*, n);
        unsigned c;

        위해 (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                만약 (r < 0) {
                        free_many_charp(l, c);
                        반환 r;
                }
                만약 (r == 0)
                        break;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                char **v = ASSERT_PTR(va_arg(ap, char**));
                *v = *i;
        }
        va_end(ap);

        반환 c;
}
