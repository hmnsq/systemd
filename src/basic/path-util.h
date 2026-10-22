/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_ਵੰਡੋ_BIN(x) x "sbin:" x "bin"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_ਵੰਡੋ_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_MERGED_BIN(x) x "bin"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_MERGED_BIN_NULSTR(x) x "bin\0"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਰਸਤਾ_WITH_ਭਰਿਆ_SBIN ਰਸਤਾ_ਵੰਡੋ_BIN("/usr/local/") ":" ਰਸਤਾ_ਵੰਡੋ_BIN("/usr/") EXTRA_EXEC_ਖੋਜ_ਰਸਤਾ_SUFFIX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਰਸਤਾ_WITH_LOCAL_SBIN ਰਸਤਾ_ਵੰਡੋ_BIN("/usr/local/") ":" ਰਸਤਾ_MERGED_BIN("/usr/") EXTRA_EXEC_ਖੋਜ_ਰਸਤਾ_SUFFIX
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਰਸਤਾ_WITHOUT_SBIN ਰਸਤਾ_MERGED_BIN("/usr/local/") ":" ਰਸਤਾ_MERGED_BIN("/usr/") EXTRA_EXEC_ਖੋਜ_ਰਸਤਾ_SUFFIX

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ਰਸਤਾ_COMPAT ਰਸਤਾ_ਵੰਡੋ_BIN("/usr/local/") ":" ਰਸਤਾ_ਵੰਡੋ_BIN("/usr/") ":" ਰਸਤਾ_ਵੰਡੋ_BIN("/") EXTRA_EXEC_ਖੋਜ_ਰਸਤਾ_SUFFIX

ਅਚਲ ਅੱਖਰ* default_ਰਸਤਾ(ਖਾਲੀ);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* default_user_ਰਸਤਾ(ਖਾਲੀ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEFAULT_USER_ਰਸਤਾ
        ਵਾਪਸ DEFAULT_USER_ਰਸਤਾ EXTRA_EXEC_ਖੋਜ_ਰਸਤਾ_SUFFIX;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ default_ਰਸਤਾ();
#ਜੇ_ਅੰਤ
}

bool is_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;

ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਰਸਤਾ_is_absolute(ਅਚਲ ਅੱਖਰ *p) {
        ਜੇ (!p) /* A NULL pointer is definitely not an absolute path */
                ਵਾਪਸ false;

        ਵਾਪਸ p[0] == '/';
}

ਪੂਰਨ ਰਸਤਾ_ਵੰਡੋ_and_make_absolute(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ ***ret);
ਅੱਖਰ* ਰਸਤਾ_make_absolute(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ);
ਪੂਰਨ ਸੁਰੱਖਿਅਤ_getcwd(ਅੱਖਰ **ret);
ਪੂਰਨ ਰਸਤਾ_make_absolute_cwd(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret);
ਪੂਰਨ ਰਸਤਾ_make_relative(ਅਚਲ ਅੱਖਰ *from, ਅਚਲ ਅੱਖਰ *to, ਅੱਖਰ **ret);
ਪੂਰਨ ਰਸਤਾ_make_relative_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ *from_ਬੱਚਾ, ਅਚਲ ਅੱਖਰ *to, ਅੱਖਰ **ret);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਰਸਤਾ_ਸ਼ੁਰੂ_With_Flags {
        ਰਸਤਾ_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        ਰਸਤਾ_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} ਰਸਤਾ_ਸ਼ੁਰੂ_With_Flags;

ਅੱਖਰ* ਰਸਤਾ_startswith_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਰਸਤਾ_ਸ਼ੁਰੂ_With_Flags flags) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* ਰਸਤਾ_startswith(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ) {
        ਵਾਪਸ ਰਸਤਾ_startswith_ਭਰਿਆ(ਰਸਤਾ, ਅਗੇਤਰ, 0);
}

