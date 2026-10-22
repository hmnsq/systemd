/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_खोज_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_खोज_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_खोज_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_खोज_PATH_SUFFIX

ध्रुव वर्ण* default_PATH(शून्य);

स्थिर अन्तःस्थ ध्रुव वर्ण* default_user_PATH(शून्य) {
#ifdef DEFAULT_USER_PATH
        प्रत्यागम DEFAULT_USER_PATH EXTRA_EXEC_खोज_PATH_SUFFIX;
#अन्यथा
        प्रत्यागम default_PATH();
#endif
}

bool is_path(ध्रुव वर्ण *p) _pure_;

स्थिर अन्तःस्थ bool path_is_absolute(ध्रुव वर्ण *p) {
        यदि (!p) /* A NULL pointer is definitely not an absolute path */
                प्रत्यागम false;

        प्रत्यागम p[0] == '/';
}

पूर्णाङ्क path_split_and_make_absolute(ध्रुव वर्ण *p, वर्ण ***ret);
वर्ण* path_make_absolute(ध्रुव वर्ण *p, ध्रुव वर्ण *prefix);
पूर्णाङ्क safe_getcwd(वर्ण **ret);
पूर्णाङ्क path_make_absolute_cwd(ध्रुव वर्ण *p, वर्ण **ret);
पूर्णाङ्क path_make_relative(ध्रुव वर्ण *from, ध्रुव वर्ण *to, वर्ण **ret);
पूर्णाङ्क path_make_relative_parent(ध्रुव वर्ण *from_child, ध्रुव वर्ण *to, वर्ण **ret);

प्रकारनिर्देश गणना PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

वर्ण* path_startswith_full(ध्रुव वर्ण *path, ध्रुव वर्ण *prefix, PathStartWithFlags flags) _pure_;
स्थिर अन्तःस्थ वर्ण* path_startswith(ध्रुव वर्ण *path, ध्रुव वर्ण *prefix) {
        प्रत्यागम path_startswith_full(path, prefix, 0);
}

पूर्णाङ्क path_compare(ध्रुव वर्ण *a, ध्रुव वर्ण *b) _pure_;
स्थिर अन्तःस्थ bool path_equal(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        प्रत्यागम path_compare(a, b) == 0;
}

पूर्णाङ्क path_compare_filename(ध्रुव वर्ण *a, ध्रुव वर्ण *b);
स्थिर अन्तःस्थ bool path_equal_filename(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        प्रत्यागम path_compare_filename(a, b) == 0;
}

पूर्णाङ्क path_equal_or_inode_same_full(ध्रुव वर्ण *a, ध्रुव वर्ण *b, पूर्णाङ्क flags);
स्थिर अन्तःस्थ bool path_equal_or_inode_same(ध्रुव वर्ण *a, ध्रुव वर्ण *b, पूर्णाङ्क flags) {
        प्रत्यागम path_equal_or_inode_same_full(a, b, flags) > 0;
}

