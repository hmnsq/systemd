/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <sys/stat.h>
#شامل <unistd.h>

#اگر HAVE_XZ
#اگر_غیر_تعریف_شدہ SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#اگر_ختم
#شامل <lzma.h>
#اگر_ختم

#اگر HAVE_LZ4
#اگر_غیر_تعریف_شدہ SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#اگر_ختم
#شامل <lz4.h>
#شامل <lz4frame.h>
#شامل <lz4hc.h>
#اگر_ختم

#اگر HAVE_ZSTD
#اگر_غیر_تعریف_شدہ SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#اگر_ختم
#شامل <zstd.h>
#شامل <zstd_errors.h>
#اگر_ختم

#اگر HAVE_ZLIB
#اگر_غیر_تعریف_شدہ SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#اگر_ختم
#شامل <zlib.h>
#اگر_ختم

#اگر HAVE_BZIP2
#اگر_غیر_تعریف_شدہ SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#اگر_ختم
#شامل <bzlib.h>
#اگر_ختم

#شامل "alloc-util.h"
#شامل "bitfield.h"
#شامل "compress.h"
#شامل "dlfcn-util.h"
#شامل "io-util.h"
#شامل "log.h"
#شامل "string-table.h"
#شامل "unaligned.h"

#اگر HAVE_XZ
ساکن DLSYM_PROTOTYPE(lzma_auto_decoder) = تہی;
ساکن DLSYM_PROTOTYPE(lzma_رمز) = تہی;
ساکن DLSYM_PROTOTYPE(lzma_easy_encoder) = تہی;
ساکن DLSYM_PROTOTYPE(lzma_انتہا) = تہی;
ساکن DLSYM_PROTOTYPE(lzma_دھارا_بفر_encode) = تہی;
ساکن DLSYM_PROTOTYPE(lzma_دھارا_decoder) = تہی;
ساکن DLSYM_PROTOTYPE(lzma_lzma_preset) = تہی;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ساکن اندرونی خالی lzma_انتہا_wrapper(lzma_دھارا *ls) {
        sym_lzma_انتہا(ls);
}
#اگر_ختم

#اگر HAVE_LZ4
ساکن DLSYM_PROTOTYPE(LZ4F_compressBegin) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4F_compressBound) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4_F_compress_انتہا) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4F_compressUpdate) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4F_decompress) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4F_isError) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4_compress_HC) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4_compress_default) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4_decompress_محفوظ) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4_decompress_محفوظ_partial) = تہی;
ساکن DLSYM_PROTOTYPE(LZ4_نسخہ_نمبر) = تہی;

ساکن ثابت LZ4F_preferences_t lz4_preferences = {
        .چوکھٹا_Info.بلاک_سائز_ID = 5,
};
#اگر_ختم

#اگر HAVE_ZSTD
ساکن DLSYM_PROTOTYPE(ZSTD_CCtx_سیٹ_Parameter) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_compress) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_compressStream2) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_createCCtx) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_createDCtx) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_CStream_اندر_سائز) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_CStream_برآمد_سائز) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_decompress_دھارا) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_DStream_اندر_سائز) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_DStream_برآمد_سائز) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_freeCCtx) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_freeDCtx) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_get_Error_رمز) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_getErrorName) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_get_چوکھٹا_Content_سائز) = تہی;
ساکن DLSYM_PROTOTYPE(ZSTD_isError) = تہی;

DEFINE_TRIVIAL_CLEANUP_FUNC_بھرا_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, تہی);

ساکن صحیح zstd_ret_مقصد_errno(size_t ret) {
        انتخاب (sym_ZSTD_get_Error_رمز(ret)) {
        حالت ZSTD_error_dst_سائز_too_Small:
                واپس -ENOBUFS;
        حالت ZSTD_error_حافظہ_تخصیص:
                واپس -ENOMEM;
        طےشدہ:
                واپس -EBADMSG;
        }
}
#اگر_ختم

#اگر HAVE_ZLIB
ساکن DLSYM_PROTOTYPE(deflateInit2_) = تہی;
ساکن DLSYM_PROTOTYPE(deflate) = تہی;
ساکن DLSYM_PROTOTYPE(deflate_انتہا) = تہی;
ساکن DLSYM_PROTOTYPE(inflateInit2_) = تہی;
ساکن DLSYM_PROTOTYPE(inflate) = تہی;
ساکن DLSYM_PROTOTYPE(inflate_انتہا) = تہی;
ساکن DLSYM_PROTOTYPE(inflateReset) = تہی;

ساکن اندرونی خالی deflate_انتہا_wrapper(z_دھارا *s) {
        sym_deflate_انتہا(s);
}

ساکن اندرونی خالی inflate_انتہا_wrapper(z_دھارا *s) {
        sym_inflate_انتہا(s);
}
#اگر_ختم

#اگر HAVE_BZIP2
ساکن DLSYM_PROTOTYPE(BZ2_bzCompressInit) = تہی;
ساکن DLSYM_PROTOTYPE(BZ2_bzCompress) = تہی;
ساکن DLSYM_PROTOTYPE(BZ2_bz_Compress_انتہا) = تہی;
ساکن DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = تہی;
ساکن DLSYM_PROTOTYPE(BZ2_bzDecompress) = تہی;
ساکن DLSYM_PROTOTYPE(BZ2_bz_Decompress_انتہا) = تہی;

ساکن اندرونی خالی BZ2_bz_Compress_انتہا_wrapper(bz_دھارا *s) {
        sym_BZ2_bz_Compress_انتہا(s);
}

ساکن اندرونی خالی BZ2_bz_Decompress_انتہا_wrapper(bz_دھارا *s) {
        sym_BZ2_bz_Decompress_انتہا(s);
}
#اگر_ختم

/* Opaque Compressor/Decompressor struct definition */
ساخت Compressor {
        Compression type;
        bool encoding;
        bool ختم_شدہ;
        اتحاد {
#اگر HAVE_XZ
                lzma_دھارا xz;
#اگر_ختم
#اگر HAVE_LZ4
                ساخت {
                        LZ4F_compressionContext_t c_lz4;
                        خالی *lz4_سرخی;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_سرخی_سائز;
                };
                LZ4F_decompressionContext_t d_lz4;
#اگر_ختم
#اگر HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#اگر_ختم
#اگر HAVE_ZLIB
                z_دھارا gzip;
#اگر_ختم
#اگر HAVE_BZIP2
                bz_دھارا bzip2;
#اگر_ختم
        };
};

#تعریف_کرو ALIGN_8(l) ALIGN_مقصد(l, حجم(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#تعریف_کرو ZLIB_WBITS_GZIP (15 + 16)

