/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#जर HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#अन्तर्भाव <lzma.h>
#endif

#जर HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#अन्तर्भाव <lz4.h>
#अन्तर्भाव <lz4frame.h>
#अन्तर्भाव <lz4hc.h>
#endif

#जर HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#अन्तर्भाव <zstd.h>
#अन्तर्भाव <zstd_errors.h>
#endif

#जर HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#अन्तर्भाव <zlib.h>
#endif

#जर HAVE_BZIP2
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

#जर HAVE_XZ
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
स्थिर ओळीत शून्य lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#जर HAVE_LZ4
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
स्थिर DLSYM_PROTOTYPE(LZ4_version_संख्या) = रिक्त;

स्थिर स्थिरांक LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#जर HAVE_ZSTD
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

स्थिर पूर्णांक zstd_ret_to_errno(माप_प्रकार ret) {
        निवड (sym_ZSTD_getErrorCode(ret)) {
        स्थिती ZSTD_error_dstSize_tooSmall:
                परत -ENOBUFS;
        स्थिती ZSTD_error_memory_allocation:
                परत -ENOMEM;
        पूर्वनिर्धारित:
                परत -EBADMSG;
        }
}
#endif

#जर HAVE_ZLIB
स्थिर DLSYM_PROTOTYPE(deflateInit2_) = रिक्त;
स्थिर DLSYM_PROTOTYPE(deflate) = रिक्त;
स्थिर DLSYM_PROTOTYPE(deflateEnd) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflateInit2_) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflate) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflateEnd) = रिक्त;
स्थिर DLSYM_PROTOTYPE(inflateReset) = रिक्त;

स्थिर ओळीत शून्य deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

स्थिर ओळीत शून्य inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#जर HAVE_BZIP2
स्थिर DLSYM_PROTOTYPE(BZ2_bzCompressInit) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzCompress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzDecompress) = रिक्त;
स्थिर DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = रिक्त;

स्थिर ओळीत शून्य BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

