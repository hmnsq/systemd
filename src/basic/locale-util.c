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

ساکن حرف* normalize_locale(ثابت حرف *name) {
        ثابت حرف *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        اگر (e) {
                _cleanup_free_ حرف *سابقہ = تہی;

                سابقہ = strndup(name, e - name);
                اگر (!سابقہ)
                        واپس تہی;

                واپس strjoin(سابقہ, ".UTF-8");
        }

        e = لڑی_تلاش(name, ".utf8@");
        اگر (e) {
                _cleanup_free_ حرف *سابقہ = تہی;

                سابقہ = strndup(name, e - name);
                اگر (!سابقہ)
                        واپس تہی;

                واپس strjoin(سابقہ, ".UTF-8@", e + 6);
        }

        واپس strdup(name);
}

#اگر_تعریف_شدہ __GLIBC__
ساکن ثابت حرف* get_locale_dir(خالی) {
        واپس secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

ساکن صحیح جوڑو_locales_from_archive(سیٹ_2 *locales) {
        /* Stolen from glibc... */

        ساخت locarhead {
                uint32_t magic;
                /* Serial number.  */
                uint32_t serial;
                /* Name hash table.  */
                uint32_t namehash_انحراف;
                uint32_t namehash_مستعمل;
                uint32_t namehash_سائز;
                /* String table.  */
                uint32_t string_انحراف;
                uint32_t string_مستعمل;
                uint32_t string_سائز;
                /* Table with locale records.  */
                uint32_t locrectab_انحراف;
                uint32_t locrectab_مستعمل;
                uint32_t locrectab_سائز;
                /* MD5 sum hash table.  */
                uint32_t sumhash_انحراف;
                uint32_t sumhash_مستعمل;
                uint32_t sumhash_سائز;
        };

        ساخت namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_انحراف;
                /* Offset of the locale record.  */
                uint32_t locrec_انحراف;
        };

        صحیح r;

        أكد(locales);

        _cleanup_free_ حرف *locale_archive_file = راستہ_join(get_locale_dir(), "locale-archive");
        اگر (!locale_archive_file)
                واپس -ENOMEM;

        _cleanup_close_ صحیح fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        اگر (fd < 0)
                واپس errno == ENOENT ? 0 : -errno;

        ساخت stat st;
        اگر (fstat(fd, &st) < 0)
                واپس -errno;

        r = stat_verify_regular(&st);
        اگر (r < 0)
                واپس r;

        اگر (st.st_size < (off_t) حجم(ساخت locarhead))
                واپس -EBADMSG;

        اگر (file_انحراف_beyond_حافظہ_سائز(st.st_size))
                واپس -EFBIG;

        خالی *p = mmap(تہی, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        اگر (p == MAP_FAILED)
                واپس -errno;

        ثابت ساخت namehashent *e;
        ثابت ساخت locarhead *h = p;
        اگر (h->magic != 0xde020109 ||
            h->namehash_انحراف + h->namehash_سائز > st.st_size ||
            h->string_انحراف + h->string_سائز > st.st_size ||
            h->locrectab_انحراف + h->locrectab_سائز > st.st_size ||
            h->sumhash_انحراف + h->sumhash_سائز > st.st_size) {
                r = -EBADMSG;
                جاؤ اختتام;
        }

        e = (ثابت ساخت namehashent*) ((ثابت uint8_t*) p + h->namehash_انحراف);
        برائے (size_t i = 0; i < h->namehash_سائز; i++) {
                حرف *z;

                اگر (e[i].locrec_انحراف == 0)
                        جاری;

                اگر (!utf8_is_valid((حرف*) p + e[i].name_انحراف))
                        جاری;

                z = normalize_locale((حرف*) p + e[i].name_انحراف);
                اگر (!z) {
                        r = -ENOMEM;
                        جاؤ اختتام;
                }

                r = سیٹ_consume(locales, z);
                اگر (r < 0)
                        جاؤ اختتام;
        }

        r = 0;

اختتام:
        munmap_محفوظ(p, st.st_size);
        واپس r;
}

ساکن صحیح جوڑو_locales_from_libdir(سیٹ_2 *locales) {
        _cleanup_closedir_ DIR *dir = تہی;
        صحیح r;

        أكد(locales);

        dir = opendir(get_locale_dir());
        اگر (!dir)
                واپس errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, واپس -errno) {
                حرف *z;

                اگر (de->d_type != DT_DIR)
                        جاری;

                z = normalize_locale(de->d_name);
                اگر (!z)
                        واپس -ENOMEM;

                r = سیٹ_consume(locales, z);
                اگر (r < 0)
                        واپس r;
        }

        واپس 0;
}

#ورنہ

ساکن ثابت حرف* ثابت musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        تہی
};

ساکن صحیح جوڑو_locales_for_musl(سیٹ_2 *locales) {
        صحیح r;

        أكد(locales);

        ثابت حرف *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                اگر (!dir)
                        جاری;

                FOREACH_DIRENT(de, dir, واپس -errno) {
                        اگر (de->d_type != DT_REG)
                                جاری;

                        حرف *z = normalize_locale(de->d_name);
                        اگر (!z)
                                واپس -ENOMEM;

                        r = سیٹ_consume(locales, z);
                        اگر (r < 0)
                                واپس r;
                }
        }

        واپس 0;
}
#اگر_ختم

