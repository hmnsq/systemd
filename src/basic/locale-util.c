/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <langinfo.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/mman.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "locale-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "set.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "utf8.h"

स्थिर वर्ण* normalize_locale(ध्रुव वर्ण *name) {
        ध्रुव वर्ण *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        यदि (e) {
                _cleanup_free_ वर्ण *prefix = रिक्त;

                prefix = strndup(name, e - name);
                यदि (!prefix)
                        प्रत्यागम रिक्त;

                प्रत्यागम strjoin(prefix, ".UTF-8");
        }

        e = म_माला(name, ".utf8@");
        यदि (e) {
                _cleanup_free_ वर्ण *prefix = रिक्त;

                prefix = strndup(name, e - name);
                यदि (!prefix)
                        प्रत्यागम रिक्त;

                प्रत्यागम strjoin(prefix, ".UTF-8@", e + 6);
        }

        प्रत्यागम strdup(name);
}

#ifdef __GLIBC__
स्थिर ध्रुव वर्ण* get_locale_dir(शून्य) {
        प्रत्यागम secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

स्थिर पूर्णाङ्क add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        संरचना locarhead {
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

        संरचना namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        पूर्णाङ्क r;

        प्रतिज्ञा(locales);

        _cleanup_free_ वर्ण *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        यदि (!locale_archive_file)
                प्रत्यागम -ENOMEM;

        _cleanup_close_ पूर्णाङ्क fd = खोलो(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        यदि (fd < 0)
                प्रत्यागम त्रुटि_सं == ENOENT ? 0 : -त्रुटि_सं;

        संरचना stat st;
        यदि (ख_स्थिति(fd, &st) < 0)
                प्रत्यागम -त्रुटि_सं;

        r = stat_verify_regular(&st);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (st.st_size < (off_t) परिमाण(संरचना locarhead))
                प्रत्यागम -EBADMSG;

        यदि (file_offset_beyond_memory_size(st.st_size))
                प्रत्यागम -EFBIG;

        शून्य *p = mmap(रिक्त, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        यदि (p == MAP_FAILED)
                प्रत्यागम -त्रुटि_सं;

        ध्रुव संरचना namehashent *e;
        ध्रुव संरचना locarhead *h = p;
        यदि (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                गच्छ finish;
        }

        e = (ध्रुव संरचना namehashent*) ((ध्रुव uint8_t*) p + h->namehash_offset);
        प्रति (माप_प्रकार i = 0; i < h->namehash_size; i++) {
                वर्ण *z;

                यदि (e[i].locrec_offset == 0)
                        अनुवर्त;

                यदि (!utf8_is_valid((वर्ण*) p + e[i].name_offset))
                        अनुवर्त;

                z = normalize_locale((वर्ण*) p + e[i].name_offset);
                यदि (!z) {
                        r = -ENOMEM;
                        गच्छ finish;
                }

                r = set_consume(locales, z);
                यदि (r < 0)
                        गच्छ finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        प्रत्यागम r;
}

स्थिर पूर्णाङ्क add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ सूची *dir = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(locales);

        dir = सूची_खोलो(get_locale_dir());
        यदि (!dir)
                प्रत्यागम त्रुटि_सं == ENOENT ? 0 : -त्रुटि_सं;

        FOREACH_DIRENT(de, dir, प्रत्यागम -त्रुटि_सं) {
                वर्ण *z;

                यदि (de->d_type != DT_DIR)
                        अनुवर्त;

                z = normalize_locale(de->d_name);
                यदि (!z)
                        प्रत्यागम -ENOMEM;

                r = set_consume(locales, z);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

#अन्यथा

स्थिर ध्रुव वर्ण* ध्रुव musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        रिक्त
};

स्थिर पूर्णाङ्क add_locales_for_musl(Set *locales) {
        पूर्णाङ्क r;

        प्रतिज्ञा(locales);

        ध्रुव वर्ण *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ सूची *dir = सूची_खोलो(*d);
                यदि (!dir)
                        अनुवर्त;

                FOREACH_DIRENT(de, dir, प्रत्यागम -त्रुटि_सं) {
                        यदि (de->d_type != DT_REG)
                                अनुवर्त;

                        वर्ण *z = normalize_locale(de->d_name);
                        यदि (!z)
                                प्रत्यागम -ENOMEM;

                        r = set_consume(locales, z);
                        यदि (r < 0)
                                प्रत्यागम r;
                }
        }

        प्रत्यागम 0;
}
#endif

पूर्णाङ्क get_locales(वर्ण ***ret) {
        _cleanup_set_free_ Set *locales = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        locales = set_new(&string_hash_ops_free);
        यदि (!locales)
                प्रत्यागम -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        यदि (r < 0 && r != -ENOENT)
                प्रत्यागम r;

        r = add_locales_from_libdir(locales);
        यदि (r < 0)
                प्रत्यागम r;
#अन्यथा
        r = add_locales_for_musl(locales);
        यदि (r < 0)
                प्रत्यागम r;
#endif

        वर्ण *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        मुक्त(set_remove(locales, locale));
        }

        _cleanup_strv_free_ वर्ण **l = set_to_strv(&locales);
        यदि (!l)
                प्रत्यागम -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        यदि (r <= 0) {
                यदि (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                वर्ण **b = l;
                STRV_FOREACH(a, l)
                        यदि (endswith(*a, "UTF-8") || म_माला(*a, ".UTF-8@"))
                                *(b++) = *a;
                        अन्यथा
                                मुक्त(*a);

                *b = रिक्त;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        प्रत्यागम 0;
}

bool locale_is_valid(ध्रुव वर्ण *name) {

        यदि (isempty(name))
                प्रत्यागम false;

        यदि (म_माप(name) >= 128)
                प्रत्यागम false;

        यदि (!utf8_is_valid(name))
                प्रत्यागम false;

        यदि (!filename_is_valid(name))
                प्रत्यागम false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        यदि (!in_charset(name, ALPHANUMERICAL "_.-@"))
                प्रत्यागम false;

        प्रत्यागम true;
}

पूर्णाङ्क locale_is_installed(ध्रुव वर्ण *name) {
        यदि (!locale_is_valid(name))
                प्रत्यागम false;

        यदि (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                प्रत्यागम true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        यदि (loc == (locale_t) 0)
                प्रत्यागम त्रुटि_सं == ENOMEM ? -ENOMEM : false;

        प्रत्यागम true;
#अन्यथा
        /* musl also has C.UTF-8 as builtin */
        यदि (streq(name, "C.UTF-8"))
                प्रत्यागम true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        ध्रुव वर्ण *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ वर्ण *p = path_join(*d, name);
                यदि (!p)
                        प्रत्यागम -ENOMEM;

                यदि (access(p, F_OK) >= 0)
                        प्रत्यागम true;
        }

        प्रत्यागम false;
#endif
}

स्थिर bool is_locale_utf8_impl(शून्य) {
        ध्रुव वर्ण *set;
        पूर्णाङ्क r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        यदि (r >= 0)
                प्रत्यागम r;
        यदि (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        यदि (!is_main_thread())
                प्रत्यागम true;

        यदि (!रखो_क्षेत्र(LC_ALL, ""))
                प्रत्यागम true;

        set = nl_langinfo(CODESET);
        यदि (!set || streq(set, "UTF-8"))
                प्रत्यागम true;

        set = रखो_क्षेत्र(LC_CTYPE, रिक्त);
        यदि (!set)
                प्रत्यागम true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        प्रत्यागम STR_IN_SET(set, "C", "POSIX") &&
                !दो_पर्या("LC_ALL") &&
                !दो_पर्या("LC_CTYPE") &&
                !दो_पर्या("LANG");
}

bool is_locale_utf8(शून्य) {
        स्थिर पूर्णाङ्क cached = -1;

        यदि (cached < 0)
                cached = is_locale_utf8_impl();

        प्रत्यागम cached;
}

शून्य locale_variables_free(वर्ण *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

शून्य locale_variables_simplify(वर्ण *l[_VARIABLE_LC_MAX]) {
        प्रतिज्ञा(l);

        प्रति (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                यदि (p == VARIABLE_LANG)
                        अनुवर्त;
                यदि (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

स्थिर ध्रुव वर्ण * ध्रुव locale_variable_table[_VARIABLE_LC_MAX] = {
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
