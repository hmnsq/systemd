/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "sd-dlopen.h"

#ضمن "forward.h"

/* Prevent dlopen helper functions (e.g., dlopen_libfoo()) from being inlined or cloned by the compiler/LTO.
 * This ensures that their specific symbols remain intact in the final executable, allowing the developer
 * test utility to verify that the functions corresponding to the dlopen ELF notes are actually invoked.
 * This is restricted to BUILD_MODE_DEVELOPER to avoid degrading production performance. */
#إذا BUILD_نما_DEVELOPER
#  define _dlopen_loader_ _noclone_ _noinline_
#وإلا
#  define _dlopen_loader_
#پایان_اگر

/* Avoid invalid priority. */
#تعریف_کن DLOPEN_CHECK_اولویت_required    1
#تعریف_کن DLOPEN_CHECK_اولویت_recommended 1
#تعریف_کن DLOPEN_CHECK_اولویت_suggested   1

/* Unlike SD_ELF_NOTE_DLOPEN_ANCHORED(), this takes feature and priority without quotation. */
#تعریف_کن ELF_NOTE_DLOPEN_ANCHORED(feature, description, priority, ...)   \
        assert_cc(DLOPEN_CHECK_اولویت##priority);                   \
        SD_ELF_NOTE_DLOPEN_ANCHORED(                                    \
                        feature##_##priority,                           \
                        STRINGIFY(feature),                             \
                        description,                                    \
                        STRINGIFY(priority),                            \
                        __VA_ARGS__)

#إذا HAVE_BZIP2
#  define LIBBZ2_NOTE(priority)                                         \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        bzip2,                                          \
                        "Support bzip2 compression and decompression",  \
                        priority,                                       \
                        "libbz2.so.1")
#وإلا
#  define LIBBZ2_NOTE(priority)
#پایان_اگر

#إذا HAVE_LZ4
#  define LIBLZ4_NOTE(priority)                                         \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        lz4,                                            \
                        "Support lz4 compression in journal and coredump files", \
                        priority,                                       \
                        "liblz4.so.1")
#وإلا
#  define LIBLZ4_NOTE(priority)
#پایان_اگر

#إذا HAVE_XZ
#  define LIBLZMA_NOTE(priority)                                        \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        lzma,                                           \
                        "Support lzma compression in journal and coredump files", \
                        priority,                                       \
                        "liblzma.so.5")
#وإلا
#  define LIBLZMA_NOTE(priority)
#پایان_اگر

#إذا HAVE_ZLIB
#  define LIBZ_NOTE(priority)                                           \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        zlib,                                           \
                        "Support gzip compression and decompression",   \
                        priority,                                       \
                        "libz.so.1")
#وإلا
#  define LIBZ_NOTE(priority)
#پایان_اگر

#إذا HAVE_ZSTD
#  define LIBZSTD_NOTE(priority)                                        \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        zstd,                                           \
                        "Support zstd compression in journal and coredump files", \
                        priority,                                       \
                        "libzstd.so.1")
#وإلا
#  define LIBZSTD_NOTE(priority)
#پایان_اگر

#تعریف_کن COMPRESS_NOTE(priority)                         \
        LIBBZ2_NOTE(priority);                          \
        LIBLZ4_NOTE(priority);                          \
        LIBLZMA_NOTE(priority);                         \
        LIBZ_NOTE(priority);                            \
        LIBZSTD_NOTE(priority)

#تعریف_کن COMPRESS_DEFAULT_NOTE                           \
        COMPRESS_JOURNAL_NOTE;                          \
        LIBBZ2_NOTE(suggested);                         \
        LIBZ_NOTE(suggested)

#تعریف_کن COMPRESS_JOURNAL_NOTE                           \
        LIBLZ4_NOTE(COMPRESSION_اولویت_LZ4);          \
        LIBLZMA_NOTE(COMPRESSION_اولویت_XZ);          \
        LIBZSTD_NOTE(COMPRESSION_اولویت_ZSTD)
