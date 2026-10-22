/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#nếu HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#include <lzma.h>
#endif

#nếu HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#include <lz4.h>
#include <lz4frame.h>
#include <lz4hc.h>
#endif

#nếu HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#include <zstd.h>
#include <zstd_errors.h>
#endif

#nếu HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#include <zlib.h>
#endif

#nếu HAVE_BZIP2
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

#nếu HAVE_XZ
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

#nếu HAVE_LZ4
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

#nếu HAVE_ZSTD
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

static số_nguyên zstd_ret_to_errno(size_t ret) {
        switch (sym_ZSTD_getErrorCode(ret)) {
        case ZSTD_error_dstSize_tooSmall:
                trả_về -ENOBUFS;
        case ZSTD_error_memory_allocation:
                trả_về -ENOMEM;
        default:
                trả_về -EBADMSG;
        }
}
#endif

#nếu HAVE_ZLIB
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

#nếu HAVE_BZIP2
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
#nếu HAVE_XZ
                lzma_stream xz;
#endif
#nếu HAVE_LZ4
                struct {
                        LZ4F_compressionContext_t c_lz4;
                        void *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#nếu HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#nếu HAVE_ZLIB
                z_stream gzip;
#endif
#nếu HAVE_BZIP2
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
        nếu (c >= 0)
                trả_về c;

        trả_về compression_uppercase_from_string(s);
}

Compression compression_from_filename(const char *filename) {
        Compression c;
        const char *e;

        assert(filename);

        e = strrchr(filename, '.');
        nếu (!e)
                trả_về COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        nếu (c < 0)
                trả_về COMPRESSION_NONE;

        trả_về c;
}

bool compression_supported_journal(Compression c) {
        static const unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        assert(c >= 0);
        assert(c < _COMPRESSION_MAX);

        trả_về BIT_SET(supported, c);
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

        trả_về BIT_SET(supported, c);
}

