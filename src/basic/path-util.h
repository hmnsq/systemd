/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

#وصف_ڪريو رستو_ورهايو_BIN(x) x "sbin:" x "bin"
#وصف_ڪريو رستو_ورهايو_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#وصف_ڪريو رستو_MERGED_BIN(x) x "bin"
#وصف_ڪريو رستو_MERGED_BIN_NULSTR(x) x "bin\0"

#وصف_ڪريو DEFAULT_رستو_WITH_ڀريل_SBIN رستو_ورهايو_BIN("/usr/local/") ":" رستو_ورهايو_BIN("/usr/") EXTRA_EXEC_ڳولا_رستو_SUFFIX
#وصف_ڪريو DEFAULT_رستو_WITH_LOCAL_SBIN رستو_ورهايو_BIN("/usr/local/") ":" رستو_MERGED_BIN("/usr/") EXTRA_EXEC_ڳولا_رستو_SUFFIX
#وصف_ڪريو DEFAULT_رستو_WITHOUT_SBIN رستو_MERGED_BIN("/usr/local/") ":" رستو_MERGED_BIN("/usr/") EXTRA_EXEC_ڳولا_رستو_SUFFIX

#وصف_ڪريو DEFAULT_رستو_COMPAT رستو_ورهايو_BIN("/usr/local/") ":" رستو_ورهايو_BIN("/usr/") ":" رستو_ورهايو_BIN("/") EXTRA_EXEC_ڳولا_رستو_SUFFIX

ثابت حرف* default_رستو(فراغ);

ساكن مضمن ثابت حرف* default_user_رستو(فراغ) {
#جيڪڏهن_وصف_ٿيل DEFAULT_USER_رستو
        أرجع DEFAULT_USER_رستو EXTRA_EXEC_ڳولا_رستو_SUFFIX;
#وإلا
        أرجع default_رستو();
#جيڪڏهن_پورو
}

bool is_رستو(ثابت حرف *p) خالص;

ساكن مضمن bool رستو_is_absolute(ثابت حرف *p) {
        إذا (!p) /* A NULL pointer is definitely not an absolute path */
                أرجع false;

        أرجع p[0] == '/';
}

صحيح رستو_ورهايو_and_make_absolute(ثابت حرف *p, حرف ***ret);
حرف* رستو_make_absolute(ثابت حرف *p, ثابت حرف *اڳياڙي);
صحيح محفوظ_getcwd(حرف **ret);
صحيح رستو_make_absolute_cwd(ثابت حرف *p, حرف **ret);
صحيح رستو_make_relative(ثابت حرف *from, ثابت حرف *to, حرف **ret);
صحيح رستو_make_relative_والد(ثابت حرف *from_ٻار, ثابت حرف *to, حرف **ret);

تعريفنوع تعداد رستو_شروعات_With_Flags {
        رستو_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        رستو_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} رستو_شروعات_With_Flags;

حرف* رستو_startswith_ڀريل(ثابت حرف *رستو, ثابت حرف *اڳياڙي, رستو_شروعات_With_Flags flags) خالص;
ساكن مضمن حرف* رستو_startswith(ثابت حرف *رستو, ثابت حرف *اڳياڙي) {
        أرجع رستو_startswith_ڀريل(رستو, اڳياڙي, 0);
}

صحيح رستو_compare(ثابت حرف *a, ثابت حرف *b) خالص;
ساكن مضمن bool رستو_equal(ثابت حرف *a, ثابت حرف *b) {
        أرجع رستو_compare(a, b) == 0;
}

صحيح رستو_compare_filename(ثابت حرف *a, ثابت حرف *b);
ساكن مضمن bool رستو_equal_filename(ثابت حرف *a, ثابت حرف *b) {
        أرجع رستو_compare_filename(a, b) == 0;
}

صحيح رستو_equal_or_inode_same_ڀريل(ثابت حرف *a, ثابت حرف *b, صحيح flags);
ساكن مضمن bool رستو_equal_or_inode_same(ثابت حرف *a, ثابت حرف *b, صحيح flags) {
        أرجع رستو_equal_or_inode_same_ڀريل(a, b, flags) > 0;
}

حرف* رستو_extend_اندروني(حرف **x, ...);
#وصف_ڪريو رستو_extend(x, ...) رستو_extend_اندروني(x, __VA_ARGS__, اشارو_MAX)
#وصف_ڪريو رستو_join(...) رستو_extend_اندروني(عدم, __VA_ARGS__, اشارو_MAX)

حرف* skip_leading_slash(ثابت حرف *p) خالص;

تعريفنوع تعداد رستو_Simplify_Flags {
        رستو_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} رستو_Simplify_Flags;

حرف* رستو_simplify_ڀريل(حرف *رستو, رستو_Simplify_Flags flags);
ساكن مضمن حرف* رستو_simplify(حرف *رستو) {
        أرجع رستو_simplify_ڀريل(رستو, 0);
}

صحيح رستو_simplify_alloc(ثابت حرف *رستو, حرف **ret);

/* Note: the search terminates on the first NULL item. */
#وصف_ڪريو رستو_اندريون_سيٽ(p, ...) رستو_strv_شامل_آهي(STRV_MAKE(__VA_ARGS__), p)

حرف* رستو_startswith_strv(ثابت حرف *p, حرف * ثابت *strv);
#وصف_ڪريو رستو_STARTSWITH_سيٽ(p, ...) رستو_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

