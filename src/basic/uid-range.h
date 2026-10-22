/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना UIDRange_प्रविष्टिः {
        uid_t start, nr;
} UIDRange_प्रविष्टिः;

प्रकारनिर्देश संरचना UIDRange {
        UIDRange_प्रविष्टिः *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

पूर्णाङ्क uid_range_सङ्कलय_आन्तरिकम्(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
स्थिर अन्तःस्थ पूर्णाङ्क uid_range_सङ्कलय(UIDRange **range, uid_t start, uid_t nr) {
        प्रत्यागम uid_range_सङ्कलय_आन्तरिकम्(range, start, nr, true);
}
पूर्णाङ्क uid_range_सङ्कलय_str_पूर्णम्(UIDRange **range, ध्रुव वर्ण *s, bool coalesce);
स्थिर अन्तःस्थ पूर्णाङ्क uid_range_सङ्कलय_str(UIDRange **range, ध्रुव वर्ण *s) {
        प्रत्यागम uid_range_सङ्कलय_str_पूर्णम्(range, s, true);
}

पूर्णाङ्क uid_range_अग्र_अधस्तनम्(ध्रुव UIDRange *range, uid_t *uid);

bool uid_range_covers(ध्रुव UIDRange *range, uid_t start, uid_t nr);
स्थिर अन्तःस्थ bool uid_range_अन्तर्भवति(ध्रुव UIDRange *range, uid_t uid) {
        प्रत्यागम uid_range_covers(range, uid, 1);
}

पूर्णाङ्क uid_map_read_one(FILE *f, uid_t *ret_आधारः, uid_t *ret_सरणम्, uid_t *ret_range);

स्थिर अन्तःस्थ size_t uid_range_entries(ध्रुव UIDRange *range) {
        प्रत्यागम range ? range->n_entries : 0;
}

अचिह्नित uid_range_आकार(ध्रुव UIDRange *range) विशुद्धम्;
bool uid_range_is_रिक्तानि(ध्रुव UIDRange *range) विशुद्धम्;

bool uid_range_equal(ध्रुव UIDRange *a, ध्रुव UIDRange *b);

प्रकारनिर्देश गणना UIDRange_Userns_बहुलकम् {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_बहुलकम्_MAX,
        UID_RANGE_USERNS_बहुलकम्_INVALID = -EINVAL,
} UIDRange_Userns_बहुलकम्;

पूर्णाङ्क uid_range_आरोपय_userns_पूर्णम्(ध्रुव वर्ण *मार्गः, UIDRange_Userns_बहुलकम् बहुलकम्, bool coalesce, UIDRange **ret);
स्थिर अन्तःस्थ पूर्णाङ्क uid_range_आरोपय_userns(ध्रुव वर्ण *मार्गः, UIDRange_Userns_बहुलकम् बहुलकम्, UIDRange **ret) {
        प्रत्यागम uid_range_आरोपय_userns_पूर्णम्(मार्गः, बहुलकम्, true, ret);
}
पूर्णाङ्क uid_range_आरोपय_userns_by_fd_पूर्णम्(पूर्णाङ्क userns_fd, UIDRange_Userns_बहुलकम् बहुलकम्, bool coalesce, UIDRange **ret);
स्थिर अन्तःस्थ पूर्णाङ्क uid_range_आरोपय_userns_by_fd(पूर्णाङ्क userns_fd, UIDRange_Userns_बहुलकम् बहुलकम्, UIDRange **ret) {
        प्रत्यागम uid_range_आरोपय_userns_by_fd_पूर्णम्(userns_fd, बहुलकम्, true, ret);
}

bool uid_range_overlaps(ध्रुव UIDRange *range, uid_t start, uid_t nr);

पूर्णाङ्क uid_range_clip(UIDRange *range, uid_t min, uid_t max);
पूर्णाङ्क uid_range_partition(UIDRange *range, uid_t आकार);
पूर्णाङ्क uid_range_copy(ध्रुव UIDRange *range, UIDRange **ret);
पूर्णाङ्क uid_range_remove(UIDRange *range, uid_t start, uid_t आकार);
पूर्णाङ्क uid_range_translate(ध्रुव UIDRange *outside, ध्रुव UIDRange *inside, uid_t uid, uid_t *ret);
पूर्णाङ्क uid_range_translate_userns_fd(पूर्णाङ्क userns_fd, UIDRange_Userns_बहुलकम् बहुलकम्, uid_t uid, uid_t *ret);

पूर्णाङ्क uid_map_अन्वेषणम्_मूलम्(pid_t pid, UIDRange_Userns_बहुलकम् बहुलकम्, uid_t *ret);

uid_t uid_range_आधारः(ध्रुव UIDRange *range);
