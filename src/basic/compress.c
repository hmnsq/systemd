/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <stdio.h>
#समावेश <sys/stat.h>
#समावेश <unistd.h>

#अगर HAVE_XZ
#अगर_अघोषित SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#पूर्ण_अगर
#समावेश <lzma.h>
#पूर्ण_अगर

#अगर HAVE_LZ4
#अगर_अघोषित SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#पूर्ण_अगर
#समावेश <lz4.h>
#समावेश <lz4frame.h>
#समावेश <lz4hc.h>
#पूर्ण_अगर

#अगर HAVE_ZSTD
#अगर_अघोषित SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#पूर्ण_अगर
#समावेश <zstd.h>
#समावेश <zstd_errors.h>
#पूर्ण_अगर

#अगर HAVE_ZLIB
#अगर_अघोषित SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#पूर्ण_अगर
#समावेश <zlib.h>
#पूर्ण_अगर

#अगर HAVE_BZIP2
#अगर_अघोषित SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#पूर्ण_अगर
#समावेश <bzlib.h>
#पूर्ण_अगर

#समावेश "alloc-util.h"
#समावेश "bitfield.h"
#समावेश "compress.h"
#समावेश "dlfcn-util.h"
#समावेश "io-util.h"
#समावेश "log.h"
#समावेश "string-table.h"
#समावेश "unaligned.h"

#अगर HAVE_XZ
अटल DLSYM_PROTOTYPE(lzma_auto_decoder) = शून्य;
अटल DLSYM_PROTOTYPE(lzma_code) = शून्य;
अटल DLSYM_PROTOTYPE(lzma_easy_encoder) = शून्य;
अटल DLSYM_PROTOTYPE(lzma_end) = शून्य;
अटल DLSYM_PROTOTYPE(lzma_stream_बफर_encode) = शून्य;
अटल DLSYM_PROTOTYPE(lzma_stream_decoder) = शून्य;
अटल DLSYM_PROTOTYPE(lzma_lzma_preset) = शून्य;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
अटल अंतरभूत व्योम lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#पूर्ण_अगर

#अगर HAVE_LZ4
अटल DLSYM_PROTOTYPE(LZ4F_compressBegin) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_compressBound) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_compressEnd) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_compressUpdate) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_decompress) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4F_isError) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4_compress_HC) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4_compress_default) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4_decompress_safe) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = शून्य;
अटल DLSYM_PROTOTYPE(LZ4_version_संख्या) = शून्य;

अटल स्थिर LZ4F_preferences_t lz4_preferences = {
        .frameInfo.block_आकार_ID = 5,
};
#पूर्ण_अगर

#अगर HAVE_ZSTD
अटल DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_compress) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_compressStream2) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_createCCtx) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_createDCtx) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_CStream_In_आकार) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_CStream_Out_आकार) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_decompressStream) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_DStream_In_आकार) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_DStream_Out_आकार) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_freeCCtx) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_freeDCtx) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_getErrorCode) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_getErrorName) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_get_Frame_Content_आकार) = शून्य;
अटल DLSYM_PROTOTYPE(ZSTD_isError) = शून्य;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, शून्य);

अटल पूर्णांक zstd_ret_to_errno(माप_प्रकार ret) {
        चयन (sym_ZSTD_getErrorCode(ret)) {
        हाल ZSTD_error_dst_आकार_too_Small:
                वापस -ENOBUFS;
        हाल ZSTD_error_memory_allocation:
                वापस -ENOMEM;
        शेष:
                वापस -EBADMSG;
        }
}
#पूर्ण_अगर

#अगर HAVE_ZLIB
अटल DLSYM_PROTOTYPE(deflateInit2_) = शून्य;
अटल DLSYM_PROTOTYPE(deflate) = शून्य;
अटल DLSYM_PROTOTYPE(deflateEnd) = शून्य;
अटल DLSYM_PROTOTYPE(inflateInit2_) = शून्य;
अटल DLSYM_PROTOTYPE(inflate) = शून्य;
अटल DLSYM_PROTOTYPE(inflateEnd) = शून्य;
अटल DLSYM_PROTOTYPE(inflateReset) = शून्य;

अटल अंतरभूत व्योम deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

अटल अंतरभूत व्योम inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#पूर्ण_अगर

#अगर HAVE_BZIP2
अटल DLSYM_PROTOTYPE(BZ2_bzCompressInit) = शून्य;
अटल DLSYM_PROTOTYPE(BZ2_bzCompress) = शून्य;
अटल DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = शून्य;
अटल DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = शून्य;
अटल DLSYM_PROTOTYPE(BZ2_bzDecompress) = शून्य;
अटल DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = शून्य;

अटल अंतरभूत व्योम BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

अटल अंतरभूत व्योम BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#पूर्ण_अगर

/* Opaque Compressor/Decompressor struct definition */
काष्ठा Compressor {
        Compression type;
        bool encoding;
        bool finished;
        जोड़ {
#अगर HAVE_XZ
                lzma_stream xz;
#पूर्ण_अगर
#अगर HAVE_LZ4
                काष्ठा {
                        LZ4F_compressionContext_t c_lz4;
                        व्योम *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        माप_प्रकार lz4_header_आकार;
                };
                LZ4F_decompressionContext_t d_lz4;
#पूर्ण_अगर
#अगर HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#पूर्ण_अगर
#अगर HAVE_ZLIB
                z_stream gzip;
#पूर्ण_अगर
#अगर HAVE_BZIP2
                bz_stream bzip2;
#पूर्ण_अगर
        };
};

#घोषणा ALIGN_8(l) ALIGN_TO(l, माप(माप_प्रकार))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#घोषणा ZLIB_WBITS_GZIP (15 + 16)

अटल स्थिर अक्षर* स्थिर compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

अटल स्थिर अक्षर* स्थिर compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

अटल स्थिर अक्षर* स्थिर compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(स्थिर अक्षर *s) {
        Compression c;

        निश्चित(s);

        c = compression_from_string(s);
        अगर (c >= 0)
                वापस c;

        वापस compression_uppercase_from_string(s);
}

Compression compression_from_filename(स्थिर अक्षर *filename) {
        Compression c;
        स्थिर अक्षर *e;

        निश्चित(filename);

        e = म_खोजप(filename, '.');
        अगर (!e)
                वापस COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        अगर (c < 0)
                वापस COMPRESSION_NONE;

        वापस c;
}

bool compression_supported_journal(Compression c) {
        अटल स्थिर अचिन्हित supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        निश्चित(c >= 0);
        निश्चित(c < _COMPRESSION_MAX);

        वापस BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        अटल स्थिर अचिन्हित supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        निश्चित(c >= 0);
        निश्चित(c < _COMPRESSION_MAX);

        वापस BIT_SET(supported, c);
}

