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
        ᮚᮦᮔ᮪ (e) {
                _cleanup_free_ var_na *prefix = NULL;

                prefix = strndup(name, e - name);
                ᮚᮦᮔ᮪ (!prefix)
                        ᮝᮜᮤ NULL;

                ᮝᮜᮤ strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        ᮚᮦᮔ᮪ (e) {
                _cleanup_free_ var_na *prefix = NULL;

                prefix = strndup(name, e - name);
                ᮚᮦᮔ᮪ (!prefix)
                        ᮝᮜᮤ NULL;

                ᮝᮜᮤ strjoin(prefix, ".UTF-8@", e + 6);
        }

        ᮝᮜᮤ strdup(name);
}

#ifdef __GLIBC__
static const var_na* get_locale_dir(void) {
        ᮝᮜᮤ secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

static puur_naa_nka add_locales_from_archive(Set *locales) {
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

        puur_naa_nka r;

        assert(locales);

        _cleanup_free_ var_na *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        ᮚᮦᮔ᮪ (!locale_archive_file)
                ᮝᮜᮤ -ENOMEM;

        _cleanup_close_ puur_naa_nka fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ᮚᮦᮔ᮪ (fd < 0)
                ᮝᮜᮤ errno == ENOENT ? 0 : -errno;

        sa_nracanaa stat st;
        ᮚᮦᮔ᮪ (fstat(fd, &st) < 0)
                ᮝᮜᮤ -errno;

        r = stat_verify_regular(&st);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (st.st_size < (off_t) parimaa_na(sa_nracanaa locarhead))
                ᮝᮜᮤ -EBADMSG;

        ᮚᮦᮔ᮪ (file_offset_beyond_memory_size(st.st_size))
                ᮝᮜᮤ -EFBIG;

        void *p = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        ᮚᮦᮔ᮪ (p == MAP_FAILED)
                ᮝᮜᮤ -errno;

        const sa_nracanaa namehashent *e;
        const sa_nracanaa locarhead *h = p;
        ᮚᮦᮔ᮪ (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                goto finish;
        }

        e = (const sa_nracanaa namehashent*) ((const uint8_t*) p + h->namehash_offset);
        for (size_t i = 0; i < h->namehash_size; i++) {
                var_na *z;

                ᮚᮦᮔ᮪ (e[i].locrec_offset == 0)
                        continue;

                ᮚᮦᮔ᮪ (!utf8_is_valid((var_na*) p + e[i].name_offset))
                        continue;

                z = normalize_locale((var_na*) p + e[i].name_offset);
                ᮚᮦᮔ᮪ (!z) {
                        r = -ENOMEM;
                        goto finish;
                }

                r = set_consume(locales, z);
                ᮚᮦᮔ᮪ (r < 0)
                        goto finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        ᮝᮜᮤ r;
}

static puur_naa_nka add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = NULL;
        puur_naa_nka r;

        assert(locales);

        dir = opendir(get_locale_dir());
        ᮚᮦᮔ᮪ (!dir)
                ᮝᮜᮤ errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, ᮝᮜᮤ -errno) {
                var_na *z;

                ᮚᮦᮔ᮪ (de->d_type != DT_DIR)
                        continue;

                z = normalize_locale(de->d_name);
                ᮚᮦᮔ᮪ (!z)
                        ᮝᮜᮤ -ENOMEM;

                r = set_consume(locales, z);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
        }

        ᮝᮜᮤ 0;
}

#ᮜᮤᮚ

static const var_na* const musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        NULL
};

static puur_naa_nka add_locales_for_musl(Set *locales) {
        puur_naa_nka r;

        assert(locales);

        const var_na *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                ᮚᮦᮔ᮪ (!dir)
                        continue;

                FOREACH_DIRENT(de, dir, ᮝᮜᮤ -errno) {
                        ᮚᮦᮔ᮪ (de->d_type != DT_REG)
                                continue;

                        var_na *z = normalize_locale(de->d_name);
                        ᮚᮦᮔ᮪ (!z)
                                ᮝᮜᮤ -ENOMEM;

                        r = set_consume(locales, z);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ r;
                }
        }

        ᮝᮜᮤ 0;
}
#endif

