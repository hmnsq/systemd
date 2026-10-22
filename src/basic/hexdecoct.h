/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

అక్షరం octchar(పూర్ణాంకం x) _const_;
పూర్ణాంకం unoctchar(అక్షరం c) _const_;

అక్షరం decchar(పూర్ణాంకం x) _const_;
పూర్ణాంకం undecchar(అక్షరం c) _const_;

అక్షరం hexchar(పూర్ణాంకం x) _const_;
పూర్ణాంకం unhexchar(అక్షరం c) _const_;

అక్షరం* hexmem(స్థిరరాశి శూన్య *p, size_t l) _nonnull_if_nonzero_(1, 2);
పూర్ణాంకం unhexmem_నిండిన(స్థిరరాశి అక్షరం *p, size_t l, bool secure, శూన్య **ret_దత్తాంశం, size_t *ret_కొలత) _nonnull_if_nonzero_(1, 2);
స్థిర అంతఃవరుస పూర్ణాంకం unhexmem(స్థిరరాశి అక్షరం *p, శూన్య **ret_దత్తాంశం, size_t *ret_కొలత) {
        తిరిగి unhexmem_నిండిన(p, SIZE_MAX, false, ret_దత్తాంశం, ret_కొలత);
}

అక్షరం base32hexchar(పూర్ణాంకం x) _const_;
పూర్ణాంకం unbase32hexchar(అక్షరం c) _const_;

అక్షరం base64char(పూర్ణాంకం x) _const_;
అక్షరం urlsafe_base64char(పూర్ణాంకం x) _const_;
పూర్ణాంకం unbase64char(అక్షరం c) _const_;

అక్షరం* base32hexmem(స్థిరరాశి శూన్య *p, size_t l, bool padding) _nonnull_if_nonzero_(1, 2);
పూర్ణాంకం unbase32hexmem(స్థిరరాశి అక్షరం *p, size_t l, bool padding, శూన్య **mem, size_t *len) _nonnull_if_nonzero_(1, 2);

ssize_t base64mem_నిండిన(స్థిరరాశి శూన్య *p, size_t l, size_t పంక్తి_break, అక్షరం **ret) _nonnull_if_nonzero_(1, 2);
స్థిర అంతఃవరుస ssize_t base64mem(స్థిరరాశి శూన్య *p, size_t l, అక్షరం **ret) {
        తిరిగి base64mem_నిండిన(p, l, SIZE_MAX, ret);
}

ssize_t base64_append(
                అక్షరం **ఉపసర్గ,
                size_t plen,
                స్థిరరాశి శూన్య *p,
                size_t l,
                size_t indent,
                size_t width);
పూర్ణాంకం unbase64mem_నిండిన(స్థిరరాశి అక్షరం *p, size_t l, bool secure, శూన్య **ret_దత్తాంశం, size_t *ret_కొలత) _nonnull_if_nonzero_(1, 2);
స్థిర అంతఃవరుస పూర్ణాంకం unbase64mem(స్థిరరాశి అక్షరం *p, శూన్య **ret_దత్తాంశం, size_t *ret_కొలత) {
        తిరిగి unbase64mem_నిండిన(p, SIZE_MAX, false, ret_దత్తాంశం, ret_కొలత);
}

శూన్య hexdump(FILE *f, స్థిరరాశి శూన్య *p, size_t s) _nonnull_if_nonzero_(2, 3);
