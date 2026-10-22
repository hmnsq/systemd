/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "dlfcn-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "log.h"

શૂન્ય* safe_dlclose(શૂન્ય *dl) {
        જો (!dl)
                પાછા ખાલી;

        assert_se(dlclose(dl) == 0);
        પાછા ખાલી;
}

સ્થિર પૂર્ણાંક dlsym_many_or_warnv(શૂન્ય *dl, પૂર્ણાંક log_level, va_list ap) {
        શૂન્ય (**fn)(શૂન્ય);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        જ્યારે ((fn = va_arg(ap, typeof(fn)))) {
                શૂન્ય (*tfn)(શૂન્ય);
                અચલ અક્ષર *પ્રતીક;

                પ્રતીક = va_arg(ap, typeof(પ્રતીક));

                tfn = (typeof(tfn)) dlsym(dl, પ્રતીક);
                જો (!tfn)
                        પાછા log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", પ્રતીક, dlerror());
                *fn = tfn;
        }

        પાછા 0;
}

પૂર્ણાંક dlsym_many_or_warn_sentinel(શૂન્ય *dl, પૂર્ણાંક log_level, ...) {
        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        પાછા r;
}

પૂર્ણાંક dlopen_verbose(શૂન્ય **dlp, અચલ અક્ષર *filename, પૂર્ણાંક log_level) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dlp);

        જો (*dlp)
                પાછા 0; /* Already loaded */

        _cleanup_(dlclosep) શૂન્ય *dl = ખાલી;
        અચલ અક્ષર *dle = ખાલી;
        r = dlopen_safe(filename, &dl, &dle);
        જો (r < 0)
                પાછા log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        પાછા 1;
}

પૂર્ણાંક dlopen_many_sym_or_warn_sentinel(શૂન્ય **dlp, અચલ અક્ષર *filename, પૂર્ણાંક log_level, ...) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dlp);

        જો (*dlp)
                પાછા 0; /* Already loaded */

        _cleanup_(dlclosep) શૂન્ય *dl = ખાલી;
        r = dlopen_verbose(&dl, filename, log_level);
        જો (r < 0)
                પાછા r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        જો (r < 0)
                પાછા r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        પાછા 1;
}

સ્થિર bool dlopen_blocked = false;

શૂન્ય block_dlopen(શૂન્ય) {
        dlopen_blocked = true;
}

પૂર્ણાંક dlopen_safe(અચલ અક્ષર *filename, શૂન્ય **ret, અચલ અક્ષર **reterr_dlerror) {
#જો BUILD_STATIC
        પાછા log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#નહીંતર
        _cleanup_(dlclosep) શૂન્ય *dl = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        પૂર્ણાંક flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        જો (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        જો (!dl) {
                જો (dlopen_blocked) {
                        (શૂન્ય) dlerror(); /* consume error, so that no later call will return it */

                        જો (reterr_dlerror)
                                *reterr_dlerror = ખાલી;

                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                જો (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                નહીંતર
                        (શૂન્ય) dlerror(); /* consume error, so that no later call will return it */

                પાછા r;
        }

        જો (ret)
                *ret = TAKE_PTR(dl);

        પાછા 0;
#endif
}
