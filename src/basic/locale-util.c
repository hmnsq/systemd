/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <langinfo.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/mman.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "locale-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "set.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "utf8.h"

স্থির অক্ষর* normalize_locale(ধ্রুবক অক্ষর *name) {
        ধ্রুবক অক্ষর *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        যদি (e) {
                _cleanup_free_ অক্ষর *prefix = ফাঁকা;

                prefix = strndup(name, e - name);
                যদি (!prefix)
                        ফেরত ফাঁকা;

                ফেরত strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        যদি (e) {
                _cleanup_free_ অক্ষর *prefix = ফাঁকা;

                prefix = strndup(name, e - name);
                যদি (!prefix)
                        ফেরত ফাঁকা;

                ফেরত strjoin(prefix, ".UTF-8@", e + 6);
        }

        ফেরত strdup(name);
}

#ifdef __GLIBC__
স্থির ধ্রুবক অক্ষর* get_locale_dir(শূন্য) {
        ফেরত secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

স্থির পূর্ণসংখ্যা add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        কাঠামো locarhead {
                uint32_t magic;
                /* Serial number.  */
                uint32_t serial;
                /* Name hash table.  */
                uint32_t namehash_offset;
                uint32_t namehash_used;
                uint32_t namehash_আকার;
                /* String table.  */
                uint32_t string_offset;
                uint32_t string_used;
                uint32_t string_আকার;
                /* Table with locale records.  */
                uint32_t locrectab_offset;
                uint32_t locrectab_used;
                uint32_t locrectab_আকার;
                /* MD5 sum hash table.  */
                uint32_t sumhash_offset;
                uint32_t sumhash_used;
                uint32_t sumhash_আকার;
        };

        কাঠামো namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(locales);

        _cleanup_free_ অক্ষর *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        যদি (!locale_archive_file)
                ফেরত -ENOMEM;

        _cleanup_close_ পূর্ণসংখ্যা fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        যদি (fd < 0)
                ফেরত errno == ENOENT ? 0 : -errno;

        কাঠামো stat st;
        যদি (fstat(fd, &st) < 0)
                ফেরত -errno;

        r = stat_verify_regular(&st);
        যদি (r < 0)
                ফেরত r;

        যদি (st.st_size < (off_t) আকার(কাঠামো locarhead))
                ফেরত -EBADMSG;

        যদি (file_offset_beyond_memory_আকার(st.st_size))
                ফেরত -EFBIG;

        শূন্য *p = mmap(ফাঁকা, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        যদি (p == MAP_FAILED)
                ফেরত -errno;

        ধ্রুবক কাঠামো namehashent *e;
        ধ্রুবক কাঠামো locarhead *h = p;
        যদি (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_আকার > st.st_size ||
            h->string_offset + h->string_আকার > st.st_size ||
            h->locrectab_offset + h->locrectab_আকার > st.st_size ||
            h->sumhash_offset + h->sumhash_আকার > st.st_size) {
                r = -EBADMSG;
                যাও finish;
        }

        e = (ধ্রুবক কাঠামো namehashent*) ((ধ্রুবক uint8_t*) p + h->namehash_offset);
        জন্য (size_t i = 0; i < h->namehash_আকার; i++) {
                অক্ষর *z;

                যদি (e[i].locrec_offset == 0)
                        চালু;

                যদি (!utf8_is_valid((অক্ষর*) p + e[i].name_offset))
                        চালু;

                z = normalize_locale((অক্ষর*) p + e[i].name_offset);
                যদি (!z) {
                        r = -ENOMEM;
                        যাও finish;
                }

                r = set_consume(locales, z);
                যদি (r < 0)
                        যাও finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        ফেরত r;
}

স্থির পূর্ণসংখ্যা add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(locales);

        dir = opendir(get_locale_dir());
        যদি (!dir)
                ফেরত errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, ফেরত -errno) {
                অক্ষর *z;

                যদি (de->d_type != DT_DIR)
                        চালু;

                z = normalize_locale(de->d_name);
                যদি (!z)
                        ফেরত -ENOMEM;

                r = set_consume(locales, z);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত 0;
}

#অন্যথা

স্থির ধ্রুবক অক্ষর* ধ্রুবক musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        ফাঁকা
};

স্থির পূর্ণসংখ্যা add_locales_for_musl(Set *locales) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(locales);

        ধ্রুবক অক্ষর *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                যদি (!dir)
                        চালু;

                FOREACH_DIRENT(de, dir, ফেরত -errno) {
                        যদি (de->d_type != DT_REG)
                                চালু;

                        অক্ষর *z = normalize_locale(de->d_name);
                        যদি (!z)
                                ফেরত -ENOMEM;

                        r = set_consume(locales, z);
                        যদি (r < 0)
                                ফেরত r;
                }
        }

        ফেরত 0;
}
#endif

