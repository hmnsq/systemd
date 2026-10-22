/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "escape.h"
#समावेश "extract-word.h"
#समावेश "log.h"
#समावेश "string-util.h"
#समावेश "utf8.h"

पूर्णांक extract_first_word(स्थिर अक्षर **p, अक्षर **ret, स्थिर अक्षर *separators, ExtractFlags flags) {
        _cleanup_free_ अक्षर *s = शून्य;
        माप_प्रकार sz = 0;
        अक्षर quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        अक्षर c;
        पूर्णांक r;

        निश्चित(p);
        निश्चित(ret);
        निश्चित(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        अगर (!*p)
                जाओ finish;
        c = **p;

        अगर (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        अगर (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                अगर (!GREEDY_REALLOC(s, sz+1))
                        वापस -ENOMEM;

        क्रम (;; (*p)++, c = **p) {
                अगर (c == 0)
                        जाओ finish_force_terminate;
                अन्यथा अगर (म_अक्षर(separators, c)) {
                        अगर (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                अगर (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                जाओ finish_force_अग्र;
                        }
                } अन्यथा {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        अगर (!GREEDY_REALLOC(s, sz+1))
                                वापस -ENOMEM;
                        अवरोध;
                }
        }

        क्रम (;; (*p)++, c = **p) {
                अगर (backslash) {
                        अगर (!GREEDY_REALLOC(s, sz+7))
                                वापस -ENOMEM;

                        अगर (c == 0) {
                                अगर ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        जाओ finish_force_terminate;
                                }
                                अगर (flags & EXTRACT_RELAX)
                                        जाओ finish_force_terminate;
                                वापस -EINVAL;
                        }

                        अगर (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                अगर ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        निश्चित(r >= 1);

                                        (*p) += r - 1;

                                        अगर (eight_bit)
                                                s[sz++] = u;
                                        अन्यथा
                                                sz += utf8_encode_unichar(s + sz, u);
                                } अन्यथा अगर ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (म_अक्षर(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                अन्यथा अगर (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } अन्यथा
                                        वापस -EINVAL;
                        } अन्यथा
                                s[sz++] = c;

                        backslash = false;

                } अन्यथा अगर (quote != 0) {     /* inside either single or double quotes */
                        क्रम (;; (*p)++, c = **p) {
                                अगर (c == 0) {
                                        अगर (flags & EXTRACT_RELAX)
                                                जाओ finish_force_terminate;
                                        वापस -EINVAL;
                                } अन्यथा अगर (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        अगर (flags & EXTRACT_UNQUOTE)
                                                अवरोध;
                                } अन्यथा अगर (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        अवरोध;
                                }

                                अगर (!GREEDY_REALLOC(s, sz+2))
                                        वापस -ENOMEM;

                                s[sz++] = c;

                                अगर (quote == 0)
                                        अवरोध;
                        }

                } अन्यथा {
                        क्रम (;; (*p)++, c = **p) {
                                अगर (c == 0)
                                        जाओ finish_force_terminate;
                                अन्यथा अगर (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        अगर (flags & EXTRACT_UNQUOTE)
                                                अवरोध;
                                } अन्यथा अगर (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        अवरोध;
                                } अन्यथा अगर (म_अक्षर(separators, c)) {
                                        अगर (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                अगर (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                जाओ finish_force_अग्र;
                                        }
                                        अगर (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                क्रम (;; (*p)++, c = **p) {
                                                        अगर (c == 0)
                                                                जाओ finish_force_terminate;
                                                        अगर (!म_अक्षर(separators, c))
                                                                अवरोध;
                                                }
                                        जाओ finish;

                                }

                                अगर (!GREEDY_REALLOC(s, sz+2))
                                        वापस -ENOMEM;

                                s[sz++] = c;

                                अगर (quote != 0)
                                        अवरोध;
                        }
                }
        }

finish_force_terminate:
        *p = शून्य;
finish:
        अगर (!s) {
                *p = शून्य;
                *ret = शून्य;
                वापस 0;
        }

finish_force_अग्र:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        वापस 1;
}

पूर्णांक extract_first_word_and_warn(
                स्थिर अक्षर **p,
                अक्षर **ret,
                स्थिर अक्षर *separators,
                ExtractFlags flags,
                स्थिर अक्षर *unit,
                स्थिर अक्षर *filename,
                अचिन्हित पंक्ति,
                स्थिर अक्षर *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        स्थिर अक्षर *save;
        पूर्णांक r;

        निश्चित(p);
        निश्चित(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        अगर (r >= 0)
                वापस r;

        अगर (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                अगर (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, पंक्ति, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        वापस r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                अगर (r == -EINVAL)
                        वापस log_syntax(unit, LOG_ERR, filename, पंक्ति, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        वापस log_syntax(unit, LOG_ERR, filename, पंक्ति, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(माप(क्रमागत ExtractFlags) <= माप(अचिन्हित));

पूर्णांक extract_many_शब्दगण_internal(स्थिर अक्षर **p, स्थिर अक्षर *separators, अचिन्हित flags, ...) {
        बहु_सूची ap;
        अचिन्हित n = 0;
        पूर्णांक r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        निश्चित(p);

        /* Count how many words are expected */
        बहु_शुरू(ap, flags);
        जबतक (बहु_तर्क(ap, अक्षर**))
                n++;
        बहु_पूर्ण(ap);

        अगर (n == 0)
                वापस 0;

        /* Read all words into a temporary array */
        अक्षर **l = newa0(अक्षर*, n);
        अचिन्हित c;

        क्रम (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                अगर (r < 0) {
                        free_many_charp(l, c);
                        वापस r;
                }
                अगर (r == 0)
                        अवरोध;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        बहु_शुरू(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                अक्षर **v = ASSERT_PTR(बहु_तर्क(ap, अक्षर**));
                *v = *i;
        }
        बहु_पूर्ण(ap);

        वापस c;
}
