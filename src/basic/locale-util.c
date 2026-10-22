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

static char* normalize_locale(const char *name) {
        const char *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        만약 (e) {
                _cleanup_free_ char *prefix = NULL;

                prefix = strndup(name, e - name);
                만약 (!prefix)
                        반환 NULL;

                반환 strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        만약 (e) {
                _cleanup_free_ char *prefix = NULL;

                prefix = strndup(name, e - name);
                만약 (!prefix)
                        반환 NULL;

                반환 strjoin(prefix, ".UTF-8@", e + 6);
        }

        반환 strdup(name);
}

#ifdef __GLIBC__
static const char* get_locale_dir(void) {
        반환 secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

static 정수 add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        struct locarhead {
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

        struct namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        정수 r;

        assert(locales);

        _cleanup_free_ char *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        만약 (!locale_archive_file)
                반환 -ENOMEM;

        _cleanup_close_ 정수 fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        만약 (fd < 0)
                반환 errno == ENOENT ? 0 : -errno;

        struct stat st;
        만약 (fstat(fd, &st) < 0)
                반환 -errno;

        r = stat_verify_regular(&st);
        만약 (r < 0)
                반환 r;

        만약 (st.st_size < (off_t) sizeof(struct locarhead))
                반환 -EBADMSG;

        만약 (file_offset_beyond_memory_size(st.st_size))
                반환 -EFBIG;

        void *p = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        만약 (p == MAP_FAILED)
                반환 -errno;

        const struct namehashent *e;
        const struct locarhead *h = p;
        만약 (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                goto finish;
        }

        e = (const struct namehashent*) ((const uint8_t*) p + h->namehash_offset);
        위해 (size_t i = 0; i < h->namehash_size; i++) {
                char *z;

                만약 (e[i].locrec_offset == 0)
                        continue;

                만약 (!utf8_is_valid((char*) p + e[i].name_offset))
                        continue;

                z = normalize_locale((char*) p + e[i].name_offset);
                만약 (!z) {
                        r = -ENOMEM;
                        goto finish;
                }

                r = set_consume(locales, z);
                만약 (r < 0)
                        goto finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        반환 r;
}

static 정수 add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = NULL;
        정수 r;

        assert(locales);

        dir = opendir(get_locale_dir());
        만약 (!dir)
                반환 errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, 반환 -errno) {
                char *z;

                만약 (de->d_type != DT_DIR)
                        continue;

                z = normalize_locale(de->d_name);
                만약 (!z)
                        반환 -ENOMEM;

                r = set_consume(locales, z);
                만약 (r < 0)
                        반환 r;
        }

        반환 0;
}

#아니면

static const char* const musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        NULL
};

static 정수 add_locales_for_musl(Set *locales) {
        정수 r;

        assert(locales);

        const char *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                만약 (!dir)
                        continue;

                FOREACH_DIRENT(de, dir, 반환 -errno) {
                        만약 (de->d_type != DT_REG)
                                continue;

                        char *z = normalize_locale(de->d_name);
                        만약 (!z)
                                반환 -ENOMEM;

                        r = set_consume(locales, z);
                        만약 (r < 0)
                                반환 r;
                }
        }

        반환 0;
}
#endif

정수 get_locales(char ***ret) {
        _cleanup_set_free_ Set *locales = NULL;
        정수 r;

        assert(ret);

        locales = set_new(&string_hash_ops_free);
        만약 (!locales)
                반환 -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        만약 (r < 0 && r != -ENOENT)
                반환 r;

        r = add_locales_from_libdir(locales);
        만약 (r < 0)
                반환 r;
#아니면
        r = add_locales_for_musl(locales);
        만약 (r < 0)
                반환 r;
#endif

        char *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                만약 (r < 0)
                        반환 r;
                만약 (r == 0)
                        free(set_remove(locales, locale));
        }

        _cleanup_strv_free_ char **l = set_to_strv(&locales);
        만약 (!l)
                반환 -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        만약 (r <= 0) {
                만약 (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                char **b = l;
                STRV_FOREACH(a, l)
                        만약 (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        아니면
                                free(*a);

                *b = NULL;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        반환 0;
}

bool locale_is_valid(const char *name) {

        만약 (isempty(name))
                반환 false;

        만약 (strlen(name) >= 128)
                반환 false;

        만약 (!utf8_is_valid(name))
                반환 false;

        만약 (!filename_is_valid(name))
                반환 false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        만약 (!in_charset(name, ALPHANUMERICAL "_.-@"))
                반환 false;

        반환 true;
}

정수 locale_is_installed(const char *name) {
        만약 (!locale_is_valid(name))
                반환 false;

        만약 (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                반환 true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        만약 (loc == (locale_t) 0)
                반환 errno == ENOMEM ? -ENOMEM : false;

        반환 true;
#아니면
        /* musl also has C.UTF-8 as builtin */
        만약 (streq(name, "C.UTF-8"))
                반환 true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        const char *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ char *p = path_join(*d, name);
                만약 (!p)
                        반환 -ENOMEM;

                만약 (access(p, F_OK) >= 0)
                        반환 true;
        }

        반환 false;
#endif
}

static bool is_locale_utf8_impl(void) {
        const char *set;
        정수 r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        만약 (r >= 0)
                반환 r;
        만약 (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        만약 (!is_main_thread())
                반환 true;

        만약 (!setlocale(LC_ALL, ""))
                반환 true;

        set = nl_langinfo(CODESET);
        만약 (!set || streq(set, "UTF-8"))
                반환 true;

        set = setlocale(LC_CTYPE, NULL);
        만약 (!set)
                반환 true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        반환 STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(void) {
        static 정수 cached = -1;

        만약 (cached < 0)
                cached = is_locale_utf8_impl();

        반환 cached;
}

void locale_variables_free(char *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

void locale_variables_simplify(char *l[_VARIABLE_LC_MAX]) {
        assert(l);

        위해 (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                만약 (p == VARIABLE_LANG)
                        continue;
                만약 (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

static const char * const locale_variable_table[_VARIABLE_LC_MAX] = {
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
