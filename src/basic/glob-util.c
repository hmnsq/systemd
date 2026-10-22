/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <dirent.h>
#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "glob-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

#నిర్వచితం_కాకపోతే __GLIBC__
స్థిర bool సురక్షిత_glob_verify(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *ఉపసర్గ) {
        సూచిక_MAY_BE_NULL(p);
        సూచిక_MAY_BE_NULL(ఉపసర్గ);

        ఒకవేళ (isempty(p))
                తిరిగి false; /* should not happen, but for safey. */

        ఒకవేళ (ఉపసర్గ) {
                /* Skip the prefix, as we allow dots in prefix.
                 * Note, glob() does not normalize paths, hence do not use path_startswith(). */
                p = startswith(p, ఉపసర్గ);
                ఒకవేళ (!p)
                        తిరిగి false; /* should not happen, but for safety. */
        }

        కొరకు (;;) {
                p += strspn(p, "/");
                ఒకవేళ (*p == '\0')
                        తిరిగి true;
                ఒకవేళ (*p == '.') {
                        p++;
                        ఒకవేళ (లోపల_సమితి(*p, '/', '\0'))
                                తిరిగి false; /* refuse dot */
                        ఒకవేళ (*p == '.') {
                                p++;
                                ఒకవేళ (లోపల_సమితి(*p, '/', '\0'))
                                        తిరిగి false; /* refuse dot dot */
                        }
                }

                p += strcspn(p, "/");
                ఒకవేళ (*p == '\0')
                        తిరిగి true;
        }
}

స్థిర పూర్ణాంకం filter_glob_ఫలితం(అక్షరం * స్థిరరాశి *ఫలితం, స్థిరరాశి అక్షరం *మార్గం, అక్షరం ***ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        ఒకవేళ (strv_isempty(ఫలితం))
                తిరిగి -ENOENT;

        _cleanup_free_ అక్షరం *ఉపసర్గ = ఖాళీ;
        r = glob_non_glob_ఉపసర్గ(మార్గం, &ఉపసర్గ);
        ఒకవేళ (r < 0 && r != -ENOENT)
                తిరిగి r;

        _cleanup_strv_free_ అక్షరం **filtered = ఖాళీ;
        size_t n_filtered = 0;
        STRV_FOREACH(p, ఫలితం) {
                ఒకవేళ (!సురక్షిత_glob_verify(*p, ఉపసర్గ))
                        కొనసాగు;

                ఒకవేళ (!ret)
                        తిరిగి 0; /* Found at least one entry, let's return earlier. */

                /* When musl is used, each entry is not a head of allocated memory. Hence, it is
                 * necessary to copy the string. */
                r = strv_extend_with_కొలత(&filtered, &n_filtered, *p);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (n_filtered == 0)
                తిరిగి -ENOENT;

        ప్రతిజ్ఞా(ret);
        *ret = TAKE_PTR(filtered);
        తిరిగి 0;
}
#అయితే_ముగింపు

DEFINE_TRIVIAL_DESTRUCTOR(closedir_wrapper, శూన్య, closedir);

పూర్ణాంకం సురక్షిత_glob_నిండిన(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, opendir_t opendir_func, అక్షరం ***ret) {
        _cleanup_(globfree) glob_t g = {
                .gl_closedir = closedir_wrapper,
                .gl_readdir = (నిర్మాణం dirent* (*)(శూన్య *)) readdir_no_dot,
                .gl_opendir = (శూన్య* (*)(స్థిరరాశి అక్షరం *)) (opendir_func ?: opendir),
                .gl_lstat = lstat,
                .gl_stat = stat,
        };
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        errno = 0;
        r = glob(మార్గం, flags | GLOB_ALTDIRFUNC, ఖాళీ, &g);
        ఒకవేళ (r == GLOB_NOMATCH)
                తిరిగి -ENOENT;
        ఒకవేళ (r == GLOB_NOSPACE)
                తిరిగి -ENOMEM;
        ఒకవేళ (r != 0)
                తిరిగి errno_or_else(EIO);

#నిర్వచితం_కాకపోతే __GLIBC__
        తిరిగి filter_glob_ఫలితం(g.gl_pathv, మార్గం, ret);
#అయితే_ముగింపు

        ఒకవేళ (strv_isempty(g.gl_pathv))
                తిరిగి -ENOENT;

        ఒకవేళ (ret) {
                *ret = g.gl_pathv;
                TAKE_STRUCT(g); /* To avoid the result being freed. */
        }

        తిరిగి 0;
}

పూర్ణాంకం glob_మొదటి(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        _cleanup_strv_free_ అక్షరం **v = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        r = సురక్షిత_glob(మార్గం, GLOB_NOSORT|GLOB_BRACE, &v);
        ఒకవేళ (r == -ENOENT) {
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి false;
        }
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రతిజ్ఞా(!strv_isempty(v));

        ఒకవేళ (ret) {
                /* Free all results except for the first one. */
                STRV_FOREACH(p, strv_skip(v, 1))
                        *p = mfree(*p);

                /* Then, take the first result. */
                *ret = TAKE_PTR(*v);
        }

        తిరిగి true;
}

పూర్ణాంకం glob_extend(అక్షరం ***strv, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags) {
        అక్షరం **v;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        r = సురక్షిత_glob(మార్గం, GLOB_NOSORT|GLOB_BRACE|flags, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి strv_extend_strv_consume(strv, v, /* filter_duplicates= */ false);
}

పూర్ణాంకం glob_non_glob_ఉపసర్గ(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret);

        /* Return the path of the path that has no glob characters. */

        size_t n = strcspn(మార్గం, GLOB_CHARS);

        ఒకవేళ (మార్గం[n] != '\0')
                అయితే (n > 0 && మార్గం[n-1] != '/')
                        n--;

        ఒకవేళ (n == 0)
                తిరిగి -ENOENT;

        అక్షరం *ans = strndup(మార్గం, n);
        ఒకవేళ (!ans)
                తిరిగి -ENOMEM;
        *ret = ans;
        తిరిగి 0;
}

bool string_is_glob(స్థిరరాశి అక్షరం *p) {
        /* Check if a string contains any glob patterns. */
        తిరిగి !!strpbrk(p, GLOB_CHARS);
}
