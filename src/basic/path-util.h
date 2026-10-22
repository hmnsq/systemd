/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

#تعریف_کرو رستہ_ونڈو_BIN(x) x "sbin:" x "bin"
#تعریف_کرو رستہ_ونڈو_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#تعریف_کرو رستہ_MERGED_BIN(x) x "bin"
#تعریف_کرو رستہ_MERGED_BIN_NULSTR(x) x "bin\0"

#تعریف_کرو DEFAULT_رستہ_WITH_بھریا_SBIN رستہ_ونڈو_BIN("/usr/local/") ":" رستہ_ونڈو_BIN("/usr/") EXTRA_EXEC_تلاش_رستہ_SUFFIX
#تعریف_کرو DEFAULT_رستہ_WITH_LOCAL_SBIN رستہ_ونڈو_BIN("/usr/local/") ":" رستہ_MERGED_BIN("/usr/") EXTRA_EXEC_تلاش_رستہ_SUFFIX
#تعریف_کرو DEFAULT_رستہ_WITHOUT_SBIN رستہ_MERGED_BIN("/usr/local/") ":" رستہ_MERGED_BIN("/usr/") EXTRA_EXEC_تلاش_رستہ_SUFFIX

#تعریف_کرو DEFAULT_رستہ_COMPAT رستہ_ونڈو_BIN("/usr/local/") ":" رستہ_ونڈو_BIN("/usr/") ":" رستہ_ونڈو_BIN("/") EXTRA_EXEC_تلاش_رستہ_SUFFIX

ثابت حرف* default_رستہ(فراغ);

ساكن مضمن ثابت حرف* default_user_رستہ(فراغ) {
#جے_تعریف_شدہ DEFAULT_USER_رستہ
        أرجع DEFAULT_USER_رستہ EXTRA_EXEC_تلاش_رستہ_SUFFIX;
#وإلا
        أرجع default_رستہ();
#جے_انت
}

bool is_رستہ(ثابت حرف *p) خالص;

ساكن مضمن bool رستہ_is_absolute(ثابت حرف *p) {
        إذا (!p) /* A NULL pointer is definitely not an absolute path */
                أرجع false;

        أرجع p[0] == '/';
}

صحيح رستہ_ونڈو_and_make_absolute(ثابت حرف *p, حرف ***ret);
حرف* رستہ_make_absolute(ثابت حرف *p, ثابت حرف *اگیتر);
صحيح محفوظ_getcwd(حرف **ret);
صحيح رستہ_make_absolute_cwd(ثابت حرف *p, حرف **ret);
صحيح رستہ_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret);
صحيح رستہ_make_relative_ماپا(ثابت حرف *from_بچہ, ثابت حرف *to, حرف **ret);

تعريفنوع تعداد رستہ_شروع_With_Flags {
        رستہ_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        رستہ_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} رستہ_شروع_With_Flags;

حرف* رستہ_startswith_بھریا(ثابت حرف *رستہ, ثابت حرف *اگیتر, رستہ_شروع_With_Flags flags) خالص;
ساكن مضمن حرف* رستہ_startswith(ثابت حرف *رستہ, ثابت حرف *اگیتر) {
        أرجع رستہ_startswith_بھریا(رستہ, اگیتر, 0);
}

صحيح رستہ_compare(ثابت حرف *a, ثابت حرف *b) خالص;
ساكن مضمن bool رستہ_equal(ثابت حرف *a, ثابت حرف *b) {
        أرجع رستہ_compare(a, b) == 0;
}

صحيح رستہ_compare_filename(ثابت حرف *a, ثابت حرف *b);
ساكن مضمن bool رستہ_equal_filename(ثابت حرف *a, ثابت حرف *b) {
        أرجع رستہ_compare_filename(a, b) == 0;
}

صحيح رستہ_equal_or_inode_same_بھریا(ثابت حرف *a, ثابت حرف *b, صحيح flags);
ساكن مضمن bool رستہ_equal_or_inode_same(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        أرجع رستہ_equal_or_inode_same_بھریا(a, b, flags) > 0;
}

حرف* رستہ_extend_اندرونی(حرف **x, ...);
#تعریف_کرو رستہ_extend(x, ...) رستہ_extend_اندرونی(x, __VA_ARGS__, اشارہ_گر_MAX)
#تعریف_کرو رستہ_join(...) رستہ_extend_اندرونی(عدم, __VA_ARGS__, اشارہ_گر_MAX)

حرف* skip_leading_slash(ثابت حرف *p) خالص;

تعريفنوع تعداد رستہ_Simplify_Flags {
        رستہ_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} رستہ_Simplify_Flags;

حرف* رستہ_simplify_بھریا(حرف *رستہ, رستہ_Simplify_Flags flags);
ساكن مضمن حرف* رستہ_simplify(حرف *رستہ) {
        أرجع رستہ_simplify_بھریا(رستہ, 0);
}

صحيح رستہ_simplify_alloc(ثابت حرف *رستہ, حرف **ret);

/* Note: the search terminates on the first NULL item. */
#تعریف_کرو رستہ_وچ_سیٹ(p, ...) رستہ_strv_شامل_اے(STRV_MAKE(__VA_ARGS__), p)

