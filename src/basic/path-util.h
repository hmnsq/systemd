/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#घोषणा PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#घोषणा PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#घोषणा PATH_MERGED_BIN(x) x "bin"
#घोषणा PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#घोषणा DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_खोज_PATH_SUFFIX
#घोषणा DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_खोज_PATH_SUFFIX
#घोषणा DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_खोज_PATH_SUFFIX

#घोषणा DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_खोज_PATH_SUFFIX

स्थिर अक्षर* default_PATH(व्योम);

अटल अंतरभूत स्थिर अक्षर* default_user_PATH(व्योम) {
#अगर_घोषित DEFAULT_USER_PATH
        वापस DEFAULT_USER_PATH EXTRA_EXEC_खोज_PATH_SUFFIX;
#अन्यथा
        वापस default_PATH();
#पूर्ण_अगर
}

bool is_path(स्थिर अक्षर *p) _pure_;

अटल अंतरभूत bool path_is_absolute(स्थिर अक्षर *p) {
        अगर (!p) /* A NULL pointer is definitely not an absolute path */
                वापस false;

        वापस p[0] == '/';
}

पूर्णांक path_split_and_make_absolute(स्थिर अक्षर *p, अक्षर ***ret);
अक्षर* path_make_absolute(स्थिर अक्षर *p, स्थिर अक्षर *prefix);
पूर्णांक safe_getcwd(अक्षर **ret);
पूर्णांक path_make_absolute_cwd(स्थिर अक्षर *p, अक्षर **ret);
पूर्णांक path_make_relative(स्थिर अक्षर *from, स्थिर अक्षर *to, अक्षर **ret);
पूर्णांक path_make_relative_parent(स्थिर अक्षर *from_child, स्थिर अक्षर *to, अक्षर **ret);

प्रकार क्रमागत PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

अक्षर* path_startswith_full(स्थिर अक्षर *path, स्थिर अक्षर *prefix, PathStartWithFlags flags) _pure_;
अटल अंतरभूत अक्षर* path_startswith(स्थिर अक्षर *path, स्थिर अक्षर *prefix) {
        वापस path_startswith_full(path, prefix, 0);
}

पूर्णांक path_compare(स्थिर अक्षर *a, स्थिर अक्षर *b) _pure_;
अटल अंतरभूत bool path_equal(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        वापस path_compare(a, b) == 0;
}

पूर्णांक path_compare_filename(स्थिर अक्षर *a, स्थिर अक्षर *b);
अटल अंतरभूत bool path_equal_filename(स्थिर अक्षर *a, स्थिर अक्षर *b) {
        वापस path_compare_filename(a, b) == 0;
}

पूर्णांक path_equal_or_inode_same_full(स्थिर अक्षर *a, स्थिर अक्षर *b, पूर्णांक flags);
अटल अंतरभूत bool path_equal_or_inode_same(स्थिर अक्षर *a, स्थिर अक्षर *b, पूर्णांक flags) {
        वापस path_equal_or_inode_same_full(a, b, flags) > 0;
}

अक्षर* path_extend_internal(अक्षर **x, ...);
#घोषणा path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#घोषणा path_join(...) path_extend_internal(शून्य, __VA_ARGS__, POINTER_MAX)

अक्षर* skip_leading_slash(स्थिर अक्षर *p) _pure_;

प्रकार क्रमागत PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

अक्षर* path_simplify_full(अक्षर *path, PathSimplifyFlags flags);
अटल अंतरभूत अक्षर* path_simplify(अक्षर *path) {
        वापस path_simplify_full(path, 0);
}

पूर्णांक path_simplify_alloc(स्थिर अक्षर *path, अक्षर **ret);

/* Note: the search terminates on the first NULL item. */
#घोषणा PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

अक्षर* path_startswith_strv(स्थिर अक्षर *p, अक्षर * स्थिर *strv);
#घोषणा PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

पूर्णांक path_strv_make_absolute_cwd(अक्षर **l);
अक्षर** path_strv_resolve(अक्षर **l, स्थिर अक्षर *मूल);
अक्षर** path_strv_resolve_uniq(अक्षर **l, स्थिर अक्षर *मूल);

