/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

నిర్మాణం strbuf {
        అక్షరం *buf;
        size_t len;
        నిర్మాణం strbuf_కణుపు *మూలం;

        size_t nodes_లెక్క;
        size_t లోపల_లెక్క;
        size_t లోపల_len;
        size_t dedup_len;
        size_t dedup_లెక్క;
};

నిర్మాణం strbuf_కణుపు {
        size_t విలువ_off;
        size_t విలువ_len;

        నిర్మాణం strbuf_బిడ్డ_నమోదు *children;
        uint8_t children_లెక్క;
};

నిర్మాణం strbuf_బిడ్డ_నమోదు {
        uint8_t c;
        నిర్మాణం strbuf_కణుపు *బిడ్డ;
};

నిర్మాణం strbuf* strbuf_new(శూన్య);
ssize_t strbuf_కూడు_string_నిండిన(నిర్మాణం strbuf *str, స్థిరరాశి అక్షరం *s, size_t len);
స్థిర అంతఃవరుస ssize_t strbuf_కూడు_string(నిర్మాణం strbuf *str, స్థిరరాశి అక్షరం *s) {
        తిరిగి strbuf_కూడు_string_నిండిన(str, s, SIZE_MAX);
}
శూన్య strbuf_complete(నిర్మాణం strbuf *str);
నిర్మాణం strbuf* strbuf_free(నిర్మాణం strbuf *str);
DEFINE_TRIVIAL_CLEANUP_FUNC(నిర్మాణం strbuf*, strbuf_free);