स्थिर ओळीत शून्य BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
संरचना Compressor {
        Compression type;
        bool encoding;
        bool finished;
        संघ {
#जर HAVE_XZ
                lzma_stream xz;
#endif
#जर HAVE_LZ4
                संरचना {
                        LZ4F_compressionContext_t c_lz4;
                        शून्य *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        माप_प्रकार lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#जर HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#जर HAVE_ZLIB
                z_stream gzip;
#endif
#जर HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, आकार(माप_प्रकार))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

स्थिर स्थिरांक अक्षर* स्थिरांक compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

स्थिर स्थिरांक अक्षर* स्थिरांक compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

स्थिर स्थिरांक अक्षर* स्थिरांक compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(स्थिरांक अक्षर *s) {
        Compression c;

        प्रतिज्ञा(s);

        c = compression_from_string(s);
        जर (c >= 0)
                परत c;

        परत compression_uppercase_from_string(s);
}

Compression compression_from_filename(स्थिरांक अक्षर *filename) {
        Compression c;
        स्थिरांक अक्षर *e;

        प्रतिज्ञा(filename);

        e = म_खोजप(filename, '.');
        जर (!e)
                परत COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        जर (c < 0)
                परत COMPRESSION_NONE;

        परत c;
}

bool compression_supported_journal(Compression c) {
        स्थिर स्थिरांक अचिन्हित supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        प्रतिज्ञा(c >= 0);
        प्रतिज्ञा(c < _COMPRESSION_MAX);

        परत BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        स्थिर स्थिरांक अचिन्हित supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        प्रतिज्ञा(c >= 0);
        प्रतिज्ञा(c < _COMPRESSION_MAX);

        परत BIT_SET(supported, c);
}

Compression compression_detect_from_magic(स्थिरांक uint8_t data[स्थिर COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        जर (स_भेद(data, (स्थिरांक uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                परत COMPRESSION_GZIP;
        जर (स_भेद(data, (स्थिरांक uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                परत COMPRESSION_XZ;
        जर (स_भेद(data, (स्थिरांक uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                परत COMPRESSION_ZSTD;
        जर (स_भेद(data, (स्थिरांक uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                परत COMPRESSION_LZ4;
        जर (स_भेद(data, (स्थिरांक uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                परत COMPRESSION_BZIP2;

        परत _COMPRESSION_INVALID;
}

पूर्णांक dlopen_xz(पूर्णांक log_level) {
#जर HAVE_XZ
        स्थिर शून्य *lzma_dl = रिक्त;

        LIBLZMA_NOTE(suggested);

        परत dlopen_many_sym_or_warn(
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
        परत log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

पूर्णांक dlopen_lz4(पूर्णांक log_level) {
#जर HAVE_LZ4
        स्थिर शून्य *lz4_dl = रिक्त;

        LIBLZ4_NOTE(suggested);

        परत dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_संख्या));
#अन्यथा
        परत log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

पूर्णांक dlopen_zstd(पूर्णांक log_level) {
#जर HAVE_ZSTD
        स्थिर शून्य *zstd_dl = रिक्त;

        LIBZSTD_NOTE(suggested);

        परत dlopen_many_sym_or_warn(
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
        परत log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

पूर्णांक dlopen_zlib(पूर्णांक log_level) {
#जर HAVE_ZLIB
        स्थिर शून्य *zlib_dl = रिक्त;

        LIBZ_NOTE(suggested);

        परत dlopen_many_sym_or_warn(
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
        परत log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

पूर्णांक dlopen_bzip2(पूर्णांक log_level) {
#जर HAVE_BZIP2
        स्थिर शून्य *bzip2_dl = रिक्त;

        LIBBZ2_NOTE(suggested);

        परत dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#अन्यथा
        परत log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

पूर्णांक dlopen_compress_journal(Compression c, पूर्णांक log_level) {
        निवड (c) {
        स्थिती COMPRESSION_NONE:
                परत 0;
        स्थिती COMPRESSION_XZ:
                परत dlopen_xz(log_level);
        स्थिती COMPRESSION_LZ4:
                परत dlopen_lz4(log_level);
        स्थिती COMPRESSION_ZSTD:
                परत dlopen_zstd(log_level);
        पूर्वनिर्धारित:
                परत -EOPNOTSUPP;
        }
}

पूर्णांक dlopen_compress(Compression c, पूर्णांक log_level) {
        निवड (c) {
        स्थिती COMPRESSION_GZIP:
                परत dlopen_zlib(log_level);
        स्थिती COMPRESSION_BZIP2:
                परत dlopen_bzip2(log_level);
        पूर्वनिर्धारित:
                परत dlopen_compress_journal(c, log_level);
        }
}

स्थिर पूर्णांक compress_blob_xz(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                माप_प्रकार dst_alloc_size,
                माप_प्रकार *dst_size,
                पूर्णांक level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#जर HAVE_XZ
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
        पूर्णांक r;

        r = dlopen_xz(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                जर (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        परत -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        जर (src_size < 80)
                परत -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ रिक्त,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        जर (ret != LZMA_OK)
                परत -ENOBUFS;

        *dst_size = out_pos;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक compress_blob_lz4(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                माप_प्रकार dst_alloc_size,
                माप_प्रकार *dst_size,
                पूर्णांक level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#जर HAVE_LZ4
        पूर्णांक r;

        r = dlopen_lz4(LOG_DEBUG);
        जर (r < 0)
                परत r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        जर (src_size < 9)
                परत -ENOBUFS;

        जर (src_size > पूर्णांक_उच्च)
                परत -EFBIG;
        जर (dst_alloc_size > पूर्णांक_उच्च)
                dst_alloc_size = पूर्णांक_उच्च;

        जर (level <= 0)
                r = sym_LZ4_compress_default(src, (अक्षर*)dst + 8, src_size, (पूर्णांक) dst_alloc_size - 8);
        अन्यथा
                r = sym_LZ4_compress_HC(src, (अक्षर*)dst + 8, src_size, (पूर्णांक) dst_alloc_size - 8, level);
        जर (r <= 0)
                परत -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक compress_blob_zstd(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                माप_प्रकार dst_alloc_size,
                माप_प्रकार *dst_size,
                पूर्णांक level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#जर HAVE_ZSTD
        माप_प्रकार k;
        पूर्णांक r;

        r = dlopen_zstd(LOG_DEBUG);
        जर (r < 0)
                परत r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        जर (sym_ZSTD_isError(k))
                परत zstd_ret_to_errno(k);

        *dst_size = k;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक compress_blob_gzip(स्थिरांक शून्य *src, uint64_t src_size,
                       शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णांक level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#जर HAVE_ZLIB
        पूर्णांक r;

        r = dlopen_zlib(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size > अच_पूर्णांक_उच्च)
                परत -EFBIG;
        जर (dst_alloc_size > अच_पूर्णांक_उच्च)
                dst_alloc_size = अच_पूर्णांक_उच्च;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (पूर्णांक) आकार(s));
        जर (r != Z_OK)
                परत -ENOMEM;

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        जर (r != Z_STREAM_END)
                परत -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक compress_blob_bzip2(
                स्थिरांक शून्य *src, uint64_t src_size,
                शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णांक level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#जर HAVE_BZIP2
        पूर्णांक r;

        r = dlopen_bzip2(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size > अच_पूर्णांक_उच्च)
                परत -EFBIG;
        जर (dst_alloc_size > अच_पूर्णांक_उच्च)
                dst_alloc_size = अच_पूर्णांक_उच्च;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        जर (r != BZ_OK)
                परत -ENOMEM;

        s.next_in = (अक्षर*) src;
        s.avail_in = src_size;
        s.next_out = (अक्षर*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        जर (r != BZ_STREAM_END)
                परत -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

पूर्णांक compress_blob_journal(
                Compression compression,
                स्थिरांक शून्य *src, uint64_t src_size,
                शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णांक level) {

        निवड (compression) {
        स्थिती COMPRESSION_XZ:
                परत compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        स्थिती COMPRESSION_LZ4:
                परत compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        स्थिती COMPRESSION_ZSTD:
                परत compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        पूर्वनिर्धारित:
                परत -EOPNOTSUPP;
        }
}

पूर्णांक compress_blob(
                Compression compression,
                स्थिरांक शून्य *src, uint64_t src_size,
                शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णांक level) {

        निवड (compression) {
        स्थिती COMPRESSION_GZIP:
                परत compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        स्थिती COMPRESSION_BZIP2:
                परत compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        पूर्वनिर्धारित:
                परत compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

स्थिर पूर्णांक decompress_blob_xz(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#जर HAVE_XZ
        पूर्णांक r;

        r = dlopen_xz(LOG_DEBUG);
        जर (r < 0)
                परत r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        जर (ret != LZMA_OK)
                परत -ENOMEM;

        माप_प्रकार space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        जर (!greedy_realloc(dst, space, 1))
                परत -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        साठी (;;) {
                माप_प्रकार used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                जर (ret == LZMA_STREAM_END)
                        थांबा;
                जर (ret != LZMA_OK)
                        परत -ENOMEM;

                जर (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        थांबा;
                जर (dst_max > 0 && space == dst_max)
                        परत -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                प्रतिज्ञा(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                जर (!greedy_realloc(dst, space, 1))
                        परत -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक decompress_blob_lz4(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#जर HAVE_LZ4
        अक्षर* out;
        पूर्णांक r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size <= 8)
                परत -EBADMSG;

        जर (src_size - 8 > पूर्णांक_उच्च)
                परत -EFBIG;

        size = unaligned_read_le64(src);
        जर (size < 0 || (अचिन्हित) size != unaligned_read_le64(src))
                परत -EFBIG;
        जर (dst_max > 0 && (माप_प्रकार) size > dst_max)
                परत -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        जर (!out)
                परत -ENOMEM;

        r = sym_LZ4_decompress_safe((अक्षर*)src + 8, out, src_size - 8, size);
        जर (r < 0 || r != size)
                परत -EBADMSG;

        *dst_size = size;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

माप_प्रकार zstd_dstream_out_size(शून्य) {
#जर HAVE_ZSTD
        जर (dlopen_zstd(LOG_DEBUG) < 0)
                परत 0;

        परत sym_ZSTD_DStreamOutSize();
#अन्यथा
        परत 0;
#endif
}

स्थिर पूर्णांक decompress_blob_zstd(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#जर HAVE_ZSTD
        पूर्णांक r;

        r = dlopen_zstd(LOG_DEBUG);
        जर (r < 0)
                परत r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        अचिन्हित दीर्घ दीर्घ size = sym_ZSTD_getFrameContentSize(src, src_size);
        जर (size == ZSTD_CONTENTSIZE_ERROR)
                परत -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        माप_प्रकार cap = dst_max ?: SIZE_MAX;

        माप_प्रकार space;
        जर (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (माप_प्रकार) MIN(size, (अचिन्हित दीर्घ दीर्घ) cap));
        अन्यथा {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (माप_प्रकार) MIN(twice, (uint64_t) cap));
        }

        जर (!greedy_realloc(dst, space, 1))
                परत -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        जर (!dctx)
                परत -ENOMEM;

        ZSTD_inBuffer आदान = {
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

        साठी (;;) {
                माप_प्रकार k = sym_ZSTD_decompressStream(dctx, &output, &आदान);
                जर (sym_ZSTD_isError(k))
                        परत log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                जर (output.pos >= cap)
                        थांबा;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                जर (k == 0 && आदान.pos >= आदान.size)
                        थांबा;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                जर (output.pos == output.size) {
                        जर (space > SIZE_MAX / 2)
                                परत -E2BIG;
                        space = MIN(2 * space, cap);
                        जर (!greedy_realloc(dst, space, 1))
                                परत -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        सुरूठेवा;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                जर (आदान.pos >= आदान.size)
                        परत log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक decompress_blob_gzip(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#जर HAVE_ZLIB
        पूर्णांक r;

        r = dlopen_zlib(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size > अच_पूर्णांक_उच्च)
                परत -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णांक) आकार(s));
        जर (r != Z_OK)
                परत -ENOMEM;

        माप_प्रकार space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
        जर (!greedy_realloc(dst, space, 1))
                परत -ENOMEM;

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        साठी (;;) {
                माप_प्रकार used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                जर (r == Z_STREAM_END)
                        थांबा;
                जर (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        परत -EBADMSG;

                जर (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        थांबा;
                जर (dst_max > 0 && space == dst_max)
                        परत -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, अच_पूर्णांक_उच्च);
                जर (!greedy_realloc(dst, space, 1))
                        परत -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक decompress_blob_bzip2(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#जर HAVE_BZIP2
        पूर्णांक r;

        r = dlopen_bzip2(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size > अच_पूर्णांक_उच्च)
                परत -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        जर (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                परत -EIO;

        माप_प्रकार space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
        जर (!greedy_realloc(dst, space, 1))
                परत -ENOMEM;

        s.next_in = (अक्षर*) src;
        s.avail_in = src_size;
        s.next_out = (अक्षर*) *dst;
        s.avail_out = space;

        साठी (;;) {
                माप_प्रकार used;

                r = sym_BZ2_bzDecompress(&s);
                जर (r == BZ_STREAM_END)
                        थांबा;
                जर (r != BZ_OK)
                        परत -EBADMSG;

                जर (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        थांबा;
                जर (dst_max > 0 && space == dst_max)
                        परत -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
                जर (!greedy_realloc(dst, space, 1))
                        परत -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (अक्षर*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

पूर्णांक decompress_blob_journal(
                Compression compression,
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        निवड (compression) {
        स्थिती COMPRESSION_XZ:
                परत decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        स्थिती COMPRESSION_LZ4:
                परत decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        स्थिती COMPRESSION_ZSTD:
                परत decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        पूर्वनिर्धारित:
                परत -EPROTONOSUPPORT;
        }
}

पूर्णांक decompress_blob(
                Compression compression,
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                माप_प्रकार *dst_size,
                माप_प्रकार dst_max) {

        निवड (compression) {
        स्थिती COMPRESSION_GZIP:
                परत decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        स्थिती COMPRESSION_BZIP2:
                परत decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        पूर्वनिर्धारित:
                परत decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

पूर्णांक decompress_zlib_raw(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                माप_प्रकार dst_size,
                पूर्णांक wbits) {

#जर HAVE_ZLIB
        पूर्णांक r;

        r = dlopen_zlib(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size > अच_पूर्णांक_उच्च)
                परत -EFBIG;
        जर (dst_size > अच_पूर्णांक_उच्च)
                परत -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (शून्य*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (पूर्णांक) आकार(s));
        जर (r != Z_OK)
                परत -EIO;

        r = sym_inflate(&s, Z_FINISH);
        माप_प्रकार produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        जर (r != Z_STREAM_END || produced != dst_size)
                परत -EBADMSG;

        परत 0;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक decompress_startswith_xz(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                स्थिरांक शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#जर HAVE_XZ
        पूर्णांक r;

        r = dlopen_xz(LOG_DEBUG);
        जर (r < 0)
                परत r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        जर (ret != LZMA_OK)
                परत -EBADMSG;

        जर (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                परत -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        साठी (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                जर (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        परत -EBADMSG;

                जर (allocated - s.avail_out >= prefix_len + 1)
                        परत स_भेद(*buffer, prefix, prefix_len) == 0 &&
                                ((स्थिरांक uint8_t*) *buffer)[prefix_len] == extra;

                जर (ret == LZMA_STREAM_END)
                        परत 0;

                s.avail_out += allocated;

                जर (!(greedy_realloc(buffer, allocated * 2, 1)))
                        परत -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक decompress_startswith_lz4(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                स्थिरांक शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#जर HAVE_LZ4
        माप_प्रकार allocated;
        पूर्णांक r;

        r = dlopen_lz4(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size <= 8)
                परत -EBADMSG;

        जर (src_size - 8 > पूर्णांक_उच्च)
                परत -EFBIG;

        जर (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                परत -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (अक्षर*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        जर (r < 0 || (माप_प्रकार) r < prefix_len + 1) {
                माप_प्रकार size;

                जर (sym_LZ4_version_संख्या() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        परत 0;

                जर (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        जर (स_भेद(*buffer, prefix, r) != 0)
                                परत 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                जर (r < 0)
                        परत r;

                जर (size < prefix_len + 1)
                        परत 0;
        }

        परत स_भेद(*buffer, prefix, prefix_len) == 0 &&
                ((स्थिरांक uint8_t*) *buffer)[prefix_len] == extra;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक decompress_startswith_zstd(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                स्थिरांक शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#जर HAVE_ZSTD
        पूर्णांक r;

        r = dlopen_zstd(LOG_DEBUG);
        जर (r < 0)
                परत r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        अचिन्हित दीर्घ दीर्घ size = sym_ZSTD_getFrameContentSize(src, src_size);
        जर (size == ZSTD_CONTENTSIZE_ERROR)
                परत -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        जर (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                परत 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        जर (!dctx)
                परत -ENOMEM;

        जर (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                परत -ENOMEM;

        ZSTD_inBuffer आदान = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        माप_प्रकार k;

        k = sym_ZSTD_decompressStream(dctx, &output, &आदान);
        जर (sym_ZSTD_isError(k))
                परत log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        जर (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                जर (k != 0)
                        परत log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                परत 0; /* Decompressed text too short to match the prefix and extra */
        }

        परत स_भेद(*buffer, prefix, prefix_len) == 0 &&
                ((स्थिरांक uint8_t*) *buffer)[prefix_len] == extra;
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक decompress_startswith_gzip(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                स्थिरांक शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#जर HAVE_ZLIB
        पूर्णांक r;

        r = dlopen_zlib(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size > अच_पूर्णांक_उच्च)
                परत -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णांक) आकार(s));
        जर (r != Z_OK)
                परत -EBADMSG;

        जर (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                परत -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (माप_प्रकार) अच_पूर्णांक_उच्च);

        साठी (;;) {
                r = sym_inflate(&s, Z_FINISH);

                जर (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        परत -EBADMSG;

                जर (allocated - s.avail_out >= prefix_len + 1)
                        परत स_भेद(*buffer, prefix, prefix_len) == 0 &&
                                ((स्थिरांक uint8_t*) *buffer)[prefix_len] == extra;

                जर (r == Z_STREAM_END)
                        परत 0;

                जर (s.avail_out > 0)
                        परत -EBADMSG; /* truncated */

                माप_प्रकार used = allocated - s.avail_out;

                जर (!(greedy_realloc(buffer, allocated * 2, 1)))
                        परत -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (माप_प्रकार) अच_पूर्णांक_उच्च);
                s.next_out = *(uint8_t**)buffer + used;
        }
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णांक decompress_startswith_bzip2(
                स्थिरांक शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                स्थिरांक शून्य *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#जर HAVE_BZIP2
        पूर्णांक r;

        r = dlopen_bzip2(LOG_DEBUG);
        जर (r < 0)
                परत r;

        जर (src_size > अच_पूर्णांक_उच्च)
                परत -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        जर (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                परत -EIO;

        जर (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                परत -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (अक्षर*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (माप_प्रकार) अच_पूर्णांक_उच्च);

        साठी (;;) {
                r = sym_BZ2_bzDecompress(&s);

                जर (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        परत -EBADMSG;

                जर (allocated - s.avail_out >= prefix_len + 1)
                        परत स_भेद(*buffer, prefix, prefix_len) == 0 &&
                                ((स्थिरांक uint8_t*) *buffer)[prefix_len] == extra;

                जर (r == BZ_STREAM_END)
                        परत 0;

                जर (s.avail_out > 0)
                        परत -EBADMSG; /* truncated */

                माप_प्रकार used = allocated - s.avail_out;

                जर (!(greedy_realloc(buffer, allocated * 2, 1)))
                        परत -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (माप_प्रकार) अच_पूर्णांक_उच्च);
                s.next_out = (अक्षर*) *buffer + used;
        }
#अन्यथा
        परत -EPROTONOSUPPORT;
#endif
}

पूर्णांक decompress_startswith_journal(
                Compression compression,
                स्थिरांक शून्य *src, uint64_t src_size,
                शून्य **buffer,
                स्थिरांक शून्य *prefix, माप_प्रकार prefix_len,
                uint8_t extra) {

        निवड (compression) {
        स्थिती COMPRESSION_XZ:
                परत decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        स्थिती COMPRESSION_LZ4:
                परत decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        स्थिती COMPRESSION_ZSTD:
                परत decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        पूर्वनिर्धारित:
                परत -EOPNOTSUPP;
        }
}

पूर्णांक decompress_startswith(
                Compression compression,
                स्थिरांक शून्य *src, uint64_t src_size,
                शून्य **buffer,
                स्थिरांक शून्य *prefix, माप_प्रकार prefix_len,
                uint8_t extra) {

        निवड (compression) {
        स्थिती COMPRESSION_GZIP:
                परत decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        स्थिती COMPRESSION_BZIP2:
                परत decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        पूर्वनिर्धारित:
                परत decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

पूर्णांक compress_stream(
                Compression type,
                पूर्णांक fdf, पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = रिक्त;
        _cleanup_free_ शून्य *buf = रिक्त;
        _cleanup_free_ uint8_t *आदान = रिक्त;
        माप_प्रकार buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        पूर्णांक r;

        प्रतिज्ञा(fdf >= 0);
        प्रतिज्ञा(fdt >= 0);

        r = compressor_new(&c, type);
        जर (r < 0)
                परत r;

        आदान = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        जर (!आदान)
                परत -ENOMEM;

        साठी (;;) {
                माप_प्रकार m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                जर (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (माप_प्रकार) max_bytes;

                n = पठ(fdf, आदान, m);
                जर (n < 0)
                        परत -त्रुटि_सं;

                जर (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        जर (r < 0)
                                परत r;

                        जर (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                जर (r < 0)
                                        परत r;
                                total_out += buf_size;
                        }
                        थांबा;
                }

                total_in += n;
                जर (max_bytes != UINT64_MAX) {
                        प्रतिज्ञा(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, आदान, n, &buf, &buf_size, &buf_alloc);
                जर (r < 0)
                        परत r;

                जर (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        जर (r < 0)
                                परत r;
                        total_out += buf_size;
                }
        }

        जर (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        जर (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        अन्यथा
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (द्विगुण) total_out / total_in * 100);

        परत 0;
}

स्थिर पूर्णांक should_sparse(पूर्णांक fd) {
        संरचना stat st;

        प्रतिज्ञा(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        जर (ख_स्थिति(fd, &st) < 0)
                परत -त्रुटि_सं;

        जर (!S_ISREG(st.st_mode))
                परत false;

        पूर्णांक flags = fcntl(fd, F_GETFL);
        जर (flags < 0)
                परत -त्रुटि_सं;

        जर (FLAGS_SET(flags, O_APPEND))
                परत false;

        off_t pos = lseek(fd, 0, प्रस्तुत_से);
        जर (pos < 0)
                परत -त्रुटि_सं;

        परत pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
स्थिर पूर्णांक finalize_sparse(पूर्णांक fd) {
        off_t pos;

        प्रतिज्ञा(fd >= 0);

        pos = lseek(fd, 0, प्रस्तुत_से);
        जर (pos < 0)
                परत -त्रुटि_सं;

        जर (ftruncate(fd, pos) < 0)
                परत -त्रुटि_सं;

        परत 0;
}

/* Common helper for decompress_stream_*() wrappers */

संरचना decompress_stream_userdata {
        पूर्णांक fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

स्थिर पूर्णांक decompress_stream_write_callback(स्थिरांक शून्य *data, माप_प्रकार size, शून्य *userdata) {
        संरचना decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        जर (u->max_bytes != UINT64_MAX) {
                जर (u->max_bytes < size)
                        परत -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        जर (u->skip > 0) {
                जर (u->skip >= size) {
                        u->skip -= size;
                        परत 0;
                }

                data = (स्थिरांक uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        जर (size == 0)
                परत 0;

        जर (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                जर (k < 0)
                        परत (पूर्णांक) k;
                परत 0;
        }

        परत loop_write(u->fd, data, size);
}

स्थिर पूर्णांक decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        प्रतिज्ञा(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        जर (!c)
                परत -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        निवड (type) {
        स्थिती COMPRESSION_NONE:
                थांबा;

#जर HAVE_XZ
        स्थिती COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                जर (xz_ret != LZMA_OK)
                        परत -EIO;
                थांबा;
        }
#endif

#जर HAVE_LZ4
        स्थिती COMPRESSION_LZ4: {
                माप_प्रकार rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                जर (sym_LZ4F_isError(rc))
                        परत -ENOMEM;

                थांबा;
        }
#endif

#जर HAVE_ZSTD
        स्थिती COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                जर (!c->d_zstd)
                        परत -ENOMEM;
                थांबा;
#endif

#जर HAVE_ZLIB
        स्थिती COMPRESSION_GZIP:
                जर (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णांक) आकार(c->gzip)) != Z_OK)
                        परत -EIO;
                थांबा;
#endif

#जर HAVE_BZIP2
        स्थिती COMPRESSION_BZIP2:
                जर (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        परत -EIO;
                थांबा;
#endif

        पूर्वनिर्धारित:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        परत 0;
}

स्थिर पूर्णांक decompress_blob_to_fd_impl(
                Compression type,
                स्थिरांक शून्य *src,
                uint64_t src_size,
                पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_size) {

        पूर्णांक r;

        प्रतिज्ञा(src);
        प्रतिज्ञा(fdt >= 0);

        जर (type == COMPRESSION_NONE)
                परत -EOPNOTSUPP;

        जर (src_size == 0)
                परत -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        जर (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                जर (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        जर (src_size <= आकार(uint64_t))
                                परत -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        जर (sz < skip)
                                परत -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                जर (max_bytes == 0)
                        परत -ENOBUFS;

                _cleanup_free_ शून्य *buf = रिक्त;
                माप_प्रकार size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (माप_प्रकार) max_bytes);
                जर (r < 0)
                        परत r;

                /* Check the result size again for safety. */
                जर (size < skip)
                        परत -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                जर (r < 0)
                        परत r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (द्विगुण) size / src_size * 100);

                जर (ret_size)
                        *ret_size = size - skip;
                परत 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = रिक्त;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        जर (r < 0)
                परत r;

        संरचना decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ढकला(c, src, src_size, decompress_stream_write_callback, &userdata);
        जर (r < 0)
                परत r;

        /* Finalize */
        r = decompressor_ढकला(c, /* data= */ रिक्त, /* size= */ 0, decompress_stream_write_callback, &userdata);
        जर (r < 0)
                परत r;

        जर (userdata.total_out < skip)
                परत -EBADMSG;

        जर (userdata.sparse) {
                r = finalize_sparse(fdt);
                जर (r < 0)
                        परत r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (द्विगुण) userdata.total_out / src_size * 100);

        जर (ret_size)
                *ret_size = userdata.total_out - skip;

        परत 0;
}

पूर्णांक decompress_blob_to_fd_journal(
                Compression type,
                स्थिरांक शून्य *src,
                uint64_t src_size,
                पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_size) {

        पूर्णांक r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        जर (r < 0)
                परत r;

        परत decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

पूर्णांक decompress_blob_to_fd(
                Compression type,
                स्थिरांक शून्य *src,
                uint64_t src_size,
                पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_size) {

        पूर्णांक r;

        r = dlopen_compress(type, LOG_DEBUG);
        जर (r < 0)
                परत r;

        परत decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

पूर्णांक decompress_stream_full(
                Compression type,
                पूर्णांक fdf, पूर्णांक fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = रिक्त;
        _cleanup_free_ uint8_t *buf = रिक्त;
        uint64_t total_in = 0;
        पूर्णांक r;

        प्रतिज्ञा(fdf >= 0);
        प्रतिज्ञा(fdt >= 0);

        जर (type == COMPRESSION_NONE)
                परत -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        जर (r < 0)
                परत r;

        r = decompressor_new(&c, type, flags);
        जर (r < 0)
                परत r;

        संरचना decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        जर (!buf)
                परत -ENOMEM;

        साठी (;;) {
                ssize_t n;

                n = पठ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                जर (n < 0)
                        परत -त्रुटि_सं;

                total_in += n;

                r = decompressor_ढकला(c, buf, n, decompress_stream_write_callback, &userdata);
                जर (r < 0)
                        परत r;

                जर (n == 0)
                        थांबा;
        }

        जर (total_in == 0)
                परत log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        जर (userdata.sparse) {
                r = finalize_sparse(fdt);
                जर (r < 0)
                        परत r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (द्विगुण) userdata.total_out / total_in * 100);

        परत 0;
}

पूर्णांक decompress_stream_by_filename(स्थिरांक अक्षर *filename, पूर्णांक fdf, पूर्णांक fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        जर (c == COMPRESSION_NONE)
                परत -EPROTONOSUPPORT;

        परत decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        जर (!c)
                परत रिक्त;

        निवड (c->type) {

#जर HAVE_XZ
        स्थिती COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                थांबा;
#endif

#जर HAVE_LZ4
        स्थिती COMPRESSION_LZ4:
                जर (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = रिक्त;
                        c->lz4_header = mfree(c->lz4_header);
                } अन्यथा {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = रिक्त;
                }
                थांबा;
#endif

#जर HAVE_ZSTD
        स्थिती COMPRESSION_ZSTD:
                जर (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = रिक्त;
                } अन्यथा {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = रिक्त;
                }
                थांबा;
#endif

#जर HAVE_ZLIB
        स्थिती COMPRESSION_GZIP:
                जर (c->encoding)
                        sym_deflateEnd(&c->gzip);
                अन्यथा
                        sym_inflateEnd(&c->gzip);
                थांबा;
#endif

#जर HAVE_BZIP2
        स्थिती COMPRESSION_BZIP2:
                जर (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                अन्यथा
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                थांबा;
#endif

        पूर्वनिर्धारित:
                थांबा;
        }

        परत mfree(c);
}

Compression compressor_type(स्थिरांक Compressor *c) {
        परत c ? c->type : _COMPRESSION_INVALID;
}

पूर्णांक decompressor_detect(Decompressor **decompressor, स्थिरांक शून्य *data, माप_प्रकार size) {
        पूर्णांक r;

        प्रतिज्ञा(decompressor);

        जर (*decompressor)
                परत 1;

        जर (size < COMPRESSION_MAGIC_BYTES_MAX)
                परत 0;

        प्रतिज्ञा(data);

        Compression type = compression_detect_from_magic(data);
        जर (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        जर (r < 0)
                परत r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        जर (r < 0)
                परत r;

        परत 1;
}

पूर्णांक decompressor_force(Decompressor **decompressor, Compression type) {
        पूर्णांक r;

        प्रतिज्ञा(decompressor);
        प्रतिज्ञा(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        जर (r < 0)
                परत r;

        *decompressor = compressor_free(*decompressor);
        परत decompressor_new(decompressor, type, /* flags= */ 0);
}

पूर्णांक decompressor_force_off(Decompressor **decompressor) {
        परत decompressor_force(decompressor, COMPRESSION_NONE);
}

पूर्णांक decompressor_ढकला(Decompressor *c, स्थिरांक शून्य *data, माप_प्रकार size, DecompressorCallback callback, शून्य *userdata) {
        पूर्णांक r;

        प्रतिज्ञा(c);
        प्रतिज्ञा(data || size == 0);
        प्रतिज्ञा(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        जर (c->encoding)
                परत -EINVAL;

        जर (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                परत 0;

#जर HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = रिक्त;
        जर (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                जर (!buffer)
                        परत -ENOMEM;
        }
#endif

        निवड (c->type) {

        स्थिती COMPRESSION_NONE:
                जर (c->finished)
                        परत -EBADMSG;

                जर (size == 0) {
                        c->finished = true;
                        परत 0;
                }

                r = callback(data, size, userdata);
                जर (r < 0)
                        परत r;

                परत 0;

#जर HAVE_XZ
        स्थिती COMPRESSION_XZ:
                जर (c->finished)
                        परत -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                साठी (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        जर (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                परत -EBADMSG;

                        जर (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                परत -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        जर (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                जर (r < 0)
                                        परत r;
                        }

                        जर (lzr == LZMA_STREAM_END) {
                                जर (c->xz.avail_in > 0)
                                        परत -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                परत 0;
                        }

                        जर (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                जर (size == 0)
                                        परत -EBADMSG; /* truncated */

                                परत 0; /* No progress possible with the current input. */
                        }
                }
#endif

#जर HAVE_LZ4
        स्थिती COMPRESSION_LZ4: {
                स्थिरांक uint8_t *src = data;
                माप_प्रकार src_remaining = size;

                c->finished = false;

                साठी (;;) {
                        माप_प्रकार produced = COMPRESS_PIPE_BUFFER_SIZE;
                        माप_प्रकार consumed = src_remaining;

                        माप_प्रकार rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, रिक्त);
                        जर (sym_LZ4F_isError(rc))
                                परत -EBADMSG;

                        जर (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                परत -EIO;

                        जर (consumed > 0) {
                                प्रतिज्ञा(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        जर (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                जर (r < 0)
                                        परत r;
                        }

                        जर (src_remaining == 0) {
                                जर (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        परत 0;
                                }

                                जर (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        सुरूठेवा;

                                जर (size == 0)
                                        परत -EBADMSG; /* truncated */

                                परत 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#जर HAVE_ZSTD
        स्थिती COMPRESSION_ZSTD: {
                ZSTD_inBuffer आदान = {
                        .src =  (शून्य*) data,
                        .size = size,
                };

                c->finished = false;

                साठी (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        माप_प्रकार res = sym_ZSTD_decompressStream(c->d_zstd, &output, &आदान);
                        जर (sym_ZSTD_isError(res))
                                परत -EBADMSG;

                        जर (आदान.pos > आदान.size ||
                            output.pos > output.size)
                                परत -EIO;

                        जर (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                जर (r < 0)
                                        परत r;
                        }

                        जर (आदान.pos == आदान.size) {
                                जर (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        परत 0;
                                }

                                जर (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        सुरूठेवा;

                                जर (size == 0)
                                        परत -EBADMSG; /* truncated */

                                परत 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#जर HAVE_ZLIB
        स्थिती COMPRESSION_GZIP:
                जर (size > अच_पूर्णांक_उच्च)
                        परत -EFBIG;

                c->gzip.next_in = (शून्य*) data;
                c->gzip.avail_in = size;

                साठी (;;) {
                        जर (c->finished) {
                                जर (sym_inflateReset(&c->gzip) != Z_OK)
                                        परत -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        पूर्णांक zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        जर (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                परत -EBADMSG;

                        जर (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                परत -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        जर (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                जर (r < 0)
                                        परत r;
                        }

                        जर (zr == Z_STREAM_END) {
                                c->finished = true;

                                जर (c->gzip.avail_in > 0)
                                        सुरूठेवा; /* New stream may be pending. */

                                परत 0;
                        }

                        जर (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                जर (size == 0)
                                        परत -EBADMSG; /* truncated */

                                परत 0; /* No progress possible with the current input. */
                        }
                }
#endif

#जर HAVE_BZIP2
        स्थिती COMPRESSION_BZIP2:
                जर (size > अच_पूर्णांक_उच्च)
                        परत -EFBIG;

                c->bzip2.next_in = (अक्षर*) data;
                c->bzip2.avail_in = size;

                साठी (;;) {
                        जर (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                जर (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        परत -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (अक्षर*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        पूर्णांक bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        जर (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                परत -EBADMSG;

                        जर (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                परत -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        जर (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                जर (r < 0)
                                        परत r;
                        }

                        जर (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                जर (c->bzip2.avail_in > 0)
                                        सुरूठेवा; /* New stream may be pending. */

                                परत 0;
                        }

                        जर (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                जर (size == 0)
                                        परत -EBADMSG; /* truncated */

                                परत 0; /* No progress possible with the current input. */
                        }
                }
#endif

        पूर्वनिर्धारित:
                assert_not_reached();
        }
}

पूर्णांक compressor_new(Compressor **ret, Compression type) {
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        जर (r < 0)
                परत r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        जर (!c)
                परत -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        निवड (type) {

#जर HAVE_XZ
        स्थिती COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                जर (xzr != LZMA_OK)
                        परत -EIO;

                c->type = COMPRESSION_XZ;
                थांबा;
        }
#endif

#जर HAVE_LZ4
        स्थिती COMPRESSION_LZ4: {
                माप_प्रकार rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                जर (sym_LZ4F_isError(rc))
                        परत -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                माप_प्रकार header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = दो_स्मृति(header_bound);
                जर (!c->lz4_header)
                        परत -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                जर (sym_LZ4F_isError(c->lz4_header_size))
                        परत -EINVAL;

                थांबा;
        }
#endif

#जर HAVE_ZSTD
        स्थिती COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                जर (!c->c_zstd)
                        परत -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                माप_प्रकार z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                जर (sym_ZSTD_isError(z))
                        परत -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                जर (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                थांबा;
#endif

#जर HAVE_ZLIB
        स्थिती COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (पूर्णांक) आकार(c->gzip));
                जर (r != Z_OK)
                        परत -EIO;

                c->type = COMPRESSION_GZIP;
                थांबा;
#endif

#जर HAVE_BZIP2
        स्थिती COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                जर (r != BZ_OK)
                        परत -EIO;

                c->type = COMPRESSION_BZIP2;
                थांबा;
#endif

        स्थिती COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                थांबा;

        पूर्वनिर्धारित:
                परत -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        परत 0;
}

#जर HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
स्थिर पूर्णांक enlarge_buffer(शून्य **buffer, माप_प्रकार *buffer_size, माप_प्रकार *buffer_allocated, माप_प्रकार need) {
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (माप_प्रकार) COMPRESS_PIPE_BUFFER_SIZE);
        जर (*buffer_allocated >= need)
                परत 0;

        जर (!greedy_realloc(buffer, need, 1))
                परत -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        परत 1;
}
#endif

पूर्णांक compressor_start(
                Compressor *c,
                स्थिरांक शून्य *data,
                माप_प्रकार size,
                शून्य **buffer,
                माप_प्रकार *buffer_size,
                माप_प्रकार *buffer_allocated) {

#जर HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णांक r;
#endif

        प्रतिज्ञा(c);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        जर (!c->encoding)
                परत -EINVAL;

        जर (size == 0)
                परत 0;

        प्रतिज्ञा(data);

        *buffer_size = 0;

        निवड (c->type) {

#जर HAVE_XZ
        स्थिती COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                पर्यंत (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        जर (r < 0)
                                परत r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        जर (lzr != LZMA_OK)
                                परत -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                थांबा;
#endif

#जर HAVE_LZ4
        स्थिती COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                जर (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        जर (r < 0)
                                परत r;

                        स_नकल(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                माप_प्रकार bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                जर (r < 0)
                        परत r;

                माप_प्रकार n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, रिक्त);
                जर (sym_LZ4F_isError(n))
                        परत -EIO;

                *buffer_size += n;
                थांबा;
        }
#endif

#जर HAVE_ZSTD
        स्थिती COMPRESSION_ZSTD: {
                ZSTD_inBuffer आदान = {
                        .src = data,
                        .size = size,
                };

                पर्यंत (आदान.pos < आदान.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        जर (r < 0)
                                परत r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        माप_प्रकार res = sym_ZSTD_compressStream2(c->c_zstd, &output, &आदान, ZSTD_e_continue);
                        जर (sym_ZSTD_isError(res))
                                परत -EIO;

                        *buffer_size += output.pos;
                }

                थांबा;
        }
#endif

#जर HAVE_ZLIB
        स्थिती COMPRESSION_GZIP:
                जर (size > अच_पूर्णांक_उच्च)
                        परत -EFBIG;

                c->gzip.next_in = (शून्य*) data;
                c->gzip.avail_in = size;

                पर्यंत (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        जर (r < 0)
                                परत r;

                        माप_प्रकार avail = MIN(*buffer_allocated - *buffer_size, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        जर (r != Z_OK)
                                परत -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                थांबा;
#endif

#जर HAVE_BZIP2
        स्थिती COMPRESSION_BZIP2:
                जर (size > अच_पूर्णांक_उच्च)
                        परत -EFBIG;

                c->bzip2.next_in = (शून्य*) data;
                c->bzip2.avail_in = size;

                पर्यंत (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        जर (r < 0)
                                परत r;

                        माप_प्रकार avail = MIN(*buffer_allocated - *buffer_size, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->bzip2.next_out = (शून्य*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        जर (r != BZ_RUN_OK)
                                परत -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                थांबा;
#endif

        स्थिती COMPRESSION_NONE:

                जर (*buffer_allocated < size) {
                        शून्य *p;

                        p = पुनः_स्मृति(*buffer, size);
                        जर (!p)
                                परत -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                स_नकल(*buffer, data, size);
                *buffer_size = size;
                थांबा;

        पूर्वनिर्धारित:
                परत -EOPNOTSUPP;
        }

        परत 0;
}

पूर्णांक compressor_finish(Compressor *c, शून्य **buffer, माप_प्रकार *buffer_size, माप_प्रकार *buffer_allocated) {
#जर HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णांक r;
#endif

        प्रतिज्ञा(c);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        जर (!c->encoding)
                परत -EINVAL;

        *buffer_size = 0;

        निवड (c->type) {

#जर HAVE_XZ
        स्थिती COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                करा {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        जर (r < 0)
                                परत r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        जर (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                परत -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } पर्यंत (lzr != LZMA_STREAM_END);

                थांबा;
        }
#endif

#जर HAVE_LZ4
        स्थिती COMPRESSION_LZ4: {
                माप_प्रकार bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                जर (r < 0)
                        परत r;

                माप_प्रकार n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, रिक्त);
                जर (sym_LZ4F_isError(n))
                        परत -EIO;

                *buffer_size = n;
                थांबा;
        }
#endif

#जर HAVE_ZSTD
        स्थिती COMPRESSION_ZSTD: {
                ZSTD_inBuffer आदान = {};
                माप_प्रकार res;

                करा {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        जर (r < 0)
                                परत r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &आदान, ZSTD_e_end);
                        जर (sym_ZSTD_isError(res))
                                परत -EIO;

                        *buffer_size += output.pos;
                } पर्यंत (res != 0);

                थांबा;
        }
#endif

#जर HAVE_ZLIB
        स्थिती COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                करा {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        जर (r < 0)
                                परत r;

                        माप_प्रकार avail = MIN(*buffer_allocated - *buffer_size, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        जर (!IN_SET(r, Z_OK, Z_STREAM_END))
                                परत -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } पर्यंत (r != Z_STREAM_END);

                थांबा;
#endif

#जर HAVE_BZIP2
        स्थिती COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                करा {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        जर (r < 0)
                                परत r;

                        माप_प्रकार avail = MIN(*buffer_allocated - *buffer_size, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->bzip2.next_out = (शून्य*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        जर (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                परत -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } पर्यंत (r != BZ_STREAM_END);

                थांबा;
#endif

        स्थिती COMPRESSION_NONE:
                थांबा;

        पूर्वनिर्धारित:
                परत -EOPNOTSUPP;
        }

        परत 0;
}
