/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "escape.h"
#include "extract-word.h"
#include "log.h"
#include "string-util.h"
#include "utf8.h"

puur_naa_nka extract_first_word(const var_na **p, var_na **ret, const var_na *separators, ExtractFlags flags) {
        _cleanup_free_ var_na *s = NULL;
        size_t sz = 0;
        var_na quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        var_na c;
        puur_naa_nka r;

        assert(p);
        assert(ret);
        assert(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        ᱡᱩᱫᱤ (!*p)
                goto finish;
        c = **p;

        ᱡᱩᱫᱤ (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        ᱡᱩᱫᱤ (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                ᱡᱩᱫᱤ (!GREEDY_REALLOC(s, sz+1))
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        for (;; (*p)++, c = **p) {
                ᱡᱩᱫᱤ (c == 0)
                        goto finish_force_terminate;
                ᱵᱟᱝ ᱡᱩᱫᱤ (strchr(separators, c)) {
                        ᱡᱩᱫᱤ (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                ᱡᱩᱫᱤ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                goto finish_force_next;
                        }
                } ᱵᱟᱝ {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        ᱡᱩᱫᱤ (!GREEDY_REALLOC(s, sz+1))
                                ᱨᱩᱣᱟᱹᱲ -ENOMEM;
                        bha_nga;
                }
        }

        for (;; (*p)++, c = **p) {
                ᱡᱩᱫᱤ (backslash) {
                        ᱡᱩᱫᱤ (!GREEDY_REALLOC(s, sz+7))
                                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                        ᱡᱩᱫᱤ (c == 0) {
                                ᱡᱩᱫᱤ ((flags & EXTRACT_UNESCAPE_RELAX) &&
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
                                ᱡᱩᱫᱤ (flags & EXTRACT_RELAX)
                                        goto finish_force_terminate;
                                ᱨᱩᱣᱟᱹᱲ -EINVAL;
                        }

                        ᱡᱩᱫᱤ (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                ᱡᱩᱫᱤ ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        assert(r >= 1);

                                        (*p) += r - 1;

                                        ᱡᱩᱫᱤ (eight_bit)
                                                s[sz++] = u;
                                        ᱵᱟᱝ
                                                sz += utf8_encode_unichar(s + sz, u);
                                } ᱵᱟᱝ ᱡᱩᱫᱤ ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                ᱵᱟᱝ ᱡᱩᱫᱤ (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } ᱵᱟᱝ
                                        ᱨᱩᱣᱟᱹᱲ -EINVAL;
                        } ᱵᱟᱝ
                                s[sz++] = c;

                        backslash = false;

                } ᱵᱟᱝ ᱡᱩᱫᱤ (quote != 0) {     /* inside either single or double quotes */
                        for (;; (*p)++, c = **p) {
                                ᱡᱩᱫᱤ (c == 0) {
                                        ᱡᱩᱫᱤ (flags & EXTRACT_RELAX)
                                                goto finish_force_terminate;
                                        ᱨᱩᱣᱟᱹᱲ -EINVAL;
                                } ᱵᱟᱝ ᱡᱩᱫᱤ (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        ᱡᱩᱫᱤ (flags & EXTRACT_UNQUOTE)
                                                bha_nga;
                                } ᱵᱟᱝ ᱡᱩᱫᱤ (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        bha_nga;
                                }

                                ᱡᱩᱫᱤ (!GREEDY_REALLOC(s, sz+2))
                                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                                s[sz++] = c;

                                ᱡᱩᱫᱤ (quote == 0)
                                        bha_nga;
                        }

                } ᱵᱟᱝ {
                        for (;; (*p)++, c = **p) {
                                ᱡᱩᱫᱤ (c == 0)
                                        goto finish_force_terminate;
                                ᱵᱟᱝ ᱡᱩᱫᱤ (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        ᱡᱩᱫᱤ (flags & EXTRACT_UNQUOTE)
                                                bha_nga;
                                } ᱵᱟᱝ ᱡᱩᱫᱤ (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        bha_nga;
                                } ᱵᱟᱝ ᱡᱩᱫᱤ (strchr(separators, c)) {
                                        ᱡᱩᱫᱤ (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                ᱡᱩᱫᱤ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                goto finish_force_next;
                                        }
                                        ᱡᱩᱫᱤ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                for (;; (*p)++, c = **p) {
                                                        ᱡᱩᱫᱤ (c == 0)
                                                                goto finish_force_terminate;
                                                        ᱡᱩᱫᱤ (!strchr(separators, c))
                                                                bha_nga;
                                                }
                                        goto finish;

                                }

                                ᱡᱩᱫᱤ (!GREEDY_REALLOC(s, sz+2))
                                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                                s[sz++] = c;

                                ᱡᱩᱫᱤ (quote != 0)
                                        bha_nga;
                        }
                }
        }

finish_force_terminate:
        *p = NULL;
finish:
        ᱡᱩᱫᱤ (!s) {
                *p = NULL;
                *ret = NULL;
                ᱨᱩᱣᱟᱹᱲ 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        ᱨᱩᱣᱟᱹᱲ 1;
}

puur_naa_nka extract_first_word_and_warn(
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
        puur_naa_nka r;

        assert(p);
        assert(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        ᱡᱩᱫᱤ (r >= 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                ᱡᱩᱫᱤ (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, line, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        ᱨᱩᱣᱟᱹᱲ r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                ᱡᱩᱫᱤ (r == -EINVAL)
                        ᱨᱩᱣᱟᱹᱲ log_syntax(unit, LOG_ERR, filename, line, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        ᱨᱩᱣᱟᱹᱲ log_syntax(unit, LOG_ERR, filename, line, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(parimaa_na(ga_nanaa ExtractFlags) <= parimaa_na(unsigned));

puur_naa_nka extract_many_words_internal(const var_na **p, const var_na *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        puur_naa_nka r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        assert(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        ᱡᱚᱠᱷᱚᱱ (va_arg(ap, var_na**))
                n++;
        va_end(ap);

        ᱡᱩᱫᱤ (n == 0)
                ᱨᱩᱣᱟᱹᱲ 0;

        /* Read all words into a temporary array */
        var_na **l = newa0(var_na*, n);
        unsigned c;

        for (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                ᱡᱩᱫᱤ (r < 0) {
                        free_many_charp(l, c);
                        ᱨᱩᱣᱟᱹᱲ r;
                }
                ᱡᱩᱫᱤ (r == 0)
                        bha_nga;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                var_na **v = ASSERT_PTR(va_arg(ap, var_na**));
                *v = *i;
        }
        va_end(ap);

        ᱨᱩᱣᱟᱹᱲ c;
}
