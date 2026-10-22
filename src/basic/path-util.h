/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

#تعریف_کړه لار_وېشل_BIN(x) x "sbin:" x "bin"
#تعریف_کړه لار_وېشل_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#تعریف_کړه لار_MERGED_BIN(x) x "bin"
#تعریف_کړه لار_MERGED_BIN_NULSTR(x) x "bin\0"

#تعریف_کړه DEFAULT_لار_WITH_ډک_SBIN لار_وېشل_BIN("/usr/local/") ":" لار_وېشل_BIN("/usr/") EXTRA_EXEC_پلټنه_لار_SUFFIX
#تعریف_کړه DEFAULT_لار_WITH_LOCAL_SBIN لار_وېشل_BIN("/usr/local/") ":" لار_MERGED_BIN("/usr/") EXTRA_EXEC_پلټنه_لار_SUFFIX
#تعریف_کړه DEFAULT_لار_WITHOUT_SBIN لار_MERGED_BIN("/usr/local/") ":" لار_MERGED_BIN("/usr/") EXTRA_EXEC_پلټنه_لار_SUFFIX

#تعریف_کړه DEFAULT_لار_COMPAT لار_وېشل_BIN("/usr/local/") ":" لار_وېشل_BIN("/usr/") ":" لار_وېشل_BIN("/") EXTRA_EXEC_پلټنه_لار_SUFFIX

ثابت حرف* default_لار(فراغ);

ساكن مضمن ثابت حرف* default_user_لار(فراغ) {
#که_تعریف_شوی DEFAULT_USER_لار
        أرجع DEFAULT_USER_لار EXTRA_EXEC_پلټنه_لار_SUFFIX;
#وإلا
        أرجع default_لار();
#د_که_پای
}

bool is_لار(ثابت حرف *p) خالص;

ساكن مضمن bool لار_is_absolute(ثابت حرف *p) {
        إذا (!p) /* A NULL pointer is definitely not an absolute path */
                أرجع false;

        أرجع p[0] == '/';
}

صحيح لار_وېشل_and_make_absolute(ثابت حرف *p, حرف ***ret);
حرف* لار_make_absolute(ثابت حرف *p, ثابت حرف *مختاړی);
صحيح خوندي_getcwd(حرف **ret);
صحيح لار_make_absolute_cwd(ثابت حرف *p, حرف **ret);
صحيح لار_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret);
صحيح لار_make_relative_پلار(ثابت حرف *from_ماشوم, ثابت حرف *to, حرف **ret);

تعريفنوع تعداد لار_پیل_With_Flags {
        لار_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        لار_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} لار_پیل_With_Flags;

حرف* لار_startswith_ډک(ثابت حرف *لار, ثابت حرف *مختاړی, لار_پیل_With_Flags flags) خالص;
ساكن مضمن حرف* لار_startswith(ثابت حرف *لار, ثابت حرف *مختاړی) {
        أرجع لار_startswith_ډک(لار, مختاړی, 0);
}

صحيح لار_compare(ثابت حرف *a, ثابت حرف *b) خالص;
ساكن مضمن bool لار_equal(ثابت حرف *a, ثابت حرف *b) {
        أرجع لار_compare(a, b) == 0;
}

صحيح لار_compare_filename(ثابت حرف *a, ثابت حرف *b);
ساكن مضمن bool لار_equal_filename(ثابت حرف *a, ثابت حرف *b) {
        أرجع لار_compare_filename(a, b) == 0;
}

صحيح لار_equal_or_inode_same_ډک(ثابت حرف *a, ثابت حرف *b, صحيح flags);
ساكن مضمن bool لار_equal_or_inode_same(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        أرجع لار_equal_or_inode_same_ډک(a, b, flags) > 0;
}

حرف* لار_extend_دننی(حرف **x, ...);
#تعریف_کړه لار_extend(x, ...) لار_extend_دننی(x, __VA_ARGS__, اشاره_ګر_MAX)
#تعریف_کړه لار_join(...) لار_extend_دننی(عدم, __VA_ARGS__, اشاره_ګر_MAX)

حرف* skip_leading_slash(ثابت حرف *p) خالص;

تعريفنوع تعداد لار_Simplify_Flags {
        لار_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} لار_Simplify_Flags;

حرف* لار_simplify_ډک(حرف *لار, لار_Simplify_Flags flags);
ساكن مضمن حرف* لار_simplify(حرف *لار) {
        أرجع لار_simplify_ډک(لار, 0);
}

صحيح لار_simplify_alloc(ثابت حرف *لار, حرف **ret);

/* Note: the search terminates on the first NULL item. */
#تعریف_کړه لار_دننه_سېټ(p, ...) لار_strv_لري(STRV_MAKE(__VA_ARGS__), p)

