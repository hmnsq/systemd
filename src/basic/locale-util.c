/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <langinfo.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/mman.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "locale-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "set.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "utf8.h"

நிலையான எழுத்து* normalize_locale(மாறிலி எழுத்து *name) {
        மாறிலி எழுத்து *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        எனில் (e) {
                _cleanup_free_ எழுத்து *prefix = வெற்று;

                prefix = strndup(name, e - name);
                எனில் (!prefix)
                        திரும்பு வெற்று;

                திரும்பு strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        எனில் (e) {
                _cleanup_free_ எழுத்து *prefix = வெற்று;

                prefix = strndup(name, e - name);
                எனில் (!prefix)
                        திரும்பு வெற்று;

                திரும்பு strjoin(prefix, ".UTF-8@", e + 6);
        }

        திரும்பு strdup(name);
}

#ifdef __GLIBC__
நிலையான மாறிலி எழுத்து* get_locale_dir(வெறுமை) {
        திரும்பு secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

நிலையான முழு add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        கட்டமைப்பு locarhead {
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

        கட்டமைப்பு namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        முழு r;

        ப்ரதிஜ்ஞா(locales);

        _cleanup_free_ எழுத்து *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        எனில் (!locale_archive_file)
                திரும்பு -ENOMEM;

        _cleanup_close_ முழு fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு errno == ENOENT ? 0 : -errno;

        கட்டமைப்பு stat st;
        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        r = stat_verify_regular(&st);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (st.st_size < (off_t) அளவு(கட்டமைப்பு locarhead))
                திரும்பு -EBADMSG;

        எனில் (file_offset_beyond_memory_size(st.st_size))
                திரும்பு -EFBIG;

        வெறுமை *p = mmap(வெற்று, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        எனில் (p == MAP_FAILED)
                திரும்பு -errno;

        மாறிலி கட்டமைப்பு namehashent *e;
        மாறிலி கட்டமைப்பு locarhead *h = p;
        எனில் (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                செல் finish;
        }

        e = (மாறிலி கட்டமைப்பு namehashent*) ((மாறிலி uint8_t*) p + h->namehash_offset);
        க்கு (size_t i = 0; i < h->namehash_size; i++) {
                எழுத்து *z;

                எனில் (e[i].locrec_offset == 0)
                        தொடர்;

                எனில் (!utf8_is_valid((எழுத்து*) p + e[i].name_offset))
                        தொடர்;

                z = normalize_locale((எழுத்து*) p + e[i].name_offset);
                எனில் (!z) {
                        r = -ENOMEM;
                        செல் finish;
                }

                r = set_consume(locales, z);
                எனில் (r < 0)
                        செல் finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        திரும்பு r;
}

நிலையான முழு add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(locales);

        dir = opendir(get_locale_dir());
        எனில் (!dir)
                திரும்பு errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, திரும்பு -errno) {
                எழுத்து *z;

                எனில் (de->d_type != DT_DIR)
                        தொடர்;

                z = normalize_locale(de->d_name);
                எனில் (!z)
                        திரும்பு -ENOMEM;

                r = set_consume(locales, z);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு 0;
}

#இல்லையெனில்

நிலையான மாறிலி எழுத்து* மாறிலி musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        வெற்று
};

நிலையான முழு add_locales_for_musl(Set *locales) {
        முழு r;

        ப்ரதிஜ்ஞா(locales);

        மாறிலி எழுத்து *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                எனில் (!dir)
                        தொடர்;

                FOREACH_DIRENT(de, dir, திரும்பு -errno) {
                        எனில் (de->d_type != DT_REG)
                                தொடர்;

                        எழுத்து *z = normalize_locale(de->d_name);
                        எனில் (!z)
                                திரும்பு -ENOMEM;

                        r = set_consume(locales, z);
                        எனில் (r < 0)
                                திரும்பு r;
                }
        }

        திரும்பு 0;
}
#endif

முழு get_locales(எழுத்து ***ret) {
        _cleanup_set_free_ Set *locales = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        locales = set_new(&string_hash_ops_free);
        எனில் (!locales)
                திரும்பு -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        எனில் (r < 0 && r != -ENOENT)
                திரும்பு r;

        r = add_locales_from_libdir(locales);
        எனில் (r < 0)
                திரும்பு r;
#இல்லையெனில்
        r = add_locales_for_musl(locales);
        எனில் (r < 0)
                திரும்பு r;
#endif

        எழுத்து *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        விடுவி(set_remove(locales, locale));
        }

        _cleanup_strv_free_ எழுத்து **l = set_to_strv(&locales);
        எனில் (!l)
                திரும்பு -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        எனில் (r <= 0) {
                எனில் (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                எழுத்து **b = l;
                STRV_FOREACH(a, l)
                        எனில் (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        இல்லையெனில்
                                விடுவி(*a);

                *b = வெற்று;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        திரும்பு 0;
}

bool locale_is_valid(மாறிலி எழுத்து *name) {

        எனில் (isempty(name))
                திரும்பு false;

        எனில் (strlen(name) >= 128)
                திரும்பு false;

        எனில் (!utf8_is_valid(name))
                திரும்பு false;

        எனில் (!filename_is_valid(name))
                திரும்பு false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        எனில் (!in_charset(name, ALPHANUMERICAL "_.-@"))
                திரும்பு false;

        திரும்பு true;
}

முழு locale_is_installed(மாறிலி எழுத்து *name) {
        எனில் (!locale_is_valid(name))
                திரும்பு false;

        எனில் (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                திரும்பு true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        எனில் (loc == (locale_t) 0)
                திரும்பு errno == ENOMEM ? -ENOMEM : false;

        திரும்பு true;
#இல்லையெனில்
        /* musl also has C.UTF-8 as builtin */
        எனில் (streq(name, "C.UTF-8"))
                திரும்பு true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        மாறிலி எழுத்து *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ எழுத்து *p = path_join(*d, name);
                எனில் (!p)
                        திரும்பு -ENOMEM;

                எனில் (access(p, F_OK) >= 0)
                        திரும்பு true;
        }

        திரும்பு false;
#endif
}

நிலையான bool is_locale_utf8_impl(வெறுமை) {
        மாறிலி எழுத்து *set;
        முழு r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        எனில் (r >= 0)
                திரும்பு r;
        எனில் (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        எனில் (!is_main_thread())
                திரும்பு true;

        எனில் (!setlocale(LC_ALL, ""))
                திரும்பு true;

        set = nl_langinfo(CODESET);
        எனில் (!set || streq(set, "UTF-8"))
                திரும்பு true;

        set = setlocale(LC_CTYPE, வெற்று);
        எனில் (!set)
                திரும்பு true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        திரும்பு STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(வெறுமை) {
        நிலையான முழு cached = -1;

        எனில் (cached < 0)
                cached = is_locale_utf8_impl();

        திரும்பு cached;
}

வெறுமை locale_variables_free(எழுத்து *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

வெறுமை locale_variables_simplify(எழுத்து *l[_VARIABLE_LC_MAX]) {
        ப்ரதிஜ்ஞா(l);

        க்கு (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                எனில் (p == VARIABLE_LANG)
                        தொடர்;
                எனில் (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

நிலையான மாறிலி எழுத்து * மாறிலி locale_variable_table[_VARIABLE_LC_MAX] = {
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
