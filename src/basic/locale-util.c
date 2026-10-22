/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <langinfo.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/mman.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "locale-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "set.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

ସ୍ଥିର ଅକ୍ଷର* normalize_locale(ଅଚଳ ଅକ୍ଷର *name) {
        ଅଚଳ ଅକ୍ଷର *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        ଯଦି (e) {
                _cleanup_free_ ଅକ୍ଷର *prefix = ଖାଲି;

                prefix = strndup(name, e - name);
                ଯଦି (!prefix)
                        ଫେରନ୍ତୁ ଖାଲି;

                ଫେରନ୍ତୁ strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        ଯଦି (e) {
                _cleanup_free_ ଅକ୍ଷର *prefix = ଖାଲି;

                prefix = strndup(name, e - name);
                ଯଦି (!prefix)
                        ଫେରନ୍ତୁ ଖାଲି;

                ଫେରନ୍ତୁ strjoin(prefix, ".UTF-8@", e + 6);
        }

        ଫେରନ୍ତୁ strdup(name);
}

#ifdef __GLIBC__
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* get_locale_dir(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        ସଂରଚନା locarhead {
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

        ସଂରଚନା namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(locales);

        _cleanup_free_ ଅକ୍ଷର *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        ଯଦି (!locale_archive_file)
                ଫେରନ୍ତୁ -ENOMEM;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ errno == ENOENT ? 0 : -errno;

        ସଂରଚନା stat st;
        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        r = stat_verify_regular(&st);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (st.st_size < (off_t) ଆକାର(ସଂରଚନା locarhead))
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (file_offset_beyond_memory_size(st.st_size))
                ଫେରନ୍ତୁ -EFBIG;

        ଶୂନ୍ୟ *p = mmap(ଖାଲି, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        ଯଦି (p == MAP_FAILED)
                ଫେରନ୍ତୁ -errno;

        ଅଚଳ ସଂରଚନା namehashent *e;
        ଅଚଳ ସଂରଚନା locarhead *h = p;
        ଯଦି (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                ଯାଅ finish;
        }

        e = (ଅଚଳ ସଂରଚନା namehashent*) ((ଅଚଳ uint8_t*) p + h->namehash_offset);
        ପାଇଁ (size_t i = 0; i < h->namehash_size; i++) {
                ଅକ୍ଷର *z;

                ଯଦି (e[i].locrec_offset == 0)
                        ଜାରି;

                ଯଦି (!utf8_is_valid((ଅକ୍ଷର*) p + e[i].name_offset))
                        ଜାରି;

                z = normalize_locale((ଅକ୍ଷର*) p + e[i].name_offset);
                ଯଦି (!z) {
                        r = -ENOMEM;
                        ଯାଅ finish;
                }

                r = set_consume(locales, z);
                ଯଦି (r < 0)
                        ଯାଅ finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(locales);

        dir = opendir(get_locale_dir());
        ଯଦି (!dir)
                ଫେରନ୍ତୁ errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, ଫେରନ୍ତୁ -errno) {
                ଅକ୍ଷର *z;

                ଯଦି (de->d_type != DT_DIR)
                        ଜାରି;

                z = normalize_locale(de->d_name);
                ଯଦି (!z)
                        ଫେରନ୍ତୁ -ENOMEM;

                r = set_consume(locales, z);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

#ଅନ୍ୟଥା

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        ଖାଲି
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_locales_for_musl(Set *locales) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(locales);

        ଅଚଳ ଅକ୍ଷର *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                ଯଦି (!dir)
                        ଜାରି;

                FOREACH_DIRENT(de, dir, ଫେରନ୍ତୁ -errno) {
                        ଯଦି (de->d_type != DT_REG)
                                ଜାରି;

                        ଅକ୍ଷର *z = normalize_locale(de->d_name);
                        ଯଦି (!z)
                                ଫେରନ୍ତୁ -ENOMEM;

                        r = set_consume(locales, z);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }
        }

        ଫେରନ୍ତୁ 0;
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_locales(ଅକ୍ଷର ***ret) {
        _cleanup_set_free_ Set *locales = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        locales = set_new(&string_hash_ops_free);
        ଯଦି (!locales)
                ଫେରନ୍ତୁ -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        ଯଦି (r < 0 && r != -ENOENT)
                ଫେରନ୍ତୁ r;

        r = add_locales_from_libdir(locales);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
#ଅନ୍ୟଥା
        r = add_locales_for_musl(locales);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
#endif

        ଅକ୍ଷର *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ମୁକ୍ତ(set_remove(locales, locale));
        }

        _cleanup_strv_free_ ଅକ୍ଷର **l = set_to_strv(&locales);
        ଯଦି (!l)
                ଫେରନ୍ତୁ -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        ଯଦି (r <= 0) {
                ଯଦି (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                ଅକ୍ଷର **b = l;
                STRV_FOREACH(a, l)
                        ଯଦି (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        ଅନ୍ୟଥା
                                ମୁକ୍ତ(*a);

                *b = ଖାଲି;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        ଫେରନ୍ତୁ 0;
}

bool locale_is_valid(ଅଚଳ ଅକ୍ଷର *name) {

        ଯଦି (isempty(name))
                ଫେରନ୍ତୁ false;

        ଯଦି (strlen(name) >= 128)
                ଫେରନ୍ତୁ false;

        ଯଦି (!utf8_is_valid(name))
                ଫେରନ୍ତୁ false;

        ଯଦି (!filename_is_valid(name))
                ଫେରନ୍ତୁ false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        ଯଦି (!in_charset(name, ALPHANUMERICAL "_.-@"))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା locale_is_installed(ଅଚଳ ଅକ୍ଷର *name) {
        ଯଦି (!locale_is_valid(name))
                ଫେରନ୍ତୁ false;

        ଯଦି (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                ଫେରନ୍ତୁ true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        ଯଦି (loc == (locale_t) 0)
                ଫେରନ୍ତୁ errno == ENOMEM ? -ENOMEM : false;

        ଫେରନ୍ତୁ true;
#ଅନ୍ୟଥା
        /* musl also has C.UTF-8 as builtin */
        ଯଦି (streq(name, "C.UTF-8"))
                ଫେରନ୍ତୁ true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ଅଚଳ ଅକ୍ଷର *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ ଅକ୍ଷର *p = path_join(*d, name);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (access(p, F_OK) >= 0)
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
#endif
}

ସ୍ଥିର bool is_locale_utf8_impl(ଶୂନ୍ୟ) {
        ଅଚଳ ଅକ୍ଷର *set;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        ଯଦି (!is_main_thread())
                ଫେରନ୍ତୁ true;

        ଯଦି (!setlocale(LC_ALL, ""))
                ଫେରନ୍ତୁ true;

        set = nl_langinfo(CODESET);
        ଯଦି (!set || streq(set, "UTF-8"))
                ଫେରନ୍ତୁ true;

        set = setlocale(LC_CTYPE, ଖାଲି);
        ଯଦି (!set)
                ଫେରନ୍ତୁ true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        ଫେରନ୍ତୁ STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(ଶୂନ୍ୟ) {
        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached = -1;

        ଯଦି (cached < 0)
                cached = is_locale_utf8_impl();

        ଫେରନ୍ତୁ cached;
}

ଶୂନ୍ୟ locale_variables_free(ଅକ୍ଷର *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

ଶୂନ୍ୟ locale_variables_simplify(ଅକ୍ଷର *l[_VARIABLE_LC_MAX]) {
        ପ୍ରତିଜ୍ଞା(l);

        ପାଇଁ (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                ଯଦି (p == VARIABLE_LANG)
                        ଜାରି;
                ଯଦି (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ locale_variable_table[_VARIABLE_LC_MAX] = {
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
