/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX

ধ্রুবক অক্ষর* default_PATH(শূন্য);

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* default_user_PATH(শূন্য) {
#ifdef DEFAULT_USER_PATH
        ফেরত DEFAULT_USER_PATH EXTRA_EXEC_অনুসন্ধান_PATH_SUFFIX;
#অন্যথা
        ফেরত default_PATH();
#endif
}

bool is_path(ধ্রুবক অক্ষর *p) _pure_;

স্থির অন্তঃরেখ bool path_is_absolute(ধ্রুবক অক্ষর *p) {
        যদি (!p) /* A NULL pointer is definitely not an absolute path */
                ফেরত false;

        ফেরত p[0] == '/';
}

পূর্ণসংখ্যা path_split_and_make_absolute(ধ্রুবক অক্ষর *p, অক্ষর ***ret);
অক্ষর* path_make_absolute(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *prefix);
পূর্ণসংখ্যা safe_getcwd(অক্ষর **ret);
পূর্ণসংখ্যা path_make_absolute_cwd(ধ্রুবক অক্ষর *p, অক্ষর **ret);
পূর্ণসংখ্যা path_make_relative(ধ্রুবক অক্ষর *from, ধ্রুবক অক্ষর *to, অক্ষর **ret);
পূর্ণসংখ্যা path_make_relative_parent(ধ্রুবক অক্ষর *from_child, ধ্রুবক অক্ষর *to, অক্ষর **ret);

প্রকারলেখ গণনা PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

অক্ষর* path_startswith_full(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *prefix, PathStartWithFlags flags) _pure_;
স্থির অন্তঃরেখ অক্ষর* path_startswith(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *prefix) {
        ফেরত path_startswith_full(path, prefix, 0);
}

পূর্ণসংখ্যা path_compare(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) _pure_;
স্থির অন্তঃরেখ bool path_equal(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        ফেরত path_compare(a, b) == 0;
}

পূর্ণসংখ্যা path_compare_filename(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b);
স্থির অন্তঃরেখ bool path_equal_filename(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b) {
        ফেরত path_compare_filename(a, b) == 0;
}

পূর্ণসংখ্যা path_equal_or_inode_same_full(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b, পূর্ণসংখ্যা flags);
স্থির অন্তঃরেখ bool path_equal_or_inode_same(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b, পূর্ণসংখ্যা flags) {
        ফেরত path_equal_or_inode_same_full(a, b, flags) > 0;
}

