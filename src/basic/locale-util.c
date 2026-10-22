/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <langinfo.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/mman.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "locale-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "set.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

ಸ್ಥಿರ ವರ್ಣ* normalize_locale(ಧ್ರುವ ವರ್ಣ *name) {
        ಧ್ರುವ ವರ್ಣ *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        ಯದಿ (e) {
                _cleanup_free_ ವರ್ಣ *prefix = ರಿಕ್ತ;

                prefix = strndup(name, e - name);
                ಯದಿ (!prefix)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                ಪ್ರತ್ಯಾಗಮ strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        ಯದಿ (e) {
                _cleanup_free_ ವರ್ಣ *prefix = ರಿಕ್ತ;

                prefix = strndup(name, e - name);
                ಯದಿ (!prefix)
                        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

                ಪ್ರತ್ಯಾಗಮ strjoin(prefix, ".UTF-8@", e + 6);
        }

        ಪ್ರತ್ಯಾಗಮ strdup(name);
}

#ifdef __GLIBC__
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* get_locale_dir(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        ಸಂರಚನಾ locarhead {
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

        ಸಂರಚನಾ namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(locales);

        _cleanup_free_ ವರ್ಣ *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        ಯದಿ (!locale_archive_file)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ errno == ENOENT ? 0 : -errno;

        ಸಂರಚನಾ stat st;
        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = stat_verify_regular(&st);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (st.st_size < (off_t) ಪರಿಮಾಣ(ಸಂರಚನಾ locarhead))
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (file_offset_beyond_memory_size(st.st_size))
                ಪ್ರತ್ಯಾಗಮ -EFBIG;

        ಶೂನ್ಯ *p = mmap(ರಿಕ್ತ, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        ಯದಿ (p == MAP_FAILED)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಧ್ರುವ ಸಂರಚನಾ namehashent *e;
        ಧ್ರುವ ಸಂರಚನಾ locarhead *h = p;
        ಯದಿ (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                ಗಚ್ಛ finish;
        }

        e = (ಧ್ರುವ ಸಂರಚನಾ namehashent*) ((ಧ್ರುವ uint8_t*) p + h->namehash_offset);
        ಪ್ರತಿ (size_t i = 0; i < h->namehash_size; i++) {
                ವರ್ಣ *z;

                ಯದಿ (e[i].locrec_offset == 0)
                        ಅನುವರ್ತ;

                ಯದಿ (!utf8_is_valid((ವರ್ಣ*) p + e[i].name_offset))
                        ಅನುವರ್ತ;

                z = normalize_locale((ವರ್ಣ*) p + e[i].name_offset);
                ಯದಿ (!z) {
                        r = -ENOMEM;
                        ಗಚ್ಛ finish;
                }

                r = set_consume(locales, z);
                ಯದಿ (r < 0)
                        ಗಚ್ಛ finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(locales);

        dir = opendir(get_locale_dir());
        ಯದಿ (!dir)
                ಪ್ರತ್ಯಾಗಮ errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, ಪ್ರತ್ಯಾಗಮ -errno) {
                ವರ್ಣ *z;

                ಯದಿ (de->d_type != DT_DIR)
                        ಅನುವರ್ತ;

                z = normalize_locale(de->d_name);
                ಯದಿ (!z)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                r = set_consume(locales, z);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

#ಅನ್ಯಥಾ

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        ರಿಕ್ತ
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ add_locales_for_musl(Set *locales) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(locales);

        ಧ್ರುವ ವರ್ಣ *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                ಯದಿ (!dir)
                        ಅನುವರ್ತ;

                FOREACH_DIRENT(de, dir, ಪ್ರತ್ಯಾಗಮ -errno) {
                        ಯದಿ (de->d_type != DT_REG)
                                ಅನುವರ್ತ;

                        ವರ್ಣ *z = normalize_locale(de->d_name);
                        ಯದಿ (!z)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        r = set_consume(locales, z);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

ಪೂರ್ಣಾಙ್ಕ get_locales(ವರ್ಣ ***ret) {
        _cleanup_set_free_ Set *locales = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        locales = set_new(&string_hash_ops_free);
        ಯದಿ (!locales)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        ಯದಿ (r < 0 && r != -ENOENT)
                ಪ್ರತ್ಯಾಗಮ r;

        r = add_locales_from_libdir(locales);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
#ಅನ್ಯಥಾ
        r = add_locales_for_musl(locales);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
#endif

        ವರ್ಣ *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಮುಕ್ತ(set_remove(locales, locale));
        }

        _cleanup_strv_free_ ವರ್ಣ **l = set_to_strv(&locales);
        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        ಯದಿ (r <= 0) {
                ಯದಿ (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                ವರ್ಣ **b = l;
                STRV_FOREACH(a, l)
                        ಯದಿ (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        ಅನ್ಯಥಾ
                                ಮುಕ್ತ(*a);

                *b = ರಿಕ್ತ;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        ಪ್ರತ್ಯಾಗಮ 0;
}

bool locale_is_valid(ಧ್ರುವ ವರ್ಣ *name) {

        ಯದಿ (isempty(name))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (strlen(name) >= 128)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!utf8_is_valid(name))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!filename_is_valid(name))
                ಪ್ರತ್ಯಾಗಮ false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        ಯದಿ (!in_charset(name, ALPHANUMERICAL "_.-@"))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ locale_is_installed(ಧ್ರುವ ವರ್ಣ *name) {
        ಯದಿ (!locale_is_valid(name))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                ಪ್ರತ್ಯಾಗಮ true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        ಯದಿ (loc == (locale_t) 0)
                ಪ್ರತ್ಯಾಗಮ errno == ENOMEM ? -ENOMEM : false;

        ಪ್ರತ್ಯಾಗಮ true;
#ಅನ್ಯಥಾ
        /* musl also has C.UTF-8 as builtin */
        ಯದಿ (streq(name, "C.UTF-8"))
                ಪ್ರತ್ಯಾಗಮ true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ಧ್ರುವ ವರ್ಣ *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ ವರ್ಣ *p = path_join(*d, name);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (access(p, F_OK) >= 0)
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
#endif
}

ಸ್ಥಿರ bool is_locale_utf8_impl(ಶೂನ್ಯ) {
        ಧ್ರುವ ವರ್ಣ *set;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        ಯದಿ (!is_main_thread())
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (!setlocale(LC_ALL, ""))
                ಪ್ರತ್ಯಾಗಮ true;

        set = nl_langinfo(CODESET);
        ಯದಿ (!set || streq(set, "UTF-8"))
                ಪ್ರತ್ಯಾಗಮ true;

        set = setlocale(LC_CTYPE, ರಿಕ್ತ);
        ಯದಿ (!set)
                ಪ್ರತ್ಯಾಗಮ true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cached = -1;

        ಯದಿ (cached < 0)
                cached = is_locale_utf8_impl();

        ಪ್ರತ್ಯಾಗಮ cached;
}

ಶೂನ್ಯ locale_variables_free(ವರ್ಣ *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

ಶೂನ್ಯ locale_variables_simplify(ವರ್ಣ *l[_VARIABLE_LC_MAX]) {
        ಪ್ರತಿಜ್ಞಾ(l);

        ಪ್ರತಿ (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                ಯದಿ (p == VARIABLE_LANG)
                        ಅನುವರ್ತ;
                ಯದಿ (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ locale_variable_table[_VARIABLE_LC_MAX] = {
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
