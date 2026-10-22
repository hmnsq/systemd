/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#تعریف_کرو راستہ_بانٹو_BIN(x) x "sbin:" x "bin"
#تعریف_کرو راستہ_بانٹو_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#تعریف_کرو راستہ_MERGED_BIN(x) x "bin"
#تعریف_کرو راستہ_MERGED_BIN_NULSTR(x) x "bin\0"

#تعریف_کرو DEFAULT_راستہ_WITH_بھرا_SBIN راستہ_بانٹو_BIN("/usr/local/") ":" راستہ_بانٹو_BIN("/usr/") EXTRA_EXEC_تلاش_راستہ_SUFFIX
#تعریف_کرو DEFAULT_راستہ_WITH_LOCAL_SBIN راستہ_بانٹو_BIN("/usr/local/") ":" راستہ_MERGED_BIN("/usr/") EXTRA_EXEC_تلاش_راستہ_SUFFIX
#تعریف_کرو DEFAULT_راستہ_WITHOUT_SBIN راستہ_MERGED_BIN("/usr/local/") ":" راستہ_MERGED_BIN("/usr/") EXTRA_EXEC_تلاش_راستہ_SUFFIX

#تعریف_کرو DEFAULT_راستہ_COMPAT راستہ_بانٹو_BIN("/usr/local/") ":" راستہ_بانٹو_BIN("/usr/") ":" راستہ_بانٹو_BIN("/") EXTRA_EXEC_تلاش_راستہ_SUFFIX

ثابت حرف* default_راستہ(خالی);

ساکن اندرونی ثابت حرف* default_user_راستہ(خالی) {
#اگر_تعریف_شدہ DEFAULT_USER_راستہ
        واپس DEFAULT_USER_راستہ EXTRA_EXEC_تلاش_راستہ_SUFFIX;
#ورنہ
        واپس default_راستہ();
#اگر_ختم
}

bool is_راستہ(ثابت حرف *p) خالص;

ساکن اندرونی bool راستہ_is_absolute(ثابت حرف *p) {
        اگر (!p) /* A NULL pointer is definitely not an absolute path */
                واپس false;

        واپس p[0] == '/';
}

صحیح راستہ_بانٹو_and_make_absolute(ثابت حرف *p, حرف ***ret);
حرف* راستہ_make_absolute(ثابت حرف *p, ثابت حرف *سابقہ);
صحیح محفوظ_getcwd(حرف **ret);
صحیح راستہ_make_absolute_cwd(ثابت حرف *p, حرف **ret);
صحیح راستہ_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret);
صحیح راستہ_make_relative_والد(ثابت حرف *from_بچہ, ثابت حرف *to, حرف **ret);

قسمتعریف شمار راستہ_آغاز_With_Flags {
        راستہ_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        راستہ_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} راستہ_آغاز_With_Flags;

حرف* راستہ_startswith_بھرا(ثابت حرف *راستہ, ثابت حرف *سابقہ, راستہ_آغاز_With_Flags flags) خالص;
ساکن اندرونی حرف* راستہ_startswith(ثابت حرف *راستہ, ثابت حرف *سابقہ) {
        واپس راستہ_startswith_بھرا(راستہ, سابقہ, 0);
}

صحیح راستہ_compare(ثابت حرف *a, ثابت حرف *b) خالص;
ساکن اندرونی bool راستہ_equal(ثابت حرف *a, ثابت حرف *b) {
        واپس راستہ_compare(a, b) == 0;
}

صحیح راستہ_compare_filename(ثابت حرف *a, ثابت حرف *b);
ساکن اندرونی bool راستہ_equal_filename(ثابت حرف *a, ثابت حرف *b) {
        واپس راستہ_compare_filename(a, b) == 0;
}

صحیح راستہ_equal_or_inode_same_بھرا(ثابت حرف *a, ثابت حرف *b, صحیح flags);
ساکن اندرونی bool راستہ_equal_or_inode_same(ثابت حرف *a, ثابت حرف *b, صحیح flags) {
        واپس راستہ_equal_or_inode_same_بھرا(a, b, flags) > 0;
}

حرف* راستہ_extend_اندرونی(حرف **x, ...);
#تعریف_کرو راستہ_extend(x, ...) راستہ_extend_اندرونی(x, __VA_ARGS__, اشارہ_گر_MAX)
#تعریف_کرو راستہ_join(...) راستہ_extend_اندرونی(تہی, __VA_ARGS__, اشارہ_گر_MAX)

حرف* skip_leading_slash(ثابت حرف *p) خالص;

قسمتعریف شمار راستہ_Simplify_Flags {
        راستہ_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} راستہ_Simplify_Flags;

حرف* راستہ_simplify_بھرا(حرف *راستہ, راستہ_Simplify_Flags flags);
ساکن اندرونی حرف* راستہ_simplify(حرف *راستہ) {
        واپس راستہ_simplify_بھرا(راستہ, 0);
}

صحیح راستہ_simplify_alloc(ثابت حرف *راستہ, حرف **ret);

/* Note: the search terminates on the first NULL item. */
#تعریف_کرو راستہ_اندر_سیٹ(p, ...) راستہ_strv_شامل_ہے(STRV_MAKE(__VA_ARGS__), p)