অক্ষর* path_extend_internal(অক্ষর **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(ফাঁকা, __VA_ARGS__, POINTER_MAX)

অক্ষর* skip_leading_slash(ধ্রুবক অক্ষর *p) _pure_;

প্রকারলেখ গণনা PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

অক্ষর* path_simplify_full(অক্ষর *path, PathSimplifyFlags flags);
স্থির অন্তঃরেখ অক্ষর* path_simplify(অক্ষর *path) {
        ফেরত path_simplify_full(path, 0);
}

পূর্ণসংখ্যা path_simplify_alloc(ধ্রুবক অক্ষর *path, অক্ষর **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

অক্ষর* path_startswith_strv(ধ্রুবক অক্ষর *p, অক্ষর * ধ্রুবক *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

পূর্ণসংখ্যা path_strv_make_absolute_cwd(অক্ষর **l);
অক্ষর** path_strv_resolve(অক্ষর **l, ধ্রুবক অক্ষর *মূল);
অক্ষর** path_strv_resolve_uniq(অক্ষর **l, ধ্রুবক অক্ষর *মূল);

পূর্ণসংখ্যা open_and_check_executable(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *মূল, অক্ষর **ret_path, পূর্ণসংখ্যা *ret_fd);
পূর্ণসংখ্যা find_executable_full(
                ধ্রুবক অক্ষর *name,
                ধ্রুবক অক্ষর *মূল,
                অক্ষর * ধ্রুবক *exec_অনুসন্ধান_path,
                bool use_path_envvar,
                অক্ষর **ret_filename,
                পূর্ণসংখ্যা *ret_fd);
স্থির অন্তঃরেখ পূর্ণসংখ্যা find_executable(ধ্রুবক অক্ষর *name, অক্ষর **ret_filename) {
        ফেরত find_executable_full(name, /* root= */ ফাঁকা, ফাঁকা, true, ret_filename, ফাঁকা);
}

পূর্ণসংখ্যা fsck_exists(শূন্য);
পূর্ণসংখ্যা fsck_exists_for_fstype(ধ্রুবক অক্ষর *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        জন্য (অক্ষর *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? ফাঁকা : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        জন্য (অক্ষর *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                যদি (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

পূর্ণসংখ্যা path_find_first_component(ধ্রুবক অক্ষর **p, bool accept_dot_dot, ধ্রুবক অক্ষর **ret);
পূর্ণসংখ্যা path_find_last_component(ধ্রুবক অক্ষর *path, bool accept_dot_dot, ধ্রুবক অক্ষর **পরবর্তী_2, ধ্রুবক অক্ষর **ret);
ধ্রুবক অক্ষর* last_path_component(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা path_split_prefix_filename(ধ্রুবক অক্ষর *path, অক্ষর **ret_dir, অক্ষর **ret_filename);
স্থির অন্তঃরেখ পূর্ণসংখ্যা path_extract_filename(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        ফেরত path_split_prefix_filename(path, ফাঁকা, ret);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা path_extract_directory(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        পূর্ণসংখ্যা r = path_split_prefix_filename(path, ret, ফাঁকা);
        ফেরত r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ধ্রুবক অক্ষর *p) _pure_;
bool filename_is_valid(ধ্রুবক অক্ষর *p) _pure_;
bool path_is_valid_full(ধ্রুবক অক্ষর *p, bool accept_dot_dot) _pure_;
স্থির অন্তঃরেখ bool path_is_valid(ধ্রুবক অক্ষর *p) {
        ফেরত path_is_valid_full(p, /* accept_dot_dot= */ true);
}
স্থির অন্তঃরেখ bool path_is_safe(ধ্রুবক অক্ষর *p) {
        ফেরত path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(ধ্রুবক অক্ষর *p) _pure_;
স্থির অন্তঃরেখ bool filename_or_absolute_path_is_valid(ধ্রুবক অক্ষর *p) {
        যদি (path_is_absolute(p))
                ফেরত path_is_valid(p);

        ফেরত filename_is_valid(p);
}

পূর্ণসংখ্যা file_in_same_dir(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *filename, অক্ষর **ret);

bool hidden_or_backup_file(ধ্রুবক অক্ষর *filename) _pure_;

bool is_device_path(ধ্রুবক অক্ষর *path) _pure_;

bool valid_device_গ্রন্থি_path(ধ্রুবক অক্ষর *path) _pure_;
bool valid_device_allow_pattern(ধ্রুবক অক্ষর *path) _pure_;

bool dot_or_dot_dot(ধ্রুবক অক্ষর *path) _pure_;

bool path_implies_directory(ধ্রুবক অক্ষর *path) _pure_;

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* skip_dev_prefix(ধ্রুবক অক্ষর *p) {
        ধ্রুবক অক্ষর *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        ফেরত e ?: p;
}

bool empty_or_মূল(ধ্রুবক অক্ষর *path) _pure_;
ধ্রুবক অক্ষর* empty_to_মূল(ধ্রুবক অক্ষর *path) _pure_;
পূর্ণসংখ্যা empty_or_মূল_harder_to_null(ধ্রুবক অক্ষর **path);

bool path_strv_contains(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *path);
bool prefixed_path_strv_contains(অক্ষর * ধ্রুবক *l, ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা path_glob_can_match(ধ্রুবক অক্ষর *pattern, ধ্রুবক অক্ষর *prefix, অক্ষর **ret);
