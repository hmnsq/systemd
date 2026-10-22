/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <langinfo.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/mman.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "locale-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "set.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "utf8.h"

സ്ഥിതം അക്ഷരം* normalize_locale(സ്ഥിരം അക്ഷരം *name) {
        സ്ഥിരം അക്ഷരം *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        എങ്കിൽ (e) {
                _cleanup_free_ അക്ഷരം *prefix = ശൂന്യ;

                prefix = strndup(name, e - name);
                എങ്കിൽ (!prefix)
                        തിരികെ ശൂന്യ;

                തിരികെ strjoin(prefix, ".UTF-8");
        }

        e = strstr(name, ".utf8@");
        എങ്കിൽ (e) {
                _cleanup_free_ അക്ഷരം *prefix = ശൂന്യ;

                prefix = strndup(name, e - name);
                എങ്കിൽ (!prefix)
                        തിരികെ ശൂന്യ;

                തിരികെ strjoin(prefix, ".UTF-8@", e + 6);
        }

        തിരികെ strdup(name);
}

#ifdef __GLIBC__
സ്ഥിതം സ്ഥിരം അക്ഷരം* get_locale_dir(ശൂന്യം) {
        തിരികെ secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

സ്ഥിതം പൂർണ്ണസംഖ്യ add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        ഘടന locarhead {
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

        ഘടന namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(locales);

        _cleanup_free_ അക്ഷരം *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        എങ്കിൽ (!locale_archive_file)
                തിരികെ -ENOMEM;

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ errno == ENOENT ? 0 : -errno;

        ഘടന stat st;
        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        r = stat_verify_regular(&st);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (st.st_size < (off_t) വലുപ്പം(ഘടന locarhead))
                തിരികെ -EBADMSG;

        എങ്കിൽ (file_offset_beyond_memory_size(st.st_size))
                തിരികെ -EFBIG;

        ശൂന്യം *p = mmap(ശൂന്യ, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        എങ്കിൽ (p == MAP_FAILED)
                തിരികെ -errno;

        സ്ഥിരം ഘടന namehashent *e;
        സ്ഥിരം ഘടന locarhead *h = p;
        എങ്കിൽ (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                പോകുക finish;
        }

        e = (സ്ഥിരം ഘടന namehashent*) ((സ്ഥിരം uint8_t*) p + h->namehash_offset);
        വേണ്ടി (size_t i = 0; i < h->namehash_size; i++) {
                അക്ഷരം *z;

                എങ്കിൽ (e[i].locrec_offset == 0)
                        തുടരുക;

                എങ്കിൽ (!utf8_is_valid((അക്ഷരം*) p + e[i].name_offset))
                        തുടരുക;

                z = normalize_locale((അക്ഷരം*) p + e[i].name_offset);
                എങ്കിൽ (!z) {
                        r = -ENOMEM;
                        പോകുക finish;
                }

                r = set_consume(locales, z);
                എങ്കിൽ (r < 0)
                        പോകുക finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        തിരികെ r;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ DIR *dir = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(locales);

        dir = opendir(get_locale_dir());
        എങ്കിൽ (!dir)
                തിരികെ errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, തിരികെ -errno) {
                അക്ഷരം *z;

                എങ്കിൽ (de->d_type != DT_DIR)
                        തുടരുക;

                z = normalize_locale(de->d_name);
                എങ്കിൽ (!z)
                        തിരികെ -ENOMEM;

                r = set_consume(locales, z);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

#അല്ലെങ്കിൽ

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        ശൂന്യ
};

സ്ഥിതം പൂർണ്ണസംഖ്യ add_locales_for_musl(Set *locales) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(locales);

        സ്ഥിരം അക്ഷരം *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                എങ്കിൽ (!dir)
                        തുടരുക;

                FOREACH_DIRENT(de, dir, തിരികെ -errno) {
                        എങ്കിൽ (de->d_type != DT_REG)
                                തുടരുക;

                        അക്ഷരം *z = normalize_locale(de->d_name);
                        എങ്കിൽ (!z)
                                തിരികെ -ENOMEM;

                        r = set_consume(locales, z);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }
        }

        തിരികെ 0;
}
#endif

പൂർണ്ണസംഖ്യ get_locales(അക്ഷരം ***ret) {
        _cleanup_set_free_ Set *locales = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        locales = set_new(&string_hash_ops_free);
        എങ്കിൽ (!locales)
                തിരികെ -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        എങ്കിൽ (r < 0 && r != -ENOENT)
                തിരികെ r;

        r = add_locales_from_libdir(locales);
        എങ്കിൽ (r < 0)
                തിരികെ r;
#അല്ലെങ്കിൽ
        r = add_locales_for_musl(locales);
        എങ്കിൽ (r < 0)
                തിരികെ r;
#endif

        അക്ഷരം *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        സ്വതന്ത്രം(set_remove(locales, locale));
        }

        _cleanup_strv_free_ അക്ഷരം **l = set_to_strv(&locales);
        എങ്കിൽ (!l)
                തിരികെ -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        എങ്കിൽ (r <= 0) {
                എങ്കിൽ (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                അക്ഷരം **b = l;
                STRV_FOREACH(a, l)
                        എങ്കിൽ (endswith(*a, "UTF-8") || strstr(*a, ".UTF-8@"))
                                *(b++) = *a;
                        അല്ലെങ്കിൽ
                                സ്വതന്ത്രം(*a);

                *b = ശൂന്യ;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        തിരികെ 0;
}

bool locale_is_valid(സ്ഥിരം അക്ഷരം *name) {

        എങ്കിൽ (isempty(name))
                തിരികെ false;

        എങ്കിൽ (strlen(name) >= 128)
                തിരികെ false;

        എങ്കിൽ (!utf8_is_valid(name))
                തിരികെ false;

        എങ്കിൽ (!filename_is_valid(name))
                തിരികെ false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        എങ്കിൽ (!in_charset(name, ALPHANUMERICAL "_.-@"))
                തിരികെ false;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ locale_is_installed(സ്ഥിരം അക്ഷരം *name) {
        എങ്കിൽ (!locale_is_valid(name))
                തിരികെ false;

        എങ്കിൽ (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                തിരികെ true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        എങ്കിൽ (loc == (locale_t) 0)
                തിരികെ errno == ENOMEM ? -ENOMEM : false;

        തിരികെ true;
#അല്ലെങ്കിൽ
        /* musl also has C.UTF-8 as builtin */
        എങ്കിൽ (streq(name, "C.UTF-8"))
                തിരികെ true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        സ്ഥിരം അക്ഷരം *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ അക്ഷരം *p = path_join(*d, name);
                എങ്കിൽ (!p)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (access(p, F_OK) >= 0)
                        തിരികെ true;
        }

        തിരികെ false;
#endif
}

സ്ഥിതം bool is_locale_utf8_impl(ശൂന്യം) {
        സ്ഥിരം അക്ഷരം *set;
        പൂർണ്ണസംഖ്യ r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        എങ്കിൽ (r >= 0)
                തിരികെ r;
        എങ്കിൽ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        എങ്കിൽ (!is_main_thread())
                തിരികെ true;

        എങ്കിൽ (!setlocale(LC_ALL, ""))
                തിരികെ true;

        set = nl_langinfo(CODESET);
        എങ്കിൽ (!set || streq(set, "UTF-8"))
                തിരികെ true;

        set = setlocale(LC_CTYPE, ശൂന്യ);
        എങ്കിൽ (!set)
                തിരികെ true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        തിരികെ STR_IN_SET(set, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(ശൂന്യം) {
        സ്ഥിതം പൂർണ്ണസംഖ്യ cached = -1;

        എങ്കിൽ (cached < 0)
                cached = is_locale_utf8_impl();

        തിരികെ cached;
}

ശൂന്യം locale_variables_free(അക്ഷരം *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

ശൂന്യം locale_variables_simplify(അക്ഷരം *l[_VARIABLE_LC_MAX]) {
        പ്രതിജ്ഞാ(l);

        വേണ്ടി (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                എങ്കിൽ (p == VARIABLE_LANG)
                        തുടരുക;
                എങ്കിൽ (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം locale_variable_table[_VARIABLE_LC_MAX] = {
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
