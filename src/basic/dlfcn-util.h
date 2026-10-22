/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <dlfcn.h>      /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಶೂನ್ಯ* ಸುರಕ್ಷಿತ_dlclose(ಶೂನ್ಯ *dl);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ dlclosep(ಶೂನ್ಯ **dlp) {
        ಸುರಕ್ಷಿತ_dlclose(*dlp);
}

ಪೂರ್ಣಾಂಕ dlopen_verbose(ಶೂನ್ಯ **dlp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಪೂರ್ಣಾಂಕ log_level);
ಪೂರ್ಣಾಂಕ dlsym_many_or_warn_sentinel(ಶೂನ್ಯ *dl, ಪೂರ್ಣಾಂಕ log_level, ...) _sentinel_;
ಪೂರ್ಣಾಂಕ dlopen_many_sym_or_warn_sentinel(ಶೂನ್ಯ **dlp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಪೂರ್ಣಾಂಕ log_level, ...) _sentinel_;

#ವ್ಯಾಖ್ಯಾನಿಸು dlsym_many_or_warn(dl, log_level, ...) \
        dlsym_many_or_warn_sentinel(dl, log_level, __VA_ARGS__, ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು dlopen_many_sym_or_warn(dlp, filename, log_level, ...) \
        dlopen_many_sym_or_warn_sentinel(dlp, filename, log_level, __VA_ARGS__, ಖಾಲಿ)

#ವ್ಯಾಖ್ಯಾನಿಸು DLSYM_PROTOTYPE(ಚಿಹ್ನೆ)                \
        typeof(ಚಿಹ್ನೆ)* sym_##symbol

/* Macro useful for putting together variable/symbol name pairs when calling dlsym_many_or_warn(). Assumes
 * that each library symbol to resolve will be placed in a variable with the "sym_" prefix, i.e. a symbol
 * "foobar" is loaded into a variable "sym_foobar". */
#ವ್ಯಾಖ್ಯಾನಿಸು DLSYM_ARG(arg) \
        ({ assert_cc(__builtin_types_compatible_p(typeof(sym_##arg), typeof(&arg))); &sym_##arg; }), STRINGIFY(arg)

/* libbpf is a bit confused about type-safety and API compatibility. Provide a macro that can tape over that mess. Sad. */
#ವ್ಯಾಖ್ಯಾನಿಸು DLSYM_ARG_FORCE(arg) \
        &sym_##arg, STRINGIFY(arg)

/* Resolve a single optional symbol from an already-opened library handle. The pointer variable is expected
 * to be named sym_<name> (same convention as DLSYM_ARG). Only assigns on success, so the pointer keeps its
 * pre-existing value if the symbol is not present — useful for fallback initialization. dlerror() is
 * cleared first so callers can distinguish "symbol not found" from "symbol's value is NULL" by checking
 * dlerror() after; for function symbols (which can never be NULL on success) the _v check below is
 * sufficient. */
#ವ್ಯಾಖ್ಯಾನಿಸು DLSYM_OPTIONAL(dl, name)                                                                \
        ({                                                                                      \
                (ಶೂನ್ಯ) dlerror();                                                               \
                typeof(sym_##name) _v = (typeof(sym_##name)) dlsym((dl), #name);                \
                ಆದರೆ (_v) sym_##name = _v;                                                        \
        })

/* If called dlopen_many_sym_or_warn() will fail with EPERM. This can be used to block lazy loading of shared
 * libs, if we transfer a process into a different namespace. Note that this does not work for all calls of
 * dlopen(), just those through our dlopen_safe() wrapper (which we use comprehensively in our
 * codebase). This hence has *no* effect on NSS. (Would be great if we could change that...) */
ಶೂನ್ಯ ಖಂಡ_dlopen(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ dlopen_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಶೂನ್ಯ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **reterr_dlerror);
