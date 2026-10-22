/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_ಹುಡುಕು_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_ಹುಡುಕು_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_ಹುಡುಕು_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_ಹುಡುಕು_PATH_SUFFIX

ಧ್ರುವ ವರ್ಣ* default_PATH(ಶೂನ್ಯ);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* default_user_PATH(ಶೂನ್ಯ) {
#ifdef DEFAULT_USER_PATH
        ಪ್ರತ್ಯಾಗಮ DEFAULT_USER_PATH EXTRA_EXEC_ಹುಡುಕು_PATH_SUFFIX;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ default_PATH();
#endif
}

bool is_path(ಧ್ರುವ ವರ್ಣ *p) _pure_;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool path_is_absolute(ಧ್ರುವ ವರ್ಣ *p) {
        ಯದಿ (!p) /* A NULL pointer is definitely not an absolute path */
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ p[0] == '/';
}

ಪೂರ್ಣಾಙ್ಕ path_split_and_make_absolute(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ ***ret);
ವರ್ಣ* path_make_absolute(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *prefix);
ಪೂರ್ಣಾಙ್ಕ safe_getcwd(ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ path_make_absolute_cwd(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ path_make_relative(ಧ್ರುವ ವರ್ಣ *from, ಧ್ರುವ ವರ್ಣ *to, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ path_make_relative_parent(ಧ್ರುವ ವರ್ಣ *from_child, ಧ್ರುವ ವರ್ಣ *to, ವರ್ಣ **ret);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

ವರ್ಣ* path_startswith_full(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *prefix, PathStartWithFlags flags) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* path_startswith(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *prefix) {
        ಪ್ರತ್ಯಾಗಮ path_startswith_full(path, prefix, 0);
}

ಪೂರ್ಣಾಙ್ಕ path_compare(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool path_equal(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        ಪ್ರತ್ಯಾಗಮ path_compare(a, b) == 0;
}

ಪೂರ್ಣಾಙ್ಕ path_compare_filename(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool path_equal_filename(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b) {
        ಪ್ರತ್ಯಾಗಮ path_compare_filename(a, b) == 0;
}

ಪೂರ್ಣಾಙ್ಕ path_equal_or_inode_same_full(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b, ಪೂರ್ಣಾಙ್ಕ flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool path_equal_or_inode_same(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b, ಪೂರ್ಣಾಙ್ಕ flags) {
        ಪ್ರತ್ಯಾಗಮ path_equal_or_inode_same_full(a, b, flags) > 0;
}

ವರ್ಣ* path_extend_internal(ವರ್ಣ **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(ರಿಕ್ತ, __VA_ARGS__, POINTER_MAX)

ವರ್ಣ* skip_leading_slash(ಧ್ರುವ ವರ್ಣ *p) _pure_;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

ವರ್ಣ* path_simplify_full(ವರ್ಣ *path, PathSimplifyFlags flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* path_simplify(ವರ್ಣ *path) {
        ಪ್ರತ್ಯಾಗಮ path_simplify_full(path, 0);
}

ಪೂರ್ಣಾಙ್ಕ path_simplify_alloc(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

ವರ್ಣ* path_startswith_strv(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ * ಧ್ರುವ *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ಪೂರ್ಣಾಙ್ಕ path_strv_make_absolute_cwd(ವರ್ಣ **l);
ವರ್ಣ** path_strv_resolve(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *ಬೇರು);
ವರ್ಣ** path_strv_resolve_uniq(ವರ್ಣ **l, ಧ್ರುವ ವರ್ಣ *ಬೇರು);

ಪೂರ್ಣಾಙ್ಕ open_and_check_executable(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd);
ಪೂರ್ಣಾಙ್ಕ find_executable_full(
                ಧ್ರುವ ವರ್ಣ *name,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ವರ್ಣ * ಧ್ರುವ *exec_ಹುಡುಕು_path,
                bool use_path_envvar,
                ವರ್ಣ **ret_filename,
                ಪೂರ್ಣಾಙ್ಕ *ret_fd);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ find_executable(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret_filename) {
        ಪ್ರತ್ಯಾಗಮ find_executable_full(name, /* root= */ ರಿಕ್ತ, ರಿಕ್ತ, true, ret_filename, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ fsck_exists(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ fsck_exists_for_fstype(ಧ್ರುವ ವರ್ಣ *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        ಪ್ರತಿ (ವರ್ಣ *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? ರಿಕ್ತ : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        ಪ್ರತಿ (ವರ್ಣ *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                ಯದಿ (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

ಪೂರ್ಣಾಙ್ಕ path_find_first_component(ಧ್ರುವ ವರ್ಣ **p, bool accept_dot_dot, ಧ್ರುವ ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ path_find_last_component(ಧ್ರುವ ವರ್ಣ *path, bool accept_dot_dot, ಧ್ರುವ ವರ್ಣ **next, ಧ್ರುವ ವರ್ಣ **ret);
ಧ್ರುವ ವರ್ಣ* last_path_component(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ path_split_prefix_filename(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_dir, ವರ್ಣ **ret_filename);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ path_extract_filename(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ path_split_prefix_filename(path, ರಿಕ್ತ, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ path_extract_directory(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r = path_split_prefix_filename(path, ret, ರಿಕ್ತ);
        ಪ್ರತ್ಯಾಗಮ r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ಧ್ರುವ ವರ್ಣ *p) _pure_;
bool filename_is_valid(ಧ್ರುವ ವರ್ಣ *p) _pure_;
bool path_is_valid_full(ಧ್ರುವ ವರ್ಣ *p, bool accept_dot_dot) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool path_is_valid(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತ್ಯಾಗಮ path_is_valid_full(p, /* accept_dot_dot= */ true);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool path_is_safe(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತ್ಯಾಗಮ path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(ಧ್ರುವ ವರ್ಣ *p) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool filename_or_absolute_path_is_valid(ಧ್ರುವ ವರ್ಣ *p) {
        ಯದಿ (path_is_absolute(p))
                ಪ್ರತ್ಯಾಗಮ path_is_valid(p);

        ಪ್ರತ್ಯಾಗಮ filename_is_valid(p);
}

ಪೂರ್ಣಾಙ್ಕ file_in_same_dir(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *filename, ವರ್ಣ **ret);

bool hidden_or_backup_file(ಧ್ರುವ ವರ್ಣ *filename) _pure_;

bool is_device_path(ಧ್ರುವ ವರ್ಣ *path) _pure_;

bool valid_device_ಗಂಟು_path(ಧ್ರುವ ವರ್ಣ *path) _pure_;
bool valid_device_allow_pattern(ಧ್ರುವ ವರ್ಣ *path) _pure_;

bool dot_or_dot_dot(ಧ್ರುವ ವರ್ಣ *path) _pure_;

bool path_implies_directory(ಧ್ರುವ ವರ್ಣ *path) _pure_;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* skip_dev_prefix(ಧ್ರುವ ವರ್ಣ *p) {
        ಧ್ರುವ ವರ್ಣ *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        ಪ್ರತ್ಯಾಗಮ e ?: p;
}

bool empty_or_ಬೇರು(ಧ್ರುವ ವರ್ಣ *path) _pure_;
ಧ್ರುವ ವರ್ಣ* empty_to_ಬೇರು(ಧ್ರುವ ವರ್ಣ *path) _pure_;
ಪೂರ್ಣಾಙ್ಕ empty_or_ಬೇರು_harder_to_null(ಧ್ರುವ ವರ್ಣ **path);

bool path_strv_contains(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *path);
bool prefixed_path_strv_contains(ವರ್ಣ * ಧ್ರುವ *l, ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ path_glob_can_match(ಧ್ರುವ ವರ್ಣ *pattern, ಧ್ರುವ ವರ್ಣ *prefix, ವರ್ಣ **ret);
