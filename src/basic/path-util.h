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

अचर वर्ण* default_PATH(शून्य);

स्थिर अन्तःस्थ अचर वर्ण* default_user_PATH(शून्य) {
#ifdef DEFAULT_USER_PATH
        फर्काऊ DEFAULT_USER_PATH EXTRA_EXEC_खोज_PATH_SUFFIX;
#अन्यथा
        फर्काऊ default_PATH();
#endif
}

bool is_path(अचर वर्ण *p) _pure_;

स्थिर अन्तःस्थ bool path_is_absolute(अचर वर्ण *p) {
        यदि (!p) /* A NULL pointer is definitely not an absolute path */
                फर्काऊ false;

        फर्काऊ p[0] == '/';
}

पूर्णाङ्क path_split_and_make_absolute(अचर वर्ण *p, वर्ण ***ret);
वर्ण* path_make_absolute(अचर वर्ण *p, अचर वर्ण *prefix);
पूर्णाङ्क safe_getcwd(वर्ण **ret);
पूर्णाङ्क path_make_absolute_cwd(अचर वर्ण *p, वर्ण **ret);
पूर्णाङ्क path_make_relative(अचर वर्ण *from, अचर वर्ण *to, वर्ण **ret);
पूर्णाङ्क path_make_relative_parent(अचर वर्ण *from_child, अचर वर्ण *to, वर्ण **ret);

प्रकारनिर्देश गणना PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

वर्ण* path_startswith_full(अचर वर्ण *path, अचर वर्ण *prefix, PathStartWithFlags flags) _pure_;
स्थिर अन्तःस्थ वर्ण* path_startswith(अचर वर्ण *path, अचर वर्ण *prefix) {
        फर्काऊ path_startswith_full(path, prefix, 0);
}

पूर्णाङ्क path_compare(अचर वर्ण *a, अचर वर्ण *b) _pure_;
स्थिर अन्तःस्थ bool path_equal(अचर वर्ण *a, अचर वर्ण *b) {
        फर्काऊ path_compare(a, b) == 0;
}

पूर्णाङ्क path_compare_filename(अचर वर्ण *a, अचर वर्ण *b);
स्थिर अन्तःस्थ bool path_equal_filename(अचर वर्ण *a, अचर वर्ण *b) {
        फर्काऊ path_compare_filename(a, b) == 0;
}

पूर्णाङ्क path_equal_or_inode_same_full(अचर वर्ण *a, अचर वर्ण *b, पूर्णाङ्क flags);
स्थिर अन्तःस्थ bool path_equal_or_inode_same(अचर वर्ण *a, अचर वर्ण *b, पूर्णाङ्क flags) {
        फर्काऊ path_equal_or_inode_same_full(a, b, flags) > 0;
}

