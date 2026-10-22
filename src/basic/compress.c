/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#якщо HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#include <lzma.h>
#endif

#якщо HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#include <lz4.h>
#include <lz4frame.h>
#include <lz4hc.h>
#endif

#якщо HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#include <zstd.h>
#include <zstd_errors.h>
#endif

#якщо HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#include <zlib.h>
#endif

#якщо HAVE_BZIP2
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

#якщо HAVE_XZ
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

#якщо HAVE_LZ4
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

#якщо HAVE_ZSTD
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

static ціле zstd_ret_to_errno(size_t ret) {
        switch (sym_ZSTD_getErrorCode(ret)) {
        case ZSTD_error_dstSize_tooSmall:
                повернути -ENOBUFS;
        case ZSTD_error_memory_allocation:
                повернути -ENOMEM;
        default:
                повернути -EBADMSG;
        }
}
#endif

#якщо HAVE_ZLIB
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

#якщо HAVE_BZIP2
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
#якщо HAVE_XZ
                lzma_stream xz;
#endif
#якщо HAVE_LZ4
                struct {
                        LZ4F_compressionContext_t c_lz4;
                        void *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#якщо HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#якщо HAVE_ZLIB
                z_stream gzip;
#endif
#якщо HAVE_BZIP2
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
        якщо (c >= 0)
                повернути c;

        повернути compression_uppercase_from_string(s);
}

Compression compression_from_filename(const char *filename) {
        Compression c;
        const char *e;

        assert(filename);

        e = strrchr(filename, '.');
        якщо (!e)
                повернути COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        якщо (c < 0)
                повернути COMPRESSION_NONE;

        повернути c;
}

bool compression_supported_journal(Compression c) {
        static const unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        assert(c >= 0);
        assert(c < _COMPRESSION_MAX);

        повернути BIT_SET(supported, c);
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

        повернути BIT_SET(supported, c);
}

