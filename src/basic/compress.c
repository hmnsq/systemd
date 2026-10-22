/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#यदि HAVE_XZ
#यदि_अपरिभाषितम् SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#यदि_अन्तः
#अन्तर्भाव <lzma.h>
#यदि_अन्तः

#यदि HAVE_LZ4
#यदि_अपरिभाषितम् SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#यदि_अन्तः
#अन्तर्भाव <lz4.h>
#अन्तर्भाव <lz4frame.h>
#अन्तर्भाव <lz4hc.h>
#यदि_अन्तः

#यदि HAVE_ZSTD
#यदि_अपरिभाषितम् SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#यदि_अन्तः
#अन्तर्भाव <zstd.h>
#अन्तर्भाव <zstd_errors.h>
#यदि_अन्तः

#यदि HAVE_ZLIB
#यदि_अपरिभाषितम् SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#यदि_अन्तः
#अन्तर्भाव <zlib.h>
#यदि_अन्तः

#यदि HAVE_BZIP2
#यदि_अपरिभाषितम् SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#यदि_अन्तः
#अन्तर्भाव <bzlib.h>
#यदि_अन्तः

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "bitfield.h"
#अन्तर्भाव "compress.h"
#अन्तर्भाव "dlfcn-util.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "unaligned.h"

#यदि HAVE_XZ
स्थिर DLSYM_PROTOTYPE(lzma_auto_decoder) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_संहिता) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_easy_encoder) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_अन्तः) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_प्रवाहः_बफर_encode) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_प्रवाहः_decoder) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_lzma_preset) = रिक्त;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
स्थिर अन्तःस्थ शून्य lzma_अन्तः_wrapper(lzma_प्रवाहः *ls) {
        sym_lzma_अन्तः(ls);
}
#यदि_अन्तः

#यदि HAVE_LZ4
स्थिर DLSYM_PROTOTYPE(LZ4F_compressBegin) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_compressBound) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4_F_compress_अन्तः) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_compressUpdate) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_decompress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_isError) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4_compress_HC) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4_compress_default) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4_decompress_safe) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4_संस्करणम्_सङ्ख्या) = रिक्त;

स्थिर ध्रुव LZ4F_preferences_t lz4_preferences = {
        .frameInfo.खण्डम्_आकार_ID = 5,
};
#यदि_अन्तः

#यदि HAVE_ZSTD
स्थिर DLSYM_PROTOTYPE(ZSTD_CCtx_समुच्चयः_Parameter) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_compress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_compressStream2) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_createCCtx) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_createDCtx) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_CStream_अन्तर्_आकार) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_CStream_बहिर्गमनम्_आकार) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_decompress_प्रवाहः) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_DStream_अन्तर्_आकार) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_DStream_बहिर्गमनम्_आकार) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_freeCCtx) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_freeDCtx) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_get_Error_संहिता) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_getErrorName) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_get_Frame_Content_आकार) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_isError) = रिक्त;

DEFINE_TRIVIAL_CLEANUP_FUNC_पूर्णम्_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, रिक्त);

स्थिर पूर्णाङ्क zstd_ret_गन्तव्यम्_errno(size_t ret) {
        विकल्प (sym_ZSTD_get_Error_संहिता(ret)) {
        पक्ष ZSTD_error_dst_आकार_too_Small:
                प्रत्यागम -ENOBUFS;
        पक्ष ZSTD_error_स्मृतिः_आवण्टनम्:
                प्रत्यागम -ENOMEM;
        स्वभाव:
                प्रत्यागम -EBADMSG;
        }
}
#यदि_अन्तः

#यदि HAVE_ZLIB
स्थिर DLSYM_PROTOTYPE(deflateInit2_) = रिक्त;
स्थिर DLSYM_PROTOTYPE(deflate) = रिक्त;
स्थिर DLSYM_PROTOTYPE(deflate_अन्तः) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflateInit2_) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflate) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflate_अन्तः) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflateReset) = रिक्त;

स्थिर अन्तःस्थ शून्य deflate_अन्तः_wrapper(z_प्रवाहः *s) {
        sym_deflate_अन्तः(s);
}

स्थिर अन्तःस्थ शून्य inflate_अन्तः_wrapper(z_प्रवाहः *s) {
        sym_inflate_अन्तः(s);
}
#यदि_अन्तः

#यदि HAVE_BZIP2
स्थिर DLSYM_PROTOTYPE(BZ2_bzCompressInit) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzCompress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bz_Compress_अन्तः) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzDecompress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bz_Decompress_अन्तः) = रिक्त;

स्थिर अन्तःस्थ शून्य BZ2_bz_Compress_अन्तः_wrapper(bz_प्रवाहः *s) {
        sym_BZ2_bz_Compress_अन्तः(s);
}

स्थिर अन्तःस्थ शून्य BZ2_bz_Decompress_अन्तः_wrapper(bz_प्रवाहः *s) {
        sym_BZ2_bz_Decompress_अन्तः(s);
}
#यदि_अन्तः

/* Opaque Compressor/Decompressor struct definition */
संरचना Compressor {
        Compression type;
        bool encoding;
        bool परिसमाप्तम्;
        सङ्घ {
#यदि HAVE_XZ
                lzma_प्रवाहः xz;
#यदि_अन्तः
#यदि HAVE_LZ4
                संरचना {
                        LZ4F_compressionContext_t c_lz4;
                        शून्य *lz4_शीर्षकम्;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_शीर्षकम्_आकार;
                };
                LZ4F_decompressionContext_t d_lz4;
#यदि_अन्तः
#यदि HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#यदि_अन्तः
#यदि HAVE_ZLIB
                z_प्रवाहः gzip;
#यदि_अन्तः
#यदि HAVE_BZIP2
                bz_प्रवाहः bzip2;
#यदि_अन्तः
        };
};

#परिभाषय ALIGN_8(l) ALIGN_गन्तव्यम्(l, परिमाण(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#परिभाषय ZLIB_WBITS_GZIP (15 + 16)

