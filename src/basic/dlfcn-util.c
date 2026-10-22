/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "dlfcn-util.h"
#समावेश "errno-util.h"
#समावेश "log.h"

व्योम* safe_dlclose(व्योम *dl) {
        अगर (!dl)
                वापस शून्य;

        assert_se(dlclose(dl) == 0);
        वापस शून्य;
}

अटल पूर्णांक dlsym_many_or_warnv(व्योम *dl, पूर्णांक log_level, बहु_सूची ap) {
        व्योम (**fn)(व्योम);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        जबतक ((fn = बहु_तर्क(ap, typeof(fn)))) {
                व्योम (*tfn)(व्योम);
                स्थिर अक्षर *प्रतीक;

                प्रतीक = बहु_तर्क(ap, typeof(प्रतीक));

                tfn = (typeof(tfn)) dlsym(dl, प्रतीक);
                अगर (!tfn)
                        वापस log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", प्रतीक, dlerror());
                *fn = tfn;
        }

        वापस 0;
}

पूर्णांक dlsym_many_or_warn_sentinel(व्योम *dl, पूर्णांक log_level, ...) {
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

पूर्णांक dlopen_verbose(व्योम **dlp, स्थिर अक्षर *filename, पूर्णांक log_level) {
        पूर्णांक r;

        निश्चित(dlp);

        अगर (*dlp)
                वापस 0; /* Already loaded */

        _cleanup_(dlclosep) व्योम *dl = शून्य;
        स्थिर अक्षर *dle = शून्य;
        r = dlopen_safe(filename, &dl, &dle);
        अगर (r < 0)
                वापस log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        वापस 1;
}

पूर्णांक dlopen_many_sym_or_warn_sentinel(व्योम **dlp, स्थिर अक्षर *filename, पूर्णांक log_level, ...) {
        पूर्णांक r;

        निश्चित(dlp);

        अगर (*dlp)
                वापस 0; /* Already loaded */

        _cleanup_(dlclosep) व्योम *dl = शून्य;
        r = dlopen_verbose(&dl, filename, log_level);
        अगर (r < 0)
                वापस r;

        बहु_सूची ap;
        बहु_शुरू(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        वापस 1;
}

अटल bool dlopen_blocked = false;

व्योम block_dlopen(व्योम) {
        dlopen_blocked = true;
}

पूर्णांक dlopen_safe(स्थिर अक्षर *filename, व्योम **ret, स्थिर अक्षर **reterr_dlerror) {
#अगर BUILD_STATIC
        वापस log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#अन्यथा
        _cleanup_(dlclosep) व्योम *dl = शून्य;
        पूर्णांक r;

        निश्चित(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        पूर्णांक flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        अगर (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        त्रुटि_सं = 0;
        dl = dlopen(filename, flags);
        अगर (!dl) {
                अगर (dlopen_blocked) {
                        (व्योम) dlerror(); /* consume error, so that no later call will return it */

                        अगर (reterr_dlerror)
                                *reterr_dlerror = शून्य;

                        वापस log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                अगर (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                अन्यथा
                        (व्योम) dlerror(); /* consume error, so that no later call will return it */

                वापस r;
        }

        अगर (ret)
                *ret = TAKE_PTR(dl);

        वापस 0;
#पूर्ण_अगर
}