حرف* لار_startswith_strv(ثابت حرف *p, حرف * ثابت *strv);
#تعریف_کړه لار_STARTSWITH_سېټ(p, ...) لار_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

صحيح لار_strv_make_absolute_cwd(حرف **l);
حرف** لار_strv_resolve(حرف **l, ثابت حرف *ریښه);
حرف** لار_strv_resolve_uniq(حرف **l, ثابت حرف *ریښه);

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *ریښه, حرف **ret_لار, صحيح *ret_fd);
صحيح پیدا_کړه_executable_ډک(
                ثابت حرف *name,
                ثابت حرف *ریښه,
                حرف * ثابت *exec_پلټنه_لار,
                bool use_لار_envvar,
                حرف **ret_filename,
                صحيح *ret_fd);
ساكن مضمن صحيح پیدا_کړه_executable(ثابت حرف *name, حرف **ret_filename) {
        أرجع پیدا_کړه_executable_ډک(name, /* root= */ عدم, عدم, true, ret_filename, عدم);
}

صحيح fsck_exists(فراغ);
صحيح fsck_exists_for_fstype(ثابت حرف *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#تعریف_کړه لار_FOREACH_مختاړی(مختاړی, لار)                               \
        لكل (حرف *_slash = ({                                          \
                                لار_simplify(strcpy(مختاړی, لار));    \
                                streq(مختاړی, "/") ? عدم : strrchr(مختاړی, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((مختاړی), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#تعریف_کړه لار_FOREACH_مختاړی_نور(مختاړی, لار)                          \
        لكل (حرف *_slash = ({                                          \
                                لار_simplify(strcpy(مختاړی, لار));    \
                                إذا (streq(مختاړی, "/"))                 \
                                        مختاړی[0] = 0;                  \
                                strrchr(مختاړی, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((مختاړی), '/'))

صحيح لار_پیدا_کړه_لومړی_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret);
صحيح لار_پیدا_کړه_وروستی_component(ثابت حرف *لار, bool accept_dot_dot, ثابت حرف **بل_2, ثابت حرف **ret);
ثابت حرف* وروستی_لار_component(ثابت حرف *لار);

صحيح لار_وېشل_مختاړی_filename(ثابت حرف *لار, حرف **ret_dir, حرف **ret_filename);
ساكن مضمن صحيح لار_ایستل_filename(ثابت حرف *لار, حرف **ret) {
        أرجع لار_وېشل_مختاړی_filename(لار, عدم, ret);
}
ساكن مضمن صحيح لار_ایستل_directory(ثابت حرف *لار, حرف **ret) {
        صحيح r = لار_وېشل_مختاړی_filename(لار, ret, عدم);
        أرجع r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ثابت حرف *p) خالص;
bool filename_is_valid(ثابت حرف *p) خالص;
bool لار_is_valid_ډک(ثابت حرف *p, bool accept_dot_dot) خالص;
ساكن مضمن bool لار_is_valid(ثابت حرف *p) {
        أرجع لار_is_valid_ډک(p, /* accept_dot_dot= */ true);
}
ساكن مضمن bool لار_is_خوندي(ثابت حرف *p) {
        أرجع لار_is_valid_ډک(p, /* accept_dot_dot= */ false);
}
bool لار_is_normalized(ثابت حرف *p) خالص;
ساكن مضمن bool filename_or_absolute_لار_is_valid(ثابت حرف *p) {
        إذا (لار_is_absolute(p))
                أرجع لار_is_valid(p);

        أرجع filename_is_valid(p);
}

صحيح file_دننه_same_dir(ثابت حرف *لار, ثابت حرف *filename, حرف **ret);

bool hidden_or_backup_file(ثابت حرف *filename) خالص;

bool is_device_لار(ثابت حرف *لار) خالص;

bool valid_device_غوټه_لار(ثابت حرف *لار) خالص;
bool valid_device_allow_نمونه(ثابت حرف *لار) خالص;

bool dot_or_dot_dot(ثابت حرف *لار) خالص;

bool لار_پایله_ورکوي_directory(ثابت حرف *لار) خالص;

ساكن مضمن ثابت حرف* skip_dev_مختاړی(ثابت حرف *p) {
        ثابت حرف *e;

        /* Drop any /dev prefix if there is any */

        e = لار_startswith(p, "/dev/");

        أرجع e ?: p;
}

bool تشه_or_ریښه(ثابت حرف *لار) خالص;
ثابت حرف* تشه_منزل_ریښه(ثابت حرف *لار) خالص;
صحيح تشه_or_ریښه_harder_منزل_null(ثابت حرف **لار);

bool لار_strv_لري(حرف * ثابت *l, ثابت حرف *لار);
bool prefixed_لار_strv_لري(حرف * ثابت *l, ثابت حرف *لار);

صحيح لار_glob_can_سمون(ثابت حرف *نمونه, ثابت حرف *مختاړی, حرف **ret);
