/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <langinfo.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dirent-util.h"
#include "env-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "locale-util.h"
#include "log.h"
#include "memory-util.h"
#include "path-util.h"
#include "process-util.h"
#include "set.h"
#include "stat-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "utf8.h"

static var_na* normalize_locale(const var_na *name) {
        const var_na *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        ถ้า (e) {
                _cleanup_free_ var_na *prefix = NULL;

                prefix = strndup(name, e - name);
                ถ้า (!prefix)
                        คืน NULL;

                คืน strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        ถ้า (e) {
                _cleanup_free_ var_na *prefix = NULL;

                prefix = strndup(name, e - name);
                ถ้า (!prefix)
                        คืน NULL;

                คืน strjoin(prefix, ".UTF-8@", e + 6);
        }

        คืน strdup(name);
}

#ifdef __GLIBC__
static const var_na* get_locale_dir(void) {
        คืน secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

static จำนวนเต็ม add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        sa_nracanaa locarhead {
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

        sa_nracanaa namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        จำนวนเต็ม r;

        assert(locales);

        _cleanup_free_ var_na *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        ถ้า (!locale_archive_file)
                คืน -ENOMEM;

        _cleanup_close_ จำนวนเต็ม fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ถ้า (fd < 0)
                คืน errno == ENOENT ? 0 : -errno;

        sa_nracanaa stat st;
        ถ้า (fstat(fd, &st) < 0)
                คืน -errno;

        r = stat_verify_regular(&st);
        ถ้า (r < 0)
                คืน r;

        ถ้า (st.st_size < (off_t) parimaa_na(sa_nracanaa locarhead))
                คืน -EBADMSG;

        ถ้า (file_offset_beyond_memory_size(st.st_size))
                คืน -EFBIG;

        void *p = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        ถ้า (p == MAP_FAILED)
                คืน -errno;

        const sa_nracanaa namehashent *e;
        const sa_nracanaa locarhead *h = p;
        ถ้า (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                goto finish;
        }

        e = (const sa_nracanaa namehashent*) ((const uint8_t*) p + h->namehash_offset);
        สำหรับ (size_t i = 0; i < h->namehash_size; i++) {
                var_na *z;

                ถ้า (e[i].locrec_offset == 0)
                        continue;

                ถ้า (!utf8_is_valid((var_na*) p + e[i].name_offset))
                        continue;

                z = normalize_locale((var_na*) p + e[i].name_offset);
                ถ้า (!z) {
                        r = -ENOMEM;
                        goto finish;
                }

                r = set_consume(locales, z);
                ถ้า (r < 0)
                        goto finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        คืน r;
}

static จำนวนเต็ม add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = NULL;
        จำนวนเต็ม r;

        assert(locales);

        dir = opendir(get_locale_dir());
        ถ้า (!dir)
                คืน errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, คืน -errno) {
                var_na *z;

                ถ้า (de->d_type != DT_DIR)
                        continue;

                z = normalize_locale(de->d_name);
                ถ้า (!z)
                        คืน -ENOMEM;

                r = set_consume(locales, z);
                ถ้า (r < 0)
                        คืน r;
        }

        คืน 0;
}

#มิฉะนั้น

static const var_na* const musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        NULL
};

static จำนวนเต็ม add_locales_for_musl(Set *locales) {
        จำนวนเต็ม r;

        assert(locales);

        const var_na *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                ถ้า (!dir)
                        continue;

                FOREACH_DIRENT(de, dir, คืน -errno) {
                        ถ้า (de->d_type != DT_REG)
                                continue;

                        var_na *z = normalize_locale(de->d_name);
                        ถ้า (!z)
                                คืน -ENOMEM;

                        r = set_consume(locales, z);
                        ถ้า (r < 0)
                                คืน r;
                }
        }

        คืน 0;
}
#endif

จำนวนเต็ม get_locales(var_na ***ret) {
        _cleanup_set_free_ Set *locales = NULL;
        จำนวนเต็ม r;

        assert(ret);

        locales = set_new(&string_hash_ops_free);
        ถ้า (!locales)
                คืน -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        ถ้า (r < 0 && r != -ENOENT)
                คืน r;

        r = add_locales_from_libdir(locales);
        ถ้า (r < 0)
                คืน r;
#มิฉะนั้น
        r = add_locales_for_musl(locales);
        ถ้า (r < 0)
                คืน r;
#endif

        var_na *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        free(set_remove(locales, locale));
        }

        _cleanup_strv_free_ var_na **l = set_to_strv(&locales);
        ถ้า (!l)
                คืน -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        ถ้า (r <= 0) {
                ถ้า (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                var_na **b = l;
                STRV_FOREACH(a, l)
                        ถ้า (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        มิฉะนั้น
                                free(*a);

                *b = NULL;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        คืน 0;
}

bool locale_is_valid(const var_na *name) {

        ถ้า (isempty(name))
                คืน false;

        ถ้า (strlen(name) >= 128)
                คืน false;

        ถ้า (!utf8_is_valid(name))
                คืน false;

        ถ้า (!filename_is_valid(name))
                คืน false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        ถ้า (!in_charset(name, ALPHANUMERICAL "_.-@"))
                คืน false;

        คืน true;
}

จำนวนเต็ม locale_is_installed(const var_na *name) {
        ถ้า (!locale_is_valid(name))
                คืน false;

        ถ้า (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                คืน true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        ถ้า (loc == (locale_t) 0)
                คืน errno == ENOMEM ? -ENOMEM : false;

        คืน true;
#มิฉะนั้น
        /* musl also has C.UTF-8 as builtin */
        ถ้า (streq(name, "C.UTF-8"))
                คืน true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        const var_na *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ var_na *p = path_join(*d, name);
                ถ้า (!p)
                        คืน -ENOMEM;

                ถ้า (access(p, F_OK) >= 0)
                        คืน true;
        }

        คืน false;
#endif
}

static bool is_locale_utf8_impl(void) {
        const var_na *set;
        จำนวนเต็ม r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        ถ้า (r >= 0)
                คืน r;
        ถ้า (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        ถ้า (!is_main_thread())
                คืน true;

        ถ้า (!setlocale(LC_ALL, ""))
                คืน true;

        set = nl_langinfo(CODESET);
        ถ้า (!set || streq(set, "UTF-8"))
                คืน true;

        set = setlocale(LC_CTYPE, NULL);
        ถ้า (!set)
                คืน true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        คืน STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(void) {
        static จำนวนเต็ม cached = -1;

        ถ้า (cached < 0)
                cached = is_locale_utf8_impl();

        คืน cached;
}

void locale_variables_free(var_na *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

void locale_variables_simplify(var_na *l[_VARIABLE_LC_MAX]) {
        assert(l);

        สำหรับ (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                ถ้า (p == VARIABLE_LANG)
                        continue;
                ถ้า (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

static const var_na * const locale_variable_table[_VARIABLE_LC_MAX] = {
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