Compression compression_detect_from_magic(const uint8_t data[static COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        nếu (memcmp(data, (const uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                trả_về COMPRESSION_GZIP;
        nếu (memcmp(data, (const uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                trả_về COMPRESSION_XZ;
        nếu (memcmp(data, (const uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                trả_về COMPRESSION_ZSTD;
        nếu (memcmp(data, (const uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                trả_về COMPRESSION_LZ4;
        nếu (memcmp(data, (const uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                trả_về COMPRESSION_BZIP2;

        trả_về _COMPRESSION_INVALID;
}

số_nguyên dlopen_xz(số_nguyên log_level) {
#nếu HAVE_XZ
        static void *lzma_dl = NULL;

        LIBLZMA_NOTE(suggested);

        trả_về dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#không_thì
        trả_về log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

số_nguyên dlopen_lz4(số_nguyên log_level) {
#nếu HAVE_LZ4
        static void *lz4_dl = NULL;

        LIBLZ4_NOTE(suggested);

        trả_về dlopen_many_sym_or_warn(
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
#không_thì
        trả_về log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

số_nguyên dlopen_zstd(số_nguyên log_level) {
#nếu HAVE_ZSTD
        static void *zstd_dl = NULL;

        LIBZSTD_NOTE(suggested);

        trả_về dlopen_many_sym_or_warn(
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
#không_thì
        trả_về log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

số_nguyên dlopen_zlib(số_nguyên log_level) {
#nếu HAVE_ZLIB
        static void *zlib_dl = NULL;

        LIBZ_NOTE(suggested);

        trả_về dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#không_thì
        trả_về log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

số_nguyên dlopen_bzip2(số_nguyên log_level) {
#nếu HAVE_BZIP2
        static void *bzip2_dl = NULL;

        LIBBZ2_NOTE(suggested);

        trả_về dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#không_thì
        trả_về log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

số_nguyên dlopen_compress_journal(Compression c, số_nguyên log_level) {
        switch (c) {
        case COMPRESSION_NONE:
                trả_về 0;
        case COMPRESSION_XZ:
                trả_về dlopen_xz(log_level);
        case COMPRESSION_LZ4:
                trả_về dlopen_lz4(log_level);
        case COMPRESSION_ZSTD:
                trả_về dlopen_zstd(log_level);
        default:
                trả_về -EOPNOTSUPP;
        }
}

số_nguyên dlopen_compress(Compression c, số_nguyên log_level) {
        switch (c) {
        case COMPRESSION_GZIP:
                trả_về dlopen_zlib(log_level);
        case COMPRESSION_BZIP2:
                trả_về dlopen_bzip2(log_level);
        default:
                trả_về dlopen_compress_journal(c, log_level);
        }
}

static số_nguyên compress_blob_xz(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                số_nguyên level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#nếu HAVE_XZ
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
        số_nguyên r;

        r = dlopen_xz(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                nếu (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        trả_về -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        nếu (src_size < 80)
                trả_về -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ NULL,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        nếu (ret != LZMA_OK)
                trả_về -ENOBUFS;

        *dst_size = out_pos;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên compress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                số_nguyên level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#nếu HAVE_LZ4
        số_nguyên r;

        r = dlopen_lz4(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        nếu (src_size < 9)
                trả_về -ENOBUFS;

        nếu (src_size > INT_MAX)
                trả_về -EFBIG;
        nếu (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        nếu (level <= 0)
                r = sym_LZ4_compress_default(src, (char*)dst + 8, src_size, (số_nguyên) dst_alloc_size - 8);
        không_thì
                r = sym_LZ4_compress_HC(src, (char*)dst + 8, src_size, (số_nguyên) dst_alloc_size - 8, level);
        nếu (r <= 0)
                trả_về -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên compress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                số_nguyên level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#nếu HAVE_ZSTD
        size_t k;
        số_nguyên r;

        r = dlopen_zstd(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        nếu (sym_ZSTD_isError(k))
                trả_về zstd_ret_to_errno(k);

        *dst_size = k;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên compress_blob_gzip(const void *src, uint64_t src_size,
                       void *dst, size_t dst_alloc_size, size_t *dst_size, số_nguyên level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#nếu HAVE_ZLIB
        số_nguyên r;

        r = dlopen_zlib(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size > UINT_MAX)
                trả_về -EFBIG;
        nếu (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (số_nguyên) sizeof(s));
        nếu (r != Z_OK)
                trả_về -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        nếu (r != Z_STREAM_END)
                trả_về -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên compress_blob_bzip2(
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, số_nguyên level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#nếu HAVE_BZIP2
        số_nguyên r;

        r = dlopen_bzip2(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size > UINT_MAX)
                trả_về -EFBIG;
        nếu (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        nếu (r != BZ_OK)
                trả_về -ENOMEM;

        s.next_in = (char*) src;
        s.avail_in = src_size;
        s.next_out = (char*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        nếu (r != BZ_STREAM_END)
                trả_về -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

số_nguyên compress_blob_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, số_nguyên level) {

        switch (compression) {
        case COMPRESSION_XZ:
                trả_về compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_LZ4:
                trả_về compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_ZSTD:
                trả_về compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                trả_về -EOPNOTSUPP;
        }
}

số_nguyên compress_blob(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, số_nguyên level) {

        switch (compression) {
        case COMPRESSION_GZIP:
                trả_về compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_BZIP2:
                trả_về compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                trả_về compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

static số_nguyên decompress_blob_xz(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#nếu HAVE_XZ
        số_nguyên r;

        r = dlopen_xz(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        nếu (ret != LZMA_OK)
                trả_về -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        nếu (!greedy_realloc(dst, space, 1))
                trả_về -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        cho (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                nếu (ret == LZMA_STREAM_END)
                        break;
                nếu (ret != LZMA_OK)
                        trả_về -ENOMEM;

                nếu (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                nếu (dst_max > 0 && space == dst_max)
                        trả_về -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                assert(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                nếu (!greedy_realloc(dst, space, 1))
                        trả_về -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên decompress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#nếu HAVE_LZ4
        char* out;
        số_nguyên r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size <= 8)
                trả_về -EBADMSG;

        nếu (src_size - 8 > INT_MAX)
                trả_về -EFBIG;

        size = unaligned_read_le64(src);
        nếu (size < 0 || (unsigned) size != unaligned_read_le64(src))
                trả_về -EFBIG;
        nếu (dst_max > 0 && (size_t) size > dst_max)
                trả_về -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        nếu (!out)
                trả_về -ENOMEM;

        r = sym_LZ4_decompress_safe((char*)src + 8, out, src_size - 8, size);
        nếu (r < 0 || r != size)
                trả_về -EBADMSG;

        *dst_size = size;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(void) {
#nếu HAVE_ZSTD
        nếu (dlopen_zstd(LOG_DEBUG) < 0)
                trả_về 0;

        trả_về sym_ZSTD_DStreamOutSize();
#không_thì
        trả_về 0;
#endif
}

static số_nguyên decompress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#nếu HAVE_ZSTD
        số_nguyên r;

        r = dlopen_zstd(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        nếu (size == ZSTD_CONTENTSIZE_ERROR)
                trả_về -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        nếu (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        không_thì {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        nếu (!greedy_realloc(dst, space, 1))
                trả_về -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        nếu (!dctx)
                trả_về -ENOMEM;

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

        cho (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &input);
                nếu (sym_ZSTD_isError(k))
                        trả_về log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                nếu (output.pos >= cap)
                        break;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                nếu (k == 0 && input.pos >= input.size)
                        break;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                nếu (output.pos == output.size) {
                        nếu (space > SIZE_MAX / 2)
                                trả_về -E2BIG;
                        space = MIN(2 * space, cap);
                        nếu (!greedy_realloc(dst, space, 1))
                                trả_về -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        continue;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                nếu (input.pos >= input.size)
                        trả_về log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên decompress_blob_gzip(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#nếu HAVE_ZLIB
        số_nguyên r;

        r = dlopen_zlib(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size > UINT_MAX)
                trả_về -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (số_nguyên) sizeof(s));
        nếu (r != Z_OK)
                trả_về -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        nếu (!greedy_realloc(dst, space, 1))
                trả_về -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        cho (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                nếu (r == Z_STREAM_END)
                        break;
                nếu (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        trả_về -EBADMSG;

                nếu (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                nếu (dst_max > 0 && space == dst_max)
                        trả_về -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                nếu (!greedy_realloc(dst, space, 1))
                        trả_về -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên decompress_blob_bzip2(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#nếu HAVE_BZIP2
        số_nguyên r;

        r = dlopen_bzip2(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size > UINT_MAX)
                trả_về -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        nếu (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                trả_về -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        nếu (!greedy_realloc(dst, space, 1))
                trả_về -ENOMEM;

        s.next_in = (char*) src;
        s.avail_in = src_size;
        s.next_out = (char*) *dst;
        s.avail_out = space;

        cho (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                nếu (r == BZ_STREAM_END)
                        break;
                nếu (r != BZ_OK)
                        trả_về -EBADMSG;

                nếu (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                nếu (dst_max > 0 && space == dst_max)
                        trả_về -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                nếu (!greedy_realloc(dst, space, 1))
                        trả_về -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (char*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

số_nguyên decompress_blob_journal(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        case COMPRESSION_XZ:
                trả_về decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_LZ4:
                trả_về decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_ZSTD:
                trả_về decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                trả_về -EPROTONOSUPPORT;
        }
}

số_nguyên decompress_blob(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        case COMPRESSION_GZIP:
                trả_về decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_BZIP2:
                trả_về decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                trả_về decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

số_nguyên decompress_zlib_raw(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_size,
                số_nguyên wbits) {

#nếu HAVE_ZLIB
        số_nguyên r;

        r = dlopen_zlib(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size > UINT_MAX)
                trả_về -EFBIG;
        nếu (dst_size > UINT_MAX)
                trả_về -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (void*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (số_nguyên) sizeof(s));
        nếu (r != Z_OK)
                trả_về -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        nếu (r != Z_STREAM_END || produced != dst_size)
                trả_về -EBADMSG;

        trả_về 0;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên decompress_startswith_xz(
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

#nếu HAVE_XZ
        số_nguyên r;

        r = dlopen_xz(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        nếu (ret != LZMA_OK)
                trả_về -EBADMSG;

        nếu (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                trả_về -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        cho (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                nếu (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        trả_về -EBADMSG;

                nếu (allocated - s.avail_out >= prefix_len + 1)
                        trả_về memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                nếu (ret == LZMA_STREAM_END)
                        trả_về 0;

                s.avail_out += allocated;

                nếu (!(greedy_realloc(buffer, allocated * 2, 1)))
                        trả_về -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên decompress_startswith_lz4(
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

#nếu HAVE_LZ4
        size_t allocated;
        số_nguyên r;

        r = dlopen_lz4(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size <= 8)
                trả_về -EBADMSG;

        nếu (src_size - 8 > INT_MAX)
                trả_về -EFBIG;

        nếu (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                trả_về -ENOMEM;
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
        nếu (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                nếu (sym_LZ4_versionNumber() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        trả_về 0;

                nếu (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        nếu (memcmp(*buffer, prefix, r) != 0)
                                trả_về 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                nếu (r < 0)
                        trả_về r;

                nếu (size < prefix_len + 1)
                        trả_về 0;
        }

        trả_về memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên decompress_startswith_zstd(
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

#nếu HAVE_ZSTD
        số_nguyên r;

        r = dlopen_zstd(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        nếu (size == ZSTD_CONTENTSIZE_ERROR)
                trả_về -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        nếu (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                trả_về 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        nếu (!dctx)
                trả_về -ENOMEM;

        nếu (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                trả_về -ENOMEM;

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
        nếu (sym_ZSTD_isError(k))
                trả_về log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        nếu (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                nếu (k != 0)
                        trả_về log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                trả_về 0; /* Decompressed text too short to match the prefix and extra */
        }

        trả_về memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên decompress_startswith_gzip(
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

#nếu HAVE_ZLIB
        số_nguyên r;

        r = dlopen_zlib(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size > UINT_MAX)
                trả_về -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (số_nguyên) sizeof(s));
        nếu (r != Z_OK)
                trả_về -EBADMSG;

        nếu (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                trả_về -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (void*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        cho (;;) {
                r = sym_inflate(&s, Z_FINISH);

                nếu (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        trả_về -EBADMSG;

                nếu (allocated - s.avail_out >= prefix_len + 1)
                        trả_về memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                nếu (r == Z_STREAM_END)
                        trả_về 0;

                nếu (s.avail_out > 0)
                        trả_về -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                nếu (!(greedy_realloc(buffer, allocated * 2, 1)))
                        trả_về -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

static số_nguyên decompress_startswith_bzip2(
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

#nếu HAVE_BZIP2
        số_nguyên r;

        r = dlopen_bzip2(LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        nếu (src_size > UINT_MAX)
                trả_về -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        nếu (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                trả_về -EIO;

        nếu (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                trả_về -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (char*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        cho (;;) {
                r = sym_BZ2_bzDecompress(&s);

                nếu (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        trả_về -EBADMSG;

                nếu (allocated - s.avail_out >= prefix_len + 1)
                        trả_về memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                nếu (r == BZ_STREAM_END)
                        trả_về 0;

                nếu (s.avail_out > 0)
                        trả_về -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                nếu (!(greedy_realloc(buffer, allocated * 2, 1)))
                        trả_về -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (char*) *buffer + used;
        }
#không_thì
        trả_về -EPROTONOSUPPORT;
#endif
}

số_nguyên decompress_startswith_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        case COMPRESSION_XZ:
                trả_về decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_LZ4:
                trả_về decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_ZSTD:
                trả_về decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                trả_về -EOPNOTSUPP;
        }
}

số_nguyên decompress_startswith(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        case COMPRESSION_GZIP:
                trả_về decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_BZIP2:
                trả_về decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                trả_về decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

số_nguyên compress_stream(
                Compression type,
                số_nguyên fdf, số_nguyên fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = NULL;
        _cleanup_free_ void *buf = NULL;
        _cleanup_free_ uint8_t *input = NULL;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        số_nguyên r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        r = compressor_new(&c, type);
        nếu (r < 0)
                trả_về r;

        input = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        nếu (!input)
                trả_về -ENOMEM;

        cho (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                nếu (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = read(fdf, input, m);
                nếu (n < 0)
                        trả_về -errno;

                nếu (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        nếu (r < 0)
                                trả_về r;

                        nếu (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                nếu (r < 0)
                                        trả_về r;
                                total_out += buf_size;
                        }
                        break;
                }

                total_in += n;
                nếu (max_bytes != UINT64_MAX) {
                        assert(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, input, n, &buf, &buf_size, &buf_alloc);
                nếu (r < 0)
                        trả_về r;

                nếu (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        nếu (r < 0)
                                trả_về r;
                        total_out += buf_size;
                }
        }

        nếu (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        nếu (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        không_thì
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (double) total_out / total_in * 100);

        trả_về 0;
}

static số_nguyên should_sparse(số_nguyên fd) {
        struct stat st;

        assert(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        nếu (fstat(fd, &st) < 0)
                trả_về -errno;

        nếu (!S_ISREG(st.st_mode))
                trả_về false;

        số_nguyên flags = fcntl(fd, F_GETFL);
        nếu (flags < 0)
                trả_về -errno;

        nếu (FLAGS_SET(flags, O_APPEND))
                trả_về false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        nếu (pos < 0)
                trả_về -errno;

        trả_về pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
static số_nguyên finalize_sparse(số_nguyên fd) {
        off_t pos;

        assert(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        nếu (pos < 0)
                trả_về -errno;

        nếu (ftruncate(fd, pos) < 0)
                trả_về -errno;

        trả_về 0;
}

/* Common helper for decompress_stream_*() wrappers */

struct decompress_stream_userdata {
        số_nguyên fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

static số_nguyên decompress_stream_write_callback(const void *data, size_t size, void *userdata) {
        struct decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        nếu (u->max_bytes != UINT64_MAX) {
                nếu (u->max_bytes < size)
                        trả_về -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        nếu (u->skip > 0) {
                nếu (u->skip >= size) {
                        u->skip -= size;
                        trả_về 0;
                }

                data = (const uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        nếu (size == 0)
                trả_về 0;

        nếu (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                nếu (k < 0)
                        trả_về (số_nguyên) k;
                trả_về 0;
        }

        trả_về loop_write(u->fd, data, size);
}

static số_nguyên decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        assert(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        nếu (!c)
                trả_về -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        switch (type) {
        case COMPRESSION_NONE:
                break;

#nếu HAVE_XZ
        case COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                nếu (xz_ret != LZMA_OK)
                        trả_về -EIO;
                break;
        }
#endif

#nếu HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                nếu (sym_LZ4F_isError(rc))
                        trả_về -ENOMEM;

                break;
        }
#endif

#nếu HAVE_ZSTD
        case COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                nếu (!c->d_zstd)
                        trả_về -ENOMEM;
                break;
#endif

#nếu HAVE_ZLIB
        case COMPRESSION_GZIP:
                nếu (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (số_nguyên) sizeof(c->gzip)) != Z_OK)
                        trả_về -EIO;
                break;
#endif

#nếu HAVE_BZIP2
        case COMPRESSION_BZIP2:
                nếu (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        trả_về -EIO;
                break;
#endif

        default:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        trả_về 0;
}

static số_nguyên decompress_blob_to_fd_impl(
                Compression type,
                const void *src,
                uint64_t src_size,
                số_nguyên fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        số_nguyên r;

        assert(src);
        assert(fdt >= 0);

        nếu (type == COMPRESSION_NONE)
                trả_về -EOPNOTSUPP;

        nếu (src_size == 0)
                trả_về -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        nếu (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                nếu (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        nếu (src_size <= sizeof(uint64_t))
                                trả_về -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        nếu (sz < skip)
                                trả_về -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                nếu (max_bytes == 0)
                        trả_về -ENOBUFS;

                _cleanup_free_ void *buf = NULL;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                nếu (r < 0)
                        trả_về r;

                /* Check the result size again for safety. */
                nếu (size < skip)
                        trả_về -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                nếu (r < 0)
                        trả_về r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (double) size / src_size * 100);

                nếu (ret_size)
                        *ret_size = size - skip;
                trả_về 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        nếu (r < 0)
                trả_về r;

        struct decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_push(c, src, src_size, decompress_stream_write_callback, &userdata);
        nếu (r < 0)
                trả_về r;

        /* Finalize */
        r = decompressor_push(c, /* data= */ NULL, /* size= */ 0, decompress_stream_write_callback, &userdata);
        nếu (r < 0)
                trả_về r;

        nếu (userdata.total_out < skip)
                trả_về -EBADMSG;

        nếu (userdata.sparse) {
                r = finalize_sparse(fdt);
                nếu (r < 0)
                        trả_về r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (double) userdata.total_out / src_size * 100);

        nếu (ret_size)
                *ret_size = userdata.total_out - skip;

        trả_về 0;
}

số_nguyên decompress_blob_to_fd_journal(
                Compression type,
                const void *src,
                uint64_t src_size,
                số_nguyên fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        số_nguyên r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        trả_về decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

số_nguyên decompress_blob_to_fd(
                Compression type,
                const void *src,
                uint64_t src_size,
                số_nguyên fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        số_nguyên r;

        r = dlopen_compress(type, LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        trả_về decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

số_nguyên decompress_stream_full(
                Compression type,
                số_nguyên fdf, số_nguyên fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        _cleanup_free_ uint8_t *buf = NULL;
        uint64_t total_in = 0;
        số_nguyên r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        nếu (type == COMPRESSION_NONE)
                trả_về -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        r = decompressor_new(&c, type, flags);
        nếu (r < 0)
                trả_về r;

        struct decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        nếu (!buf)
                trả_về -ENOMEM;

        cho (;;) {
                ssize_t n;

                n = read(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                nếu (n < 0)
                        trả_về -errno;

                total_in += n;

                r = decompressor_push(c, buf, n, decompress_stream_write_callback, &userdata);
                nếu (r < 0)
                        trả_về r;

                nếu (n == 0)
                        break;
        }

        nếu (total_in == 0)
                trả_về log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        nếu (userdata.sparse) {
                r = finalize_sparse(fdt);
                nếu (r < 0)
                        trả_về r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (double) userdata.total_out / total_in * 100);

        trả_về 0;
}

số_nguyên decompress_stream_by_filename(const char *filename, số_nguyên fdf, số_nguyên fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        nếu (c == COMPRESSION_NONE)
                trả_về -EPROTONOSUPPORT;

        trả_về decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        nếu (!c)
                trả_về NULL;

        switch (c->type) {

#nếu HAVE_XZ
        case COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                break;
#endif

#nếu HAVE_LZ4
        case COMPRESSION_LZ4:
                nếu (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = NULL;
                        c->lz4_header = mfree(c->lz4_header);
                } không_thì {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = NULL;
                }
                break;
#endif

#nếu HAVE_ZSTD
        case COMPRESSION_ZSTD:
                nếu (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = NULL;
                } không_thì {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = NULL;
                }
                break;
#endif

#nếu HAVE_ZLIB
        case COMPRESSION_GZIP:
                nếu (c->encoding)
                        sym_deflateEnd(&c->gzip);
                không_thì
                        sym_inflateEnd(&c->gzip);
                break;
#endif

#nếu HAVE_BZIP2
        case COMPRESSION_BZIP2:
                nếu (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                không_thì
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                break;
#endif

        default:
                break;
        }

        trả_về mfree(c);
}

Compression compressor_type(const Compressor *c) {
        trả_về c ? c->type : _COMPRESSION_INVALID;
}

số_nguyên decompressor_detect(Decompressor **decompressor, const void *data, size_t size) {
        số_nguyên r;

        assert(decompressor);

        nếu (*decompressor)
                trả_về 1;

        nếu (size < COMPRESSION_MAGIC_BYTES_MAX)
                trả_về 0;

        assert(data);

        Compression type = compression_detect_from_magic(data);
        nếu (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        nếu (r < 0)
                trả_về r;

        trả_về 1;
}

số_nguyên decompressor_force(Decompressor **decompressor, Compression type) {
        số_nguyên r;

        assert(decompressor);
        assert(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        *decompressor = compressor_free(*decompressor);
        trả_về decompressor_new(decompressor, type, /* flags= */ 0);
}

số_nguyên decompressor_force_off(Decompressor **decompressor) {
        trả_về decompressor_force(decompressor, COMPRESSION_NONE);
}

số_nguyên decompressor_push(Decompressor *c, const void *data, size_t size, DecompressorCallback callback, void *userdata) {
        số_nguyên r;

        assert(c);
        assert(data || size == 0);
        assert(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        nếu (c->encoding)
                trả_về -EINVAL;

        nếu (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                trả_về 0;

#nếu HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = NULL;
        nếu (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                nếu (!buffer)
                        trả_về -ENOMEM;
        }
#endif

        switch (c->type) {

        case COMPRESSION_NONE:
                nếu (c->finished)
                        trả_về -EBADMSG;

                nếu (size == 0) {
                        c->finished = true;
                        trả_về 0;
                }

                r = callback(data, size, userdata);
                nếu (r < 0)
                        trả_về r;

                trả_về 0;

#nếu HAVE_XZ
        case COMPRESSION_XZ:
                nếu (c->finished)
                        trả_về -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                cho (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        nếu (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                trả_về -EBADMSG;

                        nếu (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                trả_về -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        nếu (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                nếu (r < 0)
                                        trả_về r;
                        }

                        nếu (lzr == LZMA_STREAM_END) {
                                nếu (c->xz.avail_in > 0)
                                        trả_về -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                trả_về 0;
                        }

                        nếu (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                nếu (size == 0)
                                        trả_về -EBADMSG; /* truncated */

                                trả_về 0; /* No progress possible with the current input. */
                        }
                }
#endif

#nếu HAVE_LZ4
        case COMPRESSION_LZ4: {
                const uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                cho (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, NULL);
                        nếu (sym_LZ4F_isError(rc))
                                trả_về -EBADMSG;

                        nếu (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                trả_về -EIO;

                        nếu (consumed > 0) {
                                assert(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        nếu (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                nếu (r < 0)
                                        trả_về r;
                        }

                        nếu (src_remaining == 0) {
                                nếu (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        trả_về 0;
                                }

                                nếu (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                nếu (size == 0)
                                        trả_về -EBADMSG; /* truncated */

                                trả_về 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#nếu HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src =  (void*) data,
                        .size = size,
                };

                c->finished = false;

                cho (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &input);
                        nếu (sym_ZSTD_isError(res))
                                trả_về -EBADMSG;

                        nếu (input.pos > input.size ||
                            output.pos > output.size)
                                trả_về -EIO;

                        nếu (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                nếu (r < 0)
                                        trả_về r;
                        }

                        nếu (input.pos == input.size) {
                                nếu (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        trả_về 0;
                                }

                                nếu (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                nếu (size == 0)
                                        trả_về -EBADMSG; /* truncated */

                                trả_về 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#nếu HAVE_ZLIB
        case COMPRESSION_GZIP:
                nếu (size > UINT_MAX)
                        trả_về -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                cho (;;) {
                        nếu (c->finished) {
                                nếu (sym_inflateReset(&c->gzip) != Z_OK)
                                        trả_về -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        số_nguyên zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        nếu (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                trả_về -EBADMSG;

                        nếu (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                trả_về -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        nếu (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                nếu (r < 0)
                                        trả_về r;
                        }

                        nếu (zr == Z_STREAM_END) {
                                c->finished = true;

                                nếu (c->gzip.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                trả_về 0;
                        }

                        nếu (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                nếu (size == 0)
                                        trả_về -EBADMSG; /* truncated */

                                trả_về 0; /* No progress possible with the current input. */
                        }
                }
#endif

#nếu HAVE_BZIP2
        case COMPRESSION_BZIP2:
                nếu (size > UINT_MAX)
                        trả_về -EFBIG;

                c->bzip2.next_in = (char*) data;
                c->bzip2.avail_in = size;

                cho (;;) {
                        nếu (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                nếu (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        trả_về -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (char*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        số_nguyên bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        nếu (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                trả_về -EBADMSG;

                        nếu (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                trả_về -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        nếu (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                nếu (r < 0)
                                        trả_về r;
                        }

                        nếu (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                nếu (c->bzip2.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                trả_về 0;
                        }

                        nếu (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                nếu (size == 0)
                                        trả_về -EBADMSG; /* truncated */

                                trả_về 0; /* No progress possible with the current input. */
                        }
                }
#endif

        default:
                assert_not_reached();
        }
}

số_nguyên compressor_new(Compressor **ret, Compression type) {
        số_nguyên r;

        assert(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        nếu (r < 0)
                trả_về r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        nếu (!c)
                trả_về -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        switch (type) {

#nếu HAVE_XZ
        case COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                nếu (xzr != LZMA_OK)
                        trả_về -EIO;

                c->type = COMPRESSION_XZ;
                break;
        }
#endif

#nếu HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                nếu (sym_LZ4F_isError(rc))
                        trả_về -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                nếu (!c->lz4_header)
                        trả_về -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                nếu (sym_LZ4F_isError(c->lz4_header_size))
                        trả_về -EINVAL;

                break;
        }
#endif

#nếu HAVE_ZSTD
        case COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                nếu (!c->c_zstd)
                        trả_về -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                nếu (sym_ZSTD_isError(z))
                        trả_về -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                nếu (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                break;
#endif

#nếu HAVE_ZLIB
        case COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (số_nguyên) sizeof(c->gzip));
                nếu (r != Z_OK)
                        trả_về -EIO;

                c->type = COMPRESSION_GZIP;
                break;
#endif

#nếu HAVE_BZIP2
        case COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                nếu (r != BZ_OK)
                        trả_về -EIO;

                c->type = COMPRESSION_BZIP2;
                break;
#endif

        case COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                break;

        default:
                trả_về -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        trả_về 0;
}

#nếu HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
static số_nguyên enlarge_buffer(void **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        nếu (*buffer_allocated >= need)
                trả_về 0;

        nếu (!greedy_realloc(buffer, need, 1))
                trả_về -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        trả_về 1;
}
#endif

số_nguyên compressor_start(
                Compressor *c,
                const void *data,
                size_t size,
                void **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#nếu HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        số_nguyên r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        nếu (!c->encoding)
                trả_về -EINVAL;

        nếu (size == 0)
                trả_về 0;

        assert(data);

        *buffer_size = 0;

        switch (c->type) {

#nếu HAVE_XZ
        case COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                trong_khi (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        nếu (r < 0)
                                trả_về r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        nếu (lzr != LZMA_OK)
                                trả_về -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                break;
#endif

#nếu HAVE_LZ4
        case COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                nếu (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        nếu (r < 0)
                                trả_về r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                nếu (r < 0)
                        trả_về r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, NULL);
                nếu (sym_LZ4F_isError(n))
                        trả_về -EIO;

                *buffer_size += n;
                break;
        }
#endif

#nếu HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src = data,
                        .size = size,
                };

                trong_khi (input.pos < input.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        nếu (r < 0)
                                trả_về r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_continue);
                        nếu (sym_ZSTD_isError(res))
                                trả_về -EIO;

                        *buffer_size += output.pos;
                }

                break;
        }
#endif

#nếu HAVE_ZLIB
        case COMPRESSION_GZIP:
                nếu (size > UINT_MAX)
                        trả_về -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                trong_khi (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        nếu (r < 0)
                                trả_về r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        nếu (r != Z_OK)
                                trả_về -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                break;
#endif

#nếu HAVE_BZIP2
        case COMPRESSION_BZIP2:
                nếu (size > UINT_MAX)
                        trả_về -EFBIG;

                c->bzip2.next_in = (void*) data;
                c->bzip2.avail_in = size;

                trong_khi (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        nếu (r < 0)
                                trả_về r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        nếu (r != BZ_RUN_OK)
                                trả_về -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                break;
#endif

        case COMPRESSION_NONE:

                nếu (*buffer_allocated < size) {
                        void *p;

                        p = realloc(*buffer, size);
                        nếu (!p)
                                trả_về -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                break;

        default:
                trả_về -EOPNOTSUPP;
        }

        trả_về 0;
}

số_nguyên compressor_finish(Compressor *c, void **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#nếu HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        số_nguyên r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        nếu (!c->encoding)
                trả_về -EINVAL;

        *buffer_size = 0;

        switch (c->type) {

#nếu HAVE_XZ
        case COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        nếu (r < 0)
                                trả_về r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        nếu (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                trả_về -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } trong_khi (lzr != LZMA_STREAM_END);

                break;
        }
#endif

#nếu HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                nếu (r < 0)
                        trả_về r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, NULL);
                nếu (sym_LZ4F_isError(n))
                        trả_về -EIO;

                *buffer_size = n;
                break;
        }
#endif

#nếu HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {};
                size_t res;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        nếu (r < 0)
                                trả_về r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_end);
                        nếu (sym_ZSTD_isError(res))
                                trả_về -EIO;

                        *buffer_size += output.pos;
                } trong_khi (res != 0);

                break;
        }
#endif

#nếu HAVE_ZLIB
        case COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        nếu (r < 0)
                                trả_về r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        nếu (!IN_SET(r, Z_OK, Z_STREAM_END))
                                trả_về -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } trong_khi (r != Z_STREAM_END);

                break;
#endif

#nếu HAVE_BZIP2
        case COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        nếu (r < 0)
                                trả_về r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        nếu (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                trả_về -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } trong_khi (r != BZ_STREAM_END);

                break;
#endif

        case COMPRESSION_NONE:
                break;

        default:
                trả_về -EOPNOTSUPP;
        }

        trả_về 0;
}
