/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "dlfcn-util.h"
#include "errno-util.h"
#include "log.h"

void* safe_dlclose(void *dl) {
        만약 (!dl)
                반환 NULL;

        assert_se(dlclose(dl) == 0);
        반환 NULL;
}

static 정수 dlsym_many_or_warnv(void *dl, 정수 log_level, va_list ap) {
        void (**fn)(void);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        동안 ((fn = va_arg(ap, typeof(fn)))) {
                void (*tfn)(void);
                const char *symbol;

                symbol = va_arg(ap, typeof(symbol));

                tfn = (typeof(tfn)) dlsym(dl, symbol);
                만약 (!tfn)
                        반환 log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", symbol, dlerror());
                *fn = tfn;
        }

        반환 0;
}

정수 dlsym_many_or_warn_sentinel(void *dl, 정수 log_level, ...) {
        va_list ap;
        정수 r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        반환 r;
}

정수 dlopen_verbose(void **dlp, const char *filename, 정수 log_level) {
        정수 r;

        assert(dlp);

        만약 (*dlp)
                반환 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        const char *dle = NULL;
        r = dlopen_safe(filename, &dl, &dle);
        만약 (r < 0)
                반환 log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        반환 1;
}

정수 dlopen_many_sym_or_warn_sentinel(void **dlp, const char *filename, 정수 log_level, ...) {
        정수 r;

        assert(dlp);

        만약 (*dlp)
                반환 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        r = dlopen_verbose(&dl, filename, log_level);
        만약 (r < 0)
                반환 r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        만약 (r < 0)
                반환 r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        반환 1;
}

static bool dlopen_blocked = false;

void block_dlopen(void) {
        dlopen_blocked = true;
}

정수 dlopen_safe(const char *filename, void **ret, const char **reterr_dlerror) {
#만약 BUILD_STATIC
        반환 log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#아니면
        _cleanup_(dlclosep) void *dl = NULL;
        정수 r;

        assert(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        정수 flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        만약 (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        만약 (!dl) {
                만약 (dlopen_blocked) {
                        (void) dlerror(); /* consume error, so that no later call will return it */

                        만약 (reterr_dlerror)
                                *reterr_dlerror = NULL;

                        반환 log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                만약 (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                아니면
                        (void) dlerror(); /* consume error, so that no later call will return it */

                반환 r;
        }

        만약 (ret)
                *ret = TAKE_PTR(dl);

        반환 0;
#endif
}
