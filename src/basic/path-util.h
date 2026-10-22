/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_തിരയൽ_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_തിരയൽ_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_തിരയൽ_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_തിരയൽ_PATH_SUFFIX

സ്ഥിരം അക്ഷരം* default_PATH(ശൂന്യം);

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* default_user_PATH(ശൂന്യം) {
#ifdef DEFAULT_USER_PATH
        തിരികെ DEFAULT_USER_PATH EXTRA_EXEC_തിരയൽ_PATH_SUFFIX;
#അല്ലെങ്കിൽ
        തിരികെ default_PATH();
#endif
}

bool is_path(സ്ഥിരം അക്ഷരം *p) _pure_;

സ്ഥിതം വരിയിൽ bool path_is_absolute(സ്ഥിരം അക്ഷരം *p) {
        എങ്കിൽ (!p) /* A NULL pointer is definitely not an absolute path */
                തിരികെ false;

        തിരികെ p[0] == '/';
}

പൂർണ്ണസംഖ്യ path_split_and_make_absolute(സ്ഥിരം അക്ഷരം *p, അക്ഷരം ***ret);
അക്ഷരം* path_make_absolute(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *prefix);
പൂർണ്ണസംഖ്യ safe_getcwd(അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ path_make_absolute_cwd(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ path_make_relative(സ്ഥിരം അക്ഷരം *from, സ്ഥിരം അക്ഷരം *to, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ path_make_relative_parent(സ്ഥിരം അക്ഷരം *from_child, സ്ഥിരം അക്ഷരം *to, അക്ഷരം **ret);

തരനിർവചനം എണ്ണൽ PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

അക്ഷരം* path_startswith_full(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *prefix, PathStartWithFlags flags) _pure_;
സ്ഥിതം വരിയിൽ അക്ഷരം* path_startswith(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *prefix) {
        തിരികെ path_startswith_full(path, prefix, 0);
}

പൂർണ്ണസംഖ്യ path_compare(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) _pure_;
സ്ഥിതം വരിയിൽ bool path_equal(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        തിരികെ path_compare(a, b) == 0;
}

പൂർണ്ണസംഖ്യ path_compare_filename(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b);
സ്ഥിതം വരിയിൽ bool path_equal_filename(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        തിരികെ path_compare_filename(a, b) == 0;
}

പൂർണ്ണസംഖ്യ path_equal_or_inode_same_full(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b, പൂർണ്ണസംഖ്യ flags);
സ്ഥിതം വരിയിൽ bool path_equal_or_inode_same(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b, പൂർണ്ണസംഖ്യ flags) {
        തിരികെ path_equal_or_inode_same_full(a, b, flags) > 0;
}

അക്ഷരം* path_extend_internal(അക്ഷരം **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(ശൂന്യ, __VA_ARGS__, POINTER_MAX)

അക്ഷരം* skip_leading_slash(സ്ഥിരം അക്ഷരം *p) _pure_;

തരനിർവചനം എണ്ണൽ PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

അക്ഷരം* path_simplify_full(അക്ഷരം *path, PathSimplifyFlags flags);
സ്ഥിതം വരിയിൽ അക്ഷരം* path_simplify(അക്ഷരം *path) {
        തിരികെ path_simplify_full(path, 0);
}

പൂർണ്ണസംഖ്യ path_simplify_alloc(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

അക്ഷരം* path_startswith_strv(സ്ഥിരം അക്ഷരം *p, അക്ഷരം * സ്ഥിരം *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

പൂർണ്ണസംഖ്യ path_strv_make_absolute_cwd(അക്ഷരം **l);
അക്ഷരം** path_strv_resolve(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *വേര്);
അക്ഷരം** path_strv_resolve_uniq(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *വേര്);

പൂർണ്ണസംഖ്യ open_and_check_executable(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd);
പൂർണ്ണസംഖ്യ find_executable_full(
                സ്ഥിരം അക്ഷരം *name,
                സ്ഥിരം അക്ഷരം *വേര്,
                അക്ഷരം * സ്ഥിരം *exec_തിരയൽ_path,
                bool use_path_envvar,
                അക്ഷരം **ret_filename,
                പൂർണ്ണസംഖ്യ *ret_fd);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ find_executable(സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret_filename) {
        തിരികെ find_executable_full(name, /* root= */ ശൂന്യ, ശൂന്യ, true, ret_filename, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ fsck_exists(ശൂന്യം);
പൂർണ്ണസംഖ്യ fsck_exists_for_fstype(സ്ഥിരം അക്ഷരം *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        വേണ്ടി (അക്ഷരം *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? ശൂന്യ : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        വേണ്ടി (അക്ഷരം *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                എങ്കിൽ (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

പൂർണ്ണസംഖ്യ path_find_first_component(സ്ഥിരം അക്ഷരം **p, bool accept_dot_dot, സ്ഥിരം അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ path_find_last_component(സ്ഥിരം അക്ഷരം *path, bool accept_dot_dot, സ്ഥിരം അക്ഷരം **next, സ്ഥിരം അക്ഷരം **ret);
സ്ഥിരം അക്ഷരം* last_path_component(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ path_split_prefix_filename(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_dir, അക്ഷരം **ret_filename);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ path_extract_filename(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        തിരികെ path_split_prefix_filename(path, ശൂന്യ, ret);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ path_extract_directory(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r = path_split_prefix_filename(path, ret, ശൂന്യ);
        തിരികെ r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(സ്ഥിരം അക്ഷരം *p) _pure_;
bool filename_is_valid(സ്ഥിരം അക്ഷരം *p) _pure_;
bool path_is_valid_full(സ്ഥിരം അക്ഷരം *p, bool accept_dot_dot) _pure_;
സ്ഥിതം വരിയിൽ bool path_is_valid(സ്ഥിരം അക്ഷരം *p) {
        തിരികെ path_is_valid_full(p, /* accept_dot_dot= */ true);
}
സ്ഥിതം വരിയിൽ bool path_is_safe(സ്ഥിരം അക്ഷരം *p) {
        തിരികെ path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(സ്ഥിരം അക്ഷരം *p) _pure_;
സ്ഥിതം വരിയിൽ bool filename_or_absolute_path_is_valid(സ്ഥിരം അക്ഷരം *p) {
        എങ്കിൽ (path_is_absolute(p))
                തിരികെ path_is_valid(p);

        തിരികെ filename_is_valid(p);
}

പൂർണ്ണസംഖ്യ file_in_same_dir(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *filename, അക്ഷരം **ret);

bool hidden_or_backup_file(സ്ഥിരം അക്ഷരം *filename) _pure_;

bool is_device_path(സ്ഥിരം അക്ഷരം *path) _pure_;

bool valid_device_കണ്ണി_path(സ്ഥിരം അക്ഷരം *path) _pure_;
bool valid_device_allow_pattern(സ്ഥിരം അക്ഷരം *path) _pure_;

bool dot_or_dot_dot(സ്ഥിരം അക്ഷരം *path) _pure_;

bool path_implies_directory(സ്ഥിരം അക്ഷരം *path) _pure_;

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* skip_dev_prefix(സ്ഥിരം അക്ഷരം *p) {
        സ്ഥിരം അക്ഷരം *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        തിരികെ e ?: p;
}

bool empty_or_വേര്(സ്ഥിരം അക്ഷരം *path) _pure_;
സ്ഥിരം അക്ഷരം* empty_to_വേര്(സ്ഥിരം അക്ഷരം *path) _pure_;
പൂർണ്ണസംഖ്യ empty_or_വേര്_harder_to_null(സ്ഥിരം അക്ഷരം **path);

bool path_strv_contains(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *path);
bool prefixed_path_strv_contains(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ path_glob_can_match(സ്ഥിരം അക്ഷരം *pattern, സ്ഥിരം അക്ഷരം *prefix, അക്ഷരം **ret);
