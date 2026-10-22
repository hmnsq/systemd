/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "utf8.h"

पूर्णांक extract_first_word(स्थिरांक अक्षर **p, अक्षर **ret, स्थिरांक अक्षर *separators, ExtractFlags flags) {
        _cleanup_free_ अक्षर *s = रिक्त;
        माप_प्रकार sz = 0;
        अक्षर quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        अक्षर c;
        पूर्णांक r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);
        प्रतिज्ञा(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        जर (!*p)
                जा finish;
        c = **p;

        जर (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        जर (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                जर (!GREEDY_REALLOC(s, sz+1))
                        परत -ENOMEM;

        साठी (;; (*p)++, c = **p) {
                जर (c == 0)
                        जा finish_force_terminate;
                अन्यथा जर (म_अक्षर(separators, c)) {
                        जर (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                जर (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                जा finish_force_next;
                        }
                } अन्यथा {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        जर (!GREEDY_REALLOC(s, sz+1))
                                परत -ENOMEM;
                        थांबा;
                }
        }

        साठी (;; (*p)++, c = **p) {
                जर (backslash) {
                        जर (!GREEDY_REALLOC(s, sz+7))
                                परत -ENOMEM;

                        जर (c == 0) {
                                जर ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        जा finish_force_terminate;
                                }
                                जर (flags & EXTRACT_RELAX)
                                        जा finish_force_terminate;
                                परत -EINVAL;
                        }

                        जर (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                जर ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        प्रतिज्ञा(r >= 1);

                                        (*p) += r - 1;

                                        जर (eight_bit)
                                                s[sz++] = u;
                                        अन्यथा
                                                sz += utf8_encode_unichar(s + sz, u);
                                } अन्यथा जर ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (म_अक्षर(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                अन्यथा जर (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } अन्यथा
                                        परत -EINVAL;
                        } अन्यथा
                                s[sz++] = c;

                        backslash = false;

                } अन्यथा जर (quote != 0) {     /* inside either single or double quotes */
                        साठी (;; (*p)++, c = **p) {
                                जर (c == 0) {
                                        जर (flags & EXTRACT_RELAX)
                                                जा finish_force_terminate;
                                        परत -EINVAL;
                                } अन्यथा जर (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        जर (flags & EXTRACT_UNQUOTE)
                                                थांबा;
                                } अन्यथा जर (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        थांबा;
                                }

                                जर (!GREEDY_REALLOC(s, sz+2))
                                        परत -ENOMEM;

                                s[sz++] = c;

                                जर (quote == 0)
                                        थांबा;
                        }

                } अन्यथा {
                        साठी (;; (*p)++, c = **p) {
                                जर (c == 0)
                                        जा finish_force_terminate;
                                अन्यथा जर (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        जर (flags & EXTRACT_UNQUOTE)
                                                थांबा;
                                } अन्यथा जर (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        थांबा;
                                } अन्यथा जर (म_अक्षर(separators, c)) {
                                        जर (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                जर (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                जा finish_force_next;
                                        }
                                        जर (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                साठी (;; (*p)++, c = **p) {
                                                        जर (c == 0)
                                                                जा finish_force_terminate;
                                                        जर (!म_अक्षर(separators, c))
                                                                थांबा;
                                                }
                                        जा finish;

                                }

                                जर (!GREEDY_REALLOC(s, sz+2))
                                        परत -ENOMEM;

                                s[sz++] = c;

                                जर (quote != 0)
                                        थांबा;
                        }
                }
        }

finish_force_terminate:
        *p = रिक्त;
finish:
        जर (!s) {
                *p = रिक्त;
                *ret = रिक्त;
                परत 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        परत 1;
}

पूर्णांक extract_first_word_and_warn(
                स्थिरांक अक्षर **p,
                अक्षर **ret,
                स्थिरांक अक्षर *separators,
                ExtractFlags flags,
                स्थिरांक अक्षर *unit,
                स्थिरांक अक्षर *filename,
                अचिन्हित ओळ,
                स्थिरांक अक्षर *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        स्थिरांक अक्षर *save;
        पूर्णांक r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        जर (r >= 0)
                परत r;

        जर (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                जर (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, ओळ, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        परत r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                जर (r == -EINVAL)
                        परत log_syntax(unit, LOG_ERR, filename, ओळ, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        परत log_syntax(unit, LOG_ERR, filename, ओळ, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(आकार(गणना ExtractFlags) <= आकार(अचिन्हित));

पूर्णांक extract_many_शब्द_internal(स्थिरांक अक्षर **p, स्थिरांक अक्षर *separators, अचिन्हित flags, ...) {
        बहु_सूची ap;
        अचिन्हित n = 0;
        पूर्णांक r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        प्रतिज्ञा(p);

        /* Count how many words are expected */
        बहु_शुरू(ap, flags);
        पर्यंत (बहु_तर्क(ap, अक्षर**))
                n++;
        बहु_पूर्ण(ap);

        जर (n == 0)
                परत 0;

        /* Read all words into a temporary array */
        अक्षर **l = newa0(अक्षर*, n);
        अचिन्हित c;

        साठी (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                जर (r < 0) {
                        free_many_charp(l, c);
                        परत r;
                }
                जर (r == 0)
                        थांबा;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        बहु_शुरू(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                अक्षर **v = ASSERT_PTR(बहु_तर्क(ap, अक्षर**));
                *v = *i;
        }
        बहु_पूर्ण(ap);

        परत c;
}
