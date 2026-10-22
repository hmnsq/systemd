/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "sd-dlopen.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Prevent dlopen helper functions (e.g., dlopen_libfoo()) from being inlined or cloned by the compiler/LTO.
 * This ensures that their specific symbols remain intact in the final executable, allowing the developer
 * test utility to verify that the functions corresponding to the dlopen ELF notes are actually invoked.
 * This is restricted to BUILD_MODE_DEVELOPER to avoid degrading production performance. */
#ਜੇ BUILD_ਬਹੁਲਕ_DEVELOPER
#  define _dlopen_loader_ _noclone_ _noinline_
#ਨਹੀਂਤਾਂ
#  define _dlopen_loader_
#ਜੇ_ਅੰਤ

/* Avoid invalid priority. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLOPEN_CHECK_ਤਰਜੀਹ_required    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLOPEN_CHECK_ਤਰਜੀਹ_recommended 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLOPEN_CHECK_ਤਰਜੀਹ_suggested   1

/* Unlike SD_ELF_NOTE_DLOPEN_ANCHORED(), this takes feature and priority without quotation. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ELF_NOTE_DLOPEN_ANCHORED(feature, description, priority, ...)   \
        assert_cc(DLOPEN_CHECK_ਤਰਜੀਹ##priority);                   \
        SD_ELF_NOTE_DLOPEN_ANCHORED(                                    \
                        feature##_##priority,                           \
                        STRINGIFY(feature),                             \
                        description,                                    \
                        STRINGIFY(priority),                            \
                        __VA_ARGS__)

#ਜੇ HAVE_BZIP2
#  define LIBBZ2_NOTE(priority)                                         \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        bzip2,                                          \
                        "Support bzip2 compression and decompression",  \
                        priority,                                       \
                        "libbz2.so.1")
#ਨਹੀਂਤਾਂ
#  define LIBBZ2_NOTE(priority)
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
#  define LIBLZ4_NOTE(priority)                                         \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        lz4,                                            \
                        "Support lz4 compression in journal and coredump files", \
                        priority,                                       \
                        "liblz4.so.1")
#ਨਹੀਂਤਾਂ
#  define LIBLZ4_NOTE(priority)
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_XZ
#  define LIBLZMA_NOTE(priority)                                        \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        lzma,                                           \
                        "Support lzma compression in journal and coredump files", \
                        priority,                                       \
                        "liblzma.so.5")
#ਨਹੀਂਤਾਂ
#  define LIBLZMA_NOTE(priority)
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
#  define LIBZ_NOTE(priority)                                           \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        zlib,                                           \
                        "Support gzip compression and decompression",   \
                        priority,                                       \
                        "libz.so.1")
#ਨਹੀਂਤਾਂ
#  define LIBZ_NOTE(priority)
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
#  define LIBZSTD_NOTE(priority)                                        \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        zstd,                                           \
                        "Support zstd compression in journal and coredump files", \
                        priority,                                       \
                        "libzstd.so.1")
#ਨਹੀਂਤਾਂ
#  define LIBZSTD_NOTE(priority)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPRESS_NOTE(priority)                         \
        LIBBZ2_NOTE(priority);                          \
        LIBLZ4_NOTE(priority);                          \
        LIBLZMA_NOTE(priority);                         \
        LIBZ_NOTE(priority);                            \
        LIBZSTD_NOTE(priority)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPRESS_DEFAULT_NOTE                           \
        COMPRESS_JOURNAL_NOTE;                          \
        LIBBZ2_NOTE(suggested);                         \
        LIBZ_NOTE(suggested)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPRESS_JOURNAL_NOTE                           \
        LIBLZ4_NOTE(COMPRESSION_ਤਰਜੀਹ_LZ4);          \
        LIBLZMA_NOTE(COMPRESSION_ਤਰਜੀਹ_XZ);          \
        LIBZSTD_NOTE(COMPRESSION_ਤਰਜੀਹ_ZSTD)
