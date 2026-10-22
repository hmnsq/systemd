/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "escape.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "utf8.h"

పూర్ణాంకం వెలికితీయి_మొదటి_మాట(స్థిరరాశి అక్షరం **p, అక్షరం **ret, స్థిరరాశి అక్షరం *separators, వెలికితీయి_Flags flags) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        size_t sz = 0;
        అక్షరం quote = 0;                 /* 0 or ' or " */
        bool backslash = false;         /* whether we've just seen a backslash */
        అక్షరం c;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, వెలికితీయి_KEEP_QUOTE | వెలికితీయి_UNQUOTE));

        /* Bail early if called after last value or with no input */
        ఒకవేళ (!*p)
                వెళ్ళు ముగింపు;
        c = **p;

        ఒకవేళ (!separators)
                separators = WHITESPACE;

        /* Parses the first word of a string, and returns it in
         * *ret. Removes all quotes in the process. When parsing fails
         * (because of an uneven number of quotes or similar), leaves
         * the pointer *p at the first invalid character. */

        ఒకవేళ (flags & వెలికితీయి_DONT_COALESCE_SEPARATORS)
                ఒకవేళ (!GREEDY_REALLOC(s, sz+1))
                        తిరిగి -ENOMEM;

        కొరకు (;; (*p)++, c = **p) {
                ఒకవేళ (c == 0)
                        వెళ్ళు ముగింపు_force_terminate;
                లేకపోతే ఒకవేళ (strchr(separators, c)) {
                        ఒకవేళ (flags & వెలికితీయి_DONT_COALESCE_SEPARATORS) {
                                ఒకవేళ (!(flags & వెలికితీయి_RETAIN_SEPARATORS))
                                        (*p)++;
                                వెళ్ళు ముగింపు_force_తదుపరి;
                        }
                } లేకపోతే {
                        /* We found a non-blank character, so we will always
                         * want to return a string (even if it is empty),
                         * allocate it here. */
                        ఒకవేళ (!GREEDY_REALLOC(s, sz+1))
                                తిరిగి -ENOMEM;
                        ఆపు;
                }
        }

        కొరకు (;; (*p)++, c = **p) {
                ఒకవేళ (backslash) {
                        ఒకవేళ (!GREEDY_REALLOC(s, sz+7))
                                తిరిగి -ENOMEM;

                        ఒకవేళ (c == 0) {
                                ఒకవేళ ((flags & వెలికితీయి_UNESCAPE_RELAX) &&
                                    (quote == 0 || flags & వెలికితీయి_RELAX)) {
                                        /* If we find an unquoted trailing backslash and we're in
                                         * EXTRACT_UNESCAPE_RELAX mode, keep it verbatim in the
                                         * output.
                                         *
                                         * Unbalanced quotes will only be allowed in EXTRACT_RELAX
                                         * mode, EXTRACT_UNESCAPE_RELAX mode does not allow them.
                                         */
                                        s[sz++] = '\\';
                                        వెళ్ళు ముగింపు_force_terminate;
                                }
                                ఒకవేళ (flags & వెలికితీయి_RELAX)
                                        వెళ్ళు ముగింపు_force_terminate;
                                తిరిగి -EINVAL;
                        }

                        ఒకవేళ (flags & (వెలికితీయి_CUNESCAPE|వెలికితీయి_UNESCAPE_SEPARATORS|వెలికితీయి_UNESCAPE_RELAX)) {
                                bool eight_బిట్టు = false;
                                char32_t u;

                                ఒకవేళ ((flags & వెలికితీయి_CUNESCAPE) &&
                                    (r = cunescape_one(*p, SIZE_MAX, &u, &eight_బిట్టు, false)) >= 0) {
                                        /* A valid escaped sequence */
                                        ప్రతిజ్ఞా(r >= 1);

                                        (*p) += r - 1;

                                        ఒకవేళ (eight_బిట్టు)
                                                s[sz++] = u;
                                        లేకపోతే
                                                sz += utf8_encode_unichar(s + sz, u);
                                } లేకపోతే ఒకవేళ ((flags & వెలికితీయి_UNESCAPE_SEPARATORS) &&
                                           (strchr(separators, **p) || **p == '\\'))
                                        /* An escaped separator char or the escape char itself */
                                        s[sz++] = c;
                                లేకపోతే ఒకవేళ (flags & వెలికితీయి_UNESCAPE_RELAX) {
                                        s[sz++] = '\\';
                                        s[sz++] = c;
                                } లేకపోతే
                                        తిరిగి -EINVAL;
                        } లేకపోతే
                                s[sz++] = c;

                        backslash = false;

                } లేకపోతే ఒకవేళ (quote != 0) {     /* inside either single or double quotes */
                        కొరకు (;; (*p)++, c = **p) {
                                ఒకవేళ (c == 0) {
                                        ఒకవేళ (flags & వెలికితీయి_RELAX)
                                                వెళ్ళు ముగింపు_force_terminate;
                                        తిరిగి -EINVAL;
                                } లేకపోతే ఒకవేళ (c == quote) {        /* found the end quote */
                                        quote = 0;
                                        ఒకవేళ (flags & వెలికితీయి_UNQUOTE)
                                                ఆపు;
                                } లేకపోతే ఒకవేళ (c == '\\' && !(flags & వెలికితీయి_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ఆపు;
                                }

                                ఒకవేళ (!GREEDY_REALLOC(s, sz+2))
                                        తిరిగి -ENOMEM;

                                s[sz++] = c;

                                ఒకవేళ (quote == 0)
                                        ఆపు;
                        }

                } లేకపోతే {
                        కొరకు (;; (*p)++, c = **p) {
                                ఒకవేళ (c == 0)
                                        వెళ్ళు ముగింపు_force_terminate;
                                లేకపోతే ఒకవేళ (లోపల_సమితి(c, '\'', '"') && (flags & (వెలికితీయి_KEEP_QUOTE | వెలికితీయి_UNQUOTE))) {
                                        quote = c;
                                        ఒకవేళ (flags & వెలికితీయి_UNQUOTE)
                                                ఆపు;
                                } లేకపోతే ఒకవేళ (c == '\\' && !(flags & వెలికితీయి_RETAIN_ESCAPE)) {
                                        backslash = true;
                                        ఆపు;
                                } లేకపోతే ఒకవేళ (strchr(separators, c)) {
                                        ఒకవేళ (flags & వెలికితీయి_DONT_COALESCE_SEPARATORS) {
                                                ఒకవేళ (!(flags & వెలికితీయి_RETAIN_SEPARATORS))
                                                        (*p)++;
                                                వెళ్ళు ముగింపు_force_తదుపరి;
                                        }
                                        ఒకవేళ (!(flags & వెలికితీయి_RETAIN_SEPARATORS))
                                                /* Skip additional coalesced separators. */
                                                కొరకు (;; (*p)++, c = **p) {
                                                        ఒకవేళ (c == 0)
                                                                వెళ్ళు ముగింపు_force_terminate;
                                                        ఒకవేళ (!strchr(separators, c))
                                                                ఆపు;
                                                }
                                        వెళ్ళు ముగింపు;

                                }

                                ఒకవేళ (!GREEDY_REALLOC(s, sz+2))
                                        తిరిగి -ENOMEM;

                                s[sz++] = c;

                                ఒకవేళ (quote != 0)
                                        ఆపు;
                        }
                }
        }

ముగింపు_force_terminate:
        *p = ఖాళీ;
ముగింపు:
        ఒకవేళ (!s) {
                *p = ఖాళీ;
                *ret = ఖాళీ;
                తిరిగి 0;
        }

ముగింపు_force_తదుపరి:
        s[sz] = 0;
        *ret = TAKE_PTR(s);

        తిరిగి 1;
}

