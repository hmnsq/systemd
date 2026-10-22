/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "escape.h"
#include "extract-word.h"
#include "log.h"
#include "string-util.h"
#include "utf8.h"

ціле extract_first_word(const char **p, char **ret, const char *separators, ExtractFlags flags) {
        _cleanup_free_ char *s = NULL;
        size_t sz = 0;
        char quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        char c;
        ціле r;

        assert(p);
        assert(ret);
        assert(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        якщо (!*p)
                goto finish;
        c = **p;

        якщо (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        якщо (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                якщо (!GREEDY_REALLOC(s, sz+1))
                        повернути -ENOMEM;

        для (;; (*p)++, c = **p) {
                якщо (c == 0)
                        goto finish_force_terminate;
                інакше якщо (strchr(separators, c)) {
                        якщо (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                якщо (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                goto finish_force_next;
                        }
                } інакше {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        якщо (!GREEDY_REALLOC(s, sz+1))
                                повернути -ENOMEM;
                        break;
                }
        }

        для (;; (*p)++, c = **p) {
                якщо (backslash) {
                        якщо (!GREEDY_REALLOC(s, sz+7))
                                повернути -ENOMEM;

                        якщо (c == 0) {
                                якщо ((flags & EXTRACT_UNESCAPE_RELAX) &&
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
                                якщо (flags & EXTRACT_RELAX)
                                        goto finish_force_terminate;
                                повернути -EINVAL;
                        }

                        якщо (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                якщо ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        assert(r >= 1);

                                        (*p) += r - 1;

                                        якщо (eight_bit)
                                                s[sz++] = u;
                                        інакше
                                                sz += utf8_encode_unichar(s + sz, u);
                                } інакше якщо ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                інакше якщо (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } інакше
                                        повернути -EINVAL;
                        } інакше
                                s[sz++] = c;

                        backslash = false;

                } інакше якщо (quote != 0) {     /* inside either single or double quotes */
                        для (;; (*p)++, c = **p) {
                                якщо (c == 0) {
                                        якщо (flags & EXTRACT_RELAX)
                                                goto finish_force_terminate;
                                        повернути -EINVAL;
                                } інакше якщо (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        якщо (flags & EXTRACT_UNQUOTE)
                                                break;
                                } інакше якщо (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        break;
                                }

                                якщо (!GREEDY_REALLOC(s, sz+2))
                                        повернути -ENOMEM;

                                s[sz++] = c;

                                якщо (quote == 0)
                                        break;
                        }

                } інакше {
                        для (;; (*p)++, c = **p) {
                                якщо (c == 0)
                                        goto finish_force_terminate;
                                інакше якщо (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        якщо (flags & EXTRACT_UNQUOTE)
                                                break;
                                } інакше якщо (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        break;
                                } інакше якщо (strchr(separators, c)) {
                                        якщо (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                якщо (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                goto finish_force_next;
                                        }
                                        якщо (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                для (;; (*p)++, c = **p) {
                                                        якщо (c == 0)
                                                                goto finish_force_terminate;
                                                        якщо (!strchr(separators, c))
                                                                break;
                                                }
                                        goto finish;

                                }

                                якщо (!GREEDY_REALLOC(s, sz+2))
                                        повернути -ENOMEM;

                                s[sz++] = c;

                                якщо (quote != 0)
                                        break;
                        }
                }
        }

finish_force_terminate:
        *p = NULL;
finish:
        якщо (!s) {
                *p = NULL;
                *ret = NULL;
                повернути 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        повернути 1;
}

ціле extract_first_word_and_warn(
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
        ціле r;

        assert(p);
        assert(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        якщо (r >= 0)
                повернути r;

        якщо (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                якщо (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, line, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        повернути r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                якщо (r == -EINVAL)
                        повернути log_syntax(unit, LOG_ERR, filename, line, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        повернути log_syntax(unit, LOG_ERR, filename, line, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(sizeof(enum ExtractFlags) <= sizeof(unsigned));

ціле extract_many_words_internal(const char **p, const char *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        ціле r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        assert(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        поки (va_arg(ap, char**))
                n++;
        va_end(ap);

        якщо (n == 0)
                повернути 0;

        /* Read all words into a temporary array */
        char **l = newa0(char*, n);
        unsigned c;

        для (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                якщо (r < 0) {
                        free_many_charp(l, c);
                        повернути r;
                }
                якщо (r == 0)
                        break;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                char **v = ASSERT_PTR(va_arg(ap, char**));
                *v = *i;
        }
        va_end(ap);

        повернути c;
}
