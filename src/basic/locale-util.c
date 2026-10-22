/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <langinfo.h>
#समावेश <stdlib.h>
#समावेश <sys/mman.h>
#समावेश <sys/stat.h>
#समावेश <unistd.h>

#समावेश "dirent-util.h"
#समावेश "env-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "locale-util.h"
#समावेश "log.h"
#समावेश "memory-util.h"
#समावेश "path-util.h"
#समावेश "process-util.h"
#समावेश "set.h"
#समावेश "stat-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "utf8.h"

अटल अक्षर* normalize_locale(स्थिर अक्षर *name) {
        स्थिर अक्षर *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        अगर (e) {
                _cleanup_free_ अक्षर *prefix = शून्य;

                prefix = strndup(name, e - name);
                अगर (!prefix)
                        वापस शून्य;

                वापस strjoin(prefix, ".UTF-8");
        }

        e = म_माला(name, ".utf8@");
        अगर (e) {
                _cleanup_free_ अक्षर *prefix = शून्य;

                prefix = strndup(name, e - name);
                अगर (!prefix)
                        वापस शून्य;

                वापस strjoin(prefix, ".UTF-8@", e + 6);
        }

        वापस strdup(name);
}

#अगर_घोषित __GLIBC__
अटल स्थिर अक्षर* get_locale_dir(व्योम) {
        वापस secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

अटल पूर्णांक add_locales_from_archive(Set *locales) {
        /* Stolen from glibc... */

        काष्ठा locarhead {
                uint32_t magic;
                /* Serial number.  */
                uint32_t serial;
                /* Name hash table.  */
                uint32_t namehash_offset;
                uint32_t namehash_used;
                uint32_t namehash_आकार;
                /* String table.  */
                uint32_t string_offset;
                uint32_t string_used;
                uint32_t string_आकार;
                /* Table with locale records.  */
                uint32_t locrectab_offset;
                uint32_t locrectab_used;
                uint32_t locrectab_आकार;
                /* MD5 sum hash table.  */
                uint32_t sumhash_offset;
                uint32_t sumhash_used;
                uint32_t sumhash_आकार;
        };

        काष्ठा namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_offset;
                /* Offset of the locale record.  */
                uint32_t locrec_offset;
        };

        पूर्णांक r;

        निश्चित(locales);

        _cleanup_free_ अक्षर *locale_archive_file = path_join(get_locale_dir(), "locale-archive");
        अगर (!locale_archive_file)
                वापस -ENOMEM;

        _cleanup_close_ पूर्णांक fd = खोलो(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        अगर (fd < 0)
                वापस त्रुटि_सं == ENOENT ? 0 : -त्रुटि_सं;

        काष्ठा stat st;
        अगर (ख_स्थिति(fd, &st) < 0)
                वापस -त्रुटि_सं;

        r = stat_verify_regular(&st);
        अगर (r < 0)
                वापस r;

        अगर (st.st_size < (off_t) माप(काष्ठा locarhead))
                वापस -EBADMSG;

        अगर (file_offset_beyond_memory_आकार(st.st_size))
                वापस -EFBIG;

        व्योम *p = mmap(शून्य, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        अगर (p == MAP_FAILED)
                वापस -त्रुटि_सं;

        स्थिर काष्ठा namehashent *e;
        स्थिर काष्ठा locarhead *h = p;
        अगर (h->magic != 0xde020109 ||
            h->namehash_offset + h->namehash_आकार > st.st_size ||
            h->string_offset + h->string_आकार > st.st_size ||
            h->locrectab_offset + h->locrectab_आकार > st.st_size ||
            h->sumhash_offset + h->sumhash_आकार > st.st_size) {
                r = -EBADMSG;
                जाओ finish;
        }

        e = (स्थिर काष्ठा namehashent*) ((स्थिर uint8_t*) p + h->namehash_offset);
        क्रम (माप_प्रकार i = 0; i < h->namehash_आकार; i++) {
                अक्षर *z;

                अगर (e[i].locrec_offset == 0)
                        जारी;

                अगर (!utf8_is_valid((अक्षर*) p + e[i].name_offset))
                        जारी;

                z = normalize_locale((अक्षर*) p + e[i].name_offset);
                अगर (!z) {
                        r = -ENOMEM;
                        जाओ finish;
                }

                r = set_consume(locales, z);
                अगर (r < 0)
                        जाओ finish;
        }

        r = 0;

finish:
        munmap_safe(p, st.st_size);
        वापस r;
}

अटल पूर्णांक add_locales_from_libdir(Set *locales) {
        _cleanup_closedir_ सूची *dir = शून्य;
        पूर्णांक r;

        निश्चित(locales);

        dir = सूची_खोलो(get_locale_dir());
        अगर (!dir)
                वापस त्रुटि_सं == ENOENT ? 0 : -त्रुटि_सं;

        FOREACH_DIRENT(de, dir, वापस -त्रुटि_सं) {
                अक्षर *z;

                अगर (de->d_type != DT_DIR)
                        जारी;

                z = normalize_locale(de->d_name);
                अगर (!z)
                        वापस -ENOMEM;

                r = set_consume(locales, z);
                अगर (r < 0)
                        वापस r;
        }

        वापस 0;
}

#अन्यथा

अटल स्थिर अक्षर* स्थिर musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        शून्य
};

अटल पूर्णांक add_locales_for_musl(Set *locales) {
        पूर्णांक r;

        निश्चित(locales);

        स्थिर अक्षर *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ सूची *dir = सूची_खोलो(*d);
                अगर (!dir)
                        जारी;

                FOREACH_DIRENT(de, dir, वापस -त्रुटि_सं) {
                        अगर (de->d_type != DT_REG)
                                जारी;

                        अक्षर *z = normalize_locale(de->d_name);
                        अगर (!z)
                                वापस -ENOMEM;

                        r = set_consume(locales, z);
                        अगर (r < 0)
                                वापस r;
                }
        }

        वापस 0;
}
#पूर्ण_अगर

