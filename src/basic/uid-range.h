/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UIDRange_ਇੰਦਰਾਜ {
        uid_t ਸ਼ੁਰੂ, nr;
} UIDRange_ਇੰਦਰਾਜ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UIDRange {
        UIDRange_ਇੰਦਰਾਜ *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

ਪੂਰਨ uid_range_ਜੋੜੋ_ਅੰਦਰੂਨੀ(UIDRange **range, uid_t ਸ਼ੁਰੂ, uid_t nr, bool coalesce);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ uid_range_ਜੋੜੋ(UIDRange **range, uid_t ਸ਼ੁਰੂ, uid_t nr) {
        ਵਾਪਸ uid_range_ਜੋੜੋ_ਅੰਦਰੂਨੀ(range, ਸ਼ੁਰੂ, nr, true);
}
ਪੂਰਨ uid_range_ਜੋੜੋ_str_ਭਰਿਆ(UIDRange **range, ਅਚਲ ਅੱਖਰ *s, bool coalesce);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ uid_range_ਜੋੜੋ_str(UIDRange **range, ਅਚਲ ਅੱਖਰ *s) {
        ਵਾਪਸ uid_range_ਜੋੜੋ_str_ਭਰਿਆ(range, s, true);
}

ਪੂਰਨ uid_range_ਅਗਲਾ_ਹੇਠਲਾ(ਅਚਲ UIDRange *range, uid_t *uid);

bool uid_range_covers(ਅਚਲ UIDRange *range, uid_t ਸ਼ੁਰੂ, uid_t nr);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool uid_range_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ UIDRange *range, uid_t uid) {
        ਵਾਪਸ uid_range_covers(range, uid, 1);
}

ਪੂਰਨ uid_map_read_one(FILE *f, uid_t *ret_ਅਧਾਰ, uid_t *ret_ਖਿਸਕਾਓ, uid_t *ret_range);

ਸਥਿਰ ਸਤਰਅੰਦਰ size_t uid_range_entries(ਅਚਲ UIDRange *range) {
        ਵਾਪਸ range ? range->n_entries : 0;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ uid_range_ਮਾਪ(ਅਚਲ UIDRange *range) ਸ਼ੁੱਧ;
bool uid_range_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ UIDRange *range) ਸ਼ੁੱਧ;

bool uid_range_equal(ਅਚਲ UIDRange *a, ਅਚਲ UIDRange *b);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UIDRange_Userns_ਬਹੁਲਕ {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_ਬਹੁਲਕ_MAX,
        UID_RANGE_USERNS_ਬਹੁਲਕ_INVALID = -EINVAL,
} UIDRange_Userns_ਬਹੁਲਕ;

ਪੂਰਨ uid_range_ਲੱਦੋ_userns_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, bool coalesce, UIDRange **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ uid_range_ਲੱਦੋ_userns(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, UIDRange **ret) {
        ਵਾਪਸ uid_range_ਲੱਦੋ_userns_ਭਰਿਆ(ਰਸਤਾ, ਬਹੁਲਕ, true, ret);
}
ਪੂਰਨ uid_range_ਲੱਦੋ_userns_by_fd_ਭਰਿਆ(ਪੂਰਨ userns_fd, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, bool coalesce, UIDRange **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ uid_range_ਲੱਦੋ_userns_by_fd(ਪੂਰਨ userns_fd, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, UIDRange **ret) {
        ਵਾਪਸ uid_range_ਲੱਦੋ_userns_by_fd_ਭਰਿਆ(userns_fd, ਬਹੁਲਕ, true, ret);
}

bool uid_range_overlaps(ਅਚਲ UIDRange *range, uid_t ਸ਼ੁਰੂ, uid_t nr);

ਪੂਰਨ uid_range_clip(UIDRange *range, uid_t min, uid_t max);
ਪੂਰਨ uid_range_ਹਿੱਸੇ_ਕਰੋ(UIDRange *range, uid_t ਮਾਪ);
ਪੂਰਨ uid_range_copy(ਅਚਲ UIDRange *range, UIDRange **ret);
ਪੂਰਨ uid_range_remove(UIDRange *range, uid_t ਸ਼ੁਰੂ, uid_t ਮਾਪ);
ਪੂਰਨ uid_range_translate(ਅਚਲ UIDRange *outside, ਅਚਲ UIDRange *inside, uid_t uid, uid_t *ret);
ਪੂਰਨ uid_range_translate_userns_fd(ਪੂਰਨ userns_fd, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, uid_t uid, uid_t *ret);

ਪੂਰਨ uid_map_ਖੋਜ_ਜੜ੍ਹ(pid_t pid, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, uid_t *ret);

uid_t uid_range_ਅਧਾਰ(ਅਚਲ UIDRange *range);
