/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ fopen_temporary_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret_file, ਅੱਖਰ **ret_ਰਸਤਾ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fopen_temporary(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret_file, ਅੱਖਰ **ret_ਰਸਤਾ) {
        ਵਾਪਸ fopen_temporary_at(AT_FDCWD, ਰਸਤਾ, ret_file, ret_ਰਸਤਾ);
}

ਪੂਰਨ fopen_temporary_ਬੱਚਾ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret_file, ਅੱਖਰ **ret_ਰਸਤਾ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fopen_temporary_ਬੱਚਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, FILE **ret_file, ਅੱਖਰ **ret_ਰਸਤਾ) {
        ਵਾਪਸ fopen_temporary_ਬੱਚਾ_at(AT_FDCWD, ਰਸਤਾ, ret_file, ret_ਰਸਤਾ);
}

ਪੂਰਨ mkostemp_ਸੁਰੱਖਿਅਤ(ਅੱਖਰ *ਨਮੂਨਾ);
ਪੂਰਨ fmkostemp_ਸੁਰੱਖਿਅਤ(ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE**_f);

ਖਾਲੀ unlink_tempfilep(ਅੱਖਰ (*p)[]);

ਪੂਰਨ tempfn_xxxxxx(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *extra, ਅੱਖਰ **ret);
ਪੂਰਨ tempfn_random(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *extra, ਅੱਖਰ **ret);
ਪੂਰਨ tempfn_random_ਬੱਚਾ(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *extra, ਅੱਖਰ **ret);

ਪੂਰਨ open_tmpfile_unlinkable(ਅਚਲ ਅੱਖਰ *directory, ਪੂਰਨ flags);
ਪੂਰਨ open_tmpfile_linkable_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ flags, ਅੱਖਰ **ret_ਰਸਤਾ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ open_tmpfile_linkable(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ flags, ਅੱਖਰ **ret_ਰਸਤਾ) {
        ਵਾਪਸ open_tmpfile_linkable_at(AT_FDCWD, ਨਿਸ਼ਾਨਾ, flags, ret_ਰਸਤਾ);
}
ਪੂਰਨ fopen_tmpfile_linkable_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ flags, ਅੱਖਰ **ret_ਰਸਤਾ, FILE **ret_file);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fopen_tmpfile_linkable(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ flags, ਅੱਖਰ **ret_ਰਸਤਾ, FILE **ret_file) {
        ਵਾਪਸ fopen_tmpfile_linkable_at(AT_FDCWD, ਨਿਸ਼ਾਨਾ, flags, ret_ਰਸਤਾ, ret_file);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

ਪੂਰਨ link_tmpfile_at(ਪੂਰਨ fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, LinkTmpfileFlags flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ link_tmpfile(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, LinkTmpfileFlags flags) {
        ਵਾਪਸ link_tmpfile_at(fd, AT_FDCWD, ਰਸਤਾ, ਨਿਸ਼ਾਨਾ, flags);
}
ਪੂਰਨ flink_tmpfile_at(FILE *f, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, LinkTmpfileFlags flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ flink_tmpfile(FILE *f, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, LinkTmpfileFlags flags) {
        ਵਾਪਸ flink_tmpfile_at(f, AT_FDCWD, ਰਸਤਾ, ਨਿਸ਼ਾਨਾ, flags);
}

ਪੂਰਨ mkdtemp_malloc(ਅਚਲ ਅੱਖਰ *ਸਾਂਚਾ, ਅੱਖਰ **ret);
ਪੂਰਨ mkdtemp_open(ਅਚਲ ਅੱਖਰ *ਸਾਂਚਾ, ਪੂਰਨ flags, ਅੱਖਰ **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
ਬਣਤਰ cleanup_tmpfile_ਅੰਕੜੇ {
        ਪੂਰਨ *dir_fd;
        ਅੱਖਰ **filename;
};
ਖਾਲੀ cleanup_tmpfile_ਅੰਕੜੇ_ਪੂਰਾ(ਬਣਤਰ cleanup_tmpfile_ਅੰਕੜੇ *d);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        ਅਣਵਰਤਿਆ _cleanup_(cleanup_tmpfile_ਅੰਕੜੇ_ਪੂਰਾ) ਬਣਤਰ cleanup_tmpfile_ਅੰਕੜੇ UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
