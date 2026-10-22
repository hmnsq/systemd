/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ fopen_temporary_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret_file, ಅಕ್ಷರ **ret_ಮಾರ್ಗ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fopen_temporary(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret_file, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ fopen_temporary_at(AT_FDCWD, ಮಾರ್ಗ, ret_file, ret_ಮಾರ್ಗ);
}

ಪೂರ್ಣಾಂಕ fopen_temporary_ಮಗು_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret_file, ಅಕ್ಷರ **ret_ಮಾರ್ಗ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fopen_temporary_ಮಗು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, FILE **ret_file, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ fopen_temporary_ಮಗು_at(AT_FDCWD, ಮಾರ್ಗ, ret_file, ret_ಮಾರ್ಗ);
}

ಪೂರ್ಣಾಂಕ mkostemp_ಸುರಕ್ಷಿತ(ಅಕ್ಷರ *ನಮೂನೆ);
ಪೂರ್ಣಾಂಕ fmkostemp_ಸುರಕ್ಷಿತ(ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE**_f);

ಶೂನ್ಯ unlink_tempfilep(ಅಕ್ಷರ (*p)[]);

ಪೂರ್ಣಾಂಕ tempfn_xxxxxx(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ tempfn_random(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ tempfn_random_ಮಗು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ open_tmpfile_unlinkable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *directory, ಪೂರ್ಣಾಂಕ flags);
ಪೂರ್ಣಾಂಕ open_tmpfile_linkable_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ open_tmpfile_linkable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ open_tmpfile_linkable_at(AT_FDCWD, ಗುರಿ, flags, ret_ಮಾರ್ಗ);
}
ಪೂರ್ಣಾಂಕ fopen_tmpfile_linkable_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, FILE **ret_file);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fopen_tmpfile_linkable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, FILE **ret_file) {
        ಹಿಂತಿರುಗಿ fopen_tmpfile_linkable_at(AT_FDCWD, ಗುರಿ, flags, ret_ಮಾರ್ಗ, ret_file);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

ಪೂರ್ಣಾಂಕ link_tmpfile_at(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, LinkTmpfileFlags flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ link_tmpfile(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, LinkTmpfileFlags flags) {
        ಹಿಂತಿರುಗಿ link_tmpfile_at(fd, AT_FDCWD, ಮಾರ್ಗ, ಗುರಿ, flags);
}
ಪೂರ್ಣಾಂಕ flink_tmpfile_at(FILE *f, ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, LinkTmpfileFlags flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ flink_tmpfile(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, LinkTmpfileFlags flags) {
        ಹಿಂತಿರುಗಿ flink_tmpfile_at(f, AT_FDCWD, ಮಾರ್ಗ, ಗುರಿ, flags);
}

ಪೂರ್ಣಾಂಕ mkdtemp_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾದರಿ, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ mkdtemp_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾದರಿ, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
ರಚನೆ cleanup_tmpfile_ದತ್ತಾಂಶ {
        ಪೂರ್ಣಾಂಕ *dir_fd;
        ಅಕ್ಷರ **filename;
};
ಶೂನ್ಯ cleanup_tmpfile_ದತ್ತಾಂಶ_ಮುಗಿದ(ರಚನೆ cleanup_tmpfile_ದತ್ತಾಂಶ *d);
#ವ್ಯಾಖ್ಯಾನಿಸು _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        ಬಳಸದ _cleanup_(cleanup_tmpfile_ದತ್ತಾಂಶ_ಮುಗಿದ) ರಚನೆ cleanup_tmpfile_ದತ್ತಾಂಶ UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#ವ್ಯಾಖ್ಯಾನಿಸು CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
