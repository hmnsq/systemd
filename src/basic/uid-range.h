/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం నిర్మాణం UIDRange_నమోదు {
        uid_t ప్రారంభం, nr;
} UIDRange_నమోదు;

రకనిర్వచనం నిర్మాణం UIDRange {
        UIDRange_నమోదు *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

పూర్ణాంకం uid_range_కూడు_అంతర్గత(UIDRange **range, uid_t ప్రారంభం, uid_t nr, bool coalesce);
స్థిర అంతఃవరుస పూర్ణాంకం uid_range_కూడు(UIDRange **range, uid_t ప్రారంభం, uid_t nr) {
        తిరిగి uid_range_కూడు_అంతర్గత(range, ప్రారంభం, nr, true);
}
పూర్ణాంకం uid_range_కూడు_str_నిండిన(UIDRange **range, స్థిరరాశి అక్షరం *s, bool coalesce);
స్థిర అంతఃవరుస పూర్ణాంకం uid_range_కూడు_str(UIDRange **range, స్థిరరాశి అక్షరం *s) {
        తిరిగి uid_range_కూడు_str_నిండిన(range, s, true);
}

పూర్ణాంకం uid_range_తదుపరి_క్రింది(స్థిరరాశి UIDRange *range, uid_t *uid);

bool uid_range_covers(స్థిరరాశి UIDRange *range, uid_t ప్రారంభం, uid_t nr);
స్థిర అంతఃవరుస bool uid_range_కలిగియుంది(స్థిరరాశి UIDRange *range, uid_t uid) {
        తిరిగి uid_range_covers(range, uid, 1);
}

పూర్ణాంకం uid_map_read_one(FILE *f, uid_t *ret_పునాది, uid_t *ret_జరుపు, uid_t *ret_range);

స్థిర అంతఃవరుస size_t uid_range_entries(స్థిరరాశి UIDRange *range) {
        తిరిగి range ? range->n_entries : 0;
}

గుర్తులేని uid_range_కొలత(స్థిరరాశి UIDRange *range) శుద్ధ;
bool uid_range_is_ఖాళీలు(స్థిరరాశి UIDRange *range) శుద్ధ;

bool uid_range_equal(స్థిరరాశి UIDRange *a, స్థిరరాశి UIDRange *b);

రకనిర్వచనం గణన UIDRange_Userns_బహుళకం {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_బహుళకం_MAX,
        UID_RANGE_USERNS_బహుళకం_INVALID = -EINVAL,
} UIDRange_Userns_బహుళకం;

పూర్ణాంకం uid_range_ఎక్కించు_userns_నిండిన(స్థిరరాశి అక్షరం *మార్గం, UIDRange_Userns_బహుళకం బహుళకం, bool coalesce, UIDRange **ret);
స్థిర అంతఃవరుస పూర్ణాంకం uid_range_ఎక్కించు_userns(స్థిరరాశి అక్షరం *మార్గం, UIDRange_Userns_బహుళకం బహుళకం, UIDRange **ret) {
        తిరిగి uid_range_ఎక్కించు_userns_నిండిన(మార్గం, బహుళకం, true, ret);
}
పూర్ణాంకం uid_range_ఎక్కించు_userns_by_fd_నిండిన(పూర్ణాంకం userns_fd, UIDRange_Userns_బహుళకం బహుళకం, bool coalesce, UIDRange **ret);
స్థిర అంతఃవరుస పూర్ణాంకం uid_range_ఎక్కించు_userns_by_fd(పూర్ణాంకం userns_fd, UIDRange_Userns_బహుళకం బహుళకం, UIDRange **ret) {
        తిరిగి uid_range_ఎక్కించు_userns_by_fd_నిండిన(userns_fd, బహుళకం, true, ret);
}

bool uid_range_overlaps(స్థిరరాశి UIDRange *range, uid_t ప్రారంభం, uid_t nr);

పూర్ణాంకం uid_range_clip(UIDRange *range, uid_t min, uid_t max);
పూర్ణాంకం uid_range_విభజన(UIDRange *range, uid_t కొలత);
పూర్ణాంకం uid_range_copy(స్థిరరాశి UIDRange *range, UIDRange **ret);
పూర్ణాంకం uid_range_remove(UIDRange *range, uid_t ప్రారంభం, uid_t కొలత);
పూర్ణాంకం uid_range_translate(స్థిరరాశి UIDRange *outside, స్థిరరాశి UIDRange *inside, uid_t uid, uid_t *ret);
పూర్ణాంకం uid_range_translate_userns_fd(పూర్ణాంకం userns_fd, UIDRange_Userns_బహుళకం బహుళకం, uid_t uid, uid_t *ret);

పూర్ణాంకం uid_map_శోధన_మూలం(pid_t pid, UIDRange_Userns_బహుళకం బహుళకం, uid_t *ret);

uid_t uid_range_పునాది(స్థిరరాశి UIDRange *range);
