/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା extract_first_word(ଅଚଳ ଅକ୍ଷର **p, ଅକ୍ଷର **ret, ଅଚଳ ଅକ୍ଷର *separators, ExtractFlags flags) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        size_t sz = 0;
        ଅକ୍ଷର quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        ଅକ୍ଷର c;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        ଯଦି (!*p)
                ଯାଅ finish;
        c = **p;

        ଯଦି (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        ଯଦି (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                ଯଦି (!GREEDY_REALLOC(s, sz+1))
                        ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (;; (*p)++, c = **p) {
                ଯଦି (c == 0)
                        ଯାଅ finish_force_terminate;
                ଅନ୍ୟଥା ଯଦି (strchr(separators, c)) {
                        ଯଦି (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                ଯଦି (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                ଯାଅ finish_force_next;
                        }
                } ଅନ୍ୟଥା {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        ଯଦି (!GREEDY_REALLOC(s, sz+1))
                                ଫେରନ୍ତୁ -ENOMEM;
                        ଭାଙ୍ଗ;
                }
        }

        ପାଇଁ (;; (*p)++, c = **p) {
                ଯଦି (backslash) {
                        ଯଦି (!GREEDY_REALLOC(s, sz+7))
                                ଫେରନ୍ତୁ -ENOMEM;

                        ଯଦି (c == 0) {
                                ଯଦି ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        ଯାଅ finish_force_terminate;
                                }
                                ଯଦି (flags & EXTRACT_RELAX)
                                        ଯାଅ finish_force_terminate;
                                ଫେରନ୍ତୁ -EINVAL;
                        }

                        ଯଦି (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                ଯଦି ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        ପ୍ରତିଜ୍ଞା(r >= 1);

                                        (*p) += r - 1;

                                        ଯଦି (eight_bit)
                                                s[sz++] = u;
                                        ଅନ୍ୟଥା
                                                sz += utf8_encode_unichar(s + sz, u);
                                } ଅନ୍ୟଥା ଯଦି ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                ଅନ୍ୟଥା ଯଦି (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } ଅନ୍ୟଥା
                                        ଫେରନ୍ତୁ -EINVAL;
                        } ଅନ୍ୟଥା
                                s[sz++] = c;

                        backslash = false;

                } ଅନ୍ୟଥା ଯଦି (quote != 0) {     /* inside either single or double quotes */
                        ପାଇଁ (;; (*p)++, c = **p) {
                                ଯଦି (c == 0) {
                                        ଯଦି (flags & EXTRACT_RELAX)
                                                ଯାଅ finish_force_terminate;
                                        ଫେରନ୍ତୁ -EINVAL;
                                } ଅନ୍ୟଥା ଯଦି (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        ଯଦି (flags & EXTRACT_UNQUOTE)
                                                ଭାଙ୍ଗ;
                                } ଅନ୍ୟଥା ଯଦି (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ଭାଙ୍ଗ;
                                }

                                ଯଦି (!GREEDY_REALLOC(s, sz+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                s[sz++] = c;

                                ଯଦି (quote == 0)
                                        ଭାଙ୍ଗ;
                        }

                } ଅନ୍ୟଥା {
                        ପାଇଁ (;; (*p)++, c = **p) {
                                ଯଦି (c == 0)
                                        ଯାଅ finish_force_terminate;
                                ଅନ୍ୟଥା ଯଦି (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        ଯଦି (flags & EXTRACT_UNQUOTE)
                                                ଭାଙ୍ଗ;
                                } ଅନ୍ୟଥା ଯଦି (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ଭାଙ୍ଗ;
                                } ଅନ୍ୟଥା ଯଦି (strchr(separators, c)) {
                                        ଯଦି (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                ଯଦି (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                ଯାଅ finish_force_next;
                                        }
                                        ଯଦି (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                ପାଇଁ (;; (*p)++, c = **p) {
                                                        ଯଦି (c == 0)
                                                                ଯାଅ finish_force_terminate;
                                                        ଯଦି (!strchr(separators, c))
                                                                ଭାଙ୍ଗ;
                                                }
                                        ଯାଅ finish;

                                }

                                ଯଦି (!GREEDY_REALLOC(s, sz+2))
                                        ଫେରନ୍ତୁ -ENOMEM;

                                s[sz++] = c;

                                ଯଦି (quote != 0)
                                        ଭାଙ୍ଗ;
                        }
                }
        }

finish_force_terminate:
        *p = ଖାଲି;
finish:
        ଯଦି (!s) {
                *p = ଖାଲି;
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା extract_first_word_and_warn(
                ଅଚଳ ଅକ୍ଷର **p,
                ଅକ୍ଷର **ret,
                ଅଚଳ ଅକ୍ଷର *separators,
                ExtractFlags flags,
                ଅଚଳ ଅକ୍ଷର *unit,
                ଅଚଳ ଅକ୍ଷର *filename,
                unsigned ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ଅଚଳ ଅକ୍ଷର *save;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                ଯଦି (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, ଧାଡ଼ି, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        ଫେରନ୍ତୁ r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                ଯଦି (r == -EINVAL)
                        ଫେରନ୍ତୁ log_syntax(unit, LOG_ERR, filename, ଧାଡ଼ି, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        ଫେରନ୍ତୁ log_syntax(unit, LOG_ERR, filename, ଧାଡ଼ି, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(ଆକାର(ଗଣନା ExtractFlags) <= ଆକାର(unsigned));

ପୂର୍ଣ୍ଣସଂଖ୍ୟା extract_many_ଶବ୍ଦଗୁଡ଼ିକ_internal(ଅଚଳ ଅକ୍ଷର **p, ଅଚଳ ଅକ୍ଷର *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        ପ୍ରତିଜ୍ଞା(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        ଯେତେବେଳେ (va_arg(ap, ଅକ୍ଷର**))
                n++;
        va_end(ap);

        ଯଦି (n == 0)
                ଫେରନ୍ତୁ 0;

        /* Read all words into a temporary array */
        ଅକ୍ଷର **l = newa0(ଅକ୍ଷର*, n);
        unsigned c;

        ପାଇଁ (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                ଯଦି (r < 0) {
                        free_many_charp(l, c);
                        ଫେରନ୍ତୁ r;
                }
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                ଅକ୍ଷର **v = ASSERT_PTR(va_arg(ap, ଅକ୍ଷର**));
                *v = *i;
        }
        va_end(ap);

        ଫେରନ୍ତୁ c;
}
