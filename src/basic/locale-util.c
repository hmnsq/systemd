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

स्थिर वर्ण* normalize_locale(अचर वर्ण *name) {
        अचर वर्ण *e;

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
                        फर्काऊ रिक्त;

                फर्काऊ strjoin(prefix, ".UTF-8");
        }

        e = म_माला(name, ".utf8@");
        यदि (e) {
                _cleanup_free_ वर्ण *prefix = रिक्त;

                prefix = strndup(name, e - name);
                यदि (!prefix)
                        फर्काऊ रिक्त;

                फर्काऊ strjoin(prefix, ".UTF-8@", e + 6);
        }

        फर्काऊ strdup(name);
}

#ifdef __GLIBC__
स्थिर अचर वर्ण* get_locale_dir(शून्य) {
        फर्काऊ secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
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
                फर्काऊ -ENOMEM;

        _cleanup_close_ पूर्णाङ्क fd = खोलो(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        यदि (fd < 0)
                फर्काऊ त्रुटि_सं == ENOENT ? 0 : -त्रुटि_सं;

        संरचना stat st;
        यदि (ख_स्थिति(fd, &st) < 0)
                फर्काऊ -त्रुटि_सं;

        r = stat_verify_regular(&st);
        यदि (r < 0)
                फर्काऊ r;

        यदि (st.st_size < (off_t) परिमाण(संरचना locarhead))
                फर्काऊ -EBADMSG;

        यदि (file_offset_beyond_memory_size(st.st_size))
                फर्काऊ -EFBIG;

        शून्य *p = mmap(रिक्त, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        यदि (p == MAP_FAILED)
                फर्काऊ -त्रुटि_सं;

        अचर संरचना namehashent *e;
        अचर संरचना locarhead *h = p;
        यदि (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                गच्छ finish;
        }

        e = (अचर संरचना namehashent*) ((अचर uint8_t*) p + h->namehash_offset);
        लागि (माप_प्रकार i = 0; i < h->namehash_size; i++) {
                वर्ण *z;

                यदि (e[i].locrec_offset == 0)
                        जारी;

                यदि (!utf8_is_valid((वर्ण*) p + e[i].name_offset))
                        जारी;

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
        फर्काऊ r;
}

स्थिर पूर्णाङ्क add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ सूची *dir = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(locales);

        dir = सूची_खोलो(get_locale_dir());
        यदि (!dir)
                फर्काऊ त्रुटि_सं == ENOENT ? 0 : -त्रुटि_सं;

        FOREACH_DIRENT(de, dir, फर्काऊ -त्रुटि_सं) {
                वर्ण *z;

                यदि (de->d_type != DT_DIR)
                        जारी;

                z = normalize_locale(de->d_name);
                यदि (!z)
                        फर्काऊ -ENOMEM;

                r = set_consume(locales, z);
                यदि (r < 0)
                        फर्काऊ r;
        }

        फर्काऊ 0;
}

#अन्यथा

स्थिर अचर वर्ण* अचर musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        रिक्त
};

स्थिर पूर्णाङ्क add_locales_for_musl(Set *locales) {
        पूर्णाङ्क r;

        प्रतिज्ञा(locales);

        अचर वर्ण *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ सूची *dir = सूची_खोलो(*d);
                यदि (!dir)
                        जारी;

                FOREACH_DIRENT(de, dir, फर्काऊ -त्रुटि_सं) {
                        यदि (de->d_type != DT_REG)
                                जारी;

                        वर्ण *z = normalize_locale(de->d_name);
                        यदि (!z)
                                फर्काऊ -ENOMEM;

                        r = set_consume(locales, z);
                        यदि (r < 0)
                                फर्काऊ r;
                }
        }

        फर्काऊ 0;
}
#endif

पूर्णाङ्क get_locales(वर्ण ***ret) {
        _cleanup_set_free_ Set *locales = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        locales = set_new(&string_hash_ops_free);
        यदि (!locales)
                फर्काऊ -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        यदि (r < 0 && r != -ENOENT)
                फर्काऊ r;

        r = add_locales_from_libdir(locales);
        यदि (r < 0)
                फर्काऊ r;
#अन्यथा
        r = add_locales_for_musl(locales);
        यदि (r < 0)
                फर्काऊ r;
#endif

        वर्ण *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r == 0)
                        मुक्त(set_remove(locales, locale));
        }

        _cleanup_strv_free_ वर्ण **l = set_to_strv(&locales);
        यदि (!l)
                फर्काऊ -ENOMEM;

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

        फर्काऊ 0;
}

bool locale_is_valid(अचर वर्ण *name) {

        यदि (isempty(name))
                फर्काऊ false;

        यदि (म_माप(name) >= 128)
                फर्काऊ false;

        यदि (!utf8_is_valid(name))
                फर्काऊ false;

        यदि (!filename_is_valid(name))
                फर्काऊ false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        यदि (!in_charset(name, ALPHANUMERICAL "_.-@"))
                फर्काऊ false;

        फर्काऊ true;
}

पूर्णाङ्क locale_is_installed(अचर वर्ण *name) {
        यदि (!locale_is_valid(name))
                फर्काऊ false;

        यदि (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                फर्काऊ true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        यदि (loc == (locale_t) 0)
                फर्काऊ त्रुटि_सं == ENOMEM ? -ENOMEM : false;

        फर्काऊ true;
#अन्यथा
        /* musl also has C.UTF-8 as builtin */
        यदि (streq(name, "C.UTF-8"))
                फर्काऊ true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        अचर वर्ण *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ वर्ण *p = path_join(*d, name);
                यदि (!p)
                        फर्काऊ -ENOMEM;

                यदि (access(p, F_OK) >= 0)
                        फर्काऊ true;
        }

        फर्काऊ false;
#endif
}

स्थिर bool is_locale_utf8_impl(शून्य) {
        अचर वर्ण *set;
        पूर्णाङ्क r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        यदि (r >= 0)
                फर्काऊ r;
        यदि (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        यदि (!is_main_thread())
                फर्काऊ true;

        यदि (!रखो_क्षेत्र(LC_ALL, ""))
                फर्काऊ true;

        set = nl_langinfo(CODESET);
        यदि (!set || streq(set, "UTF-8"))
                फर्काऊ true;

        set = रखो_क्षेत्र(LC_CTYPE, रिक्त);
        यदि (!set)
                फर्काऊ true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        फर्काऊ STR_IN_SET(set, "C", "POSIX") &&
                !दो_पर्या("LC_ALL") &&
                !दो_पर्या("LC_CTYPE") &&
                !दो_पर्या("LANG");
}

bool is_locale_utf8(शून्य) {
        स्थिर पूर्णाङ्क cached = -1;

        यदि (cached < 0)
                cached = is_locale_utf8_impl();

        फर्काऊ cached;
}

शून्य locale_variables_free(वर्ण *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

शून्य locale_variables_simplify(वर्ण *l[_VARIABLE_LC_MAX]) {
        प्रतिज्ञा(l);

        लागि (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                यदि (p == VARIABLE_LANG)
                        जारी;
                यदि (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

स्थिर अचर वर्ण * अचर locale_variable_table[_VARIABLE_LC_MAX] = {
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