صحيح رستو_strv_make_absolute_cwd(حرف **l);
حرف** رستو_strv_resolve(حرف **l, ثابت حرف *پاڙ);
حرف** رستو_strv_resolve_uniq(حرف **l, ثابت حرف *پاڙ);

صحيح open_and_check_executable(ثابت حرف *name, ثابت حرف *پاڙ, حرف **ret_رستو, صحيح *ret_fd);
صحيح ڳوليو_executable_ڀريل(
                ثابت حرف *name,
                ثابت حرف *پاڙ,
                حرف * ثابت *exec_ڳولا_رستو,
                bool use_رستو_envvar,
                حرف **ret_filename,
                صحيح *ret_fd);
ساكن مضمن صحيح ڳوليو_executable(ثابت حرف *name, حرف **ret_filename) {
        أرجع ڳوليو_executable_ڀريل(name, /* root= */ عدم, عدم, true, ret_filename, عدم);
}

صحيح fsck_exists(فراغ);
صحيح fsck_exists_for_fstype(ثابت حرف *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#وصف_ڪريو رستو_FOREACH_اڳياڙي(اڳياڙي, رستو)                               \
        لكل (حرف *_slash = ({                                          \
                                رستو_simplify(strcpy(اڳياڙي, رستو));    \
                                streq(اڳياڙي, "/") ? عدم : strrchr(اڳياڙي, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((اڳياڙي), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#وصف_ڪريو رستو_FOREACH_اڳياڙي_وڌيڪ(اڳياڙي, رستو)                          \
        لكل (حرف *_slash = ({                                          \
                                رستو_simplify(strcpy(اڳياڙي, رستو));    \
                                إذا (streq(اڳياڙي, "/"))                 \
                                        اڳياڙي[0] = 0;                  \
                                strrchr(اڳياڙي, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((اڳياڙي), '/'))

صحيح رستو_ڳوليو_پهريون_component(ثابت حرف **p, bool accept_dot_dot, ثابت حرف **ret);
صحيح رستو_ڳوليو_آخري_component(ثابت حرف *رستو, bool accept_dot_dot, ثابت حرف **ايندڙ_2, ثابت حرف **ret);
ثابت حرف* آخري_رستو_component(ثابت حرف *رستو);

صحيح رستو_ورهايو_اڳياڙي_filename(ثابت حرف *رستو, حرف **ret_dir, حرف **ret_filename);
ساكن مضمن صحيح رستو_ڪڍو_filename(ثابت حرف *رستو, حرف **ret) {
        أرجع رستو_ورهايو_اڳياڙي_filename(رستو, عدم, ret);
}
ساكن مضمن صحيح رستو_ڪڍو_directory(ثابت حرف *رستو, حرف **ret) {
        صحيح r = رستو_ورهايو_اڳياڙي_filename(رستو, ret, عدم);
        أرجع r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(ثابت حرف *p) خالص;
bool filename_is_valid(ثابت حرف *p) خالص;
bool رستو_is_valid_ڀريل(ثابت حرف *p, bool accept_dot_dot) خالص;
ساكن مضمن bool رستو_is_valid(ثابت حرف *p) {
        أرجع رستو_is_valid_ڀريل(p, /* accept_dot_dot= */ true);
}
ساكن مضمن bool رستو_is_محفوظ(ثابت حرف *p) {
        أرجع رستو_is_valid_ڀريل(p, /* accept_dot_dot= */ false);
}
bool رستو_is_normalized(ثابت حرف *p) خالص;
ساكن مضمن bool filename_or_absolute_رستو_is_valid(ثابت حرف *p) {
        إذا (رستو_is_absolute(p))
                أرجع رستو_is_valid(p);

        أرجع filename_is_valid(p);
}

صحيح file_اندريون_same_dir(ثابت حرف *رستو, ثابت حرف *filename, حرف **ret);

bool hidden_or_backup_file(ثابت حرف *filename) خالص;

bool is_device_رستو(ثابت حرف *رستو) خالص;

bool valid_device_ڳنڍ_رستو(ثابت حرف *رستو) خالص;
bool valid_device_allow_نمونو(ثابت حرف *رستو) خالص;

bool dot_or_dot_dot(ثابت حرف *رستو) خالص;

bool رستو_نتيجو_ڏئي_directory(ثابت حرف *رستو) خالص;

ساكن مضمن ثابت حرف* skip_dev_اڳياڙي(ثابت حرف *p) {
        ثابت حرف *e;

        /* Drop any /dev prefix if there is any */

        e = رستو_startswith(p, "/dev/");

        أرجع e ?: p;
}

bool خالي_جايون_or_پاڙ(ثابت حرف *رستو) خالص;
ثابت حرف* خالي_جايون_منزل_پاڙ(ثابت حرف *رستو) خالص;
صحيح خالي_جايون_or_پاڙ_harder_منزل_null(ثابت حرف **رستو);

bool رستو_strv_شامل_آهي(حرف * ثابت *l, ثابت حرف *رستو);
bool prefixed_رستو_strv_شامل_آهي(حرف * ثابت *l, ثابت حرف *رستو);

صحيح رستو_glob_can_ميلاپ(ثابت حرف *نمونو, ثابت حرف *اڳياڙي, حرف **ret);
