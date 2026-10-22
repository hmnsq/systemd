/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#إذا HAVE_XZ
#اگر_تعریف_نشده SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#پایان_اگر
#ضمن <lzma.h>
#پایان_اگر

#إذا HAVE_LZ4
#اگر_تعریف_نشده SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#پایان_اگر
#ضمن <lz4.h>
#ضمن <lz4frame.h>
#ضمن <lz4hc.h>
#پایان_اگر

#إذا HAVE_ZSTD
#اگر_تعریف_نشده SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#پایان_اگر
#ضمن <zstd.h>
#ضمن <zstd_errors.h>
#پایان_اگر

#إذا HAVE_ZLIB
#اگر_تعریف_نشده SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#پایان_اگر
#ضمن <zlib.h>
#پایان_اگر

#إذا HAVE_BZIP2
#اگر_تعریف_نشده SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#پایان_اگر
#ضمن <bzlib.h>
#پایان_اگر

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
ساكن DLSYM_PROTOTYPE(lzma_کد) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_easy_encoder) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_انتها) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_جریان_میانگیر_encode) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_جریان_decoder) = عدم;
ساكن DLSYM_PROTOTYPE(lzma_lzma_preset) = عدم;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ساكن مضمن فراغ lzma_انتها_wrapper(lzma_جریان *ls) {
        sym_lzma_انتها(ls);
}
#پایان_اگر

#إذا HAVE_LZ4
ساكن DLSYM_PROTOTYPE(LZ4F_compressBegin) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_compressBound) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_F_compress_انتها) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_compressUpdate) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_decompress) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4F_isError) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_compress_HC) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_compress_default) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_decompress_امن) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_decompress_امن_partial) = عدم;
ساكن DLSYM_PROTOTYPE(LZ4_نسخه_عدد) = عدم;

ساكن ثابت LZ4F_preferences_t lz4_preferences = {
        .قاب_Info.بلوک_اندازه_ID = 5,
};
#پایان_اگر

#إذا HAVE_ZSTD
ساكن DLSYM_PROTOTYPE(ZSTD_CCtx_مجموعه_Parameter) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_compress) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_compressStream2) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_createCCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_createDCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_CStream_درون_اندازه) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_CStream_بیرون_اندازه) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_decompress_جریان) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_DStream_درون_اندازه) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_DStream_بیرون_اندازه) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_freeCCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_freeDCtx) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_get_Error_کد) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_getErrorName) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_get_قاب_Content_اندازه) = عدم;
ساكن DLSYM_PROTOTYPE(ZSTD_isError) = عدم;

DEFINE_TRIVIAL_CLEANUP_FUNC_پر_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, عدم);

ساكن صحيح zstd_ret_مقصد_errno(size_t ret) {
        بدل (sym_ZSTD_get_Error_کد(ret)) {
        حالة ZSTD_error_dst_اندازه_too_Small:
                أرجع -ENOBUFS;
        حالة ZSTD_error_حافظه_تخصیص:
                أرجع -ENOMEM;
        افتراضي:
                أرجع -EBADMSG;
        }
}
#پایان_اگر

#إذا HAVE_ZLIB
ساكن DLSYM_PROTOTYPE(deflateInit2_) = عدم;
ساكن DLSYM_PROTOTYPE(deflate) = عدم;
ساكن DLSYM_PROTOTYPE(deflate_انتها) = عدم;
ساكن DLSYM_PROTOTYPE(inflateInit2_) = عدم;
ساكن DLSYM_PROTOTYPE(inflate) = عدم;
ساكن DLSYM_PROTOTYPE(inflate_انتها) = عدم;
ساكن DLSYM_PROTOTYPE(inflateReset) = عدم;

ساكن مضمن فراغ deflate_انتها_wrapper(z_جریان *s) {
        sym_deflate_انتها(s);
}

ساكن مضمن فراغ inflate_انتها_wrapper(z_جریان *s) {
        sym_inflate_انتها(s);
}
#پایان_اگر

#إذا HAVE_BZIP2
ساكن DLSYM_PROTOTYPE(BZ2_bzCompressInit) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzCompress) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bz_Compress_انتها) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bzDecompress) = عدم;
ساكن DLSYM_PROTOTYPE(BZ2_bz_Decompress_انتها) = عدم;

ساكن مضمن فراغ BZ2_bz_Compress_انتها_wrapper(bz_جریان *s) {
        sym_BZ2_bz_Compress_انتها(s);
}

ساكن مضمن فراغ BZ2_bz_Decompress_انتها_wrapper(bz_جریان *s) {
        sym_BZ2_bz_Decompress_انتها(s);
}
#پایان_اگر

/* Opaque Compressor/Decompressor struct definition */
بنية Compressor {
        Compression type;
        bool encoding;
        bool پایانیافته;
        اتحاد {
#إذا HAVE_XZ
                lzma_جریان xz;
#پایان_اگر
#إذا HAVE_LZ4
                بنية {
                        LZ4F_compressionContext_t c_lz4;
                        فراغ *lz4_سرآیند;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_سرآیند_اندازه;
                };
                LZ4F_decompressionContext_t d_lz4;
#پایان_اگر
#إذا HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#پایان_اگر
#إذا HAVE_ZLIB
                z_جریان gzip;
#پایان_اگر
#إذا HAVE_BZIP2
                bz_جریان bzip2;
#پایان_اگر
        };
};

#تعریف_کن ALIGN_8(l) ALIGN_مقصد(l, حجم(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#تعریف_کن ZLIB_WBITS_GZIP (15 + 16)

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
        ساكن ثابت بیعلامت supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        أرجع بیت_مجموعه(supported, c);
}

bool compression_supported(Compression c) {
        ساكن ثابت بیعلامت supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        أكد(c >= 0);
        أكد(c < _COMPRESSION_MAX);

        أرجع بیت_مجموعه(supported, c);
}

