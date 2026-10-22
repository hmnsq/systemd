/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <dlfcn.h>      /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

শূন্য* safe_dlclose(শূন্য *dl);

স্থির অন্তঃস্থ শূন্য dlclosep(শূন্য **dlp) {
        safe_dlclose(*dlp);
}

পূর্ণাঙ্ক dlopen_verbose(শূন্য **dlp, ধ্রুব বর্ণ *filename, পূর্ণাঙ্ক log_level);
পূর্ণাঙ্ক dlsym_many_or_warn_sentinel(শূন্য *dl, পূর্ণাঙ্ক log_level, ...) _sentinel_;
পূর্ণাঙ্ক dlopen_many_sym_or_warn_sentinel(শূন্য **dlp, ধ্রুব বর্ণ *filename, পূর্ণাঙ্ক log_level, ...) _sentinel_;

#define dlsym_many_or_warn(dl, log_level, ...) \
        dlsym_many_or_warn_sentinel(dl, log_level, __VA_ARGS__, রিক্ত)
#define dlopen_many_sym_or_warn(dlp, filename, log_level, ...) \
        dlopen_many_sym_or_warn_sentinel(dlp, filename, log_level, __VA_ARGS__, রিক্ত)

#define DLSYM_PROTOTYPE(প্রতীক)                \
        typeof(প্রতীক)* sym_##symbol

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
                (শূন্য) dlerror();                                                               \
                typeof(sym_##name) _v = (typeof(sym_##name)) dlsym((dl), #name);                \
                যদি (_v) sym_##name = _v;                                                        \
        })

/* If called dlopen_many_sym_or_warn() will fail with EPERM. This can be used to block lazy loading of shared
 * libs, if we transfer a process into a different namespace. Note that this does not work for all calls of
 * dlopen(), just those through our dlopen_safe() wrapper (which we use comprehensively in our
 * codebase). This hence has *no* effect on NSS. (Would be great if we could change that...) */
শূন্য block_dlopen(শূন্য);

পূর্ণাঙ্ক dlopen_safe(ধ্রুব বর্ণ *filename, শূন্য **ret, ধ্রুব বর্ণ **reterr_dlerror);
