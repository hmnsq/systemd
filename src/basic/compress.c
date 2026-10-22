/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#إذا HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#ضمن <lzma.h>
#endif

#إذا HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#ضمن <lz4.h>
#ضمن <lz4frame.h>
#ضمن <lz4hc.h>
#endif

#إذا HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#ضمن <zstd.h>
#ضمن <zstd_errors.h>
#endif

#إذا HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#ضمن <zlib.h>
#endif

#إذا HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#ضمن <bzlib.h>
#endif

#ضمن "alloc-util.h"
#ضمن "bitfield.h"
#ضمن "compress.h"
#ضمن "dlfcn-util.h"
#ضمن "io-util.h"
#ضمن "log.h"
#ضمن "string-table.h"
#ضمن "unaligned.h"

#إذا HAVE_XZ
ساكن DLSYM_PROTOTYPE(lzma_auto_decoder) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_code) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_easy_encoder) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_end) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_stream_decoder) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_lzma_preset) = عدم;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ساكن مضمن فراغ lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#إذا HAVE_LZ4
ساكن DLSYM_PROTOTYPE(LZ4F_compressBegin) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_compressBound) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_compressEnd) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_compressUpdate) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_decompress) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_isError) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_compress_HC) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_compress_default) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_decompress_safe) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_version_نومبور) = عدم;

ساكن ثابت LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#إذا HAVE_ZSTD
ساكن DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_compress) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_compressStream2) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_createCCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_createDCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_decompressStream) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_freeCCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_freeDCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_getErrorCode) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_getErrorName) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_isError) = عدم;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, عدم);

ساكن صحيح zstd_ret_to_errno(size_t ret) {
        بدل (sym_ZSTD_getErrorCode(ret)) {
        حالة ZSTD_error_dstSize_tooSmall:
                أرجع -ENOBUFS;
        حالة ZSTD_error_memory_allocation:
                أرجع -ENOMEM;
        افتراضي:
                أرجع -EBADMSG;
        }
}
#endif

#إذا HAVE_ZLIB
ساكن DLSYM_PROTOTYPE(deflateInit2_) = عدم;
ساكن DLSYM_PROTOTYPE(deflate) = عدم;
ساكن DLSYM_PROTOTYPE(deflateEnd) = عدم;
ساكن DLSYM_PROTOTYPE(inflateInit2_) = عدم;
ساكن DLSYM_PROTOTYPE(inflate) = عدم;
ساكن DLSYM_PROTOTYPE(inflateEnd) = عدم;
ساكن DLSYM_PROTOTYPE(inflateReset) = عدم;

ساكن مضمن فراغ deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

ساكن مضمن فراغ inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#إذا HAVE_BZIP2
ساكن DLSYM_PROTOTYPE(BZ2_bzCompressInit) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzCompress) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzDecompress) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = عدم;

ساكن مضمن فراغ BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