حرف* رستہ_startswith_strv(ثابت حرف *p, حرف * ثابت *strv);
#تعریف_کرو رستہ_STARTSWITH_سیٹ(p, ...) رستہ_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

صحيح رستہ_strv_make_absolute_cwd(حرف **l);
حرف** رستہ_strv_resolve(حرف **l, ثابت حرف *جڑ);
حرف** رستہ_strv_resolve_uniq(حرف **l, ثابت حرف *جڑ);

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *جڑ, حرف **ret_رستہ, صحيح *ret_fd);
صحيح لبھو_executable_بھریا(
                ثابت حرف *name,
                ثابت حرف *جڑ,
                حرف * ثابت *exec_تلاش_رستہ,
                bool use_رستہ_envvar,
                حرف **ret_filename,
                صحيح *ret_fd);
ساكن مضمن صحيح لبھو_executable(ثابت حرف *name, حرف **ret_filename) {
        أرجع لبھو_executable_بھریا(name, /* root= */ عدم, عدم, true, ret_filename, عدم);
}

صحيح fsck_exists(فراغ);
صحيح fsck_exists_for_fstype(ثابت حرف *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#تعریف_کرو رستہ_FOREACH_اگیتر(اگیتر, رستہ)                               \
        لكل (حرف *_slash = ({                                          \
                                رستہ_simplify(strcpy(اگیتر, رستہ));    \
                                streq(اگیتر, "/") ? عدم : strrchr(اگیتر, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((اگیتر), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#تعریف_کرو رستہ_FOREACH_اگیتر_ہور(اگیتر, رستہ)                          \
        لكل (حرف *_slash = ({                                          \
                                رستہ_simplify(strcpy(اگیتر, رستہ));    \
                                إذا (streq(اگیتر, "/"))                 \
                                        اگیتر[0] = 0;                  \
                                strrchr(اگیتر, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((اگیتر), '/'))

صحيح رستہ_لبھو_پہلا_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret);
صحيح رستہ_لبھو_آخری_component(ثابت حرف *رستہ, bool accept_dot_dot, ثابت حرف **اگلا_2, ثابت حرف **ret);
ثابت حرف* آخری_رستہ_component(ثابت حرف *رستہ);

صحيح رستہ_ونڈو_اگیتر_filename(ثابت حرف *رستہ, حرف **ret_dir, حرف **ret_filename);
ساكن مضمن صحيح رستہ_کڈھو_filename(ثابت حرف *رستہ, حرف **ret) {
        أرجع رستہ_ونڈو_اگیتر_filename(رستہ, عدم, ret);
}
ساكن مضمن صحيح رستہ_کڈھو_directory(ثابت حرف *رستہ, حرف **ret) {
        صحيح r = رستہ_ونڈو_اگیتر_filename(رستہ, ret, عدم);
        أرجع r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ثابت حرف *p) خالص;
bool filename_is_valid(ثابت حرف *p) خالص;
bool رستہ_is_valid_بھریا(ثابت حرف *p, bool accept_dot_dot) خالص;
ساكن مضمن bool رستہ_is_valid(ثابت حرف *p) {
        أرجع رستہ_is_valid_بھریا(p, /* accept_dot_dot= */ true);
}
ساكن مضمن bool رستہ_is_محفوظ(ثابت حرف *p) {
        أرجع رستہ_is_valid_بھریا(p, /* accept_dot_dot= */ false);
}
bool رستہ_is_normalized(ثابت حرف *p) خالص;
ساكن مضمن bool filename_or_absolute_رستہ_is_valid(ثابت حرف *p) {
        إذا (رستہ_is_absolute(p))
                أرجع رستہ_is_valid(p);

        أرجع filename_is_valid(p);
}

صحيح file_وچ_same_dir(ثابت حرف *رستہ, ثابت حرف *filename, حرف **ret);

bool hidden_or_backup_file(ثابت حرف *filename) خالص;

bool is_device_رستہ(ثابت حرف *رستہ) خالص;

bool valid_device_گرہ_رستہ(ثابت حرف *رستہ) خالص;
bool valid_device_allow_نمونہ(ثابت حرف *رستہ) خالص;

bool dot_or_dot_dot(ثابت حرف *رستہ) خالص;

bool رستہ_سٹا_دیندا_directory(ثابت حرف *رستہ) خالص;

ساكن مضمن ثابت حرف* skip_dev_اگیتر(ثابت حرف *p) {
        ثابت حرف *e;

        /* Drop any /dev prefix if there is any */

        e = رستہ_startswith(p, "/dev/");

        أرجع e ?: p;
}

bool خالی_تھاواں_or_جڑ(ثابت حرف *رستہ) خالص;
ثابت حرف* خالی_تھاواں_منزل_جڑ(ثابت حرف *رستہ) خالص;
صحيح خالی_تھاواں_or_جڑ_harder_منزل_null(ثابت حرف **رستہ);

bool رستہ_strv_شامل_اے(حرف * ثابت *l, ثابت حرف *رستہ);
bool prefixed_رستہ_strv_شامل_اے(حرف * ثابت *l, ثابت حرف *رستہ);

صحيح رستہ_glob_can_میل(ثابت حرف *نمونہ, ثابت حرف *اگیتر, حرف **ret);