Compression compression_detect_from_magic(स्थिर uint8_t तथ्य[अटल COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        अगर (स_भेद(तथ्य, (स्थिर uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                वापस COMPRESSION_GZIP;
        अगर (स_भेद(तथ्य, (स्थिर uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                वापस COMPRESSION_XZ;
        अगर (स_भेद(तथ्य, (स्थिर uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                वापस COMPRESSION_ZSTD;
        अगर (स_भेद(तथ्य, (स्थिर uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                वापस COMPRESSION_LZ4;
        अगर (स_भेद(तथ्य, (स्थिर uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                वापस COMPRESSION_BZIP2;

        वापस _COMPRESSION_INVALID;
}

पूर्णांक dlopen_xz(पूर्णांक log_level) {
#अगर HAVE_XZ
        अटल व्योम *lzma_dl = शून्य;

        LIBLZMA_NOTE(suggested);

        वापस dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_बफर_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#अन्यथा
        वापस log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#पूर्ण_अगर
}

पूर्णांक dlopen_lz4(पूर्णांक log_level) {
#अगर HAVE_LZ4
        अटल व्योम *lz4_dl = शून्य;

        LIBLZ4_NOTE(suggested);

        वापस dlopen_many_sym_or_warn(
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
        वापस log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#पूर्ण_अगर
}

पूर्णांक dlopen_zstd(पूर्णांक log_level) {
#अगर HAVE_ZSTD
        अटल व्योम *zstd_dl = शून्य;

        LIBZSTD_NOTE(suggested);

        वापस dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_getErrorCode),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_Frame_Content_आकार),
                        DLSYM_ARG(ZSTD_decompressStream),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_Out_आकार),
                        DLSYM_ARG(ZSTD_CStream_In_आकार),
                        DLSYM_ARG(ZSTD_CStream_Out_आकार),
                        DLSYM_ARG(ZSTD_CCtx_setParameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_In_आकार),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#अन्यथा
        वापस log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#पूर्ण_अगर
}

पूर्णांक dlopen_zlib(पूर्णांक log_level) {
#अगर HAVE_ZLIB
        अटल व्योम *zlib_dl = शून्य;

        LIBZ_NOTE(suggested);

        वापस dlopen_many_sym_or_warn(
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
        वापस log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#पूर्ण_अगर
}

पूर्णांक dlopen_bzip2(पूर्णांक log_level) {
#अगर HAVE_BZIP2
        अटल व्योम *bzip2_dl = शून्य;

        LIBBZ2_NOTE(suggested);

        वापस dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#अन्यथा
        वापस log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#पूर्ण_अगर
}

पूर्णांक dlopen_compress_journal(Compression c, पूर्णांक log_level) {
        चयन (c) {
        हाल COMPRESSION_NONE:
                वापस 0;
        हाल COMPRESSION_XZ:
                वापस dlopen_xz(log_level);
        हाल COMPRESSION_LZ4:
                वापस dlopen_lz4(log_level);
        हाल COMPRESSION_ZSTD:
                वापस dlopen_zstd(log_level);
        शेष:
                वापस -EOPNOTSUPP;
        }
}

पूर्णांक dlopen_compress(Compression c, पूर्णांक log_level) {
        चयन (c) {
        हाल COMPRESSION_GZIP:
                वापस dlopen_zlib(log_level);
        हाल COMPRESSION_BZIP2:
                वापस dlopen_bzip2(log_level);
        शेष:
                वापस dlopen_compress_journal(c, log_level);
        }
}

अटल पूर्णांक compress_blob_xz(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम *dst,
                माप_प्रकार dst_alloc_आकार,
                माप_प्रकार *dst_आकार,
                पूर्णांक level) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_alloc_आकार > 0);
        निश्चित(dst_आकार);

#अगर HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, शून्य, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, शून्य }
        };
        lzma_ret ret;
        माप_प्रकार out_pos = 0;
        पूर्णांक r;

        r = dlopen_xz(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                अगर (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        वापस -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        अगर (src_आकार < 80)
                वापस -ENOBUFS;

        ret = sym_lzma_stream_बफर_encode(filters, LZMA_CHECK_NONE, /* allocator= */ शून्य,
                                            src, src_आकार, dst, &out_pos, dst_alloc_आकार);
        अगर (ret != LZMA_OK)
                वापस -ENOBUFS;

        *dst_आकार = out_pos;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक compress_blob_lz4(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम *dst,
                माप_प्रकार dst_alloc_आकार,
                माप_प्रकार *dst_आकार,
                पूर्णांक level) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_alloc_आकार > 0);
        निश्चित(dst_आकार);

#अगर HAVE_LZ4
        पूर्णांक r;

        r = dlopen_lz4(LOG_DEBUG);
        अगर (r < 0)
                वापस r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        अगर (src_आकार < 9)
                वापस -ENOBUFS;

        अगर (src_आकार > पूर्णांक_उच्च)
                वापस -EFBIG;
        अगर (dst_alloc_आकार > पूर्णांक_उच्च)
                dst_alloc_आकार = पूर्णांक_उच्च;

        अगर (level <= 0)
                r = sym_LZ4_compress_default(src, (अक्षर*)dst + 8, src_आकार, (पूर्णांक) dst_alloc_आकार - 8);
        अन्यथा
                r = sym_LZ4_compress_HC(src, (अक्षर*)dst + 8, src_आकार, (पूर्णांक) dst_alloc_आकार - 8, level);
        अगर (r <= 0)
                वापस -ENOBUFS;

        unaligned_write_le64(dst, src_आकार);
        *dst_आकार = r + 8;

        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक compress_blob_zstd(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम *dst,
                माप_प्रकार dst_alloc_आकार,
                माप_प्रकार *dst_आकार,
                पूर्णांक level) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_alloc_आकार > 0);
        निश्चित(dst_आकार);

#अगर HAVE_ZSTD
        माप_प्रकार k;
        पूर्णांक r;

        r = dlopen_zstd(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        k = sym_ZSTD_compress(dst, dst_alloc_आकार, src, src_आकार, level < 0 ? 0 : level);
        अगर (sym_ZSTD_isError(k))
                वापस zstd_ret_to_errno(k);

        *dst_आकार = k;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक compress_blob_gzip(स्थिर व्योम *src, uint64_t src_आकार,
                       व्योम *dst, माप_प्रकार dst_alloc_आकार, माप_प्रकार *dst_आकार, पूर्णांक level) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_alloc_आकार > 0);
        निश्चित(dst_आकार);

#अगर HAVE_ZLIB
        पूर्णांक r;

        r = dlopen_zlib(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार > अच_पूर्णांक_उच्च)
                वापस -EFBIG;
        अगर (dst_alloc_आकार > अच_पूर्णांक_उच्च)
                dst_alloc_आकार = अच_पूर्णांक_उच्च;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (पूर्णांक) माप(s));
        अगर (r != Z_OK)
                वापस -ENOMEM;

        s.अग्र_in = (व्योम*) src;
        s.avail_in = src_आकार;
        s.अग्र_out = dst;
        s.avail_out = dst_alloc_आकार;

        r = sym_deflate(&s, Z_FINISH);
        अगर (r != Z_STREAM_END)
                वापस -ENOBUFS;

        *dst_आकार = dst_alloc_आकार - s.avail_out;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक compress_blob_bzip2(
                स्थिर व्योम *src, uint64_t src_आकार,
                व्योम *dst, माप_प्रकार dst_alloc_आकार, माप_प्रकार *dst_आकार, पूर्णांक level) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_alloc_आकार > 0);
        निश्चित(dst_आकार);

#अगर HAVE_BZIP2
        पूर्णांक r;

        r = dlopen_bzip2(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार > अच_पूर्णांक_उच्च)
                वापस -EFBIG;
        अगर (dst_alloc_आकार > अच_पूर्णांक_उच्च)
                dst_alloc_आकार = अच_पूर्णांक_उच्च;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        अगर (r != BZ_OK)
                वापस -ENOMEM;

        s.अग्र_in = (अक्षर*) src;
        s.avail_in = src_आकार;
        s.अग्र_out = (अक्षर*) dst;
        s.avail_out = dst_alloc_आकार;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        अगर (r != BZ_STREAM_END)
                वापस -ENOBUFS;

        *dst_आकार = dst_alloc_आकार - s.avail_out;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

पूर्णांक compress_blob_journal(
                Compression compression,
                स्थिर व्योम *src, uint64_t src_आकार,
                व्योम *dst, माप_प्रकार dst_alloc_आकार, माप_प्रकार *dst_आकार, पूर्णांक level) {

        चयन (compression) {
        हाल COMPRESSION_XZ:
                वापस compress_blob_xz(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        हाल COMPRESSION_LZ4:
                वापस compress_blob_lz4(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        हाल COMPRESSION_ZSTD:
                वापस compress_blob_zstd(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        शेष:
                वापस -EOPNOTSUPP;
        }
}

पूर्णांक compress_blob(
                Compression compression,
                स्थिर व्योम *src, uint64_t src_आकार,
                व्योम *dst, माप_प्रकार dst_alloc_आकार, माप_प्रकार *dst_आकार, पूर्णांक level) {

        चयन (compression) {
        हाल COMPRESSION_GZIP:
                वापस compress_blob_gzip(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        हाल COMPRESSION_BZIP2:
                वापस compress_blob_bzip2(src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        शेष:
                वापस compress_blob_journal(compression, src, src_आकार, dst, dst_alloc_आकार, dst_आकार, level);
        }
}

अटल पूर्णांक decompress_blob_xz(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **dst,
                माप_प्रकार *dst_आकार,
                माप_प्रकार dst_max) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_आकार);

#अगर HAVE_XZ
        पूर्णांक r;

        r = dlopen_xz(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        अगर (ret != LZMA_OK)
                वापस -ENOMEM;

        माप_प्रकार space = MIN(src_आकार * 2, dst_max ?: SIZE_MAX);
        अगर (!greedy_realloc(dst, space, 1))
                वापस -ENOMEM;

        s.अग्र_in = src;
        s.avail_in = src_आकार;

        s.अग्र_out = *dst;
        s.avail_out = space;

        क्रम (;;) {
                माप_प्रकार used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                अगर (ret == LZMA_STREAM_END)
                        अवरोध;
                अगर (ret != LZMA_OK)
                        वापस -ENOMEM;

                अगर (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        अवरोध;
                अगर (dst_max > 0 && space == dst_max)
                        वापस -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                निश्चित(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                अगर (!greedy_realloc(dst, space, 1))
                        वापस -ENOMEM;

                s.avail_out = space - used;
                s.अग्र_out = *(uint8_t**)dst + used;
        }

        *dst_आकार = space - s.avail_out;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_blob_lz4(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **dst,
                माप_प्रकार *dst_आकार,
                माप_प्रकार dst_max) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_आकार);

#अगर HAVE_LZ4
        अक्षर* out;
        पूर्णांक r, आकार; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार <= 8)
                वापस -EBADMSG;

        अगर (src_आकार - 8 > पूर्णांक_उच्च)
                वापस -EFBIG;

        आकार = unaligned_read_le64(src);
        अगर (आकार < 0 || (अचिन्हित) आकार != unaligned_read_le64(src))
                वापस -EFBIG;
        अगर (dst_max > 0 && (माप_प्रकार) आकार > dst_max)
                वापस -ENOBUFS;
        out = greedy_realloc(dst, आकार, 1);
        अगर (!out)
                वापस -ENOMEM;

        r = sym_LZ4_decompress_safe((अक्षर*)src + 8, out, src_आकार - 8, आकार);
        अगर (r < 0 || r != आकार)
                वापस -EBADMSG;

        *dst_आकार = आकार;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

माप_प्रकार zstd_dstream_out_आकार(व्योम) {
#अगर HAVE_ZSTD
        अगर (dlopen_zstd(LOG_DEBUG) < 0)
                वापस 0;

        वापस sym_ZSTD_DStream_Out_आकार();
#अन्यथा
        वापस 0;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_blob_zstd(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **dst,
                माप_प्रकार *dst_आकार,
                माप_प्रकार dst_max) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_आकार);

#अगर HAVE_ZSTD
        पूर्णांक r;

        r = dlopen_zstd(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        अचिन्हित दीर्घ दीर्घ आकार = sym_ZSTD_get_Frame_Content_आकार(src, src_आकार);
        अगर (आकार == ZSTD_CONTENTSIZE_ERROR)
                वापस -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        माप_प्रकार cap = dst_max ?: SIZE_MAX;

        माप_प्रकार space;
        अगर (आकार != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_Out_आकार(), (माप_प्रकार) MIN(आकार, (अचिन्हित दीर्घ दीर्घ) cap));
        अन्यथा {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_आकार <= UINT64_MAX / 2 ? src_आकार * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_Out_आकार(), (माप_प्रकार) MIN(twice, (uint64_t) cap));
        }

        अगर (!greedy_realloc(dst, space, 1))
                वापस -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        अगर (!dctx)
                वापस -ENOMEM;

        ZSTD_in_बफर निवेश = {
                .src = src,
                .आकार = src_आकार,
        };
        ZSTD_out_बफर output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .आकार = MIN(space, cap),
        };

        क्रम (;;) {
                माप_प्रकार k = sym_ZSTD_decompressStream(dctx, &output, &निवेश);
                अगर (sym_ZSTD_isError(k))
                        वापस log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                अगर (output.pos >= cap)
                        अवरोध;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                अगर (k == 0 && निवेश.pos >= निवेश.आकार)
                        अवरोध;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                अगर (output.pos == output.आकार) {
                        अगर (space > SIZE_MAX / 2)
                                वापस -E2BIG;
                        space = MIN(2 * space, cap);
                        अगर (!greedy_realloc(dst, space, 1))
                                वापस -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.आकार = MIN(space, cap);
                        जारी;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                अगर (निवेश.pos >= निवेश.आकार)
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_आकार = output.pos;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_blob_gzip(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **dst,
                माप_प्रकार *dst_आकार,
                माप_प्रकार dst_max) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_आकार);

#अगर HAVE_ZLIB
        पूर्णांक r;

        r = dlopen_zlib(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार > अच_पूर्णांक_उच्च)
                वापस -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णांक) माप(s));
        अगर (r != Z_OK)
                वापस -ENOMEM;

        माप_प्रकार space = MIN3(src_आकार * 2, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
        अगर (!greedy_realloc(dst, space, 1))
                वापस -ENOMEM;

        s.अग्र_in = (व्योम*) src;
        s.avail_in = src_आकार;
        s.अग्र_out = *dst;
        s.avail_out = space;

        क्रम (;;) {
                माप_प्रकार used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                अगर (r == Z_STREAM_END)
                        अवरोध;
                अगर (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        वापस -EBADMSG;

                अगर (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        अवरोध;
                अगर (dst_max > 0 && space == dst_max)
                        वापस -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, अच_पूर्णांक_उच्च);
                अगर (!greedy_realloc(dst, space, 1))
                        वापस -ENOMEM;

                s.avail_out = space - used;
                s.अग्र_out = *(uint8_t**)dst + used;
        }

        *dst_आकार = space - s.avail_out;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_blob_bzip2(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **dst,
                माप_प्रकार *dst_आकार,
                माप_प्रकार dst_max) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(dst);
        निश्चित(dst_आकार);

#अगर HAVE_BZIP2
        पूर्णांक r;

        r = dlopen_bzip2(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार > अच_पूर्णांक_उच्च)
                वापस -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        अगर (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                वापस -EIO;

        माप_प्रकार space = MIN3(src_आकार * 2, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
        अगर (!greedy_realloc(dst, space, 1))
                वापस -ENOMEM;

        s.अग्र_in = (अक्षर*) src;
        s.avail_in = src_आकार;
        s.अग्र_out = (अक्षर*) *dst;
        s.avail_out = space;

        क्रम (;;) {
                माप_प्रकार used;

                r = sym_BZ2_bzDecompress(&s);
                अगर (r == BZ_STREAM_END)
                        अवरोध;
                अगर (r != BZ_OK)
                        वापस -EBADMSG;

                अगर (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        अवरोध;
                अगर (dst_max > 0 && space == dst_max)
                        वापस -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (माप_प्रकार) अच_पूर्णांक_उच्च);
                अगर (!greedy_realloc(dst, space, 1))
                        वापस -ENOMEM;

                s.avail_out = space - used;
                s.अग्र_out = (अक्षर*) *dst + used;
        }

        *dst_आकार = space - s.avail_out;
        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

पूर्णांक decompress_blob_journal(
                Compression compression,
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **dst,
                माप_प्रकार *dst_आकार,
                माप_प्रकार dst_max) {

        चयन (compression) {
        हाल COMPRESSION_XZ:
                वापस decompress_blob_xz(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        हाल COMPRESSION_LZ4:
                वापस decompress_blob_lz4(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        हाल COMPRESSION_ZSTD:
                वापस decompress_blob_zstd(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        शेष:
                वापस -EPROTONOSUPPORT;
        }
}

पूर्णांक decompress_blob(
                Compression compression,
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **dst,
                माप_प्रकार *dst_आकार,
                माप_प्रकार dst_max) {

        चयन (compression) {
        हाल COMPRESSION_GZIP:
                वापस decompress_blob_gzip(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        हाल COMPRESSION_BZIP2:
                वापस decompress_blob_bzip2(
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        शेष:
                वापस decompress_blob_journal(
                                compression,
                                src, src_आकार,
                                dst, dst_आकार, dst_max);
        }
}

पूर्णांक decompress_zlib_raw(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम *dst,
                माप_प्रकार dst_आकार,
                पूर्णांक wbits) {

#अगर HAVE_ZLIB
        पूर्णांक r;

        r = dlopen_zlib(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार > अच_पूर्णांक_उच्च)
                वापस -EFBIG;
        अगर (dst_आकार > अच_पूर्णांक_उच्च)
                वापस -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .अग्र_in = (व्योम*) src,
                .avail_in = src_आकार,
                .अग्र_out = dst,
                .avail_out = dst_आकार,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (पूर्णांक) माप(s));
        अगर (r != Z_OK)
                वापस -EIO;

        r = sym_inflate(&s, Z_FINISH);
        माप_प्रकार produced = (uint8_t*) s.अग्र_out - (uint8_t*) dst;

        अगर (r != Z_STREAM_END || produced != dst_आकार)
                वापस -EBADMSG;

        वापस 0;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_startswith_xz(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **बफर,
                स्थिर व्योम *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(बफर);
        निश्चित(prefix);

#अगर HAVE_XZ
        पूर्णांक r;

        r = dlopen_xz(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        अगर (ret != LZMA_OK)
                वापस -EBADMSG;

        अगर (!(greedy_realloc(बफर, ALIGN_8(prefix_len + 1), 1)))
                वापस -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*बफर);

        s.अग्र_in = src;
        s.avail_in = src_आकार;

        s.अग्र_out = *बफर;
        s.avail_out = allocated;

        क्रम (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                अगर (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        वापस -EBADMSG;

                अगर (allocated - s.avail_out >= prefix_len + 1)
                        वापस स_भेद(*बफर, prefix, prefix_len) == 0 &&
                                ((स्थिर uint8_t*) *बफर)[prefix_len] == extra;

                अगर (ret == LZMA_STREAM_END)
                        वापस 0;

                s.avail_out += allocated;

                अगर (!(greedy_realloc(बफर, allocated * 2, 1)))
                        वापस -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*बफर);
                s.अग्र_out = *(uint8_t**)बफर + allocated - s.avail_out;
        }

#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_startswith_lz4(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **बफर,
                स्थिर व्योम *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(बफर);
        निश्चित(prefix);

#अगर HAVE_LZ4
        माप_प्रकार allocated;
        पूर्णांक r;

        r = dlopen_lz4(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार <= 8)
                वापस -EBADMSG;

        अगर (src_आकार - 8 > पूर्णांक_उच्च)
                वापस -EFBIG;

        अगर (!(greedy_realloc(बफर, ALIGN_8(prefix_len + 1), 1)))
                वापस -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*बफर);

        r = sym_LZ4_decompress_safe_partial(
                        (अक्षर*)src + 8,
                        *बफर,
                        src_आकार - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        अगर (r < 0 || (माप_प्रकार) r < prefix_len + 1) {
                माप_प्रकार आकार;

                अगर (sym_LZ4_version_संख्या() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        वापस 0;

                अगर (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        अगर (स_भेद(*बफर, prefix, r) != 0)
                                वापस 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_आकार, बफर, &आकार, 0);
                अगर (r < 0)
                        वापस r;

                अगर (आकार < prefix_len + 1)
                        वापस 0;
        }

        वापस स_भेद(*बफर, prefix, prefix_len) == 0 &&
                ((स्थिर uint8_t*) *बफर)[prefix_len] == extra;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_startswith_zstd(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **बफर,
                स्थिर व्योम *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(बफर);
        निश्चित(prefix);

#अगर HAVE_ZSTD
        पूर्णांक r;

        r = dlopen_zstd(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        अचिन्हित दीर्घ दीर्घ आकार = sym_ZSTD_get_Frame_Content_आकार(src, src_आकार);
        अगर (आकार == ZSTD_CONTENTSIZE_ERROR)
                वापस -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        अगर (आकार != ZSTD_CONTENTSIZE_UNKNOWN && आकार < prefix_len + 1)
                वापस 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        अगर (!dctx)
                वापस -ENOMEM;

        अगर (!(greedy_realloc(बफर, MAX(sym_ZSTD_DStream_Out_आकार(), prefix_len + 1), 1)))
                वापस -ENOMEM;

        ZSTD_in_बफर निवेश = {
                .src = src,
                .आकार = src_आकार,
        };
        ZSTD_out_बफर output = {
                .dst = *बफर,
                .आकार = MALLOC_SIZEOF_SAFE(*बफर),
        };
        माप_प्रकार k;

        k = sym_ZSTD_decompressStream(dctx, &output, &निवेश);
        अगर (sym_ZSTD_isError(k))
                वापस log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        अगर (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                अगर (k != 0)
                        वापस log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                वापस 0; /* Decompressed text too short to match the prefix and extra */
        }

        वापस स_भेद(*बफर, prefix, prefix_len) == 0 &&
                ((स्थिर uint8_t*) *बफर)[prefix_len] == extra;
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_startswith_gzip(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **बफर,
                स्थिर व्योम *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(बफर);
        निश्चित(prefix);

#अगर HAVE_ZLIB
        पूर्णांक r;

        r = dlopen_zlib(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार > अच_पूर्णांक_उच्च)
                वापस -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णांक) माप(s));
        अगर (r != Z_OK)
                वापस -EBADMSG;

        अगर (!(greedy_realloc(बफर, ALIGN_8(prefix_len + 1), 1)))
                वापस -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*बफर);

        s.अग्र_in = (व्योम*) src;
        s.avail_in = src_आकार;

        s.अग्र_out = *बफर;
        s.avail_out = MIN(allocated, (माप_प्रकार) अच_पूर्णांक_उच्च);

        क्रम (;;) {
                r = sym_inflate(&s, Z_FINISH);

                अगर (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        वापस -EBADMSG;

                अगर (allocated - s.avail_out >= prefix_len + 1)
                        वापस स_भेद(*बफर, prefix, prefix_len) == 0 &&
                                ((स्थिर uint8_t*) *बफर)[prefix_len] == extra;

                अगर (r == Z_STREAM_END)
                        वापस 0;

                अगर (s.avail_out > 0)
                        वापस -EBADMSG; /* truncated */

                माप_प्रकार used = allocated - s.avail_out;

                अगर (!(greedy_realloc(बफर, allocated * 2, 1)))
                        वापस -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*बफर);
                s.avail_out = MIN(allocated - used, (माप_प्रकार) अच_पूर्णांक_उच्च);
                s.अग्र_out = *(uint8_t**)बफर + used;
        }
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

अटल पूर्णांक decompress_startswith_bzip2(
                स्थिर व्योम *src,
                uint64_t src_आकार,
                व्योम **बफर,
                स्थिर व्योम *prefix,
                माप_प्रकार prefix_len,
                uint8_t extra) {

        निश्चित(src);
        निश्चित(src_आकार > 0);
        निश्चित(बफर);
        निश्चित(prefix);

#अगर HAVE_BZIP2
        पूर्णांक r;

        r = dlopen_bzip2(LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        अगर (src_आकार > अच_पूर्णांक_उच्च)
                वापस -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        अगर (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                वापस -EIO;

        अगर (!(greedy_realloc(बफर, ALIGN_8(prefix_len + 1), 1)))
                वापस -ENOMEM;

        माप_प्रकार allocated = MALLOC_SIZEOF_SAFE(*बफर);

        s.अग्र_in = (अक्षर*) src;
        s.avail_in = src_आकार;

        s.अग्र_out = *बफर;
        s.avail_out = MIN(allocated, (माप_प्रकार) अच_पूर्णांक_उच्च);

        क्रम (;;) {
                r = sym_BZ2_bzDecompress(&s);

                अगर (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        वापस -EBADMSG;

                अगर (allocated - s.avail_out >= prefix_len + 1)
                        वापस स_भेद(*बफर, prefix, prefix_len) == 0 &&
                                ((स्थिर uint8_t*) *बफर)[prefix_len] == extra;

                अगर (r == BZ_STREAM_END)
                        वापस 0;

                अगर (s.avail_out > 0)
                        वापस -EBADMSG; /* truncated */

                माप_प्रकार used = allocated - s.avail_out;

                अगर (!(greedy_realloc(बफर, allocated * 2, 1)))
                        वापस -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*बफर);
                s.avail_out = MIN(allocated - used, (माप_प्रकार) अच_पूर्णांक_उच्च);
                s.अग्र_out = (अक्षर*) *बफर + used;
        }
#अन्यथा
        वापस -EPROTONOSUPPORT;
#पूर्ण_अगर
}

पूर्णांक decompress_startswith_journal(
                Compression compression,
                स्थिर व्योम *src, uint64_t src_आकार,
                व्योम **बफर,
                स्थिर व्योम *prefix, माप_प्रकार prefix_len,
                uint8_t extra) {

        चयन (compression) {
        हाल COMPRESSION_XZ:
                वापस decompress_startswith_xz(src, src_आकार, बफर, prefix, prefix_len, extra);
        हाल COMPRESSION_LZ4:
                वापस decompress_startswith_lz4(src, src_आकार, बफर, prefix, prefix_len, extra);
        हाल COMPRESSION_ZSTD:
                वापस decompress_startswith_zstd(src, src_आकार, बफर, prefix, prefix_len, extra);
        शेष:
                वापस -EOPNOTSUPP;
        }
}

पूर्णांक decompress_startswith(
                Compression compression,
                स्थिर व्योम *src, uint64_t src_आकार,
                व्योम **बफर,
                स्थिर व्योम *prefix, माप_प्रकार prefix_len,
                uint8_t extra) {

        चयन (compression) {
        हाल COMPRESSION_GZIP:
                वापस decompress_startswith_gzip(src, src_आकार, बफर, prefix, prefix_len, extra);
        हाल COMPRESSION_BZIP2:
                वापस decompress_startswith_bzip2(src, src_आकार, बफर, prefix, prefix_len, extra);
        शेष:
                वापस decompress_startswith_journal(compression, src, src_आकार, बफर, prefix, prefix_len, extra);
        }
}

पूर्णांक compress_stream(
                Compression type,
                पूर्णांक fdf, पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_आकार) {

        _cleanup_(compressor_freep) Compressor *c = शून्य;
        _cleanup_free_ व्योम *buf = शून्य;
        _cleanup_free_ uint8_t *निवेश = शून्य;
        माप_प्रकार buf_size = 0, buf_alloc = 0;
        uint64_t योग_in = 0, योग_out = 0;
        पूर्णांक r;

        निश्चित(fdf >= 0);
        निश्चित(fdt >= 0);

        r = compressor_new(&c, type);
        अगर (r < 0)
                वापस r;

        निवेश = new(uint8_t, COMPRESS_PIPE_बफर_आकार);
        अगर (!निवेश)
                वापस -ENOMEM;

        क्रम (;;) {
                माप_प्रकार m = COMPRESS_PIPE_बफर_आकार;
                ssize_t n;

                अगर (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (माप_प्रकार) max_bytes;

                n = पढ़ो(fdf, निवेश, m);
                अगर (n < 0)
                        वापस -त्रुटि_सं;

                अगर (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        अगर (r < 0)
                                वापस r;

                        अगर (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                अगर (r < 0)
                                        वापस r;
                                योग_out += buf_size;
                        }
                        अवरोध;
                }

                योग_in += n;
                अगर (max_bytes != UINT64_MAX) {
                        निश्चित(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, निवेश, n, &buf, &buf_size, &buf_alloc);
                अगर (r < 0)
                        वापस r;

                अगर (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        अगर (r < 0)
                                वापस r;
                        योग_out += buf_size;
                }
        }

        अगर (ret_uncompressed_आकार)
                *ret_uncompressed_आकार = योग_in;

        अगर (योग_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        अन्यथा
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), योग_in, योग_out, (द्विगुन) योग_out / योग_in * 100);

        वापस 0;
}

अटल पूर्णांक should_sparse(पूर्णांक fd) {
        काष्ठा stat st;

        निश्चित(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        अगर (ख_स्थिति(fd, &st) < 0)
                वापस -त्रुटि_सं;

        अगर (!S_ISREG(st.st_mode))
                वापस false;

        पूर्णांक flags = fcntl(fd, F_GETFL);
        अगर (flags < 0)
                वापस -त्रुटि_सं;

        अगर (FLAGS_SET(flags, O_APPEND))
                वापस false;

        off_t pos = lseek(fd, 0, प्रस्तुत_से);
        अगर (pos < 0)
                वापस -त्रुटि_सं;

        वापस pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
अटल पूर्णांक finalize_sparse(पूर्णांक fd) {
        off_t pos;

        निश्चित(fd >= 0);

        pos = lseek(fd, 0, प्रस्तुत_से);
        अगर (pos < 0)
                वापस -त्रुटि_सं;

        अगर (ftruncate(fd, pos) < 0)
                वापस -त्रुटि_सं;

        वापस 0;
}

/* Common helper for decompress_stream_*() wrappers */

काष्ठा decompress_stream_userdata {
        पूर्णांक fd;
        uint64_t max_bytes;
        uint64_t योग_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

अटल पूर्णांक decompress_stream_write_callback(स्थिर व्योम *तथ्य, माप_प्रकार आकार, व्योम *userdata) {
        काष्ठा decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        अगर (u->max_bytes != UINT64_MAX) {
                अगर (u->max_bytes < आकार)
                        वापस -EFBIG;
                u->max_bytes -= आकार;
        }

        u->योग_out += आकार;

        अगर (u->skip > 0) {
                अगर (u->skip >= आकार) {
                        u->skip -= आकार;
                        वापस 0;
                }

                तथ्य = (स्थिर uint8_t*) तथ्य + u->skip;
                आकार -= u->skip;
                u->skip = 0;
        }

        अगर (आकार == 0)
                वापस 0;

        अगर (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, तथ्य, आकार, 64);
                अगर (k < 0)
                        वापस (पूर्णांक) k;
                वापस 0;
        }

        वापस loop_write(u->fd, तथ्य, आकार);
}

अटल पूर्णांक decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        निश्चित(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        अगर (!c)
                वापस -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        चयन (type) {
        हाल COMPRESSION_NONE:
                अवरोध;

#अगर HAVE_XZ
        हाल COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                अगर (xz_ret != LZMA_OK)
                        वापस -EIO;
                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_LZ4
        हाल COMPRESSION_LZ4: {
                माप_प्रकार rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                अगर (sym_LZ4F_isError(rc))
                        वापस -ENOMEM;

                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_ZSTD
        हाल COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                अगर (!c->d_zstd)
                        वापस -ENOMEM;
                अवरोध;
#पूर्ण_अगर

#अगर HAVE_ZLIB
        हाल COMPRESSION_GZIP:
                अगर (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (पूर्णांक) माप(c->gzip)) != Z_OK)
                        वापस -EIO;
                अवरोध;
#पूर्ण_अगर

#अगर HAVE_BZIP2
        हाल COMPRESSION_BZIP2:
                अगर (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        वापस -EIO;
                अवरोध;
#पूर्ण_अगर

        शेष:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        वापस 0;
}

अटल पूर्णांक decompress_blob_to_fd_impl(
                Compression type,
                स्थिर व्योम *src,
                uint64_t src_आकार,
                पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_आकार) {

        पूर्णांक r;

        निश्चित(src);
        निश्चित(fdt >= 0);

        अगर (type == COMPRESSION_NONE)
                वापस -EOPNOTSUPP;

        अगर (src_आकार == 0)
                वापस -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        अगर (type == COMPRESSION_LZ4 &&
            src_आकार >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                अगर (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        अगर (src_आकार <= माप(uint64_t))
                                वापस -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        अगर (sz < skip)
                                वापस -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                अगर (max_bytes == 0)
                        वापस -ENOBUFS;

                _cleanup_free_ व्योम *buf = शून्य;
                माप_प्रकार आकार = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_आकार, &buf, &आकार,
                                        max_bytes >= SIZE_MAX ? 0 : (माप_प्रकार) max_bytes);
                अगर (r < 0)
                        वापस r;

                /* Check the result size again for safety. */
                अगर (आकार < skip)
                        वापस -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, आकार - skip);
                अगर (r < 0)
                        वापस r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_आकार, आकार, (द्विगुन) आकार / src_आकार * 100);

                अगर (ret_आकार)
                        *ret_आकार = आकार - skip;
                वापस 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = शून्य;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        अगर (r < 0)
                वापस r;

        काष्ठा decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_धकेलो(c, src, src_आकार, decompress_stream_write_callback, &userdata);
        अगर (r < 0)
                वापस r;

        /* Finalize */
        r = decompressor_धकेलो(c, /* data= */ शून्य, /* size= */ 0, decompress_stream_write_callback, &userdata);
        अगर (r < 0)
                वापस r;

        अगर (userdata.योग_out < skip)
                वापस -EBADMSG;

        अगर (userdata.sparse) {
                r = finalize_sparse(fdt);
                अगर (r < 0)
                        वापस r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_आकार, userdata.योग_out,
                  (द्विगुन) userdata.योग_out / src_आकार * 100);

        अगर (ret_आकार)
                *ret_आकार = userdata.योग_out - skip;

        वापस 0;
}

पूर्णांक decompress_blob_to_fd_journal(
                Compression type,
                स्थिर व्योम *src,
                uint64_t src_आकार,
                पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_आकार) {

        पूर्णांक r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        वापस decompress_blob_to_fd_impl(type, src, src_आकार, fdt, max_bytes, skip, ret_आकार);
}

पूर्णांक decompress_blob_to_fd(
                Compression type,
                स्थिर व्योम *src,
                uint64_t src_आकार,
                पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_आकार) {

        पूर्णांक r;

        r = dlopen_compress(type, LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        वापस decompress_blob_to_fd_impl(type, src, src_आकार, fdt, max_bytes, skip, ret_आकार);
}

पूर्णांक decompress_stream_full(
                Compression type,
                पूर्णांक fdf, पूर्णांक fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = शून्य;
        _cleanup_free_ uint8_t *buf = शून्य;
        uint64_t योग_in = 0;
        पूर्णांक r;

        निश्चित(fdf >= 0);
        निश्चित(fdt >= 0);

        अगर (type == COMPRESSION_NONE)
                वापस -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        r = decompressor_new(&c, type, flags);
        अगर (r < 0)
                वापस r;

        काष्ठा decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_बफर_आकार);
        अगर (!buf)
                वापस -ENOMEM;

        क्रम (;;) {
                ssize_t n;

                n = पढ़ो(fdf, buf, COMPRESS_PIPE_बफर_आकार);
                अगर (n < 0)
                        वापस -त्रुटि_सं;

                योग_in += n;

                r = decompressor_धकेलो(c, buf, n, decompress_stream_write_callback, &userdata);
                अगर (r < 0)
                        वापस r;

                अगर (n == 0)
                        अवरोध;
        }

        अगर (योग_in == 0)
                वापस log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        अगर (userdata.sparse) {
                r = finalize_sparse(fdt);
                अगर (r < 0)
                        वापस r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), योग_in, userdata.योग_out,
                  (द्विगुन) userdata.योग_out / योग_in * 100);

        वापस 0;
}

पूर्णांक decompress_stream_by_filename(स्थिर अक्षर *filename, पूर्णांक fdf, पूर्णांक fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        अगर (c == COMPRESSION_NONE)
                वापस -EPROTONOSUPPORT;

        वापस decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        अगर (!c)
                वापस शून्य;

        चयन (c->type) {

#अगर HAVE_XZ
        हाल COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                अवरोध;
#पूर्ण_अगर

#अगर HAVE_LZ4
        हाल COMPRESSION_LZ4:
                अगर (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = शून्य;
                        c->lz4_header = mfree(c->lz4_header);
                } अन्यथा {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = शून्य;
                }
                अवरोध;
#पूर्ण_अगर

#अगर HAVE_ZSTD
        हाल COMPRESSION_ZSTD:
                अगर (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = शून्य;
                } अन्यथा {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = शून्य;
                }
                अवरोध;
#पूर्ण_अगर

#अगर HAVE_ZLIB
        हाल COMPRESSION_GZIP:
                अगर (c->encoding)
                        sym_deflateEnd(&c->gzip);
                अन्यथा
                        sym_inflateEnd(&c->gzip);
                अवरोध;
#पूर्ण_अगर

#अगर HAVE_BZIP2
        हाल COMPRESSION_BZIP2:
                अगर (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                अन्यथा
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                अवरोध;
#पूर्ण_अगर

        शेष:
                अवरोध;
        }

        वापस mfree(c);
}

Compression compressor_type(स्थिर Compressor *c) {
        वापस c ? c->type : _COMPRESSION_INVALID;
}

पूर्णांक decompressor_detect(Decompressor **decompressor, स्थिर व्योम *तथ्य, माप_प्रकार आकार) {
        पूर्णांक r;

        निश्चित(decompressor);

        अगर (*decompressor)
                वापस 1;

        अगर (आकार < COMPRESSION_MAGIC_BYTES_MAX)
                वापस 0;

        निश्चित(तथ्य);

        Compression type = compression_detect_from_magic(तथ्य);
        अगर (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        अगर (r < 0)
                वापस r;

        वापस 1;
}

पूर्णांक decompressor_force(Decompressor **decompressor, Compression type) {
        पूर्णांक r;

        निश्चित(decompressor);
        निश्चित(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        *decompressor = compressor_free(*decompressor);
        वापस decompressor_new(decompressor, type, /* flags= */ 0);
}

पूर्णांक decompressor_force_off(Decompressor **decompressor) {
        वापस decompressor_force(decompressor, COMPRESSION_NONE);
}

पूर्णांक decompressor_धकेलो(Decompressor *c, स्थिर व्योम *तथ्य, माप_प्रकार आकार, DecompressorCallback callback, व्योम *userdata) {
        पूर्णांक r;

        निश्चित(c);
        निश्चित(तथ्य || आकार == 0);
        निश्चित(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        अगर (c->encoding)
                वापस -EINVAL;

        अगर (c->finished && आकार == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                वापस 0;

#अगर HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *बफर = शून्य;
        अगर (c->type != COMPRESSION_NONE) {
                बफर = new(uint8_t, COMPRESS_PIPE_बफर_आकार);
                अगर (!बफर)
                        वापस -ENOMEM;
        }
#पूर्ण_अगर

        चयन (c->type) {

        हाल COMPRESSION_NONE:
                अगर (c->finished)
                        वापस -EBADMSG;

                अगर (आकार == 0) {
                        c->finished = true;
                        वापस 0;
                }

                r = callback(तथ्य, आकार, userdata);
                अगर (r < 0)
                        वापस r;

                वापस 0;

#अगर HAVE_XZ
        हाल COMPRESSION_XZ:
                अगर (c->finished)
                        वापस -EBADMSG;

                c->xz.अग्र_in = तथ्य;
                c->xz.avail_in = आकार;

                क्रम (;;) {
                        c->xz.अग्र_out = बफर;
                        c->xz.avail_out = COMPRESS_PIPE_बफर_आकार;

                        lzma_ret lzr = sym_lzma_code(&c->xz, आकार == 0 ? LZMA_FINISH : LZMA_RUN);
                        अगर (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                वापस -EBADMSG;

                        अगर (c->xz.avail_out > COMPRESS_PIPE_बफर_आकार)
                                वापस -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_बफर_आकार - c->xz.avail_out;
                        अगर (produced > 0) {
                                r = callback(बफर, produced, userdata);
                                अगर (r < 0)
                                        वापस r;
                        }

                        अगर (lzr == LZMA_STREAM_END) {
                                अगर (c->xz.avail_in > 0)
                                        वापस -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                वापस 0;
                        }

                        अगर (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                अगर (आकार == 0)
                                        वापस -EBADMSG; /* truncated */

                                वापस 0; /* No progress possible with the current input. */
                        }
                }
#पूर्ण_अगर

#अगर HAVE_LZ4
        हाल COMPRESSION_LZ4: {
                स्थिर uint8_t *src = तथ्य;
                माप_प्रकार src_remaining = आकार;

                c->finished = false;

                क्रम (;;) {
                        माप_प्रकार produced = COMPRESS_PIPE_बफर_आकार;
                        माप_प्रकार consumed = src_remaining;

                        माप_प्रकार rc = sym_LZ4F_decompress(c->d_lz4, बफर, &produced, src, &consumed, शून्य);
                        अगर (sym_LZ4F_isError(rc))
                                वापस -EBADMSG;

                        अगर (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_बफर_आकार)
                                वापस -EIO;

                        अगर (consumed > 0) {
                                निश्चित(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        अगर (produced > 0) {
                                r = callback(बफर, produced, userdata);
                                अगर (r < 0)
                                        वापस r;
                        }

                        अगर (src_remaining == 0) {
                                अगर (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        वापस 0;
                                }

                                अगर (produced == COMPRESS_PIPE_बफर_आकार)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        जारी;

                                अगर (आकार == 0)
                                        वापस -EBADMSG; /* truncated */

                                वापस 0; /* No progress possible with the current input. */
                        }
                }
        }
#पूर्ण_अगर

#अगर HAVE_ZSTD
        हाल COMPRESSION_ZSTD: {
                ZSTD_in_बफर निवेश = {
                        .src =  (व्योम*) तथ्य,
                        .आकार = आकार,
                };

                c->finished = false;

                क्रम (;;) {
                        ZSTD_out_बफर output = {
                                .dst = बफर,
                                .आकार = COMPRESS_PIPE_बफर_आकार,
                        };

                        माप_प्रकार res = sym_ZSTD_decompressStream(c->d_zstd, &output, &निवेश);
                        अगर (sym_ZSTD_isError(res))
                                वापस -EBADMSG;

                        अगर (निवेश.pos > निवेश.आकार ||
                            output.pos > output.आकार)
                                वापस -EIO;

                        अगर (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                अगर (r < 0)
                                        वापस r;
                        }

                        अगर (निवेश.pos == निवेश.आकार) {
                                अगर (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        वापस 0;
                                }

                                अगर (output.pos == COMPRESS_PIPE_बफर_आकार)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        जारी;

                                अगर (आकार == 0)
                                        वापस -EBADMSG; /* truncated */

                                वापस 0; /* No progress possible with the current input. */
                        }
                }
        }
#पूर्ण_अगर

#अगर HAVE_ZLIB
        हाल COMPRESSION_GZIP:
                अगर (आकार > अच_पूर्णांक_उच्च)
                        वापस -EFBIG;

                c->gzip.अग्र_in = (व्योम*) तथ्य;
                c->gzip.avail_in = आकार;

                क्रम (;;) {
                        अगर (c->finished) {
                                अगर (sym_inflateReset(&c->gzip) != Z_OK)
                                        वापस -EIO;

                                c->finished = false;
                        }

                        c->gzip.अग्र_out = बफर;
                        c->gzip.avail_out = COMPRESS_PIPE_बफर_आकार;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        पूर्णांक zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        अगर (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                वापस -EBADMSG;

                        अगर (c->gzip.avail_out > COMPRESS_PIPE_बफर_आकार)
                                वापस -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_बफर_आकार - c->gzip.avail_out;
                        अगर (produced > 0) {
                                r = callback(बफर, produced, userdata);
                                अगर (r < 0)
                                        वापस r;
                        }

                        अगर (zr == Z_STREAM_END) {
                                c->finished = true;

                                अगर (c->gzip.avail_in > 0)
                                        जारी; /* New stream may be pending. */

                                वापस 0;
                        }

                        अगर (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                अगर (आकार == 0)
                                        वापस -EBADMSG; /* truncated */

                                वापस 0; /* No progress possible with the current input. */
                        }
                }
#पूर्ण_अगर

#अगर HAVE_BZIP2
        हाल COMPRESSION_BZIP2:
                अगर (आकार > अच_पूर्णांक_उच्च)
                        वापस -EFBIG;

                c->bzip2.अग्र_in = (अक्षर*) तथ्य;
                c->bzip2.avail_in = आकार;

                क्रम (;;) {
                        अगर (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                अगर (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        वापस -EIO;

                                c->finished = false;
                        }

                        c->bzip2.अग्र_out = (अक्षर*) बफर;
                        c->bzip2.avail_out = COMPRESS_PIPE_बफर_आकार;

                        पूर्णांक bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        अगर (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                वापस -EBADMSG;

                        अगर (c->bzip2.avail_out > COMPRESS_PIPE_बफर_आकार)
                                वापस -EIO;

                        माप_प्रकार produced = COMPRESS_PIPE_बफर_आकार - c->bzip2.avail_out;
                        अगर (produced > 0) {
                                r = callback(बफर, produced, userdata);
                                अगर (r < 0)
                                        वापस r;
                        }

                        अगर (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                अगर (c->bzip2.avail_in > 0)
                                        जारी; /* New stream may be pending. */

                                वापस 0;
                        }

                        अगर (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                अगर (आकार == 0)
                                        वापस -EBADMSG; /* truncated */

                                वापस 0; /* No progress possible with the current input. */
                        }
                }
#पूर्ण_अगर

        शेष:
                assert_not_reached();
        }
}

पूर्णांक compressor_new(Compressor **ret, Compression type) {
        पूर्णांक r;

        निश्चित(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        अगर (r < 0)
                वापस r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        अगर (!c)
                वापस -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        चयन (type) {

#अगर HAVE_XZ
        हाल COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                अगर (xzr != LZMA_OK)
                        वापस -EIO;

                c->type = COMPRESSION_XZ;
                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_LZ4
        हाल COMPRESSION_LZ4: {
                माप_प्रकार rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                अगर (sym_LZ4F_isError(rc))
                        वापस -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                माप_प्रकार header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = दो_स्मृति(header_bound);
                अगर (!c->lz4_header)
                        वापस -ENOMEM;

                c->lz4_header_आकार = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                अगर (sym_LZ4F_isError(c->lz4_header_आकार))
                        वापस -EINVAL;

                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_ZSTD
        हाल COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                अगर (!c->c_zstd)
                        वापस -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                माप_प्रकार z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                अगर (sym_ZSTD_isError(z))
                        वापस -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                अगर (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                अवरोध;
#पूर्ण_अगर

#अगर HAVE_ZLIB
        हाल COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (पूर्णांक) माप(c->gzip));
                अगर (r != Z_OK)
                        वापस -EIO;

                c->type = COMPRESSION_GZIP;
                अवरोध;
#पूर्ण_अगर

#अगर HAVE_BZIP2
        हाल COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                अगर (r != BZ_OK)
                        वापस -EIO;

                c->type = COMPRESSION_BZIP2;
                अवरोध;
#पूर्ण_अगर

        हाल COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                अवरोध;

        शेष:
                वापस -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        वापस 0;
}

#अगर HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
अटल पूर्णांक enlarge_बफर(व्योम **बफर, माप_प्रकार *बफर_आकार, माप_प्रकार *बफर_allocated, माप_प्रकार need) {
        निश्चित(बफर);
        निश्चित(बफर_आकार);
        निश्चित(बफर_allocated);

        need = MAX3(need, *बफर_आकार + 1, (माप_प्रकार) COMPRESS_PIPE_बफर_आकार);
        अगर (*बफर_allocated >= need)
                वापस 0;

        अगर (!greedy_realloc(बफर, need, 1))
                वापस -ENOMEM;

        *बफर_allocated = MALLOC_SIZEOF_SAFE(*बफर);
        वापस 1;
}
#पूर्ण_अगर

पूर्णांक compressor_start(
                Compressor *c,
                स्थिर व्योम *तथ्य,
                माप_प्रकार आकार,
                व्योम **बफर,
                माप_प्रकार *बफर_आकार,
                माप_प्रकार *बफर_allocated) {

#अगर HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णांक r;
#पूर्ण_अगर

        निश्चित(c);
        निश्चित(बफर);
        निश्चित(बफर_आकार);
        निश्चित(बफर_allocated);

        अगर (!c->encoding)
                वापस -EINVAL;

        अगर (आकार == 0)
                वापस 0;

        निश्चित(तथ्य);

        *बफर_आकार = 0;

        चयन (c->type) {

#अगर HAVE_XZ
        हाल COMPRESSION_XZ:

                c->xz.अग्र_in = तथ्य;
                c->xz.avail_in = आकार;

                जबतक (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        अगर (r < 0)
                                वापस r;

                        c->xz.अग्र_out = (uint8_t*) *बफर + *बफर_आकार;
                        c->xz.avail_out = *बफर_allocated - *बफर_आकार;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        अगर (lzr != LZMA_OK)
                                वापस -EIO;

                        *बफर_आकार += (*बफर_allocated - *बफर_आकार) - c->xz.avail_out;
                }

                अवरोध;
#पूर्ण_अगर

#अगर HAVE_LZ4
        हाल COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                अगर (c->lz4_header_आकार > 0) {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, c->lz4_header_आकार);
                        अगर (r < 0)
                                वापस r;

                        स_नकल(*बफर, c->lz4_header, c->lz4_header_आकार);
                        *बफर_आकार = c->lz4_header_आकार;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_आकार = 0;
                }

                माप_प्रकार bound = sym_LZ4F_compressBound(आकार, &lz4_preferences);
                r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, *बफर_आकार + bound);
                अगर (r < 0)
                        वापस r;

                माप_प्रकार n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *बफर + *बफर_आकार,
                                                   *बफर_allocated - *बफर_आकार,
                                                   तथ्य, आकार, शून्य);
                अगर (sym_LZ4F_isError(n))
                        वापस -EIO;

                *बफर_आकार += n;
                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_ZSTD
        हाल COMPRESSION_ZSTD: {
                ZSTD_in_बफर निवेश = {
                        .src = तथ्य,
                        .आकार = आकार,
                };

                जबतक (निवेश.pos < निवेश.आकार) {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        अगर (r < 0)
                                वापस r;

                        ZSTD_out_बफर output = {
                                .dst = ((uint8_t *) *बफर + *बफर_आकार),
                                .आकार = *बफर_allocated - *बफर_आकार,
                        };

                        माप_प्रकार res = sym_ZSTD_compressStream2(c->c_zstd, &output, &निवेश, ZSTD_e_continue);
                        अगर (sym_ZSTD_isError(res))
                                वापस -EIO;

                        *बफर_आकार += output.pos;
                }

                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_ZLIB
        हाल COMPRESSION_GZIP:
                अगर (आकार > अच_पूर्णांक_उच्च)
                        वापस -EFBIG;

                c->gzip.अग्र_in = (व्योम*) तथ्य;
                c->gzip.avail_in = आकार;

                जबतक (c->gzip.avail_in > 0) {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        अगर (r < 0)
                                वापस r;

                        माप_प्रकार avail = MIN(*बफर_allocated - *बफर_आकार, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->gzip.अग्र_out = (uint8_t*) *बफर + *बफर_आकार;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        अगर (r != Z_OK)
                                वापस -EIO;

                        *बफर_आकार += avail - c->gzip.avail_out;
                }

                अवरोध;
#पूर्ण_अगर

#अगर HAVE_BZIP2
        हाल COMPRESSION_BZIP2:
                अगर (आकार > अच_पूर्णांक_उच्च)
                        वापस -EFBIG;

                c->bzip2.अग्र_in = (व्योम*) तथ्य;
                c->bzip2.avail_in = आकार;

                जबतक (c->bzip2.avail_in > 0) {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        अगर (r < 0)
                                वापस r;

                        माप_प्रकार avail = MIN(*बफर_allocated - *बफर_आकार, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->bzip2.अग्र_out = (व्योम*) ((uint8_t*) *बफर + *बफर_आकार);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        अगर (r != BZ_RUN_OK)
                                वापस -EIO;

                        *बफर_आकार += avail - c->bzip2.avail_out;
                }

                अवरोध;
#पूर्ण_अगर

        हाल COMPRESSION_NONE:

                अगर (*बफर_allocated < आकार) {
                        व्योम *p;

                        p = पुनः_स्मृति(*बफर, आकार);
                        अगर (!p)
                                वापस -ENOMEM;

                        *बफर = p;
                        *बफर_allocated = आकार;
                }

                स_नकल(*बफर, तथ्य, आकार);
                *बफर_आकार = आकार;
                अवरोध;

        शेष:
                वापस -EOPNOTSUPP;
        }

        वापस 0;
}

पूर्णांक compressor_finish(Compressor *c, व्योम **बफर, माप_प्रकार *बफर_आकार, माप_प्रकार *बफर_allocated) {
#अगर HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        पूर्णांक r;
#पूर्ण_अगर

        निश्चित(c);
        निश्चित(बफर);
        निश्चित(बफर_आकार);
        निश्चित(बफर_allocated);

        अगर (!c->encoding)
                वापस -EINVAL;

        *बफर_आकार = 0;

        चयन (c->type) {

#अगर HAVE_XZ
        हाल COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                करो {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        अगर (r < 0)
                                वापस r;

                        c->xz.अग्र_out = (uint8_t*) *बफर + *बफर_आकार;
                        c->xz.avail_out = *बफर_allocated - *बफर_आकार;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        अगर (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                वापस -EIO;

                        *बफर_आकार += (*बफर_allocated - *बफर_आकार) - c->xz.avail_out;
                } जबतक (lzr != LZMA_STREAM_END);

                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_LZ4
        हाल COMPRESSION_LZ4: {
                माप_प्रकार bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, bound);
                अगर (r < 0)
                        वापस r;

                माप_प्रकार n = sym_LZ4F_compressEnd(c->c_lz4, *बफर, *बफर_allocated, शून्य);
                अगर (sym_LZ4F_isError(n))
                        वापस -EIO;

                *बफर_आकार = n;
                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_ZSTD
        हाल COMPRESSION_ZSTD: {
                ZSTD_in_बफर निवेश = {};
                माप_प्रकार res;

                करो {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        अगर (r < 0)
                                वापस r;

                        ZSTD_out_बफर output = {
                                .dst = ((uint8_t *) *बफर + *बफर_आकार),
                                .आकार = *बफर_allocated - *बफर_आकार,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &निवेश, ZSTD_e_end);
                        अगर (sym_ZSTD_isError(res))
                                वापस -EIO;

                        *बफर_आकार += output.pos;
                } जबतक (res != 0);

                अवरोध;
        }
#पूर्ण_अगर

#अगर HAVE_ZLIB
        हाल COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                करो {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        अगर (r < 0)
                                वापस r;

                        माप_प्रकार avail = MIN(*बफर_allocated - *बफर_आकार, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->gzip.अग्र_out = (uint8_t*) *बफर + *बफर_आकार;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        अगर (!IN_SET(r, Z_OK, Z_STREAM_END))
                                वापस -EIO;

                        *बफर_आकार += avail - c->gzip.avail_out;
                } जबतक (r != Z_STREAM_END);

                अवरोध;
#पूर्ण_अगर

#अगर HAVE_BZIP2
        हाल COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                करो {
                        r = enlarge_बफर(बफर, बफर_आकार, बफर_allocated, /* need= */ 0);
                        अगर (r < 0)
                                वापस r;

                        माप_प्रकार avail = MIN(*बफर_allocated - *बफर_आकार, (माप_प्रकार) अच_पूर्णांक_उच्च);
                        c->bzip2.अग्र_out = (व्योम*) ((uint8_t*) *बफर + *बफर_आकार);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        अगर (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                वापस -EIO;

                        *बफर_आकार += avail - c->bzip2.avail_out;
                } जबतक (r != BZ_STREAM_END);

                अवरोध;
#पूर्ण_अगर

        हाल COMPRESSION_NONE:
                अवरोध;

        शेष:
                वापस -EOPNOTSUPP;
        }

        वापस 0;
}