حرف* راستہ_startswith_strv(ثابت حرف *p, حرف * ثابت *strv);
#تعریف_کرو راستہ_STARTSWITH_سیٹ(p, ...) راستہ_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

صحیح راستہ_strv_make_absolute_cwd(حرف **l);
حرف** راستہ_strv_resolve(حرف **l, ثابت حرف *جڑ);
حرف** راستہ_strv_resolve_uniq(حرف **l, ثابت حرف *جڑ);

صحیح open_and_check_executable(ثابت حرف *name, ثابت حرف *جڑ, حرف **ret_راستہ, صحیح *ret_fd);
صحیح ڈھونڈو_executable_بھرا(
                ثابت حرف *name,
                ثابت حرف *جڑ,
                حرف * ثابت *exec_تلاش_راستہ,
                bool use_راستہ_envvar,
                حرف **ret_filename,
                صحیح *ret_fd);
ساکن اندرونی صحیح ڈھونڈو_executable(ثابت حرف *name, حرف **ret_filename) {
        واپس ڈھونڈو_executable_بھرا(name, /* root= */ تہی, تہی, true, ret_filename, تہی);
}

صحیح fsck_exists(خالی);
صحیح fsck_exists_for_fstype(ثابت حرف *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#تعریف_کرو راستہ_FOREACH_سابقہ(سابقہ, راستہ)                               \
        برائے (حرف *_slash = ({                                          \
                                راستہ_simplify(strcpy(سابقہ, راستہ));    \
                                streq(سابقہ, "/") ? تہی : strrchr(سابقہ, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((سابقہ), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#تعریف_کرو راستہ_FOREACH_سابقہ_مزید(سابقہ, راستہ)                          \
        برائے (حرف *_slash = ({                                          \
                                راستہ_simplify(strcpy(سابقہ, راستہ));    \
                                اگر (streq(سابقہ, "/"))                 \
                                        سابقہ[0] = 0;                  \
                                strrchr(سابقہ, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((سابقہ), '/'))

صحیح راستہ_ڈھونڈو_پہلا_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret);
صحیح راستہ_ڈھونڈو_آخری_component(ثابت حرف *راستہ, bool accept_dot_dot, ثابت حرف **اگلا_2, ثابت حرف **ret);
ثابت حرف* آخری_راستہ_component(ثابت حرف *راستہ);

صحیح راستہ_بانٹو_سابقہ_filename(ثابت حرف *راستہ, حرف **ret_dir, حرف **ret_filename);
ساکن اندرونی صحیح راستہ_نکالو_filename(ثابت حرف *راستہ, حرف **ret) {
        واپس راستہ_بانٹو_سابقہ_filename(راستہ, تہی, ret);
}
ساکن اندرونی صحیح راستہ_نکالو_directory(ثابت حرف *راستہ, حرف **ret) {
        صحیح r = راستہ_بانٹو_سابقہ_filename(راستہ, ret, تہی);
        واپس r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ثابت حرف *p) خالص;
bool filename_is_valid(ثابت حرف *p) خالص;
bool راستہ_is_valid_بھرا(ثابت حرف *p, bool accept_dot_dot) خالص;
ساکن اندرونی bool راستہ_is_valid(ثابت حرف *p) {
        واپس راستہ_is_valid_بھرا(p, /* accept_dot_dot= */ true);
}
ساکن اندرونی bool راستہ_is_محفوظ(ثابت حرف *p) {
        واپس راستہ_is_valid_بھرا(p, /* accept_dot_dot= */ false);
}
bool راستہ_is_normalized(ثابت حرف *p) خالص;
ساکن اندرونی bool filename_or_absolute_راستہ_is_valid(ثابت حرف *p) {
        اگر (راستہ_is_absolute(p))
                واپس راستہ_is_valid(p);

        واپس filename_is_valid(p);
}

صحیح file_اندر_same_dir(ثابت حرف *راستہ, ثابت حرف *filename, حرف **ret);

bool hidden_or_backup_file(ثابت حرف *filename) خالص;

bool is_device_راستہ(ثابت حرف *راستہ) خالص;

bool valid_device_گرہ_راستہ(ثابت حرف *راستہ) خالص;
bool valid_device_allow_نمونہ(ثابت حرف *راستہ) خالص;

bool dot_or_dot_dot(ثابت حرف *راستہ) خالص;

bool راستہ_مستلزم_directory(ثابت حرف *راستہ) خالص;

ساکن اندرونی ثابت حرف* skip_dev_سابقہ(ثابت حرف *p) {
        ثابت حرف *e;

        /* Drop any /dev prefix if there is any */

        e = راستہ_startswith(p, "/dev/");

        واپس e ?: p;
}

bool خالی_خانے_or_جڑ(ثابت حرف *راستہ) خالص;
ثابت حرف* خالی_خانے_مقصد_جڑ(ثابت حرف *راستہ) خالص;
صحیح خالی_خانے_or_جڑ_harder_مقصد_null(ثابت حرف **راستہ);

bool راستہ_strv_شامل_ہے(حرف * ثابت *l, ثابت حرف *راستہ);
bool prefixed_راستہ_strv_شامل_ہے(حرف * ثابت *l, ثابت حرف *راستہ);

صحیح راستہ_glob_can_مطابقت(ثابت حرف *نمونہ, ثابت حرف *سابقہ, حرف **ret);
