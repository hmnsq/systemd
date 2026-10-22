/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#если HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#include <lzma.h>
#endif

#если HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#include <lz4.h>
#include <lz4frame.h>
#include <lz4hc.h>
#endif

#если HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#include <zstd.h>
#include <zstd_errors.h>
#endif

#если HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#include <zlib.h>
#endif

#если HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#include <bzlib.h>
#endif

#include "alloc-util.h"
#include "bitfield.h"
#include "compress.h"
#include "dlfcn-util.h"
#include "io-util.h"
#include "log.h"
#include "string-table.h"
#include "unaligned.h"

#если HAVE_XZ
static DLSYM_PROTOTYPE(lzma_auto_decoder) = NULL;
static DLSYM_PROTOTYPE(lzma_code) = NULL;
static DLSYM_PROTOTYPE(lzma_easy_encoder) = NULL;
static DLSYM_PROTOTYPE(lzma_end) = NULL;
static DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = NULL;
static DLSYM_PROTOTYPE(lzma_stream_decoder) = NULL;
static DLSYM_PROTOTYPE(lzma_lzma_preset) = NULL;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
static inline void lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#если HAVE_LZ4
static DLSYM_PROTOTYPE(LZ4F_compressBegin) = NULL;
static DLSYM_PROTOTYPE(LZ4F_compressBound) = NULL;
static DLSYM_PROTOTYPE(LZ4F_compressEnd) = NULL;
static DLSYM_PROTOTYPE(LZ4F_compressUpdate) = NULL;
static DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = NULL;
static DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = NULL;
static DLSYM_PROTOTYPE(LZ4F_decompress) = NULL;
static DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = NULL;
static DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = NULL;
static DLSYM_PROTOTYPE(LZ4F_isError) = NULL;
static DLSYM_PROTOTYPE(LZ4_compress_HC) = NULL;
static DLSYM_PROTOTYPE(LZ4_compress_default) = NULL;
static DLSYM_PROTOTYPE(LZ4_decompress_safe) = NULL;
static DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = NULL;
static DLSYM_PROTOTYPE(LZ4_versionNumber) = NULL;

static const LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#если HAVE_ZSTD
static DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = NULL;
static DLSYM_PROTOTYPE(ZSTD_compress) = NULL;
static DLSYM_PROTOTYPE(ZSTD_compressStream2) = NULL;
static DLSYM_PROTOTYPE(ZSTD_createCCtx) = NULL;
static DLSYM_PROTOTYPE(ZSTD_createDCtx) = NULL;
static DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = NULL;
static DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = NULL;
static DLSYM_PROTOTYPE(ZSTD_decompressStream) = NULL;
static DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = NULL;
static DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = NULL;
static DLSYM_PROTOTYPE(ZSTD_freeCCtx) = NULL;
static DLSYM_PROTOTYPE(ZSTD_freeDCtx) = NULL;
static DLSYM_PROTOTYPE(ZSTD_getErrorCode) = NULL;
static DLSYM_PROTOTYPE(ZSTD_getErrorName) = NULL;
static DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = NULL;
static DLSYM_PROTOTYPE(ZSTD_isError) = NULL;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, NULL);

static целое zstd_ret_to_errno(size_t ret) {
        switch (sym_ZSTD_getErrorCode(ret)) {
        case ZSTD_error_dstSize_tooSmall:
                вернуть -ENOBUFS;
        case ZSTD_error_memory_allocation:
                вернуть -ENOMEM;
        default:
                вернуть -EBADMSG;
        }
}
#endif

#если HAVE_ZLIB
static DLSYM_PROTOTYPE(deflateInit2_) = NULL;
static DLSYM_PROTOTYPE(deflate) = NULL;
static DLSYM_PROTOTYPE(deflateEnd) = NULL;
static DLSYM_PROTOTYPE(inflateInit2_) = NULL;
static DLSYM_PROTOTYPE(inflate) = NULL;
static DLSYM_PROTOTYPE(inflateEnd) = NULL;
static DLSYM_PROTOTYPE(inflateReset) = NULL;

static inline void deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

static inline void inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#если HAVE_BZIP2
static DLSYM_PROTOTYPE(BZ2_bzCompressInit) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzCompress) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzDecompress) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = NULL;

static inline void BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

