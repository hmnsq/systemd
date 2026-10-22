/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <dlfcn.h>      /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

வெறுமை* safe_dlclose(வெறுமை *dl);

நிலையான உள்வரி வெறுமை dlclosep(வெறுமை **dlp) {
        safe_dlclose(*dlp);
}

முழு dlopen_verbose(வெறுமை **dlp, மாறிலி எழுத்து *filename, முழு log_level);
முழு dlsym_many_or_warn_sentinel(வெறுமை *dl, முழு log_level, ...) _sentinel_;
முழு dlopen_many_sym_or_warn_sentinel(வெறுமை **dlp, மாறிலி எழுத்து *filename, முழு log_level, ...) _sentinel_;

#define dlsym_many_or_warn(dl, log_level, ...) \
        dlsym_many_or_warn_sentinel(dl, log_level, __VA_ARGS__, வெற்று)
#define dlopen_many_sym_or_warn(dlp, filename, log_level, ...) \
        dlopen_many_sym_or_warn_sentinel(dlp, filename, log_level, __VA_ARGS__, வெற்று)

#define DLSYM_PROTOTYPE(குறியீடு)                \
        typeof(குறியீடு)* sym_##symbol

/* Macro useful for putting together variable/symbol name pairs when calling dlsym_many_or_warn(). Assumes
 * that each library symbol to resolve will be placed in a variable with the "sym_" prefix, i.e. a symbol
 * "foobar" is loaded into a variable "sym_foobar". */
#define DLSYM_ARG(arg) \
        ({ assert_cc(__builtin_types_compatible_p(typeof(sym_##arg), typeof(&arg))); &sym_##arg; }), STRINGIFY(arg)

/* libbpf is a bit confused about type-safety and API compatibility. Provide a macro that can tape over that mess. Sad. */
#define DLSYM_ARG_FORCE(arg) \
        &sym_##arg, STRINGIFY(arg)

/* Resolve a single optional symbol from an already-opened library handle. The pointer variable is expected
 * to be named sym_<name> (same convention as DLSYM_ARG). Only assigns on success, so the pointer keeps its
 * pre-existing value if the symbol is not present — useful for fallback initialization. dlerror() is
 * cleared first so callers can distinguish "symbol not found" from "symbol's value is NULL" by checking
 * dlerror() after; for function symbols (which can never be NULL on success) the _v check below is
 * sufficient. */
#define DLSYM_OPTIONAL(dl, name)                                                                \
        ({                                                                                      \
                (வெறுமை) dlerror();                                                               \
                typeof(sym_##name) _v = (typeof(sym_##name)) dlsym((dl), #name);                \
                எனில் (_v) sym_##name = _v;                                                        \
        })

/* If called dlopen_many_sym_or_warn() will fail with EPERM. This can be used to block lazy loading of shared
 * libs, if we transfer a process into a different namespace. Note that this does not work for all calls of
 * dlopen(), just those through our dlopen_safe() wrapper (which we use comprehensively in our
 * codebase). This hence has *no* effect on NSS. (Would be great if we could change that...) */
வெறுமை block_dlopen(வெறுமை);

முழு dlopen_safe(மாறிலி எழுத்து *filename, வெறுமை **ret, மாறிலி எழுத்து **reterr_dlerror);
