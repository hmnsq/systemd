/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#အကယ်၍ HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#include <lzma.h>
#endif

#အကယ်၍ HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#include <lz4.h>
#include <lz4frame.h>
#include <lz4hc.h>
#endif

#အကယ်၍ HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#include <zstd.h>
#include <zstd_errors.h>
#endif

#အကယ်၍ HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#include <zlib.h>
#endif

#အကယ်၍ HAVE_BZIP2
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

#အကယ်၍ HAVE_XZ
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
static anta_hstha void lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#အကယ်၍ HAVE_LZ4
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

#အကယ်၍ HAVE_ZSTD
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

static ကိန်းပြည့် zstd_ret_to_errno(size_t ret) {
        switch (sym_ZSTD_getErrorCode(ret)) {
        pak_sa ZSTD_error_dstSize_tooSmall:
                ပြန်ပေး -ENOBUFS;
        pak_sa ZSTD_error_memory_allocation:
                ပြန်ပေး -ENOMEM;
        default:
                ပြန်ပေး -EBADMSG;
        }
}
#endif

#အကယ်၍ HAVE_ZLIB
static DLSYM_PROTOTYPE(deflateInit2_) = NULL;
static DLSYM_PROTOTYPE(deflate) = NULL;
static DLSYM_PROTOTYPE(deflateEnd) = NULL;
static DLSYM_PROTOTYPE(inflateInit2_) = NULL;
static DLSYM_PROTOTYPE(inflate) = NULL;
static DLSYM_PROTOTYPE(inflateEnd) = NULL;
static DLSYM_PROTOTYPE(inflateReset) = NULL;

static anta_hstha void deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

static anta_hstha void inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#အကယ်၍ HAVE_BZIP2
static DLSYM_PROTOTYPE(BZ2_bzCompressInit) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzCompress) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzDecompress) = NULL;
static DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = NULL;

static anta_hstha void BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

