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
        אם (e) {
                _cleanup_free_ حرف *prefix = عدم;

                prefix = strndup(name, e - name);
                אם (!prefix)
                        החזר عدم;

                החזר strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        אם (e) {
                _cleanup_free_ حرف *prefix = عدم;

                prefix = strndup(name, e - name);
                אם (!prefix)
                        החזר عدم;

                החזר strjoin(prefix, ".UTF-8@", e + 6);
        }

        החזר strdup(name);
}

#ifdef __GLIBC__
ساكن ثابت حرف* get_locale_dir(فراغ) {
        החזר secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
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
        אם (!locale_archive_file)
                החזר -ENOMEM;

        _cleanup_close_ صحيح fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        אם (fd < 0)
                החזר errno == ENOENT ? 0 : -errno;

        بنية stat st;
        אם (fstat(fd, &st) < 0)
                החזר -errno;

        r = stat_verify_regular(&st);
        אם (r < 0)
                החזר r;

        אם (st.st_size < (off_t) حجم(بنية locarhead))
                החזר -EBADMSG;

        אם (file_offset_beyond_memory_size(st.st_size))
                החזר -EFBIG;

        فراغ *p = mmap(عدم, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        אם (p == MAP_FAILED)
                החזר -errno;

        ثابت بنية namehashent *e;
        ثابت بنية locarhead *h = p;
        אם (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                اذهب finish;
        }

        e = (ثابت بنية namehashent*) ((ثابت uint8_t*) p + h->namehash_offset);
        עבור (size_t i = 0; i < h->namehash_size; i++) {
                حرف *z;

                אם (e[i].locrec_offset == 0)
                        استمر;

                אם (!utf8_is_valid((حرف*) p + e[i].name_offset))
                        استمر;

                z = normalize_locale((حرف*) p + e[i].name_offset);
                אם (!z) {
                        r = -ENOMEM;
                        اذهب finish;
                }

                r = set_consume(locales, z);
                אם (r < 0)
                        اذهب finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        החזר r;
}

ساكن صحيح add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = عدم;
        صحيح r;

        أكد(locales);

        dir = opendir(get_locale_dir());
        אם (!dir)
                החזר errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, החזר -errno) {
                حرف *z;

                אם (de->d_type != DT_DIR)
                        استمر;

                z = normalize_locale(de->d_name);
                אם (!z)
                        החזר -ENOMEM;

                r = set_consume(locales, z);
                אם (r < 0)
                        החזר r;
        }

        החזר 0;
}

#אחרת

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
                אם (!dir)
                        استمر;

                FOREACH_DIRENT(de, dir, החזר -errno) {
                        אם (de->d_type != DT_REG)
                                استمر;

                        حرف *z = normalize_locale(de->d_name);
                        אם (!z)
                                החזר -ENOMEM;

                        r = set_consume(locales, z);
                        אם (r < 0)
                                החזר r;
                }
        }

        החזר 0;
}
#endif

صحيح get_locales(حرف ***ret) {
        _cleanup_set_free_ Set *locales = عدم;
        صحيح r;

        أكد(ret);

        locales = set_new(&string_hash_ops_free);
        אם (!locales)
                החזר -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        אם (r < 0 && r != -ENOENT)
                החזר r;

        r = add_locales_from_libdir(locales);
        אם (r < 0)
                החזר r;
#אחרת
        r = add_locales_for_musl(locales);
        אם (r < 0)
                החזר r;
#endif

        حرف *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                אם (r < 0)
                        החזר r;
                אם (r == 0)
                        حرر(set_remove(locales, locale));
        }

        _cleanup_strv_free_ حرف **l = set_to_strv(&locales);
        אם (!l)
                החזר -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        אם (r <= 0) {
                אם (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                حرف **b = l;
                STRV_FOREACH(a, l)
                        אם (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        אחרת
                                حرر(*a);

                *b = عدم;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        החזר 0;
}

bool locale_is_valid(ثابت حرف *name) {

        אם (isempty(name))
                החזר false;

        אם (strlen(name) >= 128)
                החזר false;

        אם (!utf8_is_valid(name))
                החזר false;

        אם (!filename_is_valid(name))
                החזר false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        אם (!in_charset(name, ALPHANUMERICAL "_.-@"))
                החזר false;

        החזר true;
}

صحيح locale_is_installed(ثابت حرف *name) {
        אם (!locale_is_valid(name))
                החזר false;

        אם (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                החזר true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        אם (loc == (locale_t) 0)
                החזר errno == ENOMEM ? -ENOMEM : false;

        החזר true;
#אחרת
        /* musl also has C.UTF-8 as builtin */
        אם (streq(name, "C.UTF-8"))
                החזר true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ثابت حرف *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ حرف *p = path_join(*d, name);
                אם (!p)
                        החזר -ENOMEM;

                אם (access(p, F_OK) >= 0)
                        החזר true;
        }

        החזר false;
#endif
}

ساكن bool is_locale_utf8_impl(فراغ) {
        ثابت حرف *set;
        صحيح r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        אם (r >= 0)
                החזר r;
        אם (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        אם (!is_main_thread())
                החזר true;

        אם (!setlocale(LC_ALL, ""))
                החזר true;

        set = nl_langinfo(CODESET);
        אם (!set || streq(set, "UTF-8"))
                החזר true;

        set = setlocale(LC_CTYPE, عدم);
        אם (!set)
                החזר true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        החזר STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(فراغ) {
        ساكن صحيح cached = -1;

        אם (cached < 0)
                cached = is_locale_utf8_impl();

        החזר cached;
}

فراغ locale_variables_free(حرف *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

فراغ locale_variables_simplify(حرف *l[_VARIABLE_LC_MAX]) {
        أكد(l);

        עבור (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                אם (p == VARIABLE_LANG)
                        استمر;
                אם (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
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
