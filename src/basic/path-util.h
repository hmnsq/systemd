/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX

ধ্রুৱ ৱর্ণ* default_PATH(শূন্য);

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* default_user_PATH(শূন্য) {
#ifdef DEFAULT_USER_PATH
        ঘূৰাই DEFAULT_USER_PATH EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX;
#অন্যথা
        ঘূৰাই default_PATH();
#endif
}

bool is_path(ধ্রুৱ ৱর্ণ *p) _pure_;

স্থির অন্তঃস্থ bool path_is_absolute(ধ্রুৱ ৱর্ণ *p) {
        যদি (!p) /* A NULL pointer is definitely not an absolute path */
                ঘূৰাই false;

        ঘূৰাই p[0] == '/';
}

পূর্ণাঙ্ক path_split_and_make_absolute(ধ্রুৱ ৱর্ণ *p, ৱর্ণ ***ret);
ৱর্ণ* path_make_absolute(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *prefix);
পূর্ণাঙ্ক safe_getcwd(ৱর্ণ **ret);
পূর্ণাঙ্ক path_make_absolute_cwd(ধ্রুৱ ৱর্ণ *p, ৱর্ণ **ret);
পূর্ণাঙ্ক path_make_relative(ধ্রুৱ ৱর্ণ *from, ধ্রুৱ ৱর্ণ *to, ৱর্ণ **ret);
পূর্ণাঙ্ক path_make_relative_parent(ধ্রুৱ ৱর্ণ *from_child, ধ্রুৱ ৱর্ণ *to, ৱর্ণ **ret);

প্রকারনির্দেশ গণনা PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

ৱর্ণ* path_startswith_full(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *prefix, PathStartWithFlags flags) _pure_;
স্থির অন্তঃস্থ ৱর্ণ* path_startswith(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *prefix) {
        ঘূৰাই path_startswith_full(path, prefix, 0);
}

পূর্ণাঙ্ক path_compare(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b) _pure_;
স্থির অন্তঃস্থ bool path_equal(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b) {
        ঘূৰাই path_compare(a, b) == 0;
}

পূর্ণাঙ্ক path_compare_filename(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b);
স্থির অন্তঃস্থ bool path_equal_filename(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b) {
        ঘূৰাই path_compare_filename(a, b) == 0;
}

পূর্ণাঙ্ক path_equal_or_inode_same_full(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b, পূর্ণাঙ্ক flags);
স্থির অন্তঃস্থ bool path_equal_or_inode_same(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b, পূর্ণাঙ্ক flags) {
        ঘূৰাই path_equal_or_inode_same_full(a, b, flags) > 0;
}