పూర్ణాంకం వెలికితీయి_మొదటి_మాట_and_warn(
                స్థిరరాశి అక్షరం **p,
                అక్షరం **ret,
                స్థిరరాశి అక్షరం *separators,
                వెలికితీయి_Flags flags,
                స్థిరరాశి అక్షరం *unit,
                స్థిరరాశి అక్షరం *filename,
                గుర్తులేని పంక్తి,
                స్థిరరాశి అక్షరం *rvalue) {

        /* Try to unquote it, if it fails, warn about it and try again
         * but this time using EXTRACT_UNESCAPE_RELAX to keep the
         * backslashes verbatim in invalid escape sequences. */

        స్థిరరాశి అక్షరం *save;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        save = *p;
        r = వెలికితీయి_మొదటి_మాట(p, ret, separators, flags);
        ఒకవేళ (r >= 0)
                తిరిగి r;

        ఒకవేళ (r == -EINVAL && !(flags & వెలికితీయి_UNESCAPE_RELAX)) {

                /* Retry it with EXTRACT_UNESCAPE_RELAX. */
                *p = save;
                r = వెలికితీయి_మొదటి_మాట(p, ret, separators, flags|వెలికితీయి_UNESCAPE_RELAX);
                ఒకవేళ (r >= 0) {
                        /* It worked this time, hence it must have been an invalid escape sequence. */
                        log_syntax(unit, LOG_WARNING, filename, పంక్తి, EINVAL, "Ignoring unknown escape sequences: \"%s\"", *ret);
                        తిరిగి r;
                }

                /* If it's still EINVAL; then it must be unbalanced quoting, report this. */
                ఒకవేళ (r == -EINVAL)
                        తిరిగి log_syntax(unit, LOG_ERR, filename, పంక్తి, r, "Unbalanced quoting, ignoring: \"%s\"", rvalue);
        }

        /* Can be any error, report it */
        తిరిగి log_syntax(unit, LOG_ERR, filename, పంక్తి, r, "Unable to decode word \"%s\", ignoring: %m", rvalue);
}

