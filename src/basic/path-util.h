/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_શોધ_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_શોધ_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_શોધ_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_શોધ_PATH_SUFFIX

અચલ અક્ષર* default_PATH(શૂન્ય);

સ્થિર અંતઃરેખ અચલ અક્ષર* default_user_PATH(શૂન્ય) {
#ifdef DEFAULT_USER_PATH
        પાછા DEFAULT_USER_PATH EXTRA_EXEC_શોધ_PATH_SUFFIX;
#નહીંતર
        પાછા default_PATH();
#endif
}

bool is_path(અચલ અક્ષર *p) _pure_;

સ્થિર અંતઃરેખ bool path_is_absolute(અચલ અક્ષર *p) {
        જો (!p) /* A NULL pointer is definitely not an absolute path */
                પાછા false;

        પાછા p[0] == '/';
}

પૂર્ણાંક path_split_and_make_absolute(અચલ અક્ષર *p, અક્ષર ***ret);
અક્ષર* path_make_absolute(અચલ અક્ષર *p, અચલ અક્ષર *prefix);
પૂર્ણાંક safe_getcwd(અક્ષર **ret);
પૂર્ણાંક path_make_absolute_cwd(અચલ અક્ષર *p, અક્ષર **ret);
પૂર્ણાંક path_make_relative(અચલ અક્ષર *from, અચલ અક્ષર *to, અક્ષર **ret);
પૂર્ણાંક path_make_relative_parent(અચલ અક્ષર *from_child, અચલ અક્ષર *to, અક્ષર **ret);

પ્રકારવ્યાખ્યા ગણના PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

અક્ષર* path_startswith_full(અચલ અક્ષર *path, અચલ અક્ષર *prefix, PathStartWithFlags flags) _pure_;
સ્થિર અંતઃરેખ અક્ષર* path_startswith(અચલ અક્ષર *path, અચલ અક્ષર *prefix) {
        પાછા path_startswith_full(path, prefix, 0);
}

પૂર્ણાંક path_compare(અચલ અક્ષર *a, અચલ અક્ષર *b) _pure_;
સ્થિર અંતઃરેખ bool path_equal(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        પાછા path_compare(a, b) == 0;
}

પૂર્ણાંક path_compare_filename(અચલ અક્ષર *a, અચલ અક્ષર *b);
સ્થિર અંતઃરેખ bool path_equal_filename(અચલ અક્ષર *a, અચલ અક્ષર *b) {
        પાછા path_compare_filename(a, b) == 0;
}

પૂર્ણાંક path_equal_or_inode_same_full(અચલ અક્ષર *a, અચલ અક્ષર *b, પૂર્ણાંક flags);
સ્થિર અંતઃરેખ bool path_equal_or_inode_same(અચલ અક્ષર *a, અચલ અક્ષર *b, પૂર્ણાંક flags) {
        પાછા path_equal_or_inode_same_full(a, b, flags) > 0;
}

