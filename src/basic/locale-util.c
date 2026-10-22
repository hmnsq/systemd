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

स्थिर अक्षर* normalize_locale(स्थिरांक अक्षर *name) {
        स्थिरांक अक्षर *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        जर (e) {
                _cleanup_free_ अक्षर *prefix = रिक्त;

                prefix = strndup(name, e - name);
                जर (!prefix)
                        परत रिक्त;

                परत strjoin(prefix, ".UTF-8");
        }

        e = म_माला(name, ".utf8@");
        जर (e) {
                _cleanup_free_ अक्षर *prefix = रिक्त;

                prefix = strndup(name, e - name);
                जर (!prefix)
                        परत रिक्त;

                परत strjoin(prefix, ".UTF-8@", e + 6);
        }

        परत strdup(name);
}

#ifdef __GLIBC__
स्थिर स्थिरांक अक्षर* get_locale_dir(शून्य) {
        परत secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

स्थिर पूर्णांक add_locales_from_archive(Set *locales) {
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

        पूर्णांक r;

        प्रतिज्ञा(locales);

        _cleanup_free_ अक्षर *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        जर (!locale_archive_file)
                परत -ENOMEM;

        _cleanup_close_ पूर्णांक fd = खोलो(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        जर (fd < 0)
                परत त्रुटि_सं == ENOENT ? 0 : -त्रुटि_सं;

        संरचना stat st;
        जर (ख_स्थिति(fd, &st) < 0)
                परत -त्रुटि_सं;

        r = stat_verify_regular(&st);
        जर (r < 0)
                परत r;

        जर (st.st_size < (off_t) आकार(संरचना locarhead))
                परत -EBADMSG;

        जर (file_offset_beyond_memory_size(st.st_size))
                परत -EFBIG;

        शून्य *p = mmap(रिक्त, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        जर (p == MAP_FAILED)
                परत -त्रुटि_सं;

        स्थिरांक संरचना namehashent *e;
        स्थिरांक संरचना locarhead *h = p;
        जर (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_size > st.st_size ||
            h->string_offset + h->string_size > st.st_size ||
            h->locrectab_offset + h->locrectab_size > st.st_size ||
            h->sumhash_offset + h->sumhash_size > st.st_size) {
                r = -EBADMSG;
                जा finish;
        }

        e = (स्थिरांक संरचना namehashent*) ((स्थिरांक uint8_t*) p + h->namehash_offset);
        साठी (माप_प्रकार i = 0; i < h->namehash_size; i++) {
                अक्षर *z;

                जर (e[i].locrec_offset == 0)
                        सुरूठेवा;

                जर (!utf8_is_valid((अक्षर*) p + e[i].name_offset))
                        सुरूठेवा;

                z = normalize_locale((अक्षर*) p + e[i].name_offset);
                जर (!z) {
                        r = -ENOMEM;
                        जा finish;
                }

                r = set_consume(locales, z);
                जर (r < 0)
                        जा finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        परत r;
}

स्थिर पूर्णांक add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ सूची *dir = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(locales);

        dir = सूची_खोलो(get_locale_dir());
        जर (!dir)
                परत त्रुटि_सं == ENOENT ? 0 : -त्रुटि_सं;

        FOREACH_DIRENT(de, dir, परत -त्रुटि_सं) {
                अक्षर *z;

                जर (de->d_type != DT_DIR)
                        सुरूठेवा;

                z = normalize_locale(de->d_name);
                जर (!z)
                        परत -ENOMEM;

                r = set_consume(locales, z);
                जर (r < 0)
                        परत r;
        }

        परत 0;
}

#अन्यथा

स्थिर स्थिरांक अक्षर* स्थिरांक musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        रिक्त
};

स्थिर पूर्णांक add_locales_for_musl(Set *locales) {
        पूर्णांक r;

        प्रतिज्ञा(locales);

        स्थिरांक अक्षर *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ सूची *dir = सूची_खोलो(*d);
                जर (!dir)
                        सुरूठेवा;

                FOREACH_DIRENT(de, dir, परत -त्रुटि_सं) {
                        जर (de->d_type != DT_REG)
                                सुरूठेवा;

                        अक्षर *z = normalize_locale(de->d_name);
                        जर (!z)
                                परत -ENOMEM;

                        r = set_consume(locales, z);
                        जर (r < 0)
                                परत r;
                }
        }

        परत 0;
}
#endif

पूर्णांक get_locales(अक्षर ***ret) {
        _cleanup_set_free_ Set *locales = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        locales = set_new(&string_hash_ops_free);
        जर (!locales)
                परत -ENOMEM;

#ifdef __GLIBC__
        r = add_locales_from_archive(locales);
        जर (r < 0 && r != -ENOENT)
                परत r;

        r = add_locales_from_libdir(locales);
        जर (r < 0)
                परत r;
#अन्यथा
        r = add_locales_for_musl(locales);
        जर (r < 0)
                परत r;
#endif

        अक्षर *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        मुक्त(set_remove(locales, locale));
        }

        _cleanup_strv_free_ अक्षर **l = set_to_strv(&locales);
        जर (!l)
                परत -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        जर (r <= 0) {
                जर (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                अक्षर **b = l;
                STRV_FOREACH(a, l)
                        जर (endswith(*a, "UTF-8") || म_माला(*a, ".UTF-8@"))
                                *(b++) = *a;
                        अन्यथा
                                मुक्त(*a);

                *b = रिक्त;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        परत 0;
}

bool locale_is_valid(स्थिरांक अक्षर *name) {

        जर (isempty(name))
                परत false;

        जर (म_माप(name) >= 128)
                परत false;

        जर (!utf8_is_valid(name))
                परत false;

        जर (!filename_is_valid(name))
                परत false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        जर (!in_charset(name, ALPHANUMERICAL "_.-@"))
                परत false;

        परत true;
}

पूर्णांक locale_is_installed(स्थिरांक अक्षर *name) {
        जर (!locale_is_valid(name))
                परत false;

        जर (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                परत true;

#ifdef __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        जर (loc == (locale_t) 0)
                परत त्रुटि_सं == ENOMEM ? -ENOMEM : false;

        परत true;
#अन्यथा
        /* musl also has C.UTF-8 as builtin */
        जर (streq(name, "C.UTF-8"))
                परत true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        स्थिरांक अक्षर *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ अक्षर *p = path_join(*d, name);
                जर (!p)
                        परत -ENOMEM;

                जर (access(p, F_OK) >= 0)
                        परत true;
        }

        परत false;
#endif
}

स्थिर bool is_locale_utf8_impl(शून्य) {
        स्थिरांक अक्षर *set;
        पूर्णांक r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        जर (r >= 0)
                परत r;
        जर (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        जर (!is_main_thread())
                परत true;

        जर (!रखो_क्षेत्र(LC_ALL, ""))
                परत true;

        set = nl_langinfo(CODESET);
        जर (!set || streq(set, "UTF-8"))
                परत true;

        set = रखो_क्षेत्र(LC_CTYPE, रिक्त);
        जर (!set)
                परत true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        परत STR_IN_SET(set, "C", "POSIX") &&
                !दो_पर्या("LC_ALL") &&
                !दो_पर्या("LC_CTYPE") &&
                !दो_पर्या("LANG");
}

bool is_locale_utf8(शून्य) {
        स्थिर पूर्णांक cached = -1;

        जर (cached < 0)
                cached = is_locale_utf8_impl();

        परत cached;
}

शून्य locale_variables_free(अक्षर *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

शून्य locale_variables_simplify(अक्षर *l[_VARIABLE_LC_MAX]) {
        प्रतिज्ञा(l);

        साठी (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                जर (p == VARIABLE_LANG)
                        सुरूठेवा;
                जर (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

स्थिर स्थिरांक अक्षर * स्थिरांक locale_variable_table[_VARIABLE_LC_MAX] = {
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