ساکن ثابت حرف* ثابت compression_جدول[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

ساکن ثابت حرف* ثابت compression_uppercase_جدول[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

ساکن ثابت حرف* ثابت compression_extension_جدول[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "",
        [COMPRESSION_XZ]    = ".xz",
        [COMPRESSION_LZ4]   = ".lz4",
        [COMPRESSION_ZSTD]  = ".zst",
        [COMPRESSION_GZIP]  = ".gz",
        [COMPRESSION_BZIP2] = ".bz2",
};

DEFINE_STRING_جدول_LOOKUP(compression, Compression);
DEFINE_STRING_جدول_LOOKUP(compression_uppercase, Compression);
DEFINE_STRING_جدول_LOOKUP(compression_extension, Compression);

Compression compression_from_string_harder(ثابت حرف *s) {
        Compression c;

        أكد(s);

        c = compression_from_string(s);
        اگر (c >= 0)
                واپس c;

        واپس compression_uppercase_from_string(s);
}

Compression compression_from_filename(ثابت حرف *filename) {
        Compression c;
        ثابت حرف *e;

        أكد(filename);

        e = strrchr(filename, '.');
        اگر (!e)
                واپس COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        اگر (c < 0)
                واپس COMPRESSION_NONE;

        واپس c;
}

bool compression_supported_journal(Compression c) {
        ساکن ثابت غیرعلامتی supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        واپس ثنائی_ہندسہ_سیٹ(supported, c);
}

bool compression_supported(Compression c) {
        ساکن ثابت غیرعلامتی supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        واپس ثنائی_ہندسہ_سیٹ(supported, c);
}

Compression compression_detect_from_magic(ثابت uint8_t ڈیٹا[ساکن COMPRESSION_MAGIC_بائٹیں_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        اگر (memcmp(ڈیٹا, (ثابت uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                واپس COMPRESSION_GZIP;
        اگر (memcmp(ڈیٹا, (ثابت uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                واپس COMPRESSION_XZ;
        اگر (memcmp(ڈیٹا, (ثابت uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                واپس COMPRESSION_ZSTD;
        اگر (memcmp(ڈیٹا, (ثابت uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                واپس COMPRESSION_LZ4;
        اگر (memcmp(ڈیٹا, (ثابت uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                واپس COMPRESSION_BZIP2;

        واپس _COMPRESSION_INVALID;
}

صحیح dlopen_xz(صحیح log_level) {
#اگر HAVE_XZ
        ساکن خالی *lzma_dl = تہی;

        LIBLZMA_NOTE(suggested);

        واپس dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_رمز),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_انتہا),
                        DLSYM_ARG(lzma_دھارا_بفر_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_دھارا_decoder));
#ورنہ
        واپس log_بھرا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#اگر_ختم
}

صحیح dlopen_lz4(صحیح log_level) {
#اگر HAVE_LZ4
        ساکن خالی *lz4_dl = تہی;

        LIBLZ4_NOTE(suggested);

        واپس dlopen_many_sym_or_warn(
                        &lz4_dl,
                        "liblz4.so.1", log_level,
                        DLSYM_ARG(LZ4F_compressBegin),
                        DLSYM_ARG(LZ4F_compressBound),
                        DLSYM_ARG(LZ4_F_compress_انتہا),
                        DLSYM_ARG(LZ4F_compressUpdate),
                        DLSYM_ARG(LZ4F_createCompressionContext),
                        DLSYM_ARG(LZ4F_createDecompressionContext),
                        DLSYM_ARG(LZ4F_decompress),
                        DLSYM_ARG(LZ4F_freeCompressionContext),
                        DLSYM_ARG(LZ4F_freeDecompressionContext),
                        DLSYM_ARG(LZ4F_isError),
                        DLSYM_ARG(LZ4_compress_default),
                        DLSYM_ARG(LZ4_compress_HC),
                        DLSYM_ARG(LZ4_decompress_محفوظ),
                        DLSYM_ARG(LZ4_decompress_محفوظ_partial),
                        DLSYM_ARG(LZ4_نسخہ_نمبر));
#ورنہ
        واپس log_بھرا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#اگر_ختم
}

صحیح dlopen_zstd(صحیح log_level) {
#اگر HAVE_ZSTD
        ساکن خالی *zstd_dl = تہی;

        LIBZSTD_NOTE(suggested);

        واپس dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_get_Error_رمز),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_چوکھٹا_Content_سائز),
                        DLSYM_ARG(ZSTD_decompress_دھارا),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_برآمد_سائز),
                        DLSYM_ARG(ZSTD_CStream_اندر_سائز),
                        DLSYM_ARG(ZSTD_CStream_برآمد_سائز),
                        DLSYM_ARG(ZSTD_CCtx_سیٹ_Parameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_اندر_سائز),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#ورنہ
        واپس log_بھرا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#اگر_ختم
}

صحیح dlopen_zlib(صحیح log_level) {
#اگر HAVE_ZLIB
        ساکن خالی *zlib_dl = تہی;

        LIBZ_NOTE(suggested);

        واپس dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflate_انتہا),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflate_انتہا),
                        DLSYM_ARG(inflateReset));
#ورنہ
        واپس log_بھرا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#اگر_ختم
}

صحیح dlopen_bzip2(صحیح log_level) {
#اگر HAVE_BZIP2
        ساکن خالی *bzip2_dl = تہی;

        LIBBZ2_NOTE(suggested);

        واپس dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bz_Compress_انتہا),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bz_Decompress_انتہا));
#ورنہ
        واپس log_بھرا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#اگر_ختم
}

صحیح dlopen_compress_journal(Compression c, صحیح log_level) {
        انتخاب (c) {
        حالت COMPRESSION_NONE:
                واپس 0;
        حالت COMPRESSION_XZ:
                واپس dlopen_xz(log_level);
        حالت COMPRESSION_LZ4:
                واپس dlopen_lz4(log_level);
        حالت COMPRESSION_ZSTD:
                واپس dlopen_zstd(log_level);
        طےشدہ:
                واپس -EOPNOTSUPP;
        }
}

صحیح dlopen_compress(Compression c, صحیح log_level) {
        انتخاب (c) {
        حالت COMPRESSION_GZIP:
                واپس dlopen_zlib(log_level);
        حالت COMPRESSION_BZIP2:
                واپس dlopen_bzip2(log_level);
        طےشدہ:
                واپس dlopen_compress_journal(c, log_level);
        }
}

ساکن صحیح compress_blob_xz(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی *dst,
                size_t dst_alloc_سائز,
                size_t *dst_سائز,
                صحیح level) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_alloc_سائز > 0);
        أكد(dst_سائز);

#اگر HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, تہی, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_منوال_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, تہی }
        };
        lzma_ret ret;
        size_t برآمد_pos = 0;
        صحیح r;

        r = dlopen_xz(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                اگر (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        واپس -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        اگر (src_سائز < 80)
                واپس -ENOBUFS;

        ret = sym_lzma_دھارا_بفر_encode(filters, LZMA_CHECK_NONE, /* allocator= */ تہی,
                                            src, src_سائز, dst, &برآمد_pos, dst_alloc_سائز);
        اگر (ret != LZMA_OK)
                واپس -ENOBUFS;

        *dst_سائز = برآمد_pos;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح compress_blob_lz4(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی *dst,
                size_t dst_alloc_سائز,
                size_t *dst_سائز,
                صحیح level) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_alloc_سائز > 0);
        أكد(dst_سائز);

#اگر HAVE_LZ4
        صحیح r;

        r = dlopen_lz4(LOG_DEBUG);
        اگر (r < 0)
                واپس r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        اگر (src_سائز < 9)
                واپس -ENOBUFS;

        اگر (src_سائز > زیادہ_سے_زیادہ_عدد)
                واپس -EFBIG;
        اگر (dst_alloc_سائز > زیادہ_سے_زیادہ_عدد)
                dst_alloc_سائز = زیادہ_سے_زیادہ_عدد;

        اگر (level <= 0)
                r = sym_LZ4_compress_default(src, (حرف*)dst + 8, src_سائز, (صحیح) dst_alloc_سائز - 8);
        ورنہ
                r = sym_LZ4_compress_HC(src, (حرف*)dst + 8, src_سائز, (صحیح) dst_alloc_سائز - 8, level);
        اگر (r <= 0)
                واپس -ENOBUFS;

        unaligned_write_le64(dst, src_سائز);
        *dst_سائز = r + 8;

        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح compress_blob_zstd(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی *dst,
                size_t dst_alloc_سائز,
                size_t *dst_سائز,
                صحیح level) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_alloc_سائز > 0);
        أكد(dst_سائز);

#اگر HAVE_ZSTD
        size_t k;
        صحیح r;

        r = dlopen_zstd(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        k = sym_ZSTD_compress(dst, dst_alloc_سائز, src, src_سائز, level < 0 ? 0 : level);
        اگر (sym_ZSTD_isError(k))
                واپس zstd_ret_مقصد_errno(k);

        *dst_سائز = k;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح compress_blob_gzip(ثابت خالی *src, uint64_t src_سائز,
                       خالی *dst, size_t dst_alloc_سائز, size_t *dst_سائز, صحیح level) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_alloc_سائز > 0);
        أكد(dst_سائز);

#اگر HAVE_ZLIB
        صحیح r;

        r = dlopen_zlib(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز > UINT_MAX)
                واپس -EFBIG;
        اگر (dst_alloc_سائز > UINT_MAX)
                dst_alloc_سائز = UINT_MAX;

        _cleanup_(deflate_انتہا_wrapper) z_دھارا s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_نسخہ, (صحیح) حجم(s));
        اگر (r != Z_OK)
                واپس -ENOMEM;

        s.اگلا_اندر = (خالی*) src;
        s.avail_اندر = src_سائز;
        s.اگلا_برآمد = dst;
        s.avail_برآمد = dst_alloc_سائز;

        r = sym_deflate(&s, Z_اختتام);
        اگر (r != Z_دھارا_انتہا)
                واپس -ENOBUFS;

        *dst_سائز = dst_alloc_سائز - s.avail_برآمد;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح compress_blob_bzip2(
                ثابت خالی *src, uint64_t src_سائز,
                خالی *dst, size_t dst_alloc_سائز, size_t *dst_سائز, صحیح level) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_alloc_سائز > 0);
        أكد(dst_سائز);

#اگر HAVE_BZIP2
        صحیح r;

        r = dlopen_bzip2(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز > UINT_MAX)
                واپس -EFBIG;
        اگر (dst_alloc_سائز > UINT_MAX)
                dst_alloc_سائز = UINT_MAX;

        _cleanup_(BZ2_bz_Compress_انتہا_wrapper) bz_دھارا s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        اگر (r != BZ_OK)
                واپس -ENOMEM;

        s.اگلا_اندر = (حرف*) src;
        s.avail_اندر = src_سائز;
        s.اگلا_برآمد = (حرف*) dst;
        s.avail_برآمد = dst_alloc_سائز;

        r = sym_BZ2_bzCompress(&s, BZ_اختتام);

        اگر (r != BZ_دھارا_انتہا)
                واپس -ENOBUFS;

        *dst_سائز = dst_alloc_سائز - s.avail_برآمد;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

صحیح compress_blob_journal(
                Compression compression,
                ثابت خالی *src, uint64_t src_سائز,
                خالی *dst, size_t dst_alloc_سائز, size_t *dst_سائز, صحیح level) {

        انتخاب (compression) {
        حالت COMPRESSION_XZ:
                واپس compress_blob_xz(src, src_سائز, dst, dst_alloc_سائز, dst_سائز, level);
        حالت COMPRESSION_LZ4:
                واپس compress_blob_lz4(src, src_سائز, dst, dst_alloc_سائز, dst_سائز, level);
        حالت COMPRESSION_ZSTD:
                واپس compress_blob_zstd(src, src_سائز, dst, dst_alloc_سائز, dst_سائز, level);
        طےشدہ:
                واپس -EOPNOTSUPP;
        }
}

صحیح compress_blob(
                Compression compression,
                ثابت خالی *src, uint64_t src_سائز,
                خالی *dst, size_t dst_alloc_سائز, size_t *dst_سائز, صحیح level) {

        انتخاب (compression) {
        حالت COMPRESSION_GZIP:
                واپس compress_blob_gzip(src, src_سائز, dst, dst_alloc_سائز, dst_سائز, level);
        حالت COMPRESSION_BZIP2:
                واپس compress_blob_bzip2(src, src_سائز, dst, dst_alloc_سائز, dst_سائز, level);
        طےشدہ:
                واپس compress_blob_journal(compression, src, src_سائز, dst, dst_alloc_سائز, dst_سائز, level);
        }
}

ساکن صحیح decompress_blob_xz(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **dst,
                size_t *dst_سائز,
                size_t dst_max) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_سائز);

#اگر HAVE_XZ
        صحیح r;

        r = dlopen_xz(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        _cleanup_(lzma_انتہا_wrapper) lzma_دھارا s = LZMA_دھارا_INIT;
        lzma_ret ret = sym_lzma_دھارا_decoder(&s, UINT64_MAX, /* flags= */ 0);
        اگر (ret != LZMA_OK)
                واپس -ENOMEM;

        size_t space = MIN(src_سائز * 2, dst_max ?: SIZE_MAX);
        اگر (!greedy_realloc(dst, space, 1))
                واپس -ENOMEM;

        s.اگلا_اندر = src;
        s.avail_اندر = src_سائز;

        s.اگلا_برآمد = *dst;
        s.avail_برآمد = space;

        برائے (;;) {
                size_t مستعمل;

                ret = sym_lzma_رمز(&s, LZMA_اختتام);
                اگر (ret == LZMA_دھارا_انتہا)
                        توڑو;
                اگر (ret != LZMA_OK)
                        واپس -ENOMEM;

                اگر (dst_max > 0 && (space - s.avail_برآمد) >= dst_max)
                        توڑو;
                اگر (dst_max > 0 && space == dst_max)
                        واپس -ENOBUFS;

                مستعمل = space - s.avail_برآمد;
                /* Silence static analyzers, space is bounded by allocation size */
                أكد(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                اگر (!greedy_realloc(dst, space, 1))
                        واپس -ENOMEM;

                s.avail_برآمد = space - مستعمل;
                s.اگلا_برآمد = *(uint8_t**)dst + مستعمل;
        }

        *dst_سائز = space - s.avail_برآمد;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح decompress_blob_lz4(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **dst,
                size_t *dst_سائز,
                size_t dst_max) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_سائز);

#اگر HAVE_LZ4
        حرف* برآمد;
        صحیح r, سائز; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز <= 8)
                واپس -EBADMSG;

        اگر (src_سائز - 8 > زیادہ_سے_زیادہ_عدد)
                واپس -EFBIG;

        سائز = unaligned_read_le64(src);
        اگر (سائز < 0 || (غیرعلامتی) سائز != unaligned_read_le64(src))
                واپس -EFBIG;
        اگر (dst_max > 0 && (size_t) سائز > dst_max)
                واپس -ENOBUFS;
        برآمد = greedy_realloc(dst, سائز, 1);
        اگر (!برآمد)
                واپس -ENOMEM;

        r = sym_LZ4_decompress_محفوظ((حرف*)src + 8, برآمد, src_سائز - 8, سائز);
        اگر (r < 0 || r != سائز)
                واپس -EBADMSG;

        *dst_سائز = سائز;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

size_t zstd_dstream_برآمد_سائز(خالی) {
#اگر HAVE_ZSTD
        اگر (dlopen_zstd(LOG_DEBUG) < 0)
                واپس 0;

        واپس sym_ZSTD_DStream_برآمد_سائز();
#ورنہ
        واپس 0;
#اگر_ختم
}

ساکن صحیح decompress_blob_zstd(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **dst,
                size_t *dst_سائز,
                size_t dst_max) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_سائز);

