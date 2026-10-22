/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "dlfcn-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "log.h"

శూన్య* సురక్షిత_dlclose(శూన్య *dl) {
        ఒకవేళ (!dl)
                తిరిగి ఖాళీ;

        assert_se(dlclose(dl) == 0);
        తిరిగి ఖాళీ;
}

స్థిర పూర్ణాంకం dlsym_many_or_warnv(శూన్య *dl, పూర్ణాంకం log_level, va_list ap) {
        శూన్య (**fn)(శూన్య);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        అయితే ((fn = va_arg(ap, typeof(fn)))) {
                శూన్య (*tfn)(శూన్య);
                స్థిరరాశి అక్షరం *చిహ్నం;

                చిహ్నం = va_arg(ap, typeof(చిహ్నం));

                tfn = (typeof(tfn)) dlsym(dl, చిహ్నం);
                ఒకవేళ (!tfn)
                        తిరిగి log_నిండిన_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", చిహ్నం, dlerror());
                *fn = tfn;
        }

        తిరిగి 0;
}

పూర్ణాంకం dlsym_many_or_warn_sentinel(శూన్య *dl, పూర్ణాంకం log_level, ...) {
        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        తిరిగి r;
}

పూర్ణాంకం dlopen_verbose(శూన్య **dlp, స్థిరరాశి అక్షరం *filename, పూర్ణాంకం log_level) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dlp);

        ఒకవేళ (*dlp)
                తిరిగి 0; /* Already loaded */

        _cleanup_(dlclosep) శూన్య *dl = ఖాళీ;
        స్థిరరాశి అక్షరం *dle = ఖాళీ;
        r = dlopen_సురక్షిత(filename, &dl, &dle);
        ఒకవేళ (r < 0)
                తిరిగి log_నిండిన_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        తిరిగి 1;
}

పూర్ణాంకం dlopen_many_sym_or_warn_sentinel(శూన్య **dlp, స్థిరరాశి అక్షరం *filename, పూర్ణాంకం log_level, ...) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dlp);

        ఒకవేళ (*dlp)
                తిరిగి 0; /* Already loaded */

        _cleanup_(dlclosep) శూన్య *dl = ఖాళీ;
        r = dlopen_verbose(&dl, filename, log_level);
        ఒకవేళ (r < 0)
                తిరిగి r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        తిరిగి 1;
}

స్థిర bool dlopen_blocked = false;

శూన్య ఖండం_dlopen(శూన్య) {
        dlopen_blocked = true;
}

పూర్ణాంకం dlopen_సురక్షిత(స్థిరరాశి అక్షరం *filename, శూన్య **ret, స్థిరరాశి అక్షరం **reterr_dlerror) {
#ఒకవేళ BUILD_STATIC
        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#లేకపోతే
        _cleanup_(dlclosep) శూన్య *dl = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        పూర్ణాంకం flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ఒకవేళ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ఒకవేళ (!dl) {
                ఒకవేళ (dlopen_blocked) {
                        (శూన్య) dlerror(); /* consume error, so that no later call will return it */

                        ఒకవేళ (reterr_dlerror)
                                *reterr_dlerror = ఖాళీ;

                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ఒకవేళ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                లేకపోతే
                        (శూన్య) dlerror(); /* consume error, so that no later call will return it */

                తిరిగి r;
        }

        ఒకవేళ (ret)
                *ret = TAKE_PTR(dl);

        తిరిగి 0;
#అయితే_ముగింపు
}
