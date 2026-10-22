/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

#تعریف_کن مسیر_تقسیم_BIN(x) x "sbin:" x "bin"
#تعریف_کن مسیر_تقسیم_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#تعریف_کن مسیر_MERGED_BIN(x) x "bin"
#تعریف_کن مسیر_MERGED_BIN_NULSTR(x) x "bin\0"

#تعریف_کن DEFAULT_مسیر_WITH_پر_SBIN مسیر_تقسیم_BIN("/usr/local/") ":" مسیر_تقسیم_BIN("/usr/") EXTRA_EXEC_جستجو_مسیر_SUFFIX
#تعریف_کن DEFAULT_مسیر_WITH_LOCAL_SBIN مسیر_تقسیم_BIN("/usr/local/") ":" مسیر_MERGED_BIN("/usr/") EXTRA_EXEC_جستجو_مسیر_SUFFIX
#تعریف_کن DEFAULT_مسیر_WITHOUT_SBIN مسیر_MERGED_BIN("/usr/local/") ":" مسیر_MERGED_BIN("/usr/") EXTRA_EXEC_جستجو_مسیر_SUFFIX

#تعریف_کن DEFAULT_مسیر_COMPAT مسیر_تقسیم_BIN("/usr/local/") ":" مسیر_تقسیم_BIN("/usr/") ":" مسیر_تقسیم_BIN("/") EXTRA_EXEC_جستجو_مسیر_SUFFIX

ثابت حرف* default_مسیر(فراغ);

ساكن مضمن ثابت حرف* default_user_مسیر(فراغ) {
#اگر_تعریف_شده DEFAULT_USER_مسیر
        أرجع DEFAULT_USER_مسیر EXTRA_EXEC_جستجو_مسیر_SUFFIX;
#وإلا
        أرجع default_مسیر();
#پایان_اگر
}

bool is_مسیر(ثابت حرف *p) خالص;

ساكن مضمن bool مسیر_is_absolute(ثابت حرف *p) {
        إذا (!p) /* A NULL pointer is definitely not an absolute path */
                أرجع false;

        أرجع p[0] == '/';
}

صحيح مسیر_تقسیم_and_make_absolute(ثابت حرف *p, حرف ***ret);
حرف* مسیر_make_absolute(ثابت حرف *p, ثابت حرف *پیشوند);
صحيح امن_getcwd(حرف **ret);
صحيح مسیر_make_absolute_cwd(ثابت حرف *p, حرف **ret);
صحيح مسیر_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret);
صحيح مسیر_make_relative_والد(ثابت حرف *from_فرزند, ثابت حرف *to, حرف **ret);

تعريفنوع تعداد مسیر_شروع_With_Flags {
        مسیر_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        مسیر_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} مسیر_شروع_With_Flags;

حرف* مسیر_startswith_پر(ثابت حرف *مسیر, ثابت حرف *پیشوند, مسیر_شروع_With_Flags flags) خالص;
ساكن مضمن حرف* مسیر_startswith(ثابت حرف *مسیر, ثابت حرف *پیشوند) {
        أرجع مسیر_startswith_پر(مسیر, پیشوند, 0);
}

صحيح مسیر_compare(ثابت حرف *a, ثابت حرف *b) خالص;
ساكن مضمن bool مسیر_equal(ثابت حرف *a, ثابت حرف *b) {
        أرجع مسیر_compare(a, b) == 0;
}

صحيح مسیر_compare_filename(ثابت حرف *a, ثابت حرف *b);
ساكن مضمن bool مسیر_equal_filename(ثابت حرف *a, ثابت حرف *b) {
        أرجع مسیر_compare_filename(a, b) == 0;
}

صحيح مسیر_equal_or_inode_same_پر(ثابت حرف *a, ثابت حرف *b, صحيح flags);
ساكن مضمن bool مسیر_equal_or_inode_same(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        أرجع مسیر_equal_or_inode_same_پر(a, b, flags) > 0;
}

حرف* مسیر_extend_داخلی(حرف **x, ...);
#تعریف_کن مسیر_extend(x, ...) مسیر_extend_داخلی(x, __VA_ARGS__, اشاره_گر_MAX)
#تعریف_کن مسیر_join(...) مسیر_extend_داخلی(عدم, __VA_ARGS__, اشاره_گر_MAX)

حرف* skip_leading_slash(ثابت حرف *p) خالص;

تعريفنوع تعداد مسیر_Simplify_Flags {
        مسیر_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} مسیر_Simplify_Flags;

حرف* مسیر_simplify_پر(حرف *مسیر, مسیر_Simplify_Flags flags);
ساكن مضمن حرف* مسیر_simplify(حرف *مسیر) {
        أرجع مسیر_simplify_پر(مسیر, 0);
}

صحيح مسیر_simplify_alloc(ثابت حرف *مسیر, حرف **ret);

/* Note: the search terminates on the first NULL item. */
#تعریف_کن مسیر_درون_مجموعه(p, ...) مسیر_strv_شامل_است(STRV_MAKE(__VA_ARGS__), p)