#اگر HAVE_ZSTD
        صحیح r;

        r = dlopen_zstd(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        غیرعلامتی طویل طویل سائز = sym_ZSTD_get_چوکھٹا_Content_سائز(src, src_سائز);
        اگر (سائز == ZSTD_CONTENTSIZE_ERROR)
                واپس -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        اگر (سائز != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_برآمد_سائز(), (size_t) MIN(سائز, (غیرعلامتی طویل طویل) cap));
        ورنہ {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_سائز <= UINT64_MAX / 2 ? src_سائز * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_برآمد_سائز(), (size_t) MIN(twice, (uint64_t) cap));
        }

        اگر (!greedy_realloc(dst, space, 1))
                واپس -ENOMEM;
        space = MALLOC_SIZEOF_محفوظ(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        اگر (!dctx)
                واپس -ENOMEM;

        ZSTD_اندر_بفر داخلہ = {
                .src = src,
                .سائز = src_سائز,
        };
        ZSTD_برآمد_بفر output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .سائز = MIN(space, cap),
        };

        برائے (;;) {
                size_t k = sym_ZSTD_decompress_دھارا(dctx, &output, &داخلہ);
                اگر (sym_ZSTD_isError(k))
                        واپس log_debug_errno(zstd_ret_مقصد_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                اگر (output.pos >= cap)
                        توڑو;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                اگر (k == 0 && داخلہ.pos >= داخلہ.سائز)
                        توڑو;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                اگر (output.pos == output.سائز) {
                        اگر (space > SIZE_MAX / 2)
                                واپس -E2BIG;
                        space = MIN(2 * space, cap);
                        اگر (!greedy_realloc(dst, space, 1))
                                واپس -ENOMEM;
                        space = MALLOC_SIZEOF_محفوظ(*dst);
                        output.dst = *dst;
                        output.سائز = MIN(space, cap);
                        جاری;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                اگر (داخلہ.pos >= داخلہ.سائز)
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_سائز = output.pos;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح decompress_blob_gzip(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **dst,
                size_t *dst_سائز,
                size_t dst_max) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_سائز);

#اگر HAVE_ZLIB
        صحیح r;

        r = dlopen_zlib(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز > UINT_MAX)
                واپس -EFBIG;

        _cleanup_(inflate_انتہا_wrapper) z_دھارا s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخہ, (صحیح) حجم(s));
        اگر (r != Z_OK)
                واپس -ENOMEM;

        size_t space = MIN3(src_سائز * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        اگر (!greedy_realloc(dst, space, 1))
                واپس -ENOMEM;

        s.اگلا_اندر = (خالی*) src;
        s.avail_اندر = src_سائز;
        s.اگلا_برآمد = *dst;
        s.avail_برآمد = space;

        برائے (;;) {
                size_t مستعمل;

                r = sym_inflate(&s, Z_NO_FLUSH);
                اگر (r == Z_دھارا_انتہا)
                        توڑو;
                اگر (!اندر_سیٹ(r, Z_OK, Z_BUF_ERROR))
                        واپس -EBADMSG;

                اگر (dst_max > 0 && (space - s.avail_برآمد) >= dst_max)
                        توڑو;
                اگر (dst_max > 0 && space == dst_max)
                        واپس -ENOBUFS;

                مستعمل = space - s.avail_برآمد;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                اگر (!greedy_realloc(dst, space, 1))
                        واپس -ENOMEM;

                s.avail_برآمد = space - مستعمل;
                s.اگلا_برآمد = *(uint8_t**)dst + مستعمل;
        }

        *dst_سائز = space - s.avail_برآمد;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح decompress_blob_bzip2(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **dst,
                size_t *dst_سائز,
                size_t dst_max) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(dst);
        أكد(dst_سائز);

#اگر HAVE_BZIP2
        صحیح r;

        r = dlopen_bzip2(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز > UINT_MAX)
                واپس -EFBIG;

        _cleanup_(BZ2_bz_Decompress_انتہا_wrapper) bz_دھارا s = {};

        اگر (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                واپس -EIO;

        size_t space = MIN3(src_سائز * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        اگر (!greedy_realloc(dst, space, 1))
                واپس -ENOMEM;

        s.اگلا_اندر = (حرف*) src;
        s.avail_اندر = src_سائز;
        s.اگلا_برآمد = (حرف*) *dst;
        s.avail_برآمد = space;

        برائے (;;) {
                size_t مستعمل;

                r = sym_BZ2_bzDecompress(&s);
                اگر (r == BZ_دھارا_انتہا)
                        توڑو;
                اگر (r != BZ_OK)
                        واپس -EBADMSG;

                اگر (dst_max > 0 && (space - s.avail_برآمد) >= dst_max)
                        توڑو;
                اگر (dst_max > 0 && space == dst_max)
                        واپس -ENOBUFS;

                مستعمل = space - s.avail_برآمد;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                اگر (!greedy_realloc(dst, space, 1))
                        واپس -ENOMEM;

                s.avail_برآمد = space - مستعمل;
                s.اگلا_برآمد = (حرف*) *dst + مستعمل;
        }

        *dst_سائز = space - s.avail_برآمد;
        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

صحیح decompress_blob_journal(
                Compression compression,
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **dst,
                size_t *dst_سائز,
                size_t dst_max) {

        انتخاب (compression) {
        حالت COMPRESSION_XZ:
                واپس decompress_blob_xz(
                                src, src_سائز,
                                dst, dst_سائز, dst_max);
        حالت COMPRESSION_LZ4:
                واپس decompress_blob_lz4(
                                src, src_سائز,
                                dst, dst_سائز, dst_max);
        حالت COMPRESSION_ZSTD:
                واپس decompress_blob_zstd(
                                src, src_سائز,
                                dst, dst_سائز, dst_max);
        طےشدہ:
                واپس -EPROTONOSUPPORT;
        }
}

صحیح decompress_blob(
                Compression compression,
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **dst,
                size_t *dst_سائز,
                size_t dst_max) {

        انتخاب (compression) {
        حالت COMPRESSION_GZIP:
                واپس decompress_blob_gzip(
                                src, src_سائز,
                                dst, dst_سائز, dst_max);
        حالت COMPRESSION_BZIP2:
                واپس decompress_blob_bzip2(
                                src, src_سائز,
                                dst, dst_سائز, dst_max);
        طےشدہ:
                واپس decompress_blob_journal(
                                compression,
                                src, src_سائز,
                                dst, dst_سائز, dst_max);
        }
}

صحیح decompress_zlib_raw(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی *dst,
                size_t dst_سائز,
                صحیح wbits) {

#اگر HAVE_ZLIB
        صحیح r;

        r = dlopen_zlib(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز > UINT_MAX)
                واپس -EFBIG;
        اگر (dst_سائز > UINT_MAX)
                واپس -EFBIG;

        _cleanup_(inflate_انتہا_wrapper) z_دھارا s = {
                .اگلا_اندر = (خالی*) src,
                .avail_اندر = src_سائز,
                .اگلا_برآمد = dst,
                .avail_برآمد = dst_سائز,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_نسخہ, (صحیح) حجم(s));
        اگر (r != Z_OK)
                واپس -EIO;

        r = sym_inflate(&s, Z_اختتام);
        size_t produced = (uint8_t*) s.اگلا_برآمد - (uint8_t*) dst;

        اگر (r != Z_دھارا_انتہا || produced != dst_سائز)
                واپس -EBADMSG;

        واپس 0;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح decompress_startswith_xz(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **بفر,
                ثابت خالی *سابقہ,
                size_t سابقہ_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_سائز > 0);
        أكد(بفر);
        أكد(سابقہ);

#اگر HAVE_XZ
        صحیح r;

        r = dlopen_xz(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        _cleanup_(lzma_انتہا_wrapper) lzma_دھارا s = LZMA_دھارا_INIT;
        lzma_ret ret = sym_lzma_دھارا_decoder(&s, UINT64_MAX, /* flags= */ 0);
        اگر (ret != LZMA_OK)
                واپس -EBADMSG;

        اگر (!(greedy_realloc(بفر, ALIGN_8(سابقہ_len + 1), 1)))
                واپس -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_محفوظ(*بفر);

        s.اگلا_اندر = src;
        s.avail_اندر = src_سائز;

        s.اگلا_برآمد = *بفر;
        s.avail_برآمد = allocated;

        برائے (;;) {
                ret = sym_lzma_رمز(&s, LZMA_اختتام);

                اگر (!اندر_سیٹ(ret, LZMA_OK, LZMA_دھارا_انتہا))
                        واپس -EBADMSG;

                اگر (allocated - s.avail_برآمد >= سابقہ_len + 1)
                        واپس memcmp(*بفر, سابقہ, سابقہ_len) == 0 &&
                                ((ثابت uint8_t*) *بفر)[سابقہ_len] == extra;

                اگر (ret == LZMA_دھارا_انتہا)
                        واپس 0;

                s.avail_برآمد += allocated;

                اگر (!(greedy_realloc(بفر, allocated * 2, 1)))
                        واپس -ENOMEM;

                allocated = MALLOC_SIZEOF_محفوظ(*بفر);
                s.اگلا_برآمد = *(uint8_t**)بفر + allocated - s.avail_برآمد;
        }

#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح decompress_startswith_lz4(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **بفر,
                ثابت خالی *سابقہ,
                size_t سابقہ_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_سائز > 0);
        أكد(بفر);
        أكد(سابقہ);

#اگر HAVE_LZ4
        size_t allocated;
        صحیح r;

        r = dlopen_lz4(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز <= 8)
                واپس -EBADMSG;

        اگر (src_سائز - 8 > زیادہ_سے_زیادہ_عدد)
                واپس -EFBIG;

        اگر (!(greedy_realloc(بفر, ALIGN_8(سابقہ_len + 1), 1)))
                واپس -ENOMEM;
        allocated = MALLOC_SIZEOF_محفوظ(*بفر);

        r = sym_LZ4_decompress_محفوظ_partial(
                        (حرف*)src + 8,
                        *بفر,
                        src_سائز - 8,
                        سابقہ_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        اگر (r < 0 || (size_t) r < سابقہ_len + 1) {
                size_t سائز;

                اگر (sym_LZ4_نسخہ_نمبر() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        واپس 0;

                اگر (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        اگر (memcmp(*بفر, سابقہ, r) != 0)
                                واپس 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_سائز, بفر, &سائز, 0);
                اگر (r < 0)
                        واپس r;

                اگر (سائز < سابقہ_len + 1)
                        واپس 0;
        }

        واپس memcmp(*بفر, سابقہ, سابقہ_len) == 0 &&
                ((ثابت uint8_t*) *بفر)[سابقہ_len] == extra;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح decompress_startswith_zstd(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **بفر,
                ثابت خالی *سابقہ,
                size_t سابقہ_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(بفر);
        أكد(سابقہ);

#اگر HAVE_ZSTD
        صحیح r;

        r = dlopen_zstd(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        غیرعلامتی طویل طویل سائز = sym_ZSTD_get_چوکھٹا_Content_سائز(src, src_سائز);
        اگر (سائز == ZSTD_CONTENTSIZE_ERROR)
                واپس -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        اگر (سائز != ZSTD_CONTENTSIZE_UNKNOWN && سائز < سابقہ_len + 1)
                واپس 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        اگر (!dctx)
                واپس -ENOMEM;

        اگر (!(greedy_realloc(بفر, MAX(sym_ZSTD_DStream_برآمد_سائز(), سابقہ_len + 1), 1)))
                واپس -ENOMEM;

        ZSTD_اندر_بفر داخلہ = {
                .src = src,
                .سائز = src_سائز,
        };
        ZSTD_برآمد_بفر output = {
                .dst = *بفر,
                .سائز = MALLOC_SIZEOF_محفوظ(*بفر),
        };
        size_t k;

        k = sym_ZSTD_decompress_دھارا(dctx, &output, &داخلہ);
        اگر (sym_ZSTD_isError(k))
                واپس log_debug_errno(zstd_ret_مقصد_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        اگر (output.pos < سابقہ_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                اگر (k != 0)
                        واپس log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                واپس 0; /* Decompressed text too short to match the prefix and extra */
        }

        واپس memcmp(*بفر, سابقہ, سابقہ_len) == 0 &&
                ((ثابت uint8_t*) *بفر)[سابقہ_len] == extra;
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح decompress_startswith_gzip(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **بفر,
                ثابت خالی *سابقہ,
                size_t سابقہ_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(بفر);
        أكد(سابقہ);

#اگر HAVE_ZLIB
        صحیح r;

        r = dlopen_zlib(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز > UINT_MAX)
                واپس -EFBIG;

        _cleanup_(inflate_انتہا_wrapper) z_دھارا s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخہ, (صحیح) حجم(s));
        اگر (r != Z_OK)
                واپس -EBADMSG;

        اگر (!(greedy_realloc(بفر, ALIGN_8(سابقہ_len + 1), 1)))
                واپس -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_محفوظ(*بفر);

        s.اگلا_اندر = (خالی*) src;
        s.avail_اندر = src_سائز;

        s.اگلا_برآمد = *بفر;
        s.avail_برآمد = MIN(allocated, (size_t) UINT_MAX);

        برائے (;;) {
                r = sym_inflate(&s, Z_اختتام);

                اگر (!اندر_سیٹ(r, Z_OK, Z_دھارا_انتہا, Z_BUF_ERROR))
                        واپس -EBADMSG;

                اگر (allocated - s.avail_برآمد >= سابقہ_len + 1)
                        واپس memcmp(*بفر, سابقہ, سابقہ_len) == 0 &&
                                ((ثابت uint8_t*) *بفر)[سابقہ_len] == extra;

                اگر (r == Z_دھارا_انتہا)
                        واپس 0;

                اگر (s.avail_برآمد > 0)
                        واپس -EBADMSG; /* truncated */

                size_t مستعمل = allocated - s.avail_برآمد;

                اگر (!(greedy_realloc(بفر, allocated * 2, 1)))
                        واپس -ENOMEM;

                allocated = MALLOC_SIZEOF_محفوظ(*بفر);
                s.avail_برآمد = MIN(allocated - مستعمل, (size_t) UINT_MAX);
                s.اگلا_برآمد = *(uint8_t**)بفر + مستعمل;
        }
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

ساکن صحیح decompress_startswith_bzip2(
                ثابت خالی *src,
                uint64_t src_سائز,
                خالی **بفر,
                ثابت خالی *سابقہ,
                size_t سابقہ_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_سائز > 0);
        أكد(بفر);
        أكد(سابقہ);

#اگر HAVE_BZIP2
        صحیح r;

        r = dlopen_bzip2(LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        اگر (src_سائز > UINT_MAX)
                واپس -EFBIG;

        _cleanup_(BZ2_bz_Decompress_انتہا_wrapper) bz_دھارا s = {};

        اگر (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                واپس -EIO;

        اگر (!(greedy_realloc(بفر, ALIGN_8(سابقہ_len + 1), 1)))
                واپس -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_محفوظ(*بفر);

        s.اگلا_اندر = (حرف*) src;
        s.avail_اندر = src_سائز;

        s.اگلا_برآمد = *بفر;
        s.avail_برآمد = MIN(allocated, (size_t) UINT_MAX);

        برائے (;;) {
                r = sym_BZ2_bzDecompress(&s);

                اگر (!اندر_سیٹ(r, BZ_OK, BZ_دھارا_انتہا))
                        واپس -EBADMSG;

                اگر (allocated - s.avail_برآمد >= سابقہ_len + 1)
                        واپس memcmp(*بفر, سابقہ, سابقہ_len) == 0 &&
                                ((ثابت uint8_t*) *بفر)[سابقہ_len] == extra;

                اگر (r == BZ_دھارا_انتہا)
                        واپس 0;

                اگر (s.avail_برآمد > 0)
                        واپس -EBADMSG; /* truncated */

                size_t مستعمل = allocated - s.avail_برآمد;

                اگر (!(greedy_realloc(بفر, allocated * 2, 1)))
                        واپس -ENOMEM;

                allocated = MALLOC_SIZEOF_محفوظ(*بفر);
                s.avail_برآمد = MIN(allocated - مستعمل, (size_t) UINT_MAX);
                s.اگلا_برآمد = (حرف*) *بفر + مستعمل;
        }
#ورنہ
        واپس -EPROTONOSUPPORT;
#اگر_ختم
}

صحیح decompress_startswith_journal(
                Compression compression,
                ثابت خالی *src, uint64_t src_سائز,
                خالی **بفر,
                ثابت خالی *سابقہ, size_t سابقہ_len,
                uint8_t extra) {

        انتخاب (compression) {
        حالت COMPRESSION_XZ:
                واپس decompress_startswith_xz(src, src_سائز, بفر, سابقہ, سابقہ_len, extra);
        حالت COMPRESSION_LZ4:
                واپس decompress_startswith_lz4(src, src_سائز, بفر, سابقہ, سابقہ_len, extra);
        حالت COMPRESSION_ZSTD:
                واپس decompress_startswith_zstd(src, src_سائز, بفر, سابقہ, سابقہ_len, extra);
        طےشدہ:
                واپس -EOPNOTSUPP;
        }
}

صحیح decompress_startswith(
                Compression compression,
                ثابت خالی *src, uint64_t src_سائز,
                خالی **بفر,
                ثابت خالی *سابقہ, size_t سابقہ_len,
                uint8_t extra) {

        انتخاب (compression) {
        حالت COMPRESSION_GZIP:
                واپس decompress_startswith_gzip(src, src_سائز, بفر, سابقہ, سابقہ_len, extra);
        حالت COMPRESSION_BZIP2:
                واپس decompress_startswith_bzip2(src, src_سائز, بفر, سابقہ, سابقہ_len, extra);
        طےشدہ:
                واپس decompress_startswith_journal(compression, src, src_سائز, بفر, سابقہ, سابقہ_len, extra);
        }
}

صحیح compress_دھارا(
                Compression type,
                صحیح fdf, صحیح fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_سائز) {

        _cleanup_(compressor_freep) Compressor *c = تہی;
        _cleanup_free_ خالی *buf = تہی;
        _cleanup_free_ uint8_t *داخلہ = تہی;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t کل_اندر = 0, کل_برآمد = 0;
        صحیح r;

        أكد(fdf >= 0);
        أكد(fdt >= 0);

        r = compressor_new(&c, type);
        اگر (r < 0)
                واپس r;

        داخلہ = new(uint8_t, COMPRESS_PIPE_بفر_سائز);
        اگر (!داخلہ)
                واپس -ENOMEM;

        برائے (;;) {
                size_t m = COMPRESS_PIPE_بفر_سائز;
                ssize_t n;

                اگر (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = اقرأ(fdf, داخلہ, m);
                اگر (n < 0)
                        واپس -errno;

                اگر (n == 0) {
                        r = compressor_اختتام(c, &buf, &buf_size, &buf_alloc);
                        اگر (r < 0)
                                واپس r;

                        اگر (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                اگر (r < 0)
                                        واپس r;
                                کل_برآمد += buf_size;
                        }
                        توڑو;
                }

                کل_اندر += n;
                اگر (max_bytes != UINT64_MAX) {
                        أكد(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_آغاز(c, داخلہ, n, &buf, &buf_size, &buf_alloc);
                اگر (r < 0)
                        واپس r;

                اگر (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        اگر (r < 0)
                                واپس r;
                        کل_برآمد += buf_size;
                }
        }

        اگر (ret_uncompressed_سائز)
                *ret_uncompressed_سائز = کل_اندر;

        اگر (کل_اندر == 0)
                log_debug("%s compression finished (no input data)", compression_مقصد_string(type));
        ورنہ
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_مقصد_string(type), کل_اندر, کل_برآمد, (دوہرا) کل_برآمد / کل_اندر * 100);

        واپس 0;
}

ساکن صحیح should_sparse(صحیح fd) {
        ساخت stat st;

        أكد(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        اگر (fstat(fd, &st) < 0)
                واپس -errno;

        اگر (!S_ISREG(st.st_mode))
                واپس false;

        صحیح flags = fcntl(fd, F_GETFL);
        اگر (flags < 0)
                واپس -errno;

        اگر (FLAGS_سیٹ(flags, O_APPEND))
                واپس false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        اگر (pos < 0)
                واپس -errno;

        واپس pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
ساکن صحیح finalize_sparse(صحیح fd) {
        off_t pos;

        أكد(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        اگر (pos < 0)
                واپس -errno;

        اگر (ftruncate(fd, pos) < 0)
                واپس -errno;

        واپس 0;
}

/* Common helper for decompress_stream_*() wrappers */

ساخت decompress_دھارا_userdata {
        صحیح fd;
        uint64_t max_bytes;
        uint64_t کل_برآمد;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ساکن صحیح decompress_دھارا_write_callback(ثابت خالی *ڈیٹا, size_t سائز, خالی *userdata) {
        ساخت decompress_دھارا_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        اگر (u->max_bytes != UINT64_MAX) {
                اگر (u->max_bytes < سائز)
                        واپس -EFBIG;
                u->max_bytes -= سائز;
        }

        u->کل_برآمد += سائز;

        اگر (u->skip > 0) {
                اگر (u->skip >= سائز) {
                        u->skip -= سائز;
                        واپس 0;
                }

                ڈیٹا = (ثابت uint8_t*) ڈیٹا + u->skip;
                سائز -= u->skip;
                u->skip = 0;
        }

        اگر (سائز == 0)
                واپس 0;

        اگر (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, ڈیٹا, سائز, 64);
                اگر (k < 0)
                        واپس (صحیح) k;
                واپس 0;
        }

        واپس loop_write(u->fd, ڈیٹا, سائز);
}

ساکن صحیح decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        أكد(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        اگر (!c)
                واپس -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        انتخاب (type) {
        حالت COMPRESSION_NONE:
                توڑو;

#اگر HAVE_XZ
        حالت COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_سیٹ(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_سیٹ(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_دھارا_decoder(&c->xz, UINT64_MAX, xz_flags);
                اگر (xz_ret != LZMA_OK)
                        واپس -EIO;
                توڑو;
        }
#اگر_ختم

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4_F_نسخہ);
                اگر (sym_LZ4F_isError(rc))
                        واپس -ENOMEM;

                توڑو;
        }
#اگر_ختم

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                اگر (!c->d_zstd)
                        واپس -ENOMEM;
                توڑو;
#اگر_ختم

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                اگر (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخہ, (صحیح) حجم(c->gzip)) != Z_OK)
                        واپس -EIO;
                توڑو;
#اگر_ختم

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                اگر (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        واپس -EIO;
                توڑو;
#اگر_ختم

        طےشدہ:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        واپس 0;
}

ساکن صحیح decompress_blob_مقصد_fd_impl(
                Compression type,
                ثابت خالی *src,
                uint64_t src_سائز,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_سائز) {

        صحیح r;

        أكد(src);
        أكد(fdt >= 0);

        اگر (type == COMPRESSION_NONE)
                واپس -EOPNOTSUPP;

        اگر (src_سائز == 0)
                واپس -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        اگر (type == COMPRESSION_LZ4 &&
            src_سائز >= COMPRESSION_MAGIC_بائٹیں_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                اگر (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        اگر (src_سائز <= حجم(uint64_t))
                                واپس -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        اگر (sz < skip)
                                واپس -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                اگر (max_bytes == 0)
                        واپس -ENOBUFS;

                _cleanup_free_ خالی *buf = تہی;
                size_t سائز = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_سائز, &buf, &سائز,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                اگر (r < 0)
                        واپس r;

                /* Check the result size again for safety. */
                اگر (سائز < skip)
                        واپس -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, سائز - skip);
                اگر (r < 0)
                        واپس r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_سائز, سائز, (دوہرا) سائز / src_سائز * 100);

                اگر (ret_سائز)
                        *ret_سائز = سائز - skip;
                واپس 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = تہی;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        اگر (r < 0)
                واپس r;

        ساخت decompress_دھارا_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ڈالو(c, src, src_سائز, decompress_دھارا_write_callback, &userdata);
        اگر (r < 0)
                واپس r;

        /* Finalize */
        r = decompressor_ڈالو(c, /* data= */ تہی, /* size= */ 0, decompress_دھارا_write_callback, &userdata);
        اگر (r < 0)
                واپس r;

        اگر (userdata.کل_برآمد < skip)
                واپس -EBADMSG;

        اگر (userdata.sparse) {
                r = finalize_sparse(fdt);
                اگر (r < 0)
                        واپس r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_مقصد_string(type), src_سائز, userdata.کل_برآمد,
                  (دوہرا) userdata.کل_برآمد / src_سائز * 100);

        اگر (ret_سائز)
                *ret_سائز = userdata.کل_برآمد - skip;

        واپس 0;
}

صحیح decompress_blob_مقصد_fd_journal(
                Compression type,
                ثابت خالی *src,
                uint64_t src_سائز,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_سائز) {

        صحیح r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        واپس decompress_blob_مقصد_fd_impl(type, src, src_سائز, fdt, max_bytes, skip, ret_سائز);
}

صحیح decompress_blob_مقصد_fd(
                Compression type,
                ثابت خالی *src,
                uint64_t src_سائز,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_سائز) {

        صحیح r;

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        واپس decompress_blob_مقصد_fd_impl(type, src, src_سائز, fdt, max_bytes, skip, ret_سائز);
}

صحیح decompress_دھارا_بھرا(
                Compression type,
                صحیح fdf, صحیح fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = تہی;
        _cleanup_free_ uint8_t *buf = تہی;
        uint64_t کل_اندر = 0;
        صحیح r;

        أكد(fdf >= 0);
        أكد(fdt >= 0);

        اگر (type == COMPRESSION_NONE)
                واپس -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        r = decompressor_new(&c, type, flags);
        اگر (r < 0)
                واپس r;

        ساخت decompress_دھارا_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_بفر_سائز);
        اگر (!buf)
                واپس -ENOMEM;

        برائے (;;) {
                ssize_t n;

                n = اقرأ(fdf, buf, COMPRESS_PIPE_بفر_سائز);
                اگر (n < 0)
                        واپس -errno;

                کل_اندر += n;

                r = decompressor_ڈالو(c, buf, n, decompress_دھارا_write_callback, &userdata);
                اگر (r < 0)
                        واپس r;

                اگر (n == 0)
                        توڑو;
        }

        اگر (کل_اندر == 0)
                واپس log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_مقصد_string(type));

        اگر (userdata.sparse) {
                r = finalize_sparse(fdt);
                اگر (r < 0)
                        واپس r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_مقصد_string(type), کل_اندر, userdata.کل_برآمد,
                  (دوہرا) userdata.کل_برآمد / کل_اندر * 100);

        واپس 0;
}

صحیح decompress_دھارا_by_filename(ثابت حرف *filename, صحیح fdf, صحیح fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        اگر (c == COMPRESSION_NONE)
                واپس -EPROTONOSUPPORT;

        واپس decompress_دھارا(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        اگر (!c)
                واپس تہی;

        انتخاب (c->type) {

#اگر HAVE_XZ
        حالت COMPRESSION_XZ:
                sym_lzma_انتہا(&c->xz);
                توڑو;
#اگر_ختم

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4:
                اگر (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = تہی;
                        c->lz4_سرخی = mfree(c->lz4_سرخی);
                } ورنہ {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = تہی;
                }
                توڑو;
#اگر_ختم

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD:
                اگر (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = تہی;
                } ورنہ {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = تہی;
                }
                توڑو;
#اگر_ختم

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                اگر (c->encoding)
                        sym_deflate_انتہا(&c->gzip);
                ورنہ
                        sym_inflate_انتہا(&c->gzip);
                توڑو;
#اگر_ختم

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                اگر (c->encoding)
                        sym_BZ2_bz_Compress_انتہا(&c->bzip2);
                ورنہ
                        sym_BZ2_bz_Decompress_انتہا(&c->bzip2);
                توڑو;
#اگر_ختم

        طےشدہ:
                توڑو;
        }

        واپس mfree(c);
}

Compression compressor_type(ثابت Compressor *c) {
        واپس c ? c->type : _COMPRESSION_INVALID;
}

صحیح decompressor_detect(Decompressor **decompressor, ثابت خالی *ڈیٹا, size_t سائز) {
        صحیح r;

        أكد(decompressor);

        اگر (*decompressor)
                واپس 1;

        اگر (سائز < COMPRESSION_MAGIC_بائٹیں_MAX)
                واپس 0;

        أكد(ڈیٹا);

        Compression type = compression_detect_from_magic(ڈیٹا);
        اگر (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_مقصد_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        اگر (r < 0)
                واپس r;

        واپس 1;
}

صحیح decompressor_force(Decompressor **decompressor, Compression type) {
        صحیح r;

        أكد(decompressor);
        أكد(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        *decompressor = compressor_free(*decompressor);
        واپس decompressor_new(decompressor, type, /* flags= */ 0);
}

صحیح decompressor_force_off(Decompressor **decompressor) {
        واپس decompressor_force(decompressor, COMPRESSION_NONE);
}

صحیح decompressor_ڈالو(Decompressor *c, ثابت خالی *ڈیٹا, size_t سائز, DecompressorCallback callback, خالی *userdata) {
        صحیح r;

        أكد(c);
        أكد(ڈیٹا || سائز == 0);
        أكد(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        اگر (c->encoding)
                واپس -EINVAL;

        اگر (c->ختم_شدہ && سائز == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                واپس 0;

#اگر HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *بفر = تہی;
        اگر (c->type != COMPRESSION_NONE) {
                بفر = new(uint8_t, COMPRESS_PIPE_بفر_سائز);
                اگر (!بفر)
                        واپس -ENOMEM;
        }
#اگر_ختم

        انتخاب (c->type) {

        حالت COMPRESSION_NONE:
                اگر (c->ختم_شدہ)
                        واپس -EBADMSG;

                اگر (سائز == 0) {
                        c->ختم_شدہ = true;
                        واپس 0;
                }

                r = callback(ڈیٹا, سائز, userdata);
                اگر (r < 0)
                        واپس r;

                واپس 0;

#اگر HAVE_XZ
        حالت COMPRESSION_XZ:
                اگر (c->ختم_شدہ)
                        واپس -EBADMSG;

                c->xz.اگلا_اندر = ڈیٹا;
                c->xz.avail_اندر = سائز;

                برائے (;;) {
                        c->xz.اگلا_برآمد = بفر;
                        c->xz.avail_برآمد = COMPRESS_PIPE_بفر_سائز;

                        lzma_ret lzr = sym_lzma_رمز(&c->xz, سائز == 0 ? LZMA_اختتام : LZMA_تسلسل);
                        اگر (!اندر_سیٹ(lzr, LZMA_OK, LZMA_دھارا_انتہا))
                                واپس -EBADMSG;

                        اگر (c->xz.avail_برآمد > COMPRESS_PIPE_بفر_سائز)
                                واپس -EIO;

                        size_t produced = COMPRESS_PIPE_بفر_سائز - c->xz.avail_برآمد;
                        اگر (produced > 0) {
                                r = callback(بفر, produced, userdata);
                                اگر (r < 0)
                                        واپس r;
                        }

                        اگر (lzr == LZMA_دھارا_انتہا) {
                                اگر (c->xz.avail_اندر > 0)
                                        واپس -EBADMSG; /* garbage data at the end */

                                c->ختم_شدہ = true;
                                واپس 0;
                        }

                        اگر (c->xz.avail_اندر == 0 && c->xz.avail_برآمد > 0) {
                                اگر (سائز == 0)
                                        واپس -EBADMSG; /* truncated */

                                واپس 0; /* No progress possible with the current input. */
                        }
                }
#اگر_ختم

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                ثابت uint8_t *src = ڈیٹا;
                size_t src_بقایا = سائز;

                c->ختم_شدہ = false;

                برائے (;;) {
                        size_t produced = COMPRESS_PIPE_بفر_سائز;
                        size_t consumed = src_بقایا;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, بفر, &produced, src, &consumed, تہی);
                        اگر (sym_LZ4F_isError(rc))
                                واپس -EBADMSG;

                        اگر (consumed > src_بقایا ||
                            produced > COMPRESS_PIPE_بفر_سائز)
                                واپس -EIO;

                        اگر (consumed > 0) {
                                أكد(src);
                                src += consumed;
                                src_بقایا -= consumed;
                        }

                        اگر (produced > 0) {
                                r = callback(بفر, produced, userdata);
                                اگر (r < 0)
                                        واپس r;
                        }

                        اگر (src_بقایا == 0) {
                                اگر (rc == 0) {
                                        /* The current stream has finished. */
                                        c->ختم_شدہ = true;
                                        واپس 0;
                                }

                                اگر (produced == COMPRESS_PIPE_بفر_سائز)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        جاری;

                                اگر (سائز == 0)
                                        واپس -EBADMSG; /* truncated */

                                واپس 0; /* No progress possible with the current input. */
                        }
                }
        }
#اگر_ختم

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD: {
                ZSTD_اندر_بفر داخلہ = {
                        .src =  (خالی*) ڈیٹا,
                        .سائز = سائز,
                };

                c->ختم_شدہ = false;

                برائے (;;) {
                        ZSTD_برآمد_بفر output = {
                                .dst = بفر,
                                .سائز = COMPRESS_PIPE_بفر_سائز,
                        };

                        size_t res = sym_ZSTD_decompress_دھارا(c->d_zstd, &output, &داخلہ);
                        اگر (sym_ZSTD_isError(res))
                                واپس -EBADMSG;

                        اگر (داخلہ.pos > داخلہ.سائز ||
                            output.pos > output.سائز)
                                واپس -EIO;

                        اگر (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                اگر (r < 0)
                                        واپس r;
                        }

                        اگر (داخلہ.pos == داخلہ.سائز) {
                                اگر (res == 0) {
                                        /* The current stream has finished. */
                                        c->ختم_شدہ = true;
                                        واپس 0;
                                }

                                اگر (output.pos == COMPRESS_PIPE_بفر_سائز)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        جاری;

                                اگر (سائز == 0)
                                        واپس -EBADMSG; /* truncated */

                                واپس 0; /* No progress possible with the current input. */
                        }
                }
        }
#اگر_ختم

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                اگر (سائز > UINT_MAX)
                        واپس -EFBIG;

                c->gzip.اگلا_اندر = (خالی*) ڈیٹا;
                c->gzip.avail_اندر = سائز;

                برائے (;;) {
                        اگر (c->ختم_شدہ) {
                                اگر (sym_inflateReset(&c->gzip) != Z_OK)
                                        واپس -EIO;

                                c->ختم_شدہ = false;
                        }

                        c->gzip.اگلا_برآمد = بفر;
                        c->gzip.avail_برآمد = COMPRESS_PIPE_بفر_سائز;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        صحیح zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        اگر (!اندر_سیٹ(zr, Z_OK, Z_دھارا_انتہا) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_اندر == 0))
                                واپس -EBADMSG;

                        اگر (c->gzip.avail_برآمد > COMPRESS_PIPE_بفر_سائز)
                                واپس -EIO;

                        size_t produced = COMPRESS_PIPE_بفر_سائز - c->gzip.avail_برآمد;
                        اگر (produced > 0) {
                                r = callback(بفر, produced, userdata);
                                اگر (r < 0)
                                        واپس r;
                        }

                        اگر (zr == Z_دھارا_انتہا) {
                                c->ختم_شدہ = true;

                                اگر (c->gzip.avail_اندر > 0)
                                        جاری; /* New stream may be pending. */

                                واپس 0;
                        }

                        اگر (c->gzip.avail_اندر == 0 && c->gzip.avail_برآمد > 0) {
                                اگر (سائز == 0)
                                        واپس -EBADMSG; /* truncated */

                                واپس 0; /* No progress possible with the current input. */
                        }
                }
#اگر_ختم

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                اگر (سائز > UINT_MAX)
                        واپس -EFBIG;

                c->bzip2.اگلا_اندر = (حرف*) ڈیٹا;
                c->bzip2.avail_اندر = سائز;

                برائے (;;) {
                        اگر (c->ختم_شدہ) {
                                sym_BZ2_bz_Decompress_انتہا(&c->bzip2);
                                اگر (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        واپس -EIO;

                                c->ختم_شدہ = false;
                        }

                        c->bzip2.اگلا_برآمد = (حرف*) بفر;
                        c->bzip2.avail_برآمد = COMPRESS_PIPE_بفر_سائز;

                        صحیح bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        اگر (!اندر_سیٹ(bzr, BZ_OK, BZ_دھارا_انتہا))
                                واپس -EBADMSG;

                        اگر (c->bzip2.avail_برآمد > COMPRESS_PIPE_بفر_سائز)
                                واپس -EIO;

                        size_t produced = COMPRESS_PIPE_بفر_سائز - c->bzip2.avail_برآمد;
                        اگر (produced > 0) {
                                r = callback(بفر, produced, userdata);
                                اگر (r < 0)
                                        واپس r;
                        }

                        اگر (bzr == BZ_دھارا_انتہا) {
                                c->ختم_شدہ = true;

                                اگر (c->bzip2.avail_اندر > 0)
                                        جاری; /* New stream may be pending. */

                                واپس 0;
                        }

                        اگر (c->bzip2.avail_اندر == 0 && c->bzip2.avail_برآمد > 0) {
                                اگر (سائز == 0)
                                        واپس -EBADMSG; /* truncated */

                                واپس 0; /* No progress possible with the current input. */
                        }
                }
#اگر_ختم

        طےشدہ:
                assert_not_reached();
        }
}

صحیح compressor_new(Compressor **ret, Compression type) {
        صحیح r;

        أكد(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        اگر (r < 0)
                واپس r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        اگر (!c)
                واپس -ENOMEM;

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
                        واپس -EIO;

                c->type = COMPRESSION_XZ;
                توڑو;
        }
#اگر_ختم

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4_F_نسخہ);
                اگر (sym_LZ4F_isError(rc))
                        واپس -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t سرخی_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_سرخی = حافظہ_دو(سرخی_bound);
                اگر (!c->lz4_سرخی)
                        واپس -ENOMEM;

                c->lz4_سرخی_سائز = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_سرخی, سرخی_bound, &lz4_preferences);
                اگر (sym_LZ4F_isError(c->lz4_سرخی_سائز))
                        واپس -EINVAL;

                توڑو;
        }
#اگر_ختم

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                اگر (!c->c_zstd)
                        واپس -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_سیٹ_Parameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                اگر (sym_ZSTD_isError(z))
                        واپس -EIO;

                z = sym_ZSTD_CCtx_سیٹ_Parameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                اگر (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                توڑو;
#اگر_ختم

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_نسخہ, (صحیح) حجم(c->gzip));
                اگر (r != Z_OK)
                        واپس -EIO;

                c->type = COMPRESSION_GZIP;
                توڑو;
#اگر_ختم

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                اگر (r != BZ_OK)
                        واپس -EIO;

                c->type = COMPRESSION_BZIP2;
                توڑو;
#اگر_ختم

        حالت COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                توڑو;

        طےشدہ:
                واپس -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        واپس 0;
}

#اگر HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
ساکن صحیح enlarge_بفر(خالی **بفر, size_t *بفر_سائز, size_t *بفر_allocated, size_t need) {
        أكد(بفر);
        أكد(بفر_سائز);
        أكد(بفر_allocated);

        need = MAX3(need, *بفر_سائز + 1, (size_t) COMPRESS_PIPE_بفر_سائز);
        اگر (*بفر_allocated >= need)
                واپس 0;

        اگر (!greedy_realloc(بفر, need, 1))
                واپس -ENOMEM;

        *بفر_allocated = MALLOC_SIZEOF_محفوظ(*بفر);
        واپس 1;
}
#اگر_ختم

صحیح compressor_آغاز(
                Compressor *c,
                ثابت خالی *ڈیٹا,
                size_t سائز,
                خالی **بفر,
                size_t *بفر_سائز,
                size_t *بفر_allocated) {

#اگر HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحیح r;
#اگر_ختم

        أكد(c);
        أكد(بفر);
        أكد(بفر_سائز);
        أكد(بفر_allocated);

        اگر (!c->encoding)
                واپس -EINVAL;

        اگر (سائز == 0)
                واپس 0;

        أكد(ڈیٹا);

        *بفر_سائز = 0;

        انتخاب (c->type) {

#اگر HAVE_XZ
        حالت COMPRESSION_XZ:

                c->xz.اگلا_اندر = ڈیٹا;
                c->xz.avail_اندر = سائز;

                جب (c->xz.avail_اندر > 0) {
                        lzma_ret lzr;

                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, /* need= */ 0);
                        اگر (r < 0)
                                واپس r;

                        c->xz.اگلا_برآمد = (uint8_t*) *بفر + *بفر_سائز;
                        c->xz.avail_برآمد = *بفر_allocated - *بفر_سائز;

                        lzr = sym_lzma_رمز(&c->xz, LZMA_تسلسل);
                        اگر (lzr != LZMA_OK)
                                واپس -EIO;

                        *بفر_سائز += (*بفر_allocated - *بفر_سائز) - c->xz.avail_برآمد;
                }

                توڑو;
#اگر_ختم

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                اگر (c->lz4_سرخی_سائز > 0) {
                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, c->lz4_سرخی_سائز);
                        اگر (r < 0)
                                واپس r;

                        حافظہ_نقل(*بفر, c->lz4_سرخی, c->lz4_سرخی_سائز);
                        *بفر_سائز = c->lz4_سرخی_سائز;
                        c->lz4_سرخی = mfree(c->lz4_سرخی);
                        c->lz4_سرخی_سائز = 0;
                }

                size_t bound = sym_LZ4F_compressBound(سائز, &lz4_preferences);
                r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, *بفر_سائز + bound);
                اگر (r < 0)
                        واپس r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *بفر + *بفر_سائز,
                                                   *بفر_allocated - *بفر_سائز,
                                                   ڈیٹا, سائز, تہی);
                اگر (sym_LZ4F_isError(n))
                        واپس -EIO;

                *بفر_سائز += n;
                توڑو;
        }
#اگر_ختم

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD: {
                ZSTD_اندر_بفر داخلہ = {
                        .src = ڈیٹا,
                        .سائز = سائز,
                };

                جب (داخلہ.pos < داخلہ.سائز) {
                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, /* need= */ 0);
                        اگر (r < 0)
                                واپس r;

                        ZSTD_برآمد_بفر output = {
                                .dst = ((uint8_t *) *بفر + *بفر_سائز),
                                .سائز = *بفر_allocated - *بفر_سائز,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &داخلہ, ZSTD_e_continue);
                        اگر (sym_ZSTD_isError(res))
                                واپس -EIO;

                        *بفر_سائز += output.pos;
                }

                توڑو;
        }
#اگر_ختم

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                اگر (سائز > UINT_MAX)
                        واپس -EFBIG;

                c->gzip.اگلا_اندر = (خالی*) ڈیٹا;
                c->gzip.avail_اندر = سائز;

                جب (c->gzip.avail_اندر > 0) {
                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, /* need= */ 0);
                        اگر (r < 0)
                                واپس r;

                        size_t avail = MIN(*بفر_allocated - *بفر_سائز, (size_t) UINT_MAX);
                        c->gzip.اگلا_برآمد = (uint8_t*) *بفر + *بفر_سائز;
                        c->gzip.avail_برآمد = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        اگر (r != Z_OK)
                                واپس -EIO;

                        *بفر_سائز += avail - c->gzip.avail_برآمد;
                }

                توڑو;
#اگر_ختم

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                اگر (سائز > UINT_MAX)
                        واپس -EFBIG;

                c->bzip2.اگلا_اندر = (خالی*) ڈیٹا;
                c->bzip2.avail_اندر = سائز;

                جب (c->bzip2.avail_اندر > 0) {
                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, /* need= */ 0);
                        اگر (r < 0)
                                واپس r;

                        size_t avail = MIN(*بفر_allocated - *بفر_سائز, (size_t) UINT_MAX);
                        c->bzip2.اگلا_برآمد = (خالی*) ((uint8_t*) *بفر + *بفر_سائز);
                        c->bzip2.avail_برآمد = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_تسلسل);
                        اگر (r != BZ_تسلسل_OK)
                                واپس -EIO;

                        *بفر_سائز += avail - c->bzip2.avail_برآمد;
                }

                توڑو;
#اگر_ختم

        حالت COMPRESSION_NONE:

                اگر (*بفر_allocated < سائز) {
                        خالی *p;

                        p = realloc(*بفر, سائز);
                        اگر (!p)
                                واپس -ENOMEM;

                        *بفر = p;
                        *بفر_allocated = سائز;
                }

                حافظہ_نقل(*بفر, ڈیٹا, سائز);
                *بفر_سائز = سائز;
                توڑو;

        طےشدہ:
                واپس -EOPNOTSUPP;
        }

        واپس 0;
}

صحیح compressor_اختتام(Compressor *c, خالی **بفر, size_t *بفر_سائز, size_t *بفر_allocated) {
#اگر HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحیح r;
#اگر_ختم

        أكد(c);
        أكد(بفر);
        أكد(بفر_سائز);
        أكد(بفر_allocated);

        اگر (!c->encoding)
                واپس -EINVAL;

        *بفر_سائز = 0;

        انتخاب (c->type) {

#اگر HAVE_XZ
        حالت COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_اندر = 0;

                کرو {
                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, /* need= */ 0);
                        اگر (r < 0)
                                واپس r;

                        c->xz.اگلا_برآمد = (uint8_t*) *بفر + *بفر_سائز;
                        c->xz.avail_برآمد = *بفر_allocated - *بفر_سائز;

                        lzr = sym_lzma_رمز(&c->xz, LZMA_اختتام);
                        اگر (!اندر_سیٹ(lzr, LZMA_OK, LZMA_دھارا_انتہا))
                                واپس -EIO;

                        *بفر_سائز += (*بفر_allocated - *بفر_سائز) - c->xz.avail_برآمد;
                } جب (lzr != LZMA_دھارا_انتہا);

                توڑو;
        }
#اگر_ختم

#اگر HAVE_LZ4
        حالت COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, bound);
                اگر (r < 0)
                        واپس r;

                size_t n = sym_LZ4_F_compress_انتہا(c->c_lz4, *بفر, *بفر_allocated, تہی);
                اگر (sym_LZ4F_isError(n))
                        واپس -EIO;

                *بفر_سائز = n;
                توڑو;
        }
#اگر_ختم

#اگر HAVE_ZSTD
        حالت COMPRESSION_ZSTD: {
                ZSTD_اندر_بفر داخلہ = {};
                size_t res;

                کرو {
                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, /* need= */ 0);
                        اگر (r < 0)
                                واپس r;

                        ZSTD_برآمد_بفر output = {
                                .dst = ((uint8_t *) *بفر + *بفر_سائز),
                                .سائز = *بفر_allocated - *بفر_سائز,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &داخلہ, ZSTD_e_انتہا);
                        اگر (sym_ZSTD_isError(res))
                                واپس -EIO;

                        *بفر_سائز += output.pos;
                } جب (res != 0);

                توڑو;
        }
#اگر_ختم

#اگر HAVE_ZLIB
        حالت COMPRESSION_GZIP:
                c->gzip.avail_اندر = 0;

                کرو {
                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, /* need= */ 0);
                        اگر (r < 0)
                                واپس r;

                        size_t avail = MIN(*بفر_allocated - *بفر_سائز, (size_t) UINT_MAX);
                        c->gzip.اگلا_برآمد = (uint8_t*) *بفر + *بفر_سائز;
                        c->gzip.avail_برآمد = avail;

                        r = sym_deflate(&c->gzip, Z_اختتام);
                        اگر (!اندر_سیٹ(r, Z_OK, Z_دھارا_انتہا))
                                واپس -EIO;

                        *بفر_سائز += avail - c->gzip.avail_برآمد;
                } جب (r != Z_دھارا_انتہا);

                توڑو;
#اگر_ختم

#اگر HAVE_BZIP2
        حالت COMPRESSION_BZIP2:
                c->bzip2.avail_اندر = 0;

                کرو {
                        r = enlarge_بفر(بفر, بفر_سائز, بفر_allocated, /* need= */ 0);
                        اگر (r < 0)
                                واپس r;

                        size_t avail = MIN(*بفر_allocated - *بفر_سائز, (size_t) UINT_MAX);
                        c->bzip2.اگلا_برآمد = (خالی*) ((uint8_t*) *بفر + *بفر_سائز);
                        c->bzip2.avail_برآمد = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_اختتام);
                        اگر (!اندر_سیٹ(r, BZ_اختتام_OK, BZ_دھارا_انتہا))
                                واپس -EIO;

                        *بفر_سائز += avail - c->bzip2.avail_برآمد;
                } جب (r != BZ_دھارا_انتہا);

                توڑو;
#اگر_ختم

        حالت COMPRESSION_NONE:
                توڑو;

        طےشدہ:
                واپس -EOPNOTSUPP;
        }

        واپس 0;
}