ساكن مضمن فراغ BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
بنية Compressor {
        Compression type;
        bool encoding;
        bool finished;
        اتحاد {
#إذا HAVE_XZ
                lzma_stream xz;
#endif
#إذا HAVE_LZ4
                بنية {
                        LZ4F_compressionContext_t c_lz4;
                        فراغ *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#إذا HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#إذا HAVE_ZLIB
                z_stream gzip;
#endif
#إذا HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, حجم(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

ساكن ثابت حرف* ثابت compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

ساكن ثابت حرف* ثابت compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

ساكن ثابت حرف* ثابت compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(ثابت حرف *s) {
        Compression c;

        أكد(s);

        c = compression_from_string(s);
        إذا (c >= 0)
                أرجع c;

        أرجع compression_uppercase_from_string(s);
}

Compression compression_from_filename(ثابت حرف *filename) {
        Compression c;
        ثابت حرف *e;

        أكد(filename);

        e = strrchr(filename, '.');
        إذا (!e)
                أرجع COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        إذا (c < 0)
                أرجع COMPRESSION_NONE;

        أرجع c;
}

bool compression_supported_journal(Compression c) {
        ساكن ثابت unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        أرجع BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        ساكن ثابت unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        أرجع BIT_SET(supported, c);
}

Compression compression_detect_from_magic(ثابت uint8_t data[ساكن COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        إذا (memcmp(data, (ثابت uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                أرجع COMPRESSION_GZIP;
        إذا (memcmp(data, (ثابت uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                أرجع COMPRESSION_XZ;
        إذا (memcmp(data, (ثابت uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                أرجع COMPRESSION_ZSTD;
        إذا (memcmp(data, (ثابت uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                أرجع COMPRESSION_LZ4;
        إذا (memcmp(data, (ثابت uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                أرجع COMPRESSION_BZIP2;

        أرجع _COMPRESSION_INVALID;
}

صحيح dlopen_xz(صحيح log_level) {
#إذا HAVE_XZ
        ساكن فراغ *lzma_dl = عدم;

        LIBLZMA_NOTE(suggested);

        أرجع dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#وإلا
        أرجع log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

صحيح dlopen_lz4(صحيح log_level) {
#إذا HAVE_LZ4
        ساكن فراغ *lz4_dl = عدم;

        LIBLZ4_NOTE(suggested);

        أرجع dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_نومبور));
#وإلا
        أرجع log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

صحيح dlopen_zstd(صحيح log_level) {
#إذا HAVE_ZSTD
        ساكن فراغ *zstd_dl = عدم;

        LIBZSTD_NOTE(suggested);

        أرجع dlopen_many_sym_or_warn(
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
#وإلا
        أرجع log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

صحيح dlopen_zlib(صحيح log_level) {
#إذا HAVE_ZLIB
        ساكن فراغ *zlib_dl = عدم;

        LIBZ_NOTE(suggested);

        أرجع dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#وإلا
        أرجع log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

صحيح dlopen_bzip2(صحيح log_level) {
#إذا HAVE_BZIP2
        ساكن فراغ *bzip2_dl = عدم;

        LIBBZ2_NOTE(suggested);

        أرجع dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#وإلا
        أرجع log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

صحيح dlopen_compress_journal(Compression c, صحيح log_level) {
        بدل (c) {
        حالة COMPRESSION_NONE:
                أرجع 0;
        حالة COMPRESSION_XZ:
                أرجع dlopen_xz(log_level);
        حالة COMPRESSION_LZ4:
                أرجع dlopen_lz4(log_level);
        حالة COMPRESSION_ZSTD:
                أرجع dlopen_zstd(log_level);
        افتراضي:
                أرجع -EOPNOTSUPP;
        }
}

صحيح dlopen_compress(Compression c, صحيح log_level) {
        بدل (c) {
        حالة COMPRESSION_GZIP:
                أرجع dlopen_zlib(log_level);
        حالة COMPRESSION_BZIP2:
                أرجع dlopen_bzip2(log_level);
        افتراضي:
                أرجع dlopen_compress_journal(c, log_level);
        }
}

ساكن صحيح compress_blob_xz(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                صحيح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#إذا HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, عدم, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, عدم }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        صحيح r;

        r = dlopen_xz(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                إذا (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        أرجع -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        إذا (src_size < 80)
                أرجع -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ عدم,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        إذا (ret != LZMA_OK)
                أرجع -ENOBUFS;

        *dst_size = out_pos;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح compress_blob_lz4(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                صحيح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#إذا HAVE_LZ4
        صحيح r;

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        إذا (src_size < 9)
                أرجع -ENOBUFS;

        إذا (src_size > INT_MAX)
                أرجع -EFBIG;
        إذا (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        إذا (level <= 0)
                r = sym_LZ4_compress_default(src, (حرف*)dst + 8, src_size, (صحيح) dst_alloc_size - 8);
        وإلا
                r = sym_LZ4_compress_HC(src, (حرف*)dst + 8, src_size, (صحيح) dst_alloc_size - 8, level);
        إذا (r <= 0)
                أرجع -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح compress_blob_zstd(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                صحيح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#إذا HAVE_ZSTD
        size_t k;
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        إذا (sym_ZSTD_isError(k))
                أرجع zstd_ret_to_errno(k);

        *dst_size = k;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح compress_blob_gzip(ثابت فراغ *src, uint64_t src_size,
                       فراغ *dst, size_t dst_alloc_size, size_t *dst_size, صحيح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -ENOMEM;

        s.next_in = (فراغ*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        إذا (r != Z_STREAM_END)
                أرجع -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح compress_blob_bzip2(
                ثابت فراغ *src, uint64_t src_size,
                فراغ *dst, size_t dst_alloc_size, size_t *dst_size, صحيح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        إذا (r != BZ_OK)
                أرجع -ENOMEM;

        s.next_in = (حرف*) src;
        s.avail_in = src_size;
        s.next_out = (حرف*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        إذا (r != BZ_STREAM_END)
                أرجع -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

صحيح compress_blob_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_size,
                فراغ *dst, size_t dst_alloc_size, size_t *dst_size, صحيح level) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        حالة COMPRESSION_LZ4:
                أرجع compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        حالة COMPRESSION_ZSTD:
                أرجع compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        افتراضي:
                أرجع -EOPNOTSUPP;
        }
}

صحيح compress_blob(
                Compression compression,
                ثابت فراغ *src, uint64_t src_size,
                فراغ *dst, size_t dst_alloc_size, size_t *dst_size, صحيح level) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        حالة COMPRESSION_BZIP2:
                أرجع compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        افتراضي:
                أرجع compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

ساكن صحيح decompress_blob_xz(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#إذا HAVE_XZ
        صحيح r;

        r = dlopen_xz(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        إذا (ret != LZMA_OK)
                أرجع -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        لكل (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                إذا (ret == LZMA_STREAM_END)
                        اكسر;
                إذا (ret != LZMA_OK)
                        أرجع -ENOMEM;

                إذا (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                أكد(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح decompress_blob_lz4(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#إذا HAVE_LZ4
        حرف* out;
        صحيح r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size <= 8)
                أرجع -EBADMSG;

        إذا (src_size - 8 > INT_MAX)
                أرجع -EFBIG;

        size = unaligned_read_le64(src);
        إذا (size < 0 || (unsigned) size != unaligned_read_le64(src))
                أرجع -EFBIG;
        إذا (dst_max > 0 && (size_t) size > dst_max)
                أرجع -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        إذا (!out)
                أرجع -ENOMEM;

        r = sym_LZ4_decompress_safe((حرف*)src + 8, out, src_size - 8, size);
        إذا (r < 0 || r != size)
                أرجع -EBADMSG;

        *dst_size = size;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(فراغ) {
#إذا HAVE_ZSTD
        إذا (dlopen_zstd(LOG_DEBUG) < 0)
                أرجع 0;

        أرجع sym_ZSTD_DStreamOutSize();
#وإلا
        أرجع 0;
#endif
}

ساكن صحيح decompress_blob_zstd(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#إذا HAVE_ZSTD
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        إذا (size == ZSTD_CONTENTSIZE_ERROR)
                أرجع -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        إذا (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        وإلا {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        إذا (!dctx)
                أرجع -ENOMEM;

        ZSTD_inBuffer ماسوقن = {
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

        لكل (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &ماسوقن);
                إذا (sym_ZSTD_isError(k))
                        أرجع log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                إذا (output.pos >= cap)
                        اكسر;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                إذا (k == 0 && ماسوقن.pos >= ماسوقن.size)
                        اكسر;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                إذا (output.pos == output.size) {
                        إذا (space > SIZE_MAX / 2)
                                أرجع -E2BIG;
                        space = MIN(2 * space, cap);
                        إذا (!greedy_realloc(dst, space, 1))
                                أرجع -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        استمر;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                إذا (ماسوقن.pos >= ماسوقن.size)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح decompress_blob_gzip(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.next_in = (فراغ*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        لكل (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                إذا (r == Z_STREAM_END)
                        اكسر;
                إذا (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        أرجع -EBADMSG;

                إذا (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح decompress_blob_bzip2(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        إذا (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                أرجع -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.next_in = (حرف*) src;
        s.avail_in = src_size;
        s.next_out = (حرف*) *dst;
        s.avail_out = space;

        لكل (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                إذا (r == BZ_STREAM_END)
                        اكسر;
                إذا (r != BZ_OK)
                        أرجع -EBADMSG;

                إذا (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (حرف*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

صحيح decompress_blob_journal(
                Compression compression,
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **dst,
                size_t *dst_size,
                size_t dst_max) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        حالة COMPRESSION_LZ4:
                أرجع decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        حالة COMPRESSION_ZSTD:
                أرجع decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        افتراضي:
                أرجع -EPROTONOSUPPORT;
        }
}

صحيح decompress_blob(
                Compression compression,
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **dst,
                size_t *dst_size,
                size_t dst_max) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        حالة COMPRESSION_BZIP2:
                أرجع decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        افتراضي:
                أرجع decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

صحيح decompress_zlib_raw(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ *dst,
                size_t dst_size,
                صحيح wbits) {

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_size > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (فراغ*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        إذا (r != Z_STREAM_END || produced != dst_size)
                أرجع -EBADMSG;

        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح decompress_startswith_xz(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **buffer,
                ثابت فراغ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#إذا HAVE_XZ
        صحيح r;

        r = dlopen_xz(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        إذا (ret != LZMA_OK)
                أرجع -EBADMSG;

        إذا (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        لكل (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                إذا (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_out >= prefix_len + 1)
                        أرجع memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;

                إذا (ret == LZMA_STREAM_END)
                        أرجع 0;

                s.avail_out += allocated;

                إذا (!(greedy_realloc(buffer, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح decompress_startswith_lz4(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **buffer,
                ثابت فراغ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#إذا HAVE_LZ4
        size_t allocated;
        صحيح r;

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size <= 8)
                أرجع -EBADMSG;

        إذا (src_size - 8 > INT_MAX)
                أرجع -EFBIG;

        إذا (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                أرجع -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (حرف*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        إذا (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                إذا (sym_LZ4_version_نومبور() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        أرجع 0;

                إذا (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        إذا (memcmp(*buffer, prefix, r) != 0)
                                أرجع 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                إذا (r < 0)
                        أرجع r;

                إذا (size < prefix_len + 1)
                        أرجع 0;
        }

        أرجع memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح decompress_startswith_zstd(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **buffer,
                ثابت فراغ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#إذا HAVE_ZSTD
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        إذا (size == ZSTD_CONTENTSIZE_ERROR)
                أرجع -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        إذا (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                أرجع 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        إذا (!dctx)
                أرجع -ENOMEM;

        إذا (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                أرجع -ENOMEM;

        ZSTD_inBuffer ماسوقن = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &ماسوقن);
        إذا (sym_ZSTD_isError(k))
                أرجع log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        إذا (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                إذا (k != 0)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                أرجع 0; /* Decompressed text too short to match the prefix and extra */
        }

        أرجع memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح decompress_startswith_gzip(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **buffer,
                ثابت فراغ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -EBADMSG;

        إذا (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (فراغ*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        لكل (;;) {
                r = sym_inflate(&s, Z_FINISH);

                إذا (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_out >= prefix_len + 1)
                        أرجع memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;

                إذا (r == Z_STREAM_END)
                        أرجع 0;

                إذا (s.avail_out > 0)
                        أرجع -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                إذا (!(greedy_realloc(buffer, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

ساكن صحيح decompress_startswith_bzip2(
                ثابت فراغ *src,
                uint64_t src_size,
                فراغ **buffer,
                ثابت فراغ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_size > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        إذا (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                أرجع -EIO;

        إذا (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (حرف*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        لكل (;;) {
                r = sym_BZ2_bzDecompress(&s);

                إذا (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_out >= prefix_len + 1)
                        أرجع memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;

                إذا (r == BZ_STREAM_END)
                        أرجع 0;

                إذا (s.avail_out > 0)
                        أرجع -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                إذا (!(greedy_realloc(buffer, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (حرف*) *buffer + used;
        }
#وإلا
        أرجع -EPROTONOSUPPORT;
#endif
}

صحيح decompress_startswith_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_size,
                فراغ **buffer,
                ثابت فراغ *prefix, size_t prefix_len,
                uint8_t extra) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        حالة COMPRESSION_LZ4:
                أرجع decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        حالة COMPRESSION_ZSTD:
                أرجع decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        افتراضي:
                أرجع -EOPNOTSUPP;
        }
}

صحيح decompress_startswith(
                Compression compression,
                ثابت فراغ *src, uint64_t src_size,
                فراغ **buffer,
                ثابت فراغ *prefix, size_t prefix_len,
                uint8_t extra) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        حالة COMPRESSION_BZIP2:
                أرجع decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        افتراضي:
                أرجع decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

صحيح compress_stream(
                Compression type,
                صحيح fdf, صحيح fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = عدم;
        _cleanup_free_ فراغ *buf = عدم;
        _cleanup_free_ uint8_t *ماسوقن = عدم;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        صحيح r;

        أكد(fdf >= 0);
        أكد(fdt >= 0);

        r = compressor_new(&c, type);
        إذا (r < 0)
                أرجع r;

        ماسوقن = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        إذا (!ماسوقن)
                أرجع -ENOMEM;

        لكل (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                إذا (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = اقرأ(fdf, ماسوقن, m);
                إذا (n < 0)
                        أرجع -errno;

                إذا (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        إذا (r < 0)
                                أرجع r;

                        إذا (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                إذا (r < 0)
                                        أرجع r;
                                total_out += buf_size;
                        }
                        اكسر;
                }

                total_in += n;
                إذا (max_bytes != UINT64_MAX) {
                        أكد(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, ماسوقن, n, &buf, &buf_size, &buf_alloc);
                إذا (r < 0)
                        أرجع r;

                إذا (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        إذا (r < 0)
                                أرجع r;
                        total_out += buf_size;
                }
        }

        إذا (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        إذا (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        وإلا
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (مزدوج) total_out / total_in * 100);

        أرجع 0;
}

ساكن صحيح should_sparse(صحيح fd) {
        بنية stat st;

        أكد(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        إذا (fstat(fd, &st) < 0)
                أرجع -errno;

        إذا (!S_ISREG(st.st_mode))
                أرجع false;

        صحيح flags = fcntl(fd, F_GETFL);
        إذا (flags < 0)
                أرجع -errno;

        إذا (FLAGS_SET(flags, O_APPEND))
                أرجع false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        إذا (pos < 0)
                أرجع -errno;

        أرجع pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
ساكن صحيح finalize_sparse(صحيح fd) {
        off_t pos;

        أكد(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        إذا (pos < 0)
                أرجع -errno;

        إذا (ftruncate(fd, pos) < 0)
                أرجع -errno;

        أرجع 0;
}

/* Common helper for decompress_stream_*() wrappers */

بنية decompress_stream_userdata {
        صحيح fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ساكن صحيح decompress_stream_write_callback(ثابت فراغ *data, size_t size, فراغ *userdata) {
        بنية decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        إذا (u->max_bytes != UINT64_MAX) {
                إذا (u->max_bytes < size)
                        أرجع -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        إذا (u->skip > 0) {
                إذا (u->skip >= size) {
                        u->skip -= size;
                        أرجع 0;
                }

                data = (ثابت uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        إذا (size == 0)
                أرجع 0;

        إذا (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                إذا (k < 0)
                        أرجع (صحيح) k;
                أرجع 0;
        }

        أرجع loop_write(u->fd, data, size);
}

ساكن صحيح decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        أكد(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        إذا (!c)
                أرجع -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        بدل (type) {
        حالة COMPRESSION_NONE:
                اكسر;

#إذا HAVE_XZ
        حالة COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                إذا (xz_ret != LZMA_OK)
                        أرجع -EIO;
                اكسر;
        }
#endif

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                إذا (sym_LZ4F_isError(rc))
                        أرجع -ENOMEM;

                اكسر;
        }
#endif

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                إذا (!c->d_zstd)
                        أرجع -ENOMEM;
                اكسر;
#endif

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (صحيح) حجم(c->gzip)) != Z_OK)
                        أرجع -EIO;
                اكسر;
#endif

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        أرجع -EIO;
                اكسر;
#endif

        افتراضي:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        أرجع 0;
}

ساكن صحيح decompress_blob_to_fd_impl(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_size,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        صحيح r;

        أكد(src);
        أكد(fdt >= 0);

        إذا (type == COMPRESSION_NONE)
                أرجع -EOPNOTSUPP;

        إذا (src_size == 0)
                أرجع -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        إذا (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                إذا (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        إذا (src_size <= حجم(uint64_t))
                                أرجع -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        إذا (sz < skip)
                                أرجع -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                إذا (max_bytes == 0)
                        أرجع -ENOBUFS;

                _cleanup_free_ فراغ *buf = عدم;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                إذا (r < 0)
                        أرجع r;

                /* Check the result size again for safety. */
                إذا (size < skip)
                        أرجع -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                إذا (r < 0)
                        أرجع r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (مزدوج) size / src_size * 100);

                إذا (ret_size)
                        *ret_size = size - skip;
                أرجع 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = عدم;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        إذا (r < 0)
                أرجع r;

        بنية decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_تولق(c, src, src_size, decompress_stream_write_callback, &userdata);
        إذا (r < 0)
                أرجع r;

        /* Finalize */
        r = decompressor_تولق(c, /* data= */ عدم, /* size= */ 0, decompress_stream_write_callback, &userdata);
        إذا (r < 0)
                أرجع r;

        إذا (userdata.total_out < skip)
                أرجع -EBADMSG;

        إذا (userdata.sparse) {
                r = finalize_sparse(fdt);
                إذا (r < 0)
                        أرجع r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (مزدوج) userdata.total_out / src_size * 100);

        إذا (ret_size)
                *ret_size = userdata.total_out - skip;

        أرجع 0;
}

صحيح decompress_blob_to_fd_journal(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_size,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        صحيح r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        أرجع decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

صحيح decompress_blob_to_fd(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_size,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        صحيح r;

        r = dlopen_compress(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        أرجع decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

صحيح decompress_stream_full(
                Compression type,
                صحيح fdf, صحيح fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = عدم;
        _cleanup_free_ uint8_t *buf = عدم;
        uint64_t total_in = 0;
        صحيح r;

        أكد(fdf >= 0);
        أكد(fdt >= 0);

        إذا (type == COMPRESSION_NONE)
                أرجع -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        r = decompressor_new(&c, type, flags);
        إذا (r < 0)
                أرجع r;

        بنية decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        إذا (!buf)
                أرجع -ENOMEM;

        لكل (;;) {
                ssize_t n;

                n = اقرأ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                إذا (n < 0)
                        أرجع -errno;

                total_in += n;

                r = decompressor_تولق(c, buf, n, decompress_stream_write_callback, &userdata);
                إذا (r < 0)
                        أرجع r;

                إذا (n == 0)
                        اكسر;
        }

        إذا (total_in == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        إذا (userdata.sparse) {
                r = finalize_sparse(fdt);
                إذا (r < 0)
                        أرجع r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (مزدوج) userdata.total_out / total_in * 100);

        أرجع 0;
}

صحيح decompress_stream_by_filename(ثابت حرف *filename, صحيح fdf, صحيح fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        إذا (c == COMPRESSION_NONE)
                أرجع -EPROTONOSUPPORT;

        أرجع decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        إذا (!c)
                أرجع عدم;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                اكسر;
#endif

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4:
                إذا (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = عدم;
                        c->lz4_header = mfree(c->lz4_header);
                } وإلا {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = عدم;
                }
                اكسر;
#endif

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD:
                إذا (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = عدم;
                } وإلا {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = عدم;
                }
                اكسر;
#endif

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (c->encoding)
                        sym_deflateEnd(&c->gzip);
                وإلا
                        sym_inflateEnd(&c->gzip);
                اكسر;
#endif

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                وإلا
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                اكسر;
#endif

        افتراضي:
                اكسر;
        }

        أرجع mfree(c);
}

Compression compressor_type(ثابت Compressor *c) {
        أرجع c ? c->type : _COMPRESSION_INVALID;
}

صحيح decompressor_detect(Decompressor **decompressor, ثابت فراغ *data, size_t size) {
        صحيح r;

        أكد(decompressor);

        إذا (*decompressor)
                أرجع 1;

        إذا (size < COMPRESSION_MAGIC_BYTES_MAX)
                أرجع 0;

        أكد(data);

        Compression type = compression_detect_from_magic(data);
        إذا (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        إذا (r < 0)
                أرجع r;

        أرجع 1;
}

صحيح decompressor_force(Decompressor **decompressor, Compression type) {
        صحيح r;

        أكد(decompressor);
        أكد(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        *decompressor = compressor_free(*decompressor);
        أرجع decompressor_new(decompressor, type, /* flags= */ 0);
}

صحيح decompressor_force_off(Decompressor **decompressor) {
        أرجع decompressor_force(decompressor, COMPRESSION_NONE);
}

صحيح decompressor_تولق(Decompressor *c, ثابت فراغ *data, size_t size, DecompressorCallback callback, فراغ *userdata) {
        صحيح r;

        أكد(c);
        أكد(data || size == 0);
        أكد(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        إذا (c->encoding)
                أرجع -EINVAL;

        إذا (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                أرجع 0;

#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = عدم;
        إذا (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                إذا (!buffer)
                        أرجع -ENOMEM;
        }
#endif

        بدل (c->type) {

        حالة COMPRESSION_NONE:
                إذا (c->finished)
                        أرجع -EBADMSG;

                إذا (size == 0) {
                        c->finished = true;
                        أرجع 0;
                }

                r = callback(data, size, userdata);
                إذا (r < 0)
                        أرجع r;

                أرجع 0;

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:
                إذا (c->finished)
                        أرجع -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                لكل (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        إذا (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                أرجع -EBADMSG;

                        إذا (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        إذا (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (lzr == LZMA_STREAM_END) {
                                إذا (c->xz.avail_in > 0)
                                        أرجع -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                أرجع 0;
                        }

                        إذا (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                إذا (size == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#endif

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                ثابت uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                لكل (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, عدم);
                        إذا (sym_LZ4F_isError(rc))
                                أرجع -EBADMSG;

                        إذا (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                أرجع -EIO;

                        إذا (consumed > 0) {
                                أكد(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        إذا (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (src_remaining == 0) {
                                إذا (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        أرجع 0;
                                }

                                إذا (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        استمر;

                                إذا (size == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_inBuffer ماسوقن = {
                        .src =  (فراغ*) data,
                        .size = size,
                };

                c->finished = false;

                لكل (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &ماسوقن);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EBADMSG;

                        إذا (ماسوقن.pos > ماسوقن.size ||
                            output.pos > output.size)
                                أرجع -EIO;

                        إذا (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (ماسوقن.pos == ماسوقن.size) {
                                إذا (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        أرجع 0;
                                }

                                إذا (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        استمر;

                                إذا (size == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (size > UINT_MAX)
                        أرجع -EFBIG;

                c->gzip.next_in = (فراغ*) data;
                c->gzip.avail_in = size;

                لكل (;;) {
                        إذا (c->finished) {
                                إذا (sym_inflateReset(&c->gzip) != Z_OK)
                                        أرجع -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        صحيح zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        إذا (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                أرجع -EBADMSG;

                        إذا (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        إذا (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (zr == Z_STREAM_END) {
                                c->finished = true;

                                إذا (c->gzip.avail_in > 0)
                                        استمر; /* New stream may be pending. */

                                أرجع 0;
                        }

                        إذا (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                إذا (size == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#endif

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (size > UINT_MAX)
                        أرجع -EFBIG;

                c->bzip2.next_in = (حرف*) data;
                c->bzip2.avail_in = size;

                لكل (;;) {
                        إذا (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                إذا (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        أرجع -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (حرف*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        صحيح bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        إذا (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                أرجع -EBADMSG;

                        إذا (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        إذا (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                إذا (c->bzip2.avail_in > 0)
                                        استمر; /* New stream may be pending. */

                                أرجع 0;
                        }

                        إذا (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                إذا (size == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#endif

        افتراضي:
                assert_not_reached();
        }
}

صحيح compressor_new(Compressor **ret, Compression type) {
        صحيح r;

        أكد(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        إذا (!c)
                أرجع -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        بدل (type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                إذا (xzr != LZMA_OK)
                        أرجع -EIO;

                c->type = COMPRESSION_XZ;
                اكسر;
        }
#endif

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                إذا (sym_LZ4F_isError(rc))
                        أرجع -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                إذا (!c->lz4_header)
                        أرجع -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                إذا (sym_LZ4F_isError(c->lz4_header_size))
                        أرجع -EINVAL;

                اكسر;
        }
#endif

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                إذا (!c->c_zstd)
                        أرجع -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                إذا (sym_ZSTD_isError(z))
                        أرجع -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                إذا (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                اكسر;
#endif

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (صحيح) حجم(c->gzip));
                إذا (r != Z_OK)
                        أرجع -EIO;

                c->type = COMPRESSION_GZIP;
                اكسر;
#endif

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                إذا (r != BZ_OK)
                        أرجع -EIO;

                c->type = COMPRESSION_BZIP2;
                اكسر;
#endif

        حالة COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                اكسر;

        افتراضي:
                أرجع -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        أرجع 0;
}

#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
ساكن صحيح enlarge_buffer(فراغ **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        أكد(buffer);
        أكد(buffer_size);
        أكد(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        إذا (*buffer_allocated >= need)
                أرجع 0;

        إذا (!greedy_realloc(buffer, need, 1))
                أرجع -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        أرجع 1;
}
#endif

صحيح compressor_start(
                Compressor *c,
                ثابت فراغ *data,
                size_t size,
                فراغ **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحيح r;
#endif

        أكد(c);
        أكد(buffer);
        أكد(buffer_size);
        أكد(buffer_allocated);

        إذا (!c->encoding)
                أرجع -EINVAL;

        إذا (size == 0)
                أرجع 0;

        أكد(data);

        *buffer_size = 0;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                طالما (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        إذا (lzr != LZMA_OK)
                                أرجع -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                اكسر;
#endif

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                إذا (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        إذا (r < 0)
                                أرجع r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                إذا (r < 0)
                        أرجع r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, عدم);
                إذا (sym_LZ4F_isError(n))
                        أرجع -EIO;

                *buffer_size += n;
                اكسر;
        }
#endif

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_inBuffer ماسوقن = {
                        .src = data,
                        .size = size,
                };

                طالما (ماسوقن.pos < ماسوقن.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ماسوقن, ZSTD_e_continue);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EIO;

                        *buffer_size += output.pos;
                }

                اكسر;
        }
#endif

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (size > UINT_MAX)
                        أرجع -EFBIG;

                c->gzip.next_in = (فراغ*) data;
                c->gzip.avail_in = size;

                طالما (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        إذا (r != Z_OK)
                                أرجع -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                اكسر;
#endif

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (size > UINT_MAX)
                        أرجع -EFBIG;

                c->bzip2.next_in = (فراغ*) data;
                c->bzip2.avail_in = size;

                طالما (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (فراغ*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        إذا (r != BZ_RUN_OK)
                                أرجع -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                اكسر;
#endif

        حالة COMPRESSION_NONE:

                إذا (*buffer_allocated < size) {
                        فراغ *p;

                        p = realloc(*buffer, size);
                        إذا (!p)
                                أرجع -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                اكسر;

        افتراضي:
                أرجع -EOPNOTSUPP;
        }

        أرجع 0;
}

صحيح compressor_finish(Compressor *c, فراغ **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحيح r;
#endif

        أكد(c);
        أكد(buffer);
        أكد(buffer_size);
        أكد(buffer_allocated);

        إذا (!c->encoding)
                أرجع -EINVAL;

        *buffer_size = 0;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                افعل {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        إذا (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                أرجع -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } طالما (lzr != LZMA_STREAM_END);

                اكسر;
        }
#endif

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                إذا (r < 0)
                        أرجع r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, عدم);
                إذا (sym_LZ4F_isError(n))
                        أرجع -EIO;

                *buffer_size = n;
                اكسر;
        }
#endif

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_inBuffer ماسوقن = {};
                size_t res;

                افعل {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ماسوقن, ZSTD_e_end);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EIO;

                        *buffer_size += output.pos;
                } طالما (res != 0);

                اكسر;
        }
#endif

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                افعل {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        إذا (!IN_SET(r, Z_OK, Z_STREAM_END))
                                أرجع -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } طالما (r != Z_STREAM_END);

                اكسر;
#endif

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                افعل {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (فراغ*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        إذا (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                أرجع -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } طالما (r != BZ_STREAM_END);

                اكسر;
#endif

        حالة COMPRESSION_NONE:
                اكسر;

        افتراضي:
                أرجع -EOPNOTSUPP;
        }

        أرجع 0;
}
