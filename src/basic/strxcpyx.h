/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

size_t strnpcpy_నిండిన(అక్షరం **dest, size_t కొలత, స్థిరరాశి అక్షరం *src, size_t len, bool *ret_truncated);
స్థిర అంతఃవరుస size_t strnpcpy(అక్షరం **dest, size_t కొలత, స్థిరరాశి అక్షరం *src, size_t len) {
        తిరిగి strnpcpy_నిండిన(dest, కొలత, src, len, ఖాళీ);
}
size_t strpcpy_నిండిన(అక్షరం **dest, size_t కొలత, స్థిరరాశి అక్షరం *src, bool *ret_truncated);
స్థిర అంతఃవరుస size_t strpcpy(అక్షరం **dest, size_t కొలత, స్థిరరాశి అక్షరం *src) {
        తిరిగి strpcpy_నిండిన(dest, కొలత, src, ఖాళీ);
}
size_t strpcpyf_నిండిన(అక్షరం **dest, size_t కొలత, bool *ret_truncated, స్థిరరాశి అక్షరం *src, ...) _printf_(4, 5);
#నిర్వచించు strpcpyf(dest, కొలత, src, ...) \
        strpcpyf_నిండిన((dest), (కొలత), ఖాళీ, (src), ##__VA_ARGS__)
size_t strpcpyl_నిండిన(అక్షరం **dest, size_t కొలత, bool *ret_truncated, స్థిరరాశి అక్షరం *src, ...) _sentinel_;
#నిర్వచించు strpcpyl(dest, కొలత, src, ...) \
        strpcpyl_నిండిన((dest), (కొలత), ఖాళీ, (src), ##__VA_ARGS__)
size_t strnscpy_నిండిన(అక్షరం *dest, size_t కొలత, స్థిరరాశి అక్షరం *src, size_t len, bool *ret_truncated);
స్థిర అంతఃవరుస size_t strnscpy(అక్షరం *dest, size_t కొలత, స్థిరరాశి అక్షరం *src, size_t len) {
        తిరిగి strnscpy_నిండిన(dest, కొలత, src, len, ఖాళీ);
}
size_t strscpy_నిండిన(అక్షరం *dest, size_t కొలత, స్థిరరాశి అక్షరం *src, bool *ret_truncated);
స్థిర అంతఃవరుస size_t strscpy(అక్షరం *dest, size_t కొలత, స్థిరరాశి అక్షరం *src) {
        తిరిగి strscpy_నిండిన(dest, కొలత, src, ఖాళీ);
}
size_t strscpyl_నిండిన(అక్షరం *dest, size_t కొలత, bool *ret_truncated, స్థిరరాశి అక్షరం *src, ...) _sentinel_;
#నిర్వచించు strscpyl(dest, కొలత, src, ...) \
        strscpyl_నిండిన(dest, కొలత, ఖాళీ, src, ##__VA_ARGS__)
