/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "utf8.h"

পূর্ণাঙ্ক extract_first_word(ধ্রুব বর্ণ **p, বর্ণ **ret, ধ্রুব বর্ণ *separators, ExtractFlags flags) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        size_t sz = 0;
        বর্ণ quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        বর্ণ c;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(!FLAGS_SET(flags, EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE));

        /* Bail early if called after last value or with no input */
        যদি (!*p)
                গচ্ছ finish;
        c = **p;

        যদি (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        যদি (flags & EXTRACT_DONT_COALESCE_SEPARATORS)
                যদি (!GREEDY_REALLOC(s, sz+1))
                        প্রত্যাগম -ENOMEM;

        প্রতি (;; (*p)++, c = **p) {
                যদি (c == 0)
                        গচ্ছ finish_force_terminate;
                অন্যথা যদি (strchr(separators, c)) {
                        যদি (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                যদি (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                        (*p)++;
                                গচ্ছ finish_force_next;
                        }
                } অন্যথা {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        যদি (!GREEDY_REALLOC(s, sz+1))
                                প্রত্যাগম -ENOMEM;
                        ভঙ্গ;
                }
        }

        প্রতি (;; (*p)++, c = **p) {
                যদি (backslash) {
                        যদি (!GREEDY_REALLOC(s, sz+7))
                                প্রত্যাগম -ENOMEM;

                        যদি (c == 0) {
                                যদি ((flags & EXTRACT_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & EXTRACT_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        গচ্ছ finish_force_terminate;
                                }
                                যদি (flags & EXTRACT_RELAX)
                                        গচ্ছ finish_force_terminate;
                                প্রত্যাগম -EINVAL;
                        }

                        যদি (flags & (EXTRACT_CUNESCAPE|EXTRACT_UNESCAPE_SEPARATORS|EXTRACT_UNESCAPE_RELAX)) {
                                bool eight_bit = false;
                                char32_t u;

                                যদি ((flags & EXTRACT_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_bit, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        প্রতিজ্ঞা(r >= 1);

                                        (*p) += r - 1;

                                        যদি (eight_bit)
                                                s[sz++] = u;
                                        অন্যথা
                                                sz += utf8_encode_unichar(s + sz, u);
                                } অন্যথা যদি ((flags & EXTRACT_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                অন্যথা যদি (flags & EXTRACT_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } অন্যথা
                                        প্রত্যাগম -EINVAL;
                        } অন্যথা
                                s[sz++] = c;

                        backslash = false;

                } অন্যথা যদি (quote != 0) {     /* inside either single or double quotes */
                        প্রতি (;; (*p)++, c = **p) {
                                যদি (c == 0) {
                                        যদি (flags & EXTRACT_RELAX)
                                                গচ্ছ finish_force_terminate;
                                        প্রত্যাগম -EINVAL;
                                } অন্যথা যদি (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        যদি (flags & EXTRACT_UNQUOTE)
                                                ভঙ্গ;
                                } অন্যথা যদি (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ভঙ্গ;
                                }

                                যদি (!GREEDY_REALLOC(s, sz+2))
                                        প্রত্যাগম -ENOMEM;

                                s[sz++] = c;

                                যদি (quote == 0)
                                        ভঙ্গ;
                        }

                } অন্যথা {
                        প্রতি (;; (*p)++, c = **p) {
                                যদি (c == 0)
                                        গচ্ছ finish_force_terminate;
                                অন্যথা যদি (IN_SET(c, '\'', '"') && (flags & (EXTRACT_KEEP_QUOTE | EXTRACT_UNQUOTE))) {
                                        quote = c;
                                        যদি (flags & EXTRACT_UNQUOTE)
                                                ভঙ্গ;
                                } অন্যথা যদি (c == '\\' && !(flags & EXTRACT_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ভঙ্গ;
                                } অন্যথা যদি (strchr(separators, c)) {
                                        যদি (flags & EXTRACT_DONT_COALESCE_SEPARATORS) {
                                                যদি (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                গচ্ছ finish_force_next;
                                        }
                                        যদি (!(flags & EXTRACT_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                প্রতি (;; (*p)++, c = **p) {
                                                        যদি (c == 0)
                                                                গচ্ছ finish_force_terminate;
                                                        যদি (!strchr(separators, c))
                                                                ভঙ্গ;
                                                }
                                        গচ্ছ finish;

                                }

                                যদি (!GREEDY_REALLOC(s, sz+2))
                                        প্রত্যাগম -ENOMEM;

                                s[sz++] = c;

                                যদি (quote != 0)
                                        ভঙ্গ;
                        }
                }
        }

finish_force_terminate:
        *p = রিক্ত;
finish:
        যদি (!s) {
                *p = রিক্ত;
                *ret = রিক্ত;
                প্রত্যাগম 0;
        }

finish_force_next:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক extract_first_word_and_warn(
                ধ্রুব বর্ণ **p,
                বর্ণ **ret,
                ধ্রুব বর্ণ *separators,
                ExtractFlags flags,
                ধ্রুব বর্ণ *unit,
                ধ্রুব বর্ণ *filename,
                unsigned সারি,
                ধ্রুব বর্ণ *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ধ্রুব বর্ণ *save;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        save = *p;
        r = extract_first_word(p, ret, separators, flags);
        যদি (r >= 0)
                প্রত্যাগম r;

        যদি (r == -EINVAL && !(flags & EXTRACT_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = extract_first_word(p, ret, separators, flags|EXTRACT_UNESCAPE_RELAX);
                যদি (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, সারি, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        প্রত্যাগম r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                যদি (r == -EINVAL)
                        প্রত্যাগম log_syntax(unit, LOG_ERR, filename, সারি, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        প্রত্যাগম log_syntax(unit, LOG_ERR, filename, সারি, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(পরিমাণ(গণনা ExtractFlags) <= পরিমাণ(unsigned));

পূর্ণাঙ্ক extract_many_শব্দগুলি_internal(ধ্রুব বর্ণ **p, ধ্রুব বর্ণ *separators, unsigned flags, ...) {
        va_list ap;
        unsigned n = 0;
        পূর্ণাঙ্ক r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        প্রতিজ্ঞা(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        যাবত্ (va_arg(ap, বর্ণ**))
                n++;
        va_end(ap);

        যদি (n == 0)
                প্রত্যাগম 0;

        /* Read all words into a temporary array */
        বর্ণ **l = newa0(বর্ণ*, n);
        unsigned c;

        প্রতি (c = 0; c < n; c++) {
                r = extract_first_word(p, &l[c], separators, flags);
                যদি (r < 0) {
                        free_many_charp(l, c);
                        প্রত্যাগম r;
                }
                যদি (r == 0)
                        ভঙ্গ;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                বর্ণ **v = ASSERT_PTR(va_arg(ap, বর্ণ**));
                *v = *i;
        }
        va_end(ap);

        প্রত্যাগম c;
}
