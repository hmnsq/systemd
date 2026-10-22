/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "utf8.h"

முழு extract_first_word(மாறிலி எழுத்து **p, எழுத்து **ret, மாறிலி எழுத்து *separators, ExtractFlags flags) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        size_t sz = 0;
        எழுத்து quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        எழுத்து c;
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        எனில் (!*p)
                செல் finish;
        c = **p;

        எனில் (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        எனில் (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                எனில் (!GREEDY_REALLOC(s, sz+1))
                        திரும்பு -ENOMEM;

        க்கு (;; (*p)++, c = **p) {
                எனில் (c == 0)
                        செல் finish_force_terminate;
                இல்லையெனில் எனில் (strchr(separators, c)) {
                        எனில் (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                எனில் (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                செல் finish_force_next;
                        }
                } இல்லையெனில் {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        எனில் (!GREEDY_REALLOC(s, sz+1))
                                திரும்பு -ENOMEM;
                        நிறுத்து;
                }
        }

        க்கு (;; (*p)++, c = **p) {
                எனில் (backslash) {
                        எனில் (!GREEDY_REALLOC(s, sz+7))
                                திரும்பு -ENOMEM;

                        எனில் (c == 0) {
                                எனில் ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        செல் finish_force_terminate;
                                }
                                எனில் (flags & EXTRACT_RELAX)
                                        செல் finish_force_terminate;
                                திரும்பு -EINVAL;
                        }

                        எனில் (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                எனில் ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        ப்ரதிஜ்ஞா(r >= 1);

                                        (*p) += r - 1;

                                        எனில் (eight_bit)
                                                s[sz++] = u;
                                        இல்லையெனில்
                                                sz += utf8_encode_unichar(s + sz, u);
                                } இல்லையெனில் எனில் ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                இல்லையெனில் எனில் (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } இல்லையெனில்
                                        திரும்பு -EINVAL;
                        } இல்லையெனில்
                                s[sz++] = c;

                        backslash = false;

                } இல்லையெனில் எனில் (quote != 0) {     /* inside either single or double quotes */
                        க்கு (;; (*p)++, c = **p) {
                                எனில் (c == 0) {
                                        எனில் (flags & EXTRACT_RELAX)
                                                செல் finish_force_terminate;
                                        திரும்பு -EINVAL;
                                } இல்லையெனில் எனில் (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        எனில் (flags & EXTRACT_UNQUOTE)
                                                நிறுத்து;
                                } இல்லையெனில் எனில் (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        நிறுத்து;
                                }

                                எனில் (!GREEDY_REALLOC(s, sz+2))
                                        திரும்பு -ENOMEM;

                                s[sz++] = c;

                                எனில் (quote == 0)
                                        நிறுத்து;
                        }

                } இல்லையெனில் {
                        க்கு (;; (*p)++, c = **p) {
                                எனில் (c == 0)
                                        செல் finish_force_terminate;
                                இல்லையெனில் எனில் (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        எனில் (flags & EXTRACT_UNQUOTE)
                                                நிறுத்து;
                                } இல்லையெனில் எனில் (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        நிறுத்து;
                                } இல்லையெனில் எனில் (strchr(separators, c)) {
                                        எனில் (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                எனில் (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                செல் finish_force_next;
                                        }
                                        எனில் (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                க்கு (;; (*p)++, c = **p) {
                                                        எனில் (c == 0)
                                                                செல் finish_force_terminate;
                                                        எனில் (!strchr(separators, c))
                                                                நிறுத்து;
                                                }
                                        செல் finish;

                                }

                                எனில் (!GREEDY_REALLOC(s, sz+2))
                                        திரும்பு -ENOMEM;

                                s[sz++] = c;

                                எனில் (quote != 0)
                                        நிறுத்து;
                        }
                }
        }

finish_force_terminate:
        *p = வெற்று;
finish:
        எனில் (!s) {
                *p = வெற்று;
                *ret = வெற்று;
                திரும்பு 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        திரும்பு 1;
}

முழு extract_first_word_and_warn(
                மாறிலி எழுத்து **p,
                எழுத்து **ret,
                மாறிலி எழுத்து *separators,
                ExtractFlags flags,
                மாறிலி எழுத்து *unit,
                மாறிலி எழுத்து *filename,
                unsigned வரி,
                மாறிலி எழுத்து *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        மாறிலி எழுத்து *save;
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        எனில் (r >= 0)
                திரும்பு r;

        எனில் (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                எனில் (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, வரி, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        திரும்பு r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                எனில் (r == -EINVAL)
                        திரும்பு log_syntax(unit, LOG_ERR, filename, வரி, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        திரும்பு log_syntax(unit, LOG_ERR, filename, வரி, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(அளவு(எண்ணிக்கை ExtractFlags) <= அளவு(unsigned));

முழு extract_many_சொற்கள்_internal(மாறிலி எழுத்து **p, மாறிலி எழுத்து *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        முழு r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        ப்ரதிஜ்ஞா(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        போது (va_arg(ap, எழுத்து**))
                n++;
        va_end(ap);

        எனில் (n == 0)
                திரும்பு 0;

        /* Read all words into a temporary array */
        எழுத்து **l = newa0(எழுத்து*, n);
        unsigned c;

        க்கு (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                எனில் (r < 0) {
                        free_many_charp(l, c);
                        திரும்பு r;
                }
                எனில் (r == 0)
                        நிறுத்து;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                எழுத்து **v = ASSERT_PTR(va_arg(ap, எழுத்து**));
                *v = *i;
        }
        va_end(ap);

        திரும்பு c;
}
