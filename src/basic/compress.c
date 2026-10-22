/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#सचे HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#अन्तर्भाव <lzma.h>
#endif

#सचे HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#अन्तर्भाव <lz4.h>
#अन्तर्भाव <lz4frame.h>
#अन्तर्भाव <lz4hc.h>
#endif

#सचे HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#अन्तर्भाव <zstd.h>
#अन्तर्भाव <zstd_errors.h>
#endif

#सचे HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#अन्तर्भाव <zlib.h>
#endif

#सचे HAVE_BZIP2
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

#सचे HAVE_XZ
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

#सचे HAVE_LZ4
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

स्थिर ध्रुव LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#सचे HAVE_ZSTD
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

स्थिर पूर्णाङ्क zstd_ret_to_errno(size_t ret) {
        विकल्प (sym_ZSTD_getErrorCode(ret)) {
        पक्ष ZSTD_error_dstSize_tooSmall:
                पटिनिवत्त -ENOBUFS;
        पक्ष ZSTD_error_memory_allocation:
                पटिनिवत्त -ENOMEM;
        स्वभाव:
                पटिनिवत्त -EBADMSG;
        }
}
#endif

#सचे HAVE_ZLIB
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

#सचे HAVE_BZIP2
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
#सचे HAVE_XZ
                lzma_stream xz;
