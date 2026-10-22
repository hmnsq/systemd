/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <sys/stat.h>
#شامل <unistd.h>

#اگر HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#شامل <lzma.h>
#endif

#اگر HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#شامل <lz4.h>
#شامل <lz4frame.h>
#شامل <lz4hc.h>
#endif

#اگر HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#شامل <zstd.h>
#شامل <zstd_errors.h>
#endif

#اگر HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#شامل <zlib.h>
#endif

#اگر HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#شامل <bzlib.h>
#endif

#شامل "alloc-util.h"
#شامل "bitfield.h"
#شامل "compress.h"
#شامل "dlfcn-util.h"
#شامل "io-util.h"
#شامل "log.h"
#شامل "string-table.h"
#شامل "unaligned.h"

#اگر HAVE_XZ
ایستا DLSYM_PROTOTYPE(lzma_auto_decoder) = پوچ;
ایستا DLSYM_PROTOTYPE(lzma_code) = پوچ;
ایستا DLSYM_PROTOTYPE(lzma_easy_encoder) = پوچ;
ایستا DLSYM_PROTOTYPE(lzma_end) = پوچ;
ایستا DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = پوچ;
ایستا DLSYM_PROTOTYPE(lzma_stream_decoder) = پوچ;
ایستا DLSYM_PROTOTYPE(lzma_lzma_preset) = پوچ;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ایستا درونخطی تهی lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#اگر HAVE_LZ4
ایستا DLSYM_PROTOTYPE(LZ4F_compressBegin) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_compressBound) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_compressEnd) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_compressUpdate) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_decompress) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4F_isError) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4_compress_HC) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4_compress_default) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4_decompress_safe) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = پوچ;
ایستا DLSYM_PROTOTYPE(LZ4_version_عدد) = پوچ;

ایستا ثابت LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#اگر HAVE_ZSTD
ایستا DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_compress) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_compressStream2) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_createCCtx) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_createDCtx) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_decompressStream) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_freeCCtx) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_freeDCtx) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_getErrorCode) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_getErrorName) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = پوچ;
ایستا DLSYM_PROTOTYPE(ZSTD_isError) = پوچ;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, پوچ);

ایستا صحیح zstd_ret_to_errno(size_t ret) {
        انتخاب (sym_ZSTD_getErrorCode(ret)) {
        حالت ZSTD_error_dstSize_tooSmall:
                بازگشت -ENOBUFS;
        حالت ZSTD_error_memory_allocation:
                بازگشت -ENOMEM;
        پیشفرض:
                بازگشت -EBADMSG;
        }
}
#endif

#اگر HAVE_ZLIB
ایستا DLSYM_PROTOTYPE(deflateInit2_) = پوچ;
ایستا DLSYM_PROTOTYPE(deflate) = پوچ;
ایستا DLSYM_PROTOTYPE(deflateEnd) = پوچ;
ایستا DLSYM_PROTOTYPE(inflateInit2_) = پوچ;
ایستا DLSYM_PROTOTYPE(inflate) = پوچ;
ایستا DLSYM_PROTOTYPE(inflateEnd) = پوچ;
ایستا DLSYM_PROTOTYPE(inflateReset) = پوچ;

ایستا درونخطی تهی deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

ایستا درونخطی تهی inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#اگر HAVE_BZIP2
ایستا DLSYM_PROTOTYPE(BZ2_bzCompressInit) = پوچ;
ایستا DLSYM_PROTOTYPE(BZ2_bzCompress) = پوچ;
ایستا DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = پوچ;
ایستا DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = پوچ;
ایستا DLSYM_PROTOTYPE(BZ2_bzDecompress) = پوچ;
ایستا DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = پوچ;

ایستا درونخطی تهی BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

