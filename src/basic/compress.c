/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#ако HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#include <lzma.h>
#endif

#ако HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#include <lz4.h>
#include <lz4frame.h>
#include <lz4hc.h>
#endif

#ако HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#include <zstd.h>
#include <zstd_errors.h>
#endif

#ако HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#include <zlib.h>
#endif

#ако HAVE_BZIP2
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

#ако HAVE_XZ
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

#ако HAVE_LZ4
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

#ако HAVE_ZSTD
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

static цяло zstd_ret_to_errno(size_t ret) {
        switch (sym_ZSTD_getErrorCode(ret)) {
        case ZSTD_error_dstSize_tooSmall:
                върни -ENOBUFS;
        case ZSTD_error_memory_allocation:
                върни -ENOMEM;
        default:
                върни -EBADMSG;
        }
}
#endif

#ако HAVE_ZLIB
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

#ако HAVE_BZIP2
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
#ако HAVE_XZ
                lzma_stream xz;
#endif
#ако HAVE_LZ4
                struct {
                        LZ4F_compressionContext_t c_lz4;
                        void *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#ако HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#ако HAVE_ZLIB
                z_stream gzip;
#endif
#ако HAVE_BZIP2
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
        ако (c >= 0)
                върни c;

        върни compression_uppercase_from_string(s);
}

Compression compression_from_filename(const char *filename) {
        Compression c;
        const char *e;

        assert(filename);

        e = strrchr(filename, '.');
        ако (!e)
                върни COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        ако (c < 0)
                върни COMPRESSION_NONE;

        върни c;
}

bool compression_supported_journal(Compression c) {
        static const unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        assert(c >= 0);
        assert(c < _COMPRESSION_MAX);

        върни BIT_SET(supported, c);
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

        върни BIT_SET(supported, c);
}

