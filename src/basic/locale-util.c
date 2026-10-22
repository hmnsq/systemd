/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <langinfo.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/mman.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "locale-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "set.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "utf8.h"

స్థిర అక్షరం* normalize_locale(స్థిరరాశి అక్షరం *name) {
        స్థిరరాశి అక్షరం *e;

        /* Locale names are weird: glibc has some magic rules when looking for the charset name on disk: it
         * lowercases everything, and removes most special chars. This means the official .UTF-8 suffix
         * becomes .utf8 when looking things up on disk. When enumerating locales, let's do the reverse
         * operation, and go back to ".UTF-8" which appears to be the more commonly accepted name. We only do
         * that for UTF-8 however, since it's kinda the only charset that matters. */

        e = endswith(name, ".utf8");
        ఒకవేళ (e) {
                _cleanup_free_ అక్షరం *ఉపసర్గ = ఖాళీ;

                ఉపసర్గ = strndup(name, e - name);
                ఒకవేళ (!ఉపసర్గ)
                        తిరిగి ఖాళీ;

                తిరిగి strjoin(ఉపసర్గ, ".UTF-8");
        }

        e = అక్షరశ్రేణి_శోధన(name, ".utf8@");
        ఒకవేళ (e) {
                _cleanup_free_ అక్షరం *ఉపసర్గ = ఖాళీ;

                ఉపసర్గ = strndup(name, e - name);
                ఒకవేళ (!ఉపసర్గ)
                        తిరిగి ఖాళీ;

                తిరిగి strjoin(ఉపసర్గ, ".UTF-8@", e + 6);
        }

        తిరిగి strdup(name);
}

#నిర్వచితమైతే __GLIBC__
స్థిర స్థిరరాశి అక్షరం* get_locale_dir(శూన్య) {
        తిరిగి secure_getenv("SYSTEMD_LOCALE_DIRECTORY") ?: "/usr/lib/locale/";
}