صحیح get_locales(حرف ***ret) {
        cleanup_سیٹ_free سیٹ_2 *locales = تہی;
        صحیح r;

        أكد(ret);

        locales = سیٹ_new(&string_hash_ops_free);
        اگر (!locales)
                واپس -ENOMEM;

#اگر_تعریف_شدہ __GLIBC__
        r = جوڑو_locales_from_archive(locales);
        اگر (r < 0 && r != -ENOENT)
                واپس r;

        r = جوڑو_locales_from_libdir(locales);
        اگر (r < 0)
                واپس r;
#ورنہ
        r = جوڑو_locales_for_musl(locales);
        اگر (r < 0)
                واپس r;
#اگر_ختم

        حرف *locale;
        سیٹ_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        آزاد(سیٹ_remove(locales, locale));
        }

        _cleanup_strv_free_ حرف **l = سیٹ_مقصد_strv(&locales);
        اگر (!l)
                واپس -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        اگر (r <= 0) {
                اگر (!اندر_سیٹ(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                حرف **b = l;
                STRV_FOREACH(a, l)
                        اگر (endswith(*a, "UTF-8") || لڑی_تلاش(*a, ".UTF-8@"))
                                *(b++) = *a;
                        ورنہ
                                آزاد(*a);

                *b = تہی;
        }

        strv_ترتیب_دو(l);

        *ret = TAKE_PTR(l);

        واپس 0;
}

bool locale_is_valid(ثابت حرف *name) {

        اگر (isempty(name))
                واپس false;

        اگر (لڑی_لمبائی(name) >= 128)
                واپس false;

        اگر (!utf8_is_valid(name))
                واپس false;

        اگر (!filename_is_valid(name))
                واپس false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        اگر (!اندر_charset(name, ALPHANUMERICAL "_.-@"))
                واپس false;

        واپس true;
}

صحیح locale_is_installed(ثابت حرف *name) {
        اگر (!locale_is_valid(name))
                واپس false;

        اگر (STR_اندر_سیٹ(name, "C", "POSIX")) /* These ones are always OK */
                واپس true;

#اگر_تعریف_شدہ __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        اگر (loc == (locale_t) 0)
                واپس errno == ENOMEM ? -ENOMEM : false;

        واپس true;
#ورنہ
        /* musl also has C.UTF-8 as builtin */
        اگر (streq(name, "C.UTF-8"))
                واپس true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ثابت حرف *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ حرف *p = راستہ_join(*d, name);
                اگر (!p)
                        واپس -ENOMEM;

                اگر (access(p, F_OK) >= 0)
                        واپس true;
        }

        واپس false;
#اگر_ختم
}

ساکن bool is_locale_utf8_impl(خالی) {
        ثابت حرف *سیٹ;
        صحیح r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        اگر (r >= 0)
                واپس r;
        اگر (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        اگر (!is_main_thread())
                واپس true;

        اگر (!setlocale(LC_ALL, ""))
                واپس true;

        سیٹ = nl_langinfo(CODESET);
        اگر (!سیٹ || streq(سیٹ, "UTF-8"))
                واپس true;

        سیٹ = setlocale(LC_CTYPE, تہی);
        اگر (!سیٹ)
                واپس true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        واپس STR_اندر_سیٹ(سیٹ, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(خالی) {
        ساکن صحیح cached = -1;

        اگر (cached < 0)
                cached = is_locale_utf8_impl();

        واپس cached;
}

خالی locale_variables_free(حرف *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

خالی locale_variables_simplify(حرف *l[_VARIABLE_LC_MAX]) {
        أكد(l);

        برائے (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                اگر (p == VARIABLE_LANG)
                        جاری;
                اگر (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

ساکن ثابت حرف * ثابت locale_variable_جدول[_VARIABLE_LC_MAX] = {
        [VARIABLE_LANG]              = "LANG",
        [VARIABLE_LANGUAGE]          = "LANGUAGE",
        [VARIABLE_LC_CTYPE]          = "LC_CTYPE",
        [VARIABLE_LC_NUMERIC]        = "LC_NUMERIC",
        [VARIABLE_LC_وقت]           = "LC_TIME",
        [VARIABLE_LC_COLLATE]        = "LC_COLLATE",
        [VARIABLE_LC_MONETARY]       = "LC_MONETARY",
        [VARIABLE_LC_MESSAGES]       = "LC_MESSAGES",
        [VARIABLE_LC_PAPER]          = "LC_PAPER",
        [VARIABLE_LC_NAME]           = "LC_NAME",
        [VARIABLE_LC_پتہ]        = "LC_ADDRESS",
        [VARIABLE_LC_TELEPHONE]      = "LC_TELEPHONE",
        [VARIABLE_LC_MEASUREMENT]    = "LC_MEASUREMENT",
        [VARIABLE_LC_IDENTIFICATION] = "LC_IDENTIFICATION"
};

DEFINE_STRING_جدول_LOOKUP(locale_variable, LocaleVariable);
