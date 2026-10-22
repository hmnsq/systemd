/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#यदि HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#अन्तर्भाव <lzma.h>
#endif

#यदि HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#अन्तर्भाव <lz4.h>
#अन्तर्भाव <lz4frame.h>
#अन्तर्भाव <lz4hc.h>
#endif

#यदि HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#अन्तर्भाव <zstd.h>
#अन्तर्भाव <zstd_errors.h>
#endif

#यदि HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#अन्तर्भाव <zlib.h>
#endif

#यदि HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#अन्तर्भाव <bzlib.h>
#endif

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
स्थिर DLSYM_PROTOTYPE(lzma_code) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_easy_encoder) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_end) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_stream_decoder) = रिक्त;
स्थिर DLSYM_PROTOTYPE(lzma_lzma_preset) = रिक्त;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
स्थिर अन्तःस्थ शून्य lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#यदि HAVE_LZ4
स्थिर DLSYM_PROTOTYPE(LZ4F_compressBegin) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_compressBound) = रिक्त;
स्थिर DLSYM_PROTOTYPE(LZ4F_compressEnd) = रिक्त;
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
स्थिर DLSYM_PROTOTYPE(LZ4_version_सङ्ख्या) = रिक्त;

स्थिर अचर LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#यदि HAVE_ZSTD
स्थिर DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_compress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_compressStream2) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_createCCtx) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_createDCtx) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_decompressStream) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_freeCCtx) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_freeDCtx) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_getErrorCode) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_getErrorName) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = रिक्त;
स्थिर DLSYM_PROTOTYPE(ZSTD_isError) = रिक्त;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, रिक्त);

स्थिर पूर्णाङ्क zstd_ret_to_errno(माप_प्रकार ret) {
        विकल्प (sym_ZSTD_getErrorCode(ret)) {
        पक्ष ZSTD_error_dstSize_tooSmall:
                फर्काऊ -ENOBUFS;
        पक्ष ZSTD_error_memory_allocation:
                फर्काऊ -ENOMEM;
        स्वभाव:
                फर्काऊ -EBADMSG;
        }
}
#endif

#यदि HAVE_ZLIB
स्थिर DLSYM_PROTOTYPE(deflateInit2_) = रिक्त;
स्थिर DLSYM_PROTOTYPE(deflate) = रिक्त;
स्थिर DLSYM_PROTOTYPE(deflateEnd) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflateInit2_) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflate) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflateEnd) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflateReset) = रिक्त;

स्थिर अन्तःस्थ शून्य deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

स्थिर अन्तःस्थ शून्य inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#यदि HAVE_BZIP2
स्थिर DLSYM_PROTOTYPE(BZ2_bzCompressInit) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzCompress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzDecompress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = रिक्त;

स्थिर अन्तःस्थ शून्य BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