స్థిర పూర్ణాంకం కూడు_locales_from_archive(సమితి_2 *locales) {
        /* Stolen from glibc... */

        నిర్మాణం locarhead {
                uint32_t magic;
                /* Serial number.  */
                uint32_t serial;
                /* Name hash table.  */
                uint32_t namehash_స్థానభ్రంశం;
                uint32_t namehash_వాడిన;
                uint32_t namehash_కొలత;
                /* String table.  */
                uint32_t string_స్థానభ్రంశం;
                uint32_t string_వాడిన;
                uint32_t string_కొలత;
                /* Table with locale records.  */
                uint32_t locrectab_స్థానభ్రంశం;
                uint32_t locrectab_వాడిన;
                uint32_t locrectab_కొలత;
                /* MD5 sum hash table.  */
                uint32_t sumhash_స్థానభ్రంశం;
                uint32_t sumhash_వాడిన;
                uint32_t sumhash_కొలత;
        };

        నిర్మాణం namehashent {
                /* Hash value of the name.  */
                uint32_t hashval;
                /* Offset of the name in the string table.  */
                uint32_t name_స్థానభ్రంశం;
                /* Offset of the locale record.  */
                uint32_t locrec_స్థానభ్రంశం;
        };

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(locales);

        _cleanup_free_ అక్షరం *locale_archive_file = మార్గం_join(get_locale_dir(), "locale-archive");
        ఒకవేళ (!locale_archive_file)
                తిరిగి -ENOMEM;

        _cleanup_close_ పూర్ణాంకం fd = open(locale_archive_file, O_RDONLY|O_NOCTTY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి errno == ENOENT ? 0 : -errno;

        నిర్మాణం stat st;
        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        r = stat_verify_regular(&st);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (st.st_size < (off_t) పరిమాణం(నిర్మాణం locarhead))
                తిరిగి -EBADMSG;

        ఒకవేళ (file_స్థానభ్రంశం_beyond_జ్ఞాపకం_కొలత(st.st_size))
                తిరిగి -EFBIG;

        శూన్య *p = mmap(ఖాళీ, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
        ఒకవేళ (p == MAP_FAILED)
                తిరిగి -errno;

        స్థిరరాశి నిర్మాణం namehashent *e;
        స్థిరరాశి నిర్మాణం locarhead *h = p;
        ఒకవేళ (h->magic != 0xde020109 ||
            h->namehash_స్థానభ్రంశం + h->namehash_కొలత > st.st_size ||
            h->string_స్థానభ్రంశం + h->string_కొలత > st.st_size ||
            h->locrectab_స్థానభ్రంశం + h->locrectab_కొలత > st.st_size ||
            h->sumhash_స్థానభ్రంశం + h->sumhash_కొలత > st.st_size) {
                r = -EBADMSG;
                వెళ్ళు ముగింపు;
        }

        e = (స్థిరరాశి నిర్మాణం namehashent*) ((స్థిరరాశి uint8_t*) p + h->namehash_స్థానభ్రంశం);
        కొరకు (size_t i = 0; i < h->namehash_కొలత; i++) {
                అక్షరం *z;

                ఒకవేళ (e[i].locrec_స్థానభ్రంశం == 0)
                        కొనసాగు;

                ఒకవేళ (!utf8_is_valid((అక్షరం*) p + e[i].name_స్థానభ్రంశం))
                        కొనసాగు;

                z = normalize_locale((అక్షరం*) p + e[i].name_స్థానభ్రంశం);
                ఒకవేళ (!z) {
                        r = -ENOMEM;
                        వెళ్ళు ముగింపు;
                }

                r = సమితి_consume(locales, z);
                ఒకవేళ (r < 0)
                        వెళ్ళు ముగింపు;
        }

        r = 0;

ముగింపు:
        munmap_సురక్షిత(p, st.st_size);
        తిరిగి r;
}

స్థిర పూర్ణాంకం కూడు_locales_from_libdir(సమితి_2 *locales) {
        _cleanup_closedir_ DIR *dir = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(locales);

        dir = opendir(get_locale_dir());
        ఒకవేళ (!dir)
                తిరిగి errno == ENOENT ? 0 : -errno;

        FOREACH_DIRENT(de, dir, తిరిగి -errno) {
                అక్షరం *z;

                ఒకవేళ (de->d_type != DT_DIR)
                        కొనసాగు;

                z = normalize_locale(de->d_name);
                ఒకవేళ (!z)
                        తిరిగి -ENOMEM;

                r = సమితి_consume(locales, z);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

#లేకపోతే

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి musl_locale_dirs[] = {
        "/etc/musl/locale",
        "/usr/share/musl/locale",
        "/share/musl/locale",
        "/usr/share/i18n/locales/musl", // compat with musl-locales 0.1.0
        ఖాళీ
};

స్థిర పూర్ణాంకం కూడు_locales_for_musl(సమితి_2 *locales) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(locales);

        స్థిరరాశి అక్షరం *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_closedir_ DIR *dir = opendir(*d);
                ఒకవేళ (!dir)
                        కొనసాగు;

                FOREACH_DIRENT(de, dir, తిరిగి -errno) {
                        ఒకవేళ (de->d_type != DT_REG)
                                కొనసాగు;

                        అక్షరం *z = normalize_locale(de->d_name);
                        ఒకవేళ (!z)
                                తిరిగి -ENOMEM;

                        r = సమితి_consume(locales, z);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }
        }

        తిరిగి 0;
}
#అయితే_ముగింపు

పూర్ణాంకం get_locales(అక్షరం ***ret) {
        cleanup_సమితి_free సమితి_2 *locales = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        locales = సమితి_new(&string_hash_ops_free);
        ఒకవేళ (!locales)
                తిరిగి -ENOMEM;

#నిర్వచితమైతే __GLIBC__
        r = కూడు_locales_from_archive(locales);
        ఒకవేళ (r < 0 && r != -ENOENT)
                తిరిగి r;

        r = కూడు_locales_from_libdir(locales);
        ఒకవేళ (r < 0)
                తిరిగి r;
#లేకపోతే
        r = కూడు_locales_for_musl(locales);
        ఒకవేళ (r < 0)
                తిరిగి r;
#అయితే_ముగింపు

        అక్షరం *locale;
        సమితి_FOREACH(locale, locales) {
                r = locale_is_installed(locale);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        విడుదల(సమితి_remove(locales, locale));
        }

        _cleanup_strv_free_ అక్షరం **l = సమితి_గమ్యం_strv(&locales);
        ఒకవేళ (!l)
                తిరిగి -ENOMEM;

        r = getenv_bool("SYSTEMD_LIST_NON_UTF8_LOCALES");
        ఒకవేళ (r <= 0) {
                ఒకవేళ (!లోపల_సమితి(r, -ENXIO, 0))
                        log_debug_errno(r, "Failed to parse $SYSTEMD_LIST_NON_UTF8_LOCALES as boolean, ignoring: %m");

                /* Filter out non-UTF-8 locales, because it's 2019, by default */
                అక్షరం **b = l;
                STRV_FOREACH(a, l)
                        ఒకవేళ (endswith(*a, "UTF-8") || అక్షరశ్రేణి_శోధన(*a, ".UTF-8@"))
                                *(b++) = *a;
                        లేకపోతే
                                విడుదల(*a);

                *b = ఖాళీ;
        }

        strv_క్రమీకరించు(l);

        *ret = TAKE_PTR(l);

        తిరిగి 0;
}

bool locale_is_valid(స్థిరరాశి అక్షరం *name) {

        ఒకవేళ (isempty(name))
                తిరిగి false;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(name) >= 128)
                తిరిగి false;

        ఒకవేళ (!utf8_is_valid(name))
                తిరిగి false;

        ఒకవేళ (!filename_is_valid(name))
                తిరిగి false;

        /* Locales look like: ll_CC.ENC@variant, where ll and CC are alphabetic, ENC is alphanumeric with
         * dashes, and variant seems to be alphabetic.
         * See: https://www.gnu.org/software/gettext/manual/html_node/Locale-Names.html */
        ఒకవేళ (!లోపల_charset(name, ALPHANUMERICAL "_.-@"))
                తిరిగి false;

        తిరిగి true;
}

పూర్ణాంకం locale_is_installed(స్థిరరాశి అక్షరం *name) {
        ఒకవేళ (!locale_is_valid(name))
                తిరిగి false;

        ఒకవేళ (STR_లోపల_సమితి(name, "C", "POSIX")) /* These ones are always OK */
                తిరిగి true;

#నిర్వచితమైతే __GLIBC__
        _cleanup_(freelocalep) locale_t loc = newlocale(LC_ALL_MASK, name, (locale_t) 0);
        ఒకవేళ (loc == (locale_t) 0)
                తిరిగి errno == ENOMEM ? -ENOMEM : false;

        తిరిగి true;
#లేకపోతే
        /* musl also has C.UTF-8 as builtin */
        ఒకవేళ (streq(name, "C.UTF-8"))
                తిరిగి true;

        /* musl's newlocale() always succeeds and provides a fake locale object even when the locale does
         * not exist. Hence, we need to explicitly check if the locale file exists. */
        స్థిరరాశి అక్షరం *e = secure_getenv("SYSTEMD_LOCALE_DIRECTORY");

        STRV_FOREACH(d, e ? STRV_MAKE_CONST(e) : musl_locale_dirs) {
                _cleanup_free_ అక్షరం *p = మార్గం_join(*d, name);
                ఒకవేళ (!p)
                        తిరిగి -ENOMEM;

                ఒకవేళ (access(p, F_OK) >= 0)
                        తిరిగి true;
        }

        తిరిగి false;
#అయితే_ముగింపు
}

స్థిర bool is_locale_utf8_impl(శూన్య) {
        స్థిరరాశి అక్షరం *సమితి;
        పూర్ణాంకం r;

        /* Note that we default to 'true' here, since today UTF8 is pretty much supported everywhere. */

        r = secure_getenv_bool("SYSTEMD_UTF8");
        ఒకవేళ (r >= 0)
                తిరిగి r;
        ఒకవేళ (r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_UTF8, ignoring: %m");

        /* This function may be called from libsystemd, and setlocale() is not thread safe. Assuming yes. */
        ఒకవేళ (!is_main_thread())
                తిరిగి true;

        ఒకవేళ (!setlocale(LC_ALL, ""))
                తిరిగి true;

        సమితి = nl_langinfo(CODESET);
        ఒకవేళ (!సమితి || streq(సమితి, "UTF-8"))
                తిరిగి true;

        సమితి = setlocale(LC_CTYPE, ఖాళీ);
        ఒకవేళ (!సమితి)
                తిరిగి true;

        /* Unless LC_CTYPE is explicitly overridden, return true. Because here CTYPE is effectively unset
         * and everything can do to UTF-8 nowadays. */
        తిరిగి STR_లోపల_సమితి(సమితి, "C", "POSIX") &&
                !getenv("LC_ALL") &&
                !getenv("LC_CTYPE") &&
                !getenv("LANG");
}

bool is_locale_utf8(శూన్య) {
        స్థిర పూర్ణాంకం cached = -1;

        ఒకవేళ (cached < 0)
                cached = is_locale_utf8_impl();

        తిరిగి cached;
}

శూన్య locale_variables_free(అక్షరం *l[_VARIABLE_LC_MAX]) {
        free_many_charp(l, _VARIABLE_LC_MAX);
}

శూన్య locale_variables_simplify(అక్షరం *l[_VARIABLE_LC_MAX]) {
        ప్రతిజ్ఞా(l);

        కొరకు (LocaleVariable p = 0; p < _VARIABLE_LC_MAX; p++) {
                ఒకవేళ (p == VARIABLE_LANG)
                        కొనసాగు;
                ఒకవేళ (isempty(l[p]) || streq_ptr(l[VARIABLE_LANG], l[p]))
                        l[p] = mfree(l[p]);
        }
}

స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి locale_variable_పట్టిక[_VARIABLE_LC_MAX] = {
        [VARIABLE_LANG]              = "LANG",
        [VARIABLE_LANGUAGE]          = "LANGUAGE",
        [VARIABLE_LC_CTYPE]          = "LC_CTYPE",
        [VARIABLE_LC_NUMERIC]        = "LC_NUMERIC",
        [VARIABLE_LC_సమయం]           = "LC_TIME",
        [VARIABLE_LC_COLLATE]        = "LC_COLLATE",
        [VARIABLE_LC_MONETARY]       = "LC_MONETARY",
        [VARIABLE_LC_MESSAGES]       = "LC_MESSAGES",
        [VARIABLE_LC_PAPER]          = "LC_PAPER",
        [VARIABLE_LC_NAME]           = "LC_NAME",
        [VARIABLE_LC_చిరునామా]        = "LC_ADDRESS",
        [VARIABLE_LC_TELEPHONE]      = "LC_TELEPHONE",
        [VARIABLE_LC_MEASUREMENT]    = "LC_MEASUREMENT",
        [VARIABLE_LC_IDENTIFICATION] = "LC_IDENTIFICATION"
};

DEFINE_STRING_పట్టిక_LOOKUP(locale_variable, LocaleVariable);
