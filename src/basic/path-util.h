/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_SEARCH_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_SEARCH_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_SEARCH_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_SEARCH_PATH_SUFFIX

const var_na* default_PATH(void);

static anta_hstha const var_na* default_user_PATH(void) {
#ifdef DEFAULT_USER_PATH
        ཕྱིར DEFAULT_USER_PATH EXTRA_EXEC_SEARCH_PATH_SUFFIX;
#དེ་མིན
        ཕྱིར default_PATH();
#endif
}

bool is_path(const var_na *p) _pure_;

static anta_hstha bool path_is_absolute(const var_na *p) {
        གལ་ཏེ (!p) /* A NULL pointer is definitely not an absolute path */
                ཕྱིར false;

        ཕྱིར p[0] == '/';
}

ཧྲིལ path_split_and_make_absolute(const var_na *p, var_na ***ret);
var_na* path_make_absolute(const var_na *p, const var_na *prefix);
ཧྲིལ safe_getcwd(var_na **ret);
ཧྲིལ path_make_absolute_cwd(const var_na *p, var_na **ret);
ཧྲིལ path_make_relative(const var_na *from, const var_na *to, var_na **ret);
ཧྲིལ path_make_relative_parent(const var_na *from_child, const var_na *to, var_na **ret);

typedef ga_nanaa PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

var_na* path_startswith_full(const var_na *path, const var_na *prefix, PathStartWithFlags flags) _pure_;
static anta_hstha var_na* path_startswith(const var_na *path, const var_na *prefix) {
        ཕྱིར path_startswith_full(path, prefix, 0);
}

ཧྲིལ path_compare(const var_na *a, const var_na *b) _pure_;
static anta_hstha bool path_equal(const var_na *a, const var_na *b) {
        ཕྱིར path_compare(a, b) == 0;
}

ཧྲིལ path_compare_filename(const var_na *a, const var_na *b);
static anta_hstha bool path_equal_filename(const var_na *a, const var_na *b) {
        ཕྱིར path_compare_filename(a, b) == 0;
}

ཧྲིལ path_equal_or_inode_same_full(const var_na *a, const var_na *b, ཧྲིལ flags);
static anta_hstha bool path_equal_or_inode_same(const var_na *a, const var_na *b, ཧྲིལ flags) {
        ཕྱིར path_equal_or_inode_same_full(a, b, flags) > 0;
}

var_na* path_extend_internal(var_na **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(NULL, __VA_ARGS__, POINTER_MAX)

var_na* skip_leading_slash(const var_na *p) _pure_;

typedef ga_nanaa PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

var_na* path_simplify_full(var_na *path, PathSimplifyFlags flags);
static anta_hstha var_na* path_simplify(var_na *path) {
        ཕྱིར path_simplify_full(path, 0);
}

ཧྲིལ path_simplify_alloc(const var_na *path, var_na **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

var_na* path_startswith_strv(const var_na *p, var_na * const *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ཧྲིལ path_strv_make_absolute_cwd(var_na **l);
var_na** path_strv_resolve(var_na **l, const var_na *root);
var_na** path_strv_resolve_uniq(var_na **l, const var_na *root);

ཧྲིལ open_and_check_executable(const var_na *name, const var_na *root, var_na **ret_path, ཧྲིལ *ret_fd);
ཧྲིལ find_executable_full(
                const var_na *name,
                const var_na *root,
                var_na * const *exec_search_path,
                bool use_path_envvar,
                var_na **ret_filename,
                ཧྲིལ *ret_fd);
static anta_hstha ཧྲིལ find_executable(const var_na *name, var_na **ret_filename) {
        ཕྱིར find_executable_full(name, /* root= */ NULL, NULL, true, ret_filename, NULL);
}

ཧྲིལ fsck_exists(void);
ཧྲིལ fsck_exists_for_fstype(const var_na *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        ཆེད་དུ (var_na *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? NULL : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        ཆེད་དུ (var_na *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                གལ་ཏེ (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

ཧྲིལ path_find_first_component(const var_na **p, bool accept_dot_dot, const var_na **ret);
ཧྲིལ path_find_last_component(const var_na *path, bool accept_dot_dot, const var_na **next, const var_na **ret);
const var_na* last_path_component(const var_na *path);

ཧྲིལ path_split_prefix_filename(const var_na *path, var_na **ret_dir, var_na **ret_filename);
static anta_hstha ཧྲིལ path_extract_filename(const var_na *path, var_na **ret) {
        ཕྱིར path_split_prefix_filename(path, NULL, ret);
}
static anta_hstha ཧྲིལ path_extract_directory(const var_na *path, var_na **ret) {
        ཧྲིལ r = path_split_prefix_filename(path, ret, NULL);
        ཕྱིར r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(const var_na *p) _pure_;
bool filename_is_valid(const var_na *p) _pure_;
bool path_is_valid_full(const var_na *p, bool accept_dot_dot) _pure_;
static anta_hstha bool path_is_valid(const var_na *p) {
        ཕྱིར path_is_valid_full(p, /* accept_dot_dot= */ true);
}
static anta_hstha bool path_is_safe(const var_na *p) {
        ཕྱིར path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(const var_na *p) _pure_;
static anta_hstha bool filename_or_absolute_path_is_valid(const var_na *p) {
        གལ་ཏེ (path_is_absolute(p))
                ཕྱིར path_is_valid(p);

        ཕྱིར filename_is_valid(p);
}

ཧྲིལ file_in_same_dir(const var_na *path, const var_na *filename, var_na **ret);

bool hidden_or_backup_file(const var_na *filename) _pure_;

bool is_device_path(const var_na *path) _pure_;

bool valid_device_node_path(const var_na *path) _pure_;
bool valid_device_allow_pattern(const var_na *path) _pure_;

bool dot_or_dot_dot(const var_na *path) _pure_;

bool path_implies_directory(const var_na *path) _pure_;

static anta_hstha const var_na* skip_dev_prefix(const var_na *p) {
        const var_na *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        ཕྱིར e ?: p;
}

bool empty_or_root(const var_na *path) _pure_;
const var_na* empty_to_root(const var_na *path) _pure_;
ཧྲིལ empty_or_root_harder_to_null(const var_na **path);

bool path_strv_contains(var_na * const *l, const var_na *path);
bool prefixed_path_strv_contains(var_na * const *l, const var_na *path);

ཧྲིལ path_glob_can_match(const var_na *pattern, const var_na *prefix, var_na **ret);