અક્ષર* path_extend_internal(અક્ષર **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(ખાલી, __VA_ARGS__, POINTER_MAX)

અક્ષર* skip_leading_slash(અચલ અક્ષર *p) _pure_;

પ્રકારવ્યાખ્યા ગણના PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

અક્ષર* path_simplify_full(અક્ષર *path, PathSimplifyFlags flags);
સ્થિર અંતઃરેખ અક્ષર* path_simplify(અક્ષર *path) {
        પાછા path_simplify_full(path, 0);
}

પૂર્ણાંક path_simplify_alloc(અચલ અક્ષર *path, અક્ષર **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

અક્ષર* path_startswith_strv(અચલ અક્ષર *p, અક્ષર * અચલ *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

પૂર્ણાંક path_strv_make_absolute_cwd(અક્ષર **l);
અક્ષર** path_strv_resolve(અક્ષર **l, અચલ અક્ષર *મૂળ);
અક્ષર** path_strv_resolve_uniq(અક્ષર **l, અચલ અક્ષર *મૂળ);

પૂર્ણાંક open_and_check_executable(અચલ અક્ષર *name, અચલ અક્ષર *મૂળ, અક્ષર **ret_path, પૂર્ણાંક *ret_fd);
પૂર્ણાંક find_executable_full(
                અચલ અક્ષર *name,
                અચલ અક્ષર *મૂળ,
                અક્ષર * અચલ *exec_શોધ_path,
                bool use_path_envvar,
                અક્ષર **ret_filename,
                પૂર્ણાંક *ret_fd);
સ્થિર અંતઃરેખ પૂર્ણાંક find_executable(અચલ અક્ષર *name, અક્ષર **ret_filename) {
        પાછા find_executable_full(name, /* root= */ ખાલી, ખાલી, true, ret_filename, ખાલી);
}

પૂર્ણાંક fsck_exists(શૂન્ય);
પૂર્ણાંક fsck_exists_for_fstype(અચલ અક્ષર *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        માટે (અક્ષર *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? ખાલી : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        માટે (અક્ષર *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                જો (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

પૂર્ણાંક path_find_first_component(અચલ અક્ષર **p, bool accept_dot_dot, અચલ અક્ષર **ret);
પૂર્ણાંક path_find_last_component(અચલ અક્ષર *path, bool accept_dot_dot, અચલ અક્ષર **next, અચલ અક્ષર **ret);
અચલ અક્ષર* last_path_component(અચલ અક્ષર *path);

પૂર્ણાંક path_split_prefix_filename(અચલ અક્ષર *path, અક્ષર **ret_dir, અક્ષર **ret_filename);
સ્થિર અંતઃરેખ પૂર્ણાંક path_extract_filename(અચલ અક્ષર *path, અક્ષર **ret) {
        પાછા path_split_prefix_filename(path, ખાલી, ret);
}
સ્થિર અંતઃરેખ પૂર્ણાંક path_extract_directory(અચલ અક્ષર *path, અક્ષર **ret) {
        પૂર્ણાંક r = path_split_prefix_filename(path, ret, ખાલી);
        પાછા r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(અચલ અક્ષર *p) _pure_;
bool filename_is_valid(અચલ અક્ષર *p) _pure_;
bool path_is_valid_full(અચલ અક્ષર *p, bool accept_dot_dot) _pure_;
સ્થિર અંતઃરેખ bool path_is_valid(અચલ અક્ષર *p) {
        પાછા path_is_valid_full(p, /* accept_dot_dot= */ true);
}
સ્થિર અંતઃરેખ bool path_is_safe(અચલ અક્ષર *p) {
        પાછા path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(અચલ અક્ષર *p) _pure_;
સ્થિર અંતઃરેખ bool filename_or_absolute_path_is_valid(અચલ અક્ષર *p) {
        જો (path_is_absolute(p))
                પાછા path_is_valid(p);

        પાછા filename_is_valid(p);
}

પૂર્ણાંક file_in_same_dir(અચલ અક્ષર *path, અચલ અક્ષર *filename, અક્ષર **ret);

bool hidden_or_backup_file(અચલ અક્ષર *filename) _pure_;

bool is_device_path(અચલ અક્ષર *path) _pure_;

bool valid_device_ગાંઠ_path(અચલ અક્ષર *path) _pure_;
bool valid_device_allow_pattern(અચલ અક્ષર *path) _pure_;

bool dot_or_dot_dot(અચલ અક્ષર *path) _pure_;

bool path_implies_directory(અચલ અક્ષર *path) _pure_;

સ્થિર અંતઃરેખ અચલ અક્ષર* skip_dev_prefix(અચલ અક્ષર *p) {
        અચલ અક્ષર *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        પાછા e ?: p;
}

bool empty_or_મૂળ(અચલ અક્ષર *path) _pure_;
અચલ અક્ષર* empty_to_મૂળ(અચલ અક્ષર *path) _pure_;
પૂર્ણાંક empty_or_મૂળ_harder_to_null(અચલ અક્ષર **path);

bool path_strv_contains(અક્ષર * અચલ *l, અચલ અક્ષર *path);
bool prefixed_path_strv_contains(અક્ષર * અચલ *l, અચલ અક્ષર *path);

પૂર્ણાંક path_glob_can_match(અચલ અક્ષર *pattern, અચલ અક્ષર *prefix, અક્ષર **ret);
