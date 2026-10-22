/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಯದಿ HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#ಅನ್ತರ್ಭಾವ <lzma.h>
#endif

#ಯದಿ HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#ಅನ್ತರ್ಭಾವ <lz4.h>
#ಅನ್ತರ್ಭಾವ <lz4frame.h>
#ಅನ್ತರ್ಭಾವ <lz4hc.h>
#endif

#ಯದಿ HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#ಅನ್ತರ್ಭಾವ <zstd.h>
#ಅನ್ತರ್ಭಾವ <zstd_errors.h>
#endif

#ಯದಿ HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#ಅನ್ತರ್ಭಾವ <zlib.h>
#endif

#ಯದಿ HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#ಅನ್ತರ್ಭಾವ <bzlib.h>
#endif

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "bitfield.h"
#ಅನ್ತರ್ಭಾವ "compress.h"
#ಅನ್ತರ್ಭಾವ "dlfcn-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"

#ಯದಿ HAVE_XZ
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_auto_decoder) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_code) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_easy_encoder) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_end) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_stream_decoder) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_lzma_preset) = ರಿಕ್ತ;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#ಯದಿ HAVE_LZ4
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_compressBegin) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_compressBound) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_compressEnd) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_compressUpdate) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_decompress) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_isError) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_compress_HC) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_compress_default) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_decompress_safe) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_version_ಸಂಖ್ಯೆ) = ರಿಕ್ತ;

ಸ್ಥಿರ ಧ್ರುವ LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#ಯದಿ HAVE_ZSTD
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_compress) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_compressStream2) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_createCCtx) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_createDCtx) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_decompressStream) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_freeCCtx) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_freeDCtx) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_getErrorCode) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_getErrorName) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_isError) = ರಿಕ್ತ;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, ರಿಕ್ತ);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ zstd_ret_to_errno(size_t ret) {
        ವಿಕಲ್ಪ (sym_ZSTD_getErrorCode(ret)) {
        ಪಕ್ಷ ZSTD_error_dstSize_tooSmall:
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;
        ಪಕ್ಷ ZSTD_error_memory_allocation:
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;
        }
}
#endif

