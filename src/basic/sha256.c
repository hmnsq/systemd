/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "sha256.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం sha256_fd(పూర్ణాంకం fd, uint64_t max_కొలత, uint8_t ret[స్థిర SHA256_DIGEST_కొలత]) {
        నిర్మాణం sha256_ctx ctx;
        uint64_t మొత్తం_కొలత = 0;

        ప్రతిజ్ఞా(fd >= 0);

        sha256_init_ctx(&ctx);

        కొరకు (;;) {
                uint8_t బఫరు[64 * 1024];
                ssize_t n;

                n = పఠ(fd, బఫరు, పరిమాణం(బఫరు));
                ఒకవేళ (n < 0)
                        తిరిగి -errno;
                ఒకవేళ (n == 0)
                        ఆపు;

                ఒకవేళ (!INC_సురక్షిత(&మొత్తం_కొలత, n) || మొత్తం_కొలత > max_కొలత)
                        తిరిగి -EFBIG;

                sha256_process_బైట్లు(బఫరు, n, &ctx);
        }

        sha256_ముగింపు_ctx(&ctx, ret);
        తిరిగి 0;
}

పూర్ణాంకం parse_sha256(స్థిరరాశి అక్షరం *s, uint8_t ret[స్థిర SHA256_DIGEST_కొలత]) {
        _cleanup_free_ uint8_t *దత్తాంశం = ఖాళీ;
        size_t కొలత = 0;
        పూర్ణాంకం r;

        ఒకవేళ (!sha256_is_valid(s))
                తిరిగి -EINVAL;

        r = unhexmem_నిండిన(s, SHA256_DIGEST_కొలత * 2, false, (శూన్య**) &దత్తాంశం, &కొలత);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(కొలత == SHA256_DIGEST_కొలత);

        జ్ఞాపకం_నకలు(ret, దత్తాంశం, కొలత);
        తిరిగి 0;
}

bool sha256_is_valid(స్థిరరాశి అక్షరం *s) {
        తిరిగి s && లోపల_charset(s, HEXDIGITS) && (అక్షరశ్రేణి_పొడవు(s) == SHA256_DIGEST_కొలత * 2);
}

అక్షరం* sha256_ప్రత్యక్ష_hex(స్థిరరాశి శూన్య *బఫరు, size_t sz) {
        ప్రతిజ్ఞా(బఫరు || sz == 0);

        ఒకవేళ (sz == SIZE_MAX)
                sz = అక్షరశ్రేణి_పొడవు(బఫరు);

        తిరిగి hexmem(SHA256_ప్రత్యక్ష(బఫరు, sz), SHA256_DIGEST_కొలత);
}
