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

ಸ್ಥಿರ ಅಕ್ಷರ* normalize_locale(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        ಆದರೆ (e) {
                _cleanup_free_ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;

                ಪೂರ್ವಪ್ರತ್ಯಯ = strndup(name, e - name);
                ಆದರೆ (!ಪೂರ್ವಪ್ರತ್ಯಯ)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                ಹಿಂತಿರುಗಿ strjoin(ಪೂರ್ವಪ್ರತ್ಯಯ, ".UTF-8");
        }

        e = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(name, ".utf8@");
        ಆದರೆ (e) {
                _cleanup_free_ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ = ಖಾಲಿ;

                ಪೂರ್ವಪ್ರತ್ಯಯ = strndup(name, e - name);
                ಆದರೆ (!ಪೂರ್ವಪ್ರತ್ಯಯ)
                        ಹಿಂತಿರುಗಿ ಖಾಲಿ;

                ಹಿಂತಿರುಗಿ strjoin(ಪೂರ್ವಪ್ರತ್ಯಯ, ".UTF-8@", e + 6);
        }

        ಹಿಂತಿರುಗಿ strdup(name);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GLIBC__
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* get_locale_dir(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_locales_from_archive(ಗಣ_2 *locales) {
        /* Stolen from glibc... */

        ರಚನೆ locarhead {
                uint32_t magic;
                /* Serial number.  */
                uint32_t serial;
                /* Name hash table.  */
                uint32_t namehash_ಸ್ಥಾನಾಂತರ;
                uint32_t namehash_ಬಳಸಿದ;
                uint32_t namehash_ಗಾತ್ರ;
                /* String table.  */
                uint32_t string_ಸ್ಥಾನಾಂತರ;
                uint32_t string_ಬಳಸಿದ;
                uint32_t string_ಗಾತ್ರ;
                /* Table with locale records.  */
                uint32_t locrectab_ಸ್ಥಾನಾಂತರ;
                uint32_t locrectab_ಬಳಸಿದ;
                uint32_t locrectab_ಗಾತ್ರ;
                /* MD5 sum hash table.  */
                uint32_t sumhash_ಸ್ಥಾನಾಂತರ;
                uint32_t sumhash_ಬಳಸಿದ;
                uint32_t sumhash_ಗಾತ್ರ;
        };

        ರಚನೆ namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_ಸ್ಥಾನಾಂತರ;
                /* Offset of the locale record.  */
                uint32_t locrec_ಸ್ಥಾನಾಂತರ;
        };

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(locales);

        _cleanup_free_ ಅಕ್ಷರ *locale_archive_file = ಮಾರ್ಗ_join(get_locale_dir(), "locale-archive");
        ಆದರೆ (!locale_archive_file)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ errno == ENOENT ? 0 : -errno;

        ರಚನೆ stat st;
        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = stat_verify_regular(&st);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (st.st_size < (off_t) ಗಾತ್ರ(ರಚನೆ locarhead))
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (file_ಸ್ಥಾನಾಂತರ_beyond_ಸ್ಮರಣೆ_ಗಾತ್ರ(st.st_size))
                ಹಿಂತಿರುಗಿ -EFBIG;

        ಶೂನ್ಯ *p = mmap(ಖಾಲಿ, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        ಆದರೆ (p == MAP_FAILED)
                ಹಿಂತಿರುಗಿ -errno;

        ಸ್ಥಿರಾಂಕ ರಚನೆ namehashent *e;
        ಸ್ಥಿರಾಂಕ ರಚನೆ locarhead *h = p;
        ಆದರೆ (h->magic != 0xde020109 ||
            h->namehash_ಸ್ಥಾನಾಂತರ + h->namehash_ಗಾತ್ರ > st.st_size ||
            h->string_ಸ್ಥಾನಾಂತರ + h->string_ಗಾತ್ರ > st.st_size ||
            h->locrectab_ಸ್ಥಾನಾಂತರ + h->locrectab_ಗಾತ್ರ > st.st_size ||
            h->sumhash_ಸ್ಥಾನಾಂತರ + h->sumhash_ಗಾತ್ರ > st.st_size) {
                r = -EBADMSG;
                ಹೋಗು ಮುಕ್ತಾಯ;
        }

        e = (ಸ್ಥಿರಾಂಕ ರಚನೆ namehashent*) ((ಸ್ಥಿರಾಂಕ uint8_t*) p + h->namehash_ಸ್ಥಾನಾಂತರ);
        ಗಾಗಿ (size_t i = 0; i < h->namehash_ಗಾತ್ರ; i++) {
                ಅಕ್ಷರ *z;

                ಆದರೆ (e[i].locrec_ಸ್ಥಾನಾಂತರ == 0)
                        ಮುಂದುವರಿ;

                ಆದರೆ (!utf8_is_valid((ಅಕ್ಷರ*) p + e[i].name_ಸ್ಥಾನಾಂತರ))
                        ಮುಂದುವರಿ;

                z = normalize_locale((ಅಕ್ಷರ*) p + e[i].name_ಸ್ಥಾನಾಂತರ);
                ಆದರೆ (!z) {
                        r = -ENOMEM;
                        ಹೋಗು ಮುಕ್ತಾಯ;
                }

                r = ಗಣ_consume(locales, z);
                ಆದರೆ (r < 0)
                        ಹೋಗು ಮುಕ್ತಾಯ;
        }

        r = 0;

ಮುಕ್ತಾಯ:
        munmap_ಸುರಕ್ಷಿತ(p, st.st_size);
        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_locales_from_libdir(ಗಣ_2 *locales) {
        _cleanup_closedir_ DIR *dir = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(locales);

        dir = opendir(get_locale_dir());
        ಆದರೆ (!dir)
                ಹಿಂತಿರುಗಿ errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, ಹಿಂತಿರುಗಿ -errno) {
                ಅಕ್ಷರ *z;

                ಆದರೆ (de->d_type != DT_DIR)
                        ಮುಂದುವರಿ;

                z = normalize_locale(de->d_name);
                ಆದರೆ (!z)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                r = ಗಣ_consume(locales, z);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

#ಇಲ್ಲದಿದ್ದರೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        ಖಾಲಿ
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_locales_for_musl(ಗಣ_2 *locales) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(locales);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                ಆದರೆ (!dir)
                        ಮುಂದುವರಿ;

                FOREACH_DIRENT(de, dir, ಹಿಂತಿರುಗಿ -errno) {
                        ಆದರೆ (de->d_type != DT_REG)
                                ಮುಂದುವರಿ;

                        ಅಕ್ಷರ *z = normalize_locale(de->d_name);
                        ಆದರೆ (!z)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        r = ಗಣ_consume(locales, z);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }
        }

        ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ get_locales(ಅಕ್ಷರ ***ret) {
        cleanup_ಗಣ_free ಗಣ_2 *locales = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        locales = ಗಣ_new(&string_hash_ops_free);
        ಆದರೆ (!locales)
                ಹಿಂತಿರುಗಿ -ENOMEM;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GLIBC__
        r = ಕೂಡಿಸು_locales_from_archive(locales);
        ಆದರೆ (r < 0 && r != -ENOENT)
                ಹಿಂತಿರುಗಿ r;

        r = ಕೂಡಿಸು_locales_from_libdir(locales);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
#ಇಲ್ಲದಿದ್ದರೆ
        r = ಕೂಡಿಸು_locales_for_musl(locales);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
#ಆದರೆ_ಅಂತ್ಯ

        ಅಕ್ಷರ *locale;
        ಗಣ_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಬಿಡುಗಡೆ(ಗಣ_remove(locales, locale));
        }

        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಗಣ_ಗಮ್ಯ_strv(&locales);
        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        ಆದರೆ (r <= 0) {
                ಆದರೆ (!ಒಳ_ಗಣ(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                ಅಕ್ಷರ **b = l;
                STRV_FOREACH(a, l)
                        ಆದರೆ (endswith(*a, "UTF-8") || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(*a, ".UTF-8@"))
                                *(b++) = *a;
                        ಇಲ್ಲದಿದ್ದರೆ
                                ಬಿಡುಗಡೆ(*a);

                *b = ಖಾಲಿ;
        }

        strv_ವಿಂಗಡಿಸು(l);

        *ret = TAKE_PTR(l);

        ಹಿಂತಿರುಗಿ 0;
}

bool locale_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {

        ಆದರೆ (isempty(name))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) >= 128)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!utf8_is_valid(name))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!filename_is_valid(name))
                ಹಿಂತಿರುಗಿ false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        ಆದರೆ (!ಒಳ_charset(name, ALPHANUMERICAL "_.-@"))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ locale_is_installed(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಆದರೆ (!locale_is_valid(name))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (STR_ಒಳ_ಗಣ(name, "C", "POSIX")) /* These ones are always OK */
                ಹಿಂತಿರುಗಿ true;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        ಆದರೆ (loc == (locale_t) 0)
                ಹಿಂತಿರುಗಿ errno == ENOMEM ? -ENOMEM : false;

        ಹಿಂತಿರುಗಿ true;
#ಇಲ್ಲದಿದ್ದರೆ
        /* musl also has C.UTF-8 as builtin */
        ಆದರೆ (streq(name, "C.UTF-8"))
                ಹಿಂತಿರುಗಿ true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ ಅಕ್ಷರ *p = ಮಾರ್ಗ_join(*d, name);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (access(p, F_OK) >= 0)
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ bool is_locale_utf8_impl(ಶೂನ್ಯ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗಣ;
        ಪೂರ್ಣಾಂಕ r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        ಆದರೆ (!is_main_thread())
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (!setlocale(LC_ALL, ""))
                ಹಿಂತಿರುಗಿ true;

        ಗಣ = nl_langinfo(CODESET);
        ಆದರೆ (!ಗಣ || streq(ಗಣ, "UTF-8"))
                ಹಿಂತಿರುಗಿ true;

        ಗಣ = setlocale(LC_CTYPE, ಖಾಲಿ);
        ಆದರೆ (!ಗಣ)
                ಹಿಂತಿರುಗಿ true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(ಗಣ, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cached = -1;

        ಆದರೆ (cached < 0)
                cached = is_locale_utf8_impl();

        ಹಿಂತಿರುಗಿ cached;
}

ಶೂನ್ಯ locale_variables_free(ಅಕ್ಷರ *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

ಶೂನ್ಯ locale_variables_simplify(ಅಕ್ಷರ *l[_VARIABLE_LC_MAX]) {
        ಪ್ರತಿಜ್ಞಾ(l);

        ಗಾಗಿ (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                ಆದರೆ (p == VARIABLE_LANG)
                        ಮುಂದುವರಿ;
                ಆದರೆ (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ locale_variable_ಕೋಷ್ಟಕ[_VARIABLE_LC_MAX] = {
        [VARIABLE_LANG]              = "LANG",
        [VARIABLE_LANGUAGE]          = "LANGUAGE",
        [VARIABLE_LC_CTYPE]          = "LC_CTYPE",
        [VARIABLE_LC_NUMERIC]        = "LC_NUMERIC",
        [VARIABLE_LC_ಸಮಯ]           = "LC_TIME",
        [VARIABLE_LC_COLLATE]        = "LC_COLLATE",
        [VARIABLE_LC_MONETARY]       = "LC_MONETARY",
        [VARIABLE_LC_MESSAGES]       = "LC_MESSAGES",
        [VARIABLE_LC_PAPER]          = "LC_PAPER",
        [VARIABLE_LC_NAME]           = "LC_NAME",
        [VARIABLE_LC_ವಿಳಾಸ]        = "LC_ADDRESS",
        [VARIABLE_LC_TELEPHONE]      = "LC_TELEPHONE",
        [VARIABLE_LC_MEASUREMENT]    = "LC_MEASUREMENT",
        [VARIABLE_LC_IDENTIFICATION] = "LC_IDENTIFICATION"
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(locale_variable, LocaleVariable);