#ಯದಿ HAVE_ZLIB
ಸ್ಥಿರ DLSYM_PROTOTYPE(deflateInit2_) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(deflate) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(deflateEnd) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(inflateInit2_) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(inflate) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(inflateEnd) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(inflateReset) = ರಿಕ್ತ;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#ಯದಿ HAVE_BZIP2
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzCompressInit) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzCompress) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzDecompress) = ರಿಕ್ತ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = ರಿಕ್ತ;

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
ಸಂರಚನಾ Compressor {
        Compression type;
        bool encoding;
        bool finished;
        ಸಙ್ಘ {
#ಯದಿ HAVE_XZ
                lzma_stream xz;
#endif
#ಯದಿ HAVE_LZ4
                ಸಂರಚನಾ {
                        LZ4F_compressionContext_t c_lz4;
                        ಶೂನ್ಯ *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#ಯದಿ HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#ಯದಿ HAVE_ZLIB
                z_stream gzip;
#endif
#ಯದಿ HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, ಪರಿಮಾಣ(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(ಧ್ರುವ ವರ್ಣ *s) {
        Compression c;

        ಪ್ರತಿಜ್ಞಾ(s);

        c = compression_from_string(s);
        ಯದಿ (c >= 0)
                ಪ್ರತ್ಯಾಗಮ c;

        ಪ್ರತ್ಯಾಗಮ compression_uppercase_from_string(s);
}

Compression compression_from_filename(ಧ್ರುವ ವರ್ಣ *filename) {
        Compression c;
        ಧ್ರುವ ವರ್ಣ *e;

        ಪ್ರತಿಜ್ಞಾ(filename);

        e = strrchr(filename, '.');
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        ಯದಿ (c < 0)
                ಪ್ರತ್ಯಾಗಮ COMPRESSION_NONE;

        ಪ್ರತ್ಯಾಗಮ c;
}

bool compression_supported_journal(Compression c) {
        ಸ್ಥಿರ ಧ್ರುವ unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        ಪ್ರತಿಜ್ಞಾ(c >= 0);
        ಪ್ರತಿಜ್ಞಾ(c < _COMPRESSION_MAX);

        ಪ್ರತ್ಯಾಗಮ BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        ಸ್ಥಿರ ಧ್ರುವ unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        ಪ್ರತಿಜ್ಞಾ(c >= 0);
        ಪ್ರತಿಜ್ಞಾ(c < _COMPRESSION_MAX);

        ಪ್ರತ್ಯಾಗಮ BIT_SET(supported, c);
}

Compression compression_detect_from_magic(ಧ್ರುವ uint8_t data[ಸ್ಥಿರ COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        ಯದಿ (memcmp(data, (ಧ್ರುವ uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                ಪ್ರತ್ಯಾಗಮ COMPRESSION_GZIP;
        ಯದಿ (memcmp(data, (ಧ್ರುವ uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                ಪ್ರತ್ಯಾಗಮ COMPRESSION_XZ;
        ಯದಿ (memcmp(data, (ಧ್ರುವ uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                ಪ್ರತ್ಯಾಗಮ COMPRESSION_ZSTD;
        ಯದಿ (memcmp(data, (ಧ್ರುವ uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                ಪ್ರತ್ಯಾಗಮ COMPRESSION_LZ4;
        ಯದಿ (memcmp(data, (ಧ್ರುವ uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                ಪ್ರತ್ಯಾಗಮ COMPRESSION_BZIP2;

        ಪ್ರತ್ಯಾಗಮ _COMPRESSION_INVALID;
}

ಪೂರ್ಣಾಙ್ಕ dlopen_xz(ಪೂರ್ಣಾಙ್ಕ log_level) {
#ಯದಿ HAVE_XZ
        ಸ್ಥಿರ ಶೂನ್ಯ *lzma_dl = ರಿಕ್ತ;

        LIBLZMA_NOTE(suggested);

        ಪ್ರತ್ಯಾಗಮ dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

ಪೂರ್ಣಾಙ್ಕ dlopen_lz4(ಪೂರ್ಣಾಙ್ಕ log_level) {
#ಯದಿ HAVE_LZ4
        ಸ್ಥಿರ ಶೂನ್ಯ *lz4_dl = ರಿಕ್ತ;

        LIBLZ4_NOTE(suggested);

        ಪ್ರತ್ಯಾಗಮ dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_ಸಂಖ್ಯೆ));
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

ಪೂರ್ಣಾಙ್ಕ dlopen_zstd(ಪೂರ್ಣಾಙ್ಕ log_level) {
#ಯದಿ HAVE_ZSTD
        ಸ್ಥಿರ ಶೂನ್ಯ *zstd_dl = ರಿಕ್ತ;

        LIBZSTD_NOTE(suggested);

        ಪ್ರತ್ಯಾಗಮ dlopen_many_sym_or_warn(
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
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

ಪೂರ್ಣಾಙ್ಕ dlopen_zlib(ಪೂರ್ಣಾಙ್ಕ log_level) {
#ಯದಿ HAVE_ZLIB
        ಸ್ಥಿರ ಶೂನ್ಯ *zlib_dl = ರಿಕ್ತ;

        LIBZ_NOTE(suggested);

        ಪ್ರತ್ಯಾಗಮ dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

ಪೂರ್ಣಾಙ್ಕ dlopen_bzip2(ಪೂರ್ಣಾಙ್ಕ log_level) {
#ಯದಿ HAVE_BZIP2
        ಸ್ಥಿರ ಶೂನ್ಯ *bzip2_dl = ರಿಕ್ತ;

        LIBBZ2_NOTE(suggested);

        ಪ್ರತ್ಯಾಗಮ dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

ಪೂರ್ಣಾಙ್ಕ dlopen_compress_journal(Compression c, ಪೂರ್ಣಾಙ್ಕ log_level) {
        ವಿಕಲ್ಪ (c) {
        ಪಕ್ಷ COMPRESSION_NONE:
                ಪ್ರತ್ಯಾಗಮ 0;
        ಪಕ್ಷ COMPRESSION_XZ:
                ಪ್ರತ್ಯಾಗಮ dlopen_xz(log_level);
        ಪಕ್ಷ COMPRESSION_LZ4:
                ಪ್ರತ್ಯಾಗಮ dlopen_lz4(log_level);
        ಪಕ್ಷ COMPRESSION_ZSTD:
                ಪ್ರತ್ಯಾಗಮ dlopen_zstd(log_level);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        }
}

ಪೂರ್ಣಾಙ್ಕ dlopen_compress(Compression c, ಪೂರ್ಣಾಙ್ಕ log_level) {
        ವಿಕಲ್ಪ (c) {
        ಪಕ್ಷ COMPRESSION_GZIP:
                ಪ್ರತ್ಯಾಗಮ dlopen_zlib(log_level);
        ಪಕ್ಷ COMPRESSION_BZIP2:
                ಪ್ರತ್ಯಾಗಮ dlopen_bzip2(log_level);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ dlopen_compress_journal(c, log_level);
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compress_blob_xz(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ಪೂರ್ಣಾಙ್ಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, ರಿಕ್ತ, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, ರಿಕ್ತ }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_xz(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                ಯದಿ (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ಯದಿ (src_size < 80)
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ ರಿಕ್ತ,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        ಯದಿ (ret != LZMA_OK)
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

        *dst_size = out_pos;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compress_blob_lz4(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ಪೂರ್ಣಾಙ್ಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_LZ4
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_lz4(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ಯದಿ (src_size < 9)
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

        ಯದಿ (src_size > INT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;
        ಯದಿ (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        ಯದಿ (level <= 0)
                r = sym_LZ4_compress_default(src, (ವರ್ಣ*)dst + 8, src_size, (ಪೂರ್ಣಾಙ್ಕ) dst_alloc_size - 8);
        ಅನ್ಯಥಾ
                r = sym_LZ4_compress_HC(src, (ವರ್ಣ*)dst + 8, src_size, (ಪೂರ್ಣಾಙ್ಕ) dst_alloc_size - 8, level);
        ಯದಿ (r <= 0)
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compress_blob_zstd(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ಪೂರ್ಣಾಙ್ಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_ZSTD
        size_t k;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_zstd(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        ಯದಿ (sym_ZSTD_isError(k))
                ಪ್ರತ್ಯಾಗಮ zstd_ret_to_errno(k);

        *dst_size = k;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compress_blob_gzip(ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                       ಶೂನ್ಯ *dst, size_t dst_alloc_size, size_t *dst_size, ಪೂರ್ಣಾಙ್ಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_ZLIB
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_zlib(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size > UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;
        ಯದಿ (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (ಪೂರ್ಣಾಙ್ಕ) ಪರಿಮಾಣ(s));
        ಯದಿ (r != Z_OK)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        s.next_in = (ಶೂನ್ಯ*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        ಯದಿ (r != Z_STREAM_END)
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compress_blob_bzip2(
                ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                ಶೂನ್ಯ *dst, size_t dst_alloc_size, size_t *dst_size, ಪೂರ್ಣಾಙ್ಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_BZIP2
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size > UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;
        ಯದಿ (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        ಯದಿ (r != BZ_OK)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        s.next_in = (ವರ್ಣ*) src;
        s.avail_in = src_size;
        s.next_out = (ವರ್ಣ*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        ಯದಿ (r != BZ_STREAM_END)
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಪೂರ್ಣಾಙ್ಕ compress_blob_journal(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                ಶೂನ್ಯ *dst, size_t dst_alloc_size, size_t *dst_size, ಪೂರ್ಣಾಙ್ಕ level) {

        ವಿಕಲ್ಪ (compression) {
        ಪಕ್ಷ COMPRESSION_XZ:
                ಪ್ರತ್ಯಾಗಮ compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        ಪಕ್ಷ COMPRESSION_LZ4:
                ಪ್ರತ್ಯಾಗಮ compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        ಪಕ್ಷ COMPRESSION_ZSTD:
                ಪ್ರತ್ಯಾಗಮ compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        }
}

ಪೂರ್ಣಾಙ್ಕ compress_blob(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                ಶೂನ್ಯ *dst, size_t dst_alloc_size, size_t *dst_size, ಪೂರ್ಣಾಙ್ಕ level) {

        ವಿಕಲ್ಪ (compression) {
        ಪಕ್ಷ COMPRESSION_GZIP:
                ಪ್ರತ್ಯಾಗಮ compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        ಪಕ್ಷ COMPRESSION_BZIP2:
                ಪ್ರತ್ಯಾಗಮ compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_blob_xz(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_XZ
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_xz(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ಯದಿ (ret != LZMA_OK)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        ಯದಿ (!greedy_realloc(dst, space, 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        ಪ್ರತಿ (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                ಯದಿ (ret == LZMA_STREAM_END)
                        ಭಙ್ಗ;
                ಯದಿ (ret != LZMA_OK)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ಭಙ್ಗ;
                ಯದಿ (dst_max > 0 && space == dst_max)
                        ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                ಪ್ರತಿಜ್ಞಾ(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                ಯದಿ (!greedy_realloc(dst, space, 1))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_blob_lz4(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_LZ4
        ವರ್ಣ* out;
        ಪೂರ್ಣಾಙ್ಕ r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size <= 8)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (src_size - 8 > INT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;

        size = unaligned_read_le64(src);
        ಯದಿ (size < 0 || (unsigned) size != unaligned_read_le64(src))
                ಪ್ರತ್ಯಾಗಮ -EFBIG;
        ಯದಿ (dst_max > 0 && (size_t) size > dst_max)
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        ಯದಿ (!out)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        r = sym_LZ4_decompress_safe((ವರ್ಣ*)src + 8, out, src_size - 8, size);
        ಯದಿ (r < 0 || r != size)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        *dst_size = size;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(ಶೂನ್ಯ) {
#ಯದಿ HAVE_ZSTD
        ಯದಿ (dlopen_zstd(LOG_DEBUG) < 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ sym_ZSTD_DStreamOutSize();
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_blob_zstd(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_ZSTD
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_zstd(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        ಯದಿ (size == ZSTD_CONTENTSIZE_ERROR)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        ಯದಿ (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        ಅನ್ಯಥಾ {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        ಯದಿ (!greedy_realloc(dst, space, 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ಯದಿ (!dctx)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ZSTD_inBuffer ಒಳಹರಿವು = {
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

        ಪ್ರತಿ (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &ಒಳಹರಿವು);
                ಯದಿ (sym_ZSTD_isError(k))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                ಯದಿ (output.pos >= cap)
                        ಭಙ್ಗ;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                ಯದಿ (k == 0 && ಒಳಹರಿವು.pos >= ಒಳಹರಿವು.size)
                        ಭಙ್ಗ;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                ಯದಿ (output.pos == output.size) {
                        ಯದಿ (space > SIZE_MAX / 2)
                                ಪ್ರತ್ಯಾಗಮ -E2BIG;
                        space = MIN(2 * space, cap);
                        ಯದಿ (!greedy_realloc(dst, space, 1))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        ಅನುವರ್ತ;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                ಯದಿ (ಒಳಹರಿವು.pos >= ಒಳಹರಿವು.size)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_blob_gzip(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_ZLIB
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_zlib(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size > UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ಪೂರ್ಣಾಙ್ಕ) ಪರಿಮಾಣ(s));
        ಯದಿ (r != Z_OK)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ಯದಿ (!greedy_realloc(dst, space, 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        s.next_in = (ಶೂನ್ಯ*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        ಪ್ರತಿ (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                ಯದಿ (r == Z_STREAM_END)
                        ಭಙ್ಗ;
                ಯದಿ (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಯದಿ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ಭಙ್ಗ;
                ಯದಿ (dst_max > 0 && space == dst_max)
                        ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                ಯದಿ (!greedy_realloc(dst, space, 1))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_blob_bzip2(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_size);

#ಯದಿ HAVE_BZIP2
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size > UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        ಯದಿ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ಯದಿ (!greedy_realloc(dst, space, 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        s.next_in = (ವರ್ಣ*) src;
        s.avail_in = src_size;
        s.next_out = (ವರ್ಣ*) *dst;
        s.avail_out = space;

        ಪ್ರತಿ (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                ಯದಿ (r == BZ_STREAM_END)
                        ಭಙ್ಗ;
                ಯದಿ (r != BZ_OK)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಯದಿ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ಭಙ್ಗ;
                ಯದಿ (dst_max > 0 && space == dst_max)
                        ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                ಯದಿ (!greedy_realloc(dst, space, 1))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (ವರ್ಣ*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಪೂರ್ಣಾಙ್ಕ decompress_blob_journal(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ವಿಕಲ್ಪ (compression) {
        ಪಕ್ಷ COMPRESSION_XZ:
                ಪ್ರತ್ಯಾಗಮ decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        ಪಕ್ಷ COMPRESSION_LZ4:
                ಪ್ರತ್ಯಾಗಮ decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        ಪಕ್ಷ COMPRESSION_ZSTD:
                ಪ್ರತ್ಯಾಗಮ decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
        }
}

ಪೂರ್ಣಾಙ್ಕ decompress_blob(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ವಿಕಲ್ಪ (compression) {
        ಪಕ್ಷ COMPRESSION_GZIP:
                ಪ್ರತ್ಯಾಗಮ decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        ಪಕ್ಷ COMPRESSION_BZIP2:
                ಪ್ರತ್ಯಾಗಮ decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

ಪೂರ್ಣಾಙ್ಕ decompress_zlib_raw(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ *dst,
                size_t dst_size,
                ಪೂರ್ಣಾಙ್ಕ wbits) {

#ಯದಿ HAVE_ZLIB
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_zlib(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size > UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;
        ಯದಿ (dst_size > UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (ಶೂನ್ಯ*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (ಪೂರ್ಣಾಙ್ಕ) ಪರಿಮಾಣ(s));
        ಯದಿ (r != Z_OK)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        ಯದಿ (r != Z_STREAM_END || produced != dst_size)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಪ್ರತ್ಯಾಗಮ 0;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_startswith_xz(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **buffer,
                ಧ್ರುವ ಶೂನ್ಯ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(prefix);

#ಯದಿ HAVE_XZ
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_xz(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ಯದಿ (ret != LZMA_OK)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        ಪ್ರತಿ (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                ಯದಿ (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಯದಿ (allocated - s.avail_out >= prefix_len + 1)
                        ಪ್ರತ್ಯಾಗಮ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ಧ್ರುವ uint8_t*) *buffer)[prefix_len] == extra;

                ಯದಿ (ret == LZMA_STREAM_END)
                        ಪ್ರತ್ಯಾಗಮ 0;

                s.avail_out += allocated;

                ಯದಿ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_startswith_lz4(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **buffer,
                ಧ್ರುವ ಶೂನ್ಯ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(prefix);

#ಯದಿ HAVE_LZ4
        size_t allocated;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_lz4(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size <= 8)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (src_size - 8 > INT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;

        ಯದಿ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (ವರ್ಣ*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        ಯದಿ (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                ಯದಿ (sym_LZ4_version_ಸಂಖ್ಯೆ() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಯದಿ (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        ಯದಿ (memcmp(*buffer, prefix, r) != 0)
                                ಪ್ರತ್ಯಾಗಮ 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (size < prefix_len + 1)
                        ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ಧ್ರುವ uint8_t*) *buffer)[prefix_len] == extra;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_startswith_zstd(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **buffer,
                ಧ್ರುವ ಶೂನ್ಯ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(prefix);

#ಯದಿ HAVE_ZSTD
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_zstd(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        ಯದಿ (size == ZSTD_CONTENTSIZE_ERROR)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        ಯದಿ (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                ಪ್ರತ್ಯಾಗಮ 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ಯದಿ (!dctx)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ZSTD_inBuffer ಒಳಹರಿವು = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &ಒಳಹರಿವು);
        ಯದಿ (sym_ZSTD_isError(k))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        ಯದಿ (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                ಯದಿ (k != 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                ಪ್ರತ್ಯಾಗಮ 0; /* Decompressed text too short to match the prefix and extra */
        }

        ಪ್ರತ್ಯಾಗಮ memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ಧ್ರುವ uint8_t*) *buffer)[prefix_len] == extra;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_startswith_gzip(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **buffer,
                ಧ್ರುವ ಶೂನ್ಯ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(prefix);

#ಯದಿ HAVE_ZLIB
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_zlib(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size > UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ಪೂರ್ಣಾಙ್ಕ) ಪರಿಮಾಣ(s));
        ಯದಿ (r != Z_OK)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (ಶೂನ್ಯ*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        ಪ್ರತಿ (;;) {
                r = sym_inflate(&s, Z_FINISH);

                ಯದಿ (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಯದಿ (allocated - s.avail_out >= prefix_len + 1)
                        ಪ್ರತ್ಯಾಗಮ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ಧ್ರುವ uint8_t*) *buffer)[prefix_len] == extra;

                ಯದಿ (r == Z_STREAM_END)
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಯದಿ (s.avail_out > 0)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                ಯದಿ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_startswith_bzip2(
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಶೂನ್ಯ **buffer,
                ಧ್ರುವ ಶೂನ್ಯ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_size > 0);
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(prefix);

#ಯದಿ HAVE_BZIP2
        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (src_size > UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        ಯದಿ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಯದಿ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (ವರ್ಣ*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        ಪ್ರತಿ (;;) {
                r = sym_BZ2_bzDecompress(&s);

                ಯದಿ (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಯದಿ (allocated - s.avail_out >= prefix_len + 1)
                        ಪ್ರತ್ಯಾಗಮ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ಧ್ರುವ uint8_t*) *buffer)[prefix_len] == extra;

                ಯದಿ (r == BZ_STREAM_END)
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಯದಿ (s.avail_out > 0)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                ಯದಿ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (ವರ್ಣ*) *buffer + used;
        }
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;
#endif
}

ಪೂರ್ಣಾಙ್ಕ decompress_startswith_journal(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                ಶೂನ್ಯ **buffer,
                ಧ್ರುವ ಶೂನ್ಯ *prefix, size_t prefix_len,
                uint8_t extra) {

        ವಿಕಲ್ಪ (compression) {
        ಪಕ್ಷ COMPRESSION_XZ:
                ಪ್ರತ್ಯಾಗಮ decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        ಪಕ್ಷ COMPRESSION_LZ4:
                ಪ್ರತ್ಯಾಗಮ decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        ಪಕ್ಷ COMPRESSION_ZSTD:
                ಪ್ರತ್ಯಾಗಮ decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        }
}

ಪೂರ್ಣಾಙ್ಕ decompress_startswith(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                ಶೂನ್ಯ **buffer,
                ಧ್ರುವ ಶೂನ್ಯ *prefix, size_t prefix_len,
                uint8_t extra) {

        ವಿಕಲ್ಪ (compression) {
        ಪಕ್ಷ COMPRESSION_GZIP:
                ಪ್ರತ್ಯಾಗಮ decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        ಪಕ್ಷ COMPRESSION_BZIP2:
                ಪ್ರತ್ಯಾಗಮ decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

ಪೂರ್ಣಾಙ್ಕ compress_stream(
                Compression type,
                ಪೂರ್ಣಾಙ್ಕ fdf, ಪೂರ್ಣಾಙ್ಕ fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = ರಿಕ್ತ;
        _cleanup_free_ ಶೂನ್ಯ *buf = ರಿಕ್ತ;
        _cleanup_free_ uint8_t *ಒಳಹರಿವು = ರಿಕ್ತ;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fdf >= 0);
        ಪ್ರತಿಜ್ಞಾ(fdt >= 0);

        r = compressor_new(&c, type);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಒಳಹರಿವು = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        ಯದಿ (!ಒಳಹರಿವು)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                ಯದಿ (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = ಪಠ(fdf, ಒಳಹರಿವು, m);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        ಯದಿ (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                                total_out += buf_size;
                        }
                        ಭಙ್ಗ;
                }

                total_in += n;
                ಯದಿ (max_bytes != UINT64_MAX) {
                        ಪ್ರತಿಜ್ಞಾ(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, ಒಳಹರಿವು, n, &buf, &buf_size, &buf_alloc);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        total_out += buf_size;
                }
        }

        ಯದಿ (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        ಯದಿ (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        ಅನ್ಯಥಾ
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (ದ್ವಿಗುಣ) total_out / total_in * 100);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ should_sparse(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಸಂರಚನಾ stat st;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (!S_ISREG(st.st_mode))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪೂರ್ಣಾಙ್ಕ flags = fcntl(fd, F_GETFL);
        ಯದಿ (flags < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (FLAGS_SET(flags, O_APPEND))
                ಪ್ರತ್ಯಾಗಮ false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        ಯದಿ (pos < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ finalize_sparse(ಪೂರ್ಣಾಙ್ಕ fd) {
        off_t pos;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        ಯದಿ (pos < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ftruncate(fd, pos) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 0;
}

/* Common helper for decompress_stream_*() wrappers */

ಸಂರಚನಾ decompress_stream_userdata {
        ಪೂರ್ಣಾಙ್ಕ fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_stream_write_callback(ಧ್ರುವ ಶೂನ್ಯ *data, size_t size, ಶೂನ್ಯ *userdata) {
        ಸಂರಚನಾ decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        ಯದಿ (u->max_bytes != UINT64_MAX) {
                ಯದಿ (u->max_bytes < size)
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        ಯದಿ (u->skip > 0) {
                ಯದಿ (u->skip >= size) {
                        u->skip -= size;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                data = (ಧ್ರುವ uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        ಯದಿ (size == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                ಯದಿ (k < 0)
                        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) k;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ loop_write(u->fd, data, size);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        ವಿಕಲ್ಪ (type) {
        ಪಕ್ಷ COMPRESSION_NONE:
                ಭಙ್ಗ;

#ಯದಿ HAVE_XZ
        ಪಕ್ಷ COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                ಯದಿ (xz_ret != LZMA_OK)
                        ಪ್ರತ್ಯಾಗಮ -EIO;
                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_LZ4
        ಪಕ್ಷ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                ಯದಿ (sym_LZ4F_isError(rc))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_ZSTD
        ಪಕ್ಷ COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                ಯದಿ (!c->d_zstd)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_ZLIB
        ಪಕ್ಷ COMPRESSION_GZIP:
                ಯದಿ (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ಪೂರ್ಣಾಙ್ಕ) ಪರಿಮಾಣ(c->gzip)) != Z_OK)
                        ಪ್ರತ್ಯಾಗಮ -EIO;
                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_BZIP2
        ಪಕ್ಷ COMPRESSION_BZIP2:
                ಯದಿ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        ಪ್ರತ್ಯಾಗಮ -EIO;
                ಭಙ್ಗ;
#endif

        ಸ್ವಭಾವ:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ decompress_blob_to_fd_impl(
                Compression type,
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಪೂರ್ಣಾಙ್ಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(fdt >= 0);

        ಯದಿ (type == COMPRESSION_NONE)
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        ಯದಿ (src_size == 0)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        ಯದಿ (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                ಯದಿ (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        ಯದಿ (src_size <= ಪರಿಮಾಣ(uint64_t))
                                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        ಯದಿ (sz < skip)
                                ಪ್ರತ್ಯಾಗಮ -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                ಯದಿ (max_bytes == 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                _cleanup_free_ ಶೂನ್ಯ *buf = ರಿಕ್ತ;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* Check the result size again for safety. */
                ಯದಿ (size < skip)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (ದ್ವಿಗುಣ) size / src_size * 100);

                ಯದಿ (ret_size)
                        *ret_size = size - skip;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = ರಿಕ್ತ;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಸಂರಚನಾ decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ತಳ್ಳು(c, src, src_size, decompress_stream_write_callback, &userdata);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Finalize */
        r = decompressor_ತಳ್ಳು(c, /* data= */ ರಿಕ್ತ, /* size= */ 0, decompress_stream_write_callback, &userdata);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (userdata.total_out < skip)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (userdata.sparse) {
                r = finalize_sparse(fdt);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (ದ್ವಿಗುಣ) userdata.total_out / src_size * 100);

        ಯದಿ (ret_size)
                *ret_size = userdata.total_out - skip;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ decompress_blob_to_fd_journal(
                Compression type,
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಪೂರ್ಣಾಙ್ಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

ಪೂರ್ಣಾಙ್ಕ decompress_blob_to_fd(
                Compression type,
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಪೂರ್ಣಾಙ್ಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ಪೂರ್ಣಾಙ್ಕ r;

        r = dlopen_compress(type, LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

ಪೂರ್ಣಾಙ್ಕ decompress_stream_full(
                Compression type,
                ಪೂರ್ಣಾಙ್ಕ fdf, ಪೂರ್ಣಾಙ್ಕ fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = ರಿಕ್ತ;
        _cleanup_free_ uint8_t *buf = ರಿಕ್ತ;
        uint64_t total_in = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fdf >= 0);
        ಪ್ರತಿಜ್ಞಾ(fdt >= 0);

        ಯದಿ (type == COMPRESSION_NONE)
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = decompressor_new(&c, type, flags);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಸಂರಚನಾ decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (;;) {
                ssize_t n;

                n = ಪಠ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                total_in += n;

                r = decompressor_ತಳ್ಳು(c, buf, n, decompress_stream_write_callback, &userdata);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (n == 0)
                        ಭಙ್ಗ;
        }

        ಯದಿ (total_in == 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        ಯದಿ (userdata.sparse) {
                r = finalize_sparse(fdt);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (ದ್ವಿಗುಣ) userdata.total_out / total_in * 100);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ decompress_stream_by_filename(ಧ್ರುವ ವರ್ಣ *filename, ಪೂರ್ಣಾಙ್ಕ fdf, ಪೂರ್ಣಾಙ್ಕ fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        ಯದಿ (c == COMPRESSION_NONE)
                ಪ್ರತ್ಯಾಗಮ -EPROTONOSUPPORT;

        ಪ್ರತ್ಯಾಗಮ decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ವಿಕಲ್ಪ (c->type) {

#ಯದಿ HAVE_XZ
        ಪಕ್ಷ COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_LZ4
        ಪಕ್ಷ COMPRESSION_LZ4:
                ಯದಿ (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = ರಿಕ್ತ;
                        c->lz4_header = mfree(c->lz4_header);
                } ಅನ್ಯಥಾ {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = ರಿಕ್ತ;
                }
                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_ZSTD
        ಪಕ್ಷ COMPRESSION_ZSTD:
                ಯದಿ (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = ರಿಕ್ತ;
                } ಅನ್ಯಥಾ {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = ರಿಕ್ತ;
                }
                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_ZLIB
        ಪಕ್ಷ COMPRESSION_GZIP:
                ಯದಿ (c->encoding)
                        sym_deflateEnd(&c->gzip);
                ಅನ್ಯಥಾ
                        sym_inflateEnd(&c->gzip);
                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_BZIP2
        ಪಕ್ಷ COMPRESSION_BZIP2:
                ಯದಿ (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                ಅನ್ಯಥಾ
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                ಭಙ್ಗ;
#endif

        ಸ್ವಭಾವ:
                ಭಙ್ಗ;
        }

        ಪ್ರತ್ಯಾಗಮ mfree(c);
}

Compression compressor_type(ಧ್ರುವ Compressor *c) {
        ಪ್ರತ್ಯಾಗಮ c ? c->type : _COMPRESSION_INVALID;
}

ಪೂರ್ಣಾಙ್ಕ decompressor_detect(Decompressor **decompressor, ಧ್ರುವ ಶೂನ್ಯ *data, size_t size) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(decompressor);

        ಯದಿ (*decompressor)
                ಪ್ರತ್ಯಾಗಮ 1;

        ಯದಿ (size < COMPRESSION_MAGIC_BYTES_MAX)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತಿಜ್ಞಾ(data);

        Compression type = compression_detect_from_magic(data);
        ಯದಿ (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ decompressor_force(Decompressor **decompressor, Compression type) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(decompressor);
        ಪ್ರತಿಜ್ಞಾ(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *decompressor = compressor_free(*decompressor);
        ಪ್ರತ್ಯಾಗಮ decompressor_new(decompressor, type, /* flags= */ 0);
}

ಪೂರ್ಣಾಙ್ಕ decompressor_force_off(Decompressor **decompressor) {
        ಪ್ರತ್ಯಾಗಮ decompressor_force(decompressor, COMPRESSION_NONE);
}

ಪೂರ್ಣಾಙ್ಕ decompressor_ತಳ್ಳು(Decompressor *c, ಧ್ರುವ ಶೂನ್ಯ *data, size_t size, DecompressorCallback callback, ಶೂನ್ಯ *userdata) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(c);
        ಪ್ರತಿಜ್ಞಾ(data || size == 0);
        ಪ್ರತಿಜ್ಞಾ(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        ಯದಿ (c->encoding)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = ರಿಕ್ತ;
        ಯದಿ (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                ಯದಿ (!buffer)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }
#endif

        ವಿಕಲ್ಪ (c->type) {

        ಪಕ್ಷ COMPRESSION_NONE:
                ಯದಿ (c->finished)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಯದಿ (size == 0) {
                        c->finished = true;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                r = callback(data, size, userdata);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ HAVE_XZ
        ಪಕ್ಷ COMPRESSION_XZ:
                ಯದಿ (c->finished)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                ಪ್ರತಿ (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        ಯದಿ (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                        ಯದಿ (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        ಯದಿ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                        }

                        ಯದಿ (lzr == LZMA_STREAM_END) {
                                ಯದಿ (c->xz.avail_in > 0)
                                        ಪ್ರತ್ಯಾಗಮ -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                ಪ್ರತ್ಯಾಗಮ 0;
                        }

                        ಯದಿ (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                ಯದಿ (size == 0)
                                        ಪ್ರತ್ಯಾಗಮ -EBADMSG; /* truncated */

                                ಪ್ರತ್ಯಾಗಮ 0; /* No progress possible with the current input. */
                        }
                }
#endif

#ಯದಿ HAVE_LZ4
        ಪಕ್ಷ COMPRESSION_LZ4: {
                ಧ್ರುವ uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                ಪ್ರತಿ (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, ರಿಕ್ತ);
                        ಯದಿ (sym_LZ4F_isError(rc))
                                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                        ಯದಿ (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        ಯದಿ (consumed > 0) {
                                ಪ್ರತಿಜ್ಞಾ(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        ಯದಿ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                        }

                        ಯದಿ (src_remaining == 0) {
                                ಯದಿ (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        ಪ್ರತ್ಯಾಗಮ 0;
                                }

                                ಯದಿ (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ಅನುವರ್ತ;

                                ಯದಿ (size == 0)
                                        ಪ್ರತ್ಯಾಗಮ -EBADMSG; /* truncated */

                                ಪ್ರತ್ಯಾಗಮ 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#ಯದಿ HAVE_ZSTD
        ಪಕ್ಷ COMPRESSION_ZSTD: {
                ZSTD_inBuffer ಒಳಹರಿವು = {
                        .src =  (ಶೂನ್ಯ*) data,
                        .size = size,
                };

                c->finished = false;

                ಪ್ರತಿ (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &ಒಳಹರಿವು);
                        ಯದಿ (sym_ZSTD_isError(res))
                                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                        ಯದಿ (ಒಳಹರಿವು.pos > ಒಳಹರಿವು.size ||
                            output.pos > output.size)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        ಯದಿ (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                        }

                        ಯದಿ (ಒಳಹರಿವು.pos == ಒಳಹರಿವು.size) {
                                ಯದಿ (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        ಪ್ರತ್ಯಾಗಮ 0;
                                }

                                ಯದಿ (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ಅನುವರ್ತ;

                                ಯದಿ (size == 0)
                                        ಪ್ರತ್ಯಾಗಮ -EBADMSG; /* truncated */

                                ಪ್ರತ್ಯಾಗಮ 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#ಯದಿ HAVE_ZLIB
        ಪಕ್ಷ COMPRESSION_GZIP:
                ಯದಿ (size > UINT_MAX)
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;

                c->gzip.next_in = (ಶೂನ್ಯ*) data;
                c->gzip.avail_in = size;

                ಪ್ರತಿ (;;) {
                        ಯದಿ (c->finished) {
                                ಯದಿ (sym_inflateReset(&c->gzip) != Z_OK)
                                        ಪ್ರತ್ಯಾಗಮ -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        ಪೂರ್ಣಾಙ್ಕ zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        ಯದಿ (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                        ಯದಿ (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        ಯದಿ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                        }

                        ಯದಿ (zr == Z_STREAM_END) {
                                c->finished = true;

                                ಯದಿ (c->gzip.avail_in > 0)
                                        ಅನುವರ್ತ; /* New stream may be pending. */

                                ಪ್ರತ್ಯಾಗಮ 0;
                        }

                        ಯದಿ (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                ಯದಿ (size == 0)
                                        ಪ್ರತ್ಯಾಗಮ -EBADMSG; /* truncated */

                                ಪ್ರತ್ಯಾಗಮ 0; /* No progress possible with the current input. */
                        }
                }
#endif

#ಯದಿ HAVE_BZIP2
        ಪಕ್ಷ COMPRESSION_BZIP2:
                ಯದಿ (size > UINT_MAX)
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;

                c->bzip2.next_in = (ವರ್ಣ*) data;
                c->bzip2.avail_in = size;

                ಪ್ರತಿ (;;) {
                        ಯದಿ (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                ಯದಿ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        ಪ್ರತ್ಯಾಗಮ -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (ವರ್ಣ*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        ಪೂರ್ಣಾಙ್ಕ bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        ಯದಿ (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                        ಯದಿ (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        ಯದಿ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                        }

                        ಯದಿ (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                ಯದಿ (c->bzip2.avail_in > 0)
                                        ಅನುವರ್ತ; /* New stream may be pending. */

                                ಪ್ರತ್ಯಾಗಮ 0;
                        }

                        ಯದಿ (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                ಯದಿ (size == 0)
                                        ಪ್ರತ್ಯಾಗಮ -EBADMSG; /* truncated */

                                ಪ್ರತ್ಯಾಗಮ 0; /* No progress possible with the current input. */
                        }
                }
#endif

        ಸ್ವಭಾವ:
                assert_not_reached();
        }
}

ಪೂರ್ಣಾಙ್ಕ compressor_new(Compressor **ret, Compression type) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        ವಿಕಲ್ಪ (type) {

#ಯದಿ HAVE_XZ
        ಪಕ್ಷ COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                ಯದಿ (xzr != LZMA_OK)
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                c->type = COMPRESSION_XZ;
                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_LZ4
        ಪಕ್ಷ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                ಯದಿ (sym_LZ4F_isError(rc))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                ಯದಿ (!c->lz4_header)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                ಯದಿ (sym_LZ4F_isError(c->lz4_header_size))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_ZSTD
        ಪಕ್ಷ COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                ಯದಿ (!c->c_zstd)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                ಯದಿ (sym_ZSTD_isError(z))
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                ಯದಿ (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_ZLIB
        ಪಕ್ಷ COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (ಪೂರ್ಣಾಙ್ಕ) ಪರಿಮಾಣ(c->gzip));
                ಯದಿ (r != Z_OK)
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                c->type = COMPRESSION_GZIP;
                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_BZIP2
        ಪಕ್ಷ COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                ಯದಿ (r != BZ_OK)
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                c->type = COMPRESSION_BZIP2;
                ಭಙ್ಗ;
#endif

        ಪಕ್ಷ COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                ಭಙ್ಗ;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        ಪ್ರತ್ಯಾಗಮ 0;
}

#ಯದಿ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ enlarge_buffer(ಶೂನ್ಯ **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(buffer_size);
        ಪ್ರತಿಜ್ಞಾ(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        ಯದಿ (*buffer_allocated >= need)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (!greedy_realloc(buffer, need, 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        ಪ್ರತ್ಯಾಗಮ 1;
}
#endif

ಪೂರ್ಣಾಙ್ಕ compressor_start(
                Compressor *c,
                ಧ್ರುವ ಶೂನ್ಯ *data,
                size_t size,
                ಶೂನ್ಯ **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#ಯದಿ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ಪೂರ್ಣಾಙ್ಕ r;
#endif

        ಪ್ರತಿಜ್ಞಾ(c);
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(buffer_size);
        ಪ್ರತಿಜ್ಞಾ(buffer_allocated);

        ಯದಿ (!c->encoding)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (size == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತಿಜ್ಞಾ(data);

        *buffer_size = 0;

        ವಿಕಲ್ಪ (c->type) {

#ಯದಿ HAVE_XZ
        ಪಕ್ಷ COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                ಯಾವತ್ (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        ಯದಿ (lzr != LZMA_OK)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_LZ4
        ಪಕ್ಷ COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                ಯದಿ (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, ರಿಕ್ತ);
                ಯದಿ (sym_LZ4F_isError(n))
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                *buffer_size += n;
                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_ZSTD
        ಪಕ್ಷ COMPRESSION_ZSTD: {
                ZSTD_inBuffer ಒಳಹರಿವು = {
                        .src = data,
                        .size = size,
                };

                ಯಾವತ್ (ಒಳಹರಿವು.pos < ಒಳಹರಿವು.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ಒಳಹರಿವು, ZSTD_e_continue);
                        ಯದಿ (sym_ZSTD_isError(res))
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        *buffer_size += output.pos;
                }

                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_ZLIB
        ಪಕ್ಷ COMPRESSION_GZIP:
                ಯದಿ (size > UINT_MAX)
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;

                c->gzip.next_in = (ಶೂನ್ಯ*) data;
                c->gzip.avail_in = size;

                ಯಾವತ್ (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        ಯದಿ (r != Z_OK)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_BZIP2
        ಪಕ್ಷ COMPRESSION_BZIP2:
                ಯದಿ (size > UINT_MAX)
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;

                c->bzip2.next_in = (ಶೂನ್ಯ*) data;
                c->bzip2.avail_in = size;

                ಯಾವತ್ (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (ಶೂನ್ಯ*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        ಯದಿ (r != BZ_RUN_OK)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                ಭಙ್ಗ;
#endif

        ಪಕ್ಷ COMPRESSION_NONE:

                ಯದಿ (*buffer_allocated < size) {
                        ಶೂನ್ಯ *p;

                        p = realloc(*buffer, size);
                        ಯದಿ (!p)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                ಭಙ್ಗ;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ compressor_finish(Compressor *c, ಶೂನ್ಯ **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#ಯದಿ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ಪೂರ್ಣಾಙ್ಕ r;
#endif

        ಪ್ರತಿಜ್ಞಾ(c);
        ಪ್ರತಿಜ್ಞಾ(buffer);
        ಪ್ರತಿಜ್ಞಾ(buffer_size);
        ಪ್ರತಿಜ್ಞಾ(buffer_allocated);

        ಯದಿ (!c->encoding)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *buffer_size = 0;

        ವಿಕಲ್ಪ (c->type) {

#ಯದಿ HAVE_XZ
        ಪಕ್ಷ COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                ಕುರು {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        ಯದಿ (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } ಯಾವತ್ (lzr != LZMA_STREAM_END);

                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_LZ4
        ಪಕ್ಷ COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, ರಿಕ್ತ);
                ಯದಿ (sym_LZ4F_isError(n))
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                *buffer_size = n;
                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_ZSTD
        ಪಕ್ಷ COMPRESSION_ZSTD: {
                ZSTD_inBuffer ಒಳಹರಿವು = {};
                size_t res;

                ಕುರು {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ಒಳಹರಿವು, ZSTD_e_end);
                        ಯದಿ (sym_ZSTD_isError(res))
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        *buffer_size += output.pos;
                } ಯಾವತ್ (res != 0);

                ಭಙ್ಗ;
        }
#endif

#ಯದಿ HAVE_ZLIB
        ಪಕ್ಷ COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                ಕುರು {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        ಯದಿ (!IN_SET(r, Z_OK, Z_STREAM_END))
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } ಯಾವತ್ (r != Z_STREAM_END);

                ಭಙ್ಗ;
#endif

#ಯದಿ HAVE_BZIP2
        ಪಕ್ಷ COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                ಕುರು {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (ಶೂನ್ಯ*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        ಯದಿ (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } ಯಾವತ್ (r != BZ_STREAM_END);

                ಭಙ್ಗ;
#endif

        ಪಕ್ಷ COMPRESSION_NONE:
                ಭಙ್ಗ;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}