পূর্ণসংখ্যা get_locales(অক্ষর ***ret) {
        _cleanup_set_free_ Set *locales = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        locales = set_new(&string_hash_ops_free);
        যদি (!locales)
                ফেরত -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        যদি (r < 0 && r != -ENOENT)
                ফেরত r;

        r = add_locales_from_libdir(locales);
        যদি (r < 0)
                ফেরত r;
#অন্যথা
        r = add_locales_for_musl(locales);
        যদি (r < 0)
                ফেরত r;
#endif

        অক্ষর *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        মুক্ত(set_remove(locales, locale));
        }

        _cleanup_strv_free_ অক্ষর **l = set_to_strv(&locales);
        যদি (!l)
                ফেরত -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        যদি (r <= 0) {
                যদি (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                অক্ষর **b = l;
                STRV_FOREACH(a, l)
                        যদি (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        অন্যথা
                                মুক্ত(*a);

                *b = ফাঁকা;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        ফেরত 0;
}

bool locale_is_valid(ধ্রুবক অক্ষর *name) {

        যদি (isempty(name))
                ফেরত false;

        যদি (strlen(name) >= 128)
                ফেরত false;

        যদি (!utf8_is_valid(name))
                ফেরত false;

        যদি (!filename_is_valid(name))
                ফেরত false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        যদি (!in_charset(name, ALPHANUMERICAL "_.-@"))
                ফেরত false;

        ফেরত true;
}

পূর্ণসংখ্যা locale_is_installed(ধ্রুবক অক্ষর *name) {
        যদি (!locale_is_valid(name))
                ফেরত false;

        যদি (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                ফেরত true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        যদি (loc == (locale_t) 0)
                ফেরত errno == ENOMEM ? -ENOMEM : false;

        ফেরত true;
#অন্যথা
        /* musl also has C.UTF-8 as builtin */
        যদি (streq(name, "C.UTF-8"))
                ফেরত true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ধ্রুবক অক্ষর *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ অক্ষর *p = path_join(*d, name);
                যদি (!p)
                        ফেরত -ENOMEM;

                যদি (access(p, F_OK) >= 0)
                        ফেরত true;
        }

        ফেরত false;
#endif
}

স্থির bool is_locale_utf8_impl(শূন্য) {
        ধ্রুবক অক্ষর *set;
        পূর্ণসংখ্যা r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        যদি (r >= 0)
                ফেরত r;
        যদি (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        যদি (!is_main_thread())
                ফেরত true;

        যদি (!setlocale(LC_ALL, ""))
                ফেরত true;

        set = nl_langinfo(CODESET);
        যদি (!set || streq(set, "UTF-8"))
                ফেরত true;

        set = setlocale(LC_CTYPE, ফাঁকা);
        যদি (!set)
                ফেরত true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        ফেরত STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(শূন্য) {
        স্থির পূর্ণসংখ্যা cached = -1;

        যদি (cached < 0)
                cached = is_locale_utf8_impl();

        ফেরত cached;
}

শূন্য locale_variables_free(অক্ষর *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

শূন্য locale_variables_simplify(অক্ষর *l[_VARIABLE_LC_MAX]) {
        প্রতিজ্ঞা(l);

        জন্য (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                যদি (p == VARIABLE_LANG)
                        চালু;
                যদি (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

স্থির ধ্রুবক অক্ষর * ধ্রুবক locale_variable_table[_VARIABLE_LC_MAX] = {
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