ਪੂਰਨ ਰਸਤਾ_compare(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਰਸਤਾ_equal(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        ਵਾਪਸ ਰਸਤਾ_compare(a, b) == 0;
}

ਪੂਰਨ ਰਸਤਾ_compare_filename(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਰਸਤਾ_equal_filename(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        ਵਾਪਸ ਰਸਤਾ_compare_filename(a, b) == 0;
}

ਪੂਰਨ ਰਸਤਾ_equal_or_inode_same_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b, ਪੂਰਨ flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਰਸਤਾ_equal_or_inode_same(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b, ਪੂਰਨ flags) {
        ਵਾਪਸ ਰਸਤਾ_equal_or_inode_same_ਭਰਿਆ(a, b, flags) > 0;
}

ਅੱਖਰ* ਰਸਤਾ_extend_ਅੰਦਰੂਨੀ(ਅੱਖਰ **x, ...);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_extend(x, ...) ਰਸਤਾ_extend_ਅੰਦਰੂਨੀ(x, __VA_ARGS__, ਸੂਚਕ_MAX)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_join(...) ਰਸਤਾ_extend_ਅੰਦਰੂਨੀ(ਨਿਰਾ, __VA_ARGS__, ਸੂਚਕ_MAX)

ਅੱਖਰ* skip_leading_slash(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਰਸਤਾ_Simplify_Flags {
        ਰਸਤਾ_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} ਰਸਤਾ_Simplify_Flags;

ਅੱਖਰ* ਰਸਤਾ_simplify_ਭਰਿਆ(ਅੱਖਰ *ਰਸਤਾ, ਰਸਤਾ_Simplify_Flags flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੱਖਰ* ਰਸਤਾ_simplify(ਅੱਖਰ *ਰਸਤਾ) {
        ਵਾਪਸ ਰਸਤਾ_simplify_ਭਰਿਆ(ਰਸਤਾ, 0);
}

ਪੂਰਨ ਰਸਤਾ_simplify_alloc(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret);

/* Note: the search terminates on the first NULL item. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_ਵਿੱਚ_ਸੈੱਟ(p, ...) ਰਸਤਾ_strv_ਸ਼ਾਮਲ_ਹੈ(STRV_MAKE(__VA_ARGS__), p)

ਅੱਖਰ* ਰਸਤਾ_startswith_strv(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ * ਅਚਲ *strv);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_STARTSWITH_ਸੈੱਟ(p, ...) ਰਸਤਾ_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

ਪੂਰਨ ਰਸਤਾ_strv_make_absolute_cwd(ਅੱਖਰ **l);
ਅੱਖਰ** ਰਸਤਾ_strv_resolve(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ);
ਅੱਖਰ** ਰਸਤਾ_strv_resolve_uniq(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ);

ਪੂਰਨ open_and_check_executable(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd);
ਪੂਰਨ ਲੱਭੋ_executable_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *name,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅੱਖਰ * ਅਚਲ *exec_ਖੋਜ_ਰਸਤਾ,
                bool use_ਰਸਤਾ_envvar,
                ਅੱਖਰ **ret_filename,
                ਪੂਰਨ *ret_fd);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਲੱਭੋ_executable(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret_filename) {
        ਵਾਪਸ ਲੱਭੋ_executable_ਭਰਿਆ(name, /* root= */ ਨਿਰਾ, ਨਿਰਾ, true, ret_filename, ਨਿਰਾ);
}

ਪੂਰਨ fsck_exists(ਖਾਲੀ);
ਪੂਰਨ fsck_exists_for_fstype(ਅਚਲ ਅੱਖਰ *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_FOREACH_ਅਗੇਤਰ(ਅਗੇਤਰ, ਰਸਤਾ)                               \
        ਲਈ (ਅੱਖਰ *_slash = ({                                          \
                                ਰਸਤਾ_simplify(strcpy(ਅਗੇਤਰ, ਰਸਤਾ));    \
                                streq(ਅਗੇਤਰ, "/") ? ਨਿਰਾ : strrchr(ਅਗੇਤਰ, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((ਅਗੇਤਰ), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਰਸਤਾ_FOREACH_ਅਗੇਤਰ_ਹੋਰ(ਅਗੇਤਰ, ਰਸਤਾ)                          \
        ਲਈ (ਅੱਖਰ *_slash = ({                                          \
                                ਰਸਤਾ_simplify(strcpy(ਅਗੇਤਰ, ਰਸਤਾ));    \
                                ਜੇ (streq(ਅਗੇਤਰ, "/"))                 \
                                        ਅਗੇਤਰ[0] = 0;                  \
                                strrchr(ਅਗੇਤਰ, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((ਅਗੇਤਰ), '/'))

ਪੂਰਨ ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(ਅਚਲ ਅੱਖਰ **p, bool accept_dot_dot, ਅਚਲ ਅੱਖਰ **ret);
ਪੂਰਨ ਰਸਤਾ_ਲੱਭੋ_ਆਖਰੀ_component(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool accept_dot_dot, ਅਚਲ ਅੱਖਰ **ਅਗਲਾ_2, ਅਚਲ ਅੱਖਰ **ret);
ਅਚਲ ਅੱਖਰ* ਆਖਰੀ_ਰਸਤਾ_component(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ ਰਸਤਾ_ਵੰਡੋ_ਅਗੇਤਰ_filename(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_dir, ਅੱਖਰ **ret_filename);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਰਸਤਾ_ਕੱਢੋ_filename(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        ਵਾਪਸ ਰਸਤਾ_ਵੰਡੋ_ਅਗੇਤਰ_filename(ਰਸਤਾ, ਨਿਰਾ, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਰਸਤਾ_ਕੱਢੋ_directory(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        ਪੂਰਨ r = ਰਸਤਾ_ਵੰਡੋ_ਅਗੇਤਰ_filename(ਰਸਤਾ, ret, ਨਿਰਾ);
        ਵਾਪਸ r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;
bool filename_is_valid(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;
bool ਰਸਤਾ_is_valid_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *p, bool accept_dot_dot) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਰਸਤਾ_is_valid(ਅਚਲ ਅੱਖਰ *p) {
        ਵਾਪਸ ਰਸਤਾ_is_valid_ਭਰਿਆ(p, /* accept_dot_dot= */ true);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਰਸਤਾ_is_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *p) {
        ਵਾਪਸ ਰਸਤਾ_is_valid_ਭਰਿਆ(p, /* accept_dot_dot= */ false);
}
bool ਰਸਤਾ_is_normalized(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool filename_or_absolute_ਰਸਤਾ_is_valid(ਅਚਲ ਅੱਖਰ *p) {
        ਜੇ (ਰਸਤਾ_is_absolute(p))
                ਵਾਪਸ ਰਸਤਾ_is_valid(p);

        ਵਾਪਸ filename_is_valid(p);
}

ਪੂਰਨ file_ਵਿੱਚ_same_dir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *filename, ਅੱਖਰ **ret);

bool hidden_or_backup_file(ਅਚਲ ਅੱਖਰ *filename) ਸ਼ੁੱਧ;

bool is_device_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) ਸ਼ੁੱਧ;

bool valid_device_ਗੰਢ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) ਸ਼ੁੱਧ;
bool valid_device_allow_ਨਮੂਨਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) ਸ਼ੁੱਧ;

bool dot_or_dot_dot(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) ਸ਼ੁੱਧ;

bool ਰਸਤਾ_ਸਿੱਟਾ_ਦਿੰਦਾ_directory(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) ਸ਼ੁੱਧ;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* skip_dev_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *p) {
        ਅਚਲ ਅੱਖਰ *e;

        /* Drop any /dev prefix if there is any */

        e = ਰਸਤਾ_startswith(p, "/dev/");

        ਵਾਪਸ e ?: p;
}

bool ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) ਸ਼ੁੱਧ;
ਅਚਲ ਅੱਖਰ* ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) ਸ਼ੁੱਧ;
ਪੂਰਨ ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ_harder_ਮੰਜ਼ਿਲ_null(ਅਚਲ ਅੱਖਰ **ਰਸਤਾ);

bool ਰਸਤਾ_strv_ਸ਼ਾਮਲ_ਹੈ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);
bool prefixed_ਰਸਤਾ_strv_ਸ਼ਾਮਲ_ਹੈ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ ਰਸਤਾ_glob_can_ਮੇਲ(ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅੱਖਰ **ret);
