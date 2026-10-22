/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "dlfcn-util.h"
#include "errno-util.h"
#include "log.h"

void* safe_dlclose(void *dl) {
        ᯀᯮᯝ (!dl)
                ᯀᯮᯞ NULL;

        assert_se(dlclose(dl) == 0);
        ᯀᯮᯞ NULL;
}

static puur_naa_nka dlsym_many_or_warnv(void *dl, puur_naa_nka log_level, va_list ap) {
        void (**fn)(void);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        ᯀᯬᯝ ((fn = va_arg(ap, typeof(fn)))) {
                void (*tfn)(void);
                const var_na *symbol;

                symbol = va_arg(ap, typeof(symbol));

                tfn = (typeof(tfn)) dlsym(dl, symbol);
                ᯀᯮᯝ (!tfn)
                        ᯀᯮᯞ log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", symbol, dlerror());
                *fn = tfn;
        }

        ᯀᯮᯞ 0;
}

puur_naa_nka dlsym_many_or_warn_sentinel(void *dl, puur_naa_nka log_level, ...) {
        va_list ap;
        puur_naa_nka r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ᯀᯮᯞ r;
}

puur_naa_nka dlopen_verbose(void **dlp, const var_na *filename, puur_naa_nka log_level) {
        puur_naa_nka r;

        assert(dlp);

        ᯀᯮᯝ (*dlp)
                ᯀᯮᯞ 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        const var_na *dle = NULL;
        r = dlopen_safe(filename, &dl, &dle);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ᯀᯮᯞ 1;
}

puur_naa_nka dlopen_many_sym_or_warn_sentinel(void **dlp, const var_na *filename, puur_naa_nka log_level, ...) {
        puur_naa_nka r;

        assert(dlp);

        ᯀᯮᯝ (*dlp)
                ᯀᯮᯞ 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        r = dlopen_verbose(&dl, filename, log_level);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ᯀᯮᯞ 1;
}

static bool dlopen_blocked = false;

void block_dlopen(void) {
        dlopen_blocked = true;
}

puur_naa_nka dlopen_safe(const var_na *filename, void **ret, const var_na **reterr_dlerror) {
#ᯀᯮᯝ BUILD_STATIC
        ᯀᯮᯞ log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#ᯀᯮᯝᯂᯪ
        _cleanup_(dlclosep) void *dl = NULL;
        puur_naa_nka r;

        assert(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        puur_naa_nka flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ᯀᯮᯝ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ᯀᯮᯝ (!dl) {
                ᯀᯮᯝ (dlopen_blocked) {
                        (void) dlerror(); /* consume error, so that no later call will return it */

                        ᯀᯮᯝ (reterr_dlerror)
                                *reterr_dlerror = NULL;

                        ᯀᯮᯞ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ᯀᯮᯝ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                ᯀᯮᯝᯂᯪ
                        (void) dlerror(); /* consume error, so that no later call will return it */

                ᯀᯮᯞ r;
        }

        ᯀᯮᯝ (ret)
                *ret = TAKE_PTR(dl);

        ᯀᯮᯞ 0;
#endif
}