स्थिर ध्रुव वर्ण* ध्रुव compression_सारणी[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

स्थिर ध्रुव वर्ण* ध्रुव compression_uppercase_सारणी[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

स्थिर ध्रुव वर्ण* ध्रुव compression_extension_सारणी[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "",
        [COMPRESSION_XZ]    = ".xz",
        [COMPRESSION_LZ4]   = ".lz4",
        [COMPRESSION_ZSTD]  = ".zst",
        [COMPRESSION_GZIP]  = ".gz",
        [COMPRESSION_BZIP2] = ".bz2",
};

DEFINE_STRING_सारणी_LOOKUP(compression, Compression);
DEFINE_STRING_सारणी_LOOKUP(compression_uppercase, Compression);
DEFINE_STRING_सारणी_LOOKUP(compression_extension, Compression);

Compression compression_from_string_harder(ध्रुव वर्ण *s) {
        Compression c;

        प्रतिज्ञा(s);

        c = compression_from_string(s);
        यदि (c >= 0)
                प्रत्यागम c;

        प्रत्यागम compression_uppercase_from_string(s);
}

Compression compression_from_filename(ध्रुव वर्ण *filename) {
        Compression c;
        ध्रुव वर्ण *e;

        प्रतिज्ञा(filename);

        e = strrchr(filename, '.');
        यदि (!e)
                प्रत्यागम COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        यदि (c < 0)
                प्रत्यागम COMPRESSION_NONE;

        प्रत्यागम c;
}

bool compression_supported_journal(Compression c) {
        स्थिर ध्रुव अचिह्नित supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        प्रतिज्ञा(c >= 0);
        प्रतिज्ञा(c < _COMPRESSION_MAX);

        प्रत्यागम BIT_समुच्चयः(supported, c);
}

bool compression_supported(Compression c) {
        स्थिर ध्रुव अचिह्नित supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        प्रतिज्ञा(c >= 0);
        प्रतिज्ञा(c < _COMPRESSION_MAX);

        प्रत्यागम BIT_समुच्चयः(supported, c);
}

Compression compression_detect_from_magic(ध्रुव uint8_t दत्त[स्थिर COMPRESSION_MAGIC_अष्टकानि_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        यदि (memcmp(दत्त, (ध्रुव uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                प्रत्यागम COMPRESSION_GZIP;
        यदि (memcmp(दत्त, (ध्रुव uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                प्रत्यागम COMPRESSION_XZ;
        यदि (memcmp(दत्त, (ध्रुव uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                प्रत्यागम COMPRESSION_ZSTD;
        यदि (memcmp(दत्त, (ध्रुव uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                प्रत्यागम COMPRESSION_LZ4;
        यदि (memcmp(दत्त, (ध्रुव uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                प्रत्यागम COMPRESSION_BZIP2;

        प्रत्यागम _COMPRESSION_INVALID;
}

पूर्णाङ्क dlopen_xz(पूर्णाङ्क log_level) {
#यदि HAVE_XZ
        स्थिर शून्य *lzma_dl = रिक्त;

        LIBLZMA_NOTE(suggested);

        प्रत्यागम dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_संहिता),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_अन्तः),
                        DLSYM_ARG(lzma_प्रवाहः_बफर_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_प्रवाहः_decoder));
#अन्यथा
        प्रत्यागम log_पूर्णम्_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#यदि_अन्तः
}

पूर्णाङ्क dlopen_lz4(पूर्णाङ्क log_level) {
#यदि HAVE_LZ4
        स्थिर शून्य *lz4_dl = रिक्त;

        LIBLZ4_NOTE(suggested);

        प्रत्यागम dlopen_many_sym_or_warn(
                        &lz4_dl,
                        "liblz4.so.1", log_level,
                        DLSYM_ARG(LZ4F_compressBegin),
                        DLSYM_ARG(LZ4F_compressBound),
                        DLSYM_ARG(LZ4_F_compress_अन्तः),
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
                        DLSYM_ARG(LZ4_संस्करणम्_सङ्ख्या));
#अन्यथा
        प्रत्यागम log_पूर्णम्_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#यदि_अन्तः
}

पूर्णाङ्क dlopen_zstd(पूर्णाङ्क log_level) {
#यदि HAVE_ZSTD
        स्थिर शून्य *zstd_dl = रिक्त;

        LIBZSTD_NOTE(suggested);

        प्रत्यागम dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_get_Error_संहिता),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_Frame_Content_आकार),
                        DLSYM_ARG(ZSTD_decompress_प्रवाहः),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_बहिर्गमनम्_आकार),
                        DLSYM_ARG(ZSTD_CStream_अन्तर्_आकार),
                        DLSYM_ARG(ZSTD_CStream_बहिर्गमनम्_आकार),
                        DLSYM_ARG(ZSTD_CCtx_समुच्चयः_Parameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_अन्तर्_आकार),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#अन्यथा
        प्रत्यागम log_पूर्णम्_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#यदि_अन्तः
}

पूर्णाङ्क dlopen_zlib(पूर्णाङ्क log_level) {
#यदि HAVE_ZLIB
        स्थिर शून्य *zlib_dl = रिक्त;

        LIBZ_NOTE(suggested);

        प्रत्यागम dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflate_अन्तः),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflate_अन्तः),
                        DLSYM_ARG(inflateReset));
#अन्यथा
        प्रत्यागम log_पूर्णम्_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#यदि_अन्तः
}

पूर्णाङ्क dlopen_bzip2(पूर्णाङ्क log_level) {
#यदि HAVE_BZIP2
        स्थिर शून्य *bzip2_dl = रिक्त;

        LIBBZ2_NOTE(suggested);

        प्रत्यागम dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bz_Compress_अन्तः),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bz_Decompress_अन्तः));
#अन्यथा
        प्रत्यागम log_पूर्णम्_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#यदि_अन्तः
}

पूर्णाङ्क dlopen_compress_journal(Compression c, पूर्णाङ्क log_level) {
        विकल्प (c) {
        पक्ष COMPRESSION_NONE:
                प्रत्यागम 0;
        पक्ष COMPRESSION_XZ:
                प्रत्यागम dlopen_xz(log_level);
        पक्ष COMPRESSION_LZ4:
                प्रत्यागम dlopen_lz4(log_level);
        पक्ष COMPRESSION_ZSTD:
                प्रत्यागम dlopen_zstd(log_level);
        स्वभाव:
                प्रत्यागम -EOPNOTSUPP;
        }
}

पूर्णाङ्क dlopen_compress(Compression c, पूर्णाङ्क log_level) {
        विकल्प (c) {
        पक्ष COMPRESSION_GZIP:
                प्रत्यागम dlopen_zlib(log_level);
        पक्ष COMPRESSION_BZIP2:
                प्रत्यागम dlopen_bzip2(log_level);
        स्वभाव:
                प्रत्यागम dlopen_compress_journal(c, log_level);
        }
}

स्थिर पूर्णाङ्क compress_blob_xz(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य *dst,
                size_t dst_alloc_आकार,
                size_t *dst_आकार,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_आकार > 0);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, रिक्त, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_बहुलकम्_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, रिक्त }
        };
        lzma_ret ret;
        size_t बहिर्गमनम्_pos = 0;
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                यदि (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        प्रत्यागम -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        यदि (src_आकार < 80)
                प्रत्यागम -ENOBUFS;

        ret = sym_lzma_प्रवाहः_बफर_encode(filters, LZMA_CHECK_NONE, /* allocator= */ रिक्त,
                                            src, src_आकार, dst, &बहिर्गमनम्_pos, dst_alloc_आकार);
        यदि (ret != LZMA_OK)
                प्रत्यागम -ENOBUFS;

        *dst_आकार = बहिर्गमनम्_pos;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क compress_blob_lz4(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य *dst,
                size_t dst_alloc_आकार,
                size_t *dst_आकार,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_आकार > 0);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_LZ4
        पूर्णाङ्क r;

        r = dlopen_lz4(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        यदि (src_आकार < 9)
                प्रत्यागम -ENOBUFS;

        यदि (src_आकार > अधिकतम_पूर्णाङ्कः)
                प्रत्यागम -EFBIG;
        यदि (dst_alloc_आकार > अधिकतम_पूर्णाङ्कः)
                dst_alloc_आकार = अधिकतम_पूर्णाङ्कः;

        यदि (level <= 0)
                r = sym_LZ4_compress_default(src, (वर्ण*)dst + 8, src_आकार, (पूर्णाङ्क) dst_alloc_आकार - 8);
        अन्यथा
                r = sym_LZ4_compress_HC(src, (वर्ण*)dst + 8, src_आकार, (पूर्णाङ्क) dst_alloc_आकार - 8, level);
        यदि (r <= 0)
                प्रत्यागम -ENOBUFS;

        unaligned_write_le64(dst, src_आकार);
        *dst_आकार = r + 8;

        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क compress_blob_zstd(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य *dst,
                size_t dst_alloc_आकार,
                size_t *dst_आकार,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_आकार > 0);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_ZSTD
        size_t k;
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        k = sym_ZSTD_compress(dst, dst_alloc_आकार, src, src_आकार, level < 0 ? 0 : level);
        यदि (sym_ZSTD_isError(k))
                प्रत्यागम zstd_ret_गन्तव्यम्_errno(k);

        *dst_आकार = k;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क compress_blob_gzip(ध्रुव शून्य *src, uint64_t src_आकार,
                       शून्य *dst, size_t dst_alloc_आकार, size_t *dst_आकार, पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_आकार > 0);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार > UINT_MAX)
                प्रत्यागम -EFBIG;
        यदि (dst_alloc_आकार > UINT_MAX)
                dst_alloc_आकार = UINT_MAX;

        _cleanup_(deflate_अन्तः_wrapper) z_प्रवाहः s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_संस्करणम्, (पूर्णाङ्क) परिमाण(s));
        यदि (r != Z_OK)
                प्रत्यागम -ENOMEM;

        s.अग्र_अन्तर् = (शून्य*) src;
        s.avail_अन्तर् = src_आकार;
        s.अग्र_बहिर्गमनम् = dst;
        s.avail_बहिर्गमनम् = dst_alloc_आकार;

        r = sym_deflate(&s, Z_FINISH);
        यदि (r != Z_प्रवाहः_अन्तः)
                प्रत्यागम -ENOBUFS;

        *dst_आकार = dst_alloc_आकार - s.avail_बहिर्गमनम्;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क compress_blob_bzip2(
                ध्रुव शून्य *src, uint64_t src_आकार,
                शून्य *dst, size_t dst_alloc_आकार, size_t *dst_आकार, पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_आकार > 0);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार > UINT_MAX)
                प्रत्यागम -EFBIG;
        यदि (dst_alloc_आकार > UINT_MAX)
                dst_alloc_आकार = UINT_MAX;

        _cleanup_(BZ2_bz_Compress_अन्तः_wrapper) bz_प्रवाहः s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        यदि (r != BZ_OK)
                प्रत्यागम -ENOMEM;

        s.अग्र_अन्तर् = (वर्ण*) src;
        s.avail_अन्तर् = src_आकार;
        s.अग्र_बहिर्गमनम् = (वर्ण*) dst;
        s.avail_बहिर्गमनम् = dst_alloc_आकार;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        यदि (r != BZ_प्रवाहः_अन्तः)
                प्रत्यागम -ENOBUFS;

        *dst_आकार = dst_alloc_आकार - s.avail_बहिर्गमनम्;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

पूर्णाङ्क compress_blob_journal(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_आकार,
                शून्य *dst, size_t dst_alloc_आकार, size_t *dst_आकार, पूर्णाङ्क level) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                प्रत्यागम compress_blob_xz(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        पक्ष COMPRESSION_LZ4:
                प्रत्यागम compress_blob_lz4(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        पक्ष COMPRESSION_ZSTD:
                प्रत्यागम compress_blob_zstd(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        स्वभाव:
                प्रत्यागम -EOPNOTSUPP;
        }
}

पूर्णाङ्क compress_blob(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_आकार,
                शून्य *dst, size_t dst_alloc_आकार, size_t *dst_आकार, पूर्णाङ्क level) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                प्रत्यागम compress_blob_gzip(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        पक्ष COMPRESSION_BZIP2:
                प्रत्यागम compress_blob_bzip2(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        स्वभाव:
                प्रत्यागम compress_blob_journal(compression, src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        }
}

स्थिर पूर्णाङ्क decompress_blob_xz(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **dst,
                size_t *dst_आकार,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_XZ
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_(lzma_अन्तः_wrapper) lzma_प्रवाहः s = LZMA_प्रवाहः_INIT;
        lzma_ret ret = sym_lzma_प्रवाहः_decoder(&s, UINT64_MAX, /* flags= */ 0);
        यदि (ret != LZMA_OK)
                प्रत्यागम -ENOMEM;

        size_t space = MIN(src_आकार * 2, dst_max ?: SIZE_MAX);
        यदि (!greedy_realloc(dst, space, 1))
                प्रत्यागम -ENOMEM;

        s.अग्र_अन्तर् = src;
        s.avail_अन्तर् = src_आकार;

        s.अग्र_बहिर्गमनम् = *dst;
        s.avail_बहिर्गमनम् = space;

        प्रति (;;) {
                size_t प्रयुक्तम्;

                ret = sym_lzma_संहिता(&s, LZMA_FINISH);
                यदि (ret == LZMA_प्रवाहः_अन्तः)
                        भङ्ग;
                यदि (ret != LZMA_OK)
                        प्रत्यागम -ENOMEM;

                यदि (dst_max > 0 && (space - s.avail_बहिर्गमनम्) >= dst_max)
                        भङ्ग;
                यदि (dst_max > 0 && space == dst_max)
                        प्रत्यागम -ENOBUFS;

                प्रयुक्तम् = space - s.avail_बहिर्गमनम्;
                /* Silence static analyzers, space is bounded by allocation size */
                प्रतिज्ञा(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                यदि (!greedy_realloc(dst, space, 1))
                        प्रत्यागम -ENOMEM;

                s.avail_बहिर्गमनम् = space - प्रयुक्तम्;
                s.अग्र_बहिर्गमनम् = *(uint8_t**)dst + प्रयुक्तम्;
        }

        *dst_आकार = space - s.avail_बहिर्गमनम्;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_blob_lz4(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **dst,
                size_t *dst_आकार,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_LZ4
        वर्ण* बहिर्गमनम्;
        पूर्णाङ्क r, आकार; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार <= 8)
                प्रत्यागम -EBADMSG;

        यदि (src_आकार - 8 > अधिकतम_पूर्णाङ्कः)
                प्रत्यागम -EFBIG;

        आकार = unaligned_read_le64(src);
        यदि (आकार < 0 || (अचिह्नित) आकार != unaligned_read_le64(src))
                प्रत्यागम -EFBIG;
        यदि (dst_max > 0 && (size_t) आकार > dst_max)
                प्रत्यागम -ENOBUFS;
        बहिर्गमनम् = greedy_realloc(dst, आकार, 1);
        यदि (!बहिर्गमनम्)
                प्रत्यागम -ENOMEM;

        r = sym_LZ4_decompress_safe((वर्ण*)src + 8, बहिर्गमनम्, src_आकार - 8, आकार);
        यदि (r < 0 || r != आकार)
                प्रत्यागम -EBADMSG;

        *dst_आकार = आकार;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

size_t zstd_dstream_बहिर्गमनम्_आकार(शून्य) {
#यदि HAVE_ZSTD
        यदि (dlopen_zstd(LOG_DEBUG) < 0)
                प्रत्यागम 0;

        प्रत्यागम sym_ZSTD_DStream_बहिर्गमनम्_आकार();
#अन्यथा
        प्रत्यागम 0;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_blob_zstd(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **dst,
                size_t *dst_आकार,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_ZSTD
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        अचिह्नित दीर्घ दीर्घ आकार = sym_ZSTD_get_Frame_Content_आकार(src, src_आकार);
        यदि (आकार == ZSTD_CONTENTSIZE_ERROR)
                प्रत्यागम -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        यदि (आकार != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_बहिर्गमनम्_आकार(), (size_t) MIN(आकार, (अचिह्नित दीर्घ दीर्घ) cap));
        अन्यथा {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_आकार <= UINT64_MAX / 2 ? src_आकार * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_बहिर्गमनम्_आकार(), (size_t) MIN(twice, (uint64_t) cap));
        }

        यदि (!greedy_realloc(dst, space, 1))
                प्रत्यागम -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        यदि (!dctx)
                प्रत्यागम -ENOMEM;

        ZSTD_अन्तर्_बफर निवेशः = {
                .src = src,
                .आकार = src_आकार,
        };
        ZSTD_बहिर्गमनम्_बफर output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .आकार = MIN(space, cap),
        };

        प्रति (;;) {
                size_t k = sym_ZSTD_decompress_प्रवाहः(dctx, &output, &निवेशः);
                यदि (sym_ZSTD_isError(k))
                        प्रत्यागम log_debug_errno(zstd_ret_गन्तव्यम्_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                यदि (output.pos >= cap)
                        भङ्ग;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                यदि (k == 0 && निवेशः.pos >= निवेशः.आकार)
                        भङ्ग;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                यदि (output.pos == output.आकार) {
                        यदि (space > SIZE_MAX / 2)
                                प्रत्यागम -E2BIG;
                        space = MIN(2 * space, cap);
                        यदि (!greedy_realloc(dst, space, 1))
                                प्रत्यागम -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.आकार = MIN(space, cap);
                        अनुवर्त;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                यदि (निवेशः.pos >= निवेशः.आकार)
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_आकार = output.pos;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_blob_gzip(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **dst,
                size_t *dst_आकार,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार > UINT_MAX)
                प्रत्यागम -EFBIG;

        _cleanup_(inflate_अन्तः_wrapper) z_प्रवाहः s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_संस्करणम्, (पूर्णाङ्क) परिमाण(s));
        यदि (r != Z_OK)
                प्रत्यागम -ENOMEM;

        size_t space = MIN3(src_आकार * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        यदि (!greedy_realloc(dst, space, 1))
                प्रत्यागम -ENOMEM;

        s.अग्र_अन्तर् = (शून्य*) src;
        s.avail_अन्तर् = src_आकार;
        s.अग्र_बहिर्गमनम् = *dst;
        s.avail_बहिर्गमनम् = space;

        प्रति (;;) {
                size_t प्रयुक्तम्;

                r = sym_inflate(&s, Z_NO_FLUSH);
                यदि (r == Z_प्रवाहः_अन्तः)
                        भङ्ग;
                यदि (!अन्तर्_समुच्चयः(r, Z_OK, Z_BUF_ERROR))
                        प्रत्यागम -EBADMSG;

                यदि (dst_max > 0 && (space - s.avail_बहिर्गमनम्) >= dst_max)
                        भङ्ग;
                यदि (dst_max > 0 && space == dst_max)
                        प्रत्यागम -ENOBUFS;

                प्रयुक्तम् = space - s.avail_बहिर्गमनम्;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                यदि (!greedy_realloc(dst, space, 1))
                        प्रत्यागम -ENOMEM;

                s.avail_बहिर्गमनम् = space - प्रयुक्तम्;
                s.अग्र_बहिर्गमनम् = *(uint8_t**)dst + प्रयुक्तम्;
        }

        *dst_आकार = space - s.avail_बहिर्गमनम्;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_blob_bzip2(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **dst,
                size_t *dst_आकार,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_आकार);

#यदि HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार > UINT_MAX)
                प्रत्यागम -EFBIG;

        _cleanup_(BZ2_bz_Decompress_अन्तः_wrapper) bz_प्रवाहः s = {};

        यदि (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                प्रत्यागम -EIO;

        size_t space = MIN3(src_आकार * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        यदि (!greedy_realloc(dst, space, 1))
                प्रत्यागम -ENOMEM;

        s.अग्र_अन्तर् = (वर्ण*) src;
        s.avail_अन्तर् = src_आकार;
        s.अग्र_बहिर्गमनम् = (वर्ण*) *dst;
        s.avail_बहिर्गमनम् = space;

        प्रति (;;) {
                size_t प्रयुक्तम्;

                r = sym_BZ2_bzDecompress(&s);
                यदि (r == BZ_प्रवाहः_अन्तः)
                        भङ्ग;
                यदि (r != BZ_OK)
                        प्रत्यागम -EBADMSG;

                यदि (dst_max > 0 && (space - s.avail_बहिर्गमनम्) >= dst_max)
                        भङ्ग;
                यदि (dst_max > 0 && space == dst_max)
                        प्रत्यागम -ENOBUFS;

                प्रयुक्तम् = space - s.avail_बहिर्गमनम्;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                यदि (!greedy_realloc(dst, space, 1))
                        प्रत्यागम -ENOMEM;

                s.avail_बहिर्गमनम् = space - प्रयुक्तम्;
                s.अग्र_बहिर्गमनम् = (वर्ण*) *dst + प्रयुक्तम्;
        }

        *dst_आकार = space - s.avail_बहिर्गमनम्;
        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

पूर्णाङ्क decompress_blob_journal(
                Compression compression,
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **dst,
                size_t *dst_आकार,
                size_t dst_max) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                प्रत्यागम decompress_blob_xz(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        पक्ष COMPRESSION_LZ4:
                प्रत्यागम decompress_blob_lz4(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        पक्ष COMPRESSION_ZSTD:
                प्रत्यागम decompress_blob_zstd(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        स्वभाव:
                प्रत्यागम -EPROTONOSUPPORT;
        }
}

पूर्णाङ्क decompress_blob(
                Compression compression,
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **dst,
                size_t *dst_आकार,
                size_t dst_max) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                प्रत्यागम decompress_blob_gzip(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        पक्ष COMPRESSION_BZIP2:
                प्रत्यागम decompress_blob_bzip2(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        स्वभाव:
                प्रत्यागम decompress_blob_journal(
                                compression,
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        }
}

पूर्णाङ्क decompress_zlib_raw(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य *dst,
                size_t dst_आकार,
                पूर्णाङ्क wbits) {

#यदि HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार > UINT_MAX)
                प्रत्यागम -EFBIG;
        यदि (dst_आकार > UINT_MAX)
                प्रत्यागम -EFBIG;

        _cleanup_(inflate_अन्तः_wrapper) z_प्रवाहः s = {
                .अग्र_अन्तर् = (शून्य*) src,
                .avail_अन्तर् = src_आकार,
                .अग्र_बहिर्गमनम् = dst,
                .avail_बहिर्गमनम् = dst_आकार,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_संस्करणम्, (पूर्णाङ्क) परिमाण(s));
        यदि (r != Z_OK)
                प्रत्यागम -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.अग्र_बहिर्गमनम् - (uint8_t*) dst;

        यदि (r != Z_प्रवाहः_अन्तः || produced != dst_आकार)
                प्रत्यागम -EBADMSG;

        प्रत्यागम 0;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_startswith_xz(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **बफर,
                ध्रुव शून्य *उपसर्गः,
                size_t उपसर्गः_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(बफर);
        प्रतिज्ञा(उपसर्गः);

#यदि HAVE_XZ
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_(lzma_अन्तः_wrapper) lzma_प्रवाहः s = LZMA_प्रवाहः_INIT;
        lzma_ret ret = sym_lzma_प्रवाहः_decoder(&s, UINT64_MAX, /* flags= */ 0);
        यदि (ret != LZMA_OK)
                प्रत्यागम -EBADMSG;

        यदि (!(greedy_realloc(बफर, ALIGN_8(उपसर्गः_len + 1), 1)))
                प्रत्यागम -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*बफर);

        s.अग्र_अन्तर् = src;
        s.avail_अन्तर् = src_आकार;

        s.अग्र_बहिर्गमनम् = *बफर;
        s.avail_बहिर्गमनम् = allocated;

        प्रति (;;) {
                ret = sym_lzma_संहिता(&s, LZMA_FINISH);

                यदि (!अन्तर्_समुच्चयः(ret, LZMA_OK, LZMA_प्रवाहः_अन्तः))
                        प्रत्यागम -EBADMSG;

                यदि (allocated - s.avail_बहिर्गमनम् >= उपसर्गः_len + 1)
                        प्रत्यागम memcmp(*बफर, उपसर्गः, उपसर्गः_len) == 0 &&
                                ((ध्रुव uint8_t*) *बफर)[उपसर्गः_len] == extra;

                यदि (ret == LZMA_प्रवाहः_अन्तः)
                        प्रत्यागम 0;

                s.avail_बहिर्गमनम् += allocated;

                यदि (!(greedy_realloc(बफर, allocated * 2, 1)))
                        प्रत्यागम -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*बफर);
                s.अग्र_बहिर्गमनम् = *(uint8_t**)बफर + allocated - s.avail_बहिर्गमनम्;
        }

#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_startswith_lz4(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **बफर,
                ध्रुव शून्य *उपसर्गः,
                size_t उपसर्गः_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(बफर);
        प्रतिज्ञा(उपसर्गः);

#यदि HAVE_LZ4
        size_t allocated;
        पूर्णाङ्क r;

        r = dlopen_lz4(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार <= 8)
                प्रत्यागम -EBADMSG;

        यदि (src_आकार - 8 > अधिकतम_पूर्णाङ्कः)
                प्रत्यागम -EFBIG;

        यदि (!(greedy_realloc(बफर, ALIGN_8(उपसर्गः_len + 1), 1)))
                प्रत्यागम -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*बफर);

        r = sym_LZ4_decompress_safe_partial(
                        (वर्ण*)src + 8,
                        *बफर,
                        src_आकार - 8,
                        उपसर्गः_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        यदि (r < 0 || (size_t) r < उपसर्गः_len + 1) {
                size_t आकार;

                यदि (sym_LZ4_संस्करणम्_सङ्ख्या() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        प्रत्यागम 0;

                यदि (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        यदि (memcmp(*बफर, उपसर्गः, r) != 0)
                                प्रत्यागम 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_आकार, बफर, &आकार, 0);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (आकार < उपसर्गः_len + 1)
                        प्रत्यागम 0;
        }

        प्रत्यागम memcmp(*बफर, उपसर्गः, उपसर्गः_len) == 0 &&
                ((ध्रुव uint8_t*) *बफर)[उपसर्गः_len] == extra;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_startswith_zstd(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **बफर,
                ध्रुव शून्य *उपसर्गः,
                size_t उपसर्गः_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(बफर);
        प्रतिज्ञा(उपसर्गः);

#यदि HAVE_ZSTD
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        अचिह्नित दीर्घ दीर्घ आकार = sym_ZSTD_get_Frame_Content_आकार(src, src_आकार);
        यदि (आकार == ZSTD_CONTENTSIZE_ERROR)
                प्रत्यागम -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        यदि (आकार != ZSTD_CONTENTSIZE_UNKNOWN && आकार < उपसर्गः_len + 1)
                प्रत्यागम 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        यदि (!dctx)
                प्रत्यागम -ENOMEM;

        यदि (!(greedy_realloc(बफर, MAX(sym_ZSTD_DStream_बहिर्गमनम्_आकार(), उपसर्गः_len + 1), 1)))
                प्रत्यागम -ENOMEM;

        ZSTD_अन्तर्_बफर निवेशः = {
                .src = src,
                .आकार = src_आकार,
        };
        ZSTD_बहिर्गमनम्_बफर output = {
                .dst = *बफर,
                .आकार = MALLOC_SIZEOF_SAFE(*बफर),
        };
        size_t k;

        k = sym_ZSTD_decompress_प्रवाहः(dctx, &output, &निवेशः);
        यदि (sym_ZSTD_isError(k))
                प्रत्यागम log_debug_errno(zstd_ret_गन्तव्यम्_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        यदि (output.pos < उपसर्गः_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                यदि (k != 0)
                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                प्रत्यागम 0; /* Decompressed text too short to match the prefix and extra */
        }

        प्रत्यागम memcmp(*बफर, उपसर्गः, उपसर्गः_len) == 0 &&
                ((ध्रुव uint8_t*) *बफर)[उपसर्गः_len] == extra;
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_startswith_gzip(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **बफर,
                ध्रुव शून्य *उपसर्गः,
                size_t उपसर्गः_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(बफर);
        प्रतिज्ञा(उपसर्गः);

#यदि HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार > UINT_MAX)
                प्रत्यागम -EFBIG;

        _cleanup_(inflate_अन्तः_wrapper) z_प्रवाहः s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_संस्करणम्, (पूर्णाङ्क) परिमाण(s));
        यदि (r != Z_OK)
                प्रत्यागम -EBADMSG;

        यदि (!(greedy_realloc(बफर, ALIGN_8(उपसर्गः_len + 1), 1)))
                प्रत्यागम -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*बफर);

        s.अग्र_अन्तर् = (शून्य*) src;
        s.avail_अन्तर् = src_आकार;

        s.अग्र_बहिर्गमनम् = *बफर;
        s.avail_बहिर्गमनम् = MIN(allocated, (size_t) UINT_MAX);

        प्रति (;;) {
                r = sym_inflate(&s, Z_FINISH);

                यदि (!अन्तर्_समुच्चयः(r, Z_OK, Z_प्रवाहः_अन्तः, Z_BUF_ERROR))
                        प्रत्यागम -EBADMSG;

                यदि (allocated - s.avail_बहिर्गमनम् >= उपसर्गः_len + 1)
                        प्रत्यागम memcmp(*बफर, उपसर्गः, उपसर्गः_len) == 0 &&
                                ((ध्रुव uint8_t*) *बफर)[उपसर्गः_len] == extra;

                यदि (r == Z_प्रवाहः_अन्तः)
                        प्रत्यागम 0;

                यदि (s.avail_बहिर्गमनम् > 0)
                        प्रत्यागम -EBADMSG; /* truncated */

                size_t प्रयुक्तम् = allocated - s.avail_बहिर्गमनम्;

                यदि (!(greedy_realloc(बफर, allocated * 2, 1)))
                        प्रत्यागम -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*बफर);
                s.avail_बहिर्गमनम् = MIN(allocated - प्रयुक्तम्, (size_t) UINT_MAX);
                s.अग्र_बहिर्गमनम् = *(uint8_t**)बफर + प्रयुक्तम्;
        }
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क decompress_startswith_bzip2(
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                शून्य **बफर,
                ध्रुव शून्य *उपसर्गः,
                size_t उपसर्गः_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_आकार > 0);
        प्रतिज्ञा(बफर);
        प्रतिज्ञा(उपसर्गः);

#यदि HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (src_आकार > UINT_MAX)
                प्रत्यागम -EFBIG;

        _cleanup_(BZ2_bz_Decompress_अन्तः_wrapper) bz_प्रवाहः s = {};

        यदि (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                प्रत्यागम -EIO;

        यदि (!(greedy_realloc(बफर, ALIGN_8(उपसर्गः_len + 1), 1)))
                प्रत्यागम -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*बफर);

        s.अग्र_अन्तर् = (वर्ण*) src;
        s.avail_अन्तर् = src_आकार;

        s.अग्र_बहिर्गमनम् = *बफर;
        s.avail_बहिर्गमनम् = MIN(allocated, (size_t) UINT_MAX);

        प्रति (;;) {
                r = sym_BZ2_bzDecompress(&s);

                यदि (!अन्तर्_समुच्चयः(r, BZ_OK, BZ_प्रवाहः_अन्तः))
                        प्रत्यागम -EBADMSG;

                यदि (allocated - s.avail_बहिर्गमनम् >= उपसर्गः_len + 1)
                        प्रत्यागम memcmp(*बफर, उपसर्गः, उपसर्गः_len) == 0 &&
                                ((ध्रुव uint8_t*) *बफर)[उपसर्गः_len] == extra;

                यदि (r == BZ_प्रवाहः_अन्तः)
                        प्रत्यागम 0;

                यदि (s.avail_बहिर्गमनम् > 0)
                        प्रत्यागम -EBADMSG; /* truncated */

                size_t प्रयुक्तम् = allocated - s.avail_बहिर्गमनम्;

                यदि (!(greedy_realloc(बफर, allocated * 2, 1)))
                        प्रत्यागम -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*बफर);
                s.avail_बहिर्गमनम् = MIN(allocated - प्रयुक्तम्, (size_t) UINT_MAX);
                s.अग्र_बहिर्गमनम् = (वर्ण*) *बफर + प्रयुक्तम्;
        }
#अन्यथा
        प्रत्यागम -EPROTONOSUPPORT;
#यदि_अन्तः
}

पूर्णाङ्क decompress_startswith_journal(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_आकार,
                शून्य **बफर,
                ध्रुव शून्य *उपसर्गः, size_t उपसर्गः_len,
                uint8_t extra) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                प्रत्यागम decompress_startswith_xz(src, src_आकार, बफर, उपसर्गः, उपसर्गः_len, extra);
        पक्ष COMPRESSION_LZ4:
                प्रत्यागम decompress_startswith_lz4(src, src_आकार, बफर, उपसर्गः, उपसर्गः_len, extra);
        पक्ष COMPRESSION_ZSTD:
                प्रत्यागम decompress_startswith_zstd(src, src_आकार, बफर, उपसर्गः, उपसर्गः_len, extra);
        स्वभाव:
                प्रत्यागम -EOPNOTSUPP;
        }
}

पूर्णाङ्क decompress_startswith(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_आकार,
                शून्य **बफर,
                ध्रुव शून्य *उपसर्गः, size_t उपसर्गः_len,
                uint8_t extra) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                प्रत्यागम decompress_startswith_gzip(src, src_आकार, बफर, उपसर्गः, उपसर्गः_len, extra);
        पक्ष COMPRESSION_BZIP2:
                प्रत्यागम decompress_startswith_bzip2(src, src_आकार, बफर, उपसर्गः, उपसर्गः_len, extra);
        स्वभाव:
                प्रत्यागम decompress_startswith_journal(compression, src, src_आकार, बफर, उपसर्गः, उपसर्गः_len, extra);
        }
}

पूर्णाङ्क compress_प्रवाहः(
                Compression type,
                पूर्णाङ्क fdf, पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_आकार) {

        _cleanup_(compressor_freep) Compressor *c = रिक्त;
        _cleanup_free_ शून्य *buf = रिक्त;
        _cleanup_free_ uint8_t *निवेशः = रिक्त;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t योग_अन्तर् = 0, योग_बहिर्गमनम् = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(fdf >= 0);
        प्रतिज्ञा(fdt >= 0);

        r = compressor_new(&c, type);
        यदि (r < 0)
                प्रत्यागम r;

        निवेशः = new(uint8_t, COMPRESS_PIPE_बफर_आकार);
        यदि (!निवेशः)
                प्रत्यागम -ENOMEM;

        प्रति (;;) {
                size_t m = COMPRESS_PIPE_बफर_आकार;
                ssize_t n;

                यदि (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = पठ(fdf, निवेशः, m);
                यदि (n < 0)
                        प्रत्यागम -errno;

                यदि (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        यदि (r < 0)
                                प्रत्यागम r;

                        यदि (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                यदि (r < 0)
                                        प्रत्यागम r;
                                योग_बहिर्गमनम् += buf_size;
                        }
                        भङ्ग;
                }

                योग_अन्तर् += n;
                यदि (max_bytes != UINT64_MAX) {
                        प्रतिज्ञा(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, निवेशः, n, &buf, &buf_size, &buf_alloc);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        यदि (r < 0)
                                प्रत्यागम r;
                        योग_बहिर्गमनम् += buf_size;
                }
        }

        यदि (ret_uncompressed_आकार)
                *ret_uncompressed_आकार = योग_अन्तर्;

        यदि (योग_अन्तर् == 0)
                log_debug("%s compression finished (no input data)", compression_गन्तव्यम्_string(type));
        अन्यथा
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_गन्तव्यम्_string(type), योग_अन्तर्, योग_बहिर्गमनम्, (द्विगुण) योग_बहिर्गमनम् / योग_अन्तर् * 100);

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क should_sparse(पूर्णाङ्क fd) {
        संरचना stat st;

        प्रतिज्ञा(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        यदि (fstat(fd, &st) < 0)
                प्रत्यागम -errno;

        यदि (!S_ISREG(st.st_mode))
                प्रत्यागम false;

        पूर्णाङ्क flags = fcntl(fd, F_GETFL);
        यदि (flags < 0)
                प्रत्यागम -errno;

        यदि (FLAGS_समुच्चयः(flags, O_APPEND))
                प्रत्यागम false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        यदि (pos < 0)
                प्रत्यागम -errno;

        प्रत्यागम pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
स्थिर पूर्णाङ्क finalize_sparse(पूर्णाङ्क fd) {
        off_t pos;

        प्रतिज्ञा(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        यदि (pos < 0)
                प्रत्यागम -errno;

        यदि (ftruncate(fd, pos) < 0)
                प्रत्यागम -errno;

        प्रत्यागम 0;
}

/* Common helper for decompress_stream_*() wrappers */

संरचना decompress_प्रवाहः_userdata {
        पूर्णाङ्क fd;
        uint64_t max_bytes;
        uint64_t योग_बहिर्गमनम्;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

स्थिर पूर्णाङ्क decompress_प्रवाहः_write_callback(ध्रुव शून्य *दत्त, size_t आकार, शून्य *userdata) {
        संरचना decompress_प्रवाहः_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        यदि (u->max_bytes != UINT64_MAX) {
                यदि (u->max_bytes < आकार)
                        प्रत्यागम -EFBIG;
                u->max_bytes -= आकार;
        }

        u->योग_बहिर्गमनम् += आकार;

        यदि (u->skip > 0) {
                यदि (u->skip >= आकार) {
                        u->skip -= आकार;
                        प्रत्यागम 0;
                }

                दत्त = (ध्रुव uint8_t*) दत्त + u->skip;
                आकार -= u->skip;
                u->skip = 0;
        }

        यदि (आकार == 0)
                प्रत्यागम 0;

        यदि (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, दत्त, आकार, 64);
                यदि (k < 0)
                        प्रत्यागम (पूर्णाङ्क) k;
                प्रत्यागम 0;
        }

        प्रत्यागम loop_write(u->fd, दत्त, आकार);
}

स्थिर पूर्णाङ्क decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        प्रतिज्ञा(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        यदि (!c)
                प्रत्यागम -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        विकल्प (type) {
        पक्ष COMPRESSION_NONE:
                भङ्ग;

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_समुच्चयः(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_समुच्चयः(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_प्रवाहः_decoder(&c->xz, UINT64_MAX, xz_flags);
                यदि (xz_ret != LZMA_OK)
                        प्रत्यागम -EIO;
                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4_F_संस्करणम्);
                यदि (sym_LZ4F_isError(rc))
                        प्रत्यागम -ENOMEM;

                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                यदि (!c->d_zstd)
                        प्रत्यागम -ENOMEM;
                भङ्ग;
#यदि_अन्तः

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                यदि (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_संस्करणम्, (पूर्णाङ्क) परिमाण(c->gzip)) != Z_OK)
                        प्रत्यागम -EIO;
                भङ्ग;
#यदि_अन्तः

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                यदि (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        प्रत्यागम -EIO;
                भङ्ग;
#यदि_अन्तः

        स्वभाव:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क decompress_blob_गन्तव्यम्_fd_impl(
                Compression type,
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_आकार) {

        पूर्णाङ्क r;

        प्रतिज्ञा(src);
        प्रतिज्ञा(fdt >= 0);

        यदि (type == COMPRESSION_NONE)
                प्रत्यागम -EOPNOTSUPP;

        यदि (src_आकार == 0)
                प्रत्यागम -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        यदि (type == COMPRESSION_LZ4 &&
            src_आकार >= COMPRESSION_MAGIC_अष्टकानि_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                यदि (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        यदि (src_आकार <= परिमाण(uint64_t))
                                प्रत्यागम -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        यदि (sz < skip)
                                प्रत्यागम -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                यदि (max_bytes == 0)
                        प्रत्यागम -ENOBUFS;

                _cleanup_free_ शून्य *buf = रिक्त;
                size_t आकार = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_आकार, &buf, &आकार,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                यदि (r < 0)
                        प्रत्यागम r;

                /* Check the result size again for safety. */
                यदि (आकार < skip)
                        प्रत्यागम -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, आकार - skip);
                यदि (r < 0)
                        प्रत्यागम r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_आकार, आकार, (द्विगुण) आकार / src_आकार * 100);

                यदि (ret_आकार)
                        *ret_आकार = आकार - skip;
                प्रत्यागम 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = रिक्त;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        यदि (r < 0)
                प्रत्यागम r;

        संरचना decompress_प्रवाहः_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_प्रक्षिप(c, src, src_आकार, decompress_प्रवाहः_write_callback, &userdata);
        यदि (r < 0)
                प्रत्यागम r;

        /* Finalize */
        r = decompressor_प्रक्षिप(c, /* data= */ रिक्त, /* size= */ 0, decompress_प्रवाहः_write_callback, &userdata);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (userdata.योग_बहिर्गमनम् < skip)
                प्रत्यागम -EBADMSG;

        यदि (userdata.sparse) {
                r = finalize_sparse(fdt);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_गन्तव्यम्_string(type), src_आकार, userdata.योग_बहिर्गमनम्,
                  (द्विगुण) userdata.योग_बहिर्गमनम् / src_आकार * 100);

        यदि (ret_आकार)
                *ret_आकार = userdata.योग_बहिर्गमनम् - skip;

        प्रत्यागम 0;
}

पूर्णाङ्क decompress_blob_गन्तव्यम्_fd_journal(
                Compression type,
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_आकार) {

        पूर्णाङ्क r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम decompress_blob_गन्तव्यम्_fd_impl(type, src, src_आकार, fdt, max_bytes, skip, ret_आकार);
}

पूर्णाङ्क decompress_blob_गन्तव्यम्_fd(
                Compression type,
                ध्रुव शून्य *src,
                uint64_t src_आकार,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_आकार) {

        पूर्णाङ्क r;

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम decompress_blob_गन्तव्यम्_fd_impl(type, src, src_आकार, fdt, max_bytes, skip, ret_आकार);
}

पूर्णाङ्क decompress_प्रवाहः_पूर्णम्(
                Compression type,
                पूर्णाङ्क fdf, पूर्णाङ्क fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = रिक्त;
        _cleanup_free_ uint8_t *buf = रिक्त;
        uint64_t योग_अन्तर् = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(fdf >= 0);
        प्रतिज्ञा(fdt >= 0);

        यदि (type == COMPRESSION_NONE)
                प्रत्यागम -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        r = decompressor_new(&c, type, flags);
        यदि (r < 0)
                प्रत्यागम r;

        संरचना decompress_प्रवाहः_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_बफर_आकार);
        यदि (!buf)
                प्रत्यागम -ENOMEM;

        प्रति (;;) {
                ssize_t n;

                n = पठ(fdf, buf, COMPRESS_PIPE_बफर_आकार);
                यदि (n < 0)
                        प्रत्यागम -errno;

                योग_अन्तर् += n;

                r = decompressor_प्रक्षिप(c, buf, n, decompress_प्रवाहः_write_callback, &userdata);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (n == 0)
                        भङ्ग;
        }

        यदि (योग_अन्तर् == 0)
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_गन्तव्यम्_string(type));

        यदि (userdata.sparse) {
                r = finalize_sparse(fdt);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_गन्तव्यम्_string(type), योग_अन्तर्, userdata.योग_बहिर्गमनम्,
                  (द्विगुण) userdata.योग_बहिर्गमनम् / योग_अन्तर् * 100);

        प्रत्यागम 0;
}

पूर्णाङ्क decompress_प्रवाहः_by_filename(ध्रुव वर्ण *filename, पूर्णाङ्क fdf, पूर्णाङ्क fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        यदि (c == COMPRESSION_NONE)
                प्रत्यागम -EPROTONOSUPPORT;

        प्रत्यागम decompress_प्रवाहः(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        यदि (!c)
                प्रत्यागम रिक्त;

        विकल्प (c->type) {

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ:
                sym_lzma_अन्तः(&c->xz);
                भङ्ग;
#यदि_अन्तः

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4:
                यदि (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = रिक्त;
                        c->lz4_शीर्षकम् = mfree(c->lz4_शीर्षकम्);
                } अन्यथा {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = रिक्त;
                }
                भङ्ग;
#यदि_अन्तः

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                यदि (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = रिक्त;
                } अन्यथा {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = रिक्त;
                }
                भङ्ग;
#यदि_अन्तः

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                यदि (c->encoding)
                        sym_deflate_अन्तः(&c->gzip);
                अन्यथा
                        sym_inflate_अन्तः(&c->gzip);
                भङ्ग;
#यदि_अन्तः

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                यदि (c->encoding)
                        sym_BZ2_bz_Compress_अन्तः(&c->bzip2);
                अन्यथा
                        sym_BZ2_bz_Decompress_अन्तः(&c->bzip2);
                भङ्ग;
#यदि_अन्तः

        स्वभाव:
                भङ्ग;
        }

        प्रत्यागम mfree(c);
}

Compression compressor_type(ध्रुव Compressor *c) {
        प्रत्यागम c ? c->type : _COMPRESSION_INVALID;
}

पूर्णाङ्क decompressor_detect(Decompressor **decompressor, ध्रुव शून्य *दत्त, size_t आकार) {
        पूर्णाङ्क r;

        प्रतिज्ञा(decompressor);

        यदि (*decompressor)
                प्रत्यागम 1;

        यदि (आकार < COMPRESSION_MAGIC_अष्टकानि_MAX)
                प्रत्यागम 0;

        प्रतिज्ञा(दत्त);

        Compression type = compression_detect_from_magic(दत्त);
        यदि (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_गन्तव्यम्_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम 1;
}

पूर्णाङ्क decompressor_force(Decompressor **decompressor, Compression type) {
        पूर्णाङ्क r;

        प्रतिज्ञा(decompressor);
        प्रतिज्ञा(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        *decompressor = compressor_free(*decompressor);
        प्रत्यागम decompressor_new(decompressor, type, /* flags= */ 0);
}

पूर्णाङ्क decompressor_force_off(Decompressor **decompressor) {
        प्रत्यागम decompressor_force(decompressor, COMPRESSION_NONE);
}

पूर्णाङ्क decompressor_प्रक्षिप(Decompressor *c, ध्रुव शून्य *दत्त, size_t आकार, DecompressorCallback callback, शून्य *userdata) {
        पूर्णाङ्क r;

        प्रतिज्ञा(c);
        प्रतिज्ञा(दत्त || आकार == 0);
        प्रतिज्ञा(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        यदि (c->encoding)
                प्रत्यागम -EINVAL;

        यदि (c->परिसमाप्तम् && आकार == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                प्रत्यागम 0;

#यदि HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *बफर = रिक्त;
        यदि (c->type != COMPRESSION_NONE) {
                बफर = new(uint8_t, COMPRESS_PIPE_बफर_आकार);
                यदि (!बफर)
                        प्रत्यागम -ENOMEM;
        }
#यदि_अन्तः

        विकल्प (c->type) {

        पक्ष COMPRESSION_NONE:
                यदि (c->परिसमाप्तम्)
                        प्रत्यागम -EBADMSG;

                यदि (आकार == 0) {
                        c->परिसमाप्तम् = true;
                        प्रत्यागम 0;
                }

                r = callback(दत्त, आकार, userdata);
                यदि (r < 0)
                        प्रत्यागम r;

                प्रत्यागम 0;

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ:
                यदि (c->परिसमाप्तम्)
                        प्रत्यागम -EBADMSG;

                c->xz.अग्र_अन्तर् = दत्त;
                c->xz.avail_अन्तर् = आकार;

                प्रति (;;) {
                        c->xz.अग्र_बहिर्गमनम् = बफर;
                        c->xz.avail_बहिर्गमनम् = COMPRESS_PIPE_बफर_आकार;

                        lzma_ret lzr = sym_lzma_संहिता(&c->xz, आकार == 0 ? LZMA_FINISH : LZMA_सततम्);
                        यदि (!अन्तर्_समुच्चयः(lzr, LZMA_OK, LZMA_प्रवाहः_अन्तः))
                                प्रत्यागम -EBADMSG;

                        यदि (c->xz.avail_बहिर्गमनम् > COMPRESS_PIPE_बफर_आकार)
                                प्रत्यागम -EIO;

                        size_t produced = COMPRESS_PIPE_बफर_आकार - c->xz.avail_बहिर्गमनम्;
                        यदि (produced > 0) {
                                r = callback(बफर, produced, userdata);
                                यदि (r < 0)
                                        प्रत्यागम r;
                        }

                        यदि (lzr == LZMA_प्रवाहः_अन्तः) {
                                यदि (c->xz.avail_अन्तर् > 0)
                                        प्रत्यागम -EBADMSG; /* garbage data at the end */

                                c->परिसमाप्तम् = true;
                                प्रत्यागम 0;
                        }

                        यदि (c->xz.avail_अन्तर् == 0 && c->xz.avail_बहिर्गमनम् > 0) {
                                यदि (आकार == 0)
                                        प्रत्यागम -EBADMSG; /* truncated */

                                प्रत्यागम 0; /* No progress possible with the current input. */
                        }
                }
#यदि_अन्तः

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                ध्रुव uint8_t *src = दत्त;
                size_t src_remaining = आकार;

                c->परिसमाप्तम् = false;

                प्रति (;;) {
                        size_t produced = COMPRESS_PIPE_बफर_आकार;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, बफर, &produced, src, &consumed, रिक्त);
                        यदि (sym_LZ4F_isError(rc))
                                प्रत्यागम -EBADMSG;

                        यदि (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_बफर_आकार)
                                प्रत्यागम -EIO;

                        यदि (consumed > 0) {
                                प्रतिज्ञा(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        यदि (produced > 0) {
                                r = callback(बफर, produced, userdata);
                                यदि (r < 0)
                                        प्रत्यागम r;
                        }

                        यदि (src_remaining == 0) {
                                यदि (rc == 0) {
                                        /* The current stream has finished. */
                                        c->परिसमाप्तम् = true;
                                        प्रत्यागम 0;
                                }

                                यदि (produced == COMPRESS_PIPE_बफर_आकार)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        अनुवर्त;

                                यदि (आकार == 0)
                                        प्रत्यागम -EBADMSG; /* truncated */

                                प्रत्यागम 0; /* No progress possible with the current input. */
                        }
                }
        }
#यदि_अन्तः

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_अन्तर्_बफर निवेशः = {
                        .src =  (शून्य*) दत्त,
                        .आकार = आकार,
                };

                c->परिसमाप्तम् = false;

                प्रति (;;) {
                        ZSTD_बहिर्गमनम्_बफर output = {
                                .dst = बफर,
                                .आकार = COMPRESS_PIPE_बफर_आकार,
                        };

                        size_t res = sym_ZSTD_decompress_प्रवाहः(c->d_zstd, &output, &निवेशः);
                        यदि (sym_ZSTD_isError(res))
                                प्रत्यागम -EBADMSG;

                        यदि (निवेशः.pos > निवेशः.आकार ||
                            output.pos > output.आकार)
                                प्रत्यागम -EIO;

                        यदि (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                यदि (r < 0)
                                        प्रत्यागम r;
                        }

                        यदि (निवेशः.pos == निवेशः.आकार) {
                                यदि (res == 0) {
                                        /* The current stream has finished. */
                                        c->परिसमाप्तम् = true;
                                        प्रत्यागम 0;
                                }

                                यदि (output.pos == COMPRESS_PIPE_बफर_आकार)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        अनुवर्त;

                                यदि (आकार == 0)
                                        प्रत्यागम -EBADMSG; /* truncated */

                                प्रत्यागम 0; /* No progress possible with the current input. */
                        }
                }
        }
#यदि_अन्तः

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                यदि (आकार > UINT_MAX)
                        प्रत्यागम -EFBIG;

                c->gzip.अग्र_अन्तर् = (शून्य*) दत्त;
                c->gzip.avail_अन्तर् = आकार;

                प्रति (;;) {
                        यदि (c->परिसमाप्तम्) {
                                यदि (sym_inflateReset(&c->gzip) != Z_OK)
                                        प्रत्यागम -EIO;

                                c->परिसमाप्तम् = false;
                        }

                        c->gzip.अग्र_बहिर्गमनम् = बफर;
                        c->gzip.avail_बहिर्गमनम् = COMPRESS_PIPE_बफर_आकार;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        पूर्णाङ्क zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        यदि (!अन्तर्_समुच्चयः(zr, Z_OK, Z_प्रवाहः_अन्तः) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_अन्तर् == 0))
                                प्रत्यागम -EBADMSG;

                        यदि (c->gzip.avail_बहिर्गमनम् > COMPRESS_PIPE_बफर_आकार)
                                प्रत्यागम -EIO;

                        size_t produced = COMPRESS_PIPE_बफर_आकार - c->gzip.avail_बहिर्गमनम्;
                        यदि (produced > 0) {
                                r = callback(बफर, produced, userdata);
                                यदि (r < 0)
                                        प्रत्यागम r;
                        }

                        यदि (zr == Z_प्रवाहः_अन्तः) {
                                c->परिसमाप्तम् = true;

                                यदि (c->gzip.avail_अन्तर् > 0)
                                        अनुवर्त; /* New stream may be pending. */

                                प्रत्यागम 0;
                        }

                        यदि (c->gzip.avail_अन्तर् == 0 && c->gzip.avail_बहिर्गमनम् > 0) {
                                यदि (आकार == 0)
                                        प्रत्यागम -EBADMSG; /* truncated */

                                प्रत्यागम 0; /* No progress possible with the current input. */
                        }
                }
#यदि_अन्तः

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                यदि (आकार > UINT_MAX)
                        प्रत्यागम -EFBIG;

                c->bzip2.अग्र_अन्तर् = (वर्ण*) दत्त;
                c->bzip2.avail_अन्तर् = आकार;

                प्रति (;;) {
                        यदि (c->परिसमाप्तम्) {
                                sym_BZ2_bz_Decompress_अन्तः(&c->bzip2);
                                यदि (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        प्रत्यागम -EIO;

                                c->परिसमाप्तम् = false;
                        }

                        c->bzip2.अग्र_बहिर्गमनम् = (वर्ण*) बफर;
                        c->bzip2.avail_बहिर्गमनम् = COMPRESS_PIPE_बफर_आकार;

                        पूर्णाङ्क bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        यदि (!अन्तर्_समुच्चयः(bzr, BZ_OK, BZ_प्रवाहः_अन्तः))
                                प्रत्यागम -EBADMSG;

                        यदि (c->bzip2.avail_बहिर्गमनम् > COMPRESS_PIPE_बफर_आकार)
                                प्रत्यागम -EIO;

                        size_t produced = COMPRESS_PIPE_बफर_आकार - c->bzip2.avail_बहिर्गमनम्;
                        यदि (produced > 0) {
                                r = callback(बफर, produced, userdata);
                                यदि (r < 0)
                                        प्रत्यागम r;
                        }

                        यदि (bzr == BZ_प्रवाहः_अन्तः) {
                                c->परिसमाप्तम् = true;

                                यदि (c->bzip2.avail_अन्तर् > 0)
                                        अनुवर्त; /* New stream may be pending. */

                                प्रत्यागम 0;
                        }

                        यदि (c->bzip2.avail_अन्तर् == 0 && c->bzip2.avail_बहिर्गमनम् > 0) {
                                यदि (आकार == 0)
                                        प्रत्यागम -EBADMSG; /* truncated */

                                प्रत्यागम 0; /* No progress possible with the current input. */
                        }
                }
#यदि_अन्तः

        स्वभाव:
                assert_not_reached();
        }
}

पूर्णाङ्क compressor_new(Compressor **ret, Compression type) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        यदि (!c)
                प्रत्यागम -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        विकल्प (type) {

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                यदि (xzr != LZMA_OK)
                        प्रत्यागम -EIO;

                c->type = COMPRESSION_XZ;
                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4_F_संस्करणम्);
                यदि (sym_LZ4F_isError(rc))
                        प्रत्यागम -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t शीर्षकम्_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_शीर्षकम् = स्मृतिं_देहि(शीर्षकम्_bound);
                यदि (!c->lz4_शीर्षकम्)
                        प्रत्यागम -ENOMEM;

                c->lz4_शीर्षकम्_आकार = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_शीर्षकम्, शीर्षकम्_bound, &lz4_preferences);
                यदि (sym_LZ4F_isError(c->lz4_शीर्षकम्_आकार))
                        प्रत्यागम -EINVAL;

                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                यदि (!c->c_zstd)
                        प्रत्यागम -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_समुच्चयः_Parameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                यदि (sym_ZSTD_isError(z))
                        प्रत्यागम -EIO;

                z = sym_ZSTD_CCtx_समुच्चयः_Parameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                यदि (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                भङ्ग;
#यदि_अन्तः

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_संस्करणम्, (पूर्णाङ्क) परिमाण(c->gzip));
                यदि (r != Z_OK)
                        प्रत्यागम -EIO;

                c->type = COMPRESSION_GZIP;
                भङ्ग;
#यदि_अन्तः

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                यदि (r != BZ_OK)
                        प्रत्यागम -EIO;

                c->type = COMPRESSION_BZIP2;
                भङ्ग;
#यदि_अन्तः

        पक्ष COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                भङ्ग;

        स्वभाव:
                प्रत्यागम -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        प्रत्यागम 0;
}

#यदि HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
स्थिर पूर्णाङ्क enlarge_बफर(शून्य **बफर, size_t *बफर_आकार, size_t *बफर_allocated, size_t need) {
        प्रतिज्ञा(बफर);
        प्रतिज्ञा(बफर_आकार);
        प्रतिज्ञा(बफर_allocated);

        need = MAX3(need, *बफर_आकार + 1, (size_t) COMPRESS_PIPE_बफर_आकार);
        यदि (*बफर_allocated >= need)
                प्रत्यागम 0;

        यदि (!greedy_realloc(बफर, need, 1))
                प्रत्यागम -ENOMEM;

        *बफर_allocated = MALLOC_SIZEOF_SAFE(*बफर);
        प्रत्यागम 1;
}
#यदि_अन्तः

पूर्णाङ्क compressor_start(
                Compressor *c,
                ध्रुव शून्य *दत्त,
                size_t आकार,
                शून्य **बफर,
                size_t *बफर_आकार,
                size_t *बफर_allocated) {

#यदि HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णाङ्क r;
#यदि_अन्तः

        प्रतिज्ञा(c);
        प्रतिज्ञा(बफर);
        प्रतिज्ञा(बफर_आकार);
        प्रतिज्ञा(बफर_allocated);

        यदि (!c->encoding)
                प्रत्यागम -EINVAL;

        यदि (आकार == 0)
                प्रत्यागम 0;

        प्रतिज्ञा(दत्त);

        *बफर_आकार = 0;

        विकल्प (c->type) {

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ:

                c->xz.अग्र_अन्तर् = दत्त;
                c->xz.avail_अन्तर् = आकार;

                यावत् (c->xz.avail_अन्तर् > 0) {
                        lzma_ret lzr;

                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        यदि (r < 0)
                                प्रत्यागम r;

                        c->xz.अग्र_बहिर्गमनम् = (uint8_t*) *बफर + *बफर_आकार;
                        c->xz.avail_बहिर्गमनम् = *बफर_allocated - *बफर_आकार;

                        lzr = sym_lzma_संहिता(&c->xz, LZMA_सततम्);
                        यदि (lzr != LZMA_OK)
                                प्रत्यागम -EIO;

                        *बफर_आकार += (*बफर_allocated - *बफर_आकार) - c->xz.avail_बहिर्गमनम्;
                }

                भङ्ग;
#यदि_अन्तः

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                यदि (c->lz4_शीर्षकम्_आकार > 0) {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, c->lz4_शीर्षकम्_आकार);
                        यदि (r < 0)
                                प्रत्यागम r;

                        स्मृतिं_प्रतिलिख(*बफर, c->lz4_शीर्षकम्, c->lz4_शीर्षकम्_आकार);
                        *बफर_आकार = c->lz4_शीर्षकम्_आकार;
                        c->lz4_शीर्षकम् = mfree(c->lz4_शीर्षकम्);
                        c->lz4_शीर्षकम्_आकार = 0;
                }

                size_t bound = sym_LZ4F_compressBound(आकार, &lz4_preferences);
                r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, *बफर_आकार + bound);
                यदि (r < 0)
                        प्रत्यागम r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *बफर + *बफर_आकार,
                                                   *बफर_allocated - *बफर_आकार,
                                                   दत्त, आकार, रिक्त);
                यदि (sym_LZ4F_isError(n))
                        प्रत्यागम -EIO;

                *बफर_आकार += n;
                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_अन्तर्_बफर निवेशः = {
                        .src = दत्त,
                        .आकार = आकार,
                };

                यावत् (निवेशः.pos < निवेशः.आकार) {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        यदि (r < 0)
                                प्रत्यागम r;

                        ZSTD_बहिर्गमनम्_बफर output = {
                                .dst = ((uint8_t *) *बफर + *बफर_आकार),
                                .आकार = *बफर_allocated - *बफर_आकार,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &निवेशः, ZSTD_e_continue);
                        यदि (sym_ZSTD_isError(res))
                                प्रत्यागम -EIO;

                        *बफर_आकार += output.pos;
                }

                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                यदि (आकार > UINT_MAX)
                        प्रत्यागम -EFBIG;

                c->gzip.अग्र_अन्तर् = (शून्य*) दत्त;
                c->gzip.avail_अन्तर् = आकार;

                यावत् (c->gzip.avail_अन्तर् > 0) {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        यदि (r < 0)
                                प्रत्यागम r;

                        size_t avail = MIN(*बफर_allocated - *बफर_आकार, (size_t) UINT_MAX);
                        c->gzip.अग्र_बहिर्गमनम् = (uint8_t*) *बफर + *बफर_आकार;
                        c->gzip.avail_बहिर्गमनम् = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        यदि (r != Z_OK)
                                प्रत्यागम -EIO;

                        *बफर_आकार += avail - c->gzip.avail_बहिर्गमनम्;
                }

                भङ्ग;
#यदि_अन्तः

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                यदि (आकार > UINT_MAX)
                        प्रत्यागम -EFBIG;

                c->bzip2.अग्र_अन्तर् = (शून्य*) दत्त;
                c->bzip2.avail_अन्तर् = आकार;

                यावत् (c->bzip2.avail_अन्तर् > 0) {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        यदि (r < 0)
                                प्रत्यागम r;

                        size_t avail = MIN(*बफर_allocated - *बफर_आकार, (size_t) UINT_MAX);
                        c->bzip2.अग्र_बहिर्गमनम् = (शून्य*) ((uint8_t*) *बफर + *बफर_आकार);
                        c->bzip2.avail_बहिर्गमनम् = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_सततम्);
                        यदि (r != BZ_सततम्_OK)
                                प्रत्यागम -EIO;

                        *बफर_आकार += avail - c->bzip2.avail_बहिर्गमनम्;
                }

                भङ्ग;
#यदि_अन्तः

        पक्ष COMPRESSION_NONE:

                यदि (*बफर_allocated < आकार) {
                        शून्य *p;

                        p = realloc(*बफर, आकार);
                        यदि (!p)
                                प्रत्यागम -ENOMEM;

                        *बफर = p;
                        *बफर_allocated = आकार;
                }

                स्मृतिं_प्रतिलिख(*बफर, दत्त, आकार);
                *बफर_आकार = आकार;
                भङ्ग;

        स्वभाव:
                प्रत्यागम -EOPNOTSUPP;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क compressor_finish(Compressor *c, शून्य **बफर, size_t *बफर_आकार, size_t *बफर_allocated) {
#यदि HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णाङ्क r;
#यदि_अन्तः

        प्रतिज्ञा(c);
        प्रतिज्ञा(बफर);
        प्रतिज्ञा(बफर_आकार);
        प्रतिज्ञा(बफर_allocated);

        यदि (!c->encoding)
                प्रत्यागम -EINVAL;

        *बफर_आकार = 0;

        विकल्प (c->type) {

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_अन्तर् = 0;

                कुरु {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        यदि (r < 0)
                                प्रत्यागम r;

                        c->xz.अग्र_बहिर्गमनम् = (uint8_t*) *बफर + *बफर_आकार;
                        c->xz.avail_बहिर्गमनम् = *बफर_allocated - *बफर_आकार;

                        lzr = sym_lzma_संहिता(&c->xz, LZMA_FINISH);
                        यदि (!अन्तर्_समुच्चयः(lzr, LZMA_OK, LZMA_प्रवाहः_अन्तः))
                                प्रत्यागम -EIO;

                        *बफर_आकार += (*बफर_allocated - *बफर_आकार) - c->xz.avail_बहिर्गमनम्;
                } यावत् (lzr != LZMA_प्रवाहः_अन्तः);

                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, bound);
                यदि (r < 0)
                        प्रत्यागम r;

                size_t n = sym_LZ4_F_compress_अन्तः(c->c_lz4, *बफर, *बफर_allocated, रिक्त);
                यदि (sym_LZ4F_isError(n))
                        प्रत्यागम -EIO;

                *बफर_आकार = n;
                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_अन्तर्_बफर निवेशः = {};
                size_t res;

                कुरु {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        यदि (r < 0)
                                प्रत्यागम r;

                        ZSTD_बहिर्गमनम्_बफर output = {
                                .dst = ((uint8_t *) *बफर + *बफर_आकार),
                                .आकार = *बफर_allocated - *बफर_आकार,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &निवेशः, ZSTD_e_अन्तः);
                        यदि (sym_ZSTD_isError(res))
                                प्रत्यागम -EIO;

                        *बफर_आकार += output.pos;
                } यावत् (res != 0);

                भङ्ग;
        }
#यदि_अन्तः

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                c->gzip.avail_अन्तर् = 0;

                कुरु {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        यदि (r < 0)
                                प्रत्यागम r;

                        size_t avail = MIN(*बफर_allocated - *बफर_आकार, (size_t) UINT_MAX);
                        c->gzip.अग्र_बहिर्गमनम् = (uint8_t*) *बफर + *बफर_आकार;
                        c->gzip.avail_बहिर्गमनम् = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        यदि (!अन्तर्_समुच्चयः(r, Z_OK, Z_प्रवाहः_अन्तः))
                                प्रत्यागम -EIO;

                        *बफर_आकार += avail - c->gzip.avail_बहिर्गमनम्;
                } यावत् (r != Z_प्रवाहः_अन्तः);

                भङ्ग;
#यदि_अन्तः

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                c->bzip2.avail_अन्तर् = 0;

                कुरु {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        यदि (r < 0)
                                प्रत्यागम r;

                        size_t avail = MIN(*बफर_allocated - *बफर_आकार, (size_t) UINT_MAX);
                        c->bzip2.अग्र_बहिर्गमनम् = (शून्य*) ((uint8_t*) *बफर + *बफर_आकार);
                        c->bzip2.avail_बहिर्गमनम् = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        यदि (!अन्तर्_समुच्चयः(r, BZ_FINISH_OK, BZ_प्रवाहः_अन्तः))
                                प्रत्यागम -EIO;

                        *बफर_आकार += avail - c->bzip2.avail_बहिर्गमनम्;
                } यावत् (r != BZ_प्रवाहः_अन्तः);

                भङ्ग;
#यदि_अन्तः

        पक्ष COMPRESSION_NONE:
                भङ्ग;

        स्वभाव:
                प्रत्यागम -EOPNOTSUPP;
        }

        प्रत्यागम 0;
}