पूर्णांक open_and_check_executable(स्थिर अक्षर *name, स्थिर अक्षर *मूल, अक्षर **ret_path, पूर्णांक *ret_fd);
पूर्णांक find_executable_full(
                स्थिर अक्षर *name,
                स्थिर अक्षर *मूल,
                अक्षर * स्थिर *exec_खोज_path,
                bool use_path_envvar,
                अक्षर **ret_filename,
                पूर्णांक *ret_fd);
अटल अंतरभूत पूर्णांक find_executable(स्थिर अक्षर *name, अक्षर **ret_filename) {
        वापस find_executable_full(name, /* root= */ शून्य, शून्य, true, ret_filename, शून्य);
}

पूर्णांक fsck_exists(व्योम);
पूर्णांक fsck_exists_for_fstype(स्थिर अक्षर *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#घोषणा PATH_FOREACH_PREFIX(prefix, path)                               \
        क्रम (अक्षर *_slash = ({                                          \
                                path_simplify(म_नकल(prefix, path));    \
                                streq(prefix, "/") ? शून्य : म_खोजप(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = म_खोजप((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#घोषणा PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        क्रम (अक्षर *_slash = ({                                          \
                                path_simplify(म_नकल(prefix, path));    \
                                अगर (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                म_खोजप(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = म_खोजप((prefix), '/'))

पूर्णांक path_find_first_component(स्थिर अक्षर **p, bool accept_dot_dot, स्थिर अक्षर **ret);
पूर्णांक path_find_last_component(स्थिर अक्षर *path, bool accept_dot_dot, स्थिर अक्षर **अग्र_2, स्थिर अक्षर **ret);
स्थिर अक्षर* last_path_component(स्थिर अक्षर *path);

पूर्णांक path_split_prefix_filename(स्थिर अक्षर *path, अक्षर **ret_dir, अक्षर **ret_filename);
अटल अंतरभूत पूर्णांक path_extract_filename(स्थिर अक्षर *path, अक्षर **ret) {
        वापस path_split_prefix_filename(path, शून्य, ret);
}
अटल अंतरभूत पूर्णांक path_extract_directory(स्थिर अक्षर *path, अक्षर **ret) {
        पूर्णांक r = path_split_prefix_filename(path, ret, शून्य);
        वापस r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(स्थिर अक्षर *p) _pure_;
bool filename_is_valid(स्थिर अक्षर *p) _pure_;
bool path_is_valid_full(स्थिर अक्षर *p, bool accept_dot_dot) _pure_;
अटल अंतरभूत bool path_is_valid(स्थिर अक्षर *p) {
        वापस path_is_valid_full(p, /* accept_dot_dot= */ true);
}
अटल अंतरभूत bool path_is_safe(स्थिर अक्षर *p) {
        वापस path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(स्थिर अक्षर *p) _pure_;
अटल अंतरभूत bool filename_or_absolute_path_is_valid(स्थिर अक्षर *p) {
        अगर (path_is_absolute(p))
                वापस path_is_valid(p);

        वापस filename_is_valid(p);
}

पूर्णांक file_in_same_dir(स्थिर अक्षर *path, स्थिर अक्षर *filename, अक्षर **ret);

bool hidden_or_backup_file(स्थिर अक्षर *filename) _pure_;

bool is_device_path(स्थिर अक्षर *path) _pure_;

bool valid_device_गाँठ_path(स्थिर अक्षर *path) _pure_;
bool valid_device_allow_pattern(स्थिर अक्षर *path) _pure_;

bool dot_or_dot_dot(स्थिर अक्षर *path) _pure_;

bool path_implies_directory(स्थिर अक्षर *path) _pure_;

अटल अंतरभूत स्थिर अक्षर* skip_dev_prefix(स्थिर अक्षर *p) {
        स्थिर अक्षर *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        वापस e ?: p;
}

bool empty_or_मूल(स्थिर अक्षर *path) _pure_;
स्थिर अक्षर* empty_to_मूल(स्थिर अक्षर *path) _pure_;
पूर्णांक empty_or_मूल_harder_to_null(स्थिर अक्षर **path);

bool path_strv_contains(अक्षर * स्थिर *l, स्थिर अक्षर *path);
bool prefixed_path_strv_contains(अक्षर * स्थिर *l, स्थिर अक्षर *path);

पूर्णांक path_glob_can_match(स्थिर अक्षर *pattern, स्थिर अक्षर *prefix, अक्षर **ret);
