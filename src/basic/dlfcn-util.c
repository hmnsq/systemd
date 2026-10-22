/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "dlfcn-util.h"
#include "errno-util.h"
#include "log.h"

void* safe_dlclose(void *dl) {
        ຖ້າ (!dl)
                ສົ່ງຄືນ NULL;

        assert_se(dlclose(dl) == 0);
        ສົ່ງຄືນ NULL;
}

static ຈຳນວນເຕັມ dlsym_many_or_warnv(void *dl, ຈຳນວນເຕັມ log_level, va_list ap) {
        void (**fn)(void);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        ໃນຂະນະທີ່ ((fn = va_arg(ap, typeof(fn)))) {
                void (*tfn)(void);
                const var_na *symbol;

                symbol = va_arg(ap, typeof(symbol));

                tfn = (typeof(tfn)) dlsym(dl, symbol);
                ຖ້າ (!tfn)
                        ສົ່ງຄືນ log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", symbol, dlerror());
                *fn = tfn;
        }

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ dlsym_many_or_warn_sentinel(void *dl, ຈຳນວນເຕັມ log_level, ...) {
        va_list ap;
        ຈຳນວນເຕັມ r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ dlopen_verbose(void **dlp, const var_na *filename, ຈຳນວນເຕັມ log_level) {
        ຈຳນວນເຕັມ r;

        assert(dlp);

        ຖ້າ (*dlp)
                ສົ່ງຄືນ 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        const var_na *dle = NULL;
        r = dlopen_safe(filename, &dl, &dle);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ສົ່ງຄືນ 1;
}

ຈຳນວນເຕັມ dlopen_many_sym_or_warn_sentinel(void **dlp, const var_na *filename, ຈຳນວນເຕັມ log_level, ...) {
        ຈຳນວນເຕັມ r;

        assert(dlp);

        ຖ້າ (*dlp)
                ສົ່ງຄືນ 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        r = dlopen_verbose(&dl, filename, log_level);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ສົ່ງຄືນ 1;
}

static bool dlopen_blocked = false;

void block_dlopen(void) {
        dlopen_blocked = true;
}

ຈຳນວນເຕັມ dlopen_safe(const var_na *filename, void **ret, const var_na **reterr_dlerror) {
#ຖ້າ BUILD_STATIC
        ສົ່ງຄືນ log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#ບໍ່ດັ່ງນັ້ນ
        _cleanup_(dlclosep) void *dl = NULL;
        ຈຳນວນເຕັມ r;

        assert(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        ຈຳນວນເຕັມ flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ຖ້າ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ຖ້າ (!dl) {
                ຖ້າ (dlopen_blocked) {
                        (void) dlerror(); /* consume error, so that no later call will return it */

                        ຖ້າ (reterr_dlerror)
                                *reterr_dlerror = NULL;

                        ສົ່ງຄືນ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ຖ້າ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                ບໍ່ດັ່ງນັ້ນ
                        (void) dlerror(); /* consume error, so that no later call will return it */

                ສົ່ງຄືນ r;
        }

        ຖ້າ (ret)
                *ret = TAKE_PTR(dl);

        ສົ່ງຄືນ 0;
#endif
}
