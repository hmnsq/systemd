/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#إذا HAVE_XZ
#جے_بنا_تعریف SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#جے_انت
#ضمن <lzma.h>
#جے_انت

#إذا HAVE_LZ4
#جے_بنا_تعریف SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#جے_انت
#ضمن <lz4.h>
#ضمن <lz4frame.h>
#ضمن <lz4hc.h>
#جے_انت

#إذا HAVE_ZSTD
#جے_بنا_تعریف SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#جے_انت
#ضمن <zstd.h>
#ضمن <zstd_errors.h>
#جے_انت

#إذا HAVE_ZLIB
#جے_بنا_تعریف SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#جے_انت
#ضمن <zlib.h>
#جے_انت

#إذا HAVE_BZIP2
#جے_بنا_تعریف SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#جے_انت
#ضمن <bzlib.h>
#جے_انت

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
ساكن DLSYM_PROTOTYPE(lzma_کوڈ) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_easy_encoder) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_سرا) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_دھارا_بفر_encode) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_دھارا_decoder) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_lzma_preset) = عدم;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ساكن مضمن فراغ lzma_سرا_wrapper(lzma_دھارا *ls) {
        sym_lzma_سرا(ls);
}
#جے_انت

#إذا HAVE_LZ4
ساكن DLSYM_PROTOTYPE(LZ4F_compressBegin) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_compressBound) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_F_compress_سرا) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_compressUpdate) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_decompress) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_isError) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_compress_HC) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_compress_default) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_decompress_محفوظ) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_decompress_محفوظ_partial) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_نسخہ_نمبر) = عدم;

ساكن ثابت LZ4F_preferences_t lz4_preferences = {
        .چوکھٹا_Info.بلاک_ناپ_ID = 5,
};
#جے_انت

#إذا HAVE_ZSTD
ساكن DLSYM_PROTOTYPE(ZSTD_CCtx_سیٹ_Parameter) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_compress) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_compressStream2) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_createCCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_createDCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_CStream_وچ_ناپ) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_CStream_باہرلا_ناپ) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_decompress_دھارا) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_DStream_وچ_ناپ) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_DStream_باہرلا_ناپ) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_freeCCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_freeDCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_get_Error_کوڈ) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_getErrorName) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_get_چوکھٹا_Content_ناپ) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_isError) = عدم;

DEFINE_TRIVIAL_CLEANUP_FUNC_بھریا_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, عدم);

ساكن صحيح zstd_ret_منزل_errno(size_t ret) {
        بدل (sym_ZSTD_get_Error_کوڈ(ret)) {
        حالة ZSTD_error_dst_ناپ_too_Small:
                أرجع -ENOBUFS;
        حالة ZSTD_error_حافظہ_ونڈ:
                أرجع -ENOMEM;
        افتراضي:
                أرجع -EBADMSG;
        }
}
#جے_انت

#إذا HAVE_ZLIB
ساكن DLSYM_PROTOTYPE(deflateInit2_) = عدم;
ساكن DLSYM_PROTOTYPE(deflate) = عدم;
ساكن DLSYM_PROTOTYPE(deflate_سرا) = عدم;
ساكن DLSYM_PROTOTYPE(inflateInit2_) = عدم;
ساكن DLSYM_PROTOTYPE(inflate) = عدم;
ساكن DLSYM_PROTOTYPE(inflate_سرا) = عدم;
ساكن DLSYM_PROTOTYPE(inflateReset) = عدم;

ساكن مضمن فراغ deflate_سرا_wrapper(z_دھارا *s) {
        sym_deflate_سرا(s);
}

ساكن مضمن فراغ inflate_سرا_wrapper(z_دھارا *s) {
        sym_inflate_سرا(s);
}
#جے_انت

#إذا HAVE_BZIP2
ساكن DLSYM_PROTOTYPE(BZ2_bzCompressInit) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzCompress) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bz_Compress_سرا) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzDecompress) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bz_Decompress_سرا) = عدم;

ساكن مضمن فراغ BZ2_bz_Compress_سرا_wrapper(bz_دھارا *s) {
        sym_BZ2_bz_Compress_سرا(s);
}

ساكن مضمن فراغ BZ2_bz_Decompress_سرا_wrapper(bz_دھارا *s) {
        sym_BZ2_bz_Decompress_سرا(s);
}
#جے_انت

/* Opaque Compressor/Decompressor struct definition */
بنية Compressor {
        Compression type;
        bool encoding;
        bool مکمل;
        اتحاد {
#إذا HAVE_XZ
                lzma_دھارا xz;
#جے_انت
#إذا HAVE_LZ4
                بنية {
                        LZ4F_compressionContext_t c_lz4;
                        فراغ *lz4_سرخی;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_سرخی_ناپ;
                };
                LZ4F_decompressionContext_t d_lz4;
#جے_انت
#إذا HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#جے_انت
#إذا HAVE_ZLIB
                z_دھارا gzip;
#جے_انت
#إذا HAVE_BZIP2
                bz_دھارا bzip2;
#جے_انت
        };
};

#تعریف_کرو ALIGN_8(l) ALIGN_منزل(l, حجم(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#تعریف_کرو ZLIB_WBITS_GZIP (15 + 16)

