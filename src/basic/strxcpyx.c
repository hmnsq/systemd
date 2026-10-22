/* SPDX-License-Identifier: LGPL-2.1-or-later */

/*
 * Concatenates/copies strings. In any case, terminates in all cases
 * with '\0' and moves the @dest pointer forward to the added '\0'.
 * Returns the remaining size, and 0 if the string was truncated.
 *
 * Due to the intended usage, these helpers silently noop invocations
 * having zero size.  This is technically an exception to the above
 * statement "terminates in all cases".  It's unexpected for such calls to
 * occur outside of a loop where this is the preferred behavior.
 */

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "strxcpyx.h"

size_t strnpcpy_నిండిన(అక్షరం **dest, size_t కొలత, స్థిరరాశి అక్షరం *src, size_t len, bool *ret_truncated) {
        bool truncated = false;

        ప్రతిజ్ఞా(dest);
        ప్రతిజ్ఞా(src);

        ఒకవేళ (కొలత == 0) {
                ఒకవేళ (ret_truncated)
                        *ret_truncated = len > 0;
                తిరిగి 0;
        }

        ఒకవేళ (len >= కొలత) {
                ఒకవేళ (కొలత > 1)
                        *dest = mempcpy(*dest, src, కొలత-1);
                కొలత = 0;
                truncated = true;
        } లేకపోతే ఒకవేళ (len > 0) {
                *dest = mempcpy(*dest, src, len);
                కొలత -= len;
        }

        ఒకవేళ (ret_truncated)
                *ret_truncated = truncated;

        (*dest)[0] = '\0';
        తిరిగి కొలత;
}

size_t strpcpy_నిండిన(అక్షరం **dest, size_t కొలత, స్థిరరాశి అక్షరం *src, bool *ret_truncated) {
        ప్రతిజ్ఞా(dest);
        ప్రతిజ్ఞా(src);

        తిరిగి strnpcpy_నిండిన(dest, కొలత, src, అక్షరశ్రేణి_పొడవు(src), ret_truncated);
}

size_t strpcpyf_నిండిన(అక్షరం **dest, size_t కొలత, bool *ret_truncated, స్థిరరాశి అక్షరం *src, ...) {
        bool truncated = false;
        va_list va;
        పూర్ణాంకం i;

        ప్రతిజ్ఞా(dest);
        ప్రతిజ్ఞా(src);

        va_start(va, src);
        i = vsnprintf(*dest, కొలత, src, va);
        va_end(va);

        ప్రతిజ్ఞా(i >= 0);

        ఒకవేళ (i < (పూర్ణాంకం) కొలత) {
                *dest += i;
                కొలత -= i;
        } లేకపోతే {
                కొలత = 0;
                truncated = i > 0;
        }

        ఒకవేళ (ret_truncated)
                *ret_truncated = truncated;

        తిరిగి కొలత;
}

size_t strpcpyl_నిండిన(అక్షరం **dest, size_t కొలత, bool *ret_truncated, స్థిరరాశి అక్షరం *src, ...) {
        bool truncated = false;
        va_list va;

        ప్రతిజ్ఞా(dest);
        ప్రతిజ్ఞా(src);

        va_start(va, src);
        చేయి {
                bool t;

                కొలత = strpcpy_నిండిన(dest, కొలత, src, &t);
                truncated = truncated || t;
                src = va_arg(va, అక్షరం *);
        } అయితే (src);
        va_end(va);

        ఒకవేళ (ret_truncated)
                *ret_truncated = truncated;
        తిరిగి కొలత;
}

size_t strnscpy_నిండిన(అక్షరం *dest, size_t కొలత, స్థిరరాశి అక్షరం *src, size_t len, bool *ret_truncated) {
        అక్షరం *s;

        ప్రతిజ్ఞా(dest);
        ప్రతిజ్ఞా(src);

        s = dest;
        తిరిగి strnpcpy_నిండిన(&s, కొలత, src, len, ret_truncated);
}

size_t strscpy_నిండిన(అక్షరం *dest, size_t కొలత, స్థిరరాశి అక్షరం *src, bool *ret_truncated) {
        ప్రతిజ్ఞా(dest);
        ప్రతిజ్ఞా(src);

        తిరిగి strnscpy_నిండిన(dest, కొలత, src, అక్షరశ్రేణి_పొడవు(src), ret_truncated);
}

size_t strscpyl_నిండిన(అక్షరం *dest, size_t కొలత, bool *ret_truncated, స్థిరరాశి అక్షరం *src, ...) {
        bool truncated = false;
        va_list va;
        అక్షరం *s;

        ప్రతిజ్ఞా(dest);
        ప్రతిజ్ఞా(src);

        va_start(va, src);
        s = dest;
        చేయి {
                bool t;

                కొలత = strpcpy_నిండిన(&s, కొలత, src, &t);
                truncated = truncated || t;
                src = va_arg(va, అక్షరం *);
        } అయితే (src);
        va_end(va);

        ఒకవేళ (ret_truncated)
                *ret_truncated = truncated;

        తిరిగి కొలత;
}