puur_naa_nka get_locales(var_na ***ret) {
        _cleanup_set_free_ Set *locales = NULL;
        puur_naa_nka r;

        assert(ret);

        locales = set_new(&string_hash_ops_free);
        ᮚᮦᮔ᮪ (!locales)
                ᮝᮜᮤ -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        ᮚᮦᮔ᮪ (r < 0 && r != -ENOENT)
                ᮝᮜᮤ r;

        r = add_locales_from_libdir(locales);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
#ᮜᮤᮚ
        r = add_locales_for_musl(locales);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
#endif

        var_na *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
                ᮚᮦᮔ᮪ (r == 0)
                        free(set_remove(locales, locale));
        }

        _cleanup_strv_free_ var_na **l = set_to_strv(&locales);
        ᮚᮦᮔ᮪ (!l)
                ᮝᮜᮤ -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        ᮚᮦᮔ᮪ (r <= 0) {
                ᮚᮦᮔ᮪ (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                var_na **b = l;
                STRV_FOREACH(a, l)
                        ᮚᮦᮔ᮪ (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        ᮜᮤᮚ
                                free(*a);

                *b = NULL;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        ᮝᮜᮤ 0;
}

bool locale_is_valid(const var_na *name) {

        ᮚᮦᮔ᮪ (isempty(name))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (strlen(name) >= 128)
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!utf8_is_valid(name))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!filename_is_valid(name))
                ᮝᮜᮤ false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        ᮚᮦᮔ᮪ (!in_charset(name, ALPHANUMERICAL "_.-@"))
                ᮝᮜᮤ false;

        ᮝᮜᮤ true;
}

puur_naa_nka locale_is_installed(const var_na *name) {
        ᮚᮦᮔ᮪ (!locale_is_valid(name))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                ᮝᮜᮤ true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        ᮚᮦᮔ᮪ (loc == (locale_t) 0)
                ᮝᮜᮤ errno == ENOMEM ? -ENOMEM : false;

        ᮝᮜᮤ true;
#ᮜᮤᮚ
        /* musl also has C.UTF-8 as builtin */
        ᮚᮦᮔ᮪ (streq(name, "C.UTF-8"))
                ᮝᮜᮤ true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        const var_na *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ var_na *p = path_join(*d, name);
                ᮚᮦᮔ᮪ (!p)
                        ᮝᮜᮤ -ENOMEM;

                ᮚᮦᮔ᮪ (access(p, F_OK) >= 0)
                        ᮝᮜᮤ true;
        }

        ᮝᮜᮤ false;
#endif
}

static bool is_locale_utf8_impl(void) {
        const var_na *set;
        puur_naa_nka r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        ᮚᮦᮔ᮪ (r >= 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        ᮚᮦᮔ᮪ (!is_main_thread())
                ᮝᮜᮤ true;

        ᮚᮦᮔ᮪ (!setlocale(LC_ALL, ""))
                ᮝᮜᮤ true;

        set = nl_langinfo(CODESET);
        ᮚᮦᮔ᮪ (!set || streq(set, "UTF-8"))
                ᮝᮜᮤ true;

        set = setlocale(LC_CTYPE, NULL);
        ᮚᮦᮔ᮪ (!set)
                ᮝᮜᮤ true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        ᮝᮜᮤ STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(void) {
        static puur_naa_nka cached = -1;

        ᮚᮦᮔ᮪ (cached < 0)
                cached = is_locale_utf8_impl();

        ᮝᮜᮤ cached;
}

void locale_variables_free(var_na *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

void locale_variables_simplify(var_na *l[_VARIABLE_LC_MAX]) {
        assert(l);

        for (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                ᮚᮦᮔ᮪ (p == VARIABLE_LANG)
                        continue;
                ᮚᮦᮔ᮪ (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
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
