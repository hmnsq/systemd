/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "utf8.h"

പൂർണ്ണസംഖ്യ extract_first_word(സ്ഥിരം അക്ഷരം **p, അക്ഷരം **ret, സ്ഥിരം അക്ഷരം *separators, ExtractFlags flags) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        size_t sz = 0;
        അക്ഷരം quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        അക്ഷരം c;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        എങ്കിൽ (!*p)
                പോകുക finish;
        c = **p;

        എങ്കിൽ (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        എങ്കിൽ (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                എങ്കിൽ (!GREEDY_REALLOC(s, sz+1))
                        തിരികെ -ENOMEM;

        വേണ്ടി (;; (*p)++, c = **p) {
                എങ്കിൽ (c == 0)
                        പോകുക finish_force_terminate;
                അല്ലെങ്കിൽ എങ്കിൽ (strchr(separators, c)) {
                        എങ്കിൽ (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                എങ്കിൽ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                പോകുക finish_force_next;
                        }
                } അല്ലെങ്കിൽ {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        എങ്കിൽ (!GREEDY_REALLOC(s, sz+1))
                                തിരികെ -ENOMEM;
                        നിർത്തുക;
                }
        }

        വേണ്ടി (;; (*p)++, c = **p) {
                എങ്കിൽ (backslash) {
                        എങ്കിൽ (!GREEDY_REALLOC(s, sz+7))
                                തിരികെ -ENOMEM;

                        എങ്കിൽ (c == 0) {
                                എങ്കിൽ ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        പോകുക finish_force_terminate;
                                }
                                എങ്കിൽ (flags & EXTRACT_RELAX)
                                        പോകുക finish_force_terminate;
                                തിരികെ -EINVAL;
                        }

                        എങ്കിൽ (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                എങ്കിൽ ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        പ്രതിജ്ഞാ(r >= 1);

                                        (*p) += r - 1;

                                        എങ്കിൽ (eight_bit)
                                                s[sz++] = u;
                                        അല്ലെങ്കിൽ
                                                sz += utf8_encode_unichar(s + sz, u);
                                } അല്ലെങ്കിൽ എങ്കിൽ ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                അല്ലെങ്കിൽ എങ്കിൽ (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } അല്ലെങ്കിൽ
                                        തിരികെ -EINVAL;
                        } അല്ലെങ്കിൽ
                                s[sz++] = c;

                        backslash = false;

                } അല്ലെങ്കിൽ എങ്കിൽ (quote != 0) {     /* inside either single or double quotes */
                        വേണ്ടി (;; (*p)++, c = **p) {
                                എങ്കിൽ (c == 0) {
                                        എങ്കിൽ (flags & EXTRACT_RELAX)
                                                പോകുക finish_force_terminate;
                                        തിരികെ -EINVAL;
                                } അല്ലെങ്കിൽ എങ്കിൽ (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        എങ്കിൽ (flags & EXTRACT_UNQUOTE)
                                                നിർത്തുക;
                                } അല്ലെങ്കിൽ എങ്കിൽ (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        നിർത്തുക;
                                }

                                എങ്കിൽ (!GREEDY_REALLOC(s, sz+2))
                                        തിരികെ -ENOMEM;

                                s[sz++] = c;

                                എങ്കിൽ (quote == 0)
                                        നിർത്തുക;
                        }

                } അല്ലെങ്കിൽ {
                        വേണ്ടി (;; (*p)++, c = **p) {
                                എങ്കിൽ (c == 0)
                                        പോകുക finish_force_terminate;
                                അല്ലെങ്കിൽ എങ്കിൽ (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        എങ്കിൽ (flags & EXTRACT_UNQUOTE)
                                                നിർത്തുക;
                                } അല്ലെങ്കിൽ എങ്കിൽ (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        നിർത്തുക;
                                } അല്ലെങ്കിൽ എങ്കിൽ (strchr(separators, c)) {
                                        എങ്കിൽ (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                എങ്കിൽ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                പോകുക finish_force_next;
                                        }
                                        എങ്കിൽ (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                വേണ്ടി (;; (*p)++, c = **p) {
                                                        എങ്കിൽ (c == 0)
                                                                പോകുക finish_force_terminate;
                                                        എങ്കിൽ (!strchr(separators, c))
                                                                നിർത്തുക;
                                                }
                                        പോകുക finish;

                                }

                                എങ്കിൽ (!GREEDY_REALLOC(s, sz+2))
                                        തിരികെ -ENOMEM;

                                s[sz++] = c;

                                എങ്കിൽ (quote != 0)
                                        നിർത്തുക;
                        }
                }
        }

finish_force_terminate:
        *p = ശൂന്യ;
finish:
        എങ്കിൽ (!s) {
                *p = ശൂന്യ;
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ extract_first_word_and_warn(
                സ്ഥിരം അക്ഷരം **p,
                അക്ഷരം **ret,
                സ്ഥിരം അക്ഷരം *separators,
                ExtractFlags flags,
                സ്ഥിരം അക്ഷരം *unit,
                സ്ഥിരം അക്ഷരം *filename,
                unsigned വരി,
                സ്ഥിരം അക്ഷരം *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        സ്ഥിരം അക്ഷരം *save;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        എങ്കിൽ (r >= 0)
                തിരികെ r;

        എങ്കിൽ (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                എങ്കിൽ (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, വരി, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        തിരികെ r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                എങ്കിൽ (r == -EINVAL)
                        തിരികെ log_syntax(unit, LOG_ERR, filename, വരി, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        തിരികെ log_syntax(unit, LOG_ERR, filename, വരി, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(വലുപ്പം(എണ്ണൽ ExtractFlags) <= വലുപ്പം(unsigned));

പൂർണ്ണസംഖ്യ extract_many_വാക്കുകൾ_internal(സ്ഥിരം അക്ഷരം **p, സ്ഥിരം അക്ഷരം *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        പൂർണ്ണസംഖ്യ r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        പ്രതിജ്ഞാ(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        ആകുമ്പോൾ (va_arg(ap, അക്ഷരം**))
                n++;
        va_end(ap);

        എങ്കിൽ (n == 0)
                തിരികെ 0;

        /* Read all words into a temporary array */
        അക്ഷരം **l = newa0(അക്ഷരം*, n);
        unsigned c;

        വേണ്ടി (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                എങ്കിൽ (r < 0) {
                        free_many_charp(l, c);
                        തിരികെ r;
                }
                എങ്കിൽ (r == 0)
                        നിർത്തുക;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                അക്ഷരം **v = ASSERT_PTR(va_arg(ap, അക്ഷരം**));
                *v = *i;
        }
        va_end(ap);

        തിരികെ c;
}