Compression compression_detect_from_magic(ثابت uint8_t داده[ساكن COMPRESSION_MAGIC_بایتها_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        إذا (memcmp(داده, (ثابت uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                أرجع COMPRESSION_GZIP;
        إذا (memcmp(داده, (ثابت uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                أرجع COMPRESSION_XZ;
        إذا (memcmp(داده, (ثابت uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                أرجع COMPRESSION_ZSTD;
        إذا (memcmp(داده, (ثابت uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                أرجع COMPRESSION_LZ4;
        إذا (memcmp(داده, (ثابت uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
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
                        DLSYM_ARG(lzma_کد),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_انتها),
                        DLSYM_ARG(lzma_جریان_میانگیر_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_جریان_decoder));
#وإلا
        أرجع log_پر_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#پایان_اگر
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
                        DLSYM_ARG(LZ4_F_compress_انتها),
                        DLSYM_ARG(LZ4F_compressUpdate),
                        DLSYM_ARG(LZ4F_createCompressionContext),
                        DLSYM_ARG(LZ4F_createDecompressionContext),
                        DLSYM_ARG(LZ4F_decompress),
                        DLSYM_ARG(LZ4F_freeCompressionContext),
                        DLSYM_ARG(LZ4F_freeDecompressionContext),
                        DLSYM_ARG(LZ4F_isError),
                        DLSYM_ARG(LZ4_compress_default),
                        DLSYM_ARG(LZ4_compress_HC),
                        DLSYM_ARG(LZ4_decompress_امن),
                        DLSYM_ARG(LZ4_decompress_امن_partial),
                        DLSYM_ARG(LZ4_نسخه_عدد));
#وإلا
        أرجع log_پر_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#پایان_اگر
}

صحيح dlopen_zstd(صحيح log_level) {
#إذا HAVE_ZSTD
        ساكن فراغ *zstd_dl = عدم;

        LIBZSTD_NOTE(suggested);

        أرجع dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_get_Error_کد),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_قاب_Content_اندازه),
                        DLSYM_ARG(ZSTD_decompress_جریان),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_بیرون_اندازه),
                        DLSYM_ARG(ZSTD_CStream_درون_اندازه),
                        DLSYM_ARG(ZSTD_CStream_بیرون_اندازه),
                        DLSYM_ARG(ZSTD_CCtx_مجموعه_Parameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_درون_اندازه),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#وإلا
        أرجع log_پر_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#پایان_اگر
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
                        DLSYM_ARG(deflate_انتها),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflate_انتها),
                        DLSYM_ARG(inflateReset));
#وإلا
        أرجع log_پر_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#پایان_اگر
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
                        DLSYM_ARG(BZ2_bz_Compress_انتها),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bz_Decompress_انتها));
#وإلا
        أرجع log_پر_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#پایان_اگر
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
                uint64_t src_اندازه,
                فراغ *dst,
                size_t dst_alloc_اندازه,
                size_t *dst_اندازه,
                صحيح level) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_alloc_اندازه > 0);
        أكد(dst_اندازه);

#إذا HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, عدم, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_نما_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, عدم }
        };
        lzma_ret ret;
        size_t بیرون_pos = 0;
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

        إذا (src_اندازه < 80)
                أرجع -ENOBUFS;

        ret = sym_lzma_جریان_میانگیر_encode(filters, LZMA_CHECK_NONE, /* allocator= */ عدم,
                                            src, src_اندازه, dst, &بیرون_pos, dst_alloc_اندازه);
        إذا (ret != LZMA_OK)
                أرجع -ENOBUFS;

        *dst_اندازه = بیرون_pos;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح compress_blob_lz4(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ *dst,
                size_t dst_alloc_اندازه,
                size_t *dst_اندازه,
                صحيح level) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_alloc_اندازه > 0);
        أكد(dst_اندازه);

#إذا HAVE_LZ4
        صحيح r;

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        إذا (src_اندازه < 9)
                أرجع -ENOBUFS;

        إذا (src_اندازه > بیشترین_صحیح)
                أرجع -EFBIG;
        إذا (dst_alloc_اندازه > بیشترین_صحیح)
                dst_alloc_اندازه = بیشترین_صحیح;

        إذا (level <= 0)
                r = sym_LZ4_compress_default(src, (حرف*)dst + 8, src_اندازه, (صحيح) dst_alloc_اندازه - 8);
        وإلا
                r = sym_LZ4_compress_HC(src, (حرف*)dst + 8, src_اندازه, (صحيح) dst_alloc_اندازه - 8, level);
        إذا (r <= 0)
                أرجع -ENOBUFS;

        unaligned_write_le64(dst, src_اندازه);
        *dst_اندازه = r + 8;

        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح compress_blob_zstd(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ *dst,
                size_t dst_alloc_اندازه,
                size_t *dst_اندازه,
                صحيح level) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_alloc_اندازه > 0);
        أكد(dst_اندازه);

#إذا HAVE_ZSTD
        size_t k;
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        k = sym_ZSTD_compress(dst, dst_alloc_اندازه, src, src_اندازه, level < 0 ? 0 : level);
        إذا (sym_ZSTD_isError(k))
                أرجع zstd_ret_مقصد_errno(k);

        *dst_اندازه = k;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح compress_blob_gzip(ثابت فراغ *src, uint64_t src_اندازه,
                       فراغ *dst, size_t dst_alloc_اندازه, size_t *dst_اندازه, صحيح level) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_alloc_اندازه > 0);
        أكد(dst_اندازه);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_alloc_اندازه > UINT_MAX)
                dst_alloc_اندازه = UINT_MAX;

        _cleanup_(deflate_انتها_wrapper) z_جریان s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_نسخه, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -ENOMEM;

        s.بعدی_درون = (فراغ*) src;
        s.avail_درون = src_اندازه;
        s.بعدی_بیرون = dst;
        s.avail_بیرون = dst_alloc_اندازه;

        r = sym_deflate(&s, Z_پایان);
        إذا (r != Z_جریان_انتها)
                أرجع -ENOBUFS;

        *dst_اندازه = dst_alloc_اندازه - s.avail_بیرون;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح compress_blob_bzip2(
                ثابت فراغ *src, uint64_t src_اندازه,
                فراغ *dst, size_t dst_alloc_اندازه, size_t *dst_اندازه, صحيح level) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_alloc_اندازه > 0);
        أكد(dst_اندازه);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_alloc_اندازه > UINT_MAX)
                dst_alloc_اندازه = UINT_MAX;

        _cleanup_(BZ2_bz_Compress_انتها_wrapper) bz_جریان s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        إذا (r != BZ_OK)
                أرجع -ENOMEM;

        s.بعدی_درون = (حرف*) src;
        s.avail_درون = src_اندازه;
        s.بعدی_بیرون = (حرف*) dst;
        s.avail_بیرون = dst_alloc_اندازه;

        r = sym_BZ2_bzCompress(&s, BZ_پایان);

        إذا (r != BZ_جریان_انتها)
                أرجع -ENOBUFS;

        *dst_اندازه = dst_alloc_اندازه - s.avail_بیرون;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

صحيح compress_blob_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_اندازه,
                فراغ *dst, size_t dst_alloc_اندازه, size_t *dst_اندازه, صحيح level) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع compress_blob_xz(src, src_اندازه, dst, dst_alloc_اندازه, dst_اندازه, level);
        حالة COMPRESSION_LZ4:
                أرجع compress_blob_lz4(src, src_اندازه, dst, dst_alloc_اندازه, dst_اندازه, level);
        حالة COMPRESSION_ZSTD:
                أرجع compress_blob_zstd(src, src_اندازه, dst, dst_alloc_اندازه, dst_اندازه, level);
        افتراضي:
                أرجع -EOPNOTSUPP;
        }
}

صحيح compress_blob(
                Compression compression,
                ثابت فراغ *src, uint64_t src_اندازه,
                فراغ *dst, size_t dst_alloc_اندازه, size_t *dst_اندازه, صحيح level) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع compress_blob_gzip(src, src_اندازه, dst, dst_alloc_اندازه, dst_اندازه, level);
        حالة COMPRESSION_BZIP2:
                أرجع compress_blob_bzip2(src, src_اندازه, dst, dst_alloc_اندازه, dst_اندازه, level);
        افتراضي:
                أرجع compress_blob_journal(compression, src, src_اندازه, dst, dst_alloc_اندازه, dst_اندازه, level);
        }
}

ساكن صحيح decompress_blob_xz(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **dst,
                size_t *dst_اندازه,
                size_t dst_max) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_اندازه);