static inline void BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
struct Compressor {
        Compression type;
        bool encoding;
        bool finished;
        union {
#если HAVE_XZ
                lzma_stream xz;
#endif
#если HAVE_LZ4
                struct {
                        LZ4F_compressionContext_t c_lz4;
                        void *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#если HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#если HAVE_ZLIB
                z_stream gzip;
#endif
#если HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, sizeof(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

static const char* const compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

static const char* const compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

static const char* const compression_extension_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "",
        [COMPRESSION_XZ]    = ".xz",
        [COMPRESSION_LZ4]   = ".lz4",
        [COMPRESSION_ZSTD]  = ".zst",
        [COMPRESSION_GZIP]  = ".gz",
        [COMPRESSION_BZIP2] = ".bz2",
};

DEFINE_STRING_TABLE_LOOKUP(compression, Compression);
DEFINE_STRING_TABLE_LOOKUP(compression_uppercase, Compression);
DEFINE_STRING_TABLE_LOOKUP(compression_extension, Compression);

Compression compression_from_string_harder(const char *s) {
        Compression c;

        assert(s);

        c = compression_from_string(s);
        если (c >= 0)
                вернуть c;

        вернуть compression_uppercase_from_string(s);
}

Compression compression_from_filename(const char *filename) {
        Compression c;
        const char *e;

        assert(filename);

        e = strrchr(filename, '.');
        если (!e)
                вернуть COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        если (c < 0)
                вернуть COMPRESSION_NONE;

        вернуть c;
}

bool compression_supported_journal(Compression c) {
        static const unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        assert(c >= 0);
        assert(c < _COMPRESSION_MAX);

        вернуть BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        static const unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        assert(c >= 0);
        assert(c < _COMPRESSION_MAX);

        вернуть BIT_SET(supported, c);
}

Compression compression_detect_from_magic(const uint8_t data[static COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        если (memcmp(data, (const uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                вернуть COMPRESSION_GZIP;
        если (memcmp(data, (const uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                вернуть COMPRESSION_XZ;
        если (memcmp(data, (const uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                вернуть COMPRESSION_ZSTD;
        если (memcmp(data, (const uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                вернуть COMPRESSION_LZ4;
        если (memcmp(data, (const uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                вернуть COMPRESSION_BZIP2;

        вернуть _COMPRESSION_INVALID;
}

целое dlopen_xz(целое log_level) {
#если HAVE_XZ
        static void *lzma_dl = NULL;

        LIBLZMA_NOTE(suggested);

        вернуть dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#иначе
        вернуть log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

целое dlopen_lz4(целое log_level) {
#если HAVE_LZ4
        static void *lz4_dl = NULL;

        LIBLZ4_NOTE(suggested);

        вернуть dlopen_many_sym_or_warn(
                        &lz4_dl,
                        "liblz4.so.1", log_level,
                        DLSYM_ARG(LZ4F_compressBegin),
                        DLSYM_ARG(LZ4F_compressBound),
                        DLSYM_ARG(LZ4F_compressEnd),
                        DLSYM_ARG(LZ4F_compressUpdate),
                        DLSYM_ARG(LZ4F_createCompressionContext),
                        DLSYM_ARG(LZ4F_createDecompressionContext),
                        DLSYM_ARG(LZ4F_decompress),
                        DLSYM_ARG(LZ4F_freeCompressionContext),
                        DLSYM_ARG(LZ4F_freeDecompressionContext),
                        DLSYM_ARG(LZ4F_isError),
                        DLSYM_ARG(LZ4_compress_default),
                        DLSYM_ARG(LZ4_compress_HC),
                        DLSYM_ARG(LZ4_decompress_safe),
                        DLSYM_ARG(LZ4_decompress_safe_partial),
                        DLSYM_ARG(LZ4_versionNumber));
#иначе
        вернуть log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

целое dlopen_zstd(целое log_level) {
#если HAVE_ZSTD
        static void *zstd_dl = NULL;

        LIBZSTD_NOTE(suggested);

        вернуть dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_getErrorCode),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_getFrameContentSize),
                        DLSYM_ARG(ZSTD_decompressStream),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStreamOutSize),
                        DLSYM_ARG(ZSTD_CStreamInSize),
                        DLSYM_ARG(ZSTD_CStreamOutSize),
                        DLSYM_ARG(ZSTD_CCtx_setParameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStreamInSize),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#иначе
        вернуть log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

целое dlopen_zlib(целое log_level) {
#если HAVE_ZLIB
        static void *zlib_dl = NULL;

        LIBZ_NOTE(suggested);

        вернуть dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#иначе
        вернуть log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

целое dlopen_bzip2(целое log_level) {
#если HAVE_BZIP2
        static void *bzip2_dl = NULL;

        LIBBZ2_NOTE(suggested);

        вернуть dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#иначе
        вернуть log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

целое dlopen_compress_journal(Compression c, целое log_level) {
        switch (c) {
        case COMPRESSION_NONE:
                вернуть 0;
        case COMPRESSION_XZ:
                вернуть dlopen_xz(log_level);
        case COMPRESSION_LZ4:
                вернуть dlopen_lz4(log_level);
        case COMPRESSION_ZSTD:
                вернуть dlopen_zstd(log_level);
        default:
                вернуть -EOPNOTSUPP;
        }
}

целое dlopen_compress(Compression c, целое log_level) {
        switch (c) {
        case COMPRESSION_GZIP:
                вернуть dlopen_zlib(log_level);
        case COMPRESSION_BZIP2:
                вернуть dlopen_bzip2(log_level);
        default:
                вернуть dlopen_compress_journal(c, log_level);
        }
}

static целое compress_blob_xz(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                целое level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#если HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, NULL, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, NULL }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        целое r;

        r = dlopen_xz(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                если (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        вернуть -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        если (src_size < 80)
                вернуть -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ NULL,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        если (ret != LZMA_OK)
                вернуть -ENOBUFS;

        *dst_size = out_pos;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое compress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                целое level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#если HAVE_LZ4
        целое r;

        r = dlopen_lz4(LOG_DEBUG);
        если (r < 0)
                вернуть r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        если (src_size < 9)
                вернуть -ENOBUFS;

        если (src_size > INT_MAX)
                вернуть -EFBIG;
        если (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        если (level <= 0)
                r = sym_LZ4_compress_default(src, (char*)dst + 8, src_size, (целое) dst_alloc_size - 8);
        иначе
                r = sym_LZ4_compress_HC(src, (char*)dst + 8, src_size, (целое) dst_alloc_size - 8, level);
        если (r <= 0)
                вернуть -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое compress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                целое level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#если HAVE_ZSTD
        size_t k;
        целое r;

        r = dlopen_zstd(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        если (sym_ZSTD_isError(k))
                вернуть zstd_ret_to_errno(k);

        *dst_size = k;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое compress_blob_gzip(const void *src, uint64_t src_size,
                       void *dst, size_t dst_alloc_size, size_t *dst_size, целое level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#если HAVE_ZLIB
        целое r;

        r = dlopen_zlib(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size > UINT_MAX)
                вернуть -EFBIG;
        если (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (целое) sizeof(s));
        если (r != Z_OK)
                вернуть -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        если (r != Z_STREAM_END)
                вернуть -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое compress_blob_bzip2(
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, целое level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#если HAVE_BZIP2
        целое r;

        r = dlopen_bzip2(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size > UINT_MAX)
                вернуть -EFBIG;
        если (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        если (r != BZ_OK)
                вернуть -ENOMEM;

        s.next_in = (char*) src;
        s.avail_in = src_size;
        s.next_out = (char*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        если (r != BZ_STREAM_END)
                вернуть -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

целое compress_blob_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, целое level) {

        switch (compression) {
        case COMPRESSION_XZ:
                вернуть compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_LZ4:
                вернуть compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_ZSTD:
                вернуть compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                вернуть -EOPNOTSUPP;
        }
}

целое compress_blob(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, целое level) {

        switch (compression) {
        case COMPRESSION_GZIP:
                вернуть compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_BZIP2:
                вернуть compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                вернуть compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

static целое decompress_blob_xz(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#если HAVE_XZ
        целое r;

        r = dlopen_xz(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        если (ret != LZMA_OK)
                вернуть -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        если (!greedy_realloc(dst, space, 1))
                вернуть -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        для (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                если (ret == LZMA_STREAM_END)
                        break;
                если (ret != LZMA_OK)
                        вернуть -ENOMEM;

                если (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                если (dst_max > 0 && space == dst_max)
                        вернуть -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                assert(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                если (!greedy_realloc(dst, space, 1))
                        вернуть -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое decompress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#если HAVE_LZ4
        char* out;
        целое r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size <= 8)
                вернуть -EBADMSG;

        если (src_size - 8 > INT_MAX)
                вернуть -EFBIG;

        size = unaligned_read_le64(src);
        если (size < 0 || (unsigned) size != unaligned_read_le64(src))
                вернуть -EFBIG;
        если (dst_max > 0 && (size_t) size > dst_max)
                вернуть -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        если (!out)
                вернуть -ENOMEM;

        r = sym_LZ4_decompress_safe((char*)src + 8, out, src_size - 8, size);
        если (r < 0 || r != size)
                вернуть -EBADMSG;

        *dst_size = size;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(void) {
#если HAVE_ZSTD
        если (dlopen_zstd(LOG_DEBUG) < 0)
                вернуть 0;

        вернуть sym_ZSTD_DStreamOutSize();
#иначе
        вернуть 0;
#endif
}

static целое decompress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#если HAVE_ZSTD
        целое r;

        r = dlopen_zstd(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        если (size == ZSTD_CONTENTSIZE_ERROR)
                вернуть -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        если (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        иначе {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        если (!greedy_realloc(dst, space, 1))
                вернуть -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        если (!dctx)
                вернуть -ENOMEM;

        ZSTD_inBuffer input = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .size = MIN(space, cap),
        };

        для (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &input);
                если (sym_ZSTD_isError(k))
                        вернуть log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                если (output.pos >= cap)
                        break;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                если (k == 0 && input.pos >= input.size)
                        break;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                если (output.pos == output.size) {
                        если (space > SIZE_MAX / 2)
                                вернуть -E2BIG;
                        space = MIN(2 * space, cap);
                        если (!greedy_realloc(dst, space, 1))
                                вернуть -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        continue;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                если (input.pos >= input.size)
                        вернуть log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое decompress_blob_gzip(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#если HAVE_ZLIB
        целое r;

        r = dlopen_zlib(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size > UINT_MAX)
                вернуть -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (целое) sizeof(s));
        если (r != Z_OK)
                вернуть -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        если (!greedy_realloc(dst, space, 1))
                вернуть -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        для (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                если (r == Z_STREAM_END)
                        break;
                если (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        вернуть -EBADMSG;

                если (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                если (dst_max > 0 && space == dst_max)
                        вернуть -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                если (!greedy_realloc(dst, space, 1))
                        вернуть -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое decompress_blob_bzip2(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#если HAVE_BZIP2
        целое r;

        r = dlopen_bzip2(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size > UINT_MAX)
                вернуть -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        если (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                вернуть -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        если (!greedy_realloc(dst, space, 1))
                вернуть -ENOMEM;

        s.next_in = (char*) src;
        s.avail_in = src_size;
        s.next_out = (char*) *dst;
        s.avail_out = space;

        для (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                если (r == BZ_STREAM_END)
                        break;
                если (r != BZ_OK)
                        вернуть -EBADMSG;

                если (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                если (dst_max > 0 && space == dst_max)
                        вернуть -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                если (!greedy_realloc(dst, space, 1))
                        вернуть -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (char*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

целое decompress_blob_journal(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        case COMPRESSION_XZ:
                вернуть decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_LZ4:
                вернуть decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_ZSTD:
                вернуть decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                вернуть -EPROTONOSUPPORT;
        }
}

целое decompress_blob(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        case COMPRESSION_GZIP:
                вернуть decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_BZIP2:
                вернуть decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                вернуть decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

целое decompress_zlib_raw(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_size,
                целое wbits) {

#если HAVE_ZLIB
        целое r;

        r = dlopen_zlib(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size > UINT_MAX)
                вернуть -EFBIG;
        если (dst_size > UINT_MAX)
                вернуть -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (void*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (целое) sizeof(s));
        если (r != Z_OK)
                вернуть -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        если (r != Z_STREAM_END || produced != dst_size)
                вернуть -EBADMSG;

        вернуть 0;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое decompress_startswith_xz(
                const void *src,
                uint64_t src_size,
                void **buffer,
                const void *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        assert(src);
        assert(src_size > 0);
        assert(buffer);
        assert(prefix);

#если HAVE_XZ
        целое r;

        r = dlopen_xz(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        если (ret != LZMA_OK)
                вернуть -EBADMSG;

        если (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                вернуть -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        для (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                если (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        вернуть -EBADMSG;

                если (allocated - s.avail_out >= prefix_len + 1)
                        вернуть memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                если (ret == LZMA_STREAM_END)
                        вернуть 0;

                s.avail_out += allocated;

                если (!(greedy_realloc(buffer, allocated * 2, 1)))
                        вернуть -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое decompress_startswith_lz4(
                const void *src,
                uint64_t src_size,
                void **buffer,
                const void *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        assert(src);
        assert(src_size > 0);
        assert(buffer);
        assert(prefix);

#если HAVE_LZ4
        size_t allocated;
        целое r;

        r = dlopen_lz4(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size <= 8)
                вернуть -EBADMSG;

        если (src_size - 8 > INT_MAX)
                вернуть -EFBIG;

        если (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                вернуть -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (char*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        если (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                если (sym_LZ4_versionNumber() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        вернуть 0;

                если (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        если (memcmp(*buffer, prefix, r) != 0)
                                вернуть 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                если (r < 0)
                        вернуть r;

                если (size < prefix_len + 1)
                        вернуть 0;
        }

        вернуть memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое decompress_startswith_zstd(
                const void *src,
                uint64_t src_size,
                void **buffer,
                const void *prefix,
                size_t prefix_len,
                uint8_t extra) {

        assert(src);
        assert(src_size > 0);
        assert(buffer);
        assert(prefix);

#если HAVE_ZSTD
        целое r;

        r = dlopen_zstd(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        если (size == ZSTD_CONTENTSIZE_ERROR)
                вернуть -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        если (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                вернуть 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        если (!dctx)
                вернуть -ENOMEM;

        если (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                вернуть -ENOMEM;

        ZSTD_inBuffer input = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &input);
        если (sym_ZSTD_isError(k))
                вернуть log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        если (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                если (k != 0)
                        вернуть log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                вернуть 0; /* Decompressed text too short to match the prefix and extra */
        }

        вернуть memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое decompress_startswith_gzip(
                const void *src,
                uint64_t src_size,
                void **buffer,
                const void *prefix,
                size_t prefix_len,
                uint8_t extra) {

        assert(src);
        assert(src_size > 0);
        assert(buffer);
        assert(prefix);

#если HAVE_ZLIB
        целое r;

        r = dlopen_zlib(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size > UINT_MAX)
                вернуть -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (целое) sizeof(s));
        если (r != Z_OK)
                вернуть -EBADMSG;

        если (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                вернуть -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (void*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        для (;;) {
                r = sym_inflate(&s, Z_FINISH);

                если (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        вернуть -EBADMSG;

                если (allocated - s.avail_out >= prefix_len + 1)
                        вернуть memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                если (r == Z_STREAM_END)
                        вернуть 0;

                если (s.avail_out > 0)
                        вернуть -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                если (!(greedy_realloc(buffer, allocated * 2, 1)))
                        вернуть -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

static целое decompress_startswith_bzip2(
                const void *src,
                uint64_t src_size,
                void **buffer,
                const void *prefix,
                size_t prefix_len,
                uint8_t extra) {

        assert(src);
        assert(src_size > 0);
        assert(buffer);
        assert(prefix);

#если HAVE_BZIP2
        целое r;

        r = dlopen_bzip2(LOG_DEBUG);
        если (r < 0)
                вернуть r;

        если (src_size > UINT_MAX)
                вернуть -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        если (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                вернуть -EIO;

        если (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                вернуть -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (char*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        для (;;) {
                r = sym_BZ2_bzDecompress(&s);

                если (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        вернуть -EBADMSG;

                если (allocated - s.avail_out >= prefix_len + 1)
                        вернуть memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                если (r == BZ_STREAM_END)
                        вернуть 0;

                если (s.avail_out > 0)
                        вернуть -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                если (!(greedy_realloc(buffer, allocated * 2, 1)))
                        вернуть -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (char*) *buffer + used;
        }
#иначе
        вернуть -EPROTONOSUPPORT;
#endif
}

целое decompress_startswith_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        case COMPRESSION_XZ:
                вернуть decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_LZ4:
                вернуть decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_ZSTD:
                вернуть decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                вернуть -EOPNOTSUPP;
        }
}

целое decompress_startswith(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        case COMPRESSION_GZIP:
                вернуть decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_BZIP2:
                вернуть decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                вернуть decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

целое compress_stream(
                Compression type,
                целое fdf, целое fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = NULL;
        _cleanup_free_ void *buf = NULL;
        _cleanup_free_ uint8_t *input = NULL;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        целое r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        r = compressor_new(&c, type);
        если (r < 0)
                вернуть r;

        input = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        если (!input)
                вернуть -ENOMEM;

        для (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                если (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = read(fdf, input, m);
                если (n < 0)
                        вернуть -errno;

                если (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        если (r < 0)
                                вернуть r;

                        если (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                если (r < 0)
                                        вернуть r;
                                total_out += buf_size;
                        }
                        break;
                }

                total_in += n;
                если (max_bytes != UINT64_MAX) {
                        assert(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, input, n, &buf, &buf_size, &buf_alloc);
                если (r < 0)
                        вернуть r;

                если (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        если (r < 0)
                                вернуть r;
                        total_out += buf_size;
                }
        }

        если (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        если (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        иначе
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (double) total_out / total_in * 100);

        вернуть 0;
}

static целое should_sparse(целое fd) {
        struct stat st;

        assert(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        если (fstat(fd, &st) < 0)
                вернуть -errno;

        если (!S_ISREG(st.st_mode))
                вернуть false;

        целое flags = fcntl(fd, F_GETFL);
        если (flags < 0)
                вернуть -errno;

        если (FLAGS_SET(flags, O_APPEND))
                вернуть false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        если (pos < 0)
                вернуть -errno;

        вернуть pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
static целое finalize_sparse(целое fd) {
        off_t pos;

        assert(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        если (pos < 0)
                вернуть -errno;

        если (ftruncate(fd, pos) < 0)
                вернуть -errno;

        вернуть 0;
}

/* Common helper for decompress_stream_*() wrappers */

struct decompress_stream_userdata {
        целое fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

static целое decompress_stream_write_callback(const void *data, size_t size, void *userdata) {
        struct decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        если (u->max_bytes != UINT64_MAX) {
                если (u->max_bytes < size)
                        вернуть -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        если (u->skip > 0) {
                если (u->skip >= size) {
                        u->skip -= size;
                        вернуть 0;
                }

                data = (const uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        если (size == 0)
                вернуть 0;

        если (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                если (k < 0)
                        вернуть (целое) k;
                вернуть 0;
        }

        вернуть loop_write(u->fd, data, size);
}

static целое decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        assert(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        если (!c)
                вернуть -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        switch (type) {
        case COMPRESSION_NONE:
                break;

#если HAVE_XZ
        case COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                если (xz_ret != LZMA_OK)
                        вернуть -EIO;
                break;
        }
#endif

#если HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                если (sym_LZ4F_isError(rc))
                        вернуть -ENOMEM;

                break;
        }
#endif

#если HAVE_ZSTD
        case COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                если (!c->d_zstd)
                        вернуть -ENOMEM;
                break;
#endif

#если HAVE_ZLIB
        case COMPRESSION_GZIP:
                если (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (целое) sizeof(c->gzip)) != Z_OK)
                        вернуть -EIO;
                break;
#endif

#если HAVE_BZIP2
        case COMPRESSION_BZIP2:
                если (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        вернуть -EIO;
                break;
#endif

        default:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        вернуть 0;
}

static целое decompress_blob_to_fd_impl(
                Compression type,
                const void *src,
                uint64_t src_size,
                целое fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        целое r;

        assert(src);
        assert(fdt >= 0);

        если (type == COMPRESSION_NONE)
                вернуть -EOPNOTSUPP;

        если (src_size == 0)
                вернуть -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        если (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                если (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        если (src_size <= sizeof(uint64_t))
                                вернуть -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        если (sz < skip)
                                вернуть -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                если (max_bytes == 0)
                        вернуть -ENOBUFS;

                _cleanup_free_ void *buf = NULL;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                если (r < 0)
                        вернуть r;

                /* Check the result size again for safety. */
                если (size < skip)
                        вернуть -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                если (r < 0)
                        вернуть r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (double) size / src_size * 100);

                если (ret_size)
                        *ret_size = size - skip;
                вернуть 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        если (r < 0)
                вернуть r;

        struct decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_push(c, src, src_size, decompress_stream_write_callback, &userdata);
        если (r < 0)
                вернуть r;

        /* Finalize */
        r = decompressor_push(c, /* data= */ NULL, /* size= */ 0, decompress_stream_write_callback, &userdata);
        если (r < 0)
                вернуть r;

        если (userdata.total_out < skip)
                вернуть -EBADMSG;

        если (userdata.sparse) {
                r = finalize_sparse(fdt);
                если (r < 0)
                        вернуть r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (double) userdata.total_out / src_size * 100);

        если (ret_size)
                *ret_size = userdata.total_out - skip;

        вернуть 0;
}

целое decompress_blob_to_fd_journal(
                Compression type,
                const void *src,
                uint64_t src_size,
                целое fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        целое r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        если (r < 0)
                вернуть r;

        вернуть decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

целое decompress_blob_to_fd(
                Compression type,
                const void *src,
                uint64_t src_size,
                целое fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        целое r;

        r = dlopen_compress(type, LOG_DEBUG);
        если (r < 0)
                вернуть r;

        вернуть decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

целое decompress_stream_full(
                Compression type,
                целое fdf, целое fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        _cleanup_free_ uint8_t *buf = NULL;
        uint64_t total_in = 0;
        целое r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        если (type == COMPRESSION_NONE)
                вернуть -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        если (r < 0)
                вернуть r;

        r = decompressor_new(&c, type, flags);
        если (r < 0)
                вернуть r;

        struct decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        если (!buf)
                вернуть -ENOMEM;

        для (;;) {
                ssize_t n;

                n = read(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                если (n < 0)
                        вернуть -errno;

                total_in += n;

                r = decompressor_push(c, buf, n, decompress_stream_write_callback, &userdata);
                если (r < 0)
                        вернуть r;

                если (n == 0)
                        break;
        }

        если (total_in == 0)
                вернуть log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        если (userdata.sparse) {
                r = finalize_sparse(fdt);
                если (r < 0)
                        вернуть r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (double) userdata.total_out / total_in * 100);

        вернуть 0;
}

целое decompress_stream_by_filename(const char *filename, целое fdf, целое fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        если (c == COMPRESSION_NONE)
                вернуть -EPROTONOSUPPORT;

        вернуть decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        если (!c)
                вернуть NULL;

        switch (c->type) {

#если HAVE_XZ
        case COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                break;
#endif

#если HAVE_LZ4
        case COMPRESSION_LZ4:
                если (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = NULL;
                        c->lz4_header = mfree(c->lz4_header);
                } иначе {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = NULL;
                }
                break;
#endif

#если HAVE_ZSTD
        case COMPRESSION_ZSTD:
                если (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = NULL;
                } иначе {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = NULL;
                }
                break;
#endif

#если HAVE_ZLIB
        case COMPRESSION_GZIP:
                если (c->encoding)
                        sym_deflateEnd(&c->gzip);
                иначе
                        sym_inflateEnd(&c->gzip);
                break;
#endif

#если HAVE_BZIP2
        case COMPRESSION_BZIP2:
                если (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                иначе
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                break;
#endif

        default:
                break;
        }

        вернуть mfree(c);
}

Compression compressor_type(const Compressor *c) {
        вернуть c ? c->type : _COMPRESSION_INVALID;
}

целое decompressor_detect(Decompressor **decompressor, const void *data, size_t size) {
        целое r;

        assert(decompressor);

        если (*decompressor)
                вернуть 1;

        если (size < COMPRESSION_MAGIC_BYTES_MAX)
                вернуть 0;

        assert(data);

        Compression type = compression_detect_from_magic(data);
        если (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        если (r < 0)
                вернуть r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        если (r < 0)
                вернуть r;

        вернуть 1;
}

целое decompressor_force(Decompressor **decompressor, Compression type) {
        целое r;

        assert(decompressor);
        assert(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        если (r < 0)
                вернуть r;

        *decompressor = compressor_free(*decompressor);
        вернуть decompressor_new(decompressor, type, /* flags= */ 0);
}

целое decompressor_force_off(Decompressor **decompressor) {
        вернуть decompressor_force(decompressor, COMPRESSION_NONE);
}

целое decompressor_push(Decompressor *c, const void *data, size_t size, DecompressorCallback callback, void *userdata) {
        целое r;

        assert(c);
        assert(data || size == 0);
        assert(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        если (c->encoding)
                вернуть -EINVAL;

        если (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                вернуть 0;

#если HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = NULL;
        если (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                если (!buffer)
                        вернуть -ENOMEM;
        }
#endif

        switch (c->type) {

        case COMPRESSION_NONE:
                если (c->finished)
                        вернуть -EBADMSG;

                если (size == 0) {
                        c->finished = true;
                        вернуть 0;
                }

                r = callback(data, size, userdata);
                если (r < 0)
                        вернуть r;

                вернуть 0;

#если HAVE_XZ
        case COMPRESSION_XZ:
                если (c->finished)
                        вернуть -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                для (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        если (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                вернуть -EBADMSG;

                        если (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                вернуть -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        если (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                если (r < 0)
                                        вернуть r;
                        }

                        если (lzr == LZMA_STREAM_END) {
                                если (c->xz.avail_in > 0)
                                        вернуть -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                вернуть 0;
                        }

                        если (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                если (size == 0)
                                        вернуть -EBADMSG; /* truncated */

                                вернуть 0; /* No progress possible with the current input. */
                        }
                }
#endif

#если HAVE_LZ4
        case COMPRESSION_LZ4: {
                const uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                для (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, NULL);
                        если (sym_LZ4F_isError(rc))
                                вернуть -EBADMSG;

                        если (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                вернуть -EIO;

                        если (consumed > 0) {
                                assert(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        если (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                если (r < 0)
                                        вернуть r;
                        }

                        если (src_remaining == 0) {
                                если (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        вернуть 0;
                                }

                                если (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                если (size == 0)
                                        вернуть -EBADMSG; /* truncated */

                                вернуть 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#если HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src =  (void*) data,
                        .size = size,
                };

                c->finished = false;

                для (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &input);
                        если (sym_ZSTD_isError(res))
                                вернуть -EBADMSG;

                        если (input.pos > input.size ||
                            output.pos > output.size)
                                вернуть -EIO;

                        если (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                если (r < 0)
                                        вернуть r;
                        }

                        если (input.pos == input.size) {
                                если (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        вернуть 0;
                                }

                                если (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                если (size == 0)
                                        вернуть -EBADMSG; /* truncated */

                                вернуть 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#если HAVE_ZLIB
        case COMPRESSION_GZIP:
                если (size > UINT_MAX)
                        вернуть -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                для (;;) {
                        если (c->finished) {
                                если (sym_inflateReset(&c->gzip) != Z_OK)
                                        вернуть -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        целое zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        если (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                вернуть -EBADMSG;

                        если (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                вернуть -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        если (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                если (r < 0)
                                        вернуть r;
                        }

                        если (zr == Z_STREAM_END) {
                                c->finished = true;

                                если (c->gzip.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                вернуть 0;
                        }

                        если (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                если (size == 0)
                                        вернуть -EBADMSG; /* truncated */

                                вернуть 0; /* No progress possible with the current input. */
                        }
                }
#endif

#если HAVE_BZIP2
        case COMPRESSION_BZIP2:
                если (size > UINT_MAX)
                        вернуть -EFBIG;

                c->bzip2.next_in = (char*) data;
                c->bzip2.avail_in = size;

                для (;;) {
                        если (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                если (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        вернуть -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (char*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        целое bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        если (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                вернуть -EBADMSG;

                        если (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                вернуть -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        если (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                если (r < 0)
                                        вернуть r;
                        }

                        если (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                если (c->bzip2.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                вернуть 0;
                        }

                        если (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                если (size == 0)
                                        вернуть -EBADMSG; /* truncated */

                                вернуть 0; /* No progress possible with the current input. */
                        }
                }
#endif

        default:
                assert_not_reached();
        }
}

целое compressor_new(Compressor **ret, Compression type) {
        целое r;

        assert(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        если (r < 0)
                вернуть r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        если (!c)
                вернуть -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        switch (type) {

#если HAVE_XZ
        case COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                если (xzr != LZMA_OK)
                        вернуть -EIO;

                c->type = COMPRESSION_XZ;
                break;
        }
#endif

#если HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                если (sym_LZ4F_isError(rc))
                        вернуть -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                если (!c->lz4_header)
                        вернуть -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                если (sym_LZ4F_isError(c->lz4_header_size))
                        вернуть -EINVAL;

                break;
        }
#endif

#если HAVE_ZSTD
        case COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                если (!c->c_zstd)
                        вернуть -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                если (sym_ZSTD_isError(z))
                        вернуть -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                если (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                break;
#endif

#если HAVE_ZLIB
        case COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (целое) sizeof(c->gzip));
                если (r != Z_OK)
                        вернуть -EIO;

                c->type = COMPRESSION_GZIP;
                break;
#endif

#если HAVE_BZIP2
        case COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                если (r != BZ_OK)
                        вернуть -EIO;

                c->type = COMPRESSION_BZIP2;
                break;
#endif

        case COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                break;

        default:
                вернуть -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        вернуть 0;
}

#если HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
static целое enlarge_buffer(void **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        если (*buffer_allocated >= need)
                вернуть 0;

        если (!greedy_realloc(buffer, need, 1))
                вернуть -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        вернуть 1;
}
#endif

целое compressor_start(
                Compressor *c,
                const void *data,
                size_t size,
                void **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#если HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        целое r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        если (!c->encoding)
                вернуть -EINVAL;

        если (size == 0)
                вернуть 0;

        assert(data);

        *buffer_size = 0;

        switch (c->type) {

#если HAVE_XZ
        case COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                пока (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        если (r < 0)
                                вернуть r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        если (lzr != LZMA_OK)
                                вернуть -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                break;
#endif

#если HAVE_LZ4
        case COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                если (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        если (r < 0)
                                вернуть r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                если (r < 0)
                        вернуть r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, NULL);
                если (sym_LZ4F_isError(n))
                        вернуть -EIO;

                *buffer_size += n;
                break;
        }
#endif

#если HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src = data,
                        .size = size,
                };

                пока (input.pos < input.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        если (r < 0)
                                вернуть r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_continue);
                        если (sym_ZSTD_isError(res))
                                вернуть -EIO;

                        *buffer_size += output.pos;
                }

                break;
        }
#endif

#если HAVE_ZLIB
        case COMPRESSION_GZIP:
                если (size > UINT_MAX)
                        вернуть -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                пока (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        если (r < 0)
                                вернуть r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        если (r != Z_OK)
                                вернуть -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                break;
#endif

#если HAVE_BZIP2
        case COMPRESSION_BZIP2:
                если (size > UINT_MAX)
                        вернуть -EFBIG;

                c->bzip2.next_in = (void*) data;
                c->bzip2.avail_in = size;

                пока (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        если (r < 0)
                                вернуть r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        если (r != BZ_RUN_OK)
                                вернуть -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                break;
#endif

        case COMPRESSION_NONE:

                если (*buffer_allocated < size) {
                        void *p;

                        p = realloc(*buffer, size);
                        если (!p)
                                вернуть -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                break;

        default:
                вернуть -EOPNOTSUPP;
        }

        вернуть 0;
}

целое compressor_finish(Compressor *c, void **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#если HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        целое r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        если (!c->encoding)
                вернуть -EINVAL;

        *buffer_size = 0;

        switch (c->type) {

#если HAVE_XZ
        case COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        если (r < 0)
                                вернуть r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        если (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                вернуть -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } пока (lzr != LZMA_STREAM_END);

                break;
        }
#endif

#если HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                если (r < 0)
                        вернуть r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, NULL);
                если (sym_LZ4F_isError(n))
                        вернуть -EIO;

                *buffer_size = n;
                break;
        }
#endif

#если HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {};
                size_t res;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        если (r < 0)
                                вернуть r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_end);
                        если (sym_ZSTD_isError(res))
                                вернуть -EIO;

                        *buffer_size += output.pos;
                } пока (res != 0);

                break;
        }
#endif

#если HAVE_ZLIB
        case COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        если (r < 0)
                                вернуть r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        если (!IN_SET(r, Z_OK, Z_STREAM_END))
                                вернуть -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } пока (r != Z_STREAM_END);

                break;
#endif

#если HAVE_BZIP2
        case COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        если (r < 0)
                                вернуть r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        если (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                вернуть -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } пока (r != BZ_STREAM_END);

                break;
#endif

        case COMPRESSION_NONE:
                break;

        default:
                вернуть -EOPNOTSUPP;
        }

        вернуть 0;
}
