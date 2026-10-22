/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <langinfo.h>
#شامل <stdlib.h>
#شامل <sys/mman.h>
#شامل <sys/stat.h>
#شامل <unistd.h>

#شامل "dirent-util.h"
#شامل "env-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "locale-util.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "path-util.h"
#شامل "process-util.h"
#شامل "set.h"
#شامل "stat-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "utf8.h"

ایستا نویسه* normalize_locale(ثابت نویسه *name) {
        ثابت نویسه *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        اگر (e) {
                _cleanup_free_ نویسه *prefix = پوچ;

                prefix = strndup(name, e - name);
                اگر (!prefix)
                        بازگشت پوچ;

                بازگشت strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        اگر (e) {
                _cleanup_free_ نویسه *prefix = پوچ;

                prefix = strndup(name, e - name);
                اگر (!prefix)
                        بازگشت پوچ;

                بازگشت strjoin(prefix, ".UTF-8@", e + 6);
        }

        بازگشت strdup(name);
}

#ifdef __GLIBC__
ایستا ثابت نویسه* get_locale_dir(تهی) {
        بازگشت secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

ایستا صحیح add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        ساختار locarhead {
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

        ساختار namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        صحیح r;

        أكد(locales);

        _cleanup_free_ نویسه *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        اگر (!locale_archive_file)
                بازگشت -ENOMEM;

        _cleanup_close_ صحیح fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        اگر (fd < 0)
                بازگشت errno == ENOENT ? 0 : -errno;

        ساختار stat st;
        اگر (fstat(fd, &st) < 0)
                بازگشت -errno;

        r = stat_verify_regular(&st);
        اگر (r < 0)
                بازگشت r;

        اگر (st.st_size < (off_t) اندازه(ساختار locarhead))
                بازگشت -EBADMSG;

        اگر (file_offset_beyond_memory_size(st.st_size))
                بازگشت -EFBIG;

        تهی *p = mmap(پوچ, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        اگر (p == MAP_FAILED)
                بازگشت -errno;

        ثابت ساختار namehashent *e;
        ثابت ساختار locarhead *h = p;
        اگر (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                برو finish;
        }

        e = (ثابت ساختار namehashent*) ((ثابت uint8_t*) p + h->namehash_offset);
        برای (size_t i = 0; i < h->namehash_size; i++) {
                نویسه *z;

                اگر (e[i].locrec_offset == 0)
                        ادامه;

                اگر (!utf8_is_valid((نویسه*) p + e[i].name_offset))
                        ادامه;

                z = normalize_locale((نویسه*) p + e[i].name_offset);
                اگر (!z) {
                        r = -ENOMEM;
                        برو finish;
                }

                r = set_consume(locales, z);
                اگر (r < 0)
                        برو finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        بازگشت r;
}

ایستا صحیح add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = پوچ;
        صحیح r;

        أكد(locales);

        dir = opendir(get_locale_dir());
        اگر (!dir)
                بازگشت errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, بازگشت -errno) {
                نویسه *z;

                اگر (de->d_type != DT_DIR)
                        ادامه;

                z = normalize_locale(de->d_name);
                اگر (!z)
                        بازگشت -ENOMEM;

                r = set_consume(locales, z);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت 0;
}

#وگرنه

ایستا ثابت نویسه* ثابت musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        پوچ
};

ایستا صحیح add_locales_for_musl(Set *locales) {
        صحیح r;

        أكد(locales);

        ثابت نویسه *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                اگر (!dir)
                        ادامه;

                FOREACH_DIRENT(de, dir, بازگشت -errno) {
                        اگر (de->d_type != DT_REG)
                                ادامه;

                        نویسه *z = normalize_locale(de->d_name);
                        اگر (!z)
                                بازگشت -ENOMEM;

                        r = set_consume(locales, z);
                        اگر (r < 0)
                                بازگشت r;
                }
        }

        بازگشت 0;
}
#endif

صحیح get_locales(نویسه ***ret) {
        _cleanup_set_free_ Set *locales = پوچ;
        صحیح r;

        أكد(ret);

        locales = set_new(&string_hash_ops_free);
        اگر (!locales)
                بازگشت -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        اگر (r < 0 && r != -ENOENT)
                بازگشت r;

        r = add_locales_from_libdir(locales);
        اگر (r < 0)
                بازگشت r;
#وگرنه
        r = add_locales_for_musl(locales);
        اگر (r < 0)
                بازگشت r;
#endif

        نویسه *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        آزاد(set_remove(locales, locale));
        }

        _cleanup_strv_free_ نویسه **l = set_to_strv(&locales);
        اگر (!l)
                بازگشت -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        اگر (r <= 0) {
                اگر (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                نویسه **b = l;
                STRV_FOREACH(a, l)
                        اگر (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        وگرنه
                                آزاد(*a);

                *b = پوچ;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        بازگشت 0;
}

bool locale_is_valid(ثابت نویسه *name) {

        اگر (isempty(name))
                بازگشت false;

        اگر (strlen(name) >= 128)
                بازگشت false;

        اگر (!utf8_is_valid(name))
                بازگشت false;

        اگر (!filename_is_valid(name))
                بازگشت false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        اگر (!in_charset(name, ALPHANUMERICAL "_.-@"))
                بازگشت false;

        بازگشت true;
}

صحیح locale_is_installed(ثابت نویسه *name) {
        اگر (!locale_is_valid(name))
                بازگشت false;

        اگر (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                بازگشت true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        اگر (loc == (locale_t) 0)
                بازگشت errno == ENOMEM ? -ENOMEM : false;

        بازگشت true;
#وگرنه
        /* musl also has C.UTF-8 as builtin */
        اگر (streq(name, "C.UTF-8"))
                بازگشت true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ثابت نویسه *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ نویسه *p = path_join(*d, name);
                اگر (!p)
                        بازگشت -ENOMEM;

                اگر (access(p, F_OK) >= 0)
                        بازگشت true;
        }

        بازگشت false;
#endif
}

ایستا bool is_locale_utf8_impl(تهی) {
        ثابت نویسه *set;
        صحیح r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        اگر (r >= 0)
                بازگشت r;
        اگر (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        اگر (!is_main_thread())
                بازگشت true;

        اگر (!setlocale(LC_ALL, ""))
                بازگشت true;

        set = nl_langinfo(CODESET);
        اگر (!set || streq(set, "UTF-8"))
                بازگشت true;

        set = setlocale(LC_CTYPE, پوچ);
        اگر (!set)
                بازگشت true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        بازگشت STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(تهی) {
        ایستا صحیح cached = -1;

        اگر (cached < 0)
                cached = is_locale_utf8_impl();

        بازگشت cached;
}

تهی locale_variables_free(نویسه *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

تهی locale_variables_simplify(نویسه *l[_VARIABLE_LC_MAX]) {
        أكد(l);

        برای (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                اگر (p == VARIABLE_LANG)
                        ادامه;
                اگر (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

ایستا ثابت نویسه * ثابت locale_variable_table[_VARIABLE_LC_MAX] = {
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
