/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define PATH_SPLIT_BIN(x) x "sbin:" x "bin"
#define PATH_SPLIT_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#define PATH_MERGED_BIN(x) x "bin"
#define PATH_MERGED_BIN_NULSTR(x) x "bin\0"

#define DEFAULT_PATH_WITH_FULL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") EXTRA_EXEC_ସନ୍ଧାନ_PATH_SUFFIX
#define DEFAULT_PATH_WITH_LOCAL_SBIN PATH_SPLIT_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_ସନ୍ଧାନ_PATH_SUFFIX
#define DEFAULT_PATH_WITHOUT_SBIN PATH_MERGED_BIN("/usr/local/") ":" PATH_MERGED_BIN("/usr/") EXTRA_EXEC_ସନ୍ଧାନ_PATH_SUFFIX

#define DEFAULT_PATH_COMPAT PATH_SPLIT_BIN("/usr/local/") ":" PATH_SPLIT_BIN("/usr/") ":" PATH_SPLIT_BIN("/") EXTRA_EXEC_ସନ୍ଧାନ_PATH_SUFFIX

ଅଚଳ ଅକ୍ଷର* default_PATH(ଶୂନ୍ୟ);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* default_user_PATH(ଶୂନ୍ୟ) {
#ifdef DEFAULT_USER_PATH
        ଫେରନ୍ତୁ DEFAULT_USER_PATH EXTRA_EXEC_ସନ୍ଧାନ_PATH_SUFFIX;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ default_PATH();
#endif
}

