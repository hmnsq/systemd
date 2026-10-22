/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

ਪੂਰਨ ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ **p, ਅੱਖਰ **ret, ਅਚਲ ਅੱਖਰ *separators, ਕੱਢੋ_Flags flags) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        size_t sz = 0;
        ਅੱਖਰ quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        ਅੱਖਰ c;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, ਕੱਢੋ_KEEP_QUOTE | ਕੱਢੋ_UNQUOTE));

        /* Bail early if called after last value or with no input */
        ਜੇ (!*p)
                ਜਾਓ ਅੰਤ;
        c = **p;

        ਜੇ (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        ਜੇ (flags & ਕੱਢੋ_DONT_COALESCE_SEPARATORS)
                ਜੇ (!GREEDY_REALLOC(s, sz+1))
                        ਵਾਪਸ -ENOMEM;

        ਲਈ (;; (*p)++, c = **p) {
                ਜੇ (c == 0)
                        ਜਾਓ ਅੰਤ_force_terminate;
                ਨਹੀਂਤਾਂ ਜੇ (strchr(separators, c)) {
                        ਜੇ (flags & ਕੱਢੋ_DONT_COALESCE_SEPARATORS) {
                                ਜੇ (!(flags & ਕੱਢੋ_RETAIN_SEPARATORS))
                                        (*p)++;
                                ਜਾਓ ਅੰਤ_force_ਅਗਲਾ;
                        }
                } ਨਹੀਂਤਾਂ {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        ਜੇ (!GREEDY_REALLOC(s, sz+1))
                                ਵਾਪਸ -ENOMEM;
                        ਰੋਕੋ;
                }
        }

        ਲਈ (;; (*p)++, c = **p) {
                ਜੇ (backslash) {
                        ਜੇ (!GREEDY_REALLOC(s, sz+7))
                                ਵਾਪਸ -ENOMEM;

                        ਜੇ (c == 0) {
                                ਜੇ ((flags & ਕੱਢੋ_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & ਕੱਢੋ_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        ਜਾਓ ਅੰਤ_force_terminate;
                                }
                                ਜੇ (flags & ਕੱਢੋ_RELAX)
                                        ਜਾਓ ਅੰਤ_force_terminate;
                                ਵਾਪਸ -EINVAL;
                        }

                        ਜੇ (flags & (ਕੱਢੋ_CUNESCAPE|ਕੱਢੋ_UNESCAPE_SEPARATORS|ਕੱਢੋ_UNESCAPE_RELAX)) {
                                bool eight_ਦੋਆਧਾਰੀ_ਅੰਕ = false;
                                char32_t u;

                                ਜੇ ((flags & ਕੱਢੋ_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_ਦੋਆਧਾਰੀ_ਅੰਕ, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        ਪ੍ਰਤਿਜ੍ਞਾ(r >= 1);

                                        (*p) += r - 1;

                                        ਜੇ (eight_ਦੋਆਧਾਰੀ_ਅੰਕ)
                                                s[sz++] = u;
                                        ਨਹੀਂਤਾਂ
                                                sz += utf8_encode_unichar(s + sz, u);
                                } ਨਹੀਂਤਾਂ ਜੇ ((flags & ਕੱਢੋ_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                ਨਹੀਂਤਾਂ ਜੇ (flags & ਕੱਢੋ_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } ਨਹੀਂਤਾਂ
                                        ਵਾਪਸ -EINVAL;
                        } ਨਹੀਂਤਾਂ
                                s[sz++] = c;

                        backslash = false;

                } ਨਹੀਂਤਾਂ ਜੇ (quote != 0) {     /* inside either single or double quotes */
                        ਲਈ (;; (*p)++, c = **p) {
                                ਜੇ (c == 0) {
                                        ਜੇ (flags & ਕੱਢੋ_RELAX)
                                                ਜਾਓ ਅੰਤ_force_terminate;
                                        ਵਾਪਸ -EINVAL;
                                } ਨਹੀਂਤਾਂ ਜੇ (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        ਜੇ (flags & ਕੱਢੋ_UNQUOTE)
                                                ਰੋਕੋ;
                                } ਨਹੀਂਤਾਂ ਜੇ (c == '\\' && !(flags & ਕੱਢੋ_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ਰੋਕੋ;
                                }

                                ਜੇ (!GREEDY_REALLOC(s, sz+2))
                                        ਵਾਪਸ -ENOMEM;

                                s[sz++] = c;

                                ਜੇ (quote == 0)
                                        ਰੋਕੋ;
                        }

                } ਨਹੀਂਤਾਂ {
                        ਲਈ (;; (*p)++, c = **p) {
                                ਜੇ (c == 0)
                                        ਜਾਓ ਅੰਤ_force_terminate;
                                ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, '\'', '"') && (flags & (ਕੱਢੋ_KEEP_QUOTE | ਕੱਢੋ_UNQUOTE))) {
                                        quote = c;
                                        ਜੇ (flags & ਕੱਢੋ_UNQUOTE)
                                                ਰੋਕੋ;
                                } ਨਹੀਂਤਾਂ ਜੇ (c == '\\' && !(flags & ਕੱਢੋ_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ਰੋਕੋ;
                                } ਨਹੀਂਤਾਂ ਜੇ (strchr(separators, c)) {
                                        ਜੇ (flags & ਕੱਢੋ_DONT_COALESCE_SEPARATORS) {
                                                ਜੇ (!(flags & ਕੱਢੋ_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                ਜਾਓ ਅੰਤ_force_ਅਗਲਾ;
                                        }
                                        ਜੇ (!(flags & ਕੱਢੋ_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                ਲਈ (;; (*p)++, c = **p) {
                                                        ਜੇ (c == 0)
                                                                ਜਾਓ ਅੰਤ_force_terminate;
                                                        ਜੇ (!strchr(separators, c))
                                                                ਰੋਕੋ;
                                                }
                                        ਜਾਓ ਅੰਤ;

                                }

                                ਜੇ (!GREEDY_REALLOC(s, sz+2))
                                        ਵਾਪਸ -ENOMEM;

                                s[sz++] = c;

                                ਜੇ (quote != 0)
                                        ਰੋਕੋ;
                        }
                }
        }

ਅੰਤ_force_terminate:
        *p = ਨਿਰਾ;
ਅੰਤ:
        ਜੇ (!s) {
                *p = ਨਿਰਾ;
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

ਅੰਤ_force_ਅਗਲਾ:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        ਵਾਪਸ 1;
}

ਪੂਰਨ ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ_and_warn(
                ਅਚਲ ਅੱਖਰ **p,
                ਅੱਖਰ **ret,
                ਅਚਲ ਅੱਖਰ *separators,
                ਕੱਢੋ_Flags flags,
                ਅਚਲ ਅੱਖਰ *unit,
                ਅਚਲ ਅੱਖਰ *filename,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        ਅਚਲ ਅੱਖਰ *save;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        save = *p;
        r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(p, ret, separators, flags);
        ਜੇ (r >= 0)
                ਵਾਪਸ r;

        ਜੇ (r == -EINVAL && !(flags & ਕੱਢੋ_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(p, ret, separators, flags|ਕੱਢੋ_UNESCAPE_RELAX);
                ਜੇ (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, ਸਤਰ, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        ਵਾਪਸ r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                ਜੇ (r == -EINVAL)
                        ਵਾਪਸ log_syntax(unit, LOG_ERR, filename, ਸਤਰ, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        ਵਾਪਸ log_syntax(unit, LOG_ERR, filename, ਸਤਰ, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(ਆਕਾਰ(ਗਿਣਤੀ ਕੱਢੋ_Flags) <= ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ));

ਪੂਰਨ ਕੱਢੋ_many_ਸ਼ਬਦ_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ **p, ਅਚਲ ਅੱਖਰ *separators, ਬਿਨਾਨਿਸ਼ਾਨੀ flags, ...) {
        va_list ap;
        ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0;
        ਪੂਰਨ r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        ਜਦੋਂ (va_arg(ap, ਅੱਖਰ**))
                n++;
        va_end(ap);

        ਜੇ (n == 0)
                ਵਾਪਸ 0;

        /* Read all words into a temporary array */
        ਅੱਖਰ **l = newa0(ਅੱਖਰ*, n);
        ਬਿਨਾਨਿਸ਼ਾਨੀ c;

        ਲਈ (c = 0; c < n; c++) {
                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(p, &l[c], separators, flags);
                ਜੇ (r < 0) {
                        free_many_charp(l, c);
                        ਵਾਪਸ r;
                }
                ਜੇ (r == 0)
                        ਰੋਕੋ;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                ਅੱਖਰ **v = ASSERT_PTR(va_arg(ap, ਅੱਖਰ**));
                *v = *i;
        }
        va_end(ap);

        ਵਾਪਸ c;
}