ساكن ثابت حرف* ثابت compression_جدول[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

ساكن ثابت حرف* ثابت compression_uppercase_جدول[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

ساكن ثابت حرف* ثابت compression_extension_جدول[_COMPRESSION_MAX] = {
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
        ساكن ثابت بنانشانی supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        أرجع ثنائی_ہندسہ_سیٹ(supported, c);
}

bool compression_supported(Compression c) {
        ساكن ثابت بنانشانی supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        أرجع ثنائی_ہندسہ_سیٹ(supported, c);
}

Compression compression_detect_from_magic(ثابت uint8_t انکڑے[ساكن COMPRESSION_MAGIC_بائٹاں_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        إذا (memcmp(انکڑے, (ثابت uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                أرجع COMPRESSION_GZIP;
        إذا (memcmp(انکڑے, (ثابت uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                أرجع COMPRESSION_XZ;
        إذا (memcmp(انکڑے, (ثابت uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                أرجع COMPRESSION_ZSTD;
        إذا (memcmp(انکڑے, (ثابت uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                أرجع COMPRESSION_LZ4;
        إذا (memcmp(انکڑے, (ثابت uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
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
                        DLSYM_ARG(lzma_کوڈ),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_سرا),
                        DLSYM_ARG(lzma_دھارا_بفر_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_دھارا_decoder));
#وإلا
        أرجع log_بھریا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#جے_انت
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
                        DLSYM_ARG(LZ4_F_compress_سرا),
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
#وإلا
        أرجع log_بھریا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#جے_انت
}

صحيح dlopen_zstd(صحيح log_level) {
#إذا HAVE_ZSTD
        ساكن فراغ *zstd_dl = عدم;

        LIBZSTD_NOTE(suggested);

        أرجع dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_get_Error_کوڈ),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_چوکھٹا_Content_ناپ),
                        DLSYM_ARG(ZSTD_decompress_دھارا),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_باہرلا_ناپ),
                        DLSYM_ARG(ZSTD_CStream_وچ_ناپ),
                        DLSYM_ARG(ZSTD_CStream_باہرلا_ناپ),
                        DLSYM_ARG(ZSTD_CCtx_سیٹ_Parameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_وچ_ناپ),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#وإلا
        أرجع log_بھریا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#جے_انت
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
                        DLSYM_ARG(deflate_سرا),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflate_سرا),
                        DLSYM_ARG(inflateReset));
#وإلا
        أرجع log_بھریا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#جے_انت
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
                        DLSYM_ARG(BZ2_bz_Compress_سرا),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bz_Decompress_سرا));
#وإلا
        أرجع log_بھریا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#جے_انت
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
                uint64_t src_ناپ,
                فراغ *dst,
                size_t dst_alloc_ناپ,
                size_t *dst_ناپ,
                صحيح level) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_alloc_ناپ > 0);
        أكد(dst_ناپ);

#إذا HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, عدم, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_منوال_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, عدم }
        };
        lzma_ret ret;
        size_t باہرلا_pos = 0;
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

        إذا (src_ناپ < 80)
                أرجع -ENOBUFS;

        ret = sym_lzma_دھارا_بفر_encode(filters, LZMA_CHECK_NONE, /* allocator= */ عدم,
                                            src, src_ناپ, dst, &باہرلا_pos, dst_alloc_ناپ);
        إذا (ret != LZMA_OK)
                أرجع -ENOBUFS;

        *dst_ناپ = باہرلا_pos;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح compress_blob_lz4(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ *dst,
                size_t dst_alloc_ناپ,
                size_t *dst_ناپ,
                صحيح level) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_alloc_ناپ > 0);
        أكد(dst_ناپ);

#إذا HAVE_LZ4
        صحيح r;

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        إذا (src_ناپ < 9)
                أرجع -ENOBUFS;

        إذا (src_ناپ > ودھ_توں_ودھ_عدد)
                أرجع -EFBIG;
        إذا (dst_alloc_ناپ > ودھ_توں_ودھ_عدد)
                dst_alloc_ناپ = ودھ_توں_ودھ_عدد;

        إذا (level <= 0)
                r = sym_LZ4_compress_default(src, (حرف*)dst + 8, src_ناپ, (صحيح) dst_alloc_ناپ - 8);
        وإلا
                r = sym_LZ4_compress_HC(src, (حرف*)dst + 8, src_ناپ, (صحيح) dst_alloc_ناپ - 8, level);
        إذا (r <= 0)
                أرجع -ENOBUFS;

        unaligned_write_le64(dst, src_ناپ);
        *dst_ناپ = r + 8;

        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح compress_blob_zstd(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ *dst,
                size_t dst_alloc_ناپ,
                size_t *dst_ناپ,
                صحيح level) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_alloc_ناپ > 0);
        أكد(dst_ناپ);

#إذا HAVE_ZSTD
        size_t k;
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        k = sym_ZSTD_compress(dst, dst_alloc_ناپ, src, src_ناپ, level < 0 ? 0 : level);
        إذا (sym_ZSTD_isError(k))
                أرجع zstd_ret_منزل_errno(k);

        *dst_ناپ = k;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح compress_blob_gzip(ثابت فراغ *src, uint64_t src_ناپ,
                       فراغ *dst, size_t dst_alloc_ناپ, size_t *dst_ناپ, صحيح level) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_alloc_ناپ > 0);
        أكد(dst_ناپ);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_alloc_ناپ > UINT_MAX)
                dst_alloc_ناپ = UINT_MAX;

        _cleanup_(deflate_سرا_wrapper) z_دھارا s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_نسخہ, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -ENOMEM;

        s.اگلا_وچ = (فراغ*) src;
        s.avail_وچ = src_ناپ;
        s.اگلا_باہرلا = dst;
        s.avail_باہرلا = dst_alloc_ناپ;

        r = sym_deflate(&s, Z_انت);
        إذا (r != Z_دھارا_سرا)
                أرجع -ENOBUFS;

        *dst_ناپ = dst_alloc_ناپ - s.avail_باہرلا;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح compress_blob_bzip2(
                ثابت فراغ *src, uint64_t src_ناپ,
                فراغ *dst, size_t dst_alloc_ناپ, size_t *dst_ناپ, صحيح level) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_alloc_ناپ > 0);
        أكد(dst_ناپ);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_alloc_ناپ > UINT_MAX)
                dst_alloc_ناپ = UINT_MAX;

        _cleanup_(BZ2_bz_Compress_سرا_wrapper) bz_دھارا s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        إذا (r != BZ_OK)
                أرجع -ENOMEM;

        s.اگلا_وچ = (حرف*) src;
        s.avail_وچ = src_ناپ;
        s.اگلا_باہرلا = (حرف*) dst;
        s.avail_باہرلا = dst_alloc_ناپ;

        r = sym_BZ2_bzCompress(&s, BZ_انت);

        إذا (r != BZ_دھارا_سرا)
                أرجع -ENOBUFS;

        *dst_ناپ = dst_alloc_ناپ - s.avail_باہرلا;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

صحيح compress_blob_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_ناپ,
                فراغ *dst, size_t dst_alloc_ناپ, size_t *dst_ناپ, صحيح level) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع compress_blob_xz(src, src_ناپ, dst, dst_alloc_ناپ, dst_ناپ, level);
        حالة COMPRESSION_LZ4:
                أرجع compress_blob_lz4(src, src_ناپ, dst, dst_alloc_ناپ, dst_ناپ, level);
        حالة COMPRESSION_ZSTD:
                أرجع compress_blob_zstd(src, src_ناپ, dst, dst_alloc_ناپ, dst_ناپ, level);
        افتراضي:
                أرجع -EOPNOTSUPP;
        }
}

صحيح compress_blob(
                Compression compression,
                ثابت فراغ *src, uint64_t src_ناپ,
                فراغ *dst, size_t dst_alloc_ناپ, size_t *dst_ناپ, صحيح level) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع compress_blob_gzip(src, src_ناپ, dst, dst_alloc_ناپ, dst_ناپ, level);
        حالة COMPRESSION_BZIP2:
                أرجع compress_blob_bzip2(src, src_ناپ, dst, dst_alloc_ناپ, dst_ناپ, level);
        افتراضي:
                أرجع compress_blob_journal(compression, src, src_ناپ, dst, dst_alloc_ناپ, dst_ناپ, level);
        }
}

ساكن صحيح decompress_blob_xz(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **dst,
                size_t *dst_ناپ,
                size_t dst_max) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_ناپ);

