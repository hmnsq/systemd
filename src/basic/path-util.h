/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_ವಿಭಜಿಸು_BIN(x) x "sbin:" x "bin"
#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_ವಿಭಜಿಸು_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_MERGED_BIN(x) x "bin"
#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_MERGED_BIN_NULSTR(x) x "bin\0"

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಮಾರ್ಗ_WITH_ತುಂಬಿದ_SBIN ಮಾರ್ಗ_ವಿಭಜಿಸು_BIN("/usr/local/") ":" ಮಾರ್ಗ_ವಿಭಜಿಸು_BIN("/usr/") EXTRA_EXEC_ಹುಡುಕು_ಮಾರ್ಗ_SUFFIX
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಮಾರ್ಗ_WITH_LOCAL_SBIN ಮಾರ್ಗ_ವಿಭಜಿಸು_BIN("/usr/local/") ":" ಮಾರ್ಗ_MERGED_BIN("/usr/") EXTRA_EXEC_ಹುಡುಕು_ಮಾರ್ಗ_SUFFIX
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಮಾರ್ಗ_WITHOUT_SBIN ಮಾರ್ಗ_MERGED_BIN("/usr/local/") ":" ಮಾರ್ಗ_MERGED_BIN("/usr/") EXTRA_EXEC_ಹುಡುಕು_ಮಾರ್ಗ_SUFFIX

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ಮಾರ್ಗ_COMPAT ಮಾರ್ಗ_ವಿಭಜಿಸು_BIN("/usr/local/") ":" ಮಾರ್ಗ_ವಿಭಜಿಸು_BIN("/usr/") ":" ಮಾರ್ಗ_ವಿಭಜಿಸು_BIN("/") EXTRA_EXEC_ಹುಡುಕು_ಮಾರ್ಗ_SUFFIX

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* default_ಮಾರ್ಗ(ಶೂನ್ಯ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* default_user_ಮಾರ್ಗ(ಶೂನ್ಯ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEFAULT_USER_ಮಾರ್ಗ
        ಹಿಂತಿರುಗಿ DEFAULT_USER_ಮಾರ್ಗ EXTRA_EXEC_ಹುಡುಕು_ಮಾರ್ಗ_SUFFIX;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ default_ಮಾರ್ಗ();
#ಆದರೆ_ಅಂತ್ಯ
}

bool is_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಮಾರ್ಗ_is_absolute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಆದರೆ (!p) /* A NULL pointer is definitely not an absolute path */
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ p[0] == '/';
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ವಿಭಜಿಸು_and_make_absolute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ ***ret);
ಅಕ್ಷರ* ಮಾರ್ಗ_make_absolute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ);
ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_getcwd(ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_make_absolute_cwd(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_make_relative(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *from, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *to, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_make_relative_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *from_ಮಗು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *to, ಅಕ್ಷರ **ret);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಮಾರ್ಗ_ಆರಂಭ_With_Flags {
        ಮಾರ್ಗ_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        ಮಾರ್ಗ_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} ಮಾರ್ಗ_ಆರಂಭ_With_Flags;

ಅಕ್ಷರ* ಮಾರ್ಗ_startswith_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ_ಆರಂಭ_With_Flags flags) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* ಮಾರ್ಗ_startswith(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_startswith_ತುಂಬಿದ(ಮಾರ್ಗ, ಪೂರ್ವಪ್ರತ್ಯಯ, 0);
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_compare(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಮಾರ್ಗ_equal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_compare(a, b) == 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_compare_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಮಾರ್ಗ_equal_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_compare_filename(a, b) == 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_equal_or_inode_same_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಮಾರ್ಗ_equal_or_inode_same(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b, ಪೂರ್ಣಾಂಕ flags) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_equal_or_inode_same_ತುಂಬಿದ(a, b, flags) > 0;
}

ಅಕ್ಷರ* ಮಾರ್ಗ_extend_ಆಂತರಿಕ(ಅಕ್ಷರ **x, ...);
#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_extend(x, ...) ಮಾರ್ಗ_extend_ಆಂತರಿಕ(x, __VA_ARGS__, ಸೂಚಕ_MAX)
#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_join(...) ಮಾರ್ಗ_extend_ಆಂತರಿಕ(ಖಾಲಿ, __VA_ARGS__, ಸೂಚಕ_MAX)

ಅಕ್ಷರ* skip_leading_slash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಮಾರ್ಗ_Simplify_Flags {
        ಮಾರ್ಗ_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} ಮಾರ್ಗ_Simplify_Flags;

ಅಕ್ಷರ* ಮಾರ್ಗ_simplify_ತುಂಬಿದ(ಅಕ್ಷರ *ಮಾರ್ಗ, ಮಾರ್ಗ_Simplify_Flags flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* ಮಾರ್ಗ_simplify(ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_simplify_ತುಂಬಿದ(ಮಾರ್ಗ, 0);
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_simplify_alloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret);

/* Note: the search terminates on the first NULL item. */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_ಒಳ_ಗಣ(p, ...) ಮಾರ್ಗ_strv_ಒಳಗೊಂಡಿದೆ(STRV_MAKE(__VA_ARGS__), p)

ಅಕ್ಷರ* ಮಾರ್ಗ_startswith_strv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *strv);
#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_STARTSWITH_ಗಣ(p, ...) ಮಾರ್ಗ_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_strv_make_absolute_cwd(ಅಕ್ಷರ **l);
ಅಕ್ಷರ** ಮಾರ್ಗ_strv_resolve(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು);
ಅಕ್ಷರ** ಮಾರ್ಗ_strv_resolve_uniq(ಅಕ್ಷರ **l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು);

ಪೂರ್ಣಾಂಕ open_and_check_executable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ret_ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret_fd);
ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_executable_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *exec_ಹುಡುಕು_ಮಾರ್ಗ,
                bool use_ಮಾರ್ಗ_envvar,
                ಅಕ್ಷರ **ret_filename,
                ಪೂರ್ಣಾಂಕ *ret_fd);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_executable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret_filename) {
        ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_executable_ತುಂಬಿದ(name, /* root= */ ಖಾಲಿ, ಖಾಲಿ, true, ret_filename, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ fsck_exists(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ fsck_exists_for_fstype(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_FOREACH_ಪೂರ್ವಪ್ರತ್ಯಯ(ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ)                               \
        ಗಾಗಿ (ಅಕ್ಷರ *_slash = ({                                          \
                                ಮಾರ್ಗ_simplify(strcpy(ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ));    \
                                streq(ಪೂರ್ವಪ್ರತ್ಯಯ, "/") ? ಖಾಲಿ : strrchr(ಪೂರ್ವಪ್ರತ್ಯಯ, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((ಪೂರ್ವಪ್ರತ್ಯಯ), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#ವ್ಯಾಖ್ಯಾನಿಸು ಮಾರ್ಗ_FOREACH_ಪೂರ್ವಪ್ರತ್ಯಯ_ಇನ್ನಷ್ಟು(ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ)                          \
        ಗಾಗಿ (ಅಕ್ಷರ *_slash = ({                                          \
                                ಮಾರ್ಗ_simplify(strcpy(ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ));    \
                                ಆದರೆ (streq(ಪೂರ್ವಪ್ರತ್ಯಯ, "/"))                 \
                                        ಪೂರ್ವಪ್ರತ್ಯಯ[0] = 0;                  \
                                strrchr(ಪೂರ್ವಪ್ರತ್ಯಯ, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((ಪೂರ್ವಪ್ರತ್ಯಯ), '/'))

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, bool accept_dot_dot, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_component(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool accept_dot_dot, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮುಂದಿನ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೊನೆಯ_ಮಾರ್ಗ_component(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ವಿಭಜಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_dir, ಅಕ್ಷರ **ret_filename);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_ವಿಭಜಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ_filename(ಮಾರ್ಗ, ಖಾಲಿ, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r = ಮಾರ್ಗ_ವಿಭಜಿಸು_ಪೂರ್ವಪ್ರತ್ಯಯ_filename(ಮಾರ್ಗ, ret, ಖಾಲಿ);
        ಹಿಂತಿರುಗಿ r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;
bool filename_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;
bool ಮಾರ್ಗ_is_valid_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, bool accept_dot_dot) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಮಾರ್ಗ_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_is_valid_ತುಂಬಿದ(p, /* accept_dot_dot= */ true);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಮಾರ್ಗ_is_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_is_valid_ತುಂಬಿದ(p, /* accept_dot_dot= */ false);
}
bool ಮಾರ್ಗ_is_normalized(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool filename_or_absolute_ಮಾರ್ಗ_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಆದರೆ (ಮಾರ್ಗ_is_absolute(p))
                ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_is_valid(p);

        ಹಿಂತಿರುಗಿ filename_is_valid(p);
}

ಪೂರ್ಣಾಂಕ file_ಒಳ_same_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ret);

bool hidden_or_backup_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename) ಶುದ್ಧ;

bool is_device_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) ಶುದ್ಧ;

bool valid_device_ಗಂಟು_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) ಶುದ್ಧ;
bool valid_device_allow_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) ಶುದ್ಧ;

bool dot_or_dot_dot(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) ಶುದ್ಧ;

bool ಮಾರ್ಗ_ಸೂಚಿಸುತ್ತದೆ_directory(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) ಶುದ್ಧ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* skip_dev_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        /* Drop any /dev prefix if there is any */

        e = ಮಾರ್ಗ_startswith(p, "/dev/");

        ಹಿಂತಿರುಗಿ e ?: p;
}

bool ಖಾಲಿಗಳು_or_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) ಶುದ್ಧ;
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ ಖಾಲಿಗಳು_or_ಬೇರು_harder_ಗಮ್ಯ_null(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮಾರ್ಗ);

bool ಮಾರ್ಗ_strv_ಒಳಗೊಂಡಿದೆ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);
bool prefixed_ಮಾರ್ಗ_strv_ಒಳಗೊಂಡಿದೆ(ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_glob_can_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಅಕ್ಷರ **ret);