वर्ण* path_extend_internal(वर्ण **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(रिक्त, __VA_ARGS__, POINTER_MAX)

वर्ण* skip_leading_slash(ध्रुव वर्ण *p) _pure_;

प्रकारनिर्देश गणना PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

वर्ण* path_simplify_full(वर्ण *path, PathSimplifyFlags flags);
स्थिर अन्तःस्थ वर्ण* path_simplify(वर्ण *path) {
        प्रत्यागम path_simplify_full(path, 0);
}

पूर्णाङ्क path_simplify_alloc(ध्रुव वर्ण *path, वर्ण **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

वर्ण* path_startswith_strv(ध्रुव वर्ण *p, वर्ण * ध्रुव *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

पूर्णाङ्क path_strv_make_absolute_cwd(वर्ण **l);
वर्ण** path_strv_resolve(वर्ण **l, ध्रुव वर्ण *मूल);
वर्ण** path_strv_resolve_uniq(वर्ण **l, ध्रुव वर्ण *मूल);

पूर्णाङ्क open_and_check_executable(ध्रुव वर्ण *name, ध्रुव वर्ण *मूल, वर्ण **ret_path, पूर्णाङ्क *ret_fd);
पूर्णाङ्क find_executable_full(
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मूल,
                वर्ण * ध्रुव *exec_खोज_path,
                bool use_path_envvar,
                वर्ण **ret_filename,
                पूर्णाङ्क *ret_fd);
स्थिर अन्तःस्थ पूर्णाङ्क find_executable(ध्रुव वर्ण *name, वर्ण **ret_filename) {
        प्रत्यागम find_executable_full(name, /* root= */ रिक्त, रिक्त, true, ret_filename, रिक्त);
}

पूर्णाङ्क fsck_exists(शून्य);
पूर्णाङ्क fsck_exists_for_fstype(ध्रुव वर्ण *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        प्रति (वर्ण *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? रिक्त : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        प्रति (वर्ण *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                यदि (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

पूर्णाङ्क path_find_first_component(ध्रुव वर्ण **p, bool accept_dot_dot, ध्रुव वर्ण **ret);
पूर्णाङ्क path_find_last_component(ध्रुव वर्ण *path, bool accept_dot_dot, ध्रुव वर्ण **next, ध्रुव वर्ण **ret);
ध्रुव वर्ण* last_path_component(ध्रुव वर्ण *path);

पूर्णाङ्क path_split_prefix_filename(ध्रुव वर्ण *path, वर्ण **ret_dir, वर्ण **ret_filename);
स्थिर अन्तःस्थ पूर्णाङ्क path_extract_filename(ध्रुव वर्ण *path, वर्ण **ret) {
        प्रत्यागम path_split_prefix_filename(path, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क path_extract_directory(ध्रुव वर्ण *path, वर्ण **ret) {
        पूर्णाङ्क r = path_split_prefix_filename(path, ret, रिक्त);
        प्रत्यागम r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ध्रुव वर्ण *p) _pure_;
bool filename_is_valid(ध्रुव वर्ण *p) _pure_;
bool path_is_valid_full(ध्रुव वर्ण *p, bool accept_dot_dot) _pure_;
स्थिर अन्तःस्थ bool path_is_valid(ध्रुव वर्ण *p) {
        प्रत्यागम path_is_valid_full(p, /* accept_dot_dot= */ true);
}
स्थिर अन्तःस्थ bool path_is_safe(ध्रुव वर्ण *p) {
        प्रत्यागम path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(ध्रुव वर्ण *p) _pure_;
स्थिर अन्तःस्थ bool filename_or_absolute_path_is_valid(ध्रुव वर्ण *p) {
        यदि (path_is_absolute(p))
                प्रत्यागम path_is_valid(p);

        प्रत्यागम filename_is_valid(p);
}

पूर्णाङ्क file_in_same_dir(ध्रुव वर्ण *path, ध्रुव वर्ण *filename, वर्ण **ret);

bool hidden_or_backup_file(ध्रुव वर्ण *filename) _pure_;

bool is_device_path(ध्रुव वर्ण *path) _pure_;

bool valid_device_गाँठ_path(ध्रुव वर्ण *path) _pure_;
bool valid_device_allow_pattern(ध्रुव वर्ण *path) _pure_;

bool dot_or_dot_dot(ध्रुव वर्ण *path) _pure_;

bool path_implies_directory(ध्रुव वर्ण *path) _pure_;

स्थिर अन्तःस्थ ध्रुव वर्ण* skip_dev_prefix(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        प्रत्यागम e ?: p;
}

bool empty_or_मूल(ध्रुव वर्ण *path) _pure_;
ध्रुव वर्ण* empty_to_मूल(ध्रुव वर्ण *path) _pure_;
पूर्णाङ्क empty_or_मूल_harder_to_null(ध्रुव वर्ण **path);

bool path_strv_contains(वर्ण * ध्रुव *l, ध्रुव वर्ण *path);
bool prefixed_path_strv_contains(वर्ण * ध्रुव *l, ध्रुव वर्ण *path);

पूर्णाङ्क path_glob_can_match(ध्रुव वर्ण *pattern, ध्रुव वर्ण *prefix, वर्ण **ret);
