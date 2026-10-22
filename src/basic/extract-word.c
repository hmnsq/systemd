/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "utf8.h"

पूर्णाङ्क extract_first_word(ध्रुव वर्ण **p, वर्ण **ret, ध्रुव वर्ण *separators, ExtractFlags flags) {
        _cleanup_free_ वर्ण *s = रिक्त;
        size_t sz = 0;
        वर्ण quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        वर्ण c;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);
        प्रतिज्ञा(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        सचे (!*p)
                गच्छ finish;
        c = **p;

        सचे (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        सचे (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                सचे (!GREEDY_REALLOC(s, sz+1))
                        पटिनिवत्त -ENOMEM;

        अत्थाय (;; (*p)++, c = **p) {
                सचे (c == 0)
                        गच्छ finish_force_terminate;
                अञ्ञथा सचे (strchr(separators, c)) {
                        सचे (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                सचे (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                गच्छ finish_force_next;
                        }
                } अञ्ञथा {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        सचे (!GREEDY_REALLOC(s, sz+1))
                                पटिनिवत्त -ENOMEM;
                        भङ्ग;
                }
        }

        अत्थाय (;; (*p)++, c = **p) {
                सचे (backslash) {
                        सचे (!GREEDY_REALLOC(s, sz+7))
                                पटिनिवत्त -ENOMEM;

                        सचे (c == 0) {
                                सचे ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        गच्छ finish_force_terminate;
                                }
                                सचे (flags & EXTRACT_RELAX)
                                        गच्छ finish_force_terminate;
                                पटिनिवत्त -EINVAL;
                        }

                        सचे (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                सचे ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        प्रतिज्ञा(r >= 1);

                                        (*p) += r - 1;

                                        सचे (eight_bit)
                                                s[sz++] = u;
                                        अञ्ञथा
                                                sz += utf8_encode_unichar(s + sz, u);
                                } अञ्ञथा सचे ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                अञ्ञथा सचे (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } अञ्ञथा
                                        पटिनिवत्त -EINVAL;
                        } अञ्ञथा
                                s[sz++] = c;

                        backslash = false;

                } अञ्ञथा सचे (quote != 0) {     /* inside either single or double quotes */
                        अत्थाय (;; (*p)++, c = **p) {
                                सचे (c == 0) {
                                        सचे (flags & EXTRACT_RELAX)
                                                गच्छ finish_force_terminate;
                                        पटिनिवत्त -EINVAL;
                                } अञ्ञथा सचे (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        सचे (flags & EXTRACT_UNQUOTE)
                                                भङ्ग;
                                } अञ्ञथा सचे (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        भङ्ग;
                                }

                                सचे (!GREEDY_REALLOC(s, sz+2))
                                        पटिनिवत्त -ENOMEM;

                                s[sz++] = c;

                                सचे (quote == 0)
                                        भङ्ग;
                        }

                } अञ्ञथा {
                        अत्थाय (;; (*p)++, c = **p) {
                                सचे (c == 0)
                                        गच्छ finish_force_terminate;
                                अञ्ञथा सचे (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        सचे (flags & EXTRACT_UNQUOTE)
                                                भङ्ग;
                                } अञ्ञथा सचे (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        भङ्ग;
                                } अञ्ञथा सचे (strchr(separators, c)) {
                                        सचे (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                सचे (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                गच्छ finish_force_next;
                                        }
                                        सचे (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                अत्थाय (;; (*p)++, c = **p) {
                                                        सचे (c == 0)
                                                                गच्छ finish_force_terminate;
                                                        सचे (!strchr(separators, c))
                                                                भङ्ग;
                                                }
                                        गच्छ finish;

                                }

                                सचे (!GREEDY_REALLOC(s, sz+2))
                                        पटिनिवत्त -ENOMEM;

                                s[sz++] = c;

                                सचे (quote != 0)
                                        भङ्ग;
                        }
                }
        }

finish_force_terminate:
        *p = रिक्त;
finish:
        सचे (!s) {
                *p = रिक्त;
                *ret = रिक्त;
                पटिनिवत्त 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        पटिनिवत्त 1;
}

पूर्णाङ्क extract_first_word_and_warn(
                ध्रुव वर्ण **p,
                वर्ण **ret,
                ध्रुव वर्ण *separators,
                ExtractFlags flags,
                ध्रुव वर्ण *unit,
                ध्रुव वर्ण *filename,
                unsigned पङ्क्तिः,
                ध्रुव वर्ण *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ध्रुव वर्ण *save;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        सचे (r >= 0)
                पटिनिवत्त r;

        सचे (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                सचे (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, पङ्क्तिः, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        पटिनिवत्त r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                सचे (r == -EINVAL)
                        पटिनिवत्त log_syntax(unit, LOG_ERR, filename, पङ्क्तिः, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        पटिनिवत्त log_syntax(unit, LOG_ERR, filename, पङ्क्तिः, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(परिमाण(गणना ExtractFlags) <= परिमाण(unsigned));

पूर्णाङ्क extract_many_शब्दाः_internal(ध्रुव वर्ण **p, ध्रुव वर्ण *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        पूर्णाङ्क r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        प्रतिज्ञा(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        याव (va_arg(ap, वर्ण**))
                n++;
        va_end(ap);

        सचे (n == 0)
                पटिनिवत्त 0;

        /* Read all words into a temporary array */
        वर्ण **l = newa0(वर्ण*, n);
        unsigned c;

        अत्थाय (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                सचे (r < 0) {
                        free_many_charp(l, c);
                        पटिनिवत्त r;
                }
                सचे (r == 0)
                        भङ्ग;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                वर्ण **v = ASSERT_PTR(va_arg(ap, वर्ण**));
                *v = *i;
        }
        va_end(ap);

        पटिनिवत्त c;
}