Compression compression_detect_from_magic(const uint8_t data[static COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        ако (memcmp(data, (const uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                върни COMPRESSION_GZIP;
        ако (memcmp(data, (const uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                върни COMPRESSION_XZ;
        ако (memcmp(data, (const uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                върни COMPRESSION_ZSTD;
        ако (memcmp(data, (const uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                върни COMPRESSION_LZ4;
        ако (memcmp(data, (const uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                върни COMPRESSION_BZIP2;

        върни _COMPRESSION_INVALID;
}

цяло dlopen_xz(цяло log_level) {
#ако HAVE_XZ
        static void *lzma_dl = NULL;

        LIBLZMA_NOTE(suggested);

        върни dlopen_many_sym_or_warn(
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
        върни log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

цяло dlopen_lz4(цяло log_level) {
#ако HAVE_LZ4
        static void *lz4_dl = NULL;

        LIBLZ4_NOTE(suggested);

        върни dlopen_many_sym_or_warn(
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
        върни log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

цяло dlopen_zstd(цяло log_level) {
#ако HAVE_ZSTD
        static void *zstd_dl = NULL;

        LIBZSTD_NOTE(suggested);

        върни dlopen_many_sym_or_warn(
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
        върни log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

цяло dlopen_zlib(цяло log_level) {
#ако HAVE_ZLIB
        static void *zlib_dl = NULL;

        LIBZ_NOTE(suggested);

        върни dlopen_many_sym_or_warn(
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
        върни log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

цяло dlopen_bzip2(цяло log_level) {
#ако HAVE_BZIP2
        static void *bzip2_dl = NULL;

        LIBBZ2_NOTE(suggested);

        върни dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#иначе
        върни log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

цяло dlopen_compress_journal(Compression c, цяло log_level) {
        switch (c) {
        case COMPRESSION_NONE:
                върни 0;
        case COMPRESSION_XZ:
                върни dlopen_xz(log_level);
        case COMPRESSION_LZ4:
                върни dlopen_lz4(log_level);
        case COMPRESSION_ZSTD:
                върни dlopen_zstd(log_level);
        default:
                върни -EOPNOTSUPP;
        }
}

цяло dlopen_compress(Compression c, цяло log_level) {
        switch (c) {
        case COMPRESSION_GZIP:
                върни dlopen_zlib(log_level);
        case COMPRESSION_BZIP2:
                върни dlopen_bzip2(log_level);
        default:
                върни dlopen_compress_journal(c, log_level);
        }
}

static цяло compress_blob_xz(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                цяло level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#ако HAVE_XZ
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
        цяло r;

        r = dlopen_xz(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                ако (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        върни -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ако (src_size < 80)
                върни -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ NULL,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        ако (ret != LZMA_OK)
                върни -ENOBUFS;

        *dst_size = out_pos;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло compress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                цяло level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#ако HAVE_LZ4
        цяло r;

        r = dlopen_lz4(LOG_DEBUG);
        ако (r < 0)
                върни r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ако (src_size < 9)
                върни -ENOBUFS;

        ако (src_size > INT_MAX)
                върни -EFBIG;
        ако (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        ако (level <= 0)
                r = sym_LZ4_compress_default(src, (char*)dst + 8, src_size, (цяло) dst_alloc_size - 8);
        иначе
                r = sym_LZ4_compress_HC(src, (char*)dst + 8, src_size, (цяло) dst_alloc_size - 8, level);
        ако (r <= 0)
                върни -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло compress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                цяло level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#ако HAVE_ZSTD
        size_t k;
        цяло r;

        r = dlopen_zstd(LOG_DEBUG);
        ако (r < 0)
                върни r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        ако (sym_ZSTD_isError(k))
                върни zstd_ret_to_errno(k);

        *dst_size = k;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло compress_blob_gzip(const void *src, uint64_t src_size,
                       void *dst, size_t dst_alloc_size, size_t *dst_size, цяло level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#ако HAVE_ZLIB
        цяло r;

        r = dlopen_zlib(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size > UINT_MAX)
                върни -EFBIG;
        ако (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (цяло) sizeof(s));
        ако (r != Z_OK)
                върни -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        ако (r != Z_STREAM_END)
                върни -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло compress_blob_bzip2(
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, цяло level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#ако HAVE_BZIP2
        цяло r;

        r = dlopen_bzip2(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size > UINT_MAX)
                върни -EFBIG;
        ако (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        ако (r != BZ_OK)
                върни -ENOMEM;

        s.next_in = (char*) src;
        s.avail_in = src_size;
        s.next_out = (char*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        ако (r != BZ_STREAM_END)
                върни -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

цяло compress_blob_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, цяло level) {

        switch (compression) {
        case COMPRESSION_XZ:
                върни compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_LZ4:
                върни compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_ZSTD:
                върни compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                върни -EOPNOTSUPP;
        }
}

цяло compress_blob(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, цяло level) {

        switch (compression) {
        case COMPRESSION_GZIP:
                върни compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        case COMPRESSION_BZIP2:
                върни compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                върни compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

static цяло decompress_blob_xz(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#ако HAVE_XZ
        цяло r;

        r = dlopen_xz(LOG_DEBUG);
        ако (r < 0)
                върни r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ако (ret != LZMA_OK)
                върни -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        ако (!greedy_realloc(dst, space, 1))
                върни -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        за (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                ако (ret == LZMA_STREAM_END)
                        break;
                ако (ret != LZMA_OK)
                        върни -ENOMEM;

                ако (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                ако (dst_max > 0 && space == dst_max)
                        върни -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                assert(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                ако (!greedy_realloc(dst, space, 1))
                        върни -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло decompress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#ако HAVE_LZ4
        char* out;
        цяло r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size <= 8)
                върни -EBADMSG;

        ако (src_size - 8 > INT_MAX)
                върни -EFBIG;

        size = unaligned_read_le64(src);
        ако (size < 0 || (unsigned) size != unaligned_read_le64(src))
                върни -EFBIG;
        ако (dst_max > 0 && (size_t) size > dst_max)
                върни -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        ако (!out)
                върни -ENOMEM;

        r = sym_LZ4_decompress_safe((char*)src + 8, out, src_size - 8, size);
        ако (r < 0 || r != size)
                върни -EBADMSG;

        *dst_size = size;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(void) {
#ако HAVE_ZSTD
        ако (dlopen_zstd(LOG_DEBUG) < 0)
                върни 0;

        върни sym_ZSTD_DStreamOutSize();
#иначе
        върни 0;
#endif
}

static цяло decompress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#ако HAVE_ZSTD
        цяло r;

        r = dlopen_zstd(LOG_DEBUG);
        ако (r < 0)
                върни r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        ако (size == ZSTD_CONTENTSIZE_ERROR)
                върни -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        ако (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        иначе {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        ако (!greedy_realloc(dst, space, 1))
                върни -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ако (!dctx)
                върни -ENOMEM;

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

        за (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &input);
                ако (sym_ZSTD_isError(k))
                        върни log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                ако (output.pos >= cap)
                        break;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                ако (k == 0 && input.pos >= input.size)
                        break;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                ако (output.pos == output.size) {
                        ако (space > SIZE_MAX / 2)
                                върни -E2BIG;
                        space = MIN(2 * space, cap);
                        ако (!greedy_realloc(dst, space, 1))
                                върни -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        continue;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                ако (input.pos >= input.size)
                        върни log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло decompress_blob_gzip(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#ако HAVE_ZLIB
        цяло r;

        r = dlopen_zlib(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size > UINT_MAX)
                върни -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (цяло) sizeof(s));
        ако (r != Z_OK)
                върни -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ако (!greedy_realloc(dst, space, 1))
                върни -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        за (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                ако (r == Z_STREAM_END)
                        break;
                ако (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        върни -EBADMSG;

                ако (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                ако (dst_max > 0 && space == dst_max)
                        върни -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                ако (!greedy_realloc(dst, space, 1))
                        върни -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло decompress_blob_bzip2(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#ако HAVE_BZIP2
        цяло r;

        r = dlopen_bzip2(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size > UINT_MAX)
                върни -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        ако (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                върни -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ако (!greedy_realloc(dst, space, 1))
                върни -ENOMEM;

        s.next_in = (char*) src;
        s.avail_in = src_size;
        s.next_out = (char*) *dst;
        s.avail_out = space;

        за (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                ако (r == BZ_STREAM_END)
                        break;
                ако (r != BZ_OK)
                        върни -EBADMSG;

                ако (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        break;
                ако (dst_max > 0 && space == dst_max)
                        върни -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                ако (!greedy_realloc(dst, space, 1))
                        върни -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (char*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

цяло decompress_blob_journal(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        case COMPRESSION_XZ:
                върни decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_LZ4:
                върни decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_ZSTD:
                върни decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                върни -EPROTONOSUPPORT;
        }
}

цяло decompress_blob(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        case COMPRESSION_GZIP:
                върни decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        case COMPRESSION_BZIP2:
                върни decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                върни decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

цяло decompress_zlib_raw(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_size,
                цяло wbits) {

#ако HAVE_ZLIB
        цяло r;

        r = dlopen_zlib(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size > UINT_MAX)
                върни -EFBIG;
        ако (dst_size > UINT_MAX)
                върни -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (void*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (цяло) sizeof(s));
        ако (r != Z_OK)
                върни -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        ако (r != Z_STREAM_END || produced != dst_size)
                върни -EBADMSG;

        върни 0;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло decompress_startswith_xz(
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

#ако HAVE_XZ
        цяло r;

        r = dlopen_xz(LOG_DEBUG);
        ако (r < 0)
                върни r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ако (ret != LZMA_OK)
                върни -EBADMSG;

        ако (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                върни -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        за (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                ако (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        върни -EBADMSG;

                ако (allocated - s.avail_out >= prefix_len + 1)
                        върни memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                ако (ret == LZMA_STREAM_END)
                        върни 0;

                s.avail_out += allocated;

                ако (!(greedy_realloc(buffer, allocated * 2, 1)))
                        върни -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло decompress_startswith_lz4(
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

#ако HAVE_LZ4
        size_t allocated;
        цяло r;

        r = dlopen_lz4(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size <= 8)
                върни -EBADMSG;

        ако (src_size - 8 > INT_MAX)
                върни -EFBIG;

        ако (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                върни -ENOMEM;
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
        ако (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                ако (sym_LZ4_versionNumber() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        върни 0;

                ако (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        ако (memcmp(*buffer, prefix, r) != 0)
                                върни 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                ако (r < 0)
                        върни r;

                ако (size < prefix_len + 1)
                        върни 0;
        }

        върни memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло decompress_startswith_zstd(
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

#ако HAVE_ZSTD
        цяло r;

        r = dlopen_zstd(LOG_DEBUG);
        ако (r < 0)
                върни r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        ако (size == ZSTD_CONTENTSIZE_ERROR)
                върни -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        ако (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                върни 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ако (!dctx)
                върни -ENOMEM;

        ако (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                върни -ENOMEM;

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
        ако (sym_ZSTD_isError(k))
                върни log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        ако (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                ако (k != 0)
                        върни log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                върни 0; /* Decompressed text too short to match the prefix and extra */
        }

        върни memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло decompress_startswith_gzip(
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

#ако HAVE_ZLIB
        цяло r;

        r = dlopen_zlib(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size > UINT_MAX)
                върни -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (цяло) sizeof(s));
        ако (r != Z_OK)
                върни -EBADMSG;

        ако (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                върни -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (void*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        за (;;) {
                r = sym_inflate(&s, Z_FINISH);

                ако (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        върни -EBADMSG;

                ако (allocated - s.avail_out >= prefix_len + 1)
                        върни memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                ако (r == Z_STREAM_END)
                        върни 0;

                ако (s.avail_out > 0)
                        върни -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                ако (!(greedy_realloc(buffer, allocated * 2, 1)))
                        върни -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

static цяло decompress_startswith_bzip2(
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

#ако HAVE_BZIP2
        цяло r;

        r = dlopen_bzip2(LOG_DEBUG);
        ако (r < 0)
                върни r;

        ако (src_size > UINT_MAX)
                върни -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        ако (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                върни -EIO;

        ако (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                върни -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (char*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        за (;;) {
                r = sym_BZ2_bzDecompress(&s);

                ако (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        върни -EBADMSG;

                ако (allocated - s.avail_out >= prefix_len + 1)
                        върни memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                ако (r == BZ_STREAM_END)
                        върни 0;

                ако (s.avail_out > 0)
                        върни -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                ако (!(greedy_realloc(buffer, allocated * 2, 1)))
                        върни -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (char*) *buffer + used;
        }
#иначе
        върни -EPROTONOSUPPORT;
#endif
}

цяло decompress_startswith_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        case COMPRESSION_XZ:
                върни decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_LZ4:
                върни decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_ZSTD:
                върни decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                върни -EOPNOTSUPP;
        }
}

цяло decompress_startswith(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        case COMPRESSION_GZIP:
                върни decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        case COMPRESSION_BZIP2:
                върни decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                върни decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

цяло compress_stream(
                Compression type,
                цяло fdf, цяло fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = NULL;
        _cleanup_free_ void *buf = NULL;
        _cleanup_free_ uint8_t *input = NULL;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        цяло r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        r = compressor_new(&c, type);
        ако (r < 0)
                върни r;

        input = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        ако (!input)
                върни -ENOMEM;

        за (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                ако (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = read(fdf, input, m);
                ако (n < 0)
                        върни -errno;

                ако (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        ако (r < 0)
                                върни r;

                        ако (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                ако (r < 0)
                                        върни r;
                                total_out += buf_size;
                        }
                        break;
                }

                total_in += n;
                ако (max_bytes != UINT64_MAX) {
                        assert(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, input, n, &buf, &buf_size, &buf_alloc);
                ако (r < 0)
                        върни r;

                ако (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        ако (r < 0)
                                върни r;
                        total_out += buf_size;
                }
        }

        ако (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        ако (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        иначе
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (double) total_out / total_in * 100);

        върни 0;
}

static цяло should_sparse(цяло fd) {
        struct stat st;

        assert(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        ако (fstat(fd, &st) < 0)
                върни -errno;

        ако (!S_ISREG(st.st_mode))
                върни false;

        цяло flags = fcntl(fd, F_GETFL);
        ако (flags < 0)
                върни -errno;

        ако (FLAGS_SET(flags, O_APPEND))
                върни false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        ако (pos < 0)
                върни -errno;

        върни pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
static цяло finalize_sparse(цяло fd) {
        off_t pos;

        assert(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        ако (pos < 0)
                върни -errno;

        ако (ftruncate(fd, pos) < 0)
                върни -errno;

        върни 0;
}

/* Common helper for decompress_stream_*() wrappers */

struct decompress_stream_userdata {
        цяло fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

static цяло decompress_stream_write_callback(const void *data, size_t size, void *userdata) {
        struct decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        ако (u->max_bytes != UINT64_MAX) {
                ако (u->max_bytes < size)
                        върни -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        ако (u->skip > 0) {
                ако (u->skip >= size) {
                        u->skip -= size;
                        върни 0;
                }

                data = (const uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        ако (size == 0)
                върни 0;

        ако (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                ако (k < 0)
                        върни (цяло) k;
                върни 0;
        }

        върни loop_write(u->fd, data, size);
}

static цяло decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        assert(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        ако (!c)
                върни -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        switch (type) {
        case COMPRESSION_NONE:
                break;

#ако HAVE_XZ
        case COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                ако (xz_ret != LZMA_OK)
                        върни -EIO;
                break;
        }
#endif

#ако HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                ако (sym_LZ4F_isError(rc))
                        върни -ENOMEM;

                break;
        }
#endif

#ако HAVE_ZSTD
        case COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                ако (!c->d_zstd)
                        върни -ENOMEM;
                break;
#endif

#ако HAVE_ZLIB
        case COMPRESSION_GZIP:
                ако (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (цяло) sizeof(c->gzip)) != Z_OK)
                        върни -EIO;
                break;
#endif

#ако HAVE_BZIP2
        case COMPRESSION_BZIP2:
                ако (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        върни -EIO;
                break;
#endif

        default:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        върни 0;
}

static цяло decompress_blob_to_fd_impl(
                Compression type,
                const void *src,
                uint64_t src_size,
                цяло fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        цяло r;

        assert(src);
        assert(fdt >= 0);

        ако (type == COMPRESSION_NONE)
                върни -EOPNOTSUPP;

        ако (src_size == 0)
                върни -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        ако (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                ако (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        ако (src_size <= sizeof(uint64_t))
                                върни -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        ако (sz < skip)
                                върни -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                ако (max_bytes == 0)
                        върни -ENOBUFS;

                _cleanup_free_ void *buf = NULL;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                ако (r < 0)
                        върни r;

                /* Check the result size again for safety. */
                ако (size < skip)
                        върни -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                ако (r < 0)
                        върни r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (double) size / src_size * 100);

                ако (ret_size)
                        *ret_size = size - skip;
                върни 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        ако (r < 0)
                върни r;

        struct decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_push(c, src, src_size, decompress_stream_write_callback, &userdata);
        ако (r < 0)
                върни r;

        /* Finalize */
        r = decompressor_push(c, /* data= */ NULL, /* size= */ 0, decompress_stream_write_callback, &userdata);
        ако (r < 0)
                върни r;

        ако (userdata.total_out < skip)
                върни -EBADMSG;

        ако (userdata.sparse) {
                r = finalize_sparse(fdt);
                ако (r < 0)
                        върни r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (double) userdata.total_out / src_size * 100);

        ако (ret_size)
                *ret_size = userdata.total_out - skip;

        върни 0;
}

цяло decompress_blob_to_fd_journal(
                Compression type,
                const void *src,
                uint64_t src_size,
                цяло fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        цяло r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        ако (r < 0)
                върни r;

        върни decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

цяло decompress_blob_to_fd(
                Compression type,
                const void *src,
                uint64_t src_size,
                цяло fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        цяло r;

        r = dlopen_compress(type, LOG_DEBUG);
        ако (r < 0)
                върни r;

        върни decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

цяло decompress_stream_full(
                Compression type,
                цяло fdf, цяло fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        _cleanup_free_ uint8_t *buf = NULL;
        uint64_t total_in = 0;
        цяло r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        ако (type == COMPRESSION_NONE)
                върни -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        ако (r < 0)
                върни r;

        r = decompressor_new(&c, type, flags);
        ако (r < 0)
                върни r;

        struct decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        ако (!buf)
                върни -ENOMEM;

        за (;;) {
                ssize_t n;

                n = read(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                ако (n < 0)
                        върни -errno;

                total_in += n;

                r = decompressor_push(c, buf, n, decompress_stream_write_callback, &userdata);
                ако (r < 0)
                        върни r;

                ако (n == 0)
                        break;
        }

        ако (total_in == 0)
                върни log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        ако (userdata.sparse) {
                r = finalize_sparse(fdt);
                ако (r < 0)
                        върни r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (double) userdata.total_out / total_in * 100);

        върни 0;
}

цяло decompress_stream_by_filename(const char *filename, цяло fdf, цяло fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        ако (c == COMPRESSION_NONE)
                върни -EPROTONOSUPPORT;

        върни decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        ако (!c)
                върни NULL;

        switch (c->type) {

#ако HAVE_XZ
        case COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                break;
#endif

#ако HAVE_LZ4
        case COMPRESSION_LZ4:
                ако (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = NULL;
                        c->lz4_header = mfree(c->lz4_header);
                } иначе {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = NULL;
                }
                break;
#endif

#ако HAVE_ZSTD
        case COMPRESSION_ZSTD:
                ако (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = NULL;
                } иначе {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = NULL;
                }
                break;
#endif

#ако HAVE_ZLIB
        case COMPRESSION_GZIP:
                ако (c->encoding)
                        sym_deflateEnd(&c->gzip);
                иначе
                        sym_inflateEnd(&c->gzip);
                break;
#endif

#ако HAVE_BZIP2
        case COMPRESSION_BZIP2:
                ако (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                иначе
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                break;
#endif

        default:
                break;
        }

        върни mfree(c);
}

Compression compressor_type(const Compressor *c) {
        върни c ? c->type : _COMPRESSION_INVALID;
}

цяло decompressor_detect(Decompressor **decompressor, const void *data, size_t size) {
        цяло r;

        assert(decompressor);

        ако (*decompressor)
                върни 1;

        ако (size < COMPRESSION_MAGIC_BYTES_MAX)
                върни 0;

        assert(data);

        Compression type = compression_detect_from_magic(data);
        ако (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        ако (r < 0)
                върни r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        ако (r < 0)
                върни r;

        върни 1;
}

цяло decompressor_force(Decompressor **decompressor, Compression type) {
        цяло r;

        assert(decompressor);
        assert(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        ако (r < 0)
                върни r;

        *decompressor = compressor_free(*decompressor);
        върни decompressor_new(decompressor, type, /* flags= */ 0);
}

цяло decompressor_force_off(Decompressor **decompressor) {
        върни decompressor_force(decompressor, COMPRESSION_NONE);
}

цяло decompressor_push(Decompressor *c, const void *data, size_t size, DecompressorCallback callback, void *userdata) {
        цяло r;

        assert(c);
        assert(data || size == 0);
        assert(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        ако (c->encoding)
                върни -EINVAL;

        ако (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                върни 0;

#ако HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = NULL;
        ако (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                ако (!buffer)
                        върни -ENOMEM;
        }
#endif

        switch (c->type) {

        case COMPRESSION_NONE:
                ако (c->finished)
                        върни -EBADMSG;

                ако (size == 0) {
                        c->finished = true;
                        върни 0;
                }

                r = callback(data, size, userdata);
                ако (r < 0)
                        върни r;

                върни 0;

#ако HAVE_XZ
        case COMPRESSION_XZ:
                ако (c->finished)
                        върни -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                за (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        ако (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                върни -EBADMSG;

                        ако (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                върни -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        ако (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ако (r < 0)
                                        върни r;
                        }

                        ако (lzr == LZMA_STREAM_END) {
                                ако (c->xz.avail_in > 0)
                                        върни -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                върни 0;
                        }

                        ако (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                ако (size == 0)
                                        върни -EBADMSG; /* truncated */

                                върни 0; /* No progress possible with the current input. */
                        }
                }
#endif

#ако HAVE_LZ4
        case COMPRESSION_LZ4: {
                const uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                за (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, NULL);
                        ако (sym_LZ4F_isError(rc))
                                върни -EBADMSG;

                        ако (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                върни -EIO;

                        ако (consumed > 0) {
                                assert(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        ако (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ако (r < 0)
                                        върни r;
                        }

                        ако (src_remaining == 0) {
                                ако (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        върни 0;
                                }

                                ако (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                ако (size == 0)
                                        върни -EBADMSG; /* truncated */

                                върни 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#ако HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src =  (void*) data,
                        .size = size,
                };

                c->finished = false;

                за (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &input);
                        ако (sym_ZSTD_isError(res))
                                върни -EBADMSG;

                        ако (input.pos > input.size ||
                            output.pos > output.size)
                                върни -EIO;

                        ако (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                ако (r < 0)
                                        върни r;
                        }

                        ако (input.pos == input.size) {
                                ако (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        върни 0;
                                }

                                ако (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                ако (size == 0)
                                        върни -EBADMSG; /* truncated */

                                върни 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#ако HAVE_ZLIB
        case COMPRESSION_GZIP:
                ако (size > UINT_MAX)
                        върни -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                за (;;) {
                        ако (c->finished) {
                                ако (sym_inflateReset(&c->gzip) != Z_OK)
                                        върни -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        цяло zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        ако (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                върни -EBADMSG;

                        ако (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                върни -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        ако (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ако (r < 0)
                                        върни r;
                        }

                        ако (zr == Z_STREAM_END) {
                                c->finished = true;

                                ако (c->gzip.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                върни 0;
                        }

                        ако (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                ако (size == 0)
                                        върни -EBADMSG; /* truncated */

                                върни 0; /* No progress possible with the current input. */
                        }
                }
#endif

#ако HAVE_BZIP2
        case COMPRESSION_BZIP2:
                ако (size > UINT_MAX)
                        върни -EFBIG;

                c->bzip2.next_in = (char*) data;
                c->bzip2.avail_in = size;

                за (;;) {
                        ако (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                ако (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        върни -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (char*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        цяло bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        ако (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                върни -EBADMSG;

                        ако (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                върни -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        ако (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ако (r < 0)
                                        върни r;
                        }

                        ако (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                ако (c->bzip2.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                върни 0;
                        }

                        ако (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                ако (size == 0)
                                        върни -EBADMSG; /* truncated */

                                върни 0; /* No progress possible with the current input. */
                        }
                }
#endif

        default:
                assert_not_reached();
        }
}

цяло compressor_new(Compressor **ret, Compression type) {
        цяло r;

        assert(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        ако (r < 0)
                върни r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        ако (!c)
                върни -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        switch (type) {

#ако HAVE_XZ
        case COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                ако (xzr != LZMA_OK)
                        върни -EIO;

                c->type = COMPRESSION_XZ;
                break;
        }
#endif

#ако HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                ако (sym_LZ4F_isError(rc))
                        върни -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                ако (!c->lz4_header)
                        върни -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                ако (sym_LZ4F_isError(c->lz4_header_size))
                        върни -EINVAL;

                break;
        }
#endif

#ако HAVE_ZSTD
        case COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                ако (!c->c_zstd)
                        върни -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                ако (sym_ZSTD_isError(z))
                        върни -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                ако (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                break;
#endif

#ако HAVE_ZLIB
        case COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (цяло) sizeof(c->gzip));
                ако (r != Z_OK)
                        върни -EIO;

                c->type = COMPRESSION_GZIP;
                break;
#endif

#ако HAVE_BZIP2
        case COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                ако (r != BZ_OK)
                        върни -EIO;

                c->type = COMPRESSION_BZIP2;
                break;
#endif

        case COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                break;

        default:
                върни -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        върни 0;
}

#ако HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
static цяло enlarge_buffer(void **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        ако (*buffer_allocated >= need)
                върни 0;

        ако (!greedy_realloc(buffer, need, 1))
                върни -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        върни 1;
}
#endif

цяло compressor_start(
                Compressor *c,
                const void *data,
                size_t size,
                void **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#ако HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        цяло r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        ако (!c->encoding)
                върни -EINVAL;

        ако (size == 0)
                върни 0;

        assert(data);

        *buffer_size = 0;

        switch (c->type) {

#ако HAVE_XZ
        case COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                докато (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ако (r < 0)
                                върни r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        ако (lzr != LZMA_OK)
                                върни -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                break;
#endif

#ако HAVE_LZ4
        case COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                ако (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        ако (r < 0)
                                върни r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                ако (r < 0)
                        върни r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, NULL);
                ако (sym_LZ4F_isError(n))
                        върни -EIO;

                *buffer_size += n;
                break;
        }
#endif

#ако HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src = data,
                        .size = size,
                };

                докато (input.pos < input.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ако (r < 0)
                                върни r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_continue);
                        ако (sym_ZSTD_isError(res))
                                върни -EIO;

                        *buffer_size += output.pos;
                }

                break;
        }
#endif

#ако HAVE_ZLIB
        case COMPRESSION_GZIP:
                ако (size > UINT_MAX)
                        върни -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                докато (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ако (r < 0)
                                върни r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        ако (r != Z_OK)
                                върни -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                break;
#endif

#ако HAVE_BZIP2
        case COMPRESSION_BZIP2:
                ако (size > UINT_MAX)
                        върни -EFBIG;

                c->bzip2.next_in = (void*) data;
                c->bzip2.avail_in = size;

                докато (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ако (r < 0)
                                върни r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        ако (r != BZ_RUN_OK)
                                върни -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                break;
#endif

        case COMPRESSION_NONE:

                ако (*buffer_allocated < size) {
                        void *p;

                        p = realloc(*buffer, size);
                        ако (!p)
                                върни -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                break;

        default:
                върни -EOPNOTSUPP;
        }

        върни 0;
}

цяло compressor_finish(Compressor *c, void **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#ако HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        цяло r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        ако (!c->encoding)
                върни -EINVAL;

        *buffer_size = 0;

        switch (c->type) {

#ако HAVE_XZ
        case COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ако (r < 0)
                                върни r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        ако (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                върни -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } докато (lzr != LZMA_STREAM_END);

                break;
        }
#endif

#ако HAVE_LZ4
        case COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                ако (r < 0)
                        върни r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, NULL);
                ако (sym_LZ4F_isError(n))
                        върни -EIO;

                *buffer_size = n;
                break;
        }
#endif

#ако HAVE_ZSTD
        case COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {};
                size_t res;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ако (r < 0)
                                върни r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_end);
                        ако (sym_ZSTD_isError(res))
                                върни -EIO;

                        *buffer_size += output.pos;
                } докато (res != 0);

                break;
        }
#endif

#ако HAVE_ZLIB
        case COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ако (r < 0)
                                върни r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        ако (!IN_SET(r, Z_OK, Z_STREAM_END))
                                върни -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } докато (r != Z_STREAM_END);

                break;
#endif

#ако HAVE_BZIP2
        case COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ако (r < 0)
                                върни r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        ако (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                върни -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } докато (r != BZ_STREAM_END);

                break;
#endif

        case COMPRESSION_NONE:
                break;

        default:
                върни -EOPNOTSUPP;
        }

        върни 0;
}
