/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_शोध_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_शोध_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_शोध_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_शोध_PATH_SUFFIX

स्थिरांक अक्षर* default_PATH(शून्य);

स्थिर ओळीत स्थिरांक अक्षर* default_user_PATH(शून्य) {
#ifdef DEFAULT_USER_PATH
        परत DEFAULT_USER_PATH EXTRA_EXEC_शोध_PATH_SUFFIX;
#अन्यथा
        परत default_PATH();
#endif
}

bool is_path(स्थिरांक अक्षर *p) _pure_;

स्थिर ओळीत bool path_is_absolute(स्थिरांक अक्षर *p) {
        जर (!p) /* A NULL pointer is definitely not an absolute path */
                परत false;

        परत p[0] == '/';
}

पूर्णांक path_split_and_make_absolute(स्थिरांक अक्षर *p, अक्षर ***ret);
अक्षर* path_make_absolute(स्थिरांक अक्षर *p, स्थिरांक अक्षर *prefix);
पूर्णांक safe_getcwd(अक्षर **ret);
पूर्णांक path_make_absolute_cwd(स्थिरांक अक्षर *p, अक्षर **ret);
पूर्णांक path_make_relative(स्थिरांक अक्षर *from, स्थिरांक अक्षर *to, अक्षर **ret);
पूर्णांक path_make_relative_parent(स्थिरांक अक्षर *from_child, स्थिरांक अक्षर *to, अक्षर **ret);

प्रकारव्याख्या गणना PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

अक्षर* path_startswith_full(स्थिरांक अक्षर *path, स्थिरांक अक्षर *prefix, PathStartWithFlags flags) _pure_;
स्थिर ओळीत अक्षर* path_startswith(स्थिरांक अक्षर *path, स्थिरांक अक्षर *prefix) {
        परत path_startswith_full(path, prefix, 0);
}

पूर्णांक path_compare(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) _pure_;
स्थिर ओळीत bool path_equal(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        परत path_compare(a, b) == 0;
}

पूर्णांक path_compare_filename(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b);
स्थिर ओळीत bool path_equal_filename(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b) {
        परत path_compare_filename(a, b) == 0;
}

पूर्णांक path_equal_or_inode_same_full(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b, पूर्णांक flags);
स्थिर ओळीत bool path_equal_or_inode_same(स्थिरांक अक्षर *a, स्थिरांक अक्षर *b, पूर्णांक flags) {
        परत path_equal_or_inode_same_full(a, b, flags) > 0;
}

