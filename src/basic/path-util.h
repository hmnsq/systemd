/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_தேடல்_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_தேடல்_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_தேடல்_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_தேடல்_PATH_SUFFIX

மாறிலி எழுத்து* default_PATH(வெறுமை);

நிலையான உள்வரி மாறிலி எழுத்து* default_user_PATH(வெறுமை) {
#ifdef DEFAULT_USER_PATH
        திரும்பு DEFAULT_USER_PATH EXTRA_EXEC_தேடல்_PATH_SUFFIX;
#இல்லையெனில்
        திரும்பு default_PATH();
#endif
}

bool is_path(மாறிலி எழுத்து *p) _pure_;

நிலையான உள்வரி bool path_is_absolute(மாறிலி எழுத்து *p) {
        எனில் (!p) /* A NULL pointer is definitely not an absolute path */
                திரும்பு false;

        திரும்பு p[0] == '/';
}

முழு path_split_and_make_absolute(மாறிலி எழுத்து *p, எழுத்து ***ret);
எழுத்து* path_make_absolute(மாறிலி எழுத்து *p, மாறிலி எழுத்து *prefix);
முழு safe_getcwd(எழுத்து **ret);
முழு path_make_absolute_cwd(மாறிலி எழுத்து *p, எழுத்து **ret);
முழு path_make_relative(மாறிலி எழுத்து *from, மாறிலி எழுத்து *to, எழுத்து **ret);
முழு path_make_relative_parent(மாறிலி எழுத்து *from_child, மாறிலி எழுத்து *to, எழுத்து **ret);

வகைவரை எண்ணிக்கை PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

எழுத்து* path_startswith_full(மாறிலி எழுத்து *path, மாறிலி எழுத்து *prefix, PathStartWithFlags flags) _pure_;
நிலையான உள்வரி எழுத்து* path_startswith(மாறிலி எழுத்து *path, மாறிலி எழுத்து *prefix) {
        திரும்பு path_startswith_full(path, prefix, 0);
}

முழு path_compare(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) _pure_;
நிலையான உள்வரி bool path_equal(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        திரும்பு path_compare(a, b) == 0;
}

முழு path_compare_filename(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b);
நிலையான உள்வரி bool path_equal_filename(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        திரும்பு path_compare_filename(a, b) == 0;
}

முழு path_equal_or_inode_same_full(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b, முழு flags);
நிலையான உள்வரி bool path_equal_or_inode_same(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b, முழு flags) {
        திரும்பு path_equal_or_inode_same_full(a, b, flags) > 0;
}

எழுத்து* path_extend_internal(எழுத்து **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(வெற்று, __VA_ARGS__, POINTER_MAX)

எழுத்து* skip_leading_slash(மாறிலி எழுத்து *p) _pure_;

வகைவரை எண்ணிக்கை PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

எழுத்து* path_simplify_full(எழுத்து *path, PathSimplifyFlags flags);
நிலையான உள்வரி எழுத்து* path_simplify(எழுத்து *path) {
        திரும்பு path_simplify_full(path, 0);
}

முழு path_simplify_alloc(மாறிலி எழுத்து *path, எழுத்து **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

எழுத்து* path_startswith_strv(மாறிலி எழுத்து *p, எழுத்து * மாறிலி *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

முழு path_strv_make_absolute_cwd(எழுத்து **l);
எழுத்து** path_strv_resolve(எழுத்து **l, மாறிலி எழுத்து *வேர்);
எழுத்து** path_strv_resolve_uniq(எழுத்து **l, மாறிலி எழுத்து *வேர்);

முழு open_and_check_executable(மாறிலி எழுத்து *name, மாறிலி எழுத்து *வேர், எழுத்து **ret_path, முழு *ret_fd);
முழு find_executable_full(
                மாறிலி எழுத்து *name,
                மாறிலி எழுத்து *வேர்,
                எழுத்து * மாறிலி *exec_தேடல்_path,
                bool use_path_envvar,
                எழுத்து **ret_filename,
                முழு *ret_fd);
நிலையான உள்வரி முழு find_executable(மாறிலி எழுத்து *name, எழுத்து **ret_filename) {
        திரும்பு find_executable_full(name, /* root= */ வெற்று, வெற்று, true, ret_filename, வெற்று);
}

முழு fsck_exists(வெறுமை);
முழு fsck_exists_for_fstype(மாறிலி எழுத்து *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        க்கு (எழுத்து *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? வெற்று : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        க்கு (எழுத்து *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                எனில் (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

முழு path_find_first_component(மாறிலி எழுத்து **p, bool accept_dot_dot, மாறிலி எழுத்து **ret);
முழு path_find_last_component(மாறிலி எழுத்து *path, bool accept_dot_dot, மாறிலி எழுத்து **next, மாறிலி எழுத்து **ret);
மாறிலி எழுத்து* last_path_component(மாறிலி எழுத்து *path);

முழு path_split_prefix_filename(மாறிலி எழுத்து *path, எழுத்து **ret_dir, எழுத்து **ret_filename);
நிலையான உள்வரி முழு path_extract_filename(மாறிலி எழுத்து *path, எழுத்து **ret) {
        திரும்பு path_split_prefix_filename(path, வெற்று, ret);
}
நிலையான உள்வரி முழு path_extract_directory(மாறிலி எழுத்து *path, எழுத்து **ret) {
        முழு r = path_split_prefix_filename(path, ret, வெற்று);
        திரும்பு r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(மாறிலி எழுத்து *p) _pure_;
bool filename_is_valid(மாறிலி எழுத்து *p) _pure_;
bool path_is_valid_full(மாறிலி எழுத்து *p, bool accept_dot_dot) _pure_;
நிலையான உள்வரி bool path_is_valid(மாறிலி எழுத்து *p) {
        திரும்பு path_is_valid_full(p, /* accept_dot_dot= */ true);
}
நிலையான உள்வரி bool path_is_safe(மாறிலி எழுத்து *p) {
        திரும்பு path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(மாறிலி எழுத்து *p) _pure_;
நிலையான உள்வரி bool filename_or_absolute_path_is_valid(மாறிலி எழுத்து *p) {
        எனில் (path_is_absolute(p))
                திரும்பு path_is_valid(p);

        திரும்பு filename_is_valid(p);
}

முழு file_in_same_dir(மாறிலி எழுத்து *path, மாறிலி எழுத்து *filename, எழுத்து **ret);

bool hidden_or_backup_file(மாறிலி எழுத்து *filename) _pure_;

bool is_device_path(மாறிலி எழுத்து *path) _pure_;

bool valid_device_முனை_path(மாறிலி எழுத்து *path) _pure_;
bool valid_device_allow_pattern(மாறிலி எழுத்து *path) _pure_;

bool dot_or_dot_dot(மாறிலி எழுத்து *path) _pure_;

bool path_implies_directory(மாறிலி எழுத்து *path) _pure_;

நிலையான உள்வரி மாறிலி எழுத்து* skip_dev_prefix(மாறிலி எழுத்து *p) {
        மாறிலி எழுத்து *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        திரும்பு e ?: p;
}

bool empty_or_வேர்(மாறிலி எழுத்து *path) _pure_;
மாறிலி எழுத்து* empty_to_வேர்(மாறிலி எழுத்து *path) _pure_;
முழு empty_or_வேர்_harder_to_null(மாறிலி எழுத்து **path);

bool path_strv_contains(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *path);
bool prefixed_path_strv_contains(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *path);

முழு path_glob_can_match(மாறிலி எழுத்து *pattern, மாறிலி எழுத்து *prefix, எழுத்து **ret);
