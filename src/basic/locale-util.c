/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <langinfo.h>
#ضمن <stdlib.h>
#ضمن <sys/mman.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "dirent-util.h"
#ضمن "env-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "locale-util.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "path-util.h"
#ضمن "process-util.h"
#ضمن "set.h"
#ضمن "stat-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"

ساكن حرف* normalize_locale(ثابت حرف *name) {
        ثابت حرف *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        إذا (e) {
                _cleanup_free_ حرف *prefix = عدم;

                prefix = strndup(name, e - name);
                إذا (!prefix)
                        أرجع عدم;

                أرجع strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        إذا (e) {
                _cleanup_free_ حرف *prefix = عدم;

                prefix = strndup(name, e - name);
                إذا (!prefix)
                        أرجع عدم;

                أرجع strjoin(prefix, ".UTF-8@", e + 6);
        }

        أرجع strdup(name);
}

#ifdef __GLIBC__
ساكن ثابت حرف* get_locale_dir(فراغ) {
        أرجع secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

ساكن صحيح add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        بنية locarhead {
                uint32_t magic;
                /* Serial number.  */
                uint32_t serial;
                /* Name hash table.  */
                uint32_t namehash_offset;
                uint32_t namehash_used;
                uint32_t namehash_size;
                /* String table.  */
                uint32_t string_offset;
                uint32_t string_used;
                uint32_t string_size;
                /* Table with locale records.  */
                uint32_t locrectab_offset;
                uint32_t locrectab_used;
                uint32_t locrectab_size;
                /* MD5 sum hash table.  */
                uint32_t sumhash_offset;
                uint32_t sumhash_used;
                uint32_t sumhash_size;
        };

        بنية namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        صحيح r;

        أكد(locales);

        _cleanup_free_ حرف *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        إذا (!locale_archive_file)
                أرجع -ENOMEM;

        _cleanup_close_ صحيح fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع errno == ENOENT ? 0 : -errno;

        بنية stat st;
        إذا (fstat(fd, &st) < 0)
                أرجع -errno;

        r = stat_verify_regular(&st);
        إذا (r < 0)
                أرجع r;

        إذا (st.st_size < (off_t) حجم(بنية locarhead))
                أرجع -EBADMSG;

        إذا (file_offset_beyond_memory_size(st.st_size))
                أرجع -EFBIG;

        فراغ *p = mmap(عدم, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        إذا (p == MAP_FAILED)
                أرجع -errno;

        ثابت بنية namehashent *e;
        ثابت بنية locarhead *h = p;
        إذا (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                اذهب finish;
        }

        e = (ثابت بنية namehashent*) ((ثابت uint8_t*) p + h->namehash_offset);
        لكل (size_t i = 0; i < h->namehash_size; i++) {
                حرف *z;

                إذا (e[i].locrec_offset == 0)
                        استمر;

                إذا (!utf8_is_valid((حرف*) p + e[i].name_offset))
                        استمر;

                z = normalize_locale((حرف*) p + e[i].name_offset);
                إذا (!z) {
                        r = -ENOMEM;
                        اذهب finish;
                }

                r = set_consume(locales, z);
                إذا (r < 0)
                        اذهب finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        أرجع r;
}

ساكن صحيح add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = عدم;
        صحيح r;

        أكد(locales);

        dir = opendir(get_locale_dir());
        إذا (!dir)
                أرجع errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, أرجع -errno) {
                حرف *z;

                إذا (de->d_type != DT_DIR)
                        استمر;

                z = normalize_locale(de->d_name);
                إذا (!z)
                        أرجع -ENOMEM;

                r = set_consume(locales, z);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

#وإلا

ساكن ثابت حرف* ثابت musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        عدم
};

ساكن صحيح add_locales_for_musl(Set *locales) {
        صحيح r;

        أكد(locales);

        ثابت حرف *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                إذا (!dir)
                        استمر;

                FOREACH_DIRENT(de, dir, أرجع -errno) {
                        إذا (de->d_type != DT_REG)
                                استمر;

                        حرف *z = normalize_locale(de->d_name);
                        إذا (!z)
                                أرجع -ENOMEM;

                        r = set_consume(locales, z);
                        إذا (r < 0)
                                أرجع r;
                }
        }

        أرجع 0;
}
#endif

صحيح get_locales(حرف ***ret) {
        _cleanup_set_free_ Set *locales = عدم;
        صحيح r;

        أكد(ret);

        locales = set_new(&string_hash_ops_free);
        إذا (!locales)
                أرجع -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        إذا (r < 0 && r != -ENOENT)
                أرجع r;

        r = add_locales_from_libdir(locales);
        إذا (r < 0)
                أرجع r;
#وإلا
        r = add_locales_for_musl(locales);
        إذا (r < 0)
                أرجع r;
#endif

        حرف *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        حرر(set_remove(locales, locale));
        }

        _cleanup_strv_free_ حرف **l = set_to_strv(&locales);
        إذا (!l)
                أرجع -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        إذا (r <= 0) {
                إذا (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                حرف **b = l;
                STRV_FOREACH(a, l)
                        إذا (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        وإلا
                                حرر(*a);

                *b = عدم;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        أرجع 0;
}

bool locale_is_valid(ثابت حرف *name) {

        إذا (isempty(name))
                أرجع false;

        إذا (strlen(name) >= 128)
                أرجع false;

        إذا (!utf8_is_valid(name))
                أرجع false;

        إذا (!filename_is_valid(name))
                أرجع false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        إذا (!in_charset(name, ALPHANUMERICAL "_.-@"))
                أرجع false;

        أرجع true;
}

صحيح locale_is_installed(ثابت حرف *name) {
        إذا (!locale_is_valid(name))
                أرجع false;

        إذا (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                أرجع true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        إذا (loc == (locale_t) 0)
                أرجع errno == ENOMEM ? -ENOMEM : false;

        أرجع true;
#وإلا
        /* musl also has C.UTF-8 as builtin */
        إذا (streq(name, "C.UTF-8"))
                أرجع true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ثابت حرف *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ حرف *p = path_join(*d, name);
                إذا (!p)
                        أرجع -ENOMEM;

                إذا (access(p, F_OK) >= 0)
                        أرجع true;
        }

        أرجع false;
#endif
}

ساكن bool is_locale_utf8_impl(فراغ) {
        ثابت حرف *set;
        صحيح r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        إذا (r >= 0)
                أرجع r;
        إذا (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        إذا (!is_main_thread())
                أرجع true;

        إذا (!setlocale(LC_ALL, ""))
                أرجع true;

        set = nl_langinfo(CODESET);
        إذا (!set || streq(set, "UTF-8"))
                أرجع true;

        set = setlocale(LC_CTYPE, عدم);
        إذا (!set)
                أرجع true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        أرجع STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(فراغ) {
        ساكن صحيح cached = -1;

        إذا (cached < 0)
                cached = is_locale_utf8_impl();

        أرجع cached;
}

فراغ locale_variables_free(حرف *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

فراغ locale_variables_simplify(حرف *l[_VARIABLE_LC_MAX]) {
        أكد(l);

        لكل (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                إذا (p == VARIABLE_LANG)
                        استمر;
                إذا (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

ساكن ثابت حرف * ثابت locale_variable_table[_VARIABLE_LC_MAX] = {
        [VARIABLE_LANG]              = "LANG",
        [VARIABLE_LANGUAGE]          = "LANGUAGE",
        [VARIABLE_LC_CTYPE]          = "LC_CTYPE",
        [VARIABLE_LC_NUMERIC]        = "LC_NUMERIC",
        [VARIABLE_LC_TIME]           = "LC_TIME",
        [VARIABLE_LC_COLLATE]        = "LC_COLLATE",
        [VARIABLE_LC_MONETARY]       = "LC_MONETARY",
        [VARIABLE_LC_MESSAGES]       = "LC_MESSAGES",
        [VARIABLE_LC_PAPER]          = "LC_PAPER",
        [VARIABLE_LC_NAME]           = "LC_NAME",
        [VARIABLE_LC_ADDRESS]        = "LC_ADDRESS",
        [VARIABLE_LC_TELEPHONE]      = "LC_TELEPHONE",
        [VARIABLE_LC_MEASUREMENT]    = "LC_MEASUREMENT",
        [VARIABLE_LC_IDENTIFICATION] = "LC_IDENTIFICATION"
};

DEFINE_STRING_TABLE_LOOKUP(locale_variable, LocaleVariable);
