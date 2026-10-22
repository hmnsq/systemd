/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_جستجو_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_جستجو_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_جستجو_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_جستجو_PATH_SUFFIX

ثابت نویسه* default_PATH(تهی);

ایستا درونخطی ثابت نویسه* default_user_PATH(تهی) {
#ifdef DEFAULT_USER_PATH
        بازگشت DEFAULT_USER_PATH EXTRA_EXEC_جستجو_PATH_SUFFIX;
#وگرنه
        بازگشت default_PATH();
#endif
}

bool is_path(ثابت نویسه *p) _pure_;

ایستا درونخطی bool path_is_absolute(ثابت نویسه *p) {
        اگر (!p) /* A NULL pointer is definitely not an absolute path */
                بازگشت false;

        بازگشت p[0] == '/';
}

صحیح path_split_and_make_absolute(ثابت نویسه *p, نویسه ***ret);
نویسه* path_make_absolute(ثابت نویسه *p, ثابت نویسه *prefix);
صحیح safe_getcwd(نویسه **ret);
صحیح path_make_absolute_cwd(ثابت نویسه *p, نویسه **ret);
صحیح path_make_relative(ثابت نویسه *from, ثابت نویسه *to, نویسه **ret);
صحیح path_make_relative_parent(ثابت نویسه *from_child, ثابت نویسه *to, نویسه **ret);

تعریفنوع شمارش PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

نویسه* path_startswith_full(ثابت نویسه *path, ثابت نویسه *prefix, PathStartWithFlags flags) _pure_;
ایستا درونخطی نویسه* path_startswith(ثابت نویسه *path, ثابت نویسه *prefix) {
        بازگشت path_startswith_full(path, prefix, 0);
}

صحیح path_compare(ثابت نویسه *a, ثابت نویسه *b) _pure_;
ایستا درونخطی bool path_equal(ثابت نویسه *a, ثابت نویسه *b) {
        بازگشت path_compare(a, b) == 0;
}

صحیح path_compare_filename(ثابت نویسه *a, ثابت نویسه *b);
ایستا درونخطی bool path_equal_filename(ثابت نویسه *a, ثابت نویسه *b) {
        بازگشت path_compare_filename(a, b) == 0;
}

صحیح path_equal_or_inode_same_full(ثابت نویسه *a, ثابت نویسه *b, صحیح flags);
ایستا درونخطی bool path_equal_or_inode_same(ثابت نویسه *a, ثابت نویسه *b, صحیح flags) {
        بازگشت path_equal_or_inode_same_full(a, b, flags) > 0;
}

نویسه* path_extend_internal(نویسه **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(پوچ, __VA_ARGS__, POINTER_MAX)

نویسه* skip_leading_slash(ثابت نویسه *p) _pure_;

تعریفنوع شمارش PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

نویسه* path_simplify_full(نویسه *path, PathSimplifyFlags flags);
ایستا درونخطی نویسه* path_simplify(نویسه *path) {
        بازگشت path_simplify_full(path, 0);
}

صحیح path_simplify_alloc(ثابت نویسه *path, نویسه **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

نویسه* path_startswith_strv(ثابت نویسه *p, نویسه * ثابت *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

صحیح path_strv_make_absolute_cwd(نویسه **l);
نویسه** path_strv_resolve(نویسه **l, ثابت نویسه *ریشه);
نویسه** path_strv_resolve_uniq(نویسه **l, ثابت نویسه *ریشه);

صحیح open_and_check_executable(ثابت نویسه *name, ثابت نویسه *ریشه, نویسه **ret_path, صحیح *ret_fd);
صحیح find_executable_full(
                ثابت نویسه *name,
                ثابت نویسه *ریشه,
                نویسه * ثابت *exec_جستجو_path,
                bool use_path_envvar,
                نویسه **ret_filename,
                صحیح *ret_fd);
ایستا درونخطی صحیح find_executable(ثابت نویسه *name, نویسه **ret_filename) {
        بازگشت find_executable_full(name, /* root= */ پوچ, پوچ, true, ret_filename, پوچ);
}

صحیح fsck_exists(تهی);
صحیح fsck_exists_for_fstype(ثابت نویسه *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        برای (نویسه *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? پوچ : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        برای (نویسه *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                اگر (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

صحیح path_find_first_component(ثابت نویسه **p, bool accept_dot_dot, ثابت نویسه **ret);
صحیح path_find_last_component(ثابت نویسه *path, bool accept_dot_dot, ثابت نویسه **next, ثابت نویسه **ret);
ثابت نویسه* last_path_component(ثابت نویسه *path);

صحیح path_split_prefix_filename(ثابت نویسه *path, نویسه **ret_dir, نویسه **ret_filename);
ایستا درونخطی صحیح path_extract_filename(ثابت نویسه *path, نویسه **ret) {
        بازگشت path_split_prefix_filename(path, پوچ, ret);
}
ایستا درونخطی صحیح path_extract_directory(ثابت نویسه *path, نویسه **ret) {
        صحیح r = path_split_prefix_filename(path, ret, پوچ);
        بازگشت r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ثابت نویسه *p) _pure_;
bool filename_is_valid(ثابت نویسه *p) _pure_;
bool path_is_valid_full(ثابت نویسه *p, bool accept_dot_dot) _pure_;
ایستا درونخطی bool path_is_valid(ثابت نویسه *p) {
        بازگشت path_is_valid_full(p, /* accept_dot_dot= */ true);
}
ایستا درونخطی bool path_is_safe(ثابت نویسه *p) {
        بازگشت path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(ثابت نویسه *p) _pure_;
ایستا درونخطی bool filename_or_absolute_path_is_valid(ثابت نویسه *p) {
        اگر (path_is_absolute(p))
                بازگشت path_is_valid(p);

        بازگشت filename_is_valid(p);
}

صحیح file_in_same_dir(ثابت نویسه *path, ثابت نویسه *filename, نویسه **ret);

bool hidden_or_backup_file(ثابت نویسه *filename) _pure_;

bool is_device_path(ثابت نویسه *path) _pure_;

bool valid_device_گره_path(ثابت نویسه *path) _pure_;
bool valid_device_allow_pattern(ثابت نویسه *path) _pure_;

bool dot_or_dot_dot(ثابت نویسه *path) _pure_;

bool path_implies_directory(ثابت نویسه *path) _pure_;

ایستا درونخطی ثابت نویسه* skip_dev_prefix(ثابت نویسه *p) {
        ثابت نویسه *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        بازگشت e ?: p;
}

bool empty_or_ریشه(ثابت نویسه *path) _pure_;
ثابت نویسه* empty_to_ریشه(ثابت نویسه *path) _pure_;
صحیح empty_or_ریشه_harder_to_null(ثابت نویسه **path);

bool path_strv_contains(نویسه * ثابت *l, ثابت نویسه *path);
bool prefixed_path_strv_contains(نویسه * ثابت *l, ثابت نویسه *path);

صحیح path_glob_can_match(ثابت نویسه *pattern, ثابت نویسه *prefix, نویسه **ret);