/* We pass ExtractFlags as unsigned int (to avoid undefined behaviour when passing
 * an object that undergoes default argument promotion as an argument to va_start).
 * Let's make sure that ExtractFlags fits into an unsigned int. */
assert_cc(పరిమాణం(గణన వెలికితీయి_Flags) <= పరిమాణం(గుర్తులేని));

పూర్ణాంకం వెలికితీయి_many_పదాలు_అంతర్గత(స్థిరరాశి అక్షరం **p, స్థిరరాశి అక్షరం *separators, గుర్తులేని flags, ...) {
        va_list ap;
        గుర్తులేని n = 0;
        పూర్ణాంకం r;

        /* Parses a number of words from a string, stripping any quotes if necessary. */

        ప్రతిజ్ఞా(p);

        /* Count how many words are expected */
        va_start(ap, flags);
        అయితే (va_arg(ap, అక్షరం**))
                n++;
        va_end(ap);

        ఒకవేళ (n == 0)
                తిరిగి 0;

        /* Read all words into a temporary array */
        అక్షరం **l = newa0(అక్షరం*, n);
        గుర్తులేని c;

        కొరకు (c = 0; c < n; c++) {
                r = వెలికితీయి_మొదటి_మాట(p, &l[c], separators, flags);
                ఒకవేళ (r < 0) {
                        free_many_charp(l, c);
                        తిరిగి r;
                }
                ఒకవేళ (r == 0)
                        ఆపు;
        }

        /* If we managed to parse all words, return them in the passed in parameters */
        va_start(ap, flags);
        FOREACH_ARRAY(i, l, n) {
                అక్షరం **v = ASSERT_PTR(va_arg(ap, అక్షరం**));
                *v = *i;
        }
        va_end(ap);

        తిరిగి c;
}
