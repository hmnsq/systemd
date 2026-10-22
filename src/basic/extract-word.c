/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "utf8.h"

पूर्णाङ्क extract_first_word(अचर वर्ण **p, वर्ण **ret, अचर वर्ण *separators, ExtractFlags flags) {
        _cleanup_free_ वर्ण *s = रिक्त;
        माप_प्रकार sz = 0;
        वर्ण quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        वर्ण c;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);
        प्रतिज्ञा(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        यदि (!*p)
                गच्छ finish;
        c = **p;

        यदि (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        यदि (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                यदि (!GREEDY_REALLOC(s, sz+1))
                        फर्काऊ -ENOMEM;

        लागि (;; (*p)++, c = **p) {
                यदि (c == 0)
                        गच्छ finish_force_terminate;
                अन्यथा यदि (म_अक्षर(separators, c)) {
                        यदि (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                यदि (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                गच्छ finish_force_next;
                        }
                } अन्यथा {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        यदि (!GREEDY_REALLOC(s, sz+1))
                                फर्काऊ -ENOMEM;
                        रोक;
                }
        }

        लागि (;; (*p)++, c = **p) {
                यदि (backslash) {
                        यदि (!GREEDY_REALLOC(s, sz+7))
                                फर्काऊ -ENOMEM;

                        यदि (c == 0) {
                                यदि ((flags & EXTRACT_UNESCAPE_RELAX) &&
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
                                यदि (flags & EXTRACT_RELAX)
                                        गच्छ finish_force_terminate;
                                फर्काऊ -EINVAL;
                        }

                        यदि (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                यदि ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        प्रतिज्ञा(r >= 1);

                                        (*p) += r - 1;

                                        यदि (eight_bit)
                                                s[sz++] = u;
                                        अन्यथा
                                                sz += utf8_encode_unichar(s + sz, u);
                                } अन्यथा यदि ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (म_अक्षर(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                अन्यथा यदि (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } अन्यथा
                                        फर्काऊ -EINVAL;
                        } अन्यथा
                                s[sz++] = c;

                        backslash = false;

                } अन्यथा यदि (quote != 0) {     /* inside either single or double quotes */
                        लागि (;; (*p)++, c = **p) {
                                यदि (c == 0) {
                                        यदि (flags & EXTRACT_RELAX)
                                                गच्छ finish_force_terminate;
                                        फर्काऊ -EINVAL;
                                } अन्यथा यदि (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        यदि (flags & EXTRACT_UNQUOTE)
                                                रोक;
                                } अन्यथा यदि (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        रोक;
                                }

                                यदि (!GREEDY_REALLOC(s, sz+2))
                                        फर्काऊ -ENOMEM;

                                s[sz++] = c;

                                यदि (quote == 0)
                                        रोक;
                        }

                } अन्यथा {
                        लागि (;; (*p)++, c = **p) {
                                यदि (c == 0)
                                        गच्छ finish_force_terminate;
                                अन्यथा यदि (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        यदि (flags & EXTRACT_UNQUOTE)
                                                रोक;
                                } अन्यथा यदि (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        रोक;
                                } अन्यथा यदि (म_अक्षर(separators, c)) {
                                        यदि (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                यदि (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                गच्छ finish_force_next;
                                        }
                                        यदि (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                लागि (;; (*p)++, c = **p) {
                                                        यदि (c == 0)
                                                                गच्छ finish_force_terminate;
                                                        यदि (!म_अक्षर(separators, c))
                                                                रोक;
                                                }
                                        गच्छ finish;

                                }

                                यदि (!GREEDY_REALLOC(s, sz+2))
                                        फर्काऊ -ENOMEM;

                                s[sz++] = c;

                                यदि (quote != 0)
                                        रोक;
                        }
                }
        }

finish_force_terminate:
        *p = रिक्त;
finish:
        यदि (!s) {
                *p = रिक्त;
                *ret = रिक्त;
                फर्काऊ 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        फर्काऊ 1;
}

पूर्णाङ्क extract_first_word_and_warn(
                अचर वर्ण **p,
                वर्ण **ret,
                अचर वर्ण *separators,
                ExtractFlags flags,
                अचर वर्ण *unit,
                अचर वर्ण *filename,
                अचिन्हित पङ्क्ति,
                अचर वर्ण *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        अचर वर्ण *save;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        यदि (r >= 0)
                फर्काऊ r;

        यदि (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                यदि (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, पङ्क्ति, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        फर्काऊ r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                यदि (r == -EINVAL)
                        फर्काऊ log_syntax(unit, LOG_ERR, filename, पङ्क्ति, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        फर्काऊ log_syntax(unit, LOG_ERR, filename, पङ्क्ति, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(परिमाण(गणना ExtractFlags) <= परिमाण(अचिन्हित));

पूर्णाङ्क extract_many_शब्दहरू_internal(अचर वर्ण **p, अचर वर्ण *separators, अचिन्हित flags, ...) {
        बहु_सूची ap;
        अचिन्हित n = 0;
        पूर्णाङ्क r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        प्रतिज्ञा(p);

        /* Count how many words are expected */
        बहु_शुरू(ap, flags);
        जबसम्म (बहु_तर्क(ap, वर्ण**))
                n++;
        बहु_पूर्ण(ap);

        यदि (n == 0)
                फर्काऊ 0;

        /* Read all words into a temporary array */
        वर्ण **l = newa0(वर्ण*, n);
        अचिन्हित c;

        लागि (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                यदि (r < 0) {
                        free_many_charp(l, c);
                        फर्काऊ r;
                }
                यदि (r == 0)
                        रोक;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        बहु_शुरू(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                वर्ण **v = ASSERT_PTR(बहु_तर्क(ap, वर्ण**));
                *v = *i;
        }
        बहु_पूर्ण(ap);

        फर्काऊ c;
}