वर्ण* path_extend_internal(वर्ण **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(रिक्त, __VA_ARGS__, POINTER_MAX)

वर्ण* skip_leading_slash(अचर वर्ण *p) _pure_;

प्रकारनिर्देश गणना PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

वर्ण* path_simplify_full(वर्ण *path, PathSimplifyFlags flags);
स्थिर अन्तःस्थ वर्ण* path_simplify(वर्ण *path) {
        फर्काऊ path_simplify_full(path, 0);
}

पूर्णाङ्क path_simplify_alloc(अचर वर्ण *path, वर्ण **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

वर्ण* path_startswith_strv(अचर वर्ण *p, वर्ण * अचर *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

पूर्णाङ्क path_strv_make_absolute_cwd(वर्ण **l);
वर्ण** path_strv_resolve(वर्ण **l, अचर वर्ण *जरा);
वर्ण** path_strv_resolve_uniq(वर्ण **l, अचर वर्ण *जरा);

पूर्णाङ्क open_and_check_executable(अचर वर्ण *name, अचर वर्ण *जरा, वर्ण **ret_path, पूर्णाङ्क *ret_fd);
पूर्णाङ्क find_executable_full(
                अचर वर्ण *name,
                अचर वर्ण *जरा,
                वर्ण * अचर *exec_खोज_path,
                bool use_path_envvar,
                वर्ण **ret_filename,
                पूर्णाङ्क *ret_fd);
स्थिर अन्तःस्थ पूर्णाङ्क find_executable(अचर वर्ण *name, वर्ण **ret_filename) {
        फर्काऊ find_executable_full(name, /* root= */ रिक्त, रिक्त, true, ret_filename, रिक्त);
}

पूर्णाङ्क fsck_exists(शून्य);
पूर्णाङ्क fsck_exists_for_fstype(अचर वर्ण *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        लागि (वर्ण *_slash = ({                                          \
                                path_simplify(म_नकल(prefix, path));    \
                                streq(prefix, "/") ? रिक्त : म_खोजप(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = म_खोजप((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        लागि (वर्ण *_slash = ({                                          \
                                path_simplify(म_नकल(prefix, path));    \
                                यदि (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                म_खोजप(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = म_खोजप((prefix), '/'))

पूर्णाङ्क path_find_first_component(अचर वर्ण **p, bool accept_dot_dot, अचर वर्ण **ret);
पूर्णाङ्क path_find_last_component(अचर वर्ण *path, bool accept_dot_dot, अचर वर्ण **next, अचर वर्ण **ret);
अचर वर्ण* last_path_component(अचर वर्ण *path);

पूर्णाङ्क path_split_prefix_filename(अचर वर्ण *path, वर्ण **ret_dir, वर्ण **ret_filename);
स्थिर अन्तःस्थ पूर्णाङ्क path_extract_filename(अचर वर्ण *path, वर्ण **ret) {
        फर्काऊ path_split_prefix_filename(path, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क path_extract_directory(अचर वर्ण *path, वर्ण **ret) {
        पूर्णाङ्क r = path_split_prefix_filename(path, ret, रिक्त);
        फर्काऊ r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(अचर वर्ण *p) _pure_;
bool filename_is_valid(अचर वर्ण *p) _pure_;
bool path_is_valid_full(अचर वर्ण *p, bool accept_dot_dot) _pure_;
स्थिर अन्तःस्थ bool path_is_valid(अचर वर्ण *p) {
        फर्काऊ path_is_valid_full(p, /* accept_dot_dot= */ true);
}
स्थिर अन्तःस्थ bool path_is_safe(अचर वर्ण *p) {
        फर्काऊ path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(अचर वर्ण *p) _pure_;
स्थिर अन्तःस्थ bool filename_or_absolute_path_is_valid(अचर वर्ण *p) {
        यदि (path_is_absolute(p))
                फर्काऊ path_is_valid(p);

        फर्काऊ filename_is_valid(p);
}

पूर्णाङ्क file_in_same_dir(अचर वर्ण *path, अचर वर्ण *filename, वर्ण **ret);

bool hidden_or_backup_file(अचर वर्ण *filename) _pure_;

bool is_device_path(अचर वर्ण *path) _pure_;

bool valid_device_गाँठो_path(अचर वर्ण *path) _pure_;
bool valid_device_allow_pattern(अचर वर्ण *path) _pure_;

bool dot_or_dot_dot(अचर वर्ण *path) _pure_;

bool path_implies_directory(अचर वर्ण *path) _pure_;

स्थिर अन्तःस्थ अचर वर्ण* skip_dev_prefix(अचर वर्ण *p) {
        अचर वर्ण *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        फर्काऊ e ?: p;
}

bool empty_or_जरा(अचर वर्ण *path) _pure_;
अचर वर्ण* empty_to_जरा(अचर वर्ण *path) _pure_;
पूर्णाङ्क empty_or_जरा_harder_to_null(अचर वर्ण **path);

bool path_strv_contains(वर्ण * अचर *l, अचर वर्ण *path);
bool prefixed_path_strv_contains(वर्ण * अचर *l, अचर वर्ण *path);

पूर्णाङ्क path_glob_can_match(अचर वर्ण *pattern, अचर वर्ण *prefix, वर्ण **ret);