#endif
#सचे HAVE_LZ4
                संरचना {
                        LZ4F_compressionContext_t c_lz4;
                        शून्य *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#सचे HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#सचे HAVE_ZLIB
                z_stream gzip;
#endif
#सचे HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, परिमाण(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

स्थिर ध्रुव वर्ण* ध्रुव compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

स्थिर ध्रुव वर्ण* ध्रुव compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

स्थिर ध्रुव वर्ण* ध्रुव compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(ध्रुव वर्ण *s) {
        Compression c;

        प्रतिज्ञा(s);

        c = compression_from_string(s);
        सचे (c >= 0)
                पटिनिवत्त c;

        पटिनिवत्त compression_uppercase_from_string(s);
}

Compression compression_from_filename(ध्रुव वर्ण *filename) {
        Compression c;
        ध्रुव वर्ण *e;

        प्रतिज्ञा(filename);

        e = strrchr(filename, '.');
        सचे (!e)
                पटिनिवत्त COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        सचे (c < 0)
                पटिनिवत्त COMPRESSION_NONE;

        पटिनिवत्त c;
}

bool compression_supported_journal(Compression c) {
        स्थिर ध्रुव unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        प्रतिज्ञा(c >= 0);
        प्रतिज्ञा(c < _COMPRESSION_MAX);

        पटिनिवत्त BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        स्थिर ध्रुव unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        प्रतिज्ञा(c >= 0);
        प्रतिज्ञा(c < _COMPRESSION_MAX);

        पटिनिवत्त BIT_SET(supported, c);
}

Compression compression_detect_from_magic(ध्रुव uint8_t data[स्थिर COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        सचे (memcmp(data, (ध्रुव uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                पटिनिवत्त COMPRESSION_GZIP;
        सचे (memcmp(data, (ध्रुव uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                पटिनिवत्त COMPRESSION_XZ;
        सचे (memcmp(data, (ध्रुव uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                पटिनिवत्त COMPRESSION_ZSTD;
        सचे (memcmp(data, (ध्रुव uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                पटिनिवत्त COMPRESSION_LZ4;
        सचे (memcmp(data, (ध्रुव uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                पटिनिवत्त COMPRESSION_BZIP2;

        पटिनिवत्त _COMPRESSION_INVALID;
}

पूर्णाङ्क dlopen_xz(पूर्णाङ्क log_level) {
#सचे HAVE_XZ
        स्थिर शून्य *lzma_dl = रिक्त;

        LIBLZMA_NOTE(suggested);

        पटिनिवत्त dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#अञ्ञथा
        पटिनिवत्त log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_lz4(पूर्णाङ्क log_level) {
#सचे HAVE_LZ4
        स्थिर शून्य *lz4_dl = रिक्त;

        LIBLZ4_NOTE(suggested);

        पटिनिवत्त dlopen_many_sym_or_warn(
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
#अञ्ञथा
        पटिनिवत्त log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_zstd(पूर्णाङ्क log_level) {
#सचे HAVE_ZSTD
        स्थिर शून्य *zstd_dl = रिक्त;

        LIBZSTD_NOTE(suggested);

        पटिनिवत्त dlopen_many_sym_or_warn(
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
#अञ्ञथा
        पटिनिवत्त log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_zlib(पूर्णाङ्क log_level) {
#सचे HAVE_ZLIB
        स्थिर शून्य *zlib_dl = रिक्त;

        LIBZ_NOTE(suggested);

        पटिनिवत्त dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#अञ्ञथा
        पटिनिवत्त log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_bzip2(पूर्णाङ्क log_level) {
#सचे HAVE_BZIP2
        स्थिर शून्य *bzip2_dl = रिक्त;

        LIBBZ2_NOTE(suggested);

        पटिनिवत्त dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#अञ्ञथा
        पटिनिवत्त log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

पूर्णाङ्क dlopen_compress_journal(Compression c, पूर्णाङ्क log_level) {
        विकल्प (c) {
        पक्ष COMPRESSION_NONE:
                पटिनिवत्त 0;
        पक्ष COMPRESSION_XZ:
                पटिनिवत्त dlopen_xz(log_level);
        पक्ष COMPRESSION_LZ4:
                पटिनिवत्त dlopen_lz4(log_level);
        पक्ष COMPRESSION_ZSTD:
                पटिनिवत्त dlopen_zstd(log_level);
        स्वभाव:
                पटिनिवत्त -EOPNOTSUPP;
        }
}

पूर्णाङ्क dlopen_compress(Compression c, पूर्णाङ्क log_level) {
        विकल्प (c) {
        पक्ष COMPRESSION_GZIP:
                पटिनिवत्त dlopen_zlib(log_level);
        पक्ष COMPRESSION_BZIP2:
                पटिनिवत्त dlopen_bzip2(log_level);
        स्वभाव:
                पटिनिवत्त dlopen_compress_journal(c, log_level);
        }
}

स्थिर पूर्णाङ्क compress_blob_xz(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#सचे HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, रिक्त, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, रिक्त }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                सचे (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        पटिनिवत्त -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        सचे (src_size < 80)
                पटिनिवत्त -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ रिक्त,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        सचे (ret != LZMA_OK)
                पटिनिवत्त -ENOBUFS;

        *dst_size = out_pos;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क compress_blob_lz4(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#सचे HAVE_LZ4
        पूर्णाङ्क r;

        r = dlopen_lz4(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        सचे (src_size < 9)
                पटिनिवत्त -ENOBUFS;

        सचे (src_size > INT_MAX)
                पटिनिवत्त -EFBIG;
        सचे (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        सचे (level <= 0)
                r = sym_LZ4_compress_default(src, (वर्ण*)dst + 8, src_size, (पूर्णाङ्क) dst_alloc_size - 8);
        अञ्ञथा
                r = sym_LZ4_compress_HC(src, (वर्ण*)dst + 8, src_size, (पूर्णाङ्क) dst_alloc_size - 8, level);
        सचे (r <= 0)
                पटिनिवत्त -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क compress_blob_zstd(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#सचे HAVE_ZSTD
        size_t k;
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        सचे (sym_ZSTD_isError(k))
                पटिनिवत्त zstd_ret_to_errno(k);

        *dst_size = k;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क compress_blob_gzip(ध्रुव शून्य *src, uint64_t src_size,
                       शून्य *dst, size_t dst_alloc_size, size_t *dst_size, पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#सचे HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size > UINT_MAX)
                पटिनिवत्त -EFBIG;
        सचे (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (पूर्णाङ्क) परिमाण(s));
        सचे (r != Z_OK)
                पटिनिवत्त -ENOMEM;

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        सचे (r != Z_STREAM_END)
                पटिनिवत्त -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क compress_blob_bzip2(
                ध्रुव शून्य *src, uint64_t src_size,
                शून्य *dst, size_t dst_alloc_size, size_t *dst_size, पूर्णाङ्क level) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_alloc_size > 0);
        प्रतिज्ञा(dst_size);

#सचे HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size > UINT_MAX)
                पटिनिवत्त -EFBIG;
        सचे (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        सचे (r != BZ_OK)
                पटिनिवत्त -ENOMEM;

        s.next_in = (वर्ण*) src;
        s.avail_in = src_size;
        s.next_out = (वर्ण*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        सचे (r != BZ_STREAM_END)
                पटिनिवत्त -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

पूर्णाङ्क compress_blob_journal(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_size,
                शून्य *dst, size_t dst_alloc_size, size_t *dst_size, पूर्णाङ्क level) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                पटिनिवत्त compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        पक्ष COMPRESSION_LZ4:
                पटिनिवत्त compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        पक्ष COMPRESSION_ZSTD:
                पटिनिवत्त compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        स्वभाव:
                पटिनिवत्त -EOPNOTSUPP;
        }
}

पूर्णाङ्क compress_blob(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_size,
                शून्य *dst, size_t dst_alloc_size, size_t *dst_size, पूर्णाङ्क level) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                पटिनिवत्त compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        पक्ष COMPRESSION_BZIP2:
                पटिनिवत्त compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        स्वभाव:
                पटिनिवत्त compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

स्थिर पूर्णाङ्क decompress_blob_xz(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                size_t *dst_size,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#सचे HAVE_XZ
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        सचे (ret != LZMA_OK)
                पटिनिवत्त -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        सचे (!greedy_realloc(dst, space, 1))
                पटिनिवत्त -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        अत्थाय (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                सचे (ret == LZMA_STREAM_END)
                        भङ्ग;
                सचे (ret != LZMA_OK)
                        पटिनिवत्त -ENOMEM;

                सचे (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        भङ्ग;
                सचे (dst_max > 0 && space == dst_max)
                        पटिनिवत्त -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                प्रतिज्ञा(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                सचे (!greedy_realloc(dst, space, 1))
                        पटिनिवत्त -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_blob_lz4(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                size_t *dst_size,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#सचे HAVE_LZ4
        वर्ण* out;
        पूर्णाङ्क r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size <= 8)
                पटिनिवत्त -EBADMSG;

        सचे (src_size - 8 > INT_MAX)
                पटिनिवत्त -EFBIG;

        size = unaligned_read_le64(src);
        सचे (size < 0 || (unsigned) size != unaligned_read_le64(src))
                पटिनिवत्त -EFBIG;
        सचे (dst_max > 0 && (size_t) size > dst_max)
                पटिनिवत्त -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        सचे (!out)
                पटिनिवत्त -ENOMEM;

        r = sym_LZ4_decompress_safe((वर्ण*)src + 8, out, src_size - 8, size);
        सचे (r < 0 || r != size)
                पटिनिवत्त -EBADMSG;

        *dst_size = size;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(शून्य) {
#सचे HAVE_ZSTD
        सचे (dlopen_zstd(LOG_DEBUG) < 0)
                पटिनिवत्त 0;

        पटिनिवत्त sym_ZSTD_DStreamOutSize();
#अञ्ञथा
        पटिनिवत्त 0;
#endif
}

स्थिर पूर्णाङ्क decompress_blob_zstd(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                size_t *dst_size,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#सचे HAVE_ZSTD
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        सचे (size == ZSTD_CONTENTSIZE_ERROR)
                पटिनिवत्त -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        सचे (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        अञ्ञथा {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        सचे (!greedy_realloc(dst, space, 1))
                पटिनिवत्त -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        सचे (!dctx)
                पटिनिवत्त -ENOMEM;

        ZSTD_inBuffer निवेशः = {
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

        अत्थाय (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &निवेशः);
                सचे (sym_ZSTD_isError(k))
                        पटिनिवत्त log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                सचे (output.pos >= cap)
                        भङ्ग;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                सचे (k == 0 && निवेशः.pos >= निवेशः.size)
                        भङ्ग;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                सचे (output.pos == output.size) {
                        सचे (space > SIZE_MAX / 2)
                                पटिनिवत्त -E2BIG;
                        space = MIN(2 * space, cap);
                        सचे (!greedy_realloc(dst, space, 1))
                                पटिनिवत्त -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        अनुवर्त;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                सचे (निवेशः.pos >= निवेशः.size)
                        पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_blob_gzip(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                size_t *dst_size,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#सचे HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size > UINT_MAX)
                पटिनिवत्त -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णाङ्क) परिमाण(s));
        सचे (r != Z_OK)
                पटिनिवत्त -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        सचे (!greedy_realloc(dst, space, 1))
                पटिनिवत्त -ENOMEM;

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        अत्थाय (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                सचे (r == Z_STREAM_END)
                        भङ्ग;
                सचे (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        पटिनिवत्त -EBADMSG;

                सचे (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        भङ्ग;
                सचे (dst_max > 0 && space == dst_max)
                        पटिनिवत्त -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                सचे (!greedy_realloc(dst, space, 1))
                        पटिनिवत्त -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_blob_bzip2(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                size_t *dst_size,
                size_t dst_max) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(dst);
        प्रतिज्ञा(dst_size);

#सचे HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size > UINT_MAX)
                पटिनिवत्त -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        सचे (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                पटिनिवत्त -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        सचे (!greedy_realloc(dst, space, 1))
                पटिनिवत्त -ENOMEM;

        s.next_in = (वर्ण*) src;
        s.avail_in = src_size;
        s.next_out = (वर्ण*) *dst;
        s.avail_out = space;

        अत्थाय (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                सचे (r == BZ_STREAM_END)
                        भङ्ग;
                सचे (r != BZ_OK)
                        पटिनिवत्त -EBADMSG;

                सचे (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        भङ्ग;
                सचे (dst_max > 0 && space == dst_max)
                        पटिनिवत्त -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                सचे (!greedy_realloc(dst, space, 1))
                        पटिनिवत्त -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (वर्ण*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

पूर्णाङ्क decompress_blob_journal(
                Compression compression,
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                size_t *dst_size,
                size_t dst_max) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                पटिनिवत्त decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        पक्ष COMPRESSION_LZ4:
                पटिनिवत्त decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        पक्ष COMPRESSION_ZSTD:
                पटिनिवत्त decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        स्वभाव:
                पटिनिवत्त -EPROTONOSUPPORT;
        }
}

पूर्णाङ्क decompress_blob(
                Compression compression,
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **dst,
                size_t *dst_size,
                size_t dst_max) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                पटिनिवत्त decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        पक्ष COMPRESSION_BZIP2:
                पटिनिवत्त decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        स्वभाव:
                पटिनिवत्त decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

पूर्णाङ्क decompress_zlib_raw(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य *dst,
                size_t dst_size,
                पूर्णाङ्क wbits) {

#सचे HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size > UINT_MAX)
                पटिनिवत्त -EFBIG;
        सचे (dst_size > UINT_MAX)
                पटिनिवत्त -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (शून्य*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (पूर्णाङ्क) परिमाण(s));
        सचे (r != Z_OK)
                पटिनिवत्त -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        सचे (r != Z_STREAM_END || produced != dst_size)
                पटिनिवत्त -EBADMSG;

        पटिनिवत्त 0;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_xz(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                ध्रुव शून्य *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#सचे HAVE_XZ
        पूर्णाङ्क r;

        r = dlopen_xz(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        सचे (ret != LZMA_OK)
                पटिनिवत्त -EBADMSG;

        सचे (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                पटिनिवत्त -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        अत्थाय (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                सचे (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        पटिनिवत्त -EBADMSG;

                सचे (allocated - s.avail_out >= prefix_len + 1)
                        पटिनिवत्त memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ध्रुव uint8_t*) *buffer)[prefix_len] == extra;

                सचे (ret == LZMA_STREAM_END)
                        पटिनिवत्त 0;

                s.avail_out += allocated;

                सचे (!(greedy_realloc(buffer, allocated * 2, 1)))
                        पटिनिवत्त -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_lz4(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                ध्रुव शून्य *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#सचे HAVE_LZ4
        size_t allocated;
        पूर्णाङ्क r;

        r = dlopen_lz4(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size <= 8)
                पटिनिवत्त -EBADMSG;

        सचे (src_size - 8 > INT_MAX)
                पटिनिवत्त -EFBIG;

        सचे (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                पटिनिवत्त -ENOMEM;
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
        सचे (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                सचे (sym_LZ4_version_सङ्ख्या() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        पटिनिवत्त 0;

                सचे (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        सचे (memcmp(*buffer, prefix, r) != 0)
                                पटिनिवत्त 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (size < prefix_len + 1)
                        पटिनिवत्त 0;
        }

        पटिनिवत्त memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ध्रुव uint8_t*) *buffer)[prefix_len] == extra;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_zstd(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                ध्रुव शून्य *prefix,
                size_t prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#सचे HAVE_ZSTD
        पूर्णाङ्क r;

        r = dlopen_zstd(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        सचे (size == ZSTD_CONTENTSIZE_ERROR)
                पटिनिवत्त -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        सचे (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                पटिनिवत्त 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        सचे (!dctx)
                पटिनिवत्त -ENOMEM;

        सचे (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                पटिनिवत्त -ENOMEM;

        ZSTD_inBuffer निवेशः = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &निवेशः);
        सचे (sym_ZSTD_isError(k))
                पटिनिवत्त log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        सचे (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                सचे (k != 0)
                        पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                पटिनिवत्त 0; /* Decompressed text too short to match the prefix and extra */
        }

        पटिनिवत्त memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ध्रुव uint8_t*) *buffer)[prefix_len] == extra;
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_gzip(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                ध्रुव शून्य *prefix,
                size_t prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#सचे HAVE_ZLIB
        पूर्णाङ्क r;

        r = dlopen_zlib(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size > UINT_MAX)
                पटिनिवत्त -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णाङ्क) परिमाण(s));
        सचे (r != Z_OK)
                पटिनिवत्त -EBADMSG;

        सचे (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                पटिनिवत्त -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (शून्य*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        अत्थाय (;;) {
                r = sym_inflate(&s, Z_FINISH);

                सचे (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        पटिनिवत्त -EBADMSG;

                सचे (allocated - s.avail_out >= prefix_len + 1)
                        पटिनिवत्त memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ध्रुव uint8_t*) *buffer)[prefix_len] == extra;

                सचे (r == Z_STREAM_END)
                        पटिनिवत्त 0;

                सचे (s.avail_out > 0)
                        पटिनिवत्त -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                सचे (!(greedy_realloc(buffer, allocated * 2, 1)))
                        पटिनिवत्त -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

स्थिर पूर्णाङ्क decompress_startswith_bzip2(
                ध्रुव शून्य *src,
                uint64_t src_size,
                शून्य **buffer,
                ध्रुव शून्य *prefix,
                size_t prefix_len,
                uint8_t extra) {

        प्रतिज्ञा(src);
        प्रतिज्ञा(src_size > 0);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(prefix);

#सचे HAVE_BZIP2
        पूर्णाङ्क r;

        r = dlopen_bzip2(LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (src_size > UINT_MAX)
                पटिनिवत्त -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        सचे (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                पटिनिवत्त -EIO;

        सचे (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                पटिनिवत्त -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (वर्ण*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        अत्थाय (;;) {
                r = sym_BZ2_bzDecompress(&s);

                सचे (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        पटिनिवत्त -EBADMSG;

                सचे (allocated - s.avail_out >= prefix_len + 1)
                        पटिनिवत्त memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ध्रुव uint8_t*) *buffer)[prefix_len] == extra;

                सचे (r == BZ_STREAM_END)
                        पटिनिवत्त 0;

                सचे (s.avail_out > 0)
                        पटिनिवत्त -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                सचे (!(greedy_realloc(buffer, allocated * 2, 1)))
                        पटिनिवत्त -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (वर्ण*) *buffer + used;
        }
#अञ्ञथा
        पटिनिवत्त -EPROTONOSUPPORT;
#endif
}

पूर्णाङ्क decompress_startswith_journal(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_size,
                शून्य **buffer,
                ध्रुव शून्य *prefix, size_t prefix_len,
                uint8_t extra) {

        विकल्प (compression) {
        पक्ष COMPRESSION_XZ:
                पटिनिवत्त decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        पक्ष COMPRESSION_LZ4:
                पटिनिवत्त decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        पक्ष COMPRESSION_ZSTD:
                पटिनिवत्त decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        स्वभाव:
                पटिनिवत्त -EOPNOTSUPP;
        }
}

पूर्णाङ्क decompress_startswith(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_size,
                शून्य **buffer,
                ध्रुव शून्य *prefix, size_t prefix_len,
                uint8_t extra) {

        विकल्प (compression) {
        पक्ष COMPRESSION_GZIP:
                पटिनिवत्त decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        पक्ष COMPRESSION_BZIP2:
                पटिनिवत्त decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        स्वभाव:
                पटिनिवत्त decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

पूर्णाङ्क compress_stream(
                Compression type,
                पूर्णाङ्क fdf, पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = रिक्त;
        _cleanup_free_ शून्य *buf = रिक्त;
        _cleanup_free_ uint8_t *निवेशः = रिक्त;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        पूर्णाङ्क r;

        प्रतिज्ञा(fdf >= 0);
        प्रतिज्ञा(fdt >= 0);

        r = compressor_new(&c, type);
        सचे (r < 0)
                पटिनिवत्त r;

        निवेशः = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        सचे (!निवेशः)
                पटिनिवत्त -ENOMEM;

        अत्थाय (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                सचे (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = पठ(fdf, निवेशः, m);
                सचे (n < 0)
                        पटिनिवत्त -errno;

                सचे (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        सचे (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                                total_out += buf_size;
                        }
                        भङ्ग;
                }

                total_in += n;
                सचे (max_bytes != UINT64_MAX) {
                        प्रतिज्ञा(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, निवेशः, n, &buf, &buf_size, &buf_alloc);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        सचे (r < 0)
                                पटिनिवत्त r;
                        total_out += buf_size;
                }
        }

        सचे (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        सचे (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        अञ्ञथा
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (द्विगुण) total_out / total_in * 100);

        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क should_sparse(पूर्णाङ्क fd) {
        संरचना stat st;

        प्रतिज्ञा(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        सचे (fstat(fd, &st) < 0)
                पटिनिवत्त -errno;

        सचे (!S_ISREG(st.st_mode))
                पटिनिवत्त false;

        पूर्णाङ्क flags = fcntl(fd, F_GETFL);
        सचे (flags < 0)
                पटिनिवत्त -errno;

        सचे (FLAGS_SET(flags, O_APPEND))
                पटिनिवत्त false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        सचे (pos < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
स्थिर पूर्णाङ्क finalize_sparse(पूर्णाङ्क fd) {
        off_t pos;

        प्रतिज्ञा(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        सचे (pos < 0)
                पटिनिवत्त -errno;

        सचे (ftruncate(fd, pos) < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त 0;
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

स्थिर पूर्णाङ्क decompress_stream_write_callback(ध्रुव शून्य *data, size_t size, शून्य *userdata) {
        संरचना decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        सचे (u->max_bytes != UINT64_MAX) {
                सचे (u->max_bytes < size)
                        पटिनिवत्त -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        सचे (u->skip > 0) {
                सचे (u->skip >= size) {
                        u->skip -= size;
                        पटिनिवत्त 0;
                }

                data = (ध्रुव uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        सचे (size == 0)
                पटिनिवत्त 0;

        सचे (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                सचे (k < 0)
                        पटिनिवत्त (पूर्णाङ्क) k;
                पटिनिवत्त 0;
        }

        पटिनिवत्त loop_write(u->fd, data, size);
}

स्थिर पूर्णाङ्क decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        प्रतिज्ञा(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        सचे (!c)
                पटिनिवत्त -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        विकल्प (type) {
        पक्ष COMPRESSION_NONE:
                भङ्ग;

#सचे HAVE_XZ
        पक्ष COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                सचे (xz_ret != LZMA_OK)
                        पटिनिवत्त -EIO;
                भङ्ग;
        }
#endif

#सचे HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                सचे (sym_LZ4F_isError(rc))
                        पटिनिवत्त -ENOMEM;

                भङ्ग;
        }
#endif

#सचे HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                सचे (!c->d_zstd)
                        पटिनिवत्त -ENOMEM;
                भङ्ग;
#endif

#सचे HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                सचे (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णाङ्क) परिमाण(c->gzip)) != Z_OK)
                        पटिनिवत्त -EIO;
                भङ्ग;
#endif

#सचे HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                सचे (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        पटिनिवत्त -EIO;
                भङ्ग;
#endif

        स्वभाव:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क decompress_blob_to_fd_impl(
                Compression type,
                ध्रुव शून्य *src,
                uint64_t src_size,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        पूर्णाङ्क r;

        प्रतिज्ञा(src);
        प्रतिज्ञा(fdt >= 0);

        सचे (type == COMPRESSION_NONE)
                पटिनिवत्त -EOPNOTSUPP;

        सचे (src_size == 0)
                पटिनिवत्त -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        सचे (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                सचे (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        सचे (src_size <= परिमाण(uint64_t))
                                पटिनिवत्त -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        सचे (sz < skip)
                                पटिनिवत्त -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                सचे (max_bytes == 0)
                        पटिनिवत्त -ENOBUFS;

                _cleanup_free_ शून्य *buf = रिक्त;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                सचे (r < 0)
                        पटिनिवत्त r;

                /* Check the result size again for safety. */
                सचे (size < skip)
                        पटिनिवत्त -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                सचे (r < 0)
                        पटिनिवत्त r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (द्विगुण) size / src_size * 100);

                सचे (ret_size)
                        *ret_size = size - skip;
                पटिनिवत्त 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = रिक्त;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        सचे (r < 0)
                पटिनिवत्त r;

        संरचना decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_प्रक्षिप(c, src, src_size, decompress_stream_write_callback, &userdata);
        सचे (r < 0)
                पटिनिवत्त r;

        /* Finalize */
        r = decompressor_प्रक्षिप(c, /* data= */ रिक्त, /* size= */ 0, decompress_stream_write_callback, &userdata);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (userdata.total_out < skip)
                पटिनिवत्त -EBADMSG;

        सचे (userdata.sparse) {
                r = finalize_sparse(fdt);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (द्विगुण) userdata.total_out / src_size * 100);

        सचे (ret_size)
                *ret_size = userdata.total_out - skip;

        पटिनिवत्त 0;
}

पूर्णाङ्क decompress_blob_to_fd_journal(
                Compression type,
                ध्रुव शून्य *src,
                uint64_t src_size,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        पूर्णाङ्क r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

पूर्णाङ्क decompress_blob_to_fd(
                Compression type,
                ध्रुव शून्य *src,
                uint64_t src_size,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        पूर्णाङ्क r;

        r = dlopen_compress(type, LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
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

        सचे (type == COMPRESSION_NONE)
                पटिनिवत्त -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        r = decompressor_new(&c, type, flags);
        सचे (r < 0)
                पटिनिवत्त r;

        संरचना decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        सचे (!buf)
                पटिनिवत्त -ENOMEM;

        अत्थाय (;;) {
                ssize_t n;

                n = पठ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                सचे (n < 0)
                        पटिनिवत्त -errno;

                total_in += n;

                r = decompressor_प्रक्षिप(c, buf, n, decompress_stream_write_callback, &userdata);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (n == 0)
                        भङ्ग;
        }

        सचे (total_in == 0)
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        सचे (userdata.sparse) {
                r = finalize_sparse(fdt);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (द्विगुण) userdata.total_out / total_in * 100);

        पटिनिवत्त 0;
}

पूर्णाङ्क decompress_stream_by_filename(ध्रुव वर्ण *filename, पूर्णाङ्क fdf, पूर्णाङ्क fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        सचे (c == COMPRESSION_NONE)
                पटिनिवत्त -EPROTONOSUPPORT;

        पटिनिवत्त decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        सचे (!c)
                पटिनिवत्त रिक्त;

        विकल्प (c->type) {

#सचे HAVE_XZ
        पक्ष COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                भङ्ग;
#endif

#सचे HAVE_LZ4
        पक्ष COMPRESSION_LZ4:
                सचे (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = रिक्त;
                        c->lz4_header = mfree(c->lz4_header);
                } अञ्ञथा {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = रिक्त;
                }
                भङ्ग;
#endif

#सचे HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                सचे (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = रिक्त;
                } अञ्ञथा {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = रिक्त;
                }
                भङ्ग;
#endif

#सचे HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                सचे (c->encoding)
                        sym_deflateEnd(&c->gzip);
                अञ्ञथा
                        sym_inflateEnd(&c->gzip);
                भङ्ग;
#endif

#सचे HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                सचे (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                अञ्ञथा
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                भङ्ग;
#endif

        स्वभाव:
                भङ्ग;
        }

        पटिनिवत्त mfree(c);
}

Compression compressor_type(ध्रुव Compressor *c) {
        पटिनिवत्त c ? c->type : _COMPRESSION_INVALID;
}

पूर्णाङ्क decompressor_detect(Decompressor **decompressor, ध्रुव शून्य *data, size_t size) {
        पूर्णाङ्क r;

        प्रतिज्ञा(decompressor);

        सचे (*decompressor)
                पटिनिवत्त 1;

        सचे (size < COMPRESSION_MAGIC_BYTES_MAX)
                पटिनिवत्त 0;

        प्रतिज्ञा(data);

        Compression type = compression_detect_from_magic(data);
        सचे (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त 1;
}

पूर्णाङ्क decompressor_force(Decompressor **decompressor, Compression type) {
        पूर्णाङ्क r;

        प्रतिज्ञा(decompressor);
        प्रतिज्ञा(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        सचे (r < 0)
                पटिनिवत्त r;

        *decompressor = compressor_free(*decompressor);
        पटिनिवत्त decompressor_new(decompressor, type, /* flags= */ 0);
}

पूर्णाङ्क decompressor_force_off(Decompressor **decompressor) {
        पटिनिवत्त decompressor_force(decompressor, COMPRESSION_NONE);
}

पूर्णाङ्क decompressor_प्रक्षिप(Decompressor *c, ध्रुव शून्य *data, size_t size, DecompressorCallback callback, शून्य *userdata) {
        पूर्णाङ्क r;

        प्रतिज्ञा(c);
        प्रतिज्ञा(data || size == 0);
        प्रतिज्ञा(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        सचे (c->encoding)
                पटिनिवत्त -EINVAL;

        सचे (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                पटिनिवत्त 0;

#सचे HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = रिक्त;
        सचे (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                सचे (!buffer)
                        पटिनिवत्त -ENOMEM;
        }
#endif

        विकल्प (c->type) {

        पक्ष COMPRESSION_NONE:
                सचे (c->finished)
                        पटिनिवत्त -EBADMSG;

                सचे (size == 0) {
                        c->finished = true;
                        पटिनिवत्त 0;
                }

                r = callback(data, size, userdata);
                सचे (r < 0)
                        पटिनिवत्त r;

                पटिनिवत्त 0;

#सचे HAVE_XZ
        पक्ष COMPRESSION_XZ:
                सचे (c->finished)
                        पटिनिवत्त -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                अत्थाय (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        सचे (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                पटिनिवत्त -EBADMSG;

                        सचे (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                पटिनिवत्त -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        सचे (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                        }

                        सचे (lzr == LZMA_STREAM_END) {
                                सचे (c->xz.avail_in > 0)
                                        पटिनिवत्त -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                पटिनिवत्त 0;
                        }

                        सचे (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                सचे (size == 0)
                                        पटिनिवत्त -EBADMSG; /* truncated */

                                पटिनिवत्त 0; /* No progress possible with the current input. */
                        }
                }
#endif

#सचे HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                ध्रुव uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                अत्थाय (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, रिक्त);
                        सचे (sym_LZ4F_isError(rc))
                                पटिनिवत्त -EBADMSG;

                        सचे (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                पटिनिवत्त -EIO;

                        सचे (consumed > 0) {
                                प्रतिज्ञा(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        सचे (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                        }

                        सचे (src_remaining == 0) {
                                सचे (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        पटिनिवत्त 0;
                                }

                                सचे (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        अनुवर्त;

                                सचे (size == 0)
                                        पटिनिवत्त -EBADMSG; /* truncated */

                                पटिनिवत्त 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#सचे HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_inBuffer निवेशः = {
                        .src =  (शून्य*) data,
                        .size = size,
                };

                c->finished = false;

                अत्थाय (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &निवेशः);
                        सचे (sym_ZSTD_isError(res))
                                पटिनिवत्त -EBADMSG;

                        सचे (निवेशः.pos > निवेशः.size ||
                            output.pos > output.size)
                                पटिनिवत्त -EIO;

                        सचे (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                        }

                        सचे (निवेशः.pos == निवेशः.size) {
                                सचे (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        पटिनिवत्त 0;
                                }

                                सचे (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        अनुवर्त;

                                सचे (size == 0)
                                        पटिनिवत्त -EBADMSG; /* truncated */

                                पटिनिवत्त 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#सचे HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                सचे (size > UINT_MAX)
                        पटिनिवत्त -EFBIG;

                c->gzip.next_in = (शून्य*) data;
                c->gzip.avail_in = size;

                अत्थाय (;;) {
                        सचे (c->finished) {
                                सचे (sym_inflateReset(&c->gzip) != Z_OK)
                                        पटिनिवत्त -EIO;

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
                        सचे (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                पटिनिवत्त -EBADMSG;

                        सचे (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                पटिनिवत्त -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        सचे (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                        }

                        सचे (zr == Z_STREAM_END) {
                                c->finished = true;

                                सचे (c->gzip.avail_in > 0)
                                        अनुवर्त; /* New stream may be pending. */

                                पटिनिवत्त 0;
                        }

                        सचे (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                सचे (size == 0)
                                        पटिनिवत्त -EBADMSG; /* truncated */

                                पटिनिवत्त 0; /* No progress possible with the current input. */
                        }
                }
#endif

#सचे HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                सचे (size > UINT_MAX)
                        पटिनिवत्त -EFBIG;

                c->bzip2.next_in = (वर्ण*) data;
                c->bzip2.avail_in = size;

                अत्थाय (;;) {
                        सचे (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                सचे (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        पटिनिवत्त -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (वर्ण*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        पूर्णाङ्क bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        सचे (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                पटिनिवत्त -EBADMSG;

                        सचे (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                पटिनिवत्त -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        सचे (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                        }

                        सचे (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                सचे (c->bzip2.avail_in > 0)
                                        अनुवर्त; /* New stream may be pending. */

                                पटिनिवत्त 0;
                        }

                        सचे (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                सचे (size == 0)
                                        पटिनिवत्त -EBADMSG; /* truncated */

                                पटिनिवत्त 0; /* No progress possible with the current input. */
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
        सचे (r < 0)
                पटिनिवत्त r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        सचे (!c)
                पटिनिवत्त -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        विकल्प (type) {

#सचे HAVE_XZ
        पक्ष COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                सचे (xzr != LZMA_OK)
                        पटिनिवत्त -EIO;

                c->type = COMPRESSION_XZ;
                भङ्ग;
        }
#endif

#सचे HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                सचे (sym_LZ4F_isError(rc))
                        पटिनिवत्त -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                सचे (!c->lz4_header)
                        पटिनिवत्त -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                सचे (sym_LZ4F_isError(c->lz4_header_size))
                        पटिनिवत्त -EINVAL;

                भङ्ग;
        }
#endif

#सचे HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                सचे (!c->c_zstd)
                        पटिनिवत्त -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                सचे (sym_ZSTD_isError(z))
                        पटिनिवत्त -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                सचे (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                भङ्ग;
#endif

#सचे HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (पूर्णाङ्क) परिमाण(c->gzip));
                सचे (r != Z_OK)
                        पटिनिवत्त -EIO;

                c->type = COMPRESSION_GZIP;
                भङ्ग;
#endif

#सचे HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                सचे (r != BZ_OK)
                        पटिनिवत्त -EIO;

                c->type = COMPRESSION_BZIP2;
                भङ्ग;
#endif

        पक्ष COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                भङ्ग;

        स्वभाव:
                पटिनिवत्त -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        पटिनिवत्त 0;
}

#सचे HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
स्थिर पूर्णाङ्क enlarge_buffer(शून्य **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        सचे (*buffer_allocated >= need)
                पटिनिवत्त 0;

        सचे (!greedy_realloc(buffer, need, 1))
                पटिनिवत्त -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        पटिनिवत्त 1;
}
#endif

पूर्णाङ्क compressor_start(
                Compressor *c,
                ध्रुव शून्य *data,
                size_t size,
                शून्य **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#सचे HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णाङ्क r;
#endif

        प्रतिज्ञा(c);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        सचे (!c->encoding)
                पटिनिवत्त -EINVAL;

        सचे (size == 0)
                पटिनिवत्त 0;

        प्रतिज्ञा(data);

        *buffer_size = 0;

        विकल्प (c->type) {

#सचे HAVE_XZ
        पक्ष COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                याव (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        सचे (lzr != LZMA_OK)
                                पटिनिवत्त -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                भङ्ग;
#endif

#सचे HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                सचे (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                सचे (r < 0)
                        पटिनिवत्त r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, रिक्त);
                सचे (sym_LZ4F_isError(n))
                        पटिनिवत्त -EIO;

                *buffer_size += n;
                भङ्ग;
        }
#endif

#सचे HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_inBuffer निवेशः = {
                        .src = data,
                        .size = size,
                };

                याव (निवेशः.pos < निवेशः.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &निवेशः, ZSTD_e_continue);
                        सचे (sym_ZSTD_isError(res))
                                पटिनिवत्त -EIO;

                        *buffer_size += output.pos;
                }

                भङ्ग;
        }
#endif

#सचे HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                सचे (size > UINT_MAX)
                        पटिनिवत्त -EFBIG;

                c->gzip.next_in = (शून्य*) data;
                c->gzip.avail_in = size;

                याव (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        सचे (r != Z_OK)
                                पटिनिवत्त -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                भङ्ग;
#endif

#सचे HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                सचे (size > UINT_MAX)
                        पटिनिवत्त -EFBIG;

                c->bzip2.next_in = (शून्य*) data;
                c->bzip2.avail_in = size;

                याव (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (शून्य*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        सचे (r != BZ_RUN_OK)
                                पटिनिवत्त -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                भङ्ग;
#endif

        पक्ष COMPRESSION_NONE:

                सचे (*buffer_allocated < size) {
                        शून्य *p;

                        p = realloc(*buffer, size);
                        सचे (!p)
                                पटिनिवत्त -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                भङ्ग;

        स्वभाव:
                पटिनिवत्त -EOPNOTSUPP;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क compressor_finish(Compressor *c, शून्य **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#सचे HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णाङ्क r;
#endif

        प्रतिज्ञा(c);
        प्रतिज्ञा(buffer);
        प्रतिज्ञा(buffer_size);
        प्रतिज्ञा(buffer_allocated);

        सचे (!c->encoding)
                पटिनिवत्त -EINVAL;

        *buffer_size = 0;

        विकल्प (c->type) {

#सचे HAVE_XZ
        पक्ष COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                करोहि {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        सचे (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                पटिनिवत्त -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } याव (lzr != LZMA_STREAM_END);

                भङ्ग;
        }
#endif

#सचे HAVE_LZ4
        पक्ष COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                सचे (r < 0)
                        पटिनिवत्त r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, रिक्त);
                सचे (sym_LZ4F_isError(n))
                        पटिनिवत्त -EIO;

                *buffer_size = n;
                भङ्ग;
        }
#endif

#सचे HAVE_ZSTD
        पक्ष COMPRESSION_ZSTD: {
                ZSTD_inBuffer निवेशः = {};
                size_t res;

                करोहि {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &निवेशः, ZSTD_e_end);
                        सचे (sym_ZSTD_isError(res))
                                पटिनिवत्त -EIO;

                        *buffer_size += output.pos;
                } याव (res != 0);

                भङ्ग;
        }
#endif

#सचे HAVE_ZLIB
        पक्ष COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                करोहि {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        सचे (!IN_SET(r, Z_OK, Z_STREAM_END))
                                पटिनिवत्त -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } याव (r != Z_STREAM_END);

                भङ्ग;
#endif

#सचे HAVE_BZIP2
        पक्ष COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                करोहि {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (शून्य*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        सचे (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                पटिनिवत्त -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } याव (r != BZ_STREAM_END);

                भङ्ग;
#endif

        पक्ष COMPRESSION_NONE:
                भङ्ग;

        स्वभाव:
                पटिनिवत्त -EOPNOTSUPP;
        }

        पटिनिवत्त 0;
}
