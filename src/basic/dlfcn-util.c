/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "dlfcn-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "log.h"

शून्य* safe_dlclose(शून्य *dl) {
        जर (!dl)
                परत रिक्त;

        assert_se(dlclose(dl) == 0);
        परत रिक्त;
}

स्थिर पूर्णांक dlsym_many_or_warnv(शून्य *dl, पूर्णांक log_level, बहु_सूची ap) {
        शून्य (**fn)(शून्य);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        पर्यंत ((fn = बहु_तर्क(ap, typeof(fn)))) {
                शून्य (*tfn)(शून्य);
                स्थिरांक अक्षर *चिन्ह;

                चिन्ह = बहु_तर्क(ap, typeof(चिन्ह));

                tfn = (typeof(tfn)) dlsym(dl, चिन्ह);
                जर (!tfn)
                        परत log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", चिन्ह, dlerror());
                *fn = tfn;
        }

        परत 0;
}

पूर्णांक dlsym_many_or_warn_sentinel(शून्य *dl, पूर्णांक log_level, ...) {
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        बहु_पूर्ण(ap);

        परत r;
}

पूर्णांक dlopen_verbose(शून्य **dlp, स्थिरांक अक्षर *filename, पूर्णांक log_level) {
        पूर्णांक r;

        प्रतिज्ञा(dlp);

        जर (*dlp)
                परत 0; /* Already loaded */

        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        स्थिरांक अक्षर *dle = रिक्त;
        r = dlopen_safe(filename, &dl, &dle);
        जर (r < 0)
                परत log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        परत 1;
}

पूर्णांक dlopen_many_sym_or_warn_sentinel(शून्य **dlp, स्थिरांक अक्षर *filename, पूर्णांक log_level, ...) {
        पूर्णांक r;

        प्रतिज्ञा(dlp);

        जर (*dlp)
                परत 0; /* Already loaded */

        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        r = dlopen_verbose(&dl, filename, log_level);
        जर (r < 0)
                परत r;

        बहु_सूची ap;
        बहु_शुरू(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        परत 1;
}

स्थिर bool dlopen_blocked = false;

शून्य block_dlopen(शून्य) {
        dlopen_blocked = true;
}

पूर्णांक dlopen_safe(स्थिरांक अक्षर *filename, शून्य **ret, स्थिरांक अक्षर **reterr_dlerror) {
#जर BUILD_STATIC
        परत log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#अन्यथा
        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        पूर्णांक flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        जर (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        त्रुटि_सं = 0;
        dl = dlopen(filename, flags);
        जर (!dl) {
                जर (dlopen_blocked) {
                        (शून्य) dlerror(); /* consume error, so that no later call will return it */

                        जर (reterr_dlerror)
                                *reterr_dlerror = रिक्त;

                        परत log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                जर (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                अन्यथा
                        (शून्य) dlerror(); /* consume error, so that no later call will return it */

                परत r;
        }

        जर (ret)
                *ret = TAKE_PTR(dl);

        परत 0;
#endif
}
