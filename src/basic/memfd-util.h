/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/mman.h>           /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

పూర్ణాంకం memfd_create_wrapper(స్థిరరాశి అక్షరం *name, గుర్తులేని బహుళకం);

పూర్ణాంకం memfd_new_నిండిన(స్థిరరాశి అక్షరం *name, గుర్తులేని extra_flags);
స్థిర అంతఃవరుస పూర్ణాంకం memfd_new(స్థిరరాశి అక్షరం *name) {
        తిరిగి memfd_new_నిండిన(name, 0);
}

పూర్ణాంకం memfd_new_and_seal(స్థిరరాశి అక్షరం *name, స్థిరరాశి శూన్య *దత్తాంశం, size_t sz) _nonnull_if_nonzero_(2, 3);
స్థిర అంతఃవరుస పూర్ణాంకం memfd_new_and_seal_string(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *s) {
        తిరిగి memfd_new_and_seal(name, s, SIZE_MAX);
}

పూర్ణాంకం memfd_సమితి_sealed(పూర్ణాంకం fd);
పూర్ణాంకం memfd_get_sealed(పూర్ణాంకం fd);
పూర్ణాంకం memfd_సమితి_sealed_gracefully(పూర్ణాంకం fd);

పూర్ణాంకం memfd_get_కొలత(పూర్ణాంకం fd, uint64_t *ret);
పూర్ణాంకం memfd_సమితి_కొలత(పూర్ణాంకం fd, uint64_t sz);