حرف* مسیر_startswith_strv(ثابت حرف *p, حرف * ثابت *strv);
#تعریف_کن مسیر_STARTSWITH_مجموعه(p, ...) مسیر_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

صحيح مسیر_strv_make_absolute_cwd(حرف **l);
حرف** مسیر_strv_resolve(حرف **l, ثابت حرف *ریشه);
حرف** مسیر_strv_resolve_uniq(حرف **l, ثابت حرف *ریشه);

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *ریشه, حرف **ret_مسیر, صحيح *ret_fd);
صحيح پیدا_کن_executable_پر(
                ثابت حرف *name,
                ثابت حرف *ریشه,
                حرف * ثابت *exec_جستجو_مسیر,
                bool use_مسیر_envvar,
                حرف **ret_filename,
                صحيح *ret_fd);
ساكن مضمن صحيح پیدا_کن_executable(ثابت حرف *name, حرف **ret_filename) {
        أرجع پیدا_کن_executable_پر(name, /* root= */ عدم, عدم, true, ret_filename, عدم);
}

صحيح fsck_exists(فراغ);
صحيح fsck_exists_for_fstype(ثابت حرف *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#تعریف_کن مسیر_FOREACH_پیشوند(پیشوند, مسیر)                               \
        لكل (حرف *_slash = ({                                          \
                                مسیر_simplify(strcpy(پیشوند, مسیر));    \
                                streq(پیشوند, "/") ? عدم : strrchr(پیشوند, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((پیشوند), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#تعریف_کن مسیر_FOREACH_پیشوند_بیشتر(پیشوند, مسیر)                          \
        لكل (حرف *_slash = ({                                          \
                                مسیر_simplify(strcpy(پیشوند, مسیر));    \
                                إذا (streq(پیشوند, "/"))                 \
                                        پیشوند[0] = 0;                  \
                                strrchr(پیشوند, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((پیشوند), '/'))

صحيح مسیر_پیدا_کن_اولی_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret);
صحيح مسیر_پیدا_کن_آخرین_component(ثابت حرف *مسیر, bool accept_dot_dot, ثابت حرف **بعدی_2, ثابت حرف **ret);
ثابت حرف* آخرین_مسیر_component(ثابت حرف *مسیر);

صحيح مسیر_تقسیم_پیشوند_filename(ثابت حرف *مسیر, حرف **ret_dir, حرف **ret_filename);
ساكن مضمن صحيح مسیر_استخراج_filename(ثابت حرف *مسیر, حرف **ret) {
        أرجع مسیر_تقسیم_پیشوند_filename(مسیر, عدم, ret);
}
ساكن مضمن صحيح مسیر_استخراج_directory(ثابت حرف *مسیر, حرف **ret) {
        صحيح r = مسیر_تقسیم_پیشوند_filename(مسیر, ret, عدم);
        أرجع r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ثابت حرف *p) خالص;
bool filename_is_valid(ثابت حرف *p) خالص;
bool مسیر_is_valid_پر(ثابت حرف *p, bool accept_dot_dot) خالص;
ساكن مضمن bool مسیر_is_valid(ثابت حرف *p) {
        أرجع مسیر_is_valid_پر(p, /* accept_dot_dot= */ true);
}
ساكن مضمن bool مسیر_is_امن(ثابت حرف *p) {
        أرجع مسیر_is_valid_پر(p, /* accept_dot_dot= */ false);
}
bool مسیر_is_normalized(ثابت حرف *p) خالص;
ساكن مضمن bool filename_or_absolute_مسیر_is_valid(ثابت حرف *p) {
        إذا (مسیر_is_absolute(p))
                أرجع مسیر_is_valid(p);

        أرجع filename_is_valid(p);
}

صحيح file_درون_same_dir(ثابت حرف *مسیر, ثابت حرف *filename, حرف **ret);

bool hidden_or_backup_file(ثابت حرف *filename) خالص;

bool is_device_مسیر(ثابت حرف *مسیر) خالص;

bool valid_device_گره_مسیر(ثابت حرف *مسیر) خالص;
bool valid_device_allow_الگو(ثابت حرف *مسیر) خالص;

bool dot_or_dot_dot(ثابت حرف *مسیر) خالص;

bool مسیر_نتیجه_میدهد_directory(ثابت حرف *مسیر) خالص;

ساكن مضمن ثابت حرف* skip_dev_پیشوند(ثابت حرف *p) {
        ثابت حرف *e;

        /* Drop any /dev prefix if there is any */

        e = مسیر_startswith(p, "/dev/");

        أرجع e ?: p;
}

bool خالی_or_ریشه(ثابت حرف *مسیر) خالص;
ثابت حرف* خالی_مقصد_ریشه(ثابت حرف *مسیر) خالص;
صحيح خالی_or_ریشه_harder_مقصد_null(ثابت حرف **مسیر);

bool مسیر_strv_شامل_است(حرف * ثابت *l, ثابت حرف *مسیر);
bool prefixed_مسیر_strv_شامل_است(حرف * ثابت *l, ثابت حرف *مسیر);

صحيح مسیر_glob_can_تطابق(ثابت حرف *الگو, ثابت حرف *پیشوند, حرف **ret);
