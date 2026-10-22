/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UIDRange_ನಮೂದು {
        uid_t ಆರಂಭ, nr;
} UIDRange_ನಮೂದು;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UIDRange {
        UIDRange_ನಮೂದು *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

ಪೂರ್ಣಾಂಕ uid_range_ಕೂಡಿಸು_ಆಂತರಿಕ(UIDRange **range, uid_t ಆರಂಭ, uid_t nr, bool coalesce);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ uid_range_ಕೂಡಿಸು(UIDRange **range, uid_t ಆರಂಭ, uid_t nr) {
        ಹಿಂತಿರುಗಿ uid_range_ಕೂಡಿಸು_ಆಂತರಿಕ(range, ಆರಂಭ, nr, true);
}
ಪೂರ್ಣಾಂಕ uid_range_ಕೂಡಿಸು_str_ತುಂಬಿದ(UIDRange **range, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, bool coalesce);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ uid_range_ಕೂಡಿಸು_str(UIDRange **range, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಹಿಂತಿರುಗಿ uid_range_ಕೂಡಿಸು_str_ತುಂಬಿದ(range, s, true);
}

ಪೂರ್ಣಾಂಕ uid_range_ಮುಂದಿನ_ಅಧೋ(ಸ್ಥಿರಾಂಕ UIDRange *range, uid_t *uid);

bool uid_range_covers(ಸ್ಥಿರಾಂಕ UIDRange *range, uid_t ಆರಂಭ, uid_t nr);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool uid_range_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ UIDRange *range, uid_t uid) {
        ಹಿಂತಿರುಗಿ uid_range_covers(range, uid, 1);
}

ಪೂರ್ಣಾಂಕ uid_map_read_one(FILE *f, uid_t *ret_ತಳ, uid_t *ret_ಸರಿಸು, uid_t *ret_range);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t uid_range_entries(ಸ್ಥಿರಾಂಕ UIDRange *range) {
        ಹಿಂತಿರುಗಿ range ? range->n_entries : 0;
}

ಚಿಹ್ನೆರಹಿತ uid_range_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ UIDRange *range) ಶುದ್ಧ;
bool uid_range_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ UIDRange *range) ಶುದ್ಧ;

bool uid_range_equal(ಸ್ಥಿರಾಂಕ UIDRange *a, ಸ್ಥಿರಾಂಕ UIDRange *b);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UIDRange_Userns_ಬಹುಲಕ {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_ಬಹುಲಕ_MAX,
        UID_RANGE_USERNS_ಬಹುಲಕ_INVALID = -EINVAL,
} UIDRange_Userns_ಬಹುಲಕ;

ಪೂರ್ಣಾಂಕ uid_range_ತುಂಬು_userns_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, bool coalesce, UIDRange **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ uid_range_ತುಂಬು_userns(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, UIDRange **ret) {
        ಹಿಂತಿರುಗಿ uid_range_ತುಂಬು_userns_ತುಂಬಿದ(ಮಾರ್ಗ, ಬಹುಲಕ, true, ret);
}
ಪೂರ್ಣಾಂಕ uid_range_ತುಂಬು_userns_by_fd_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ userns_fd, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, bool coalesce, UIDRange **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ uid_range_ತುಂಬು_userns_by_fd(ಪೂರ್ಣಾಂಕ userns_fd, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, UIDRange **ret) {
        ಹಿಂತಿರುಗಿ uid_range_ತುಂಬು_userns_by_fd_ತುಂಬಿದ(userns_fd, ಬಹುಲಕ, true, ret);
}

bool uid_range_overlaps(ಸ್ಥಿರಾಂಕ UIDRange *range, uid_t ಆರಂಭ, uid_t nr);

ಪೂರ್ಣಾಂಕ uid_range_clip(UIDRange *range, uid_t min, uid_t max);
ಪೂರ್ಣಾಂಕ uid_range_ವಿಭಜನೆ(UIDRange *range, uid_t ಗಾತ್ರ_2);
ಪೂರ್ಣಾಂಕ uid_range_copy(ಸ್ಥಿರಾಂಕ UIDRange *range, UIDRange **ret);
ಪೂರ್ಣಾಂಕ uid_range_remove(UIDRange *range, uid_t ಆರಂಭ, uid_t ಗಾತ್ರ_2);
ಪೂರ್ಣಾಂಕ uid_range_translate(ಸ್ಥಿರಾಂಕ UIDRange *outside, ಸ್ಥಿರಾಂಕ UIDRange *inside, uid_t uid, uid_t *ret);
ಪೂರ್ಣಾಂಕ uid_range_translate_userns_fd(ಪೂರ್ಣಾಂಕ userns_fd, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, uid_t uid, uid_t *ret);

ಪೂರ್ಣಾಂಕ uid_map_ಹುಡುಕು_ಬೇರು(pid_t pid, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, uid_t *ret);

uid_t uid_range_ತಳ(ಸ್ಥಿರಾಂಕ UIDRange *range);