अक्षर* path_extend_internal(अक्षर **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(रिक्त, __VA_ARGS__, POINTER_MAX)

अक्षर* skip_leading_slash(स्थिरांक अक्षर *p) _pure_;

प्रकारव्याख्या गणना PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

अक्षर* path_simplify_full(अक्षर *path, PathSimplifyFlags flags);
स्थिर ओळीत अक्षर* path_simplify(अक्षर *path) {
        परत path_simplify_full(path, 0);
}

पूर्णांक path_simplify_alloc(स्थिरांक अक्षर *path, अक्षर **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

अक्षर* path_startswith_strv(स्थिरांक अक्षर *p, अक्षर * स्थिरांक *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

पूर्णांक path_strv_make_absolute_cwd(अक्षर **l);
अक्षर** path_strv_resolve(अक्षर **l, स्थिरांक अक्षर *मूळ);
अक्षर** path_strv_resolve_uniq(अक्षर **l, स्थिरांक अक्षर *मूळ);

पूर्णांक open_and_check_executable(स्थिरांक अक्षर *name, स्थिरांक अक्षर *मूळ, अक्षर **ret_path, पूर्णांक *ret_fd);
पूर्णांक find_executable_full(
                स्थिरांक अक्षर *name,
                स्थिरांक अक्षर *मूळ,
                अक्षर * स्थिरांक *exec_शोध_path,
                bool use_path_envvar,
                अक्षर **ret_filename,
                पूर्णांक *ret_fd);
स्थिर ओळीत पूर्णांक find_executable(स्थिरांक अक्षर *name, अक्षर **ret_filename) {
        परत find_executable_full(name, /* root= */ रिक्त, रिक्त, true, ret_filename, रिक्त);
}

पूर्णांक fsck_exists(शून्य);
पूर्णांक fsck_exists_for_fstype(स्थिरांक अक्षर *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        साठी (अक्षर *_slash = ({                                          \
                                path_simplify(म_नकल(prefix, path));    \
                                streq(prefix, "/") ? रिक्त : म_खोजप(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = म_खोजप((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        साठी (अक्षर *_slash = ({                                          \
                                path_simplify(म_नकल(prefix, path));    \
                                जर (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                म_खोजप(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = म_खोजप((prefix), '/'))

पूर्णांक path_find_first_component(स्थिरांक अक्षर **p, bool accept_dot_dot, स्थिरांक अक्षर **ret);
पूर्णांक path_find_last_component(स्थिरांक अक्षर *path, bool accept_dot_dot, स्थिरांक अक्षर **next, स्थिरांक अक्षर **ret);
स्थिरांक अक्षर* last_path_component(स्थिरांक अक्षर *path);

पूर्णांक path_split_prefix_filename(स्थिरांक अक्षर *path, अक्षर **ret_dir, अक्षर **ret_filename);
स्थिर ओळीत पूर्णांक path_extract_filename(स्थिरांक अक्षर *path, अक्षर **ret) {
        परत path_split_prefix_filename(path, रिक्त, ret);
}
स्थिर ओळीत पूर्णांक path_extract_directory(स्थिरांक अक्षर *path, अक्षर **ret) {
        पूर्णांक r = path_split_prefix_filename(path, ret, रिक्त);
        परत r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(स्थिरांक अक्षर *p) _pure_;
bool filename_is_valid(स्थिरांक अक्षर *p) _pure_;
bool path_is_valid_full(स्थिरांक अक्षर *p, bool accept_dot_dot) _pure_;
स्थिर ओळीत bool path_is_valid(स्थिरांक अक्षर *p) {
        परत path_is_valid_full(p, /* accept_dot_dot= */ true);
}
स्थिर ओळीत bool path_is_safe(स्थिरांक अक्षर *p) {
        परत path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(स्थिरांक अक्षर *p) _pure_;
स्थिर ओळीत bool filename_or_absolute_path_is_valid(स्थिरांक अक्षर *p) {
        जर (path_is_absolute(p))
                परत path_is_valid(p);

        परत filename_is_valid(p);
}

पूर्णांक file_in_same_dir(स्थिरांक अक्षर *path, स्थिरांक अक्षर *filename, अक्षर **ret);

bool hidden_or_backup_file(स्थिरांक अक्षर *filename) _pure_;

bool is_device_path(स्थिरांक अक्षर *path) _pure_;

bool valid_device_गाठ_path(स्थिरांक अक्षर *path) _pure_;
bool valid_device_allow_pattern(स्थिरांक अक्षर *path) _pure_;

bool dot_or_dot_dot(स्थिरांक अक्षर *path) _pure_;

bool path_implies_directory(स्थिरांक अक्षर *path) _pure_;

स्थिर ओळीत स्थिरांक अक्षर* skip_dev_prefix(स्थिरांक अक्षर *p) {
        स्थिरांक अक्षर *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        परत e ?: p;
}

bool empty_or_मूळ(स्थिरांक अक्षर *path) _pure_;
स्थिरांक अक्षर* empty_to_मूळ(स्थिरांक अक्षर *path) _pure_;
पूर्णांक empty_or_मूळ_harder_to_null(स्थिरांक अक्षर **path);

bool path_strv_contains(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *path);
bool prefixed_path_strv_contains(अक्षर * स्थिरांक *l, स्थिरांक अक्षर *path);

पूर्णांक path_glob_can_match(स्थिरांक अक्षर *pattern, स्थिरांक अक्षर *prefix, अक्षर **ret);