पूर्णांक get_locales(अक्षर ***ret) {
        _cleanup_set_free_ Set *locales = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        locales = set_new(&string_hash_ops_free);
        अगर (!locales)
                वापस -ENOMEM;

#अगर_घोषित __GLIBC__
        r = add_locales_from_archive(locales);
        अगर (r < 0 && r != -ENOENT)
                वापस r;

        r = add_locales_from_libdir(locales);
        अगर (r < 0)
                वापस r;
#अन्यथा
        r = add_locales_for_musl(locales);
        अगर (r < 0)
                वापस r;
#पूर्ण_अगर

        अक्षर *locale;
        SET_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        मुक्त(set_remove(locales, locale));
        }

        _cleanup_strv_free_ अक्षर **l = set_to_strv(&locales);
        अगर (!l)
                वापस -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        अगर (r <= 0) {
                अगर (!IN_SET(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                अक्षर **b = l;
                STRV_FOREACH(a, l)
                        अगर (endswith(*a, "UTF-8") || म_माला(*a, ".UTF-8@"))
                                *(b++) = *a;
                        अन्यथा
                                मुक्त(*a);

                *b = शून्य;
        }

        strv_sort(l);

        *ret = TAKE_PTR(l);

        वापस 0;
}

bool locale_is_valid(स्थिर अक्षर *name) {

        अगर (isempty(name))
                वापस false;

        अगर (म_माप(name) >= 128)
                वापस false;

        अगर (!utf8_is_valid(name))
                वापस false;

        अगर (!filename_is_valid(name))
                वापस false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        अगर (!in_charset(name, ALPHANUMERICAL "_.-@"))
                वापस false;

        वापस true;
}

पूर्णांक locale_is_installed(स्थिर अक्षर *name) {
        अगर (!locale_is_valid(name))
                वापस false;

        अगर (STR_IN_SET(name, "C", "POSIX")) /* These ones are always OK */
                वापस true;

#अगर_घोषित __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        अगर (loc == (locale_t) 0)
                वापस त्रुटि_सं == ENOMEM ? -ENOMEM : false;

        वापस true;
#अन्यथा
        /* musl also has C.UTF-8 as builtin */
        अगर (streq(name, "C.UTF-8"))
                वापस true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        स्थिर अक्षर *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ अक्षर *p = path_join(*d, name);
                अगर (!p)
                        वापस -ENOMEM;

                अगर (access(p, F_OK) >= 0)
                        वापस true;
        }

        वापस false;
#पूर्ण_अगर
}

अटल bool is_locale_utf8_impl(व्योम) {
        स्थिर अक्षर *set;
        पूर्णांक r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        अगर (r >= 0)
                वापस r;
        अगर (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        अगर (!is_main_thread())
                वापस true;

        अगर (!रखो_क्षेत्र(LC_ALL, ""))
                वापस true;

        set = nl_langinfo(CODESET);
        अगर (!set || streq(set, "UTF-8"))
                वापस true;

        set = रखो_क्षेत्र(LC_CTYPE, शून्य);
        अगर (!set)
                वापस true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        वापस STR_IN_SET(set, "C", "POSIX") &&
                !दो_पर्या("LC_ALL") &&
                !दो_पर्या("LC_CTYPE") &&
                !दो_पर्या("LANG");
}

bool is_locale_utf8(व्योम) {
        अटल पूर्णांक cached = -1;

        अगर (cached < 0)
                cached = is_locale_utf8_impl();

        वापस cached;
}

व्योम locale_variables_free(अक्षर *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

व्योम locale_variables_simplify(अक्षर *l[_VARIABLE_LC_MAX]) {
        निश्चित(l);

        क्रम (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                अगर (p == VARIABLE_LANG)
                        जारी;
                अगर (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

अटल स्थिर अक्षर * स्थिर locale_variable_table[_VARIABLE_LC_MAX] = {
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