ৱর্ণ* path_extend_internal(ৱর্ণ **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(রিক্ত, __VA_ARGS__, POINTER_MAX)

ৱর্ণ* skip_leading_slash(ধ্রুৱ ৱর্ণ *p) _pure_;

প্রকারনির্দেশ গণনা PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

ৱর্ণ* path_simplify_full(ৱর্ণ *path, PathSimplifyFlags flags);
স্থির অন্তঃস্থ ৱর্ণ* path_simplify(ৱর্ণ *path) {
        ঘূৰাই path_simplify_full(path, 0);
}

পূর্ণাঙ্ক path_simplify_alloc(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

ৱর্ণ* path_startswith_strv(ধ্রুৱ ৱর্ণ *p, ৱর্ণ * ধ্রুৱ *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

পূর্ণাঙ্ক path_strv_make_absolute_cwd(ৱর্ণ **l);
ৱর্ণ** path_strv_resolve(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *মূল);
ৱর্ণ** path_strv_resolve_uniq(ৱর্ণ **l, ধ্রুৱ ৱর্ণ *মূল);

পূর্ণাঙ্ক open_and_check_executable(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ ৱর্ণ *মূল, ৱর্ণ **ret_path, পূর্ণাঙ্ক *ret_fd);
পূর্ণাঙ্ক find_executable_full(
                ধ্রুৱ ৱর্ণ *name,
                ধ্রুৱ ৱর্ণ *মূল,
                ৱর্ণ * ধ্রুৱ *exec_অনুসন্ধান_path,
                bool use_path_envvar,
                ৱর্ণ **ret_filename,
                পূর্ণাঙ্ক *ret_fd);
স্থির অন্তঃস্থ পূর্ণাঙ্ক find_executable(ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret_filename) {
        ঘূৰাই find_executable_full(name, /* root= */ রিক্ত, রিক্ত, true, ret_filename, রিক্ত);
}

পূর্ণাঙ্ক fsck_exists(শূন্য);
পূর্ণাঙ্ক fsck_exists_for_fstype(ধ্রুৱ ৱর্ণ *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        বাবে (ৱর্ণ *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? রিক্ত : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        বাবে (ৱর্ণ *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                যদি (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

পূর্ণাঙ্ক path_find_first_component(ধ্রুৱ ৱর্ণ **p, bool accept_dot_dot, ধ্রুৱ ৱর্ণ **ret);
পূর্ণাঙ্ক path_find_last_component(ধ্রুৱ ৱর্ণ *path, bool accept_dot_dot, ধ্রুৱ ৱর্ণ **next, ধ্রুৱ ৱর্ণ **ret);
ধ্রুৱ ৱর্ণ* last_path_component(ধ্রুৱ ৱর্ণ *path);

পূর্ণাঙ্ক path_split_prefix_filename(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret_dir, ৱর্ণ **ret_filename);
স্থির অন্তঃস্থ পূর্ণাঙ্ক path_extract_filename(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret) {
        ঘূৰাই path_split_prefix_filename(path, রিক্ত, ret);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক path_extract_directory(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret) {
        পূর্ণাঙ্ক r = path_split_prefix_filename(path, ret, রিক্ত);
        ঘূৰাই r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ধ্রুৱ ৱর্ণ *p) _pure_;
bool filename_is_valid(ধ্রুৱ ৱর্ণ *p) _pure_;
bool path_is_valid_full(ধ্রুৱ ৱর্ণ *p, bool accept_dot_dot) _pure_;
স্থির অন্তঃস্থ bool path_is_valid(ধ্রুৱ ৱর্ণ *p) {
        ঘূৰাই path_is_valid_full(p, /* accept_dot_dot= */ true);
}
স্থির অন্তঃস্থ bool path_is_safe(ধ্রুৱ ৱর্ণ *p) {
        ঘূৰাই path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(ধ্রুৱ ৱর্ণ *p) _pure_;
স্থির অন্তঃস্থ bool filename_or_absolute_path_is_valid(ধ্রুৱ ৱর্ণ *p) {
        যদি (path_is_absolute(p))
                ঘূৰাই path_is_valid(p);

        ঘূৰাই filename_is_valid(p);
}

পূর্ণাঙ্ক file_in_same_dir(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *filename, ৱর্ণ **ret);

bool hidden_or_backup_file(ধ্রুৱ ৱর্ণ *filename) _pure_;

bool is_device_path(ধ্রুৱ ৱর্ণ *path) _pure_;

bool valid_device_গ্ৰন্থি_path(ধ্রুৱ ৱর্ণ *path) _pure_;
bool valid_device_allow_pattern(ধ্রুৱ ৱর্ণ *path) _pure_;

bool dot_or_dot_dot(ধ্রুৱ ৱর্ণ *path) _pure_;

bool path_implies_directory(ধ্রুৱ ৱর্ণ *path) _pure_;

স্থির অন্তঃস্থ ধ্রুৱ ৱর্ণ* skip_dev_prefix(ধ্রুৱ ৱর্ণ *p) {
        ধ্রুৱ ৱর্ণ *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        ঘূৰাই e ?: p;
}

bool empty_or_মূল(ধ্রুৱ ৱর্ণ *path) _pure_;
ধ্রুৱ ৱর্ণ* empty_to_মূল(ধ্রুৱ ৱর্ণ *path) _pure_;
পূর্ণাঙ্ক empty_or_মূল_harder_to_null(ধ্রুৱ ৱর্ণ **path);

bool path_strv_contains(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *path);
bool prefixed_path_strv_contains(ৱর্ণ * ধ্রুৱ *l, ধ্রুৱ ৱর্ণ *path);

পূর্ণাঙ্ক path_glob_can_match(ধ্রুৱ ৱর্ণ *pattern, ধ্রুৱ ৱর্ণ *prefix, ৱর্ণ **ret);