Compression compression_detect_from_magic(const uint8_t data[static COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        якщо (memcmp(data, (const uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                повернути COMPRESSION_GZIP;
        якщо (memcmp(data, (const uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                повернути COMPRESSION_XZ;
        якщо (memcmp(data, (const uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                повернути COMPRESSION_ZSTD;
        якщо (memcmp(data, (const uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                повернути COMPRESSION_LZ4;
        якщо (memcmp(data, (const uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                повернути COMPRESSION_BZIP2;

        повернути _COMPRESSION_INVALID;
}

ціле dlopen_xz(ціле log_level) {
#якщо HAVE_XZ
        static void *lzma_dl = NULL;

        LIBLZMA_NOTE(suggested);

        повернути dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#інакше
        повернути log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

ціле dlopen_lz4(ціле log_level) {
#якщо HAVE_LZ4
        static void *lz4_dl = NULL;

        LIBLZ4_NOTE(suggested);

        повернути dlopen_many_sym_or_warn(
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
#інакше
        повернути log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

ціле dlopen_zstd(ціле log_level) {
#якщо HAVE_ZSTD
        static void *zstd_dl = NULL;

        LIBZSTD_NOTE(suggested);

        повернути dlopen_many_sym_or_warn(
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
#інакше
        повернути log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

ціле dlopen_zlib(ціле log_level) {
#якщо HAVE_ZLIB
        static void *zlib_dl = NULL;

        LIBZ_NOTE(suggested);

        повернути dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#інакше
        повернути log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

ціле dlopen_bzip2(ціле log_level) {
#якщо HAVE_BZIP2
        static void *bzip2_dl = NULL;

        LIBBZ2_NOTE(suggested);

        повернути dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#інакше
        повернути log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

ціле dlopen_compress_journal(Compression c, ціле log_level) {
        switch (c) {
        case COMPRESSION_NONE:
                повернути 0;
        case COMPRESSION_XZ:
                повернути dlopen_xz(log_level);
        case COMPRESSION_LZ4:
                повернути dlopen_lz4(log_level);
        case COMPRESSION_ZSTD:
                повернути dlopen_zstd(log_level);
        default:
                повернути -EOPNOTSUPP;
        }
}

ціле dlopen_compress(Compression c, ціле log_level) {
        switch (c) {
        case COMPRESSION_GZIP:
                повернути dlopen_zlib(log_level);
        case COMPRESSION_BZIP2:
                повернути dlopen_bzip2(log_level);
        default:
                повернути dlopen_compress_journal(c, log_level);
        }
}

static ціле compress_blob_xz(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ціле level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#якщо HAVE_XZ
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
        ціле r;

        r = dlopen_xz(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                якщо (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        повернути -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        якщо (src_size < 80)
                повернути -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ NULL,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        якщо (ret != LZMA_OK)
                повернути -ENOBUFS;

        *dst_size = out_pos;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле compress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ціле level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#якщо HAVE_LZ4
        ціле r;

        r = dlopen_lz4(LOG_DEBUG);
        якщо (r < 0)
                повернути r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        якщо (src_size < 9)
                повернути -ENOBUFS;

        якщо (src_size > INT_MAX)
                повернути -EFBIG;
        якщо (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        якщо (level <= 0)
                r = sym_LZ4_compress_default(src, (char*)dst + 8, src_size, (ціле) dst_alloc_size - 8);
        інакше
                r = sym_LZ4_compress_HC(src, (char*)dst + 8, src_size, (ціле) dst_alloc_size - 8, level);
        якщо (r <= 0)
                повернути -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле compress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ціле level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#якщо HAVE_ZSTD
        size_t k;
        ціле r;

        r = dlopen_zstd(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        якщо (sym_ZSTD_isError(k))
                повернути zstd_ret_to_errno(k);

        *dst_size = k;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле compress_blob_gzip(const void *src, uint64_t src_size,
                       void *dst, size_t dst_alloc_size, size_t *dst_size, ціле level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#якщо HAVE_ZLIB
        ціле r;

        r = dlopen_zlib(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size > UINT_MAX)
                повернути -EFBIG;
        якщо (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (ціле) sizeof(s));
        якщо (r != Z_OK)
                повернути -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        якщо (r != Z_STREAM_END)
                повернути -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле compress_blob_bzip2(
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, ціле level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#якщо HAVE_BZIP2
        ціле r;

        r = dlopen_bzip2(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size > UINT_MAX)
                повернути -EFBIG;
        якщо (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        якщо (r != BZ_OK)
                повернути -ENOMEM;

        s.next_in = (char*) src;
        s.avail_in = src_size;
        s.next_out = (char*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        якщо (r != BZ_STREAM_END)
                повернути -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

ціле compress_blob_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, ціле level) {

        switch (compression) {
        case COMPRESSION_XZ:
                повернути compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_LZ4:
                повернути compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_ZSTD:
                повернути compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                повернути -EOPNOTSUPP;
        }
}

ціле compress_blob(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, ціле level) {

        switch (compression) {
        case COMPRESSION_GZIP:
                повернути compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_BZIP2:
                повернути compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                повернути compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

static ціле decompress_blob_xz(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#якщо HAVE_XZ
        ціле r;

        r = dlopen_xz(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        якщо (ret != LZMA_OK)
                повернути -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        якщо (!greedy_realloc(dst, space, 1))
                повернути -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        для (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                якщо (ret == LZMA_STREAM_END)
                        break;
                якщо (ret != LZMA_OK)
                        повернути -ENOMEM;

                якщо (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                якщо (dst_max > 0 && space == dst_max)
                        повернути -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                assert(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                якщо (!greedy_realloc(dst, space, 1))
                        повернути -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле decompress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#якщо HAVE_LZ4
        char* out;
        ціле r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size <= 8)
                повернути -EBADMSG;

        якщо (src_size - 8 > INT_MAX)
                повернути -EFBIG;

        size = unaligned_read_le64(src);
        якщо (size < 0 || (unsigned) size != unaligned_read_le64(src))
                повернути -EFBIG;
        якщо (dst_max > 0 && (size_t) size > dst_max)
                повернути -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        якщо (!out)
                повернути -ENOMEM;

        r = sym_LZ4_decompress_safe((char*)src + 8, out, src_size - 8, size);
        якщо (r < 0 || r != size)
                повернути -EBADMSG;

        *dst_size = size;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(void) {
#якщо HAVE_ZSTD
        якщо (dlopen_zstd(LOG_DEBUG) < 0)
                повернути 0;

        повернути sym_ZSTD_DStreamOutSize();
#інакше
        повернути 0;
#endif
}

static ціле decompress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#якщо HAVE_ZSTD
        ціле r;

        r = dlopen_zstd(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        якщо (size == ZSTD_CONTENTSIZE_ERROR)
                повернути -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        якщо (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        інакше {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        якщо (!greedy_realloc(dst, space, 1))
                повернути -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        якщо (!dctx)
                повернути -ENOMEM;

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
                якщо (sym_ZSTD_isError(k))
                        повернути log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                якщо (output.pos >= cap)
                        break;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                якщо (k == 0 && input.pos >= input.size)
                        break;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                якщо (output.pos == output.size) {
                        якщо (space > SIZE_MAX / 2)
                                повернути -E2BIG;
                        space = MIN(2 * space, cap);
                        якщо (!greedy_realloc(dst, space, 1))
                                повернути -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        continue;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                якщо (input.pos >= input.size)
                        повернути log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле decompress_blob_gzip(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#якщо HAVE_ZLIB
        ціле r;

        r = dlopen_zlib(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size > UINT_MAX)
                повернути -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ціле) sizeof(s));
        якщо (r != Z_OK)
                повернути -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        якщо (!greedy_realloc(dst, space, 1))
                повернути -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        для (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                якщо (r == Z_STREAM_END)
                        break;
                якщо (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        повернути -EBADMSG;

                якщо (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                якщо (dst_max > 0 && space == dst_max)
                        повернути -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                якщо (!greedy_realloc(dst, space, 1))
                        повернути -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле decompress_blob_bzip2(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#якщо HAVE_BZIP2
        ціле r;

        r = dlopen_bzip2(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size > UINT_MAX)
                повернути -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        якщо (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                повернути -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        якщо (!greedy_realloc(dst, space, 1))
                повернути -ENOMEM;

        s.next_in = (char*) src;
        s.avail_in = src_size;
        s.next_out = (char*) *dst;
        s.avail_out = space;

        для (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                якщо (r == BZ_STREAM_END)
                        break;
                якщо (r != BZ_OK)
                        повернути -EBADMSG;

                якщо (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                якщо (dst_max > 0 && space == dst_max)
                        повернути -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                якщо (!greedy_realloc(dst, space, 1))
                        повернути -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (char*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

ціле decompress_blob_journal(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        case COMPRESSION_XZ:
                повернути decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_LZ4:
                повернути decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_ZSTD:
                повернути decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                повернути -EPROTONOSUPPORT;
        }
}

ціле decompress_blob(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        case COMPRESSION_GZIP:
                повернути decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_BZIP2:
                повернути decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                повернути decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

ціле decompress_zlib_raw(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_size,
                ціле wbits) {

#якщо HAVE_ZLIB
        ціле r;

        r = dlopen_zlib(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size > UINT_MAX)
                повернути -EFBIG;
        якщо (dst_size > UINT_MAX)
                повернути -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (void*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (ціле) sizeof(s));
        якщо (r != Z_OK)
                повернути -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        якщо (r != Z_STREAM_END || produced != dst_size)
                повернути -EBADMSG;

        повернути 0;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле decompress_startswith_xz(
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

#якщо HAVE_XZ
        ціле r;

        r = dlopen_xz(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        якщо (ret != LZMA_OK)
                повернути -EBADMSG;

        якщо (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                повернути -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        для (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                якщо (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        повернути -EBADMSG;

                якщо (allocated - s.avail_out >= prefix_len + 1)
                        повернути memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                якщо (ret == LZMA_STREAM_END)
                        повернути 0;

                s.avail_out += allocated;

                якщо (!(greedy_realloc(buffer, allocated * 2, 1)))
                        повернути -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле decompress_startswith_lz4(
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

#якщо HAVE_LZ4
        size_t allocated;
        ціле r;

        r = dlopen_lz4(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size <= 8)
                повернути -EBADMSG;

        якщо (src_size - 8 > INT_MAX)
                повернути -EFBIG;

        якщо (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                повернути -ENOMEM;
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
        якщо (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                якщо (sym_LZ4_versionNumber() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        повернути 0;

                якщо (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        якщо (memcmp(*buffer, prefix, r) != 0)
                                повернути 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                якщо (r < 0)
                        повернути r;

                якщо (size < prefix_len + 1)
                        повернути 0;
        }

        повернути memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле decompress_startswith_zstd(
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

#якщо HAVE_ZSTD
        ціле r;

        r = dlopen_zstd(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        якщо (size == ZSTD_CONTENTSIZE_ERROR)
                повернути -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        якщо (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                повернути 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        якщо (!dctx)
                повернути -ENOMEM;

        якщо (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                повернути -ENOMEM;

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
        якщо (sym_ZSTD_isError(k))
                повернути log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        якщо (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                якщо (k != 0)
                        повернути log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                повернути 0; /* Decompressed text too short to match the prefix and extra */
        }

        повернути memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле decompress_startswith_gzip(
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

#якщо HAVE_ZLIB
        ціле r;

        r = dlopen_zlib(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size > UINT_MAX)
                повернути -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ціле) sizeof(s));
        якщо (r != Z_OK)
                повернути -EBADMSG;

        якщо (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                повернути -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (void*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        для (;;) {
                r = sym_inflate(&s, Z_FINISH);

                якщо (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        повернути -EBADMSG;

                якщо (allocated - s.avail_out >= prefix_len + 1)
                        повернути memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                якщо (r == Z_STREAM_END)
                        повернути 0;

                якщо (s.avail_out > 0)
                        повернути -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                якщо (!(greedy_realloc(buffer, allocated * 2, 1)))
                        повернути -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

static ціле decompress_startswith_bzip2(
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

#якщо HAVE_BZIP2
        ціле r;

        r = dlopen_bzip2(LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        якщо (src_size > UINT_MAX)
                повернути -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        якщо (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                повернути -EIO;

        якщо (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                повернути -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (char*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        для (;;) {
                r = sym_BZ2_bzDecompress(&s);

                якщо (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        повернути -EBADMSG;

                якщо (allocated - s.avail_out >= prefix_len + 1)
                        повернути memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                якщо (r == BZ_STREAM_END)
                        повернути 0;

                якщо (s.avail_out > 0)
                        повернути -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                якщо (!(greedy_realloc(buffer, allocated * 2, 1)))
                        повернути -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (char*) *buffer + used;
        }
#інакше
        повернути -EPROTONOSUPPORT;
#endif
}

ціле decompress_startswith_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        case COMPRESSION_XZ:
                повернути decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_LZ4:
                повернути decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_ZSTD:
                повернути decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                повернути -EOPNOTSUPP;
        }
}

ціле decompress_startswith(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        case COMPRESSION_GZIP:
                повернути decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_BZIP2:
                повернути decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                повернути decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

ціле compress_stream(
                Compression type,
                ціле fdf, ціле fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = NULL;
        _cleanup_free_ void *buf = NULL;
        _cleanup_free_ uint8_t *input = NULL;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        ціле r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        r = compressor_new(&c, type);
        якщо (r < 0)
                повернути r;

        input = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        якщо (!input)
                повернути -ENOMEM;

        для (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                якщо (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = read(fdf, input, m);
                якщо (n < 0)
                        повернути -errno;

                якщо (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        якщо (r < 0)
                                повернути r;

                        якщо (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                якщо (r < 0)
                                        повернути r;
                                total_out += buf_size;
                        }
                        break;
                }

                total_in += n;
                якщо (max_bytes != UINT64_MAX) {
                        assert(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, input, n, &buf, &buf_size, &buf_alloc);
                якщо (r < 0)
                        повернути r;

                якщо (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        якщо (r < 0)
                                повернути r;
                        total_out += buf_size;
                }
        }

        якщо (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        якщо (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        інакше
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (double) total_out / total_in * 100);

        повернути 0;
}

static ціле should_sparse(ціле fd) {
        struct stat st;

        assert(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        якщо (fstat(fd, &st) < 0)
                повернути -errno;

        якщо (!S_ISREG(st.st_mode))
                повернути false;

        ціле flags = fcntl(fd, F_GETFL);
        якщо (flags < 0)
                повернути -errno;

        якщо (FLAGS_SET(flags, O_APPEND))
                повернути false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        якщо (pos < 0)
                повернути -errno;

        повернути pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
static ціле finalize_sparse(ціле fd) {
        off_t pos;

        assert(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        якщо (pos < 0)
                повернути -errno;

        якщо (ftruncate(fd, pos) < 0)
                повернути -errno;

        повернути 0;
}

/* Common helper for decompress_stream_*() wrappers */

struct decompress_stream_userdata {
        ціле fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

static ціле decompress_stream_write_callback(const void *data, size_t size, void *userdata) {
        struct decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        якщо (u->max_bytes != UINT64_MAX) {
                якщо (u->max_bytes < size)
                        повернути -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        якщо (u->skip > 0) {
                якщо (u->skip >= size) {
                        u->skip -= size;
                        повернути 0;
                }

                data = (const uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        якщо (size == 0)
                повернути 0;

        якщо (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                якщо (k < 0)
                        повернути (ціле) k;
                повернути 0;
        }

        повернути loop_write(u->fd, data, size);
}

static ціле decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        assert(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        якщо (!c)
                повернути -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        switch (type) {
        case COMPRESSION_NONE:
                break;

#якщо HAVE_XZ
        case COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                якщо (xz_ret != LZMA_OK)
                        повернути -EIO;
                break;
        }
#endif

#якщо HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                якщо (sym_LZ4F_isError(rc))
                        повернути -ENOMEM;

                break;
        }
#endif

#якщо HAVE_ZSTD
        case COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                якщо (!c->d_zstd)
                        повернути -ENOMEM;
                break;
#endif

#якщо HAVE_ZLIB
        case COMPRESSION_GZIP:
                якщо (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ціле) sizeof(c->gzip)) != Z_OK)
                        повернути -EIO;
                break;
#endif

#якщо HAVE_BZIP2
        case COMPRESSION_BZIP2:
                якщо (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        повернути -EIO;
                break;
#endif

        default:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        повернути 0;
}

static ціле decompress_blob_to_fd_impl(
                Compression type,
                const void *src,
                uint64_t src_size,
                ціле fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ціле r;

        assert(src);
        assert(fdt >= 0);

        якщо (type == COMPRESSION_NONE)
                повернути -EOPNOTSUPP;

        якщо (src_size == 0)
                повернути -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        якщо (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                якщо (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        якщо (src_size <= sizeof(uint64_t))
                                повернути -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        якщо (sz < skip)
                                повернути -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                якщо (max_bytes == 0)
                        повернути -ENOBUFS;

                _cleanup_free_ void *buf = NULL;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                якщо (r < 0)
                        повернути r;

                /* Check the result size again for safety. */
                якщо (size < skip)
                        повернути -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                якщо (r < 0)
                        повернути r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (double) size / src_size * 100);

                якщо (ret_size)
                        *ret_size = size - skip;
                повернути 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        якщо (r < 0)
                повернути r;

        struct decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_push(c, src, src_size, decompress_stream_write_callback, &userdata);
        якщо (r < 0)
                повернути r;

        /* Finalize */
        r = decompressor_push(c, /* data= */ NULL, /* size= */ 0, decompress_stream_write_callback, &userdata);
        якщо (r < 0)
                повернути r;

        якщо (userdata.total_out < skip)
                повернути -EBADMSG;

        якщо (userdata.sparse) {
                r = finalize_sparse(fdt);
                якщо (r < 0)
                        повернути r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (double) userdata.total_out / src_size * 100);

        якщо (ret_size)
                *ret_size = userdata.total_out - skip;

        повернути 0;
}

ціле decompress_blob_to_fd_journal(
                Compression type,
                const void *src,
                uint64_t src_size,
                ціле fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ціле r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        повернути decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

ціле decompress_blob_to_fd(
                Compression type,
                const void *src,
                uint64_t src_size,
                ціле fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ціле r;

        r = dlopen_compress(type, LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        повернути decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

ціле decompress_stream_full(
                Compression type,
                ціле fdf, ціле fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        _cleanup_free_ uint8_t *buf = NULL;
        uint64_t total_in = 0;
        ціле r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        якщо (type == COMPRESSION_NONE)
                повернути -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        r = decompressor_new(&c, type, flags);
        якщо (r < 0)
                повернути r;

        struct decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        якщо (!buf)
                повернути -ENOMEM;

        для (;;) {
                ssize_t n;

                n = read(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                якщо (n < 0)
                        повернути -errno;

                total_in += n;

                r = decompressor_push(c, buf, n, decompress_stream_write_callback, &userdata);
                якщо (r < 0)
                        повернути r;

                якщо (n == 0)
                        break;
        }

        якщо (total_in == 0)
                повернути log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        якщо (userdata.sparse) {
                r = finalize_sparse(fdt);
                якщо (r < 0)
                        повернути r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (double) userdata.total_out / total_in * 100);

        повернути 0;
}

ціле decompress_stream_by_filename(const char *filename, ціле fdf, ціле fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        якщо (c == COMPRESSION_NONE)
                повернути -EPROTONOSUPPORT;

        повернути decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        якщо (!c)
                повернути NULL;

        switch (c->type) {

#якщо HAVE_XZ
        case COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                break;
#endif

#якщо HAVE_LZ4
        case COMPRESSION_LZ4:
                якщо (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = NULL;
                        c->lz4_header = mfree(c->lz4_header);
                } інакше {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = NULL;
                }
                break;
#endif

#якщо HAVE_ZSTD
        case COMPRESSION_ZSTD:
                якщо (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = NULL;
                } інакше {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = NULL;
                }
                break;
#endif

#якщо HAVE_ZLIB
        case COMPRESSION_GZIP:
                якщо (c->encoding)
                        sym_deflateEnd(&c->gzip);
                інакше
                        sym_inflateEnd(&c->gzip);
                break;
#endif

#якщо HAVE_BZIP2
        case COMPRESSION_BZIP2:
                якщо (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                інакше
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                break;
#endif

        default:
                break;
        }

        повернути mfree(c);
}

Compression compressor_type(const Compressor *c) {
        повернути c ? c->type : _COMPRESSION_INVALID;
}

ціле decompressor_detect(Decompressor **decompressor, const void *data, size_t size) {
        ціле r;

        assert(decompressor);

        якщо (*decompressor)
                повернути 1;

        якщо (size < COMPRESSION_MAGIC_BYTES_MAX)
                повернути 0;

        assert(data);

        Compression type = compression_detect_from_magic(data);
        якщо (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        якщо (r < 0)
                повернути r;

        повернути 1;
}

ціле decompressor_force(Decompressor **decompressor, Compression type) {
        ціле r;

        assert(decompressor);
        assert(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        *decompressor = compressor_free(*decompressor);
        повернути decompressor_new(decompressor, type, /* flags= */ 0);
}

ціле decompressor_force_off(Decompressor **decompressor) {
        повернути decompressor_force(decompressor, COMPRESSION_NONE);
}

ціле decompressor_push(Decompressor *c, const void *data, size_t size, DecompressorCallback callback, void *userdata) {
        ціле r;

        assert(c);
        assert(data || size == 0);
        assert(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        якщо (c->encoding)
                повернути -EINVAL;

        якщо (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                повернути 0;

#якщо HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = NULL;
        якщо (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                якщо (!buffer)
                        повернути -ENOMEM;
        }
#endif

        switch (c->type) {

        case COMPRESSION_NONE:
                якщо (c->finished)
                        повернути -EBADMSG;

                якщо (size == 0) {
                        c->finished = true;
                        повернути 0;
                }

                r = callback(data, size, userdata);
                якщо (r < 0)
                        повернути r;

                повернути 0;

#якщо HAVE_XZ
        case COMPRESSION_XZ:
                якщо (c->finished)
                        повернути -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                для (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        якщо (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                повернути -EBADMSG;

                        якщо (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                повернути -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        якщо (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                якщо (r < 0)
                                        повернути r;
                        }

                        якщо (lzr == LZMA_STREAM_END) {
                                якщо (c->xz.avail_in > 0)
                                        повернути -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                повернути 0;
                        }

                        якщо (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                якщо (size == 0)
                                        повернути -EBADMSG; /* truncated */

                                повернути 0; /* No progress possible with the current input. */
                        }
                }
#endif

#якщо HAVE_LZ4
        case COMPRESSION_LZ4: {
                const uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                для (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, NULL);
                        якщо (sym_LZ4F_isError(rc))
                                повернути -EBADMSG;

                        якщо (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                повернути -EIO;

                        якщо (consumed > 0) {
                                assert(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        якщо (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                якщо (r < 0)
                                        повернути r;
                        }

                        якщо (src_remaining == 0) {
                                якщо (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        повернути 0;
                                }

                                якщо (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                якщо (size == 0)
                                        повернути -EBADMSG; /* truncated */

                                повернути 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#якщо HAVE_ZSTD
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
                        якщо (sym_ZSTD_isError(res))
                                повернути -EBADMSG;

                        якщо (input.pos > input.size ||
                            output.pos > output.size)
                                повернути -EIO;

                        якщо (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                якщо (r < 0)
                                        повернути r;
                        }

                        якщо (input.pos == input.size) {
                                якщо (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        повернути 0;
                                }

                                якщо (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                якщо (size == 0)
                                        повернути -EBADMSG; /* truncated */

                                повернути 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#якщо HAVE_ZLIB
        case COMPRESSION_GZIP:
                якщо (size > UINT_MAX)
                        повернути -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                для (;;) {
                        якщо (c->finished) {
                                якщо (sym_inflateReset(&c->gzip) != Z_OK)
                                        повернути -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        ціле zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        якщо (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                повернути -EBADMSG;

                        якщо (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                повернути -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        якщо (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                якщо (r < 0)
                                        повернути r;
                        }

                        якщо (zr == Z_STREAM_END) {
                                c->finished = true;

                                якщо (c->gzip.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                повернути 0;
                        }

                        якщо (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                якщо (size == 0)
                                        повернути -EBADMSG; /* truncated */

                                повернути 0; /* No progress possible with the current input. */
                        }
                }
#endif

#якщо HAVE_BZIP2
        case COMPRESSION_BZIP2:
                якщо (size > UINT_MAX)
                        повернути -EFBIG;

                c->bzip2.next_in = (char*) data;
                c->bzip2.avail_in = size;

                для (;;) {
                        якщо (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                якщо (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        повернути -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (char*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        ціле bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        якщо (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                повернути -EBADMSG;

                        якщо (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                повернути -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        якщо (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                якщо (r < 0)
                                        повернути r;
                        }

                        якщо (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                якщо (c->bzip2.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                повернути 0;
                        }

                        якщо (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                якщо (size == 0)
                                        повернути -EBADMSG; /* truncated */

                                повернути 0; /* No progress possible with the current input. */
                        }
                }
#endif

        default:
                assert_not_reached();
        }
}

ціле compressor_new(Compressor **ret, Compression type) {
        ціле r;

        assert(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        якщо (r < 0)
                повернути r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        якщо (!c)
                повернути -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        switch (type) {

#якщо HAVE_XZ
        case COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                якщо (xzr != LZMA_OK)
                        повернути -EIO;

                c->type = COMPRESSION_XZ;
                break;
        }
#endif

#якщо HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                якщо (sym_LZ4F_isError(rc))
                        повернути -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                якщо (!c->lz4_header)
                        повернути -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                якщо (sym_LZ4F_isError(c->lz4_header_size))
                        повернути -EINVAL;

                break;
        }
#endif

#якщо HAVE_ZSTD
        case COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                якщо (!c->c_zstd)
                        повернути -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                якщо (sym_ZSTD_isError(z))
                        повернути -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                якщо (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                break;
#endif

#якщо HAVE_ZLIB
        case COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (ціле) sizeof(c->gzip));
                якщо (r != Z_OK)
                        повернути -EIO;

                c->type = COMPRESSION_GZIP;
                break;
#endif

#якщо HAVE_BZIP2
        case COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                якщо (r != BZ_OK)
                        повернути -EIO;

                c->type = COMPRESSION_BZIP2;
                break;
#endif

        case COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                break;

        default:
                повернути -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        повернути 0;
}

#якщо HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
static ціле enlarge_buffer(void **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        якщо (*buffer_allocated >= need)
                повернути 0;

        якщо (!greedy_realloc(buffer, need, 1))
                повернути -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        повернути 1;
}
#endif

ціле compressor_start(
                Compressor *c,
                const void *data,
                size_t size,
                void **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#якщо HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ціле r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        якщо (!c->encoding)
                повернути -EINVAL;

        якщо (size == 0)
                повернути 0;

        assert(data);

        *buffer_size = 0;

        switch (c->type) {

#якщо HAVE_XZ
        case COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                поки (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        якщо (r < 0)
                                повернути r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        якщо (lzr != LZMA_OK)
                                повернути -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                break;
#endif

#якщо HAVE_LZ4
        case COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                якщо (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        якщо (r < 0)
                                повернути r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                якщо (r < 0)
                        повернути r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, NULL);
                якщо (sym_LZ4F_isError(n))
                        повернути -EIO;

                *buffer_size += n;
                break;
        }
#endif

#якщо HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src = data,
                        .size = size,
                };

                поки (input.pos < input.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        якщо (r < 0)
                                повернути r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_continue);
                        якщо (sym_ZSTD_isError(res))
                                повернути -EIO;

                        *buffer_size += output.pos;
                }

                break;
        }
#endif

#якщо HAVE_ZLIB
        case COMPRESSION_GZIP:
                якщо (size > UINT_MAX)
                        повернути -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                поки (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        якщо (r < 0)
                                повернути r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        якщо (r != Z_OK)
                                повернути -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                break;
#endif

#якщо HAVE_BZIP2
        case COMPRESSION_BZIP2:
                якщо (size > UINT_MAX)
                        повернути -EFBIG;

                c->bzip2.next_in = (void*) data;
                c->bzip2.avail_in = size;

                поки (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        якщо (r < 0)
                                повернути r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        якщо (r != BZ_RUN_OK)
                                повернути -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                break;
#endif

        case COMPRESSION_NONE:

                якщо (*buffer_allocated < size) {
                        void *p;

                        p = realloc(*buffer, size);
                        якщо (!p)
                                повернути -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                break;

        default:
                повернути -EOPNOTSUPP;
        }

        повернути 0;
}

ціле compressor_finish(Compressor *c, void **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#якщо HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ціле r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        якщо (!c->encoding)
                повернути -EINVAL;

        *buffer_size = 0;

        switch (c->type) {

#якщо HAVE_XZ
        case COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        якщо (r < 0)
                                повернути r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        якщо (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                повернути -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } поки (lzr != LZMA_STREAM_END);

                break;
        }
#endif

#якщо HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                якщо (r < 0)
                        повернути r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, NULL);
                якщо (sym_LZ4F_isError(n))
                        повернути -EIO;

                *buffer_size = n;
                break;
        }
#endif

#якщо HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {};
                size_t res;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        якщо (r < 0)
                                повернути r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_end);
                        якщо (sym_ZSTD_isError(res))
                                повернути -EIO;

                        *buffer_size += output.pos;
                } поки (res != 0);

                break;
        }
#endif

#якщо HAVE_ZLIB
        case COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        якщо (r < 0)
                                повернути r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        якщо (!IN_SET(r, Z_OK, Z_STREAM_END))
                                повернути -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } поки (r != Z_STREAM_END);

                break;
#endif

#якщо HAVE_BZIP2
        case COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        якщо (r < 0)
                                повернути r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        якщо (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                повернути -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } поки (r != BZ_STREAM_END);

                break;
#endif

        case COMPRESSION_NONE:
                break;

        default:
                повернути -EOPNOTSUPP;
        }

        повернути 0;
}