ایستا درونخطی تهی BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
ساختار Compressor {
        Compression type;
        bool encoding;
        bool finished;
        اجتماع {
#اگر HAVE_XZ
                lzma_stream xz;
#endif
#اگر HAVE_LZ4
                ساختار {
                        LZ4F_compressionContext_t c_lz4;
                        تهی *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#اگر HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#اگر HAVE_ZLIB
                z_stream gzip;
#endif
#اگر HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, اندازه(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

ایستا ثابت نویسه* ثابت compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

ایستا ثابت نویسه* ثابت compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

ایستا ثابت نویسه* ثابت compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(ثابت نویسه *s) {
        Compression c;

        أكد(s);

        c = compression_from_string(s);
        اگر (c >= 0)
                بازگشت c;

        بازگشت compression_uppercase_from_string(s);
}

Compression compression_from_filename(ثابت نویسه *filename) {
        Compression c;
        ثابت نویسه *e;

        أكد(filename);

        e = strrchr(filename, '.');
        اگر (!e)
                بازگشت COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        اگر (c < 0)
                بازگشت COMPRESSION_NONE;

        بازگشت c;
}

bool compression_supported_journal(Compression c) {
        ایستا ثابت unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        بازگشت BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        ایستا ثابت unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        بازگشت BIT_SET(supported, c);
}

Compression compression_detect_from_magic(ثابت uint8_t data[ایستا COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        اگر (memcmp(data, (ثابت uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                بازگشت COMPRESSION_GZIP;
        اگر (memcmp(data, (ثابت uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                بازگشت COMPRESSION_XZ;
        اگر (memcmp(data, (ثابت uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                بازگشت COMPRESSION_ZSTD;
        اگر (memcmp(data, (ثابت uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                بازگشت COMPRESSION_LZ4;
        اگر (memcmp(data, (ثابت uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                بازگشت COMPRESSION_BZIP2;

        بازگشت _COMPRESSION_INVALID;
}

صحیح dlopen_xz(صحیح log_level) {
#اگر HAVE_XZ
        ایستا تهی *lzma_dl = پوچ;

        LIBLZMA_NOTE(suggested);

        بازگشت dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#وگرنه
        بازگشت log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

صحیح dlopen_lz4(صحیح log_level) {
#اگر HAVE_LZ4
        ایستا تهی *lz4_dl = پوچ;

        LIBLZ4_NOTE(suggested);

        بازگشت dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_عدد));
#وگرنه
        بازگشت log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

صحیح dlopen_zstd(صحیح log_level) {
#اگر HAVE_ZSTD
        ایستا تهی *zstd_dl = پوچ;

        LIBZSTD_NOTE(suggested);

        بازگشت dlopen_many_sym_or_warn(
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
#وگرنه
        بازگشت log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

صحیح dlopen_zlib(صحیح log_level) {
#اگر HAVE_ZLIB
        ایستا تهی *zlib_dl = پوچ;

        LIBZ_NOTE(suggested);

        بازگشت dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#وگرنه
        بازگشت log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

صحیح dlopen_bzip2(صحیح log_level) {
#اگر HAVE_BZIP2
        ایستا تهی *bzip2_dl = پوچ;

        LIBBZ2_NOTE(suggested);

        بازگشت dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#وگرنه
        بازگشت log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

صحیح dlopen_compress_journal(Compression c, صحیح log_level) {
        انتخاب (c) {
        حالت COMPRESSION_NONE:
                بازگشت 0;
        حالت COMPRESSION_XZ:
                بازگشت dlopen_xz(log_level);
        حالت COMPRESSION_LZ4:
                بازگشت dlopen_lz4(log_level);
        حالت COMPRESSION_ZSTD:
                بازگشت dlopen_zstd(log_level);
        پیشفرض:
                بازگشت -EOPNOTSUPP;
        }
}

صحیح dlopen_compress(Compression c, صحیح log_level) {
        انتخاب (c) {
        حالت COMPRESSION_GZIP:
                بازگشت dlopen_zlib(log_level);
        حالت COMPRESSION_BZIP2:
                بازگشت dlopen_bzip2(log_level);
        پیشفرض:
                بازگشت dlopen_compress_journal(c, log_level);
        }
}

ایستا صحیح compress_blob_xz(
                ثابت تهی *src,
                uint64_t src_size,
                تهی *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                صحیح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#اگر HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, پوچ, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, پوچ }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        صحیح r;

        r = dlopen_xz(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                اگر (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        بازگشت -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        اگر (src_size < 80)
                بازگشت -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ پوچ,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        اگر (ret != LZMA_OK)
                بازگشت -ENOBUFS;

        *dst_size = out_pos;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح compress_blob_lz4(
                ثابت تهی *src,
                uint64_t src_size,
                تهی *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                صحیح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#اگر HAVE_LZ4
        صحیح r;

        r = dlopen_lz4(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        اگر (src_size < 9)
                بازگشت -ENOBUFS;

        اگر (src_size > INT_MAX)
                بازگشت -EFBIG;
        اگر (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        اگر (level <= 0)
                r = sym_LZ4_compress_default(src, (نویسه*)dst + 8, src_size, (صحیح) dst_alloc_size - 8);
        وگرنه
                r = sym_LZ4_compress_HC(src, (نویسه*)dst + 8, src_size, (صحیح) dst_alloc_size - 8, level);
        اگر (r <= 0)
                بازگشت -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح compress_blob_zstd(
                ثابت تهی *src,
                uint64_t src_size,
                تهی *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                صحیح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#اگر HAVE_ZSTD
        size_t k;
        صحیح r;

        r = dlopen_zstd(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        اگر (sym_ZSTD_isError(k))
                بازگشت zstd_ret_to_errno(k);

        *dst_size = k;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح compress_blob_gzip(ثابت تهی *src, uint64_t src_size,
                       تهی *dst, size_t dst_alloc_size, size_t *dst_size, صحیح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#اگر HAVE_ZLIB
        صحیح r;

        r = dlopen_zlib(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size > UINT_MAX)
                بازگشت -EFBIG;
        اگر (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (صحیح) اندازه(s));
        اگر (r != Z_OK)
                بازگشت -ENOMEM;

        s.next_in = (تهی*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        اگر (r != Z_STREAM_END)
                بازگشت -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح compress_blob_bzip2(
                ثابت تهی *src, uint64_t src_size,
                تهی *dst, size_t dst_alloc_size, size_t *dst_size, صحیح level) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_alloc_size > 0);
        أكد(dst_size);

#اگر HAVE_BZIP2
        صحیح r;

        r = dlopen_bzip2(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size > UINT_MAX)
                بازگشت -EFBIG;
        اگر (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        اگر (r != BZ_OK)
                بازگشت -ENOMEM;

        s.next_in = (نویسه*) src;
        s.avail_in = src_size;
        s.next_out = (نویسه*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        اگر (r != BZ_STREAM_END)
                بازگشت -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

صحیح compress_blob_journal(
                Compression compression,
                ثابت تهی *src, uint64_t src_size,
                تهی *dst, size_t dst_alloc_size, size_t *dst_size, صحیح level) {

        انتخاب (compression) {
        حالت COMPRESSION_XZ:
                بازگشت compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        حالت COMPRESSION_LZ4:
                بازگشت compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        حالت COMPRESSION_ZSTD:
                بازگشت compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        پیشفرض:
                بازگشت -EOPNOTSUPP;
        }
}

صحیح compress_blob(
                Compression compression,
                ثابت تهی *src, uint64_t src_size,
                تهی *dst, size_t dst_alloc_size, size_t *dst_size, صحیح level) {

        انتخاب (compression) {
        حالت COMPRESSION_GZIP:
                بازگشت compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        حالت COMPRESSION_BZIP2:
                بازگشت compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        پیشفرض:
                بازگشت compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

ایستا صحیح decompress_blob_xz(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#اگر HAVE_XZ
        صحیح r;

        r = dlopen_xz(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        اگر (ret != LZMA_OK)
                بازگشت -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        اگر (!greedy_realloc(dst, space, 1))
                بازگشت -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        برای (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                اگر (ret == LZMA_STREAM_END)
                        بشکن;
                اگر (ret != LZMA_OK)
                        بازگشت -ENOMEM;

                اگر (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        بشکن;
                اگر (dst_max > 0 && space == dst_max)
                        بازگشت -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                أكد(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                اگر (!greedy_realloc(dst, space, 1))
                        بازگشت -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح decompress_blob_lz4(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#اگر HAVE_LZ4
        نویسه* out;
        صحیح r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size <= 8)
                بازگشت -EBADMSG;

        اگر (src_size - 8 > INT_MAX)
                بازگشت -EFBIG;

        size = unaligned_read_le64(src);
        اگر (size < 0 || (unsigned) size != unaligned_read_le64(src))
                بازگشت -EFBIG;
        اگر (dst_max > 0 && (size_t) size > dst_max)
                بازگشت -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        اگر (!out)
                بازگشت -ENOMEM;

        r = sym_LZ4_decompress_safe((نویسه*)src + 8, out, src_size - 8, size);
        اگر (r < 0 || r != size)
                بازگشت -EBADMSG;

        *dst_size = size;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(تهی) {
#اگر HAVE_ZSTD
        اگر (dlopen_zstd(LOG_DEBUG) < 0)
                بازگشت 0;

        بازگشت sym_ZSTD_DStreamOutSize();
#وگرنه
        بازگشت 0;
#endif
}

ایستا صحیح decompress_blob_zstd(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#اگر HAVE_ZSTD
        صحیح r;

        r = dlopen_zstd(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        اگر (size == ZSTD_CONTENTSIZE_ERROR)
                بازگشت -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        اگر (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        وگرنه {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        اگر (!greedy_realloc(dst, space, 1))
                بازگشت -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        اگر (!dctx)
                بازگشت -ENOMEM;

        ZSTD_inBuffer درون_داد = {
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

        برای (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &درون_داد);
                اگر (sym_ZSTD_isError(k))
                        بازگشت log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                اگر (output.pos >= cap)
                        بشکن;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                اگر (k == 0 && درون_داد.pos >= درون_داد.size)
                        بشکن;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                اگر (output.pos == output.size) {
                        اگر (space > SIZE_MAX / 2)
                                بازگشت -E2BIG;
                        space = MIN(2 * space, cap);
                        اگر (!greedy_realloc(dst, space, 1))
                                بازگشت -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        ادامه;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                اگر (درون_داد.pos >= درون_داد.size)
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح decompress_blob_gzip(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#اگر HAVE_ZLIB
        صحیح r;

        r = dlopen_zlib(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size > UINT_MAX)
                بازگشت -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (صحیح) اندازه(s));
        اگر (r != Z_OK)
                بازگشت -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        اگر (!greedy_realloc(dst, space, 1))
                بازگشت -ENOMEM;

        s.next_in = (تهی*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        برای (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                اگر (r == Z_STREAM_END)
                        بشکن;
                اگر (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        بازگشت -EBADMSG;

                اگر (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        بشکن;
                اگر (dst_max > 0 && space == dst_max)
                        بازگشت -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                اگر (!greedy_realloc(dst, space, 1))
                        بازگشت -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح decompress_blob_bzip2(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **dst,
                size_t *dst_size,
                size_t dst_max) {

        أكد(src);
        أكد(src_size > 0);
        أكد(dst);
        أكد(dst_size);

#اگر HAVE_BZIP2
        صحیح r;

        r = dlopen_bzip2(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size > UINT_MAX)
                بازگشت -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        اگر (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                بازگشت -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        اگر (!greedy_realloc(dst, space, 1))
                بازگشت -ENOMEM;

        s.next_in = (نویسه*) src;
        s.avail_in = src_size;
        s.next_out = (نویسه*) *dst;
        s.avail_out = space;

        برای (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                اگر (r == BZ_STREAM_END)
                        بشکن;
                اگر (r != BZ_OK)
                        بازگشت -EBADMSG;

                اگر (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        بشکن;
                اگر (dst_max > 0 && space == dst_max)
                        بازگشت -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                اگر (!greedy_realloc(dst, space, 1))
                        بازگشت -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (نویسه*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

صحیح decompress_blob_journal(
                Compression compression,
                ثابت تهی *src,
                uint64_t src_size,
                تهی **dst,
                size_t *dst_size,
                size_t dst_max) {

        انتخاب (compression) {
        حالت COMPRESSION_XZ:
                بازگشت decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        حالت COMPRESSION_LZ4:
                بازگشت decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        حالت COMPRESSION_ZSTD:
                بازگشت decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        پیشفرض:
                بازگشت -EPROTONOSUPPORT;
        }
}

صحیح decompress_blob(
                Compression compression,
                ثابت تهی *src,
                uint64_t src_size,
                تهی **dst,
                size_t *dst_size,
                size_t dst_max) {

        انتخاب (compression) {
        حالت COMPRESSION_GZIP:
                بازگشت decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        حالت COMPRESSION_BZIP2:
                بازگشت decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        پیشفرض:
                بازگشت decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

صحیح decompress_zlib_raw(
                ثابت تهی *src,
                uint64_t src_size,
                تهی *dst,
                size_t dst_size,
                صحیح wbits) {

#اگر HAVE_ZLIB
        صحیح r;

        r = dlopen_zlib(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size > UINT_MAX)
                بازگشت -EFBIG;
        اگر (dst_size > UINT_MAX)
                بازگشت -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (تهی*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (صحیح) اندازه(s));
        اگر (r != Z_OK)
                بازگشت -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        اگر (r != Z_STREAM_END || produced != dst_size)
                بازگشت -EBADMSG;

        بازگشت 0;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح decompress_startswith_xz(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **buffer,
                ثابت تهی *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#اگر HAVE_XZ
        صحیح r;

        r = dlopen_xz(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        اگر (ret != LZMA_OK)
                بازگشت -EBADMSG;

        اگر (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                بازگشت -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        برای (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                اگر (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        بازگشت -EBADMSG;

                اگر (allocated - s.avail_out >= prefix_len + 1)
                        بازگشت memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;

                اگر (ret == LZMA_STREAM_END)
                        بازگشت 0;

                s.avail_out += allocated;

                اگر (!(greedy_realloc(buffer, allocated * 2, 1)))
                        بازگشت -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح decompress_startswith_lz4(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **buffer,
                ثابت تهی *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#اگر HAVE_LZ4
        size_t allocated;
        صحیح r;

        r = dlopen_lz4(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size <= 8)
                بازگشت -EBADMSG;

        اگر (src_size - 8 > INT_MAX)
                بازگشت -EFBIG;

        اگر (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                بازگشت -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (نویسه*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        اگر (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                اگر (sym_LZ4_version_عدد() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        بازگشت 0;

                اگر (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        اگر (memcmp(*buffer, prefix, r) != 0)
                                بازگشت 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                اگر (r < 0)
                        بازگشت r;

                اگر (size < prefix_len + 1)
                        بازگشت 0;
        }

        بازگشت memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح decompress_startswith_zstd(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **buffer,
                ثابت تهی *prefix,
                size_t prefix_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#اگر HAVE_ZSTD
        صحیح r;

        r = dlopen_zstd(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        اگر (size == ZSTD_CONTENTSIZE_ERROR)
                بازگشت -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        اگر (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                بازگشت 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        اگر (!dctx)
                بازگشت -ENOMEM;

        اگر (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                بازگشت -ENOMEM;

        ZSTD_inBuffer درون_داد = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &درون_داد);
        اگر (sym_ZSTD_isError(k))
                بازگشت log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        اگر (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                اگر (k != 0)
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                بازگشت 0; /* Decompressed text too short to match the prefix and extra */
        }

        بازگشت memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح decompress_startswith_gzip(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **buffer,
                ثابت تهی *prefix,
                size_t prefix_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#اگر HAVE_ZLIB
        صحیح r;

        r = dlopen_zlib(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size > UINT_MAX)
                بازگشت -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (صحیح) اندازه(s));
        اگر (r != Z_OK)
                بازگشت -EBADMSG;

        اگر (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                بازگشت -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (تهی*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        برای (;;) {
                r = sym_inflate(&s, Z_FINISH);

                اگر (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        بازگشت -EBADMSG;

                اگر (allocated - s.avail_out >= prefix_len + 1)
                        بازگشت memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;

                اگر (r == Z_STREAM_END)
                        بازگشت 0;

                اگر (s.avail_out > 0)
                        بازگشت -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                اگر (!(greedy_realloc(buffer, allocated * 2, 1)))
                        بازگشت -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

ایستا صحیح decompress_startswith_bzip2(
                ثابت تهی *src,
                uint64_t src_size,
                تهی **buffer,
                ثابت تهی *prefix,
                size_t prefix_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_size > 0);
        أكد(buffer);
        أكد(prefix);

#اگر HAVE_BZIP2
        صحیح r;

        r = dlopen_bzip2(LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        اگر (src_size > UINT_MAX)
                بازگشت -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        اگر (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                بازگشت -EIO;

        اگر (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                بازگشت -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (نویسه*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        برای (;;) {
                r = sym_BZ2_bzDecompress(&s);

                اگر (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        بازگشت -EBADMSG;

                اگر (allocated - s.avail_out >= prefix_len + 1)
                        بازگشت memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ثابت uint8_t*) *buffer)[prefix_len] == extra;

                اگر (r == BZ_STREAM_END)
                        بازگشت 0;

                اگر (s.avail_out > 0)
                        بازگشت -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                اگر (!(greedy_realloc(buffer, allocated * 2, 1)))
                        بازگشت -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (نویسه*) *buffer + used;
        }
#وگرنه
        بازگشت -EPROTONOSUPPORT;
#endif
}

صحیح decompress_startswith_journal(
                Compression compression,
                ثابت تهی *src, uint64_t src_size,
                تهی **buffer,
                ثابت تهی *prefix, size_t prefix_len,
                uint8_t extra) {

        انتخاب (compression) {
        حالت COMPRESSION_XZ:
                بازگشت decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        حالت COMPRESSION_LZ4:
                بازگشت decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        حالت COMPRESSION_ZSTD:
                بازگشت decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        پیشفرض:
                بازگشت -EOPNOTSUPP;
        }
}

صحیح decompress_startswith(
                Compression compression,
                ثابت تهی *src, uint64_t src_size,
                تهی **buffer,
                ثابت تهی *prefix, size_t prefix_len,
                uint8_t extra) {

        انتخاب (compression) {
        حالت COMPRESSION_GZIP:
                بازگشت decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        حالت COMPRESSION_BZIP2:
                بازگشت decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        پیشفرض:
                بازگشت decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

صحیح compress_stream(
                Compression type,
                صحیح fdf, صحیح fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = پوچ;
        _cleanup_free_ تهی *buf = پوچ;
        _cleanup_free_ uint8_t *درون_داد = پوچ;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        صحیح r;

        أكد(fdf >= 0);
        أكد(fdt >= 0);

        r = compressor_new(&c, type);
        اگر (r < 0)
                بازگشت r;

        درون_داد = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        اگر (!درون_داد)
                بازگشت -ENOMEM;

        برای (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                اگر (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = اقرأ(fdf, درون_داد, m);
                اگر (n < 0)
                        بازگشت -errno;

                اگر (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        اگر (r < 0)
                                بازگشت r;

                        اگر (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                اگر (r < 0)
                                        بازگشت r;
                                total_out += buf_size;
                        }
                        بشکن;
                }

                total_in += n;
                اگر (max_bytes != UINT64_MAX) {
                        أكد(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, درون_داد, n, &buf, &buf_size, &buf_alloc);
                اگر (r < 0)
                        بازگشت r;

                اگر (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        اگر (r < 0)
                                بازگشت r;
                        total_out += buf_size;
                }
        }

        اگر (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        اگر (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        وگرنه
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (دوگانه) total_out / total_in * 100);

        بازگشت 0;
}

ایستا صحیح should_sparse(صحیح fd) {
        ساختار stat st;

        أكد(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        اگر (fstat(fd, &st) < 0)
                بازگشت -errno;

        اگر (!S_ISREG(st.st_mode))
                بازگشت false;

        صحیح flags = fcntl(fd, F_GETFL);
        اگر (flags < 0)
                بازگشت -errno;

        اگر (FLAGS_SET(flags, O_APPEND))
                بازگشت false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        اگر (pos < 0)
                بازگشت -errno;

        بازگشت pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
ایستا صحیح finalize_sparse(صحیح fd) {
        off_t pos;

        أكد(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        اگر (pos < 0)
                بازگشت -errno;

        اگر (ftruncate(fd, pos) < 0)
                بازگشت -errno;

        بازگشت 0;
}

/* Common helper for decompress_stream_*() wrappers */

ساختار decompress_stream_userdata {
        صحیح fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ایستا صحیح decompress_stream_write_callback(ثابت تهی *data, size_t size, تهی *userdata) {
        ساختار decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        اگر (u->max_bytes != UINT64_MAX) {
                اگر (u->max_bytes < size)
                        بازگشت -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        اگر (u->skip > 0) {
                اگر (u->skip >= size) {
                        u->skip -= size;
                        بازگشت 0;
                }

                data = (ثابت uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        اگر (size == 0)
                بازگشت 0;

        اگر (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                اگر (k < 0)
                        بازگشت (صحیح) k;
                بازگشت 0;
        }

        بازگشت loop_write(u->fd, data, size);
}

ایستا صحیح decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        أكد(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        اگر (!c)
                بازگشت -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        انتخاب (type) {
        حالت COMPRESSION_NONE:
                بشکن;

#اگر HAVE_XZ
        حالت COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                اگر (xz_ret != LZMA_OK)
                        بازگشت -EIO;
                بشکن;
        }
#endif

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                اگر (sym_LZ4F_isError(rc))
                        بازگشت -ENOMEM;

                بشکن;
        }
#endif

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                اگر (!c->d_zstd)
                        بازگشت -ENOMEM;
                بشکن;
#endif

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                اگر (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (صحیح) اندازه(c->gzip)) != Z_OK)
                        بازگشت -EIO;
                بشکن;
#endif

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                اگر (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        بازگشت -EIO;
                بشکن;
#endif

        پیشفرض:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        بازگشت 0;
}

ایستا صحیح decompress_blob_to_fd_impl(
                Compression type,
                ثابت تهی *src,
                uint64_t src_size,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        صحیح r;

        أكد(src);
        أكد(fdt >= 0);

        اگر (type == COMPRESSION_NONE)
                بازگشت -EOPNOTSUPP;

        اگر (src_size == 0)
                بازگشت -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        اگر (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                اگر (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        اگر (src_size <= اندازه(uint64_t))
                                بازگشت -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        اگر (sz < skip)
                                بازگشت -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                اگر (max_bytes == 0)
                        بازگشت -ENOBUFS;

                _cleanup_free_ تهی *buf = پوچ;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                اگر (r < 0)
                        بازگشت r;

                /* Check the result size again for safety. */
                اگر (size < skip)
                        بازگشت -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                اگر (r < 0)
                        بازگشت r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (دوگانه) size / src_size * 100);

                اگر (ret_size)
                        *ret_size = size - skip;
                بازگشت 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = پوچ;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        اگر (r < 0)
                بازگشت r;

        ساختار decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_افزودن(c, src, src_size, decompress_stream_write_callback, &userdata);
        اگر (r < 0)
                بازگشت r;

        /* Finalize */
        r = decompressor_افزودن(c, /* data= */ پوچ, /* size= */ 0, decompress_stream_write_callback, &userdata);
        اگر (r < 0)
                بازگشت r;

        اگر (userdata.total_out < skip)
                بازگشت -EBADMSG;

        اگر (userdata.sparse) {
                r = finalize_sparse(fdt);
                اگر (r < 0)
                        بازگشت r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (دوگانه) userdata.total_out / src_size * 100);

        اگر (ret_size)
                *ret_size = userdata.total_out - skip;

        بازگشت 0;
}

صحیح decompress_blob_to_fd_journal(
                Compression type,
                ثابت تهی *src,
                uint64_t src_size,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        صحیح r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        بازگشت decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

صحیح decompress_blob_to_fd(
                Compression type,
                ثابت تهی *src,
                uint64_t src_size,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        صحیح r;

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        بازگشت decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

صحیح decompress_stream_full(
                Compression type,
                صحیح fdf, صحیح fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = پوچ;
        _cleanup_free_ uint8_t *buf = پوچ;
        uint64_t total_in = 0;
        صحیح r;

        أكد(fdf >= 0);
        أكد(fdt >= 0);

        اگر (type == COMPRESSION_NONE)
                بازگشت -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        r = decompressor_new(&c, type, flags);
        اگر (r < 0)
                بازگشت r;

        ساختار decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        اگر (!buf)
                بازگشت -ENOMEM;

        برای (;;) {
                ssize_t n;

                n = اقرأ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                اگر (n < 0)
                        بازگشت -errno;

                total_in += n;

                r = decompressor_افزودن(c, buf, n, decompress_stream_write_callback, &userdata);
                اگر (r < 0)
                        بازگشت r;

                اگر (n == 0)
                        بشکن;
        }

        اگر (total_in == 0)
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        اگر (userdata.sparse) {
                r = finalize_sparse(fdt);
                اگر (r < 0)
                        بازگشت r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (دوگانه) userdata.total_out / total_in * 100);

        بازگشت 0;
}

صحیح decompress_stream_by_filename(ثابت نویسه *filename, صحیح fdf, صحیح fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        اگر (c == COMPRESSION_NONE)
                بازگشت -EPROTONOSUPPORT;

        بازگشت decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        اگر (!c)
                بازگشت پوچ;

        انتخاب (c->type) {

#اگر HAVE_XZ
        حالت COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                بشکن;
#endif

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4:
                اگر (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = پوچ;
                        c->lz4_header = mfree(c->lz4_header);
                } وگرنه {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = پوچ;
                }
                بشکن;
#endif

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD:
                اگر (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = پوچ;
                } وگرنه {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = پوچ;
                }
                بشکن;
#endif

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                اگر (c->encoding)
                        sym_deflateEnd(&c->gzip);
                وگرنه
                        sym_inflateEnd(&c->gzip);
                بشکن;
#endif

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                اگر (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                وگرنه
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                بشکن;
#endif

        پیشفرض:
                بشکن;
        }

        بازگشت mfree(c);
}

Compression compressor_type(ثابت Compressor *c) {
        بازگشت c ? c->type : _COMPRESSION_INVALID;
}

صحیح decompressor_detect(Decompressor **decompressor, ثابت تهی *data, size_t size) {
        صحیح r;

        أكد(decompressor);

        اگر (*decompressor)
                بازگشت 1;

        اگر (size < COMPRESSION_MAGIC_BYTES_MAX)
                بازگشت 0;

        أكد(data);

        Compression type = compression_detect_from_magic(data);
        اگر (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        اگر (r < 0)
                بازگشت r;

        بازگشت 1;
}

صحیح decompressor_force(Decompressor **decompressor, Compression type) {
        صحیح r;

        أكد(decompressor);
        أكد(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        *decompressor = compressor_free(*decompressor);
        بازگشت decompressor_new(decompressor, type, /* flags= */ 0);
}

صحیح decompressor_force_off(Decompressor **decompressor) {
        بازگشت decompressor_force(decompressor, COMPRESSION_NONE);
}

صحیح decompressor_افزودن(Decompressor *c, ثابت تهی *data, size_t size, DecompressorCallback callback, تهی *userdata) {
        صحیح r;

        أكد(c);
        أكد(data || size == 0);
        أكد(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        اگر (c->encoding)
                بازگشت -EINVAL;

        اگر (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                بازگشت 0;

#اگر HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = پوچ;
        اگر (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                اگر (!buffer)
                        بازگشت -ENOMEM;
        }
#endif

        انتخاب (c->type) {

        حالت COMPRESSION_NONE:
                اگر (c->finished)
                        بازگشت -EBADMSG;

                اگر (size == 0) {
                        c->finished = true;
                        بازگشت 0;
                }

                r = callback(data, size, userdata);
                اگر (r < 0)
                        بازگشت r;

                بازگشت 0;

#اگر HAVE_XZ
        حالت COMPRESSION_XZ:
                اگر (c->finished)
                        بازگشت -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                برای (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        اگر (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                بازگشت -EBADMSG;

                        اگر (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                بازگشت -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        اگر (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                اگر (r < 0)
                                        بازگشت r;
                        }

                        اگر (lzr == LZMA_STREAM_END) {
                                اگر (c->xz.avail_in > 0)
                                        بازگشت -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                بازگشت 0;
                        }

                        اگر (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                اگر (size == 0)
                                        بازگشت -EBADMSG; /* truncated */

                                بازگشت 0; /* No progress possible with the current input. */
                        }
                }
#endif

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                ثابت uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                برای (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, پوچ);
                        اگر (sym_LZ4F_isError(rc))
                                بازگشت -EBADMSG;

                        اگر (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                بازگشت -EIO;

                        اگر (consumed > 0) {
                                أكد(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        اگر (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                اگر (r < 0)
                                        بازگشت r;
                        }

                        اگر (src_remaining == 0) {
                                اگر (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        بازگشت 0;
                                }

                                اگر (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ادامه;

                                اگر (size == 0)
                                        بازگشت -EBADMSG; /* truncated */

                                بازگشت 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD: {
                ZSTD_inBuffer درون_داد = {
                        .src =  (تهی*) data,
                        .size = size,
                };

                c->finished = false;

                برای (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &درون_داد);
                        اگر (sym_ZSTD_isError(res))
                                بازگشت -EBADMSG;

                        اگر (درون_داد.pos > درون_داد.size ||
                            output.pos > output.size)
                                بازگشت -EIO;

                        اگر (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                اگر (r < 0)
                                        بازگشت r;
                        }

                        اگر (درون_داد.pos == درون_داد.size) {
                                اگر (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        بازگشت 0;
                                }

                                اگر (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ادامه;

                                اگر (size == 0)
                                        بازگشت -EBADMSG; /* truncated */

                                بازگشت 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                اگر (size > UINT_MAX)
                        بازگشت -EFBIG;

                c->gzip.next_in = (تهی*) data;
                c->gzip.avail_in = size;

                برای (;;) {
                        اگر (c->finished) {
                                اگر (sym_inflateReset(&c->gzip) != Z_OK)
                                        بازگشت -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        صحیح zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        اگر (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                بازگشت -EBADMSG;

                        اگر (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                بازگشت -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        اگر (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                اگر (r < 0)
                                        بازگشت r;
                        }

                        اگر (zr == Z_STREAM_END) {
                                c->finished = true;

                                اگر (c->gzip.avail_in > 0)
                                        ادامه; /* New stream may be pending. */

                                بازگشت 0;
                        }

                        اگر (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                اگر (size == 0)
                                        بازگشت -EBADMSG; /* truncated */

                                بازگشت 0; /* No progress possible with the current input. */
                        }
                }
#endif

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                اگر (size > UINT_MAX)
                        بازگشت -EFBIG;

                c->bzip2.next_in = (نویسه*) data;
                c->bzip2.avail_in = size;

                برای (;;) {
                        اگر (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                اگر (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        بازگشت -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (نویسه*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        صحیح bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        اگر (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                بازگشت -EBADMSG;

                        اگر (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                بازگشت -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        اگر (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                اگر (r < 0)
                                        بازگشت r;
                        }

                        اگر (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                اگر (c->bzip2.avail_in > 0)
                                        ادامه; /* New stream may be pending. */

                                بازگشت 0;
                        }

                        اگر (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                اگر (size == 0)
                                        بازگشت -EBADMSG; /* truncated */

                                بازگشت 0; /* No progress possible with the current input. */
                        }
                }
#endif

        پیشفرض:
                assert_not_reached();
        }
}

صحیح compressor_new(Compressor **ret, Compression type) {
        صحیح r;

        أكد(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                بازگشت r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        اگر (!c)
                بازگشت -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        انتخاب (type) {

#اگر HAVE_XZ
        حالت COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                اگر (xzr != LZMA_OK)
                        بازگشت -EIO;

                c->type = COMPRESSION_XZ;
                بشکن;
        }
#endif

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                اگر (sym_LZ4F_isError(rc))
                        بازگشت -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                اگر (!c->lz4_header)
                        بازگشت -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                اگر (sym_LZ4F_isError(c->lz4_header_size))
                        بازگشت -EINVAL;

                بشکن;
        }
#endif

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                اگر (!c->c_zstd)
                        بازگشت -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                اگر (sym_ZSTD_isError(z))
                        بازگشت -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                اگر (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                بشکن;
#endif

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (صحیح) اندازه(c->gzip));
                اگر (r != Z_OK)
                        بازگشت -EIO;

                c->type = COMPRESSION_GZIP;
                بشکن;
#endif

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                اگر (r != BZ_OK)
                        بازگشت -EIO;

                c->type = COMPRESSION_BZIP2;
                بشکن;
#endif

        حالت COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                بشکن;

        پیشفرض:
                بازگشت -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        بازگشت 0;
}

#اگر HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
ایستا صحیح enlarge_buffer(تهی **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        أكد(buffer);
        أكد(buffer_size);
        أكد(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        اگر (*buffer_allocated >= need)
                بازگشت 0;

        اگر (!greedy_realloc(buffer, need, 1))
                بازگشت -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        بازگشت 1;
}
#endif

صحیح compressor_start(
                Compressor *c,
                ثابت تهی *data,
                size_t size,
                تهی **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#اگر HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحیح r;
#endif

        أكد(c);
        أكد(buffer);
        أكد(buffer_size);
        أكد(buffer_allocated);

        اگر (!c->encoding)
                بازگشت -EINVAL;

        اگر (size == 0)
                بازگشت 0;

        أكد(data);

        *buffer_size = 0;

        انتخاب (c->type) {

#اگر HAVE_XZ
        حالت COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                وقتی (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        اگر (r < 0)
                                بازگشت r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        اگر (lzr != LZMA_OK)
                                بازگشت -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                بشکن;
#endif

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                اگر (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        اگر (r < 0)
                                بازگشت r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                اگر (r < 0)
                        بازگشت r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, پوچ);
                اگر (sym_LZ4F_isError(n))
                        بازگشت -EIO;

                *buffer_size += n;
                بشکن;
        }
#endif

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD: {
                ZSTD_inBuffer درون_داد = {
                        .src = data,
                        .size = size,
                };

                وقتی (درون_داد.pos < درون_داد.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        اگر (r < 0)
                                بازگشت r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &درون_داد, ZSTD_e_continue);
                        اگر (sym_ZSTD_isError(res))
                                بازگشت -EIO;

                        *buffer_size += output.pos;
                }

                بشکن;
        }
#endif

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                اگر (size > UINT_MAX)
                        بازگشت -EFBIG;

                c->gzip.next_in = (تهی*) data;
                c->gzip.avail_in = size;

                وقتی (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        اگر (r < 0)
                                بازگشت r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        اگر (r != Z_OK)
                                بازگشت -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                بشکن;
#endif

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                اگر (size > UINT_MAX)
                        بازگشت -EFBIG;

                c->bzip2.next_in = (تهی*) data;
                c->bzip2.avail_in = size;

                وقتی (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        اگر (r < 0)
                                بازگشت r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (تهی*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        اگر (r != BZ_RUN_OK)
                                بازگشت -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                بشکن;
#endif

        حالت COMPRESSION_NONE:

                اگر (*buffer_allocated < size) {
                        تهی *p;

                        p = realloc(*buffer, size);
                        اگر (!p)
                                بازگشت -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                بشکن;

        پیشفرض:
                بازگشت -EOPNOTSUPP;
        }

        بازگشت 0;
}

صحیح compressor_finish(Compressor *c, تهی **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#اگر HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحیح r;
#endif

        أكد(c);
        أكد(buffer);
        أكد(buffer_size);
        أكد(buffer_allocated);

        اگر (!c->encoding)
                بازگشت -EINVAL;

        *buffer_size = 0;

        انتخاب (c->type) {

#اگر HAVE_XZ
        حالت COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                انجامبده {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        اگر (r < 0)
                                بازگشت r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        اگر (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                بازگشت -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } وقتی (lzr != LZMA_STREAM_END);

                بشکن;
        }
#endif

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                اگر (r < 0)
                        بازگشت r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, پوچ);
                اگر (sym_LZ4F_isError(n))
                        بازگشت -EIO;

                *buffer_size = n;
                بشکن;
        }
#endif

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD: {
                ZSTD_inBuffer درون_داد = {};
                size_t res;

                انجامبده {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        اگر (r < 0)
                                بازگشت r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &درون_داد, ZSTD_e_end);
                        اگر (sym_ZSTD_isError(res))
                                بازگشت -EIO;

                        *buffer_size += output.pos;
                } وقتی (res != 0);

                بشکن;
        }
#endif

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                انجامبده {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        اگر (r < 0)
                                بازگشت r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        اگر (!IN_SET(r, Z_OK, Z_STREAM_END))
                                بازگشت -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } وقتی (r != Z_STREAM_END);

                بشکن;
#endif

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                انجامبده {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        اگر (r < 0)
                                بازگشت r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (تهی*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        اگر (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                بازگشت -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } وقتی (r != BZ_STREAM_END);

                بشکن;
#endif

        حالت COMPRESSION_NONE:
                بشکن;

        پیشفرض:
                بازگشت -EOPNOTSUPP;
        }

        بازگشت 0;
}