#إذا HAVE_XZ
        صحيح r;

        r = dlopen_xz(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        _cleanup_(lzma_سرا_wrapper) lzma_دھارا s = LZMA_دھارا_INIT;
        lzma_ret ret = sym_lzma_دھارا_decoder(&s, UINT64_MAX, /* flags= */ 0);
        إذا (ret != LZMA_OK)
                أرجع -ENOMEM;

        size_t space = MIN(src_ناپ * 2, dst_max ?: SIZE_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.اگلا_وچ = src;
        s.avail_وچ = src_ناپ;

        s.اگلا_باہرلا = *dst;
        s.avail_باہرلا = space;

        لكل (;;) {
                size_t ورتیا;

                ret = sym_lzma_کوڈ(&s, LZMA_انت);
                إذا (ret == LZMA_دھارا_سرا)
                        اكسر;
                إذا (ret != LZMA_OK)
                        أرجع -ENOMEM;

                إذا (dst_max > 0 && (space - s.avail_باہرلا) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                ورتیا = space - s.avail_باہرلا;
                /* Silence static analyzers, space is bounded by allocation size */
                أكد(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_باہرلا = space - ورتیا;
                s.اگلا_باہرلا = *(uint8_t**)dst + ورتیا;
        }

        *dst_ناپ = space - s.avail_باہرلا;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح decompress_blob_lz4(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **dst,
                size_t *dst_ناپ,
                size_t dst_max) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_ناپ);

#إذا HAVE_LZ4
        حرف* باہرلا;
        صحيح r, ناپ; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ <= 8)
                أرجع -EBADMSG;

        إذا (src_ناپ - 8 > ودھ_توں_ودھ_عدد)
                أرجع -EFBIG;

        ناپ = unaligned_read_le64(src);
        إذا (ناپ < 0 || (بنانشانی) ناپ != unaligned_read_le64(src))
                أرجع -EFBIG;
        إذا (dst_max > 0 && (size_t) ناپ > dst_max)
                أرجع -ENOBUFS;
        باہرلا = greedy_realloc(dst, ناپ, 1);
        إذا (!باہرلا)
                أرجع -ENOMEM;

        r = sym_LZ4_decompress_محفوظ((حرف*)src + 8, باہرلا, src_ناپ - 8, ناپ);
        إذا (r < 0 || r != ناپ)
                أرجع -EBADMSG;

        *dst_ناپ = ناپ;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

size_t zstd_dstream_باہرلا_ناپ(فراغ) {
#إذا HAVE_ZSTD
        إذا (dlopen_zstd(LOG_DEBUG) < 0)
                أرجع 0;

        أرجع sym_ZSTD_DStream_باہرلا_ناپ();
#وإلا
        أرجع 0;
#جے_انت
}

ساكن صحيح decompress_blob_zstd(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **dst,
                size_t *dst_ناپ,
                size_t dst_max) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_ناپ);

#إذا HAVE_ZSTD
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        بنانشانی لما لما ناپ = sym_ZSTD_get_چوکھٹا_Content_ناپ(src, src_ناپ);
        إذا (ناپ == ZSTD_CONTENTSIZE_ERROR)
                أرجع -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        إذا (ناپ != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_باہرلا_ناپ(), (size_t) MIN(ناپ, (بنانشانی لما لما) cap));
        وإلا {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_ناپ <= UINT64_MAX / 2 ? src_ناپ * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_باہرلا_ناپ(), (size_t) MIN(twice, (uint64_t) cap));
        }

        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;
        space = MALLOC_SIZEOF_محفوظ(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        إذا (!dctx)
                أرجع -ENOMEM;

        ZSTD_وچ_بفر داخلہ = {
                .src = src,
                .ناپ = src_ناپ,
        };
        ZSTD_باہرلا_بفر output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .ناپ = MIN(space, cap),
        };

        لكل (;;) {
                size_t k = sym_ZSTD_decompress_دھارا(dctx, &output, &داخلہ);
                إذا (sym_ZSTD_isError(k))
                        أرجع log_debug_errno(zstd_ret_منزل_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

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
                إذا (k == 0 && داخلہ.pos >= داخلہ.ناپ)
                        اكسر;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                إذا (output.pos == output.ناپ) {
                        إذا (space > SIZE_MAX / 2)
                                أرجع -E2BIG;
                        space = MIN(2 * space, cap);
                        إذا (!greedy_realloc(dst, space, 1))
                                أرجع -ENOMEM;
                        space = MALLOC_SIZEOF_محفوظ(*dst);
                        output.dst = *dst;
                        output.ناپ = MIN(space, cap);
                        استمر;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                إذا (داخلہ.pos >= داخلہ.ناپ)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_ناپ = output.pos;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح decompress_blob_gzip(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **dst,
                size_t *dst_ناپ,
                size_t dst_max) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_ناپ);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflate_سرا_wrapper) z_دھارا s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخہ, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -ENOMEM;

        size_t space = MIN3(src_ناپ * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.اگلا_وچ = (فراغ*) src;
        s.avail_وچ = src_ناپ;
        s.اگلا_باہرلا = *dst;
        s.avail_باہرلا = space;

        لكل (;;) {
                size_t ورتیا;

                r = sym_inflate(&s, Z_NO_FLUSH);
                إذا (r == Z_دھارا_سرا)
                        اكسر;
                إذا (!وچ_سیٹ(r, Z_OK, Z_BUF_ERROR))
                        أرجع -EBADMSG;

                إذا (dst_max > 0 && (space - s.avail_باہرلا) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                ورتیا = space - s.avail_باہرلا;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_باہرلا = space - ورتیا;
                s.اگلا_باہرلا = *(uint8_t**)dst + ورتیا;
        }

        *dst_ناپ = space - s.avail_باہرلا;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح decompress_blob_bzip2(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **dst,
                size_t *dst_ناپ,
                size_t dst_max) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(dst);
        أكد(dst_ناپ);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(BZ2_bz_Decompress_سرا_wrapper) bz_دھارا s = {};

        إذا (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                أرجع -EIO;

        size_t space = MIN3(src_ناپ * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.اگلا_وچ = (حرف*) src;
        s.avail_وچ = src_ناپ;
        s.اگلا_باہرلا = (حرف*) *dst;
        s.avail_باہرلا = space;

        لكل (;;) {
                size_t ورتیا;

                r = sym_BZ2_bzDecompress(&s);
                إذا (r == BZ_دھارا_سرا)
                        اكسر;
                إذا (r != BZ_OK)
                        أرجع -EBADMSG;

                إذا (dst_max > 0 && (space - s.avail_باہرلا) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                ورتیا = space - s.avail_باہرلا;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_باہرلا = space - ورتیا;
                s.اگلا_باہرلا = (حرف*) *dst + ورتیا;
        }

        *dst_ناپ = space - s.avail_باہرلا;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

صحيح decompress_blob_journal(
                Compression compression,
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **dst,
                size_t *dst_ناپ,
                size_t dst_max) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع decompress_blob_xz(
                                src, src_ناپ,
                                dst, dst_ناپ, dst_max);
        حالة COMPRESSION_LZ4:
                أرجع decompress_blob_lz4(
                                src, src_ناپ,
                                dst, dst_ناپ, dst_max);
        حالة COMPRESSION_ZSTD:
                أرجع decompress_blob_zstd(
                                src, src_ناپ,
                                dst, dst_ناپ, dst_max);
        افتراضي:
                أرجع -EPROTONOSUPPORT;
        }
}

صحيح decompress_blob(
                Compression compression,
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **dst,
                size_t *dst_ناپ,
                size_t dst_max) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع decompress_blob_gzip(
                                src, src_ناپ,
                                dst, dst_ناپ, dst_max);
        حالة COMPRESSION_BZIP2:
                أرجع decompress_blob_bzip2(
                                src, src_ناپ,
                                dst, dst_ناپ, dst_max);
        افتراضي:
                أرجع decompress_blob_journal(
                                compression,
                                src, src_ناپ,
                                dst, dst_ناپ, dst_max);
        }
}

صحيح decompress_zlib_raw(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ *dst,
                size_t dst_ناپ,
                صحيح wbits) {

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_ناپ > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflate_سرا_wrapper) z_دھارا s = {
                .اگلا_وچ = (فراغ*) src,
                .avail_وچ = src_ناپ,
                .اگلا_باہرلا = dst,
                .avail_باہرلا = dst_ناپ,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_نسخہ, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -EIO;

        r = sym_inflate(&s, Z_انت);
        size_t produced = (uint8_t*) s.اگلا_باہرلا - (uint8_t*) dst;

        إذا (r != Z_دھارا_سرا || produced != dst_ناپ)
                أرجع -EBADMSG;

        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح decompress_startswith_xz(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **بفر,
                ثابت فراغ *اگیتر,
                size_t اگیتر_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(بفر);
        أكد(اگیتر);

#إذا HAVE_XZ
        صحيح r;

        r = dlopen_xz(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        _cleanup_(lzma_سرا_wrapper) lzma_دھارا s = LZMA_دھارا_INIT;
        lzma_ret ret = sym_lzma_دھارا_decoder(&s, UINT64_MAX, /* flags= */ 0);
        إذا (ret != LZMA_OK)
                أرجع -EBADMSG;

        إذا (!(greedy_realloc(بفر, ALIGN_8(اگیتر_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_محفوظ(*بفر);

        s.اگلا_وچ = src;
        s.avail_وچ = src_ناپ;

        s.اگلا_باہرلا = *بفر;
        s.avail_باہرلا = allocated;

        لكل (;;) {
                ret = sym_lzma_کوڈ(&s, LZMA_انت);

                إذا (!وچ_سیٹ(ret, LZMA_OK, LZMA_دھارا_سرا))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_باہرلا >= اگیتر_len + 1)
                        أرجع memcmp(*بفر, اگیتر, اگیتر_len) == 0 &&
                                ((ثابت uint8_t*) *بفر)[اگیتر_len] == extra;

                إذا (ret == LZMA_دھارا_سرا)
                        أرجع 0;

                s.avail_باہرلا += allocated;

                إذا (!(greedy_realloc(بفر, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_محفوظ(*بفر);
                s.اگلا_باہرلا = *(uint8_t**)بفر + allocated - s.avail_باہرلا;
        }

#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح decompress_startswith_lz4(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **بفر,
                ثابت فراغ *اگیتر,
                size_t اگیتر_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(بفر);
        أكد(اگیتر);

#إذا HAVE_LZ4
        size_t allocated;
        صحيح r;

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ <= 8)
                أرجع -EBADMSG;

        إذا (src_ناپ - 8 > ودھ_توں_ودھ_عدد)
                أرجع -EFBIG;

        إذا (!(greedy_realloc(بفر, ALIGN_8(اگیتر_len + 1), 1)))
                أرجع -ENOMEM;
        allocated = MALLOC_SIZEOF_محفوظ(*بفر);

        r = sym_LZ4_decompress_محفوظ_partial(
                        (حرف*)src + 8,
                        *بفر,
                        src_ناپ - 8,
                        اگیتر_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        إذا (r < 0 || (size_t) r < اگیتر_len + 1) {
                size_t ناپ;

                إذا (sym_LZ4_نسخہ_نمبر() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        أرجع 0;

                إذا (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        إذا (memcmp(*بفر, اگیتر, r) != 0)
                                أرجع 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_ناپ, بفر, &ناپ, 0);
                إذا (r < 0)
                        أرجع r;

                إذا (ناپ < اگیتر_len + 1)
                        أرجع 0;
        }

        أرجع memcmp(*بفر, اگیتر, اگیتر_len) == 0 &&
                ((ثابت uint8_t*) *بفر)[اگیتر_len] == extra;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح decompress_startswith_zstd(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **بفر,
                ثابت فراغ *اگیتر,
                size_t اگیتر_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(بفر);
        أكد(اگیتر);

#إذا HAVE_ZSTD
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        بنانشانی لما لما ناپ = sym_ZSTD_get_چوکھٹا_Content_ناپ(src, src_ناپ);
        إذا (ناپ == ZSTD_CONTENTSIZE_ERROR)
                أرجع -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        إذا (ناپ != ZSTD_CONTENTSIZE_UNKNOWN && ناپ < اگیتر_len + 1)
                أرجع 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        إذا (!dctx)
                أرجع -ENOMEM;

        إذا (!(greedy_realloc(بفر, MAX(sym_ZSTD_DStream_باہرلا_ناپ(), اگیتر_len + 1), 1)))
                أرجع -ENOMEM;

        ZSTD_وچ_بفر داخلہ = {
                .src = src,
                .ناپ = src_ناپ,
        };
        ZSTD_باہرلا_بفر output = {
                .dst = *بفر,
                .ناپ = MALLOC_SIZEOF_محفوظ(*بفر),
        };
        size_t k;

        k = sym_ZSTD_decompress_دھارا(dctx, &output, &داخلہ);
        إذا (sym_ZSTD_isError(k))
                أرجع log_debug_errno(zstd_ret_منزل_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        إذا (output.pos < اگیتر_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                إذا (k != 0)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                أرجع 0; /* Decompressed text too short to match the prefix and extra */
        }

        أرجع memcmp(*بفر, اگیتر, اگیتر_len) == 0 &&
                ((ثابت uint8_t*) *بفر)[اگیتر_len] == extra;
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح decompress_startswith_gzip(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **بفر,
                ثابت فراغ *اگیتر,
                size_t اگیتر_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(بفر);
        أكد(اگیتر);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflate_سرا_wrapper) z_دھارا s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخہ, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -EBADMSG;

        إذا (!(greedy_realloc(بفر, ALIGN_8(اگیتر_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_محفوظ(*بفر);

        s.اگلا_وچ = (فراغ*) src;
        s.avail_وچ = src_ناپ;

        s.اگلا_باہرلا = *بفر;
        s.avail_باہرلا = MIN(allocated, (size_t) UINT_MAX);

        لكل (;;) {
                r = sym_inflate(&s, Z_انت);

                إذا (!وچ_سیٹ(r, Z_OK, Z_دھارا_سرا, Z_BUF_ERROR))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_باہرلا >= اگیتر_len + 1)
                        أرجع memcmp(*بفر, اگیتر, اگیتر_len) == 0 &&
                                ((ثابت uint8_t*) *بفر)[اگیتر_len] == extra;

                إذا (r == Z_دھارا_سرا)
                        أرجع 0;

                إذا (s.avail_باہرلا > 0)
                        أرجع -EBADMSG; /* truncated */

                size_t ورتیا = allocated - s.avail_باہرلا;

                إذا (!(greedy_realloc(بفر, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_محفوظ(*بفر);
                s.avail_باہرلا = MIN(allocated - ورتیا, (size_t) UINT_MAX);
                s.اگلا_باہرلا = *(uint8_t**)بفر + ورتیا;
        }
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

ساكن صحيح decompress_startswith_bzip2(
                ثابت فراغ *src,
                uint64_t src_ناپ,
                فراغ **بفر,
                ثابت فراغ *اگیتر,
                size_t اگیتر_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_ناپ > 0);
        أكد(بفر);
        أكد(اگیتر);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_ناپ > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(BZ2_bz_Decompress_سرا_wrapper) bz_دھارا s = {};

        إذا (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                أرجع -EIO;

        إذا (!(greedy_realloc(بفر, ALIGN_8(اگیتر_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_محفوظ(*بفر);

        s.اگلا_وچ = (حرف*) src;
        s.avail_وچ = src_ناپ;

        s.اگلا_باہرلا = *بفر;
        s.avail_باہرلا = MIN(allocated, (size_t) UINT_MAX);

        لكل (;;) {
                r = sym_BZ2_bzDecompress(&s);

                إذا (!وچ_سیٹ(r, BZ_OK, BZ_دھارا_سرا))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_باہرلا >= اگیتر_len + 1)
                        أرجع memcmp(*بفر, اگیتر, اگیتر_len) == 0 &&
                                ((ثابت uint8_t*) *بفر)[اگیتر_len] == extra;

                إذا (r == BZ_دھارا_سرا)
                        أرجع 0;

                إذا (s.avail_باہرلا > 0)
                        أرجع -EBADMSG; /* truncated */

                size_t ورتیا = allocated - s.avail_باہرلا;

                إذا (!(greedy_realloc(بفر, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_محفوظ(*بفر);
                s.avail_باہرلا = MIN(allocated - ورتیا, (size_t) UINT_MAX);
                s.اگلا_باہرلا = (حرف*) *بفر + ورتیا;
        }
#وإلا
        أرجع -EPROTONOSUPPORT;
#جے_انت
}

صحيح decompress_startswith_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_ناپ,
                فراغ **بفر,
                ثابت فراغ *اگیتر, size_t اگیتر_len,
                uint8_t extra) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع decompress_startswith_xz(src, src_ناپ, بفر, اگیتر, اگیتر_len, extra);
        حالة COMPRESSION_LZ4:
                أرجع decompress_startswith_lz4(src, src_ناپ, بفر, اگیتر, اگیتر_len, extra);
        حالة COMPRESSION_ZSTD:
                أرجع decompress_startswith_zstd(src, src_ناپ, بفر, اگیتر, اگیتر_len, extra);
        افتراضي:
                أرجع -EOPNOTSUPP;
        }
}

صحيح decompress_startswith(
                Compression compression,
                ثابت فراغ *src, uint64_t src_ناپ,
                فراغ **بفر,
                ثابت فراغ *اگیتر, size_t اگیتر_len,
                uint8_t extra) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع decompress_startswith_gzip(src, src_ناپ, بفر, اگیتر, اگیتر_len, extra);
        حالة COMPRESSION_BZIP2:
                أرجع decompress_startswith_bzip2(src, src_ناپ, بفر, اگیتر, اگیتر_len, extra);
        افتراضي:
                أرجع decompress_startswith_journal(compression, src, src_ناپ, بفر, اگیتر, اگیتر_len, extra);
        }
}

صحيح compress_دھارا(
                Compression type,
                صحيح fdf, صحيح fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_ناپ) {

        _cleanup_(compressor_freep) Compressor *c = عدم;
        _cleanup_free_ فراغ *buf = عدم;
        _cleanup_free_ uint8_t *داخلہ = عدم;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t کل_وچ = 0, کل_باہرلا = 0;
        صحيح r;

        أكد(fdf >= 0);
        أكد(fdt >= 0);

        r = compressor_new(&c, type);
        إذا (r < 0)
                أرجع r;

        داخلہ = new(uint8_t, COMPRESS_PIPE_بفر_ناپ);
        إذا (!داخلہ)
                أرجع -ENOMEM;

        لكل (;;) {
                size_t m = COMPRESS_PIPE_بفر_ناپ;
                ssize_t n;

                إذا (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = اقرأ(fdf, داخلہ, m);
                إذا (n < 0)
                        أرجع -errno;

                إذا (n == 0) {
                        r = compressor_انت(c, &buf, &buf_size, &buf_alloc);
                        إذا (r < 0)
                                أرجع r;

                        إذا (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                إذا (r < 0)
                                        أرجع r;
                                کل_باہرلا += buf_size;
                        }
                        اكسر;
                }

                کل_وچ += n;
                إذا (max_bytes != UINT64_MAX) {
                        أكد(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_شروع(c, داخلہ, n, &buf, &buf_size, &buf_alloc);
                إذا (r < 0)
                        أرجع r;

                إذا (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        إذا (r < 0)
                                أرجع r;
                        کل_باہرلا += buf_size;
                }
        }

        إذا (ret_uncompressed_ناپ)
                *ret_uncompressed_ناپ = کل_وچ;

        إذا (کل_وچ == 0)
                log_debug("%s compression finished (no input data)", compression_منزل_string(type));
        وإلا
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_منزل_string(type), کل_وچ, کل_باہرلا, (مزدوج) کل_باہرلا / کل_وچ * 100);

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

        إذا (FLAGS_سیٹ(flags, O_APPEND))
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

بنية decompress_دھارا_userdata {
        صحيح fd;
        uint64_t max_bytes;
        uint64_t کل_باہرلا;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ساكن صحيح decompress_دھارا_write_callback(ثابت فراغ *انکڑے, size_t ناپ, فراغ *userdata) {
        بنية decompress_دھارا_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        إذا (u->max_bytes != UINT64_MAX) {
                إذا (u->max_bytes < ناپ)
                        أرجع -EFBIG;
                u->max_bytes -= ناپ;
        }

        u->کل_باہرلا += ناپ;

        إذا (u->skip > 0) {
                إذا (u->skip >= ناپ) {
                        u->skip -= ناپ;
                        أرجع 0;
                }

                انکڑے = (ثابت uint8_t*) انکڑے + u->skip;
                ناپ -= u->skip;
                u->skip = 0;
        }

        إذا (ناپ == 0)
                أرجع 0;

        إذا (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, انکڑے, ناپ, 64);
                إذا (k < 0)
                        أرجع (صحيح) k;
                أرجع 0;
        }

        أرجع loop_write(u->fd, انکڑے, ناپ);
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
                        (FLAGS_سیٹ(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_سیٹ(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_دھارا_decoder(&c->xz, UINT64_MAX, xz_flags);
                إذا (xz_ret != LZMA_OK)
                        أرجع -EIO;
                اكسر;
        }
#جے_انت

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4_F_نسخہ);
                إذا (sym_LZ4F_isError(rc))
                        أرجع -ENOMEM;

                اكسر;
        }
#جے_انت

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                إذا (!c->d_zstd)
                        أرجع -ENOMEM;
                اكسر;
#جے_انت

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخہ, (صحيح) حجم(c->gzip)) != Z_OK)
                        أرجع -EIO;
                اكسر;
#جے_انت

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        أرجع -EIO;
                اكسر;
#جے_انت

        افتراضي:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        أرجع 0;
}

ساكن صحيح decompress_blob_منزل_fd_impl(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_ناپ,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ناپ) {

        صحيح r;

        أكد(src);
        أكد(fdt >= 0);

        إذا (type == COMPRESSION_NONE)
                أرجع -EOPNOTSUPP;

        إذا (src_ناپ == 0)
                أرجع -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        إذا (type == COMPRESSION_LZ4 &&
            src_ناپ >= COMPRESSION_MAGIC_بائٹاں_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                إذا (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        إذا (src_ناپ <= حجم(uint64_t))
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
                size_t ناپ = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_ناپ, &buf, &ناپ,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                إذا (r < 0)
                        أرجع r;

                /* Check the result size again for safety. */
                إذا (ناپ < skip)
                        أرجع -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, ناپ - skip);
                إذا (r < 0)
                        أرجع r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_ناپ, ناپ, (مزدوج) ناپ / src_ناپ * 100);

                إذا (ret_ناپ)
                        *ret_ناپ = ناپ - skip;
                أرجع 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = عدم;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        إذا (r < 0)
                أرجع r;

        بنية decompress_دھارا_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ڈالو(c, src, src_ناپ, decompress_دھارا_write_callback, &userdata);
        إذا (r < 0)
                أرجع r;

        /* Finalize */
        r = decompressor_ڈالو(c, /* data= */ عدم, /* size= */ 0, decompress_دھارا_write_callback, &userdata);
        إذا (r < 0)
                أرجع r;

        إذا (userdata.کل_باہرلا < skip)
                أرجع -EBADMSG;

        إذا (userdata.sparse) {
                r = finalize_sparse(fdt);
                إذا (r < 0)
                        أرجع r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_منزل_string(type), src_ناپ, userdata.کل_باہرلا,
                  (مزدوج) userdata.کل_باہرلا / src_ناپ * 100);

        إذا (ret_ناپ)
                *ret_ناپ = userdata.کل_باہرلا - skip;

        أرجع 0;
}

صحيح decompress_blob_منزل_fd_journal(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_ناپ,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ناپ) {

        صحيح r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        أرجع decompress_blob_منزل_fd_impl(type, src, src_ناپ, fdt, max_bytes, skip, ret_ناپ);
}

صحيح decompress_blob_منزل_fd(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_ناپ,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ناپ) {

        صحيح r;

        r = dlopen_compress(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        أرجع decompress_blob_منزل_fd_impl(type, src, src_ناپ, fdt, max_bytes, skip, ret_ناپ);
}

صحيح decompress_دھارا_بھریا(
                Compression type,
                صحيح fdf, صحيح fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = عدم;
        _cleanup_free_ uint8_t *buf = عدم;
        uint64_t کل_وچ = 0;
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

        بنية decompress_دھارا_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_بفر_ناپ);
        إذا (!buf)
                أرجع -ENOMEM;

        لكل (;;) {
                ssize_t n;

                n = اقرأ(fdf, buf, COMPRESS_PIPE_بفر_ناپ);
                إذا (n < 0)
                        أرجع -errno;

                کل_وچ += n;

                r = decompressor_ڈالو(c, buf, n, decompress_دھارا_write_callback, &userdata);
                إذا (r < 0)
                        أرجع r;

                إذا (n == 0)
                        اكسر;
        }

        إذا (کل_وچ == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_منزل_string(type));

        إذا (userdata.sparse) {
                r = finalize_sparse(fdt);
                إذا (r < 0)
                        أرجع r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_منزل_string(type), کل_وچ, userdata.کل_باہرلا,
                  (مزدوج) userdata.کل_باہرلا / کل_وچ * 100);

        أرجع 0;
}

صحيح decompress_دھارا_by_filename(ثابت حرف *filename, صحيح fdf, صحيح fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        إذا (c == COMPRESSION_NONE)
                أرجع -EPROTONOSUPPORT;

        أرجع decompress_دھارا(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        إذا (!c)
                أرجع عدم;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:
                sym_lzma_سرا(&c->xz);
                اكسر;
#جے_انت

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4:
                إذا (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = عدم;
                        c->lz4_سرخی = mfree(c->lz4_سرخی);
                } وإلا {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = عدم;
                }
                اكسر;
#جے_انت

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
#جے_انت

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (c->encoding)
                        sym_deflate_سرا(&c->gzip);
                وإلا
                        sym_inflate_سرا(&c->gzip);
                اكسر;
#جے_انت

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (c->encoding)
                        sym_BZ2_bz_Compress_سرا(&c->bzip2);
                وإلا
                        sym_BZ2_bz_Decompress_سرا(&c->bzip2);
                اكسر;
#جے_انت

        افتراضي:
                اكسر;
        }

        أرجع mfree(c);
}

Compression compressor_type(ثابت Compressor *c) {
        أرجع c ? c->type : _COMPRESSION_INVALID;
}

صحيح decompressor_detect(Decompressor **decompressor, ثابت فراغ *انکڑے, size_t ناپ) {
        صحيح r;

        أكد(decompressor);

        إذا (*decompressor)
                أرجع 1;

        إذا (ناپ < COMPRESSION_MAGIC_بائٹاں_MAX)
                أرجع 0;

        أكد(انکڑے);

        Compression type = compression_detect_from_magic(انکڑے);
        إذا (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_منزل_string(type));

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

صحيح decompressor_ڈالو(Decompressor *c, ثابت فراغ *انکڑے, size_t ناپ, DecompressorCallback callback, فراغ *userdata) {
        صحيح r;

        أكد(c);
        أكد(انکڑے || ناپ == 0);
        أكد(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        إذا (c->encoding)
                أرجع -EINVAL;

        إذا (c->مکمل && ناپ == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                أرجع 0;

#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *بفر = عدم;
        إذا (c->type != COMPRESSION_NONE) {
                بفر = new(uint8_t, COMPRESS_PIPE_بفر_ناپ);
                إذا (!بفر)
                        أرجع -ENOMEM;
        }
#جے_انت

        بدل (c->type) {

        حالة COMPRESSION_NONE:
                إذا (c->مکمل)
                        أرجع -EBADMSG;

                إذا (ناپ == 0) {
                        c->مکمل = true;
                        أرجع 0;
                }

                r = callback(انکڑے, ناپ, userdata);
                إذا (r < 0)
                        أرجع r;

                أرجع 0;

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:
                إذا (c->مکمل)
                        أرجع -EBADMSG;

                c->xz.اگلا_وچ = انکڑے;
                c->xz.avail_وچ = ناپ;

                لكل (;;) {
                        c->xz.اگلا_باہرلا = بفر;
                        c->xz.avail_باہرلا = COMPRESS_PIPE_بفر_ناپ;

                        lzma_ret lzr = sym_lzma_کوڈ(&c->xz, ناپ == 0 ? LZMA_انت : LZMA_لگاتار);
                        إذا (!وچ_سیٹ(lzr, LZMA_OK, LZMA_دھارا_سرا))
                                أرجع -EBADMSG;

                        إذا (c->xz.avail_باہرلا > COMPRESS_PIPE_بفر_ناپ)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_بفر_ناپ - c->xz.avail_باہرلا;
                        إذا (produced > 0) {
                                r = callback(بفر, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (lzr == LZMA_دھارا_سرا) {
                                إذا (c->xz.avail_وچ > 0)
                                        أرجع -EBADMSG; /* garbage data at the end */

                                c->مکمل = true;
                                أرجع 0;
                        }

                        إذا (c->xz.avail_وچ == 0 && c->xz.avail_باہرلا > 0) {
                                إذا (ناپ == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#جے_انت

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                ثابت uint8_t *src = انکڑے;
                size_t src_بچیا = ناپ;

                c->مکمل = false;

                لكل (;;) {
                        size_t produced = COMPRESS_PIPE_بفر_ناپ;
                        size_t consumed = src_بچیا;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, بفر, &produced, src, &consumed, عدم);
                        إذا (sym_LZ4F_isError(rc))
                                أرجع -EBADMSG;

                        إذا (consumed > src_بچیا ||
                            produced > COMPRESS_PIPE_بفر_ناپ)
                                أرجع -EIO;

                        إذا (consumed > 0) {
                                أكد(src);
                                src += consumed;
                                src_بچیا -= consumed;
                        }

                        إذا (produced > 0) {
                                r = callback(بفر, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (src_بچیا == 0) {
                                إذا (rc == 0) {
                                        /* The current stream has finished. */
                                        c->مکمل = true;
                                        أرجع 0;
                                }

                                إذا (produced == COMPRESS_PIPE_بفر_ناپ)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        استمر;

                                إذا (ناپ == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
        }
#جے_انت

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_وچ_بفر داخلہ = {
                        .src =  (فراغ*) انکڑے,
                        .ناپ = ناپ,
                };

                c->مکمل = false;

                لكل (;;) {
                        ZSTD_باہرلا_بفر output = {
                                .dst = بفر,
                                .ناپ = COMPRESS_PIPE_بفر_ناپ,
                        };

                        size_t res = sym_ZSTD_decompress_دھارا(c->d_zstd, &output, &داخلہ);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EBADMSG;

                        إذا (داخلہ.pos > داخلہ.ناپ ||
                            output.pos > output.ناپ)
                                أرجع -EIO;

                        إذا (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (داخلہ.pos == داخلہ.ناپ) {
                                إذا (res == 0) {
                                        /* The current stream has finished. */
                                        c->مکمل = true;
                                        أرجع 0;
                                }

                                إذا (output.pos == COMPRESS_PIPE_بفر_ناپ)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        استمر;

                                إذا (ناپ == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
        }
#جے_انت

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (ناپ > UINT_MAX)
                        أرجع -EFBIG;

                c->gzip.اگلا_وچ = (فراغ*) انکڑے;
                c->gzip.avail_وچ = ناپ;

                لكل (;;) {
                        إذا (c->مکمل) {
                                إذا (sym_inflateReset(&c->gzip) != Z_OK)
                                        أرجع -EIO;

                                c->مکمل = false;
                        }

                        c->gzip.اگلا_باہرلا = بفر;
                        c->gzip.avail_باہرلا = COMPRESS_PIPE_بفر_ناپ;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        صحيح zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        إذا (!وچ_سیٹ(zr, Z_OK, Z_دھارا_سرا) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_وچ == 0))
                                أرجع -EBADMSG;

                        إذا (c->gzip.avail_باہرلا > COMPRESS_PIPE_بفر_ناپ)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_بفر_ناپ - c->gzip.avail_باہرلا;
                        إذا (produced > 0) {
                                r = callback(بفر, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (zr == Z_دھارا_سرا) {
                                c->مکمل = true;

                                إذا (c->gzip.avail_وچ > 0)
                                        استمر; /* New stream may be pending. */

                                أرجع 0;
                        }

                        إذا (c->gzip.avail_وچ == 0 && c->gzip.avail_باہرلا > 0) {
                                إذا (ناپ == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#جے_انت

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (ناپ > UINT_MAX)
                        أرجع -EFBIG;

                c->bzip2.اگلا_وچ = (حرف*) انکڑے;
                c->bzip2.avail_وچ = ناپ;

                لكل (;;) {
                        إذا (c->مکمل) {
                                sym_BZ2_bz_Decompress_سرا(&c->bzip2);
                                إذا (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        أرجع -EIO;

                                c->مکمل = false;
                        }

                        c->bzip2.اگلا_باہرلا = (حرف*) بفر;
                        c->bzip2.avail_باہرلا = COMPRESS_PIPE_بفر_ناپ;

                        صحيح bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        إذا (!وچ_سیٹ(bzr, BZ_OK, BZ_دھارا_سرا))
                                أرجع -EBADMSG;

                        إذا (c->bzip2.avail_باہرلا > COMPRESS_PIPE_بفر_ناپ)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_بفر_ناپ - c->bzip2.avail_باہرلا;
                        إذا (produced > 0) {
                                r = callback(بفر, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (bzr == BZ_دھارا_سرا) {
                                c->مکمل = true;

                                إذا (c->bzip2.avail_وچ > 0)
                                        استمر; /* New stream may be pending. */

                                أرجع 0;
                        }

                        إذا (c->bzip2.avail_وچ == 0 && c->bzip2.avail_باہرلا > 0) {
                                إذا (ناپ == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#جے_انت

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
#جے_انت

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4_F_نسخہ);
                إذا (sym_LZ4F_isError(rc))
                        أرجع -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t سرخی_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_سرخی = حافظہ_دیو(سرخی_bound);
                إذا (!c->lz4_سرخی)
                        أرجع -ENOMEM;

                c->lz4_سرخی_ناپ = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_سرخی, سرخی_bound, &lz4_preferences);
                إذا (sym_LZ4F_isError(c->lz4_سرخی_ناپ))
                        أرجع -EINVAL;

                اكسر;
        }
#جے_انت

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                إذا (!c->c_zstd)
                        أرجع -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_سیٹ_Parameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                إذا (sym_ZSTD_isError(z))
                        أرجع -EIO;

                z = sym_ZSTD_CCtx_سیٹ_Parameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                إذا (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                اكسر;
#جے_انت

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_نسخہ, (صحيح) حجم(c->gzip));
                إذا (r != Z_OK)
                        أرجع -EIO;

                c->type = COMPRESSION_GZIP;
                اكسر;
#جے_انت

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                إذا (r != BZ_OK)
                        أرجع -EIO;

                c->type = COMPRESSION_BZIP2;
                اكسر;
#جے_انت

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
ساكن صحيح enlarge_بفر(فراغ **بفر, size_t *بفر_ناپ, size_t *بفر_allocated, size_t need) {
        أكد(بفر);
        أكد(بفر_ناپ);
        أكد(بفر_allocated);

        need = MAX3(need, *بفر_ناپ + 1, (size_t) COMPRESS_PIPE_بفر_ناپ);
        إذا (*بفر_allocated >= need)
                أرجع 0;

        إذا (!greedy_realloc(بفر, need, 1))
                أرجع -ENOMEM;

        *بفر_allocated = MALLOC_SIZEOF_محفوظ(*بفر);
        أرجع 1;
}
#جے_انت

صحيح compressor_شروع(
                Compressor *c,
                ثابت فراغ *انکڑے,
                size_t ناپ,
                فراغ **بفر,
                size_t *بفر_ناپ,
                size_t *بفر_allocated) {

#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحيح r;
#جے_انت

        أكد(c);
        أكد(بفر);
        أكد(بفر_ناپ);
        أكد(بفر_allocated);

        إذا (!c->encoding)
                أرجع -EINVAL;

        إذا (ناپ == 0)
                أرجع 0;

        أكد(انکڑے);

        *بفر_ناپ = 0;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:

                c->xz.اگلا_وچ = انکڑے;
                c->xz.avail_وچ = ناپ;

                طالما (c->xz.avail_وچ > 0) {
                        lzma_ret lzr;

                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        c->xz.اگلا_باہرلا = (uint8_t*) *بفر + *بفر_ناپ;
                        c->xz.avail_باہرلا = *بفر_allocated - *بفر_ناپ;

                        lzr = sym_lzma_کوڈ(&c->xz, LZMA_لگاتار);
                        إذا (lzr != LZMA_OK)
                                أرجع -EIO;

                        *بفر_ناپ += (*بفر_allocated - *بفر_ناپ) - c->xz.avail_باہرلا;
                }

                اكسر;
#جے_انت

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                إذا (c->lz4_سرخی_ناپ > 0) {
                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, c->lz4_سرخی_ناپ);
                        إذا (r < 0)
                                أرجع r;

                        حافظہ_نقل(*بفر, c->lz4_سرخی, c->lz4_سرخی_ناپ);
                        *بفر_ناپ = c->lz4_سرخی_ناپ;
                        c->lz4_سرخی = mfree(c->lz4_سرخی);
                        c->lz4_سرخی_ناپ = 0;
                }

                size_t bound = sym_LZ4F_compressBound(ناپ, &lz4_preferences);
                r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, *بفر_ناپ + bound);
                إذا (r < 0)
                        أرجع r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *بفر + *بفر_ناپ,
                                                   *بفر_allocated - *بفر_ناپ,
                                                   انکڑے, ناپ, عدم);
                إذا (sym_LZ4F_isError(n))
                        أرجع -EIO;

                *بفر_ناپ += n;
                اكسر;
        }
#جے_انت

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_وچ_بفر داخلہ = {
                        .src = انکڑے,
                        .ناپ = ناپ,
                };

                طالما (داخلہ.pos < داخلہ.ناپ) {
                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        ZSTD_باہرلا_بفر output = {
                                .dst = ((uint8_t *) *بفر + *بفر_ناپ),
                                .ناپ = *بفر_allocated - *بفر_ناپ,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &داخلہ, ZSTD_e_continue);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EIO;

                        *بفر_ناپ += output.pos;
                }

                اكسر;
        }
#جے_انت

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (ناپ > UINT_MAX)
                        أرجع -EFBIG;

                c->gzip.اگلا_وچ = (فراغ*) انکڑے;
                c->gzip.avail_وچ = ناپ;

                طالما (c->gzip.avail_وچ > 0) {
                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*بفر_allocated - *بفر_ناپ, (size_t) UINT_MAX);
                        c->gzip.اگلا_باہرلا = (uint8_t*) *بفر + *بفر_ناپ;
                        c->gzip.avail_باہرلا = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        إذا (r != Z_OK)
                                أرجع -EIO;

                        *بفر_ناپ += avail - c->gzip.avail_باہرلا;
                }

                اكسر;
#جے_انت

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (ناپ > UINT_MAX)
                        أرجع -EFBIG;

                c->bzip2.اگلا_وچ = (فراغ*) انکڑے;
                c->bzip2.avail_وچ = ناپ;

                طالما (c->bzip2.avail_وچ > 0) {
                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*بفر_allocated - *بفر_ناپ, (size_t) UINT_MAX);
                        c->bzip2.اگلا_باہرلا = (فراغ*) ((uint8_t*) *بفر + *بفر_ناپ);
                        c->bzip2.avail_باہرلا = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_لگاتار);
                        إذا (r != BZ_لگاتار_OK)
                                أرجع -EIO;

                        *بفر_ناپ += avail - c->bzip2.avail_باہرلا;
                }

                اكسر;
#جے_انت

        حالة COMPRESSION_NONE:

                إذا (*بفر_allocated < ناپ) {
                        فراغ *p;

                        p = realloc(*بفر, ناپ);
                        إذا (!p)
                                أرجع -ENOMEM;

                        *بفر = p;
                        *بفر_allocated = ناپ;
                }

                حافظہ_نقل(*بفر, انکڑے, ناپ);
                *بفر_ناپ = ناپ;
                اكسر;

        افتراضي:
                أرجع -EOPNOTSUPP;
        }

        أرجع 0;
}

صحيح compressor_انت(Compressor *c, فراغ **بفر, size_t *بفر_ناپ, size_t *بفر_allocated) {
#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحيح r;
#جے_انت

        أكد(c);
        أكد(بفر);
        أكد(بفر_ناپ);
        أكد(بفر_allocated);

        إذا (!c->encoding)
                أرجع -EINVAL;

        *بفر_ناپ = 0;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_وچ = 0;

                افعل {
                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        c->xz.اگلا_باہرلا = (uint8_t*) *بفر + *بفر_ناپ;
                        c->xz.avail_باہرلا = *بفر_allocated - *بفر_ناپ;

                        lzr = sym_lzma_کوڈ(&c->xz, LZMA_انت);
                        إذا (!وچ_سیٹ(lzr, LZMA_OK, LZMA_دھارا_سرا))
                                أرجع -EIO;

                        *بفر_ناپ += (*بفر_allocated - *بفر_ناپ) - c->xz.avail_باہرلا;
                } طالما (lzr != LZMA_دھارا_سرا);

                اكسر;
        }
#جے_انت

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, bound);
                إذا (r < 0)
                        أرجع r;

                size_t n = sym_LZ4_F_compress_سرا(c->c_lz4, *بفر, *بفر_allocated, عدم);
                إذا (sym_LZ4F_isError(n))
                        أرجع -EIO;

                *بفر_ناپ = n;
                اكسر;
        }
#جے_انت

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_وچ_بفر داخلہ = {};
                size_t res;

                افعل {
                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        ZSTD_باہرلا_بفر output = {
                                .dst = ((uint8_t *) *بفر + *بفر_ناپ),
                                .ناپ = *بفر_allocated - *بفر_ناپ,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &داخلہ, ZSTD_e_سرا);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EIO;

                        *بفر_ناپ += output.pos;
                } طالما (res != 0);

                اكسر;
        }
#جے_انت

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                c->gzip.avail_وچ = 0;

                افعل {
                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*بفر_allocated - *بفر_ناپ, (size_t) UINT_MAX);
                        c->gzip.اگلا_باہرلا = (uint8_t*) *بفر + *بفر_ناپ;
                        c->gzip.avail_باہرلا = avail;

                        r = sym_deflate(&c->gzip, Z_انت);
                        إذا (!وچ_سیٹ(r, Z_OK, Z_دھارا_سرا))
                                أرجع -EIO;

                        *بفر_ناپ += avail - c->gzip.avail_باہرلا;
                } طالما (r != Z_دھارا_سرا);

                اكسر;
#جے_انت

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                c->bzip2.avail_وچ = 0;

                افعل {
                        r = enlarge_بفر(بفر, بفر_ناپ, بفر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*بفر_allocated - *بفر_ناپ, (size_t) UINT_MAX);
                        c->bzip2.اگلا_باہرلا = (فراغ*) ((uint8_t*) *بفر + *بفر_ناپ);
                        c->bzip2.avail_باہرلا = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_انت);
                        إذا (!وچ_سیٹ(r, BZ_انت_OK, BZ_دھارا_سرا))
                                أرجع -EIO;

                        *بفر_ناپ += avail - c->bzip2.avail_باہرلا;
                } طالما (r != BZ_دھارا_سرا);

                اكسر;
#جے_انت

        حالة COMPRESSION_NONE:
                اكسر;

        افتراضي:
                أرجع -EOPNOTSUPP;
        }

        أرجع 0;
}
