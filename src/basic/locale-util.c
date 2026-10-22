/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <langinfo.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/mman.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "locale-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "set.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "utf8.h"

સ્થિર અક્ષર* normalize_locale(અચલ અક્ષર *name) {
        અચલ અક્ષર *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        જો (e) {
                _cleanup_free_ અક્ષર *prefix = ખાલી;

                prefix = strndup(name, e - name);
                જો (!prefix)
                        પાછા ખાલી;

                પાછા strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        જો (e) {
                _cleanup_free_ અક્ષર *prefix = ખાલી;

                prefix = strndup(name, e - name);
                જો (!prefix)
                        પાછા ખાલી;

                પાછા strjoin(prefix, ".UTF-8@", e + 6);
        }

        પાછા strdup(name);
}

#ifdef __GLIBC__
સ્થિર અચલ અક્ષર* get_locale_dir(શૂન્ય) {
        પાછા secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

સ્થિર પૂર્ણાંક add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        સંરચના locarhead {
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

        સંરચના namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(locales);

        _cleanup_free_ અક્ષર *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        જો (!locale_archive_file)
                પાછા -ENOMEM;

        _cleanup_close_ પૂર્ણાંક fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        જો (fd < 0)
                પાછા errno == ENOENT ? 0 : -errno;

        સંરચના stat st;
        જો (fstat(fd, &st) < 0)
                પાછા -errno;

        r = stat_verify_regular(&st);
        જો (r < 0)
                પાછા r;

        જો (st.st_size < (off_t) કદ(સંરચના locarhead))
                પાછા -EBADMSG;

        જો (file_offset_beyond_memory_size(st.st_size))
                પાછા -EFBIG;

        શૂન્ય *p = mmap(ખાલી, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        જો (p == MAP_FAILED)
                પાછા -errno;

        અચલ સંરચના namehashent *e;
        અચલ સંરચના locarhead *h = p;
        જો (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                જાઓ finish;
        }

        e = (અચલ સંરચના namehashent*) ((અચલ uint8_t*) p + h->namehash_offset);
        માટે (size_t i = 0; i < h->namehash_size; i++) {
                અક્ષર *z;

                જો (e[i].locrec_offset == 0)
                        ચાલુ;

                જો (!utf8_is_valid((અક્ષર*) p + e[i].name_offset))
                        ચાલુ;

                z = normalize_locale((અક્ષર*) p + e[i].name_offset);
                જો (!z) {
                        r = -ENOMEM;
                        જાઓ finish;
                }

                r = set_consume(locales, z);
                જો (r < 0)
                        જાઓ finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        પાછા r;
}

સ્થિર પૂર્ણાંક add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(locales);

        dir = opendir(get_locale_dir());
        જો (!dir)
                પાછા errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, પાછા -errno) {
                અક્ષર *z;

                જો (de->d_type != DT_DIR)
                        ચાલુ;

                z = normalize_locale(de->d_name);
                જો (!z)
                        પાછા -ENOMEM;

                r = set_consume(locales, z);
                જો (r < 0)
                        પાછા r;
        }

        પાછા 0;
}

#નહીંતર

સ્થિર અચલ અક્ષર* અચલ musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        ખાલી
};

સ્થિર પૂર્ણાંક add_locales_for_musl(Set *locales) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(locales);

        અચલ અક્ષર *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                જો (!dir)
                        ચાલુ;

                FOREACH_DIRENT(de, dir, પાછા -errno) {
                        જો (de->d_type != DT_REG)
                                ચાલુ;

                        અક્ષર *z = normalize_locale(de->d_name);
                        જો (!z)
                                પાછા -ENOMEM;

                        r = set_consume(locales, z);
                        જો (r < 0)
                                પાછા r;
                }
        }

        પાછા 0;
}
#endif

પૂર્ણાંક get_locales(અક્ષર ***ret) {
        _cleanup_set_free_ Set *locales = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        locales = set_new(&string_hash_ops_free);
        જો (!locales)
                પાછા -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        જો (r < 0 && r != -ENOENT)
                પાછા r;

        r = add_locales_from_libdir(locales);
        જો (r < 0)
                પાછા r;
#નહીંતર
        r = add_locales_for_musl(locales);
        જો (r < 0)
                પાછા r;
#endif

        અક્ષર *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        મુક્ત(set_remove(locales, locale));
        }

        _cleanup_strv_free_ અક્ષર **l = set_to_strv(&locales);
        જો (!l)
                પાછા -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        જો (r <= 0) {
                જો (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                અક્ષર **b = l;
                STRV_FOREACH(a, l)
                        જો (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        નહીંતર
                                મુક્ત(*a);

                *b = ખાલી;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        પાછા 0;
}

bool locale_is_valid(અચલ અક્ષર *name) {

        જો (isempty(name))
                પાછા false;

        જો (strlen(name) >= 128)
                પાછા false;

        જો (!utf8_is_valid(name))
                પાછા false;

        જો (!filename_is_valid(name))
                પાછા false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        જો (!in_charset(name, ALPHANUMERICAL "_.-@"))
                પાછા false;

        પાછા true;
}

પૂર્ણાંક locale_is_installed(અચલ અક્ષર *name) {
        જો (!locale_is_valid(name))
                પાછા false;

        જો (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                પાછા true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        જો (loc == (locale_t) 0)
                પાછા errno == ENOMEM ? -ENOMEM : false;

        પાછા true;
#નહીંતર
        /* musl also has C.UTF-8 as builtin */
        જો (streq(name, "C.UTF-8"))
                પાછા true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        અચલ અક્ષર *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ અક્ષર *p = path_join(*d, name);
                જો (!p)
                        પાછા -ENOMEM;

                જો (access(p, F_OK) >= 0)
                        પાછા true;
        }

        પાછા false;
#endif
}

સ્થિર bool is_locale_utf8_impl(શૂન્ય) {
        અચલ અક્ષર *set;
        પૂર્ણાંક r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        જો (r >= 0)
                પાછા r;
        જો (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        જો (!is_main_thread())
                પાછા true;

        જો (!setlocale(LC_ALL, ""))
                પાછા true;

        set = nl_langinfo(CODESET);
        જો (!set || streq(set, "UTF-8"))
                પાછા true;

        set = setlocale(LC_CTYPE, ખાલી);
        જો (!set)
                પાછા true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        પાછા STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(શૂન્ય) {
        સ્થિર પૂર્ણાંક cached = -1;

        જો (cached < 0)
                cached = is_locale_utf8_impl();

        પાછા cached;
}

શૂન્ય locale_variables_free(અક્ષર *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

શૂન્ય locale_variables_simplify(અક્ષર *l[_VARIABLE_LC_MAX]) {
        પ્રતિજ્ઞા(l);

        માટે (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                જો (p == VARIABLE_LANG)
                        ચાલુ;
                જો (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

સ્થિર અચલ અક્ષર * અચલ locale_variable_table[_VARIABLE_LC_MAX] = {
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