#إذا HAVE_XZ
        صحيح r;

        r = dlopen_xz(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        _cleanup_(lzma_انتها_wrapper) lzma_جریان s = LZMA_جریان_INIT;
        lzma_ret ret = sym_lzma_جریان_decoder(&s, UINT64_MAX, /* flags= */ 0);
        إذا (ret != LZMA_OK)
                أرجع -ENOMEM;

        size_t space = MIN(src_اندازه * 2, dst_max ?: SIZE_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.بعدی_درون = src;
        s.avail_درون = src_اندازه;

        s.بعدی_بیرون = *dst;
        s.avail_بیرون = space;

        لكل (;;) {
                size_t استفاده_شده;

                ret = sym_lzma_کد(&s, LZMA_پایان);
                إذا (ret == LZMA_جریان_انتها)
                        اكسر;
                إذا (ret != LZMA_OK)
                        أرجع -ENOMEM;

                إذا (dst_max > 0 && (space - s.avail_بیرون) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                استفاده_شده = space - s.avail_بیرون;
                /* Silence static analyzers, space is bounded by allocation size */
                أكد(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_بیرون = space - استفاده_شده;
                s.بعدی_بیرون = *(uint8_t**)dst + استفاده_شده;
        }

        *dst_اندازه = space - s.avail_بیرون;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح decompress_blob_lz4(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **dst,
                size_t *dst_اندازه,
                size_t dst_max) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_اندازه);

#إذا HAVE_LZ4
        حرف* بیرون;
        صحيح r, اندازه; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه <= 8)
                أرجع -EBADMSG;

        إذا (src_اندازه - 8 > بیشترین_صحیح)
                أرجع -EFBIG;

        اندازه = unaligned_read_le64(src);
        إذا (اندازه < 0 || (بیعلامت) اندازه != unaligned_read_le64(src))
                أرجع -EFBIG;
        إذا (dst_max > 0 && (size_t) اندازه > dst_max)
                أرجع -ENOBUFS;
        بیرون = greedy_realloc(dst, اندازه, 1);
        إذا (!بیرون)
                أرجع -ENOMEM;

        r = sym_LZ4_decompress_امن((حرف*)src + 8, بیرون, src_اندازه - 8, اندازه);
        إذا (r < 0 || r != اندازه)
                أرجع -EBADMSG;

        *dst_اندازه = اندازه;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

size_t zstd_dstream_بیرون_اندازه(فراغ) {
#إذا HAVE_ZSTD
        إذا (dlopen_zstd(LOG_DEBUG) < 0)
                أرجع 0;

        أرجع sym_ZSTD_DStream_بیرون_اندازه();
#وإلا
        أرجع 0;
#پایان_اگر
}

ساكن صحيح decompress_blob_zstd(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **dst,
                size_t *dst_اندازه,
                size_t dst_max) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_اندازه);

#إذا HAVE_ZSTD
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        بیعلامت دراز دراز اندازه = sym_ZSTD_get_قاب_Content_اندازه(src, src_اندازه);
        إذا (اندازه == ZSTD_CONTENTSIZE_ERROR)
                أرجع -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        إذا (اندازه != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_بیرون_اندازه(), (size_t) MIN(اندازه, (بیعلامت دراز دراز) cap));
        وإلا {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_اندازه <= UINT64_MAX / 2 ? src_اندازه * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_بیرون_اندازه(), (size_t) MIN(twice, (uint64_t) cap));
        }

        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;
        space = MALLOC_SIZEOF_امن(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        إذا (!dctx)
                أرجع -ENOMEM;

        ZSTD_درون_میانگیر درون_داد = {
                .src = src,
                .اندازه = src_اندازه,
        };
        ZSTD_بیرون_میانگیر output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .اندازه = MIN(space, cap),
        };

        لكل (;;) {
                size_t k = sym_ZSTD_decompress_جریان(dctx, &output, &درون_داد);
                إذا (sym_ZSTD_isError(k))
                        أرجع log_debug_errno(zstd_ret_مقصد_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

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
                إذا (k == 0 && درون_داد.pos >= درون_داد.اندازه)
                        اكسر;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                إذا (output.pos == output.اندازه) {
                        إذا (space > SIZE_MAX / 2)
                                أرجع -E2BIG;
                        space = MIN(2 * space, cap);
                        إذا (!greedy_realloc(dst, space, 1))
                                أرجع -ENOMEM;
                        space = MALLOC_SIZEOF_امن(*dst);
                        output.dst = *dst;
                        output.اندازه = MIN(space, cap);
                        استمر;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                إذا (درون_داد.pos >= درون_داد.اندازه)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_اندازه = output.pos;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح decompress_blob_gzip(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **dst,
                size_t *dst_اندازه,
                size_t dst_max) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_اندازه);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflate_انتها_wrapper) z_جریان s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخه, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -ENOMEM;

        size_t space = MIN3(src_اندازه * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.بعدی_درون = (فراغ*) src;
        s.avail_درون = src_اندازه;
        s.بعدی_بیرون = *dst;
        s.avail_بیرون = space;

        لكل (;;) {
                size_t استفاده_شده;

                r = sym_inflate(&s, Z_NO_FLUSH);
                إذا (r == Z_جریان_انتها)
                        اكسر;
                إذا (!درون_مجموعه(r, Z_OK, Z_BUF_ERROR))
                        أرجع -EBADMSG;

                إذا (dst_max > 0 && (space - s.avail_بیرون) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                استفاده_شده = space - s.avail_بیرون;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_بیرون = space - استفاده_شده;
                s.بعدی_بیرون = *(uint8_t**)dst + استفاده_شده;
        }

        *dst_اندازه = space - s.avail_بیرون;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح decompress_blob_bzip2(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **dst,
                size_t *dst_اندازه,
                size_t dst_max) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(dst);
        أكد(dst_اندازه);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(BZ2_bz_Decompress_انتها_wrapper) bz_جریان s = {};

        إذا (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                أرجع -EIO;

        size_t space = MIN3(src_اندازه * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        إذا (!greedy_realloc(dst, space, 1))
                أرجع -ENOMEM;

        s.بعدی_درون = (حرف*) src;
        s.avail_درون = src_اندازه;
        s.بعدی_بیرون = (حرف*) *dst;
        s.avail_بیرون = space;

        لكل (;;) {
                size_t استفاده_شده;

                r = sym_BZ2_bzDecompress(&s);
                إذا (r == BZ_جریان_انتها)
                        اكسر;
                إذا (r != BZ_OK)
                        أرجع -EBADMSG;

                إذا (dst_max > 0 && (space - s.avail_بیرون) >= dst_max)
                        اكسر;
                إذا (dst_max > 0 && space == dst_max)
                        أرجع -ENOBUFS;

                استفاده_شده = space - s.avail_بیرون;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                إذا (!greedy_realloc(dst, space, 1))
                        أرجع -ENOMEM;

                s.avail_بیرون = space - استفاده_شده;
                s.بعدی_بیرون = (حرف*) *dst + استفاده_شده;
        }

        *dst_اندازه = space - s.avail_بیرون;
        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

صحيح decompress_blob_journal(
                Compression compression,
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **dst,
                size_t *dst_اندازه,
                size_t dst_max) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع decompress_blob_xz(
                                src, src_اندازه,
                                dst, dst_اندازه, dst_max);
        حالة COMPRESSION_LZ4:
                أرجع decompress_blob_lz4(
                                src, src_اندازه,
                                dst, dst_اندازه, dst_max);
        حالة COMPRESSION_ZSTD:
                أرجع decompress_blob_zstd(
                                src, src_اندازه,
                                dst, dst_اندازه, dst_max);
        افتراضي:
                أرجع -EPROTONOSUPPORT;
        }
}

صحيح decompress_blob(
                Compression compression,
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **dst,
                size_t *dst_اندازه,
                size_t dst_max) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع decompress_blob_gzip(
                                src, src_اندازه,
                                dst, dst_اندازه, dst_max);
        حالة COMPRESSION_BZIP2:
                أرجع decompress_blob_bzip2(
                                src, src_اندازه,
                                dst, dst_اندازه, dst_max);
        افتراضي:
                أرجع decompress_blob_journal(
                                compression,
                                src, src_اندازه,
                                dst, dst_اندازه, dst_max);
        }
}

صحيح decompress_zlib_raw(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ *dst,
                size_t dst_اندازه,
                صحيح wbits) {

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه > UINT_MAX)
                أرجع -EFBIG;
        إذا (dst_اندازه > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflate_انتها_wrapper) z_جریان s = {
                .بعدی_درون = (فراغ*) src,
                .avail_درون = src_اندازه,
                .بعدی_بیرون = dst,
                .avail_بیرون = dst_اندازه,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_نسخه, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -EIO;

        r = sym_inflate(&s, Z_پایان);
        size_t produced = (uint8_t*) s.بعدی_بیرون - (uint8_t*) dst;

        إذا (r != Z_جریان_انتها || produced != dst_اندازه)
                أرجع -EBADMSG;

        أرجع 0;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح decompress_startswith_xz(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **میانگیر,
                ثابت فراغ *پیشوند,
                size_t پیشوند_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(میانگیر);
        أكد(پیشوند);

#إذا HAVE_XZ
        صحيح r;

        r = dlopen_xz(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        _cleanup_(lzma_انتها_wrapper) lzma_جریان s = LZMA_جریان_INIT;
        lzma_ret ret = sym_lzma_جریان_decoder(&s, UINT64_MAX, /* flags= */ 0);
        إذا (ret != LZMA_OK)
                أرجع -EBADMSG;

        إذا (!(greedy_realloc(میانگیر, ALIGN_8(پیشوند_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_امن(*میانگیر);

        s.بعدی_درون = src;
        s.avail_درون = src_اندازه;

        s.بعدی_بیرون = *میانگیر;
        s.avail_بیرون = allocated;

        لكل (;;) {
                ret = sym_lzma_کد(&s, LZMA_پایان);

                إذا (!درون_مجموعه(ret, LZMA_OK, LZMA_جریان_انتها))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_بیرون >= پیشوند_len + 1)
                        أرجع memcmp(*میانگیر, پیشوند, پیشوند_len) == 0 &&
                                ((ثابت uint8_t*) *میانگیر)[پیشوند_len] == extra;

                إذا (ret == LZMA_جریان_انتها)
                        أرجع 0;

                s.avail_بیرون += allocated;

                إذا (!(greedy_realloc(میانگیر, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_امن(*میانگیر);
                s.بعدی_بیرون = *(uint8_t**)میانگیر + allocated - s.avail_بیرون;
        }

#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح decompress_startswith_lz4(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **میانگیر,
                ثابت فراغ *پیشوند,
                size_t پیشوند_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(میانگیر);
        أكد(پیشوند);

#إذا HAVE_LZ4
        size_t allocated;
        صحيح r;

        r = dlopen_lz4(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه <= 8)
                أرجع -EBADMSG;

        إذا (src_اندازه - 8 > بیشترین_صحیح)
                أرجع -EFBIG;

        إذا (!(greedy_realloc(میانگیر, ALIGN_8(پیشوند_len + 1), 1)))
                أرجع -ENOMEM;
        allocated = MALLOC_SIZEOF_امن(*میانگیر);

        r = sym_LZ4_decompress_امن_partial(
                        (حرف*)src + 8,
                        *میانگیر,
                        src_اندازه - 8,
                        پیشوند_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        إذا (r < 0 || (size_t) r < پیشوند_len + 1) {
                size_t اندازه;

                إذا (sym_LZ4_نسخه_عدد() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        أرجع 0;

                إذا (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        إذا (memcmp(*میانگیر, پیشوند, r) != 0)
                                أرجع 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_اندازه, میانگیر, &اندازه, 0);
                إذا (r < 0)
                        أرجع r;

                إذا (اندازه < پیشوند_len + 1)
                        أرجع 0;
        }

        أرجع memcmp(*میانگیر, پیشوند, پیشوند_len) == 0 &&
                ((ثابت uint8_t*) *میانگیر)[پیشوند_len] == extra;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح decompress_startswith_zstd(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **میانگیر,
                ثابت فراغ *پیشوند,
                size_t پیشوند_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(میانگیر);
        أكد(پیشوند);

#إذا HAVE_ZSTD
        صحيح r;

        r = dlopen_zstd(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        بیعلامت دراز دراز اندازه = sym_ZSTD_get_قاب_Content_اندازه(src, src_اندازه);
        إذا (اندازه == ZSTD_CONTENTSIZE_ERROR)
                أرجع -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        إذا (اندازه != ZSTD_CONTENTSIZE_UNKNOWN && اندازه < پیشوند_len + 1)
                أرجع 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        إذا (!dctx)
                أرجع -ENOMEM;

        إذا (!(greedy_realloc(میانگیر, MAX(sym_ZSTD_DStream_بیرون_اندازه(), پیشوند_len + 1), 1)))
                أرجع -ENOMEM;

        ZSTD_درون_میانگیر درون_داد = {
                .src = src,
                .اندازه = src_اندازه,
        };
        ZSTD_بیرون_میانگیر output = {
                .dst = *میانگیر,
                .اندازه = MALLOC_SIZEOF_امن(*میانگیر),
        };
        size_t k;

        k = sym_ZSTD_decompress_جریان(dctx, &output, &درون_داد);
        إذا (sym_ZSTD_isError(k))
                أرجع log_debug_errno(zstd_ret_مقصد_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        إذا (output.pos < پیشوند_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                إذا (k != 0)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                أرجع 0; /* Decompressed text too short to match the prefix and extra */
        }

        أرجع memcmp(*میانگیر, پیشوند, پیشوند_len) == 0 &&
                ((ثابت uint8_t*) *میانگیر)[پیشوند_len] == extra;
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح decompress_startswith_gzip(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **میانگیر,
                ثابت فراغ *پیشوند,
                size_t پیشوند_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(میانگیر);
        أكد(پیشوند);

#إذا HAVE_ZLIB
        صحيح r;

        r = dlopen_zlib(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(inflate_انتها_wrapper) z_جریان s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخه, (صحيح) حجم(s));
        إذا (r != Z_OK)
                أرجع -EBADMSG;

        إذا (!(greedy_realloc(میانگیر, ALIGN_8(پیشوند_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_امن(*میانگیر);

        s.بعدی_درون = (فراغ*) src;
        s.avail_درون = src_اندازه;

        s.بعدی_بیرون = *میانگیر;
        s.avail_بیرون = MIN(allocated, (size_t) UINT_MAX);

        لكل (;;) {
                r = sym_inflate(&s, Z_پایان);

                إذا (!درون_مجموعه(r, Z_OK, Z_جریان_انتها, Z_BUF_ERROR))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_بیرون >= پیشوند_len + 1)
                        أرجع memcmp(*میانگیر, پیشوند, پیشوند_len) == 0 &&
                                ((ثابت uint8_t*) *میانگیر)[پیشوند_len] == extra;

                إذا (r == Z_جریان_انتها)
                        أرجع 0;

                إذا (s.avail_بیرون > 0)
                        أرجع -EBADMSG; /* truncated */

                size_t استفاده_شده = allocated - s.avail_بیرون;

                إذا (!(greedy_realloc(میانگیر, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_امن(*میانگیر);
                s.avail_بیرون = MIN(allocated - استفاده_شده, (size_t) UINT_MAX);
                s.بعدی_بیرون = *(uint8_t**)میانگیر + استفاده_شده;
        }
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

ساكن صحيح decompress_startswith_bzip2(
                ثابت فراغ *src,
                uint64_t src_اندازه,
                فراغ **میانگیر,
                ثابت فراغ *پیشوند,
                size_t پیشوند_len,
                uint8_t extra) {

        أكد(src);
        أكد(src_اندازه > 0);
        أكد(میانگیر);
        أكد(پیشوند);

#إذا HAVE_BZIP2
        صحيح r;

        r = dlopen_bzip2(LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        إذا (src_اندازه > UINT_MAX)
                أرجع -EFBIG;

        _cleanup_(BZ2_bz_Decompress_انتها_wrapper) bz_جریان s = {};

        إذا (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                أرجع -EIO;

        إذا (!(greedy_realloc(میانگیر, ALIGN_8(پیشوند_len + 1), 1)))
                أرجع -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_امن(*میانگیر);

        s.بعدی_درون = (حرف*) src;
        s.avail_درون = src_اندازه;

        s.بعدی_بیرون = *میانگیر;
        s.avail_بیرون = MIN(allocated, (size_t) UINT_MAX);

        لكل (;;) {
                r = sym_BZ2_bzDecompress(&s);

                إذا (!درون_مجموعه(r, BZ_OK, BZ_جریان_انتها))
                        أرجع -EBADMSG;

                إذا (allocated - s.avail_بیرون >= پیشوند_len + 1)
                        أرجع memcmp(*میانگیر, پیشوند, پیشوند_len) == 0 &&
                                ((ثابت uint8_t*) *میانگیر)[پیشوند_len] == extra;

                إذا (r == BZ_جریان_انتها)
                        أرجع 0;

                إذا (s.avail_بیرون > 0)
                        أرجع -EBADMSG; /* truncated */

                size_t استفاده_شده = allocated - s.avail_بیرون;

                إذا (!(greedy_realloc(میانگیر, allocated * 2, 1)))
                        أرجع -ENOMEM;

                allocated = MALLOC_SIZEOF_امن(*میانگیر);
                s.avail_بیرون = MIN(allocated - استفاده_شده, (size_t) UINT_MAX);
                s.بعدی_بیرون = (حرف*) *میانگیر + استفاده_شده;
        }
#وإلا
        أرجع -EPROTONOSUPPORT;
#پایان_اگر
}

صحيح decompress_startswith_journal(
                Compression compression,
                ثابت فراغ *src, uint64_t src_اندازه,
                فراغ **میانگیر,
                ثابت فراغ *پیشوند, size_t پیشوند_len,
                uint8_t extra) {

        بدل (compression) {
        حالة COMPRESSION_XZ:
                أرجع decompress_startswith_xz(src, src_اندازه, میانگیر, پیشوند, پیشوند_len, extra);
        حالة COMPRESSION_LZ4:
                أرجع decompress_startswith_lz4(src, src_اندازه, میانگیر, پیشوند, پیشوند_len, extra);
        حالة COMPRESSION_ZSTD:
                أرجع decompress_startswith_zstd(src, src_اندازه, میانگیر, پیشوند, پیشوند_len, extra);
        افتراضي:
                أرجع -EOPNOTSUPP;
        }
}

صحيح decompress_startswith(
                Compression compression,
                ثابت فراغ *src, uint64_t src_اندازه,
                فراغ **میانگیر,
                ثابت فراغ *پیشوند, size_t پیشوند_len,
                uint8_t extra) {

        بدل (compression) {
        حالة COMPRESSION_GZIP:
                أرجع decompress_startswith_gzip(src, src_اندازه, میانگیر, پیشوند, پیشوند_len, extra);
        حالة COMPRESSION_BZIP2:
                أرجع decompress_startswith_bzip2(src, src_اندازه, میانگیر, پیشوند, پیشوند_len, extra);
        افتراضي:
                أرجع decompress_startswith_journal(compression, src, src_اندازه, میانگیر, پیشوند, پیشوند_len, extra);
        }
}

صحيح compress_جریان(
                Compression type,
                صحيح fdf, صحيح fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_اندازه) {

        _cleanup_(compressor_freep) Compressor *c = عدم;
        _cleanup_free_ فراغ *buf = عدم;
        _cleanup_free_ uint8_t *درون_داد = عدم;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t مجموع_درون = 0, مجموع_بیرون = 0;
        صحيح r;

        أكد(fdf >= 0);
        أكد(fdt >= 0);

        r = compressor_new(&c, type);
        إذا (r < 0)
                أرجع r;

        درون_داد = new(uint8_t, COMPRESS_PIPE_میانگیر_اندازه);
        إذا (!درون_داد)
                أرجع -ENOMEM;

        لكل (;;) {
                size_t m = COMPRESS_PIPE_میانگیر_اندازه;
                ssize_t n;

                إذا (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = اقرأ(fdf, درون_داد, m);
                إذا (n < 0)
                        أرجع -errno;

                إذا (n == 0) {
                        r = compressor_پایان(c, &buf, &buf_size, &buf_alloc);
                        إذا (r < 0)
                                أرجع r;

                        إذا (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                إذا (r < 0)
                                        أرجع r;
                                مجموع_بیرون += buf_size;
                        }
                        اكسر;
                }

                مجموع_درون += n;
                إذا (max_bytes != UINT64_MAX) {
                        أكد(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_شروع(c, درون_داد, n, &buf, &buf_size, &buf_alloc);
                إذا (r < 0)
                        أرجع r;

                إذا (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        إذا (r < 0)
                                أرجع r;
                        مجموع_بیرون += buf_size;
                }
        }

        إذا (ret_uncompressed_اندازه)
                *ret_uncompressed_اندازه = مجموع_درون;

        إذا (مجموع_درون == 0)
                log_debug("%s compression finished (no input data)", compression_مقصد_string(type));
        وإلا
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_مقصد_string(type), مجموع_درون, مجموع_بیرون, (مزدوج) مجموع_بیرون / مجموع_درون * 100);

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

        إذا (FLAGS_مجموعه(flags, O_APPEND))
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

بنية decompress_جریان_userdata {
        صحيح fd;
        uint64_t max_bytes;
        uint64_t مجموع_بیرون;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ساكن صحيح decompress_جریان_write_callback(ثابت فراغ *داده, size_t اندازه, فراغ *userdata) {
        بنية decompress_جریان_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        إذا (u->max_bytes != UINT64_MAX) {
                إذا (u->max_bytes < اندازه)
                        أرجع -EFBIG;
                u->max_bytes -= اندازه;
        }

        u->مجموع_بیرون += اندازه;

        إذا (u->skip > 0) {
                إذا (u->skip >= اندازه) {
                        u->skip -= اندازه;
                        أرجع 0;
                }

                داده = (ثابت uint8_t*) داده + u->skip;
                اندازه -= u->skip;
                u->skip = 0;
        }

        إذا (اندازه == 0)
                أرجع 0;

        إذا (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, داده, اندازه, 64);
                إذا (k < 0)
                        أرجع (صحيح) k;
                أرجع 0;
        }

        أرجع loop_write(u->fd, داده, اندازه);
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
                        (FLAGS_مجموعه(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_مجموعه(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_جریان_decoder(&c->xz, UINT64_MAX, xz_flags);
                إذا (xz_ret != LZMA_OK)
                        أرجع -EIO;
                اكسر;
        }
#پایان_اگر

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4_F_نسخه);
                إذا (sym_LZ4F_isError(rc))
                        أرجع -ENOMEM;

                اكسر;
        }
#پایان_اگر

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                إذا (!c->d_zstd)
                        أرجع -ENOMEM;
                اكسر;
#پایان_اگر

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_نسخه, (صحيح) حجم(c->gzip)) != Z_OK)
                        أرجع -EIO;
                اكسر;
#پایان_اگر

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        أرجع -EIO;
                اكسر;
#پایان_اگر

        افتراضي:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        أرجع 0;
}

ساكن صحيح decompress_blob_مقصد_fd_impl(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_اندازه,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_اندازه) {

        صحيح r;

        أكد(src);
        أكد(fdt >= 0);

        إذا (type == COMPRESSION_NONE)
                أرجع -EOPNOTSUPP;

        إذا (src_اندازه == 0)
                أرجع -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        إذا (type == COMPRESSION_LZ4 &&
            src_اندازه >= COMPRESSION_MAGIC_بایتها_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                إذا (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        إذا (src_اندازه <= حجم(uint64_t))
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
                size_t اندازه = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_اندازه, &buf, &اندازه,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                إذا (r < 0)
                        أرجع r;

                /* Check the result size again for safety. */
                إذا (اندازه < skip)
                        أرجع -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, اندازه - skip);
                إذا (r < 0)
                        أرجع r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_اندازه, اندازه, (مزدوج) اندازه / src_اندازه * 100);

                إذا (ret_اندازه)
                        *ret_اندازه = اندازه - skip;
                أرجع 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = عدم;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        إذا (r < 0)
                أرجع r;

        بنية decompress_جریان_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_افزودن(c, src, src_اندازه, decompress_جریان_write_callback, &userdata);
        إذا (r < 0)
                أرجع r;

        /* Finalize */
        r = decompressor_افزودن(c, /* data= */ عدم, /* size= */ 0, decompress_جریان_write_callback, &userdata);
        إذا (r < 0)
                أرجع r;

        إذا (userdata.مجموع_بیرون < skip)
                أرجع -EBADMSG;

        إذا (userdata.sparse) {
                r = finalize_sparse(fdt);
                إذا (r < 0)
                        أرجع r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_مقصد_string(type), src_اندازه, userdata.مجموع_بیرون,
                  (مزدوج) userdata.مجموع_بیرون / src_اندازه * 100);

        إذا (ret_اندازه)
                *ret_اندازه = userdata.مجموع_بیرون - skip;

        أرجع 0;
}

صحيح decompress_blob_مقصد_fd_journal(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_اندازه,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_اندازه) {

        صحيح r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        أرجع decompress_blob_مقصد_fd_impl(type, src, src_اندازه, fdt, max_bytes, skip, ret_اندازه);
}

صحيح decompress_blob_مقصد_fd(
                Compression type,
                ثابت فراغ *src,
                uint64_t src_اندازه,
                صحيح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_اندازه) {

        صحيح r;

        r = dlopen_compress(type, LOG_DEBUG);
        إذا (r < 0)
                أرجع r;

        أرجع decompress_blob_مقصد_fd_impl(type, src, src_اندازه, fdt, max_bytes, skip, ret_اندازه);
}

صحيح decompress_جریان_پر(
                Compression type,
                صحيح fdf, صحيح fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = عدم;
        _cleanup_free_ uint8_t *buf = عدم;
        uint64_t مجموع_درون = 0;
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

        بنية decompress_جریان_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_میانگیر_اندازه);
        إذا (!buf)
                أرجع -ENOMEM;

        لكل (;;) {
                ssize_t n;

                n = اقرأ(fdf, buf, COMPRESS_PIPE_میانگیر_اندازه);
                إذا (n < 0)
                        أرجع -errno;

                مجموع_درون += n;

                r = decompressor_افزودن(c, buf, n, decompress_جریان_write_callback, &userdata);
                إذا (r < 0)
                        أرجع r;

                إذا (n == 0)
                        اكسر;
        }

        إذا (مجموع_درون == 0)
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_مقصد_string(type));

        إذا (userdata.sparse) {
                r = finalize_sparse(fdt);
                إذا (r < 0)
                        أرجع r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_مقصد_string(type), مجموع_درون, userdata.مجموع_بیرون,
                  (مزدوج) userdata.مجموع_بیرون / مجموع_درون * 100);

        أرجع 0;
}

صحيح decompress_جریان_by_filename(ثابت حرف *filename, صحيح fdf, صحيح fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        إذا (c == COMPRESSION_NONE)
                أرجع -EPROTONOSUPPORT;

        أرجع decompress_جریان(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        إذا (!c)
                أرجع عدم;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:
                sym_lzma_انتها(&c->xz);
                اكسر;
#پایان_اگر

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4:
                إذا (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = عدم;
                        c->lz4_سرآیند = mfree(c->lz4_سرآیند);
                } وإلا {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = عدم;
                }
                اكسر;
#پایان_اگر

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
#پایان_اگر

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (c->encoding)
                        sym_deflate_انتها(&c->gzip);
                وإلا
                        sym_inflate_انتها(&c->gzip);
                اكسر;
#پایان_اگر

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (c->encoding)
                        sym_BZ2_bz_Compress_انتها(&c->bzip2);
                وإلا
                        sym_BZ2_bz_Decompress_انتها(&c->bzip2);
                اكسر;
#پایان_اگر

        افتراضي:
                اكسر;
        }

        أرجع mfree(c);
}

Compression compressor_type(ثابت Compressor *c) {
        أرجع c ? c->type : _COMPRESSION_INVALID;
}

صحيح decompressor_detect(Decompressor **decompressor, ثابت فراغ *داده, size_t اندازه) {
        صحيح r;

        أكد(decompressor);

        إذا (*decompressor)
                أرجع 1;

        إذا (اندازه < COMPRESSION_MAGIC_بایتها_MAX)
                أرجع 0;

        أكد(داده);

        Compression type = compression_detect_from_magic(داده);
        إذا (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_مقصد_string(type));

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

صحيح decompressor_افزودن(Decompressor *c, ثابت فراغ *داده, size_t اندازه, DecompressorCallback callback, فراغ *userdata) {
        صحيح r;

        أكد(c);
        أكد(داده || اندازه == 0);
        أكد(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        إذا (c->encoding)
                أرجع -EINVAL;

        إذا (c->پایانیافته && اندازه == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                أرجع 0;

#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *میانگیر = عدم;
        إذا (c->type != COMPRESSION_NONE) {
                میانگیر = new(uint8_t, COMPRESS_PIPE_میانگیر_اندازه);
                إذا (!میانگیر)
                        أرجع -ENOMEM;
        }
#پایان_اگر

        بدل (c->type) {

        حالة COMPRESSION_NONE:
                إذا (c->پایانیافته)
                        أرجع -EBADMSG;

                إذا (اندازه == 0) {
                        c->پایانیافته = true;
                        أرجع 0;
                }

                r = callback(داده, اندازه, userdata);
                إذا (r < 0)
                        أرجع r;

                أرجع 0;

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:
                إذا (c->پایانیافته)
                        أرجع -EBADMSG;

                c->xz.بعدی_درون = داده;
                c->xz.avail_درون = اندازه;

                لكل (;;) {
                        c->xz.بعدی_بیرون = میانگیر;
                        c->xz.avail_بیرون = COMPRESS_PIPE_میانگیر_اندازه;

                        lzma_ret lzr = sym_lzma_کد(&c->xz, اندازه == 0 ? LZMA_پایان : LZMA_پیاپی);
                        إذا (!درون_مجموعه(lzr, LZMA_OK, LZMA_جریان_انتها))
                                أرجع -EBADMSG;

                        إذا (c->xz.avail_بیرون > COMPRESS_PIPE_میانگیر_اندازه)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_میانگیر_اندازه - c->xz.avail_بیرون;
                        إذا (produced > 0) {
                                r = callback(میانگیر, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (lzr == LZMA_جریان_انتها) {
                                إذا (c->xz.avail_درون > 0)
                                        أرجع -EBADMSG; /* garbage data at the end */

                                c->پایانیافته = true;
                                أرجع 0;
                        }

                        إذا (c->xz.avail_درون == 0 && c->xz.avail_بیرون > 0) {
                                إذا (اندازه == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#پایان_اگر

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                ثابت uint8_t *src = داده;
                size_t src_مانده = اندازه;

                c->پایانیافته = false;

                لكل (;;) {
                        size_t produced = COMPRESS_PIPE_میانگیر_اندازه;
                        size_t consumed = src_مانده;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, میانگیر, &produced, src, &consumed, عدم);
                        إذا (sym_LZ4F_isError(rc))
                                أرجع -EBADMSG;

                        إذا (consumed > src_مانده ||
                            produced > COMPRESS_PIPE_میانگیر_اندازه)
                                أرجع -EIO;

                        إذا (consumed > 0) {
                                أكد(src);
                                src += consumed;
                                src_مانده -= consumed;
                        }

                        إذا (produced > 0) {
                                r = callback(میانگیر, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (src_مانده == 0) {
                                إذا (rc == 0) {
                                        /* The current stream has finished. */
                                        c->پایانیافته = true;
                                        أرجع 0;
                                }

                                إذا (produced == COMPRESS_PIPE_میانگیر_اندازه)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        استمر;

                                إذا (اندازه == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
        }
#پایان_اگر

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_درون_میانگیر درون_داد = {
                        .src =  (فراغ*) داده,
                        .اندازه = اندازه,
                };

                c->پایانیافته = false;

                لكل (;;) {
                        ZSTD_بیرون_میانگیر output = {
                                .dst = میانگیر,
                                .اندازه = COMPRESS_PIPE_میانگیر_اندازه,
                        };

                        size_t res = sym_ZSTD_decompress_جریان(c->d_zstd, &output, &درون_داد);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EBADMSG;

                        إذا (درون_داد.pos > درون_داد.اندازه ||
                            output.pos > output.اندازه)
                                أرجع -EIO;

                        إذا (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (درون_داد.pos == درون_داد.اندازه) {
                                إذا (res == 0) {
                                        /* The current stream has finished. */
                                        c->پایانیافته = true;
                                        أرجع 0;
                                }

                                إذا (output.pos == COMPRESS_PIPE_میانگیر_اندازه)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        استمر;

                                إذا (اندازه == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
        }
#پایان_اگر

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (اندازه > UINT_MAX)
                        أرجع -EFBIG;

                c->gzip.بعدی_درون = (فراغ*) داده;
                c->gzip.avail_درون = اندازه;

                لكل (;;) {
                        إذا (c->پایانیافته) {
                                إذا (sym_inflateReset(&c->gzip) != Z_OK)
                                        أرجع -EIO;

                                c->پایانیافته = false;
                        }

                        c->gzip.بعدی_بیرون = میانگیر;
                        c->gzip.avail_بیرون = COMPRESS_PIPE_میانگیر_اندازه;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        صحيح zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        إذا (!درون_مجموعه(zr, Z_OK, Z_جریان_انتها) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_درون == 0))
                                أرجع -EBADMSG;

                        إذا (c->gzip.avail_بیرون > COMPRESS_PIPE_میانگیر_اندازه)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_میانگیر_اندازه - c->gzip.avail_بیرون;
                        إذا (produced > 0) {
                                r = callback(میانگیر, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (zr == Z_جریان_انتها) {
                                c->پایانیافته = true;

                                إذا (c->gzip.avail_درون > 0)
                                        استمر; /* New stream may be pending. */

                                أرجع 0;
                        }

                        إذا (c->gzip.avail_درون == 0 && c->gzip.avail_بیرون > 0) {
                                إذا (اندازه == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#پایان_اگر

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (اندازه > UINT_MAX)
                        أرجع -EFBIG;

                c->bzip2.بعدی_درون = (حرف*) داده;
                c->bzip2.avail_درون = اندازه;

                لكل (;;) {
                        إذا (c->پایانیافته) {
                                sym_BZ2_bz_Decompress_انتها(&c->bzip2);
                                إذا (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        أرجع -EIO;

                                c->پایانیافته = false;
                        }

                        c->bzip2.بعدی_بیرون = (حرف*) میانگیر;
                        c->bzip2.avail_بیرون = COMPRESS_PIPE_میانگیر_اندازه;

                        صحيح bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        إذا (!درون_مجموعه(bzr, BZ_OK, BZ_جریان_انتها))
                                أرجع -EBADMSG;

                        إذا (c->bzip2.avail_بیرون > COMPRESS_PIPE_میانگیر_اندازه)
                                أرجع -EIO;

                        size_t produced = COMPRESS_PIPE_میانگیر_اندازه - c->bzip2.avail_بیرون;
                        إذا (produced > 0) {
                                r = callback(میانگیر, produced, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        إذا (bzr == BZ_جریان_انتها) {
                                c->پایانیافته = true;

                                إذا (c->bzip2.avail_درون > 0)
                                        استمر; /* New stream may be pending. */

                                أرجع 0;
                        }

                        إذا (c->bzip2.avail_درون == 0 && c->bzip2.avail_بیرون > 0) {
                                إذا (اندازه == 0)
                                        أرجع -EBADMSG; /* truncated */

                                أرجع 0; /* No progress possible with the current input. */
                        }
                }
#پایان_اگر

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
#پایان_اگر

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4_F_نسخه);
                إذا (sym_LZ4F_isError(rc))
                        أرجع -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t سرآیند_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_سرآیند = حافظه_بده(سرآیند_bound);
                إذا (!c->lz4_سرآیند)
                        أرجع -ENOMEM;

                c->lz4_سرآیند_اندازه = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_سرآیند, سرآیند_bound, &lz4_preferences);
                إذا (sym_LZ4F_isError(c->lz4_سرآیند_اندازه))
                        أرجع -EINVAL;

                اكسر;
        }
#پایان_اگر

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                إذا (!c->c_zstd)
                        أرجع -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_مجموعه_Parameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                إذا (sym_ZSTD_isError(z))
                        أرجع -EIO;

                z = sym_ZSTD_CCtx_مجموعه_Parameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                إذا (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                اكسر;
#پایان_اگر

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_نسخه, (صحيح) حجم(c->gzip));
                إذا (r != Z_OK)
                        أرجع -EIO;

                c->type = COMPRESSION_GZIP;
                اكسر;
#پایان_اگر

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                إذا (r != BZ_OK)
                        أرجع -EIO;

                c->type = COMPRESSION_BZIP2;
                اكسر;
#پایان_اگر

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
ساكن صحيح enlarge_میانگیر(فراغ **میانگیر, size_t *میانگیر_اندازه, size_t *میانگیر_allocated, size_t need) {
        أكد(میانگیر);
        أكد(میانگیر_اندازه);
        أكد(میانگیر_allocated);

        need = MAX3(need, *میانگیر_اندازه + 1, (size_t) COMPRESS_PIPE_میانگیر_اندازه);
        إذا (*میانگیر_allocated >= need)
                أرجع 0;

        إذا (!greedy_realloc(میانگیر, need, 1))
                أرجع -ENOMEM;

        *میانگیر_allocated = MALLOC_SIZEOF_امن(*میانگیر);
        أرجع 1;
}
#پایان_اگر

صحيح compressor_شروع(
                Compressor *c,
                ثابت فراغ *داده,
                size_t اندازه,
                فراغ **میانگیر,
                size_t *میانگیر_اندازه,
                size_t *میانگیر_allocated) {

#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحيح r;
#پایان_اگر

        أكد(c);
        أكد(میانگیر);
        أكد(میانگیر_اندازه);
        أكد(میانگیر_allocated);

        إذا (!c->encoding)
                أرجع -EINVAL;

        إذا (اندازه == 0)
                أرجع 0;

        أكد(داده);

        *میانگیر_اندازه = 0;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ:

                c->xz.بعدی_درون = داده;
                c->xz.avail_درون = اندازه;

                طالما (c->xz.avail_درون > 0) {
                        lzma_ret lzr;

                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        c->xz.بعدی_بیرون = (uint8_t*) *میانگیر + *میانگیر_اندازه;
                        c->xz.avail_بیرون = *میانگیر_allocated - *میانگیر_اندازه;

                        lzr = sym_lzma_کد(&c->xz, LZMA_پیاپی);
                        إذا (lzr != LZMA_OK)
                                أرجع -EIO;

                        *میانگیر_اندازه += (*میانگیر_allocated - *میانگیر_اندازه) - c->xz.avail_بیرون;
                }

                اكسر;
#پایان_اگر

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                إذا (c->lz4_سرآیند_اندازه > 0) {
                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, c->lz4_سرآیند_اندازه);
                        إذا (r < 0)
                                أرجع r;

                        کپی_حافظه(*میانگیر, c->lz4_سرآیند, c->lz4_سرآیند_اندازه);
                        *میانگیر_اندازه = c->lz4_سرآیند_اندازه;
                        c->lz4_سرآیند = mfree(c->lz4_سرآیند);
                        c->lz4_سرآیند_اندازه = 0;
                }

                size_t bound = sym_LZ4F_compressBound(اندازه, &lz4_preferences);
                r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, *میانگیر_اندازه + bound);
                إذا (r < 0)
                        أرجع r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *میانگیر + *میانگیر_اندازه,
                                                   *میانگیر_allocated - *میانگیر_اندازه,
                                                   داده, اندازه, عدم);
                إذا (sym_LZ4F_isError(n))
                        أرجع -EIO;

                *میانگیر_اندازه += n;
                اكسر;
        }
#پایان_اگر

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_درون_میانگیر درون_داد = {
                        .src = داده,
                        .اندازه = اندازه,
                };

                طالما (درون_داد.pos < درون_داد.اندازه) {
                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        ZSTD_بیرون_میانگیر output = {
                                .dst = ((uint8_t *) *میانگیر + *میانگیر_اندازه),
                                .اندازه = *میانگیر_allocated - *میانگیر_اندازه,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &درون_داد, ZSTD_e_continue);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EIO;

                        *میانگیر_اندازه += output.pos;
                }

                اكسر;
        }
#پایان_اگر

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                إذا (اندازه > UINT_MAX)
                        أرجع -EFBIG;

                c->gzip.بعدی_درون = (فراغ*) داده;
                c->gzip.avail_درون = اندازه;

                طالما (c->gzip.avail_درون > 0) {
                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*میانگیر_allocated - *میانگیر_اندازه, (size_t) UINT_MAX);
                        c->gzip.بعدی_بیرون = (uint8_t*) *میانگیر + *میانگیر_اندازه;
                        c->gzip.avail_بیرون = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        إذا (r != Z_OK)
                                أرجع -EIO;

                        *میانگیر_اندازه += avail - c->gzip.avail_بیرون;
                }

                اكسر;
#پایان_اگر

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                إذا (اندازه > UINT_MAX)
                        أرجع -EFBIG;

                c->bzip2.بعدی_درون = (فراغ*) داده;
                c->bzip2.avail_درون = اندازه;

                طالما (c->bzip2.avail_درون > 0) {
                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*میانگیر_allocated - *میانگیر_اندازه, (size_t) UINT_MAX);
                        c->bzip2.بعدی_بیرون = (فراغ*) ((uint8_t*) *میانگیر + *میانگیر_اندازه);
                        c->bzip2.avail_بیرون = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_پیاپی);
                        إذا (r != BZ_پیاپی_OK)
                                أرجع -EIO;

                        *میانگیر_اندازه += avail - c->bzip2.avail_بیرون;
                }

                اكسر;
#پایان_اگر

        حالة COMPRESSION_NONE:

                إذا (*میانگیر_allocated < اندازه) {
                        فراغ *p;

                        p = realloc(*میانگیر, اندازه);
                        إذا (!p)
                                أرجع -ENOMEM;

                        *میانگیر = p;
                        *میانگیر_allocated = اندازه;
                }

                کپی_حافظه(*میانگیر, داده, اندازه);
                *میانگیر_اندازه = اندازه;
                اكسر;

        افتراضي:
                أرجع -EOPNOTSUPP;
        }

        أرجع 0;
}

صحيح compressor_پایان(Compressor *c, فراغ **میانگیر, size_t *میانگیر_اندازه, size_t *میانگیر_allocated) {
#إذا HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        صحيح r;
#پایان_اگر

        أكد(c);
        أكد(میانگیر);
        أكد(میانگیر_اندازه);
        أكد(میانگیر_allocated);

        إذا (!c->encoding)
                أرجع -EINVAL;

        *میانگیر_اندازه = 0;

        بدل (c->type) {

#إذا HAVE_XZ
        حالة COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_درون = 0;

                افعل {
                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        c->xz.بعدی_بیرون = (uint8_t*) *میانگیر + *میانگیر_اندازه;
                        c->xz.avail_بیرون = *میانگیر_allocated - *میانگیر_اندازه;

                        lzr = sym_lzma_کد(&c->xz, LZMA_پایان);
                        إذا (!درون_مجموعه(lzr, LZMA_OK, LZMA_جریان_انتها))
                                أرجع -EIO;

                        *میانگیر_اندازه += (*میانگیر_allocated - *میانگیر_اندازه) - c->xz.avail_بیرون;
                } طالما (lzr != LZMA_جریان_انتها);

                اكسر;
        }
#پایان_اگر

#إذا HAVE_LZ4
        حالة COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, bound);
                إذا (r < 0)
                        أرجع r;

                size_t n = sym_LZ4_F_compress_انتها(c->c_lz4, *میانگیر, *میانگیر_allocated, عدم);
                إذا (sym_LZ4F_isError(n))
                        أرجع -EIO;

                *میانگیر_اندازه = n;
                اكسر;
        }
#پایان_اگر

#إذا HAVE_ZSTD
        حالة COMPRESSION_ZSTD: {
                ZSTD_درون_میانگیر درون_داد = {};
                size_t res;

                افعل {
                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        ZSTD_بیرون_میانگیر output = {
                                .dst = ((uint8_t *) *میانگیر + *میانگیر_اندازه),
                                .اندازه = *میانگیر_allocated - *میانگیر_اندازه,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &درون_داد, ZSTD_e_انتها);
                        إذا (sym_ZSTD_isError(res))
                                أرجع -EIO;

                        *میانگیر_اندازه += output.pos;
                } طالما (res != 0);

                اكسر;
        }
#پایان_اگر

#إذا HAVE_ZLIB
        حالة COMPRESSION_GZIP:
                c->gzip.avail_درون = 0;

                افعل {
                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*میانگیر_allocated - *میانگیر_اندازه, (size_t) UINT_MAX);
                        c->gzip.بعدی_بیرون = (uint8_t*) *میانگیر + *میانگیر_اندازه;
                        c->gzip.avail_بیرون = avail;

                        r = sym_deflate(&c->gzip, Z_پایان);
                        إذا (!درون_مجموعه(r, Z_OK, Z_جریان_انتها))
                                أرجع -EIO;

                        *میانگیر_اندازه += avail - c->gzip.avail_بیرون;
                } طالما (r != Z_جریان_انتها);

                اكسر;
#پایان_اگر

#إذا HAVE_BZIP2
        حالة COMPRESSION_BZIP2:
                c->bzip2.avail_درون = 0;

                افعل {
                        r = enlarge_میانگیر(میانگیر, میانگیر_اندازه, میانگیر_allocated, /* need= */ 0);
                        إذا (r < 0)
                                أرجع r;

                        size_t avail = MIN(*میانگیر_allocated - *میانگیر_اندازه, (size_t) UINT_MAX);
                        c->bzip2.بعدی_بیرون = (فراغ*) ((uint8_t*) *میانگیر + *میانگیر_اندازه);
                        c->bzip2.avail_بیرون = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_پایان);
                        إذا (!درون_مجموعه(r, BZ_پایان_OK, BZ_جریان_انتها))
                                أرجع -EIO;

                        *میانگیر_اندازه += avail - c->bzip2.avail_بیرون;
                } طالما (r != BZ_جریان_انتها);

                اكسر;
#پایان_اگر

        حالة COMPRESSION_NONE:
                اكسر;

        افتراضي:
                أرجع -EOPNOTSUPP;
        }

        أرجع 0;
}