bool is_path(ଅଚଳ ଅକ୍ଷର *p) _pure_;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool path_is_absolute(ଅଚଳ ଅକ୍ଷର *p) {
        ଯଦି (!p) /* A NULL pointer is definitely not an absolute path */
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ p[0] == '/';
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_split_and_make_absolute(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର ***ret);
ଅକ୍ଷର* path_make_absolute(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *prefix);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_getcwd(ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_make_absolute_cwd(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_make_relative(ଅଚଳ ଅକ୍ଷର *from, ଅଚଳ ଅକ୍ଷର *to, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_make_relative_parent(ଅଚଳ ଅକ୍ଷର *from_child, ଅଚଳ ଅକ୍ଷର *to, ଅକ୍ଷର **ret);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା PathStartWithFlags {
        PATH_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        PATH_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} PathStartWithFlags;

ଅକ୍ଷର* path_startswith_full(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *prefix, PathStartWithFlags flags) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* path_startswith(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *prefix) {
        ଫେରନ୍ତୁ path_startswith_full(path, prefix, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_compare(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool path_equal(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        ଫେରନ୍ତୁ path_compare(a, b) == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_compare_filename(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool path_equal_filename(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        ଫେରନ୍ତୁ path_compare_filename(a, b) == 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_equal_or_inode_same_full(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool path_equal_or_inode_same(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ଫେରନ୍ତୁ path_equal_or_inode_same_full(a, b, flags) > 0;
}

ଅକ୍ଷର* path_extend_internal(ଅକ୍ଷର **x, ...);
#define path_extend(x, ...) path_extend_internal(x, __VA_ARGS__, POINTER_MAX)
#define path_join(...) path_extend_internal(ଖାଲି, __VA_ARGS__, POINTER_MAX)

ଅକ୍ଷର* skip_leading_slash(ଅଚଳ ଅକ୍ଷର *p) _pure_;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା PathSimplifyFlags {
        PATH_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} PathSimplifyFlags;

ଅକ୍ଷର* path_simplify_full(ଅକ୍ଷର *path, PathSimplifyFlags flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* path_simplify(ଅକ୍ଷର *path) {
        ଫେରନ୍ତୁ path_simplify_full(path, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_simplify_alloc(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret);

/* Note: the search terminates on the first NULL item. */
#define PATH_IN_SET(p, ...) path_strv_contains(STRV_MAKE(__VA_ARGS__), p)

ଅକ୍ଷର* path_startswith_strv(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର * ଅଚଳ *strv);
#define PATH_STARTSWITH_SET(p, ...) path_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_strv_make_absolute_cwd(ଅକ୍ଷର **l);
ଅକ୍ଷର** path_strv_resolve(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *ମୂଳ);
ଅକ୍ଷର** path_strv_resolve_uniq(ଅକ୍ଷର **l, ଅଚଳ ଅକ୍ଷର *ମୂଳ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_and_check_executable(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_executable_full(
                ଅଚଳ ଅକ୍ଷର *name,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅକ୍ଷର * ଅଚଳ *exec_ସନ୍ଧାନ_path,
                bool use_path_envvar,
                ଅକ୍ଷର **ret_filename,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_executable(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret_filename) {
        ଫେରନ୍ତୁ find_executable_full(name, /* root= */ ଖାଲି, ଖାଲି, true, ret_filename, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsck_exists(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsck_exists_for_fstype(ଅଚଳ ଅକ୍ଷର *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#define PATH_FOREACH_PREFIX(prefix, path)                               \
        ପାଇଁ (ଅକ୍ଷର *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                streq(prefix, "/") ? ଖାଲି : strrchr(prefix, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#define PATH_FOREACH_PREFIX_MORE(prefix, path)                          \
        ପାଇଁ (ଅକ୍ଷର *_slash = ({                                          \
                                path_simplify(strcpy(prefix, path));    \
                                ଯଦି (streq(prefix, "/"))                 \
                                        prefix[0] = 0;                  \
                                strrchr(prefix, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((prefix), '/'))

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_find_first_component(ଅଚଳ ଅକ୍ଷର **p, bool accept_dot_dot, ଅଚଳ ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_find_last_component(ଅଚଳ ଅକ୍ଷର *path, bool accept_dot_dot, ଅଚଳ ଅକ୍ଷର **next, ଅଚଳ ଅକ୍ଷର **ret);
ଅଚଳ ଅକ୍ଷର* last_path_component(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_split_prefix_filename(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_dir, ଅକ୍ଷର **ret_filename);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_extract_filename(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ path_split_prefix_filename(path, ଖାଲି, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_extract_directory(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = path_split_prefix_filename(path, ret, ଖାଲି);
        ଫେରନ୍ତୁ r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ଅଚଳ ଅକ୍ଷର *p) _pure_;
bool filename_is_valid(ଅଚଳ ଅକ୍ଷର *p) _pure_;
bool path_is_valid_full(ଅଚଳ ଅକ୍ଷର *p, bool accept_dot_dot) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool path_is_valid(ଅଚଳ ଅକ୍ଷର *p) {
        ଫେରନ୍ତୁ path_is_valid_full(p, /* accept_dot_dot= */ true);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool path_is_safe(ଅଚଳ ଅକ୍ଷର *p) {
        ଫେରନ୍ତୁ path_is_valid_full(p, /* accept_dot_dot= */ false);
}
bool path_is_normalized(ଅଚଳ ଅକ୍ଷର *p) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool filename_or_absolute_path_is_valid(ଅଚଳ ଅକ୍ଷର *p) {
        ଯଦି (path_is_absolute(p))
                ଫେରନ୍ତୁ path_is_valid(p);

        ଫେରନ୍ତୁ filename_is_valid(p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା file_in_same_dir(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *filename, ଅକ୍ଷର **ret);

bool hidden_or_backup_file(ଅଚଳ ଅକ୍ଷର *filename) _pure_;

bool is_device_path(ଅଚଳ ଅକ୍ଷର *path) _pure_;

bool valid_device_ଗଣ୍ଠି_path(ଅଚଳ ଅକ୍ଷର *path) _pure_;
bool valid_device_allow_pattern(ଅଚଳ ଅକ୍ଷର *path) _pure_;

bool dot_or_dot_dot(ଅଚଳ ଅକ୍ଷର *path) _pure_;

bool path_implies_directory(ଅଚଳ ଅକ୍ଷର *path) _pure_;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* skip_dev_prefix(ଅଚଳ ଅକ୍ଷର *p) {
        ଅଚଳ ଅକ୍ଷର *e;

        /* Drop any /dev prefix if there is any */

        e = path_startswith(p, "/dev/");

        ଫେରନ୍ତୁ e ?: p;
}

bool empty_or_ମୂଳ(ଅଚଳ ଅକ୍ଷର *path) _pure_;
ଅଚଳ ଅକ୍ଷର* empty_to_ମୂଳ(ଅଚଳ ଅକ୍ଷର *path) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା empty_or_ମୂଳ_harder_to_null(ଅଚଳ ଅକ୍ଷର **path);

bool path_strv_contains(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *path);
bool prefixed_path_strv_contains(ଅକ୍ଷର * ଅଚଳ *l, ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_glob_can_match(ଅଚଳ ଅକ୍ଷର *pattern, ଅଚଳ ଅକ୍ଷର *prefix, ଅକ୍ଷର **ret);
