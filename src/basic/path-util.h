/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_ئىزدەش_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_ئىزدەش_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_ئىزدەش_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_ئىزدەش_PATH_SUFFIX

ثابت حرف* default_PATH(فراغ);

ساكن مضمن ثابت حرف* default_user_PATH(فراغ) {
#ifdef DEFAULT_USER_PATH
        أرجع DEFAULT_USER_PATH EXTRA_EXEC_ئىزدەش_PATH_SUFFIX;
#وإلا
        أرجع default_PATH();
#endif
}

bool is_path(ثابت حرف *p) _pure_;

ساكن مضمن bool path_is_absolute(ثابت حرف *p) {
        إذا (!p) /* A NULL pointer is definitely not an absolute path */
                أرجع false;

        أرجع p[0] == '/';
}

صحيح path_split_and_make_absolute(ثابت حرف *p, حرف ***ret);
حرف* path_make_absolute(ثابت حرف *p, ثابت حرف *prefix);
صحيح safe_getcwd(حرف **ret);
صحيح path_make_absolute_cwd(ثابت حرف *p, حرف **ret);
صحيح path_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret);
صحيح path_make_relative_parent(ثابت حرف *from_child, ثابت حرف *to, حرف **ret);

تعريفنوع تعداد PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

حرف* path_startswith_full(ثابت حرف *path, ثابت حرف *prefix, PathStartWithFlags flags) _pure_;
ساكن مضمن حرف* path_startswith(ثابت حرف *path, ثابت حرف *prefix) {
        أرجع path_startswith_full(path, prefix, 0);
}

صحيح path_compare(ثابت حرف *a, ثابت حرف *b) _pure_;
ساكن مضمن bool path_equal(ثابت حرف *a, ثابت حرف *b) {
        أرجع path_compare(a, b) == 0;
}

صحيح path_compare_filename(ثابت حرف *a, ثابت حرف *b);
ساكن مضمن bool path_equal_filename(ثابت حرف *a, ثابت حرف *b) {
        أرجع path_compare_filename(a, b) == 0;
}

صحيح path_equal_or_inode_same_full(ثابت حرف *a, ثابت حرف *b, صحيح flags);
ساكن مضمن bool path_equal_or_inode_same(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        أرجع path_equal_or_inode_same_full(a, b, flags) > 0;
}

حرف* path_extend_internal(حرف **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(عدم, __VA_ARGS__, POINTER_MAX)

حرف* skip_leading_slash(ثابت حرف *p) _pure_;

تعريفنوع تعداد PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

حرف* path_simplify_full(حرف *path, PathSimplifyFlags flags);
ساكن مضمن حرف* path_simplify(حرف *path) {
        أرجع path_simplify_full(path, 0);
}

صحيح path_simplify_alloc(ثابت حرف *path, حرف **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

حرف* path_startswith_strv(ثابت حرف *p, حرف * ثابت *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

صحيح path_strv_make_absolute_cwd(حرف **l);
حرف** path_strv_resolve(حرف **l, ثابت حرف *يىلتىز);
حرف** path_strv_resolve_uniq(حرف **l, ثابت حرف *يىلتىز);

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *يىلتىز, حرف **ret_path, صحيح *ret_fd);
صحيح find_executable_full(
                ثابت حرف *name,
                ثابت حرف *يىلتىز,
                حرف * ثابت *exec_ئىزدەش_path,
                bool use_path_envvar,
                حرف **ret_filename,
                صحيح *ret_fd);
ساكن مضمن صحيح find_executable(ثابت حرف *name, حرف **ret_filename) {
        أرجع find_executable_full(name, /* root= */ عدم, عدم, true, ret_filename, عدم);
}

صحيح fsck_exists(فراغ);
صحيح fsck_exists_for_fstype(ثابت حرف *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        لكل (حرف *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? عدم : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        لكل (حرف *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                إذا (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

صحيح path_find_first_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret);
صحيح path_find_last_component(ثابت حرف *path, bool accept_dot_dot, ثابت حرف **next, ثابت حرف **ret);
ثابت حرف* last_path_component(ثابت حرف *path);

صحيح path_split_prefix_filename(ثابت حرف *path, حرف **ret_dir, حرف **ret_filename);
ساكن مضمن صحيح path_extract_filename(ثابت حرف *path, حرف **ret) {
        أرجع path_split_prefix_filename(path, عدم, ret);
}
ساكن مضمن صحيح path_extract_directory(ثابت حرف *path, حرف **ret) {
        صحيح r = path_split_prefix_filename(path, ret, عدم);
        أرجع r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ثابت حرف *p) _pure_;
bool filename_is_valid(ثابت حرف *p) _pure_;
bool path_is_valid_full(ثابت حرف *p, bool accept_dot_dot) _pure_;
ساكن مضمن bool path_is_valid(ثابت حرف *p) {
        أرجع path_is_valid_full(p, /* accept_dot_dot= */ true);
}
ساكن مضمن bool path_is_safe(ثابت حرف *p) {
        أرجع path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(ثابت حرف *p) _pure_;
ساكن مضمن bool filename_or_absolute_path_is_valid(ثابت حرف *p) {
        إذا (path_is_absolute(p))
                أرجع path_is_valid(p);

        أرجع filename_is_valid(p);
}

صحيح file_in_same_dir(ثابت حرف *path, ثابت حرف *filename, حرف **ret);

bool hidden_or_backup_file(ثابت حرف *filename) _pure_;

bool is_device_path(ثابت حرف *path) _pure_;

bool valid_device_تۈگۈن_path(ثابت حرف *path) _pure_;
bool valid_device_allow_pattern(ثابت حرف *path) _pure_;

bool dot_or_dot_dot(ثابت حرف *path) _pure_;

bool path_implies_directory(ثابت حرف *path) _pure_;

ساكن مضمن ثابت حرف* skip_dev_prefix(ثابت حرف *p) {
        ثابت حرف *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        أرجع e ?: p;
}

bool empty_or_يىلتىز(ثابت حرف *path) _pure_;
ثابت حرف* empty_to_يىلتىز(ثابت حرف *path) _pure_;
صحيح empty_or_يىلتىز_harder_to_null(ثابت حرف **path);

bool path_strv_contains(حرف * ثابت *l, ثابت حرف *path);
bool prefixed_path_strv_contains(حرف * ثابت *l, ثابت حرف *path);

صحيح path_glob_can_match(ثابت حرف *pattern, ثابت حرف *prefix, حرف **ret);
