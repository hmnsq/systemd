/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <langinfo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mman.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locale-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "set.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

ਸਥਿਰ ਅੱਖਰ* normalize_locale(ਅਚਲ ਅੱਖਰ *name) {
        ਅਚਲ ਅੱਖਰ *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        ਜੇ (e) {
                _cleanup_free_ ਅੱਖਰ *ਅਗੇਤਰ = ਨਿਰਾ;

                ਅਗੇਤਰ = strndup(name, e - name);
                ਜੇ (!ਅਗੇਤਰ)
                        ਵਾਪਸ ਨਿਰਾ;

                ਵਾਪਸ strjoin(ਅਗੇਤਰ, ".UTF-8");
        }

        e = ਲੜੀ_ਖੋਜ(name, ".utf8@");
        ਜੇ (e) {
                _cleanup_free_ ਅੱਖਰ *ਅਗੇਤਰ = ਨਿਰਾ;

                ਅਗੇਤਰ = strndup(name, e - name);
                ਜੇ (!ਅਗੇਤਰ)
                        ਵਾਪਸ ਨਿਰਾ;

                ਵਾਪਸ strjoin(ਅਗੇਤਰ, ".UTF-8@", e + 6);
        }

        ਵਾਪਸ strdup(name);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GLIBC__
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* get_locale_dir(ਖਾਲੀ) {
        ਵਾਪਸ secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_locales_from_archive(ਸੈੱਟ_2 *locales) {
        /* Stolen from glibc... */

        ਬਣਤਰ locarhead {
                uint32_t magic;
                /* Serial number.  */
                uint32_t serial;
                /* Name hash table.  */
                uint32_t namehash_ਵਿੱਥ;
                uint32_t namehash_ਵਰਤਿਆ;
                uint32_t namehash_ਮਾਪ;
                /* String table.  */
                uint32_t string_ਵਿੱਥ;
                uint32_t string_ਵਰਤਿਆ;
                uint32_t string_ਮਾਪ;
                /* Table with locale records.  */
                uint32_t locrectab_ਵਿੱਥ;
                uint32_t locrectab_ਵਰਤਿਆ;
                uint32_t locrectab_ਮਾਪ;
                /* MD5 sum hash table.  */
                uint32_t sumhash_ਵਿੱਥ;
                uint32_t sumhash_ਵਰਤਿਆ;
                uint32_t sumhash_ਮਾਪ;
        };

        ਬਣਤਰ namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_ਵਿੱਥ;
                /* Offset of the locale record.  */
                uint32_t locrec_ਵਿੱਥ;
        };

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(locales);

        _cleanup_free_ ਅੱਖਰ *locale_archive_file = ਰਸਤਾ_join(get_locale_dir(), "locale-archive");
        ਜੇ (!locale_archive_file)
                ਵਾਪਸ -ENOMEM;

        _cleanup_close_ ਪੂਰਨ fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ errno == ENOENT ? 0 : -errno;

        ਬਣਤਰ stat st;
        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        r = stat_verify_regular(&st);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (st.st_size < (off_t) ਆਕਾਰ(ਬਣਤਰ locarhead))
                ਵਾਪਸ -EBADMSG;

        ਜੇ (file_ਵਿੱਥ_beyond_ਸਮ੍ਰਿਤੀ_ਮਾਪ(st.st_size))
                ਵਾਪਸ -EFBIG;

        ਖਾਲੀ *p = mmap(ਨਿਰਾ, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        ਜੇ (p == MAP_FAILED)
                ਵਾਪਸ -errno;

        ਅਚਲ ਬਣਤਰ namehashent *e;
        ਅਚਲ ਬਣਤਰ locarhead *h = p;
        ਜੇ (h->magic != 0xde020109 ||
            h->namehash_ਵਿੱਥ + h->namehash_ਮਾਪ > st.st_size ||
            h->string_ਵਿੱਥ + h->string_ਮਾਪ > st.st_size ||
            h->locrectab_ਵਿੱਥ + h->locrectab_ਮਾਪ > st.st_size ||
            h->sumhash_ਵਿੱਥ + h->sumhash_ਮਾਪ > st.st_size) {
                r = -EBADMSG;
                ਜਾਓ ਅੰਤ;
        }

        e = (ਅਚਲ ਬਣਤਰ namehashent*) ((ਅਚਲ uint8_t*) p + h->namehash_ਵਿੱਥ);
        ਲਈ (size_t i = 0; i < h->namehash_ਮਾਪ; i++) {
                ਅੱਖਰ *z;

                ਜੇ (e[i].locrec_ਵਿੱਥ == 0)
                        ਜਾਰੀ;

                ਜੇ (!utf8_is_valid((ਅੱਖਰ*) p + e[i].name_ਵਿੱਥ))
                        ਜਾਰੀ;

                z = normalize_locale((ਅੱਖਰ*) p + e[i].name_ਵਿੱਥ);
                ਜੇ (!z) {
                        r = -ENOMEM;
                        ਜਾਓ ਅੰਤ;
                }

                r = ਸੈੱਟ_consume(locales, z);
                ਜੇ (r < 0)
                        ਜਾਓ ਅੰਤ;
        }

        r = 0;

ਅੰਤ:
        munmap_ਸੁਰੱਖਿਅਤ(p, st.st_size);
        ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_locales_from_libdir(ਸੈੱਟ_2 *locales) {
        _cleanup_closedir_ DIR *dir = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(locales);

        dir = opendir(get_locale_dir());
        ਜੇ (!dir)
                ਵਾਪਸ errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, ਵਾਪਸ -errno) {
                ਅੱਖਰ *z;

                ਜੇ (de->d_type != DT_DIR)
                        ਜਾਰੀ;

                z = normalize_locale(de->d_name);
                ਜੇ (!z)
                        ਵਾਪਸ -ENOMEM;

                r = ਸੈੱਟ_consume(locales, z);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

#ਨਹੀਂਤਾਂ

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        ਨਿਰਾ
};

ਸਥਿਰ ਪੂਰਨ ਜੋੜੋ_locales_for_musl(ਸੈੱਟ_2 *locales) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(locales);

        ਅਚਲ ਅੱਖਰ *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                ਜੇ (!dir)
                        ਜਾਰੀ;

                FOREACH_DIRENT(de, dir, ਵਾਪਸ -errno) {
                        ਜੇ (de->d_type != DT_REG)
                                ਜਾਰੀ;

                        ਅੱਖਰ *z = normalize_locale(de->d_name);
                        ਜੇ (!z)
                                ਵਾਪਸ -ENOMEM;

                        r = ਸੈੱਟ_consume(locales, z);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }
        }

        ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ get_locales(ਅੱਖਰ ***ret) {
        cleanup_ਸੈੱਟ_free ਸੈੱਟ_2 *locales = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        locales = ਸੈੱਟ_new(&string_hash_ops_free);
        ਜੇ (!locales)
                ਵਾਪਸ -ENOMEM;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GLIBC__
        r = ਜੋੜੋ_locales_from_archive(locales);
        ਜੇ (r < 0 && r != -ENOENT)
                ਵਾਪਸ r;

        r = ਜੋੜੋ_locales_from_libdir(locales);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
#ਨਹੀਂਤਾਂ
        r = ਜੋੜੋ_locales_for_musl(locales);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
#ਜੇ_ਅੰਤ

        ਅੱਖਰ *locale;
        ਸੈੱਟ_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਮੁਕਤ(ਸੈੱਟ_remove(locales, locale));
        }

        _cleanup_strv_free_ ਅੱਖਰ **l = ਸੈੱਟ_ਮੰਜ਼ਿਲ_strv(&locales);
        ਜੇ (!l)
                ਵਾਪਸ -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        ਜੇ (r <= 0) {
                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                ਅੱਖਰ **b = l;
                STRV_FOREACH(a, l)
                        ਜੇ (endswith(*a, "UTF-8") || ਲੜੀ_ਖੋਜ(*a, ".UTF-8@"))
                                *(b++) = *a;
                        ਨਹੀਂਤਾਂ
                                ਮੁਕਤ(*a);

                *b = ਨਿਰਾ;
        }

        strv_ਛਾਂਟੋ(l);

        *ret = TAKE_PTR(l);

        ਵਾਪਸ 0;
}

bool locale_is_valid(ਅਚਲ ਅੱਖਰ *name) {

        ਜੇ (isempty(name))
                ਵਾਪਸ false;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(name) >= 128)
                ਵਾਪਸ false;

        ਜੇ (!utf8_is_valid(name))
                ਵਾਪਸ false;

        ਜੇ (!filename_is_valid(name))
                ਵਾਪਸ false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        ਜੇ (!ਵਿੱਚ_charset(name, ALPHANUMERICAL "_.-@"))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਪੂਰਨ locale_is_installed(ਅਚਲ ਅੱਖਰ *name) {
        ਜੇ (!locale_is_valid(name))
                ਵਾਪਸ false;

        ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(name, "C", "POSIX")) /* These ones are always OK */
                ਵਾਪਸ true;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        ਜੇ (loc == (locale_t) 0)
                ਵਾਪਸ errno == ENOMEM ? -ENOMEM : false;

        ਵਾਪਸ true;
#ਨਹੀਂਤਾਂ
        /* musl also has C.UTF-8 as builtin */
        ਜੇ (streq(name, "C.UTF-8"))
                ਵਾਪਸ true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ਅਚਲ ਅੱਖਰ *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ ਅੱਖਰ *p = ਰਸਤਾ_join(*d, name);
                ਜੇ (!p)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (access(p, F_OK) >= 0)
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ bool is_locale_utf8_impl(ਖਾਲੀ) {
        ਅਚਲ ਅੱਖਰ *ਸੈੱਟ;
        ਪੂਰਨ r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        ਜੇ (r >= 0)
                ਵਾਪਸ r;
        ਜੇ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        ਜੇ (!is_main_thread())
                ਵਾਪਸ true;

        ਜੇ (!setlocale(LC_ALL, ""))
                ਵਾਪਸ true;

        ਸੈੱਟ = nl_langinfo(CODESET);
        ਜੇ (!ਸੈੱਟ || streq(ਸੈੱਟ, "UTF-8"))
                ਵਾਪਸ true;

        ਸੈੱਟ = setlocale(LC_CTYPE, ਨਿਰਾ);
        ਜੇ (!ਸੈੱਟ)
                ਵਾਪਸ true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(ਸੈੱਟ, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(ਖਾਲੀ) {
        ਸਥਿਰ ਪੂਰਨ cached = -1;

        ਜੇ (cached < 0)
                cached = is_locale_utf8_impl();

        ਵਾਪਸ cached;
}

ਖਾਲੀ locale_variables_free(ਅੱਖਰ *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

ਖਾਲੀ locale_variables_simplify(ਅੱਖਰ *l[_VARIABLE_LC_MAX]) {
        ਪ੍ਰਤਿਜ੍ਞਾ(l);

        ਲਈ (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                ਜੇ (p == VARIABLE_LANG)
                        ਜਾਰੀ;
                ਜੇ (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ locale_variable_ਸਾਰਣੀ[_VARIABLE_LC_MAX] = {
        [VARIABLE_LANG]              = "LANG",
        [VARIABLE_LANGUAGE]          = "LANGUAGE",
        [VARIABLE_LC_CTYPE]          = "LC_CTYPE",
        [VARIABLE_LC_NUMERIC]        = "LC_NUMERIC",
        [VARIABLE_LC_ਸਮਾਂ]           = "LC_TIME",
        [VARIABLE_LC_COLLATE]        = "LC_COLLATE",
        [VARIABLE_LC_MONETARY]       = "LC_MONETARY",
        [VARIABLE_LC_MESSAGES]       = "LC_MESSAGES",
        [VARIABLE_LC_PAPER]          = "LC_PAPER",
        [VARIABLE_LC_NAME]           = "LC_NAME",
        [VARIABLE_LC_ਪਤਾ]        = "LC_ADDRESS",
        [VARIABLE_LC_TELEPHONE]      = "LC_TELEPHONE",
        [VARIABLE_LC_MEASUREMENT]    = "LC_MEASUREMENT",
        [VARIABLE_LC_IDENTIFICATION] = "LC_IDENTIFICATION"
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(locale_variable, LocaleVariable);