स्थिर अन्तःस्थ शून्य BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
संरचना Compressor {
        Compression type;
        bool encoding;
        bool finished;
        सङ्घ {
#यदि HAVE_XZ
                lzma_stream xz;
#endif
#यदि HAVE_LZ4
                संरचना {
                        LZ4F_compressionContext_t c_lz4;
                        शून्य *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        माप_प्रकार lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#यदि HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#यदि HAVE_ZLIB
                z_stream gzip;
#endif
#यदि HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, परिमाण(माप_प्रकार))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

स्थिर अचर वर्ण* अचर compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

स्थिर अचर वर्ण* अचर compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

स्थिर अचर वर्ण* अचर compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(अचर वर्ण *s) {
        Compression c;

        प्रतिज्ञा(s);

        c = compression_from_string(s);
        यदि (c >= 0)
                फर्काऊ c;

        फर्काऊ compression_uppercase_from_string(s);
}

Compression compression_from_filename(अचर वर्ण *filename) {
        Compression c;
        अचर वर्ण *e;

        प्रतिज्ञा(filename);

        e = म_खोजप(filename, '.');
        यदि (!e)
                फर्काऊ COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        यदि (c < 0)
                फर्काऊ COMPRESSION_NONE;

        फर्काऊ c;
}

bool compression_supported_journal(Compression c) {
        स्थिर अचर अचिन्हित supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        प्रतिज्ञा(c >= 0);
        प्रतिज्ञा(c < _COMPRESSION_MAX);

        फर्काऊ BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        स्थिर अचर अचिन्हित supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        प्रतिज्ञा(c >= 0);
        प्रतिज्ञा(c < _COMPRESSION_MAX);

        फर्काऊ BIT_SET(supported, c);
}

Compression compression_detect_from_magic(अचर uint8_t data[स्थिर COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        यदि (स_भेद(data, (अचर uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                फर्काऊ COMPRESSION_GZIP;
        यदि (स_भेद(data, (अचर uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                फर्काऊ COMPRESSION_XZ;
        यदि (स_भेद(data, (अचर uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                फर्काऊ COMPRESSION_ZSTD;
        यदि (स_भेद(data, (अचर uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                फर्काऊ COMPRESSION_LZ4;
        यदि (स_भेद(data, (अचर uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                फर्काऊ COMPRESSION_BZIP2;

        फर्काऊ _COMPRESSION_INVALID;
}

पूर्णाङ्क dlopen_xz(पूर्णाङ्क log_level) {
#यदि HAVE_XZ
        स्थिर शून्य *lzma_dl = रिक्त;

        LIBLZMA_NOTE(suggested);

        फर्काऊ dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#अन्यथा
        फर्काऊ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_lz4(पूर्णाङ्क log_level) {
#यदि HAVE_LZ4
        स्थिर शून्य *lz4_dl = रिक्त;

        LIBLZ4_NOTE(suggested);

        फर्काऊ dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_सङ्ख्या));
#अन्यथा
        फर्काऊ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_zstd(पूर्णाङ्क log_level) {
#यदि HAVE_ZSTD
        स्थिर शून्य *zstd_dl = रिक्त;

        LIBZSTD_NOTE(suggested);

        फर्काऊ dlopen_many_sym_or_warn(
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
#अन्यथा
        फर्काऊ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_zlib(पूर्णाङ्क log_level) {
#यदि HAVE_ZLIB
        स्थिर शून्य *zlib_dl = रिक्त;

        LIBZ_NOTE(suggested);

        फर्काऊ dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#अन्यथा
        फर्काऊ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_bzip2(पूर्णाङ्क log_level) {
#यदि HAVE_BZIP2
        स्थिर शून्य *bzip2_dl = रिक्त;

        LIBBZ2_NOTE(suggested);

        फर्काऊ dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#अन्यथा
        फर्काऊ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_compress_journal(Compression c, पूर्णाङ्क log_level) {
        विकल्प (c) {
        पक्ष COMPRESSION_NONE:
                फर्काऊ 0;
        पक्ष COMPRESSION_XZ:
                फर्काऊ dlopen_xz(log_level);
        पक्ष COMPRESSION_LZ4:
                फर्काऊ dlopen_lz4(log_level);
        पक्ष COMPRESSION_ZSTD:
                फर्काऊ dlopen_zstd(log_level);
        स्वभाव:
                फर्काऊ -EOPNOTSUPP;
        }
}

पूर्णाङ्क dlopen_compress(Compression c, पूर्णाङ्क log_level) {
        विकल्प (c) {
        पक्ष COMPRESSION_GZIP:
                फर्काऊ dlopen_zlib(log_level);
        पक्ष COMPRESSION_BZIP2:
                फर्काऊ dlopen_bzip2(log_level);
        स्वभाव:
                फर्काऊ dlopen_compress_journal(c, log_level);
        }
}

स्थिर पूर्णाङ्क compress_blob_xz(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                माप_प्रकार dst_alloc_size,
                माप_प्रकार *dst_size,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#यदि HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, रिक्त, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, रिक्त }
        };
        lzma_ret ret;
        माप_प्रकार out_pos = 0;
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                यदि (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        फर्काऊ -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        यदि (src_size < 80)
                फर्काऊ -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ रिक्त,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        यदि (ret != LZMA_OK)
                फर्काऊ -ENOBUFS;

        *dst_size = out_pos;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क compress_blob_lz4(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                माप_प्रकार dst_alloc_size,
                माप_प्रकार *dst_size,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#यदि HAVE_LZ4
        पूर्णाङ्क r;

        r = dlopen_lz4(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        यदि (src_size < 9)
                फर्काऊ -ENOBUFS;

        यदि (src_size > पूर्णांक_उच्च)
                फर्काऊ -EFBIG;
        यदि (dst_alloc_size > पूर्णांक_उच्च)
                dst_alloc_size = पूर्णांक_उच्च;

        यदि (level <= 0)
                r = sym_LZ4_compress_default(src, (वर्ण*)dst + 8, src_size, (पूर्णाङ्क) dst_alloc_size - 8);
        अन्यथा
                r = sym_LZ4_compress_HC(src, (वर्ण*)dst + 8, src_size, (पूर्णाङ्क) dst_alloc_size - 8, level);
        यदि (r <= 0)
                फर्काऊ -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क compress_blob_zstd(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                माप_प्रकार dst_alloc_size,
                माप_प्रकार *dst_size,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#यदि HAVE_ZSTD
        माप_प्रकार k;
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        यदि (sym_ZSTD_isError(k))
                फर्काऊ zstd_ret_to_errno(k);

        *dst_size = k;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क compress_blob_gzip(अचर शून्य *src, uint64_t src_size,
                       शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#यदि HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size > अच_पूर्णांक_उच्च)
                फर्काऊ -EFBIG;
        यदि (dst_alloc_size > अच_पूर्णांक_उच्च)
                dst_alloc_size = अच_पूर्णांक_उच्च;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (पूर्णाङ्क) परिमाण(s));
        यदि (r != Z_OK)
                फर्काऊ -ENOMEM;

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        यदि (r != Z_STREAM_END)
                फर्काऊ -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क compress_blob_bzip2(
                अचर शून्य *src, uint64_t src_size,
                शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#यदि HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size > अच_पूर्णांक_उच्च)
                फर्काऊ -EFBIG;
        यदि (dst_alloc_size > अच_पूर्णांक_उच्च)
                dst_alloc_size = अच_पूर्णांक_उच्च;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        यदि (r != BZ_OK)
                फर्काऊ -ENOMEM;

        s.next_in = (वर्ण*) src;
        s.avail_in = src_size;
        s.next_out = (वर्ण*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        यदि (r != BZ_STREAM_END)
                फर्काऊ -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

पूर्णाङ्क compress_blob_journal(
                Compression compression,
                अचर शून्य *src, uint64_t src_size,
                शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णाङ्क level) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                फर्काऊ compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        पक्ष COMPRESSION_LZ4:
                फर्काऊ compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        पक्ष COMPRESSION_ZSTD:
                फर्काऊ compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        स्वभाव:
                फर्काऊ -EOPNOTSUPP;
        }
}

पूर्णाङ्क compress_blob(
                Compression compression,
                अचर शून्य *src, uint64_t src_size,
                शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णाङ्क level) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                फर्काऊ compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        पक्ष COMPRESSION_BZIP2:
                फर्काऊ compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        स्वभाव:
                फर्काऊ compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

स्थिर पूर्णाङ्क decompress_blob_xz(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#यदि HAVE_XZ
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        यदि (ret != LZMA_OK)
                फर्काऊ -ENOMEM;

        माप_प्रकार space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        यदि (!greedy_realloc(dst, space, 1))
                फर्काऊ -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        लागि (;;) {
                माप_प्रकार used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                यदि (ret == LZMA_STREAM_END)
                        रोक;
                यदि (ret != LZMA_OK)
                        फर्काऊ -ENOMEM;

                यदि (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        रोक;
                यदि (dst_max > 0 && space == dst_max)
                        फर्काऊ -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                प्रतिज्ञा(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                यदि (!greedy_realloc(dst, space, 1))
                        फर्काऊ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_blob_lz4(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#यदि HAVE_LZ4
        वर्ण* out;
        पूर्णाङ्क r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size <= 8)
                फर्काऊ -EBADMSG;

        यदि (src_size - 8 > पूर्णांक_उच्च)
                फर्काऊ -EFBIG;

        size = unaligned_read_le64(src);
        यदि (size < 0 || (अचिन्हित) size != unaligned_read_le64(src))
                फर्काऊ -EFBIG;
        यदि (dst_max > 0 && (माप_प्रकार) size > dst_max)
                फर्काऊ -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        यदि (!out)
                फर्काऊ -ENOMEM;

        r = sym_LZ4_decompress_safe((वर्ण*)src + 8, out, src_size - 8, size);
        यदि (r < 0 || r != size)
                फर्काऊ -EBADMSG;

        *dst_size = size;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

माप_प्रकार zstd_dstream_out_size(शून्य) {
#यदि HAVE_ZSTD
        यदि (dlopen_zstd(LOG_DEBUG) < 0)
                फर्काऊ 0;

        फर्काऊ sym_ZSTD_DStreamOutSize();
#अन्यथा
        फर्काऊ 0;
#endif
}

स्थिर पूर्णाङ्क decompress_blob_zstd(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#यदि HAVE_ZSTD
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        अचिन्हित दीर्घ दीर्घ size = sym_ZSTD_getFrameContentSize(src, src_size);
        यदि (size == ZSTD_CONTENTSIZE_ERROR)
                फर्काऊ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        माप_प्रकार cap = dst_max ?: SIZE_MAX;

        माप_प्रकार space;
        यदि (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (माप_प्रकार) MIN(size, (अचिन्हित दीर्घ दीर्घ) cap));
        अन्यथा {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (माप_प्रकार) MIN(twice, (uint64_t) cap));
        }

        यदि (!greedy_realloc(dst, space, 1))
                फर्काऊ -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        यदि (!dctx)
                फर्काऊ -ENOMEM;

        ZSTD_inBuffer आगत = {
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

        लागि (;;) {
                माप_प्रकार k = sym_ZSTD_decompressStream(dctx, &output, &आगत);
                यदि (sym_ZSTD_isError(k))
                        फर्काऊ log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                यदि (output.pos >= cap)
                        रोक;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                यदि (k == 0 && आगत.pos >= आगत.size)
                        रोक;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                यदि (output.pos == output.size) {
                        यदि (space > SIZE_MAX / 2)
                                फर्काऊ -E2BIG;
                        space = MIN(2 * space, cap);
                        यदि (!greedy_realloc(dst, space, 1))
                                फर्काऊ -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        जारी;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                यदि (आगत.pos >= आगत.size)
                        फर्काऊ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_blob_gzip(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#यदि HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size > अच_पूर्णांक_उच्च)
                फर्काऊ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णाङ्क) परिमाण(s));
        यदि (r != Z_OK)
                फर्काऊ -ENOMEM;

        माप_प्रकार space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
        यदि (!greedy_realloc(dst, space, 1))
                फर्काऊ -ENOMEM;

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        लागि (;;) {
                माप_प्रकार used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                यदि (r == Z_STREAM_END)
                        रोक;
                यदि (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        फर्काऊ -EBADMSG;

                यदि (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        रोक;
                यदि (dst_max > 0 && space == dst_max)
                        फर्काऊ -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, अच_पूर्णांक_उच्च);
                यदि (!greedy_realloc(dst, space, 1))
                        फर्काऊ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_blob_bzip2(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#यदि HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size > अच_पूर्णांक_उच्च)
                फर्काऊ -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        यदि (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                फर्काऊ -EIO;

        माप_प्रकार space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
        यदि (!greedy_realloc(dst, space, 1))
                फर्काऊ -ENOMEM;

        s.next_in = (वर्ण*) src;
        s.avail_in = src_size;
        s.next_out = (वर्ण*) *dst;
        s.avail_out = space;

        लागि (;;) {
                माप_प्रकार used;

                r = sym_BZ2_bzDecompress(&s);
                यदि (r == BZ_STREAM_END)
                        रोक;
                यदि (r != BZ_OK)
                        फर्काऊ -EBADMSG;

                यदि (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        रोक;
                यदि (dst_max > 0 && space == dst_max)
                        फर्काऊ -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
                यदि (!greedy_realloc(dst, space, 1))
                        फर्काऊ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (वर्ण*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

पूर्णाङ्क decompress_blob_journal(
                Compression compression,
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                फर्काऊ decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        पक्ष COMPRESSION_LZ4:
                फर्काऊ decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        पक्ष COMPRESSION_ZSTD:
                फर्काऊ decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        स्वभाव:
                फर्काऊ -EPROTONOSUPPORT;
        }
}

पूर्णाङ्क decompress_blob(
                Compression compression,
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                फर्काऊ decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        पक्ष COMPRESSION_BZIP2:
                फर्काऊ decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        स्वभाव:
                फर्काऊ decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

पूर्णाङ्क decompress_zlib_raw(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                माप_प्रकार dst_size,
                पूर्णाङ्क wbits) {

#यदि HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size > अच_पूर्णांक_उच्च)
                फर्काऊ -EFBIG;
        यदि (dst_size > अच_पूर्णांक_उच्च)
                फर्काऊ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (शून्य*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (पूर्णाङ्क) परिमाण(s));
        यदि (r != Z_OK)
                फर्काऊ -EIO;

        r = sym_inflate(&s, Z_FINISH);
        माप_प्रकार produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        यदि (r != Z_STREAM_END || produced != dst_size)
                फर्काऊ -EBADMSG;

        फर्काऊ 0;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_xz(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                अचर शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#यदि HAVE_XZ
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        यदि (ret != LZMA_OK)
                फर्काऊ -EBADMSG;

        यदि (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                फर्काऊ -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        लागि (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                यदि (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        फर्काऊ -EBADMSG;

                यदि (allocated - s.avail_out >= prefix_len + 1)
                        फर्काऊ स_भेद(*buffer, prefix, prefix_len) == 0 &&
                                ((अचर uint8_t*) *buffer)[prefix_len] == extra;

                यदि (ret == LZMA_STREAM_END)
                        फर्काऊ 0;

                s.avail_out += allocated;

                यदि (!(greedy_realloc(buffer, allocated * 2, 1)))
                        फर्काऊ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_lz4(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                अचर शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#यदि HAVE_LZ4
        माप_प्रकार allocated;
        पूर्णाङ्क r;

        r = dlopen_lz4(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size <= 8)
                फर्काऊ -EBADMSG;

        यदि (src_size - 8 > पूर्णांक_उच्च)
                फर्काऊ -EFBIG;

        यदि (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                फर्काऊ -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (वर्ण*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        यदि (r < 0 || (माप_प्रकार) r < prefix_len + 1) {
                माप_प्रकार size;

                यदि (sym_LZ4_version_सङ्ख्या() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        फर्काऊ 0;

                यदि (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        यदि (स_भेद(*buffer, prefix, r) != 0)
                                फर्काऊ 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (size < prefix_len + 1)
                        फर्काऊ 0;
        }

        फर्काऊ स_भेद(*buffer, prefix, prefix_len) == 0 &&
                ((अचर uint8_t*) *buffer)[prefix_len] == extra;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_zstd(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                अचर शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#यदि HAVE_ZSTD
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        अचिन्हित दीर्घ दीर्घ size = sym_ZSTD_getFrameContentSize(src, src_size);
        यदि (size == ZSTD_CONTENTSIZE_ERROR)
                फर्काऊ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        यदि (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                फर्काऊ 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        यदि (!dctx)
                फर्काऊ -ENOMEM;

        यदि (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                फर्काऊ -ENOMEM;

        ZSTD_inBuffer आगत = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        माप_प्रकार k;

        k = sym_ZSTD_decompressStream(dctx, &output, &आगत);
        यदि (sym_ZSTD_isError(k))
                फर्काऊ log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        यदि (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                यदि (k != 0)
                        फर्काऊ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                फर्काऊ 0; /* Decompressed text too short to match the prefix and extra */
        }

        फर्काऊ स_भेद(*buffer, prefix, prefix_len) == 0 &&
                ((अचर uint8_t*) *buffer)[prefix_len] == extra;
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_gzip(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                अचर शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#यदि HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size > अच_पूर्णांक_उच्च)
                फर्काऊ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णाङ्क) परिमाण(s));
        यदि (r != Z_OK)
                फर्काऊ -EBADMSG;

        यदि (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                फर्काऊ -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (माप_प्रकार) अच_पूर्णांक_उच्च);

        लागि (;;) {
                r = sym_inflate(&s, Z_FINISH);

                यदि (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        फर्काऊ -EBADMSG;

                यदि (allocated - s.avail_out >= prefix_len + 1)
                        फर्काऊ स_भेद(*buffer, prefix, prefix_len) == 0 &&
                                ((अचर uint8_t*) *buffer)[prefix_len] == extra;

                यदि (r == Z_STREAM_END)
                        फर्काऊ 0;

                यदि (s.avail_out > 0)
                        फर्काऊ -EBADMSG; /* truncated */

                माप_प्रकार used = allocated - s.avail_out;

                यदि (!(greedy_realloc(buffer, allocated * 2, 1)))
                        फर्काऊ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (माप_प्रकार) अच_पूर्णांक_उच्च);
                s.next_out = *(uint8_t**)buffer + used;
        }
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_bzip2(
                अचर शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                अचर शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#यदि HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        यदि (src_size > अच_पूर्णांक_उच्च)
                फर्काऊ -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        यदि (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                फर्काऊ -EIO;

        यदि (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                फर्काऊ -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (वर्ण*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (माप_प्रकार) अच_पूर्णांक_उच्च);

        लागि (;;) {
                r = sym_BZ2_bzDecompress(&s);

                यदि (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        फर्काऊ -EBADMSG;

                यदि (allocated - s.avail_out >= prefix_len + 1)
                        फर्काऊ स_भेद(*buffer, prefix, prefix_len) == 0 &&
                                ((अचर uint8_t*) *buffer)[prefix_len] == extra;

                यदि (r == BZ_STREAM_END)
                        फर्काऊ 0;

                यदि (s.avail_out > 0)
                        फर्काऊ -EBADMSG; /* truncated */

                माप_प्रकार used = allocated - s.avail_out;

                यदि (!(greedy_realloc(buffer, allocated * 2, 1)))
                        फर्काऊ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (माप_प्रकार) अच_पूर्णांक_उच्च);
                s.next_out = (वर्ण*) *buffer + used;
        }
#अन्यथा
        फर्काऊ -EPROTONOSUPPORT;
#endif
}

पूर्णाङ्क decompress_startswith_journal(
                Compression compression,
                अचर शून्य *src, uint64_t src_size,
                शून्य **buffer,
                अचर शून्य *prefix, माप_प्रकार prefix_len,
                uint8_t extra) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                फर्काऊ decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        पक्ष COMPRESSION_LZ4:
                फर्काऊ decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        पक्ष COMPRESSION_ZSTD:
                फर्काऊ decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        स्वभाव:
                फर्काऊ -EOPNOTSUPP;
        }
}

पूर्णाङ्क decompress_startswith(
                Compression compression,
                अचर शून्य *src, uint64_t src_size,
                शून्य **buffer,
                अचर शून्य *prefix, माप_प्रकार prefix_len,
                uint8_t extra) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                फर्काऊ decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        पक्ष COMPRESSION_BZIP2:
                फर्काऊ decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        स्वभाव:
                फर्काऊ decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

पूर्णाङ्क compress_stream(
                Compression type,
                पूर्णाङ्क fdf, पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = रिक्त;
        _cleanup_free_ शून्य *buf = रिक्त;
        _cleanup_free_ uint8_t *आगत = रिक्त;
        माप_प्रकार buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(fdf >= 0);
        प्रतिज्ञा(fdt >= 0);

        r = compressor_new(&c, type);
        यदि (r < 0)
                फर्काऊ r;

        आगत = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        यदि (!आगत)
                फर्काऊ -ENOMEM;

        लागि (;;) {
                माप_प्रकार m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                यदि (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (माप_प्रकार) max_bytes;

                n = पठ(fdf, आगत, m);
                यदि (n < 0)
                        फर्काऊ -त्रुटि_सं;

                यदि (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        यदि (r < 0)
                                फर्काऊ r;

                        यदि (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                यदि (r < 0)
                                        फर्काऊ r;
                                total_out += buf_size;
                        }
                        रोक;
                }

                total_in += n;
                यदि (max_bytes != UINT64_MAX) {
                        प्रतिज्ञा(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, आगत, n, &buf, &buf_size, &buf_alloc);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        यदि (r < 0)
                                फर्काऊ r;
                        total_out += buf_size;
                }
        }

        यदि (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        यदि (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        अन्यथा
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (द्विगुण) total_out / total_in * 100);

        फर्काऊ 0;
}

स्थिर पूर्णाङ्क should_sparse(पूर्णाङ्क fd) {
        संरचना stat st;

        प्रतिज्ञा(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        यदि (ख_स्थिति(fd, &st) < 0)
                फर्काऊ -त्रुटि_सं;

        यदि (!S_ISREG(st.st_mode))
                फर्काऊ false;

        पूर्णाङ्क flags = fcntl(fd, F_GETFL);
        यदि (flags < 0)
                फर्काऊ -त्रुटि_सं;

        यदि (FLAGS_SET(flags, O_APPEND))
                फर्काऊ false;

        off_t pos = lseek(fd, 0, प्रस्तुत_से);
        यदि (pos < 0)
                फर्काऊ -त्रुटि_सं;

        फर्काऊ pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
स्थिर पूर्णाङ्क finalize_sparse(पूर्णाङ्क fd) {
        off_t pos;

        प्रतिज्ञा(fd >= 0);

        pos = lseek(fd, 0, प्रस्तुत_से);
        यदि (pos < 0)
                फर्काऊ -त्रुटि_सं;

        यदि (ftruncate(fd, pos) < 0)
                फर्काऊ -त्रुटि_सं;

        फर्काऊ 0;
}

/* Common helper for decompress_stream_*() wrappers */

संरचना decompress_stream_userdata {
        पूर्णाङ्क fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

स्थिर पूर्णाङ्क decompress_stream_write_callback(अचर शून्य *data, माप_प्रकार size, शून्य *userdata) {
        संरचना decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        यदि (u->max_bytes != UINT64_MAX) {
                यदि (u->max_bytes < size)
                        फर्काऊ -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        यदि (u->skip > 0) {
                यदि (u->skip >= size) {
                        u->skip -= size;
                        फर्काऊ 0;
                }

                data = (अचर uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        यदि (size == 0)
                फर्काऊ 0;

        यदि (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                यदि (k < 0)
                        फर्काऊ (पूर्णाङ्क) k;
                फर्काऊ 0;
        }

        फर्काऊ loop_write(u->fd, data, size);
}

स्थिर पूर्णाङ्क decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        प्रतिज्ञा(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        यदि (!c)
                फर्काऊ -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        विकल्प (type) {
        पक्ष COMPRESSION_NONE:
                रोक;

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                यदि (xz_ret != LZMA_OK)
                        फर्काऊ -EIO;
                रोक;
        }
#endif

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                माप_प्रकार rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                यदि (sym_LZ4F_isError(rc))
                        फर्काऊ -ENOMEM;

                रोक;
        }
#endif

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                यदि (!c->d_zstd)
                        फर्काऊ -ENOMEM;
                रोक;
#endif

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                यदि (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णाङ्क) परिमाण(c->gzip)) != Z_OK)
                        फर्काऊ -EIO;
                रोक;
#endif

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                यदि (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        फर्काऊ -EIO;
                रोक;
#endif

        स्वभाव:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        फर्काऊ 0;
}

स्थिर पूर्णाङ्क decompress_blob_to_fd_impl(
                Compression type,
                अचर शून्य *src,
                uint64_t src_size,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_size) {

        पूर्णाङ्क r;

        प्रतिज्ञा(src);
        प्रतिज्ञा(fdt >= 0);

        यदि (type == COMPRESSION_NONE)
                फर्काऊ -EOPNOTSUPP;

        यदि (src_size == 0)
                फर्काऊ -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        यदि (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                यदि (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        यदि (src_size <= परिमाण(uint64_t))
                                फर्काऊ -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        यदि (sz < skip)
                                फर्काऊ -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                यदि (max_bytes == 0)
                        फर्काऊ -ENOBUFS;

                _cleanup_free_ शून्य *buf = रिक्त;
                माप_प्रकार size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (माप_प्रकार) max_bytes);
                यदि (r < 0)
                        फर्काऊ r;

                /* Check the result size again for safety. */
                यदि (size < skip)
                        फर्काऊ -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                यदि (r < 0)
                        फर्काऊ r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (द्विगुण) size / src_size * 100);

                यदि (ret_size)
                        *ret_size = size - skip;
                फर्काऊ 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = रिक्त;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        यदि (r < 0)
                फर्काऊ r;

        संरचना decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_थप(c, src, src_size, decompress_stream_write_callback, &userdata);
        यदि (r < 0)
                फर्काऊ r;

        /* Finalize */
        r = decompressor_थप(c, /* data= */ रिक्त, /* size= */ 0, decompress_stream_write_callback, &userdata);
        यदि (r < 0)
                फर्काऊ r;

        यदि (userdata.total_out < skip)
                फर्काऊ -EBADMSG;

        यदि (userdata.sparse) {
                r = finalize_sparse(fdt);
                यदि (r < 0)
                        फर्काऊ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (द्विगुण) userdata.total_out / src_size * 100);

        यदि (ret_size)
                *ret_size = userdata.total_out - skip;

        फर्काऊ 0;
}

पूर्णाङ्क decompress_blob_to_fd_journal(
                Compression type,
                अचर शून्य *src,
                uint64_t src_size,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_size) {

        पूर्णाङ्क r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

पूर्णाङ्क decompress_blob_to_fd(
                Compression type,
                अचर शून्य *src,
                uint64_t src_size,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_size) {

        पूर्णाङ्क r;

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

पूर्णाङ्क decompress_stream_full(
                Compression type,
                पूर्णाङ्क fdf, पूर्णाङ्क fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = रिक्त;
        _cleanup_free_ uint8_t *buf = रिक्त;
        uint64_t total_in = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(fdf >= 0);
        प्रतिज्ञा(fdt >= 0);

        यदि (type == COMPRESSION_NONE)
                फर्काऊ -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        r = decompressor_new(&c, type, flags);
        यदि (r < 0)
                फर्काऊ r;

        संरचना decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        यदि (!buf)
                फर्काऊ -ENOMEM;

        लागि (;;) {
                ssize_t n;

                n = पठ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                यदि (n < 0)
                        फर्काऊ -त्रुटि_सं;

                total_in += n;

                r = decompressor_थप(c, buf, n, decompress_stream_write_callback, &userdata);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (n == 0)
                        रोक;
        }

        यदि (total_in == 0)
                फर्काऊ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        यदि (userdata.sparse) {
                r = finalize_sparse(fdt);
                यदि (r < 0)
                        फर्काऊ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (द्विगुण) userdata.total_out / total_in * 100);

        फर्काऊ 0;
}

पूर्णाङ्क decompress_stream_by_filename(अचर वर्ण *filename, पूर्णाङ्क fdf, पूर्णाङ्क fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        यदि (c == COMPRESSION_NONE)
                फर्काऊ -EPROTONOSUPPORT;

        फर्काऊ decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        यदि (!c)
                फर्काऊ रिक्त;

        विकल्प (c->type) {

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                रोक;
#endif

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4:
                यदि (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = रिक्त;
                        c->lz4_header = mfree(c->lz4_header);
                } अन्यथा {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = रिक्त;
                }
                रोक;
#endif

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                यदि (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = रिक्त;
                } अन्यथा {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = रिक्त;
                }
                रोक;
#endif

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                यदि (c->encoding)
                        sym_deflateEnd(&c->gzip);
                अन्यथा
                        sym_inflateEnd(&c->gzip);
                रोक;
#endif

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                यदि (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                अन्यथा
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                रोक;
#endif

        स्वभाव:
                रोक;
        }

        फर्काऊ mfree(c);
}

Compression compressor_type(अचर Compressor *c) {
        फर्काऊ c ? c->type : _COMPRESSION_INVALID;
}

पूर्णाङ्क decompressor_detect(Decompressor **decompressor, अचर शून्य *data, माप_प्रकार size) {
        पूर्णाङ्क r;

        प्रतिज्ञा(decompressor);

        यदि (*decompressor)
                फर्काऊ 1;

        यदि (size < COMPRESSION_MAGIC_BYTES_MAX)
                फर्काऊ 0;

        प्रतिज्ञा(data);

        Compression type = compression_detect_from_magic(data);
        यदि (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ 1;
}

पूर्णाङ्क decompressor_force(Decompressor **decompressor, Compression type) {
        पूर्णाङ्क r;

        प्रतिज्ञा(decompressor);
        प्रतिज्ञा(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        *decompressor = compressor_free(*decompressor);
        फर्काऊ decompressor_new(decompressor, type, /* flags= */ 0);
}

पूर्णाङ्क decompressor_force_off(Decompressor **decompressor) {
        फर्काऊ decompressor_force(decompressor, COMPRESSION_NONE);
}

पूर्णाङ्क decompressor_थप(Decompressor *c, अचर शून्य *data, माप_प्रकार size, DecompressorCallback callback, शून्य *userdata) {
        पूर्णाङ्क r;

        प्रतिज्ञा(c);
        प्रतिज्ञा(data || size == 0);
        प्रतिज्ञा(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        यदि (c->encoding)
                फर्काऊ -EINVAL;

        यदि (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                फर्काऊ 0;

#यदि HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = रिक्त;
        यदि (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                यदि (!buffer)
                        फर्काऊ -ENOMEM;
        }
#endif

        विकल्प (c->type) {

        पक्ष COMPRESSION_NONE:
                यदि (c->finished)
                        फर्काऊ -EBADMSG;

                यदि (size == 0) {
                        c->finished = true;
                        फर्काऊ 0;
                }

                r = callback(data, size, userdata);
                यदि (r < 0)
                        फर्काऊ r;

                फर्काऊ 0;

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ:
                यदि (c->finished)
                        फर्काऊ -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                लागि (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        यदि (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                फर्काऊ -EBADMSG;

                        यदि (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                फर्काऊ -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        यदि (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                यदि (r < 0)
                                        फर्काऊ r;
                        }

                        यदि (lzr == LZMA_STREAM_END) {
                                यदि (c->xz.avail_in > 0)
                                        फर्काऊ -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                फर्काऊ 0;
                        }

                        यदि (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                यदि (size == 0)
                                        फर्काऊ -EBADMSG; /* truncated */

                                फर्काऊ 0; /* No progress possible with the current input. */
                        }
                }
#endif

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                अचर uint8_t *src = data;
                माप_प्रकार src_remaining = size;

                c->finished = false;

                लागि (;;) {
                        माप_प्रकार produced = COMPRESS_PIPE_BUFFER_SIZE;
                        माप_प्रकार consumed = src_remaining;

                        माप_प्रकार rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, रिक्त);
                        यदि (sym_LZ4F_isError(rc))
                                फर्काऊ -EBADMSG;

                        यदि (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                फर्काऊ -EIO;

                        यदि (consumed > 0) {
                                प्रतिज्ञा(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        यदि (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                यदि (r < 0)
                                        फर्काऊ r;
                        }

                        यदि (src_remaining == 0) {
                                यदि (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        फर्काऊ 0;
                                }

                                यदि (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        जारी;

                                यदि (size == 0)
                                        फर्काऊ -EBADMSG; /* truncated */

                                फर्काऊ 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_inBuffer आगत = {
                        .src =  (शून्य*) data,
                        .size = size,
                };

                c->finished = false;

                लागि (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        माप_प्रकार res = sym_ZSTD_decompressStream(c->d_zstd, &output, &आगत);
                        यदि (sym_ZSTD_isError(res))
                                फर्काऊ -EBADMSG;

                        यदि (आगत.pos > आगत.size ||
                            output.pos > output.size)
                                फर्काऊ -EIO;

                        यदि (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                यदि (r < 0)
                                        फर्काऊ r;
                        }

                        यदि (आगत.pos == आगत.size) {
                                यदि (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        फर्काऊ 0;
                                }

                                यदि (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        जारी;

                                यदि (size == 0)
                                        फर्काऊ -EBADMSG; /* truncated */

                                फर्काऊ 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                यदि (size > अच_पूर्णांक_उच्च)
                        फर्काऊ -EFBIG;

                c->gzip.next_in = (शून्य*) data;
                c->gzip.avail_in = size;

                लागि (;;) {
                        यदि (c->finished) {
                                यदि (sym_inflateReset(&c->gzip) != Z_OK)
                                        फर्काऊ -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        पूर्णाङ्क zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        यदि (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                फर्काऊ -EBADMSG;

                        यदि (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                फर्काऊ -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        यदि (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                यदि (r < 0)
                                        फर्काऊ r;
                        }

                        यदि (zr == Z_STREAM_END) {
                                c->finished = true;

                                यदि (c->gzip.avail_in > 0)
                                        जारी; /* New stream may be pending. */

                                फर्काऊ 0;
                        }

                        यदि (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                यदि (size == 0)
                                        फर्काऊ -EBADMSG; /* truncated */

                                फर्काऊ 0; /* No progress possible with the current input. */
                        }
                }
#endif

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                यदि (size > अच_पूर्णांक_उच्च)
                        फर्काऊ -EFBIG;

                c->bzip2.next_in = (वर्ण*) data;
                c->bzip2.avail_in = size;

                लागि (;;) {
                        यदि (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                यदि (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        फर्काऊ -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (वर्ण*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        पूर्णाङ्क bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        यदि (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                फर्काऊ -EBADMSG;

                        यदि (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                फर्काऊ -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        यदि (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                यदि (r < 0)
                                        फर्काऊ r;
                        }

                        यदि (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                यदि (c->bzip2.avail_in > 0)
                                        जारी; /* New stream may be pending. */

                                फर्काऊ 0;
                        }

                        यदि (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                यदि (size == 0)
                                        फर्काऊ -EBADMSG; /* truncated */

                                फर्काऊ 0; /* No progress possible with the current input. */
                        }
                }
#endif

        स्वभाव:
                assert_not_reached();
        }
}

पूर्णाङ्क compressor_new(Compressor **ret, Compression type) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        यदि (r < 0)
                फर्काऊ r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        यदि (!c)
                फर्काऊ -ENOMEM;

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
                        फर्काऊ -EIO;

                c->type = COMPRESSION_XZ;
                रोक;
        }
#endif

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                माप_प्रकार rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                यदि (sym_LZ4F_isError(rc))
                        फर्काऊ -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                माप_प्रकार header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = दो_स्मृति(header_bound);
                यदि (!c->lz4_header)
                        फर्काऊ -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                यदि (sym_LZ4F_isError(c->lz4_header_size))
                        फर्काऊ -EINVAL;

                रोक;
        }
#endif

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                यदि (!c->c_zstd)
                        फर्काऊ -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                माप_प्रकार z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                यदि (sym_ZSTD_isError(z))
                        फर्काऊ -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                यदि (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                रोक;
#endif

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (पूर्णाङ्क) परिमाण(c->gzip));
                यदि (r != Z_OK)
                        फर्काऊ -EIO;

                c->type = COMPRESSION_GZIP;
                रोक;
#endif

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                यदि (r != BZ_OK)
                        फर्काऊ -EIO;

                c->type = COMPRESSION_BZIP2;
                रोक;
#endif

        पक्ष COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                रोक;

        स्वभाव:
                फर्काऊ -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        फर्काऊ 0;
}

#यदि HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
स्थिर पूर्णाङ्क enlarge_buffer(शून्य **buffer, माप_प्रकार *buffer_size, माप_प्रकार *buffer_allocated, माप_प्रकार need) {
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (माप_प्रकार) COMPRESS_PIPE_BUFFER_SIZE);
        यदि (*buffer_allocated >= need)
                फर्काऊ 0;

        यदि (!greedy_realloc(buffer, need, 1))
                फर्काऊ -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        फर्काऊ 1;
}
#endif

पूर्णाङ्क compressor_start(
                Compressor *c,
                अचर शून्य *data,
                माप_प्रकार size,
                शून्य **buffer,
                माप_प्रकार *buffer_size,
                माप_प्रकार *buffer_allocated) {

#यदि HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णाङ्क r;
#endif

        प्रतिज्ञा(c);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        यदि (!c->encoding)
                फर्काऊ -EINVAL;

        यदि (size == 0)
                फर्काऊ 0;

        प्रतिज्ञा(data);

        *buffer_size = 0;

        विकल्प (c->type) {

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                जबसम्म (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        यदि (r < 0)
                                फर्काऊ r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        यदि (lzr != LZMA_OK)
                                फर्काऊ -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                रोक;
#endif

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                यदि (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        यदि (r < 0)
                                फर्काऊ r;

                        स_नकल(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                माप_प्रकार bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                यदि (r < 0)
                        फर्काऊ r;

                माप_प्रकार n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, रिक्त);
                यदि (sym_LZ4F_isError(n))
                        फर्काऊ -EIO;

                *buffer_size += n;
                रोक;
        }
#endif

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_inBuffer आगत = {
                        .src = data,
                        .size = size,
                };

                जबसम्म (आगत.pos < आगत.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        यदि (r < 0)
                                फर्काऊ r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        माप_प्रकार res = sym_ZSTD_compressStream2(c->c_zstd, &output, &आगत, ZSTD_e_continue);
                        यदि (sym_ZSTD_isError(res))
                                फर्काऊ -EIO;

                        *buffer_size += output.pos;
                }

                रोक;
        }
#endif

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                यदि (size > अच_पूर्णांक_उच्च)
                        फर्काऊ -EFBIG;

                c->gzip.next_in = (शून्य*) data;
                c->gzip.avail_in = size;

                जबसम्म (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        यदि (r < 0)
                                फर्काऊ r;

                        माप_प्रकार avail = MIN(*buffer_allocated - *buffer_size, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        यदि (r != Z_OK)
                                फर्काऊ -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                रोक;
#endif

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                यदि (size > अच_पूर्णांक_उच्च)
                        फर्काऊ -EFBIG;

                c->bzip2.next_in = (शून्य*) data;
                c->bzip2.avail_in = size;

                जबसम्म (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        यदि (r < 0)
                                फर्काऊ r;

                        माप_प्रकार avail = MIN(*buffer_allocated - *buffer_size, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->bzip2.next_out = (शून्य*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        यदि (r != BZ_RUN_OK)
                                फर्काऊ -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                रोक;
#endif

        पक्ष COMPRESSION_NONE:

                यदि (*buffer_allocated < size) {
                        शून्य *p;

                        p = पुनः_स्मृति(*buffer, size);
                        यदि (!p)
                                फर्काऊ -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                स_नकल(*buffer, data, size);
                *buffer_size = size;
                रोक;

        स्वभाव:
                फर्काऊ -EOPNOTSUPP;
        }

        फर्काऊ 0;
}

पूर्णाङ्क compressor_finish(Compressor *c, शून्य **buffer, माप_प्रकार *buffer_size, माप_प्रकार *buffer_allocated) {
#यदि HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णाङ्क r;
#endif

        प्रतिज्ञा(c);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        यदि (!c->encoding)
                फर्काऊ -EINVAL;

        *buffer_size = 0;

        विकल्प (c->type) {

#यदि HAVE_XZ
        पक्ष COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                गर {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        यदि (r < 0)
                                फर्काऊ r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        यदि (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                फर्काऊ -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } जबसम्म (lzr != LZMA_STREAM_END);

                रोक;
        }
#endif

#यदि HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                माप_प्रकार bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                यदि (r < 0)
                        फर्काऊ r;

                माप_प्रकार n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, रिक्त);
                यदि (sym_LZ4F_isError(n))
                        फर्काऊ -EIO;

                *buffer_size = n;
                रोक;
        }
#endif

#यदि HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_inBuffer आगत = {};
                माप_प्रकार res;

                गर {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        यदि (r < 0)
                                फर्काऊ r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &आगत, ZSTD_e_end);
                        यदि (sym_ZSTD_isError(res))
                                फर्काऊ -EIO;

                        *buffer_size += output.pos;
                } जबसम्म (res != 0);

                रोक;
        }
#endif

#यदि HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                गर {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        यदि (r < 0)
                                फर्काऊ r;

                        माप_प्रकार avail = MIN(*buffer_allocated - *buffer_size, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        यदि (!IN_SET(r, Z_OK, Z_STREAM_END))
                                फर्काऊ -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } जबसम्म (r != Z_STREAM_END);

                रोक;
#endif

#यदि HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                गर {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        यदि (r < 0)
                                फर्काऊ r;

                        माप_प्रकार avail = MIN(*buffer_allocated - *buffer_size, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->bzip2.next_out = (शून्य*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        यदि (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                फर्काऊ -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } जबसम्म (r != BZ_STREAM_END);

                रोक;
#endif

        पक्ष COMPRESSION_NONE:
                रोक;

        स्वभाव:
                फर्काऊ -EOPNOTSUPP;
        }

        फर्काऊ 0;
}
