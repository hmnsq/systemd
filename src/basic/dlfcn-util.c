/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "dlfcn-util.h"
#include "errno-util.h"
#include "log.h"

void* safe_dlclose(void *dl) {
        ако (!dl)
                врати NULL;

        assert_se(dlclose(dl) == 0);
        врати NULL;
}

static цео dlsym_many_or_warnv(void *dl, цео log_level, va_list ap) {
        void (**fn)(void);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        док ((fn = va_arg(ap, typeof(fn)))) {
                void (*tfn)(void);
                const char *symbol;

                symbol = va_arg(ap, typeof(symbol));

                tfn = (typeof(tfn)) dlsym(dl, symbol);
                ако (!tfn)
                        врати log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", symbol, dlerror());
                *fn = tfn;
        }

        врати 0;
}

цео dlsym_many_or_warn_sentinel(void *dl, цео log_level, ...) {
        va_list ap;
        цео r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        врати r;
}

цео dlopen_verbose(void **dlp, const char *filename, цео log_level) {
        цео r;

        assert(dlp);

        ако (*dlp)
                врати 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        const char *dle = NULL;
        r = dlopen_safe(filename, &dl, &dle);
        ако (r < 0)
                врати log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        врати 1;
}

цео dlopen_many_sym_or_warn_sentinel(void **dlp, const char *filename, цео log_level, ...) {
        цео r;

        assert(dlp);

        ако (*dlp)
                врати 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        r = dlopen_verbose(&dl, filename, log_level);
        ако (r < 0)
                врати r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ако (r < 0)
                врати r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        врати 1;
}

static bool dlopen_blocked = false;

void block_dlopen(void) {
        dlopen_blocked = true;
}

цео dlopen_safe(const char *filename, void **ret, const char **reterr_dlerror) {
#ако BUILD_STATIC
        врати log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#инако
        _cleanup_(dlclosep) void *dl = NULL;
        цео r;

        assert(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        цео flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ако (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ако (!dl) {
                ако (dlopen_blocked) {
                        (void) dlerror(); /* consume error, so that no later call will return it */

                        ако (reterr_dlerror)
                                *reterr_dlerror = NULL;

                        врати log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ако (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                инако
                        (void) dlerror(); /* consume error, so that no later call will return it */

                врати r;
        }

        ако (ret)
                *ret = TAKE_PTR(dl);

        врати 0;
#endif
}