static anta_hstha void BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
sa_nracanaa Compressor {
        Compression type;
        bool encoding;
        bool finished;
        sa_ngha {
#အကယ်၍ HAVE_XZ
                lzma_stream xz;
#endif
#အကယ်၍ HAVE_LZ4
                sa_nracanaa {
                        LZ4F_compressionContext_t c_lz4;
                        void *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#အကယ်၍ HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#အကယ်၍ HAVE_ZLIB
                z_stream gzip;
#endif
#အကယ်၍ HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, parimaa_na(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

static const var_na* const compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

static const var_na* const compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

static const var_na* const compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(const var_na *s) {
        Compression c;

        assert(s);

        c = compression_from_string(s);
        အကယ်၍ (c >= 0)
                ပြန်ပေး c;

        ပြန်ပေး compression_uppercase_from_string(s);
}

Compression compression_from_filename(const var_na *filename) {
        Compression c;
        const var_na *e;

        assert(filename);

        e = strrchr(filename, '.');
        အကယ်၍ (!e)
                ပြန်ပေး COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        အကယ်၍ (c < 0)
                ပြန်ပေး COMPRESSION_NONE;

        ပြန်ပေး c;
}

bool compression_supported_journal(Compression c) {
        static const unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        assert(c >= 0);
        assert(c < _COMPRESSION_MAX);

        ပြန်ပေး BIT_SET(supported, c);
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

        ပြန်ပေး BIT_SET(supported, c);
}

Compression compression_detect_from_magic(const uint8_t data[static COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        အကယ်၍ (memcmp(data, (const uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                ပြန်ပေး COMPRESSION_GZIP;
        အကယ်၍ (memcmp(data, (const uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                ပြန်ပေး COMPRESSION_XZ;
        အကယ်၍ (memcmp(data, (const uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                ပြန်ပေး COMPRESSION_ZSTD;
        အကယ်၍ (memcmp(data, (const uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                ပြန်ပေး COMPRESSION_LZ4;
        အကယ်၍ (memcmp(data, (const uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                ပြန်ပေး COMPRESSION_BZIP2;

        ပြန်ပေး _COMPRESSION_INVALID;
}

ကိန်းပြည့် dlopen_xz(ကိန်းပြည့် log_level) {
#အကယ်၍ HAVE_XZ
        static void *lzma_dl = NULL;

        LIBLZMA_NOTE(suggested);

        ပြန်ပေး dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#မဟုတ်ပါက
        ပြန်ပေး log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

ကိန်းပြည့် dlopen_lz4(ကိန်းပြည့် log_level) {
#အကယ်၍ HAVE_LZ4
        static void *lz4_dl = NULL;

        LIBLZ4_NOTE(suggested);

        ပြန်ပေး dlopen_many_sym_or_warn(
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
#မဟုတ်ပါက
        ပြန်ပေး log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

ကိန်းပြည့် dlopen_zstd(ကိန်းပြည့် log_level) {
#အကယ်၍ HAVE_ZSTD
        static void *zstd_dl = NULL;

        LIBZSTD_NOTE(suggested);

        ပြန်ပေး dlopen_many_sym_or_warn(
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
#မဟုတ်ပါက
        ပြန်ပေး log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

ကိန်းပြည့် dlopen_zlib(ကိန်းပြည့် log_level) {
#အကယ်၍ HAVE_ZLIB
        static void *zlib_dl = NULL;

        LIBZ_NOTE(suggested);

        ပြန်ပေး dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#မဟုတ်ပါက
        ပြန်ပေး log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

ကိန်းပြည့် dlopen_bzip2(ကိန်းပြည့် log_level) {
#အကယ်၍ HAVE_BZIP2
        static void *bzip2_dl = NULL;

        LIBBZ2_NOTE(suggested);

        ပြန်ပေး dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#မဟုတ်ပါက
        ပြန်ပေး log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

ကိန်းပြည့် dlopen_compress_journal(Compression c, ကိန်းပြည့် log_level) {
        switch (c) {
        pak_sa COMPRESSION_NONE:
                ပြန်ပေး 0;
        pak_sa COMPRESSION_XZ:
                ပြန်ပေး dlopen_xz(log_level);
        pak_sa COMPRESSION_LZ4:
                ပြန်ပေး dlopen_lz4(log_level);
        pak_sa COMPRESSION_ZSTD:
                ပြန်ပေး dlopen_zstd(log_level);
        default:
                ပြန်ပေး -EOPNOTSUPP;
        }
}

ကိန်းပြည့် dlopen_compress(Compression c, ကိန်းပြည့် log_level) {
        switch (c) {
        pak_sa COMPRESSION_GZIP:
                ပြန်ပေး dlopen_zlib(log_level);
        pak_sa COMPRESSION_BZIP2:
                ပြန်ပေး dlopen_bzip2(log_level);
        default:
                ပြန်ပေး dlopen_compress_journal(c, log_level);
        }
}

static ကိန်းပြည့် compress_blob_xz(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ကိန်းပြည့် level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#အကယ်၍ HAVE_XZ
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
        ကိန်းပြည့် r;

        r = dlopen_xz(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                အကယ်၍ (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        ပြန်ပေး -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        အကယ်၍ (src_size < 80)
                ပြန်ပေး -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ NULL,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        အကယ်၍ (ret != LZMA_OK)
                ပြန်ပေး -ENOBUFS;

        *dst_size = out_pos;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် compress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ကိန်းပြည့် level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#အကယ်၍ HAVE_LZ4
        ကိန်းပြည့် r;

        r = dlopen_lz4(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        အကယ်၍ (src_size < 9)
                ပြန်ပေး -ENOBUFS;

        အကယ်၍ (src_size > INT_MAX)
                ပြန်ပေး -EFBIG;
        အကယ်၍ (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        အကယ်၍ (level <= 0)
                r = sym_LZ4_compress_default(src, (var_na*)dst + 8, src_size, (ကိန်းပြည့်) dst_alloc_size - 8);
        မဟုတ်ပါက
                r = sym_LZ4_compress_HC(src, (var_na*)dst + 8, src_size, (ကိန်းပြည့်) dst_alloc_size - 8, level);
        အကယ်၍ (r <= 0)
                ပြန်ပေး -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် compress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ကိန်းပြည့် level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#အကယ်၍ HAVE_ZSTD
        size_t k;
        ကိန်းပြည့် r;

        r = dlopen_zstd(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        အကယ်၍ (sym_ZSTD_isError(k))
                ပြန်ပေး zstd_ret_to_errno(k);

        *dst_size = k;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် compress_blob_gzip(const void *src, uint64_t src_size,
                       void *dst, size_t dst_alloc_size, size_t *dst_size, ကိန်းပြည့် level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#အကယ်၍ HAVE_ZLIB
        ကိန်းပြည့် r;

        r = dlopen_zlib(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size > UINT_MAX)
                ပြန်ပေး -EFBIG;
        အကယ်၍ (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (ကိန်းပြည့်) parimaa_na(s));
        အကယ်၍ (r != Z_OK)
                ပြန်ပေး -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        အကယ်၍ (r != Z_STREAM_END)
                ပြန်ပေး -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် compress_blob_bzip2(
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, ကိန်းပြည့် level) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_alloc_size > 0);
        assert(dst_size);

#အကယ်၍ HAVE_BZIP2
        ကိန်းပြည့် r;

        r = dlopen_bzip2(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size > UINT_MAX)
                ပြန်ပေး -EFBIG;
        အကယ်၍ (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        အကယ်၍ (r != BZ_OK)
                ပြန်ပေး -ENOMEM;

        s.next_in = (var_na*) src;
        s.avail_in = src_size;
        s.next_out = (var_na*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        အကယ်၍ (r != BZ_STREAM_END)
                ပြန်ပေး -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

ကိန်းပြည့် compress_blob_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, ကိန်းပြည့် level) {

        switch (compression) {
        pak_sa COMPRESSION_XZ:
                ပြန်ပေး compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        pak_sa COMPRESSION_LZ4:
                ပြန်ပေး compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        pak_sa COMPRESSION_ZSTD:
                ပြန်ပေး compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                ပြန်ပေး -EOPNOTSUPP;
        }
}

ကိန်းပြည့် compress_blob(
                Compression compression,
                const void *src, uint64_t src_size,
                void *dst, size_t dst_alloc_size, size_t *dst_size, ကိန်းပြည့် level) {

        switch (compression) {
        pak_sa COMPRESSION_GZIP:
                ပြန်ပေး compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        pak_sa COMPRESSION_BZIP2:
                ပြန်ပေး compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        default:
                ပြန်ပေး compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

static ကိန်းပြည့် decompress_blob_xz(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#အကယ်၍ HAVE_XZ
        ကိန်းပြည့် r;

        r = dlopen_xz(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        အကယ်၍ (ret != LZMA_OK)
                ပြန်ပေး -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        အကယ်၍ (!greedy_realloc(dst, space, 1))
                ပြန်ပေး -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        အတွက် (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                အကယ်၍ (ret == LZMA_STREAM_END)
                        bha_nga;
                အကယ်၍ (ret != LZMA_OK)
                        ပြန်ပေး -ENOMEM;

                အကယ်၍ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        bha_nga;
                အကယ်၍ (dst_max > 0 && space == dst_max)
                        ပြန်ပေး -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                assert(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                အကယ်၍ (!greedy_realloc(dst, space, 1))
                        ပြန်ပေး -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် decompress_blob_lz4(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#အကယ်၍ HAVE_LZ4
        var_na* out;
        ကိန်းပြည့် r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size <= 8)
                ပြန်ပေး -EBADMSG;

        အကယ်၍ (src_size - 8 > INT_MAX)
                ပြန်ပေး -EFBIG;

        size = unaligned_read_le64(src);
        အကယ်၍ (size < 0 || (unsigned) size != unaligned_read_le64(src))
                ပြန်ပေး -EFBIG;
        အကယ်၍ (dst_max > 0 && (size_t) size > dst_max)
                ပြန်ပေး -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        အကယ်၍ (!out)
                ပြန်ပေး -ENOMEM;

        r = sym_LZ4_decompress_safe((var_na*)src + 8, out, src_size - 8, size);
        အကယ်၍ (r < 0 || r != size)
                ပြန်ပေး -EBADMSG;

        *dst_size = size;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(void) {
#အကယ်၍ HAVE_ZSTD
        အကယ်၍ (dlopen_zstd(LOG_DEBUG) < 0)
                ပြန်ပေး 0;

        ပြန်ပေး sym_ZSTD_DStreamOutSize();
#မဟုတ်ပါက
        ပြန်ပေး 0;
#endif
}

static ကိန်းပြည့် decompress_blob_zstd(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#အကယ်၍ HAVE_ZSTD
        ကိန်းပြည့် r;

        r = dlopen_zstd(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        အကယ်၍ (size == ZSTD_CONTENTSIZE_ERROR)
                ပြန်ပေး -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        အကယ်၍ (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        မဟုတ်ပါက {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        အကယ်၍ (!greedy_realloc(dst, space, 1))
                ပြန်ပေး -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        အကယ်၍ (!dctx)
                ပြန်ပေး -ENOMEM;

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

        အတွက် (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &input);
                အကယ်၍ (sym_ZSTD_isError(k))
                        ပြန်ပေး log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                အကယ်၍ (output.pos >= cap)
                        bha_nga;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                အကယ်၍ (k == 0 && input.pos >= input.size)
                        bha_nga;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                အကယ်၍ (output.pos == output.size) {
                        အကယ်၍ (space > SIZE_MAX / 2)
                                ပြန်ပေး -E2BIG;
                        space = MIN(2 * space, cap);
                        အကယ်၍ (!greedy_realloc(dst, space, 1))
                                ပြန်ပေး -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        continue;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                အကယ်၍ (input.pos >= input.size)
                        ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် decompress_blob_gzip(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#အကယ်၍ HAVE_ZLIB
        ကိန်းပြည့် r;

        r = dlopen_zlib(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size > UINT_MAX)
                ပြန်ပေး -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ကိန်းပြည့်) parimaa_na(s));
        အကယ်၍ (r != Z_OK)
                ပြန်ပေး -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        အကယ်၍ (!greedy_realloc(dst, space, 1))
                ပြန်ပေး -ENOMEM;

        s.next_in = (void*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        အတွက် (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                အကယ်၍ (r == Z_STREAM_END)
                        bha_nga;
                အကယ်၍ (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        ပြန်ပေး -EBADMSG;

                အကယ်၍ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        bha_nga;
                အကယ်၍ (dst_max > 0 && space == dst_max)
                        ပြန်ပေး -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                အကယ်၍ (!greedy_realloc(dst, space, 1))
                        ပြန်ပေး -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် decompress_blob_bzip2(
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        assert(src);
        assert(src_size > 0);
        assert(dst);
        assert(dst_size);

#အကယ်၍ HAVE_BZIP2
        ကိန်းပြည့် r;

        r = dlopen_bzip2(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size > UINT_MAX)
                ပြန်ပေး -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        အကယ်၍ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ပြန်ပေး -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        အကယ်၍ (!greedy_realloc(dst, space, 1))
                ပြန်ပေး -ENOMEM;

        s.next_in = (var_na*) src;
        s.avail_in = src_size;
        s.next_out = (var_na*) *dst;
        s.avail_out = space;

        အတွက် (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                အကယ်၍ (r == BZ_STREAM_END)
                        bha_nga;
                အကယ်၍ (r != BZ_OK)
                        ပြန်ပေး -EBADMSG;

                အကယ်၍ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        bha_nga;
                အကယ်၍ (dst_max > 0 && space == dst_max)
                        ပြန်ပေး -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                အကယ်၍ (!greedy_realloc(dst, space, 1))
                        ပြန်ပေး -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (var_na*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

ကိန်းပြည့် decompress_blob_journal(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        pak_sa COMPRESSION_XZ:
                ပြန်ပေး decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        pak_sa COMPRESSION_LZ4:
                ပြန်ပေး decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        pak_sa COMPRESSION_ZSTD:
                ပြန်ပေး decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                ပြန်ပေး -EPROTONOSUPPORT;
        }
}

ကိန်းပြည့် decompress_blob(
                Compression compression,
                const void *src,
                uint64_t src_size,
                void **dst,
                size_t *dst_size,
                size_t dst_max) {

        switch (compression) {
        pak_sa COMPRESSION_GZIP:
                ပြန်ပေး decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        pak_sa COMPRESSION_BZIP2:
                ပြန်ပေး decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        default:
                ပြန်ပေး decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

ကိန်းပြည့် decompress_zlib_raw(
                const void *src,
                uint64_t src_size,
                void *dst,
                size_t dst_size,
                ကိန်းပြည့် wbits) {

#အကယ်၍ HAVE_ZLIB
        ကိန်းပြည့် r;

        r = dlopen_zlib(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size > UINT_MAX)
                ပြန်ပေး -EFBIG;
        အကယ်၍ (dst_size > UINT_MAX)
                ပြန်ပေး -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (void*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (ကိန်းပြည့်) parimaa_na(s));
        အကယ်၍ (r != Z_OK)
                ပြန်ပေး -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        အကယ်၍ (r != Z_STREAM_END || produced != dst_size)
                ပြန်ပေး -EBADMSG;

        ပြန်ပေး 0;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် decompress_startswith_xz(
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

#အကယ်၍ HAVE_XZ
        ကိန်းပြည့် r;

        r = dlopen_xz(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        အကယ်၍ (ret != LZMA_OK)
                ပြန်ပေး -EBADMSG;

        အကယ်၍ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ပြန်ပေး -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        အတွက် (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                အကယ်၍ (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        ပြန်ပေး -EBADMSG;

                အကယ်၍ (allocated - s.avail_out >= prefix_len + 1)
                        ပြန်ပေး memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                အကယ်၍ (ret == LZMA_STREAM_END)
                        ပြန်ပေး 0;

                s.avail_out += allocated;

                အကယ်၍ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ပြန်ပေး -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် decompress_startswith_lz4(
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

#အကယ်၍ HAVE_LZ4
        size_t allocated;
        ကိန်းပြည့် r;

        r = dlopen_lz4(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size <= 8)
                ပြန်ပေး -EBADMSG;

        အကယ်၍ (src_size - 8 > INT_MAX)
                ပြန်ပေး -EFBIG;

        အကယ်၍ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ပြန်ပေး -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (var_na*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        အကယ်၍ (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                အကယ်၍ (sym_LZ4_versionNumber() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        ပြန်ပေး 0;

                အကယ်၍ (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        အကယ်၍ (memcmp(*buffer, prefix, r) != 0)
                                ပြန်ပေး 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (size < prefix_len + 1)
                        ပြန်ပေး 0;
        }

        ပြန်ပေး memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် decompress_startswith_zstd(
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

#အကယ်၍ HAVE_ZSTD
        ကိန်းပြည့် r;

        r = dlopen_zstd(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        အကယ်၍ (size == ZSTD_CONTENTSIZE_ERROR)
                ပြန်ပေး -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        အကယ်၍ (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                ပြန်ပေး 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        အကယ်၍ (!dctx)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                ပြန်ပေး -ENOMEM;

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
        အကယ်၍ (sym_ZSTD_isError(k))
                ပြန်ပေး log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        အကယ်၍ (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                အကယ်၍ (k != 0)
                        ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                ပြန်ပေး 0; /* Decompressed text too short to match the prefix and extra */
        }

        ပြန်ပေး memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((const uint8_t*) *buffer)[prefix_len] == extra;
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် decompress_startswith_gzip(
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

#အကယ်၍ HAVE_ZLIB
        ကိန်းပြည့် r;

        r = dlopen_zlib(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size > UINT_MAX)
                ပြန်ပေး -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ကိန်းပြည့်) parimaa_na(s));
        အကယ်၍ (r != Z_OK)
                ပြန်ပေး -EBADMSG;

        အကယ်၍ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ပြန်ပေး -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (void*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        အတွက် (;;) {
                r = sym_inflate(&s, Z_FINISH);

                အကယ်၍ (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        ပြန်ပေး -EBADMSG;

                အကယ်၍ (allocated - s.avail_out >= prefix_len + 1)
                        ပြန်ပေး memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                အကယ်၍ (r == Z_STREAM_END)
                        ပြန်ပေး 0;

                အကယ်၍ (s.avail_out > 0)
                        ပြန်ပေး -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                အကယ်၍ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ပြန်ပေး -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

static ကိန်းပြည့် decompress_startswith_bzip2(
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

#အကယ်၍ HAVE_BZIP2
        ကိန်းပြည့် r;

        r = dlopen_bzip2(LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (src_size > UINT_MAX)
                ပြန်ပေး -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        အကယ်၍ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ပြန်ပေး -EIO;

        အကယ်၍ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ပြန်ပေး -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (var_na*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        အတွက် (;;) {
                r = sym_BZ2_bzDecompress(&s);

                အကယ်၍ (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        ပြန်ပေး -EBADMSG;

                အကယ်၍ (allocated - s.avail_out >= prefix_len + 1)
                        ပြန်ပေး memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((const uint8_t*) *buffer)[prefix_len] == extra;

                အကယ်၍ (r == BZ_STREAM_END)
                        ပြန်ပေး 0;

                အကယ်၍ (s.avail_out > 0)
                        ပြန်ပေး -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                အကယ်၍ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ပြန်ပေး -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (var_na*) *buffer + used;
        }
#မဟုတ်ပါက
        ပြန်ပေး -EPROTONOSUPPORT;
#endif
}

ကိန်းပြည့် decompress_startswith_journal(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        pak_sa COMPRESSION_XZ:
                ပြန်ပေး decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        pak_sa COMPRESSION_LZ4:
                ပြန်ပေး decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        pak_sa COMPRESSION_ZSTD:
                ပြန်ပေး decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                ပြန်ပေး -EOPNOTSUPP;
        }
}

ကိန်းပြည့် decompress_startswith(
                Compression compression,
                const void *src, uint64_t src_size,
                void **buffer,
                const void *prefix, size_t prefix_len,
                uint8_t extra) {

        switch (compression) {
        pak_sa COMPRESSION_GZIP:
                ပြန်ပေး decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        pak_sa COMPRESSION_BZIP2:
                ပြန်ပေး decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        default:
                ပြန်ပေး decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

ကိန်းပြည့် compress_stream(
                Compression type,
                ကိန်းပြည့် fdf, ကိန်းပြည့် fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = NULL;
        _cleanup_free_ void *buf = NULL;
        _cleanup_free_ uint8_t *input = NULL;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        ကိန်းပြည့် r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        r = compressor_new(&c, type);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        input = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        အကယ်၍ (!input)
                ပြန်ပေး -ENOMEM;

        အတွက် (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                အကယ်၍ (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = read(fdf, input, m);
                အကယ်၍ (n < 0)
                        ပြန်ပေး -errno;

                အကယ်၍ (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        အကယ်၍ (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;
                                total_out += buf_size;
                        }
                        bha_nga;
                }

                total_in += n;
                အကယ်၍ (max_bytes != UINT64_MAX) {
                        assert(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, input, n, &buf, &buf_size, &buf_alloc);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;
                        total_out += buf_size;
                }
        }

        အကယ်၍ (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        အကယ်၍ (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        မဟုတ်ပါက
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (dviguu_na) total_out / total_in * 100);

        ပြန်ပေး 0;
}

static ကိန်းပြည့် should_sparse(ကိန်းပြည့် fd) {
        sa_nracanaa stat st;

        assert(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        အကယ်၍ (fstat(fd, &st) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (!S_ISREG(st.st_mode))
                ပြန်ပေး false;

        ကိန်းပြည့် flags = fcntl(fd, F_GETFL);
        အကယ်၍ (flags < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (FLAGS_SET(flags, O_APPEND))
                ပြန်ပေး false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        အကယ်၍ (pos < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
static ကိန်းပြည့် finalize_sparse(ကိန်းပြည့် fd) {
        off_t pos;

        assert(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        အကယ်၍ (pos < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (ftruncate(fd, pos) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး 0;
}

/* Common helper for decompress_stream_*() wrappers */

sa_nracanaa decompress_stream_userdata {
        ကိန်းပြည့် fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

static ကိန်းပြည့် decompress_stream_write_callback(const void *data, size_t size, void *userdata) {
        sa_nracanaa decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        အကယ်၍ (u->max_bytes != UINT64_MAX) {
                အကယ်၍ (u->max_bytes < size)
                        ပြန်ပေး -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        အကယ်၍ (u->skip > 0) {
                အကယ်၍ (u->skip >= size) {
                        u->skip -= size;
                        ပြန်ပေး 0;
                }

                data = (const uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        အကယ်၍ (size == 0)
                ပြန်ပေး 0;

        အကယ်၍ (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                အကယ်၍ (k < 0)
                        ပြန်ပေး (ကိန်းပြည့်) k;
                ပြန်ပေး 0;
        }

        ပြန်ပေး loop_write(u->fd, data, size);
}

static ကိန်းပြည့် decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        assert(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        switch (type) {
        pak_sa COMPRESSION_NONE:
                bha_nga;

#အကယ်၍ HAVE_XZ
        pak_sa COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                အကယ်၍ (xz_ret != LZMA_OK)
                        ပြန်ပေး -EIO;
                bha_nga;
        }
#endif

#အကယ်၍ HAVE_LZ4
        pak_sa COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                အကယ်၍ (sym_LZ4F_isError(rc))
                        ပြန်ပေး -ENOMEM;

                bha_nga;
        }
#endif

#အကယ်၍ HAVE_ZSTD
        pak_sa COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                အကယ်၍ (!c->d_zstd)
                        ပြန်ပေး -ENOMEM;
                bha_nga;
#endif

#အကယ်၍ HAVE_ZLIB
        pak_sa COMPRESSION_GZIP:
                အကယ်၍ (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ကိန်းပြည့်) parimaa_na(c->gzip)) != Z_OK)
                        ပြန်ပေး -EIO;
                bha_nga;
#endif

#အကယ်၍ HAVE_BZIP2
        pak_sa COMPRESSION_BZIP2:
                အကယ်၍ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        ပြန်ပေး -EIO;
                bha_nga;
#endif

        default:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        ပြန်ပေး 0;
}

static ကိန်းပြည့် decompress_blob_to_fd_impl(
                Compression type,
                const void *src,
                uint64_t src_size,
                ကိန်းပြည့် fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ကိန်းပြည့် r;

        assert(src);
        assert(fdt >= 0);

        အကယ်၍ (type == COMPRESSION_NONE)
                ပြန်ပေး -EOPNOTSUPP;

        အကယ်၍ (src_size == 0)
                ပြန်ပေး -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        အကယ်၍ (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                အကယ်၍ (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        အကယ်၍ (src_size <= parimaa_na(uint64_t))
                                ပြန်ပေး -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        အကယ်၍ (sz < skip)
                                ပြန်ပေး -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                အကယ်၍ (max_bytes == 0)
                        ပြန်ပေး -ENOBUFS;

                _cleanup_free_ void *buf = NULL;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                /* Check the result size again for safety. */
                အကယ်၍ (size < skip)
                        ပြန်ပေး -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (dviguu_na) size / src_size * 100);

                အကယ်၍ (ret_size)
                        *ret_size = size - skip;
                ပြန်ပေး 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        sa_nracanaa decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_push(c, src, src_size, decompress_stream_write_callback, &userdata);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Finalize */
        r = decompressor_push(c, /* data= */ NULL, /* size= */ 0, decompress_stream_write_callback, &userdata);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (userdata.total_out < skip)
                ပြန်ပေး -EBADMSG;

        အကယ်၍ (userdata.sparse) {
                r = finalize_sparse(fdt);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (dviguu_na) userdata.total_out / src_size * 100);

        အကယ်၍ (ret_size)
                *ret_size = userdata.total_out - skip;

        ပြန်ပေး 0;
}

ကိန်းပြည့် decompress_blob_to_fd_journal(
                Compression type,
                const void *src,
                uint64_t src_size,
                ကိန်းပြည့် fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ကိန်းပြည့် r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

ကိန်းပြည့် decompress_blob_to_fd(
                Compression type,
                const void *src,
                uint64_t src_size,
                ကိန်းပြည့် fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ကိန်းပြည့် r;

        r = dlopen_compress(type, LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

ကိန်းပြည့် decompress_stream_full(
                Compression type,
                ကိန်းပြည့် fdf, ကိန်းပြည့် fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = NULL;
        _cleanup_free_ uint8_t *buf = NULL;
        uint64_t total_in = 0;
        ကိန်းပြည့် r;

        assert(fdf >= 0);
        assert(fdt >= 0);

        အကယ်၍ (type == COMPRESSION_NONE)
                ပြန်ပေး -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = decompressor_new(&c, type, flags);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        sa_nracanaa decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        အကယ်၍ (!buf)
                ပြန်ပေး -ENOMEM;

        အတွက် (;;) {
                ssize_t n;

                n = read(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                အကယ်၍ (n < 0)
                        ပြန်ပေး -errno;

                total_in += n;

                r = decompressor_push(c, buf, n, decompress_stream_write_callback, &userdata);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (n == 0)
                        bha_nga;
        }

        အကယ်၍ (total_in == 0)
                ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        အကယ်၍ (userdata.sparse) {
                r = finalize_sparse(fdt);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (dviguu_na) userdata.total_out / total_in * 100);

        ပြန်ပေး 0;
}

ကိန်းပြည့် decompress_stream_by_filename(const var_na *filename, ကိန်းပြည့် fdf, ကိန်းပြည့် fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        အကယ်၍ (c == COMPRESSION_NONE)
                ပြန်ပေး -EPROTONOSUPPORT;

        ပြန်ပေး decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        အကယ်၍ (!c)
                ပြန်ပေး NULL;

        switch (c->type) {

#အကယ်၍ HAVE_XZ
        pak_sa COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                bha_nga;
#endif

#အကယ်၍ HAVE_LZ4
        pak_sa COMPRESSION_LZ4:
                အကယ်၍ (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = NULL;
                        c->lz4_header = mfree(c->lz4_header);
                } မဟုတ်ပါက {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = NULL;
                }
                bha_nga;
#endif

#အကယ်၍ HAVE_ZSTD
        pak_sa COMPRESSION_ZSTD:
                အကယ်၍ (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = NULL;
                } မဟုတ်ပါက {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = NULL;
                }
                bha_nga;
#endif

#အကယ်၍ HAVE_ZLIB
        pak_sa COMPRESSION_GZIP:
                အကယ်၍ (c->encoding)
                        sym_deflateEnd(&c->gzip);
                မဟုတ်ပါက
                        sym_inflateEnd(&c->gzip);
                bha_nga;
#endif

#အကယ်၍ HAVE_BZIP2
        pak_sa COMPRESSION_BZIP2:
                အကယ်၍ (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                မဟုတ်ပါက
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                bha_nga;
#endif

        default:
                bha_nga;
        }

        ပြန်ပေး mfree(c);
}

Compression compressor_type(const Compressor *c) {
        ပြန်ပေး c ? c->type : _COMPRESSION_INVALID;
}

ကိန်းပြည့် decompressor_detect(Decompressor **decompressor, const void *data, size_t size) {
        ကိန်းပြည့် r;

        assert(decompressor);

        အကယ်၍ (*decompressor)
                ပြန်ပေး 1;

        အကယ်၍ (size < COMPRESSION_MAGIC_BYTES_MAX)
                ပြန်ပေး 0;

        assert(data);

        Compression type = compression_detect_from_magic(data);
        အကယ်၍ (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး 1;
}

ကိန်းပြည့် decompressor_force(Decompressor **decompressor, Compression type) {
        ကိန်းပြည့် r;

        assert(decompressor);
        assert(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *decompressor = compressor_free(*decompressor);
        ပြန်ပေး decompressor_new(decompressor, type, /* flags= */ 0);
}

ကိန်းပြည့် decompressor_force_off(Decompressor **decompressor) {
        ပြန်ပေး decompressor_force(decompressor, COMPRESSION_NONE);
}

ကိန်းပြည့် decompressor_push(Decompressor *c, const void *data, size_t size, DecompressorCallback callback, void *userdata) {
        ကိန်းပြည့် r;

        assert(c);
        assert(data || size == 0);
        assert(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        အကယ်၍ (c->encoding)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                ပြန်ပေး 0;

#အကယ်၍ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = NULL;
        အကယ်၍ (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                အကယ်၍ (!buffer)
                        ပြန်ပေး -ENOMEM;
        }
#endif

        switch (c->type) {

        pak_sa COMPRESSION_NONE:
                အကယ်၍ (c->finished)
                        ပြန်ပေး -EBADMSG;

                အကယ်၍ (size == 0) {
                        c->finished = true;
                        ပြန်ပေး 0;
                }

                r = callback(data, size, userdata);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                ပြန်ပေး 0;

#အကယ်၍ HAVE_XZ
        pak_sa COMPRESSION_XZ:
                အကယ်၍ (c->finished)
                        ပြန်ပေး -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                အတွက် (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        အကယ်၍ (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                ပြန်ပေး -EBADMSG;

                        အကယ်၍ (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ပြန်ပေး -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        အကယ်၍ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;
                        }

                        အကယ်၍ (lzr == LZMA_STREAM_END) {
                                အကယ်၍ (c->xz.avail_in > 0)
                                        ပြန်ပေး -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                ပြန်ပေး 0;
                        }

                        အကယ်၍ (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                အကယ်၍ (size == 0)
                                        ပြန်ပေး -EBADMSG; /* truncated */

                                ပြန်ပေး 0; /* No progress possible with the current input. */
                        }
                }
#endif

#အကယ်၍ HAVE_LZ4
        pak_sa COMPRESSION_LZ4: {
                const uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                အတွက် (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, NULL);
                        အကယ်၍ (sym_LZ4F_isError(rc))
                                ပြန်ပေး -EBADMSG;

                        အကယ်၍ (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                ပြန်ပေး -EIO;

                        အကယ်၍ (consumed > 0) {
                                assert(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        အကယ်၍ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;
                        }

                        အကယ်၍ (src_remaining == 0) {
                                အကယ်၍ (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        ပြန်ပေး 0;
                                }

                                အကယ်၍ (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                အကယ်၍ (size == 0)
                                        ပြန်ပေး -EBADMSG; /* truncated */

                                ပြန်ပေး 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#အကယ်၍ HAVE_ZSTD
        pak_sa COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src =  (void*) data,
                        .size = size,
                };

                c->finished = false;

                အတွက် (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &input);
                        အကယ်၍ (sym_ZSTD_isError(res))
                                ပြန်ပေး -EBADMSG;

                        အကယ်၍ (input.pos > input.size ||
                            output.pos > output.size)
                                ပြန်ပေး -EIO;

                        အကယ်၍ (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;
                        }

                        အကယ်၍ (input.pos == input.size) {
                                အကယ်၍ (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        ပြန်ပေး 0;
                                }

                                အကယ်၍ (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        continue;

                                အကယ်၍ (size == 0)
                                        ပြန်ပေး -EBADMSG; /* truncated */

                                ပြန်ပေး 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#အကယ်၍ HAVE_ZLIB
        pak_sa COMPRESSION_GZIP:
                အကယ်၍ (size > UINT_MAX)
                        ပြန်ပေး -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                အတွက် (;;) {
                        အကယ်၍ (c->finished) {
                                အကယ်၍ (sym_inflateReset(&c->gzip) != Z_OK)
                                        ပြန်ပေး -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        ကိန်းပြည့် zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        အကယ်၍ (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                ပြန်ပေး -EBADMSG;

                        အကယ်၍ (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ပြန်ပေး -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        အကယ်၍ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;
                        }

                        အကယ်၍ (zr == Z_STREAM_END) {
                                c->finished = true;

                                အကယ်၍ (c->gzip.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                ပြန်ပေး 0;
                        }

                        အကယ်၍ (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                အကယ်၍ (size == 0)
                                        ပြန်ပေး -EBADMSG; /* truncated */

                                ပြန်ပေး 0; /* No progress possible with the current input. */
                        }
                }
#endif

#အကယ်၍ HAVE_BZIP2
        pak_sa COMPRESSION_BZIP2:
                အကယ်၍ (size > UINT_MAX)
                        ပြန်ပေး -EFBIG;

                c->bzip2.next_in = (var_na*) data;
                c->bzip2.avail_in = size;

                အတွက် (;;) {
                        အကယ်၍ (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                အကယ်၍ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        ပြန်ပေး -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (var_na*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        ကိန်းပြည့် bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        အကယ်၍ (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                ပြန်ပေး -EBADMSG;

                        အကယ်၍ (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ပြန်ပေး -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        အကယ်၍ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                အကယ်၍ (r < 0)
                                        ပြန်ပေး r;
                        }

                        အကယ်၍ (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                အကယ်၍ (c->bzip2.avail_in > 0)
                                        continue; /* New stream may be pending. */

                                ပြန်ပေး 0;
                        }

                        အကယ်၍ (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                အကယ်၍ (size == 0)
                                        ပြန်ပေး -EBADMSG; /* truncated */

                                ပြန်ပေး 0; /* No progress possible with the current input. */
                        }
                }
#endif

        default:
                assert_not_reached();
        }
}

ကိန်းပြည့် compressor_new(Compressor **ret, Compression type) {
        ကိန်းပြည့် r;

        assert(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        switch (type) {

#အကယ်၍ HAVE_XZ
        pak_sa COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                အကယ်၍ (xzr != LZMA_OK)
                        ပြန်ပေး -EIO;

                c->type = COMPRESSION_XZ;
                bha_nga;
        }
#endif

#အကယ်၍ HAVE_LZ4
        pak_sa COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                အကယ်၍ (sym_LZ4F_isError(rc))
                        ပြန်ပေး -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                အကယ်၍ (!c->lz4_header)
                        ပြန်ပေး -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                အကယ်၍ (sym_LZ4F_isError(c->lz4_header_size))
                        ပြန်ပေး -EINVAL;

                bha_nga;
        }
#endif

#အကယ်၍ HAVE_ZSTD
        pak_sa COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                အကယ်၍ (!c->c_zstd)
                        ပြန်ပေး -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                အကယ်၍ (sym_ZSTD_isError(z))
                        ပြန်ပေး -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                အကယ်၍ (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                bha_nga;
#endif

#အကယ်၍ HAVE_ZLIB
        pak_sa COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (ကိန်းပြည့်) parimaa_na(c->gzip));
                အကယ်၍ (r != Z_OK)
                        ပြန်ပေး -EIO;

                c->type = COMPRESSION_GZIP;
                bha_nga;
#endif

#အကယ်၍ HAVE_BZIP2
        pak_sa COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                အကယ်၍ (r != BZ_OK)
                        ပြန်ပေး -EIO;

                c->type = COMPRESSION_BZIP2;
                bha_nga;
#endif

        pak_sa COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                bha_nga;

        default:
                ပြန်ပေး -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        ပြန်ပေး 0;
}

#အကယ်၍ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
static ကိန်းပြည့် enlarge_buffer(void **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        အကယ်၍ (*buffer_allocated >= need)
                ပြန်ပေး 0;

        အကယ်၍ (!greedy_realloc(buffer, need, 1))
                ပြန်ပေး -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        ပြန်ပေး 1;
}
#endif

ကိန်းပြည့် compressor_start(
                Compressor *c,
                const void *data,
                size_t size,
                void **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#အကယ်၍ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ကိန်းပြည့် r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        အကယ်၍ (!c->encoding)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (size == 0)
                ပြန်ပေး 0;

        assert(data);

        *buffer_size = 0;

        switch (c->type) {

#အကယ်၍ HAVE_XZ
        pak_sa COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                နေစဉ် (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        အကယ်၍ (lzr != LZMA_OK)
                                ပြန်ပေး -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                bha_nga;
#endif

#အကယ်၍ HAVE_LZ4
        pak_sa COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                အကယ်၍ (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, NULL);
                အကယ်၍ (sym_LZ4F_isError(n))
                        ပြန်ပေး -EIO;

                *buffer_size += n;
                bha_nga;
        }
#endif

#အကယ်၍ HAVE_ZSTD
        pak_sa COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {
                        .src = data,
                        .size = size,
                };

                နေစဉ် (input.pos < input.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_continue);
                        အကယ်၍ (sym_ZSTD_isError(res))
                                ပြန်ပေး -EIO;

                        *buffer_size += output.pos;
                }

                bha_nga;
        }
#endif

#အကယ်၍ HAVE_ZLIB
        pak_sa COMPRESSION_GZIP:
                အကယ်၍ (size > UINT_MAX)
                        ပြန်ပေး -EFBIG;

                c->gzip.next_in = (void*) data;
                c->gzip.avail_in = size;

                နေစဉ် (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        အကယ်၍ (r != Z_OK)
                                ပြန်ပေး -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                bha_nga;
#endif

#အကယ်၍ HAVE_BZIP2
        pak_sa COMPRESSION_BZIP2:
                အကယ်၍ (size > UINT_MAX)
                        ပြန်ပေး -EFBIG;

                c->bzip2.next_in = (void*) data;
                c->bzip2.avail_in = size;

                နေစဉ် (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        အကယ်၍ (r != BZ_RUN_OK)
                                ပြန်ပေး -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                bha_nga;
#endif

        pak_sa COMPRESSION_NONE:

                အကယ်၍ (*buffer_allocated < size) {
                        void *p;

                        p = realloc(*buffer, size);
                        အကယ်၍ (!p)
                                ပြန်ပေး -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                bha_nga;

        default:
                ပြန်ပေး -EOPNOTSUPP;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် compressor_finish(Compressor *c, void **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#အကယ်၍ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ကိန်းပြည့် r;
#endif

        assert(c);
        assert(buffer);
        assert(buffer_size);
        assert(buffer_allocated);

        အကယ်၍ (!c->encoding)
                ပြန်ပေး -EINVAL;

        *buffer_size = 0;

        switch (c->type) {

#အကယ်၍ HAVE_XZ
        pak_sa COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        အကယ်၍ (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                ပြန်ပေး -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } နေစဉ် (lzr != LZMA_STREAM_END);

                bha_nga;
        }
#endif

#အကယ်၍ HAVE_LZ4
        pak_sa COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, NULL);
                အကယ်၍ (sym_LZ4F_isError(n))
                        ပြန်ပေး -EIO;

                *buffer_size = n;
                bha_nga;
        }
#endif

#အကယ်၍ HAVE_ZSTD
        pak_sa COMPRESSION_ZSTD: {
                ZSTD_inBuffer input = {};
                size_t res;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &input, ZSTD_e_end);
                        အကယ်၍ (sym_ZSTD_isError(res))
                                ပြန်ပေး -EIO;

                        *buffer_size += output.pos;
                } နေစဉ် (res != 0);

                bha_nga;
        }
#endif

#အကယ်၍ HAVE_ZLIB
        pak_sa COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        အကယ်၍ (!IN_SET(r, Z_OK, Z_STREAM_END))
                                ပြန်ပေး -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } နေစဉ် (r != Z_STREAM_END);

                bha_nga;
#endif

#အကယ်၍ HAVE_BZIP2
        pak_sa COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                do {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        အကယ်၍ (r < 0)
                                ပြန်ပေး r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (void*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        အကယ်၍ (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                ပြန်ပေး -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } နေစဉ် (r != BZ_STREAM_END);

                bha_nga;
#endif

        pak_sa COMPRESSION_NONE:
                bha_nga;

        default:
                ပြန်ပေး -EOPNOTSUPP;
        }

        ပြန်ပေး 0;
}
