/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "sd-dlopen.h"

#समावेश "forward.h"

/* Prevent dlopen helper functions (e.g., dlopen_libfoo()) from being inlined or cloned by the compiler/LTO.
 * This ensures that their specific symbols remain intact in the final executable, allowing the developer
 * test utility to verify that the functions corresponding to the dlopen ELF notes are actually invoked.
 * This is restricted to BUILD_MODE_DEVELOPER to avoid degrading production performance. */
#अगर BUILD_MODE_DEVELOPER
#  define _dlopen_loader_ _noclone_ _noinline_
#अन्यथा
#  define _dlopen_loader_
#पूर्ण_अगर

/* Avoid invalid priority. */
#घोषणा _DLOPEN_CHECK_PRIORITY_required    1
#घोषणा _DLOPEN_CHECK_PRIORITY_recommended 1
#घोषणा _DLOPEN_CHECK_PRIORITY_suggested   1

/* Unlike SD_ELF_NOTE_DLOPEN_ANCHORED(), this takes feature and priority without quotation. */
#घोषणा ELF_NOTE_DLOPEN_ANCHORED(feature, description, priority, ...)   \
        assert_cc(_DLOPEN_CHECK_PRIORITY_##priority);                   \
        SD_ELF_NOTE_DLOPEN_ANCHORED(                                    \
                        feature##_##priority,                           \
                        STRINGIFY(feature),                             \
                        description,                                    \
                        STRINGIFY(priority),                            \
                        __VA_ARGS__)

#अगर HAVE_BZIP2
#  define LIBBZ2_NOTE(priority)                                         \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        bzip2,                                          \
                        "Support bzip2 compression and decompression",  \
                        priority,                                       \
                        "libbz2.so.1")
#अन्यथा
#  define LIBBZ2_NOTE(priority)
#पूर्ण_अगर

#अगर HAVE_LZ4
#  define LIBLZ4_NOTE(priority)                                         \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        lz4,                                            \
                        "Support lz4 compression in journal and coredump files", \
                        priority,                                       \
                        "liblz4.so.1")
#अन्यथा
#  define LIBLZ4_NOTE(priority)
#पूर्ण_अगर

#अगर HAVE_XZ
#  define LIBLZMA_NOTE(priority)                                        \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        lzma,                                           \
                        "Support lzma compression in journal and coredump files", \
                        priority,                                       \
                        "liblzma.so.5")
#अन्यथा
#  define LIBLZMA_NOTE(priority)
#पूर्ण_अगर

#अगर HAVE_ZLIB
#  define LIBZ_NOTE(priority)                                           \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        zlib,                                           \
                        "Support gzip compression and decompression",   \
                        priority,                                       \
                        "libz.so.1")
#अन्यथा
#  define LIBZ_NOTE(priority)
#पूर्ण_अगर

#अगर HAVE_ZSTD
#  define LIBZSTD_NOTE(priority)                                        \
        ELF_NOTE_DLOPEN_ANCHORED(                                       \
                        zstd,                                           \
                        "Support zstd compression in journal and coredump files", \
                        priority,                                       \
                        "libzstd.so.1")
#अन्यथा
#  define LIBZSTD_NOTE(priority)
#पूर्ण_अगर

#घोषणा COMPRESS_NOTE(priority)                         \
        LIBBZ2_NOTE(priority);                          \
        LIBLZ4_NOTE(priority);                          \
        LIBLZMA_NOTE(priority);                         \
        LIBZ_NOTE(priority);                            \
        LIBZSTD_NOTE(priority)

#घोषणा COMPRESS_DEFAULT_NOTE                           \
        COMPRESS_JOURNAL_NOTE;                          \
        LIBBZ2_NOTE(suggested);                         \
        LIBZ_NOTE(suggested)

#घोषणा COMPRESS_JOURNAL_NOTE                           \
        LIBLZ4_NOTE(COMPRESSION_PRIORITY_LZ4);          \
        LIBLZMA_NOTE(COMPRESSION_PRIORITY_XZ);          \
        LIBZSTD_NOTE(COMPRESSION_PRIORITY_ZSTD)
