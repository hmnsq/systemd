/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "utf8.h"

પૂર્ણાંક extract_first_word(અચલ અક્ષર **p, અક્ષર **ret, અચલ અક્ષર *separators, ExtractFlags flags) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        size_t sz = 0;
        અક્ષર quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        અક્ષર c;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        જો (!*p)
                જાઓ finish;
        c = **p;

        જો (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        જો (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                જો (!GREEDY_REALLOC(s, sz+1))
                        પાછા -ENOMEM;

        માટે (;; (*p)++, c = **p) {
                જો (c == 0)
                        જાઓ finish_force_terminate;
                નહીંતર જો (strchr(separators, c)) {
                        જો (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                જો (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                જાઓ finish_force_next;
                        }
                } નહીંતર {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        જો (!GREEDY_REALLOC(s, sz+1))
                                પાછા -ENOMEM;
                        તોડો;
                }
        }

        માટે (;; (*p)++, c = **p) {
                જો (backslash) {
                        જો (!GREEDY_REALLOC(s, sz+7))
                                પાછા -ENOMEM;

                        જો (c == 0) {
                                જો ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        જાઓ finish_force_terminate;
                                }
                                જો (flags & EXTRACT_RELAX)
                                        જાઓ finish_force_terminate;
                                પાછા -EINVAL;
                        }

                        જો (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                જો ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        પ્રતિજ્ઞા(r >= 1);

                                        (*p) += r - 1;

                                        જો (eight_bit)
                                                s[sz++] = u;
                                        નહીંતર
                                                sz += utf8_encode_unichar(s + sz, u);
                                } નહીંતર જો ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                નહીંતર જો (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } નહીંતર
                                        પાછા -EINVAL;
                        } નહીંતર
                                s[sz++] = c;

                        backslash = false;

                } નહીંતર જો (quote != 0) {     /* inside either single or double quotes */
                        માટે (;; (*p)++, c = **p) {
                                જો (c == 0) {
                                        જો (flags & EXTRACT_RELAX)
                                                જાઓ finish_force_terminate;
                                        પાછા -EINVAL;
                                } નહીંતર જો (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        જો (flags & EXTRACT_UNQUOTE)
                                                તોડો;
                                } નહીંતર જો (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        તોડો;
                                }

                                જો (!GREEDY_REALLOC(s, sz+2))
                                        પાછા -ENOMEM;

                                s[sz++] = c;

                                જો (quote == 0)
                                        તોડો;
                        }

                } નહીંતર {
                        માટે (;; (*p)++, c = **p) {
                                જો (c == 0)
                                        જાઓ finish_force_terminate;
                                નહીંતર જો (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        જો (flags & EXTRACT_UNQUOTE)
                                                તોડો;
                                } નહીંતર જો (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        તોડો;
                                } નહીંતર જો (strchr(separators, c)) {
                                        જો (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                જો (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                જાઓ finish_force_next;
                                        }
                                        જો (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                માટે (;; (*p)++, c = **p) {
                                                        જો (c == 0)
                                                                જાઓ finish_force_terminate;
                                                        જો (!strchr(separators, c))
                                                                તોડો;
                                                }
                                        જાઓ finish;

                                }

                                જો (!GREEDY_REALLOC(s, sz+2))
                                        પાછા -ENOMEM;

                                s[sz++] = c;

                                જો (quote != 0)
                                        તોડો;
                        }
                }
        }

finish_force_terminate:
        *p = ખાલી;
finish:
        જો (!s) {
                *p = ખાલી;
                *ret = ખાલી;
                પાછા 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        પાછા 1;
}

પૂર્ણાંક extract_first_word_and_warn(
                અચલ અક્ષર **p,
                અક્ષર **ret,
                અચલ અક્ષર *separators,
                ExtractFlags flags,
                અચલ અક્ષર *unit,
                અચલ અક્ષર *filename,
                unsigned લીટી,
                અચલ અક્ષર *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        અચલ અક્ષર *save;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        જો (r >= 0)
                પાછા r;

        જો (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                જો (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, લીટી, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        પાછા r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                જો (r == -EINVAL)
                        પાછા log_syntax(unit, LOG_ERR, filename, લીટી, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        પાછા log_syntax(unit, LOG_ERR, filename, લીટી, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(કદ(ગણના ExtractFlags) <= કદ(unsigned));

પૂર્ણાંક extract_many_શબ્દો_internal(અચલ અક્ષર **p, અચલ અક્ષર *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        પૂર્ણાંક r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        પ્રતિજ્ઞા(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        જ્યારે (va_arg(ap, અક્ષર**))
                n++;
        va_end(ap);

        જો (n == 0)
                પાછા 0;

        /* Read all words into a temporary array */
        અક્ષર **l = newa0(અક્ષર*, n);
        unsigned c;

        માટે (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                જો (r < 0) {
                        free_many_charp(l, c);
                        પાછા r;
                }
                જો (r == 0)
                        તોડો;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                અક્ષર **v = ASSERT_PTR(va_arg(ap, અક્ષર**));
                *v = *i;
        }
        va_end(ap);

        પાછા c;
}
