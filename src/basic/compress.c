/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#എങ്കിൽ HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#അന്തര്ഭാവ <lzma.h>
#endif

#എങ്കിൽ HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#അന്തര്ഭാവ <lz4.h>
#അന്തര്ഭാവ <lz4frame.h>
#അന്തര്ഭാവ <lz4hc.h>
#endif

#എങ്കിൽ HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#അന്തര്ഭാവ <zstd.h>
#അന്തര്ഭാവ <zstd_errors.h>
#endif

#എങ്കിൽ HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#അന്തര്ഭാവ <zlib.h>
#endif

#എങ്കിൽ HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#അന്തര്ഭാവ <bzlib.h>
#endif

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "bitfield.h"
#അന്തര്ഭാവ "compress.h"
#അന്തര്ഭാവ "dlfcn-util.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "unaligned.h"

#എങ്കിൽ HAVE_XZ
സ്ഥിതം DLSYM_PROTOTYPE(lzma_auto_decoder) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(lzma_code) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(lzma_easy_encoder) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(lzma_end) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(lzma_stream_decoder) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(lzma_lzma_preset) = ശൂന്യ;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
സ്ഥിതം വരിയിൽ ശൂന്യം lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#എങ്കിൽ HAVE_LZ4
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_compressBegin) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_compressBound) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_compressEnd) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_compressUpdate) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_decompress) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4F_isError) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4_compress_HC) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4_compress_default) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4_decompress_safe) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(LZ4_version_സംഖ്യ) = ശൂന്യ;

സ്ഥിതം സ്ഥിരം LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#എങ്കിൽ HAVE_ZSTD
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_compress) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_compressStream2) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_createCCtx) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_createDCtx) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_decompressStream) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_freeCCtx) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_freeDCtx) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_getErrorCode) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_getErrorName) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(ZSTD_isError) = ശൂന്യ;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, ശൂന്യ);

സ്ഥിതം പൂർണ്ണസംഖ്യ zstd_ret_to_errno(size_t ret) {
        തിരഞ്ഞെടുപ്പ് (sym_ZSTD_getErrorCode(ret)) {
        അവസ്ഥ ZSTD_error_dstSize_tooSmall:
                തിരികെ -ENOBUFS;
        അവസ്ഥ ZSTD_error_memory_allocation:
                തിരികെ -ENOMEM;
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EBADMSG;
        }
}
#endif

#എങ്കിൽ HAVE_ZLIB
സ്ഥിതം DLSYM_PROTOTYPE(deflateInit2_) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(deflate) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(deflateEnd) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(inflateInit2_) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(inflate) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(inflateEnd) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(inflateReset) = ശൂന്യ;

സ്ഥിതം വരിയിൽ ശൂന്യം deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

സ്ഥിതം വരിയിൽ ശൂന്യം inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#എങ്കിൽ HAVE_BZIP2
സ്ഥിതം DLSYM_PROTOTYPE(BZ2_bzCompressInit) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(BZ2_bzCompress) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(BZ2_bzDecompress) = ശൂന്യ;
സ്ഥിതം DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = ശൂന്യ;

സ്ഥിതം വരിയിൽ ശൂന്യം BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

സ്ഥിതം വരിയിൽ ശൂന്യം BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
ഘടന Compressor {
        Compression type;
        bool encoding;
        bool finished;
        സംഘം {
#എങ്കിൽ HAVE_XZ
                lzma_stream xz;
#endif
#എങ്കിൽ HAVE_LZ4
                ഘടന {
                        LZ4F_compressionContext_t c_lz4;
                        ശൂന്യം *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#എങ്കിൽ HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#എങ്കിൽ HAVE_ZLIB
                z_stream gzip;
#endif
#എങ്കിൽ HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, വലുപ്പം(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(സ്ഥിരം അക്ഷരം *s) {
        Compression c;

        പ്രതിജ്ഞാ(s);

        c = compression_from_string(s);
        എങ്കിൽ (c >= 0)
                തിരികെ c;

        തിരികെ compression_uppercase_from_string(s);
}

Compression compression_from_filename(സ്ഥിരം അക്ഷരം *filename) {
        Compression c;
        സ്ഥിരം അക്ഷരം *e;

        പ്രതിജ്ഞാ(filename);

        e = strrchr(filename, '.');
        എങ്കിൽ (!e)
                തിരികെ COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        എങ്കിൽ (c < 0)
                തിരികെ COMPRESSION_NONE;

        തിരികെ c;
}

bool compression_supported_journal(Compression c) {
        സ്ഥിതം സ്ഥിരം unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        പ്രതിജ്ഞാ(c >= 0);
        പ്രതിജ്ഞാ(c < _COMPRESSION_MAX);

        തിരികെ BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        സ്ഥിതം സ്ഥിരം unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        പ്രതിജ്ഞാ(c >= 0);
        പ്രതിജ്ഞാ(c < _COMPRESSION_MAX);

        തിരികെ BIT_SET(supported, c);
}

Compression compression_detect_from_magic(സ്ഥിരം uint8_t data[സ്ഥിതം COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        എങ്കിൽ (memcmp(data, (സ്ഥിരം uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                തിരികെ COMPRESSION_GZIP;
        എങ്കിൽ (memcmp(data, (സ്ഥിരം uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                തിരികെ COMPRESSION_XZ;
        എങ്കിൽ (memcmp(data, (സ്ഥിരം uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                തിരികെ COMPRESSION_ZSTD;
        എങ്കിൽ (memcmp(data, (സ്ഥിരം uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                തിരികെ COMPRESSION_LZ4;
        എങ്കിൽ (memcmp(data, (സ്ഥിരം uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                തിരികെ COMPRESSION_BZIP2;

        തിരികെ _COMPRESSION_INVALID;
}

പൂർണ്ണസംഖ്യ dlopen_xz(പൂർണ്ണസംഖ്യ log_level) {
#എങ്കിൽ HAVE_XZ
        സ്ഥിതം ശൂന്യം *lzma_dl = ശൂന്യ;

        LIBLZMA_NOTE(suggested);

        തിരികെ dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#അല്ലെങ്കിൽ
        തിരികെ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

പൂർണ്ണസംഖ്യ dlopen_lz4(പൂർണ്ണസംഖ്യ log_level) {
#എങ്കിൽ HAVE_LZ4
        സ്ഥിതം ശൂന്യം *lz4_dl = ശൂന്യ;

        LIBLZ4_NOTE(suggested);

        തിരികെ dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_സംഖ്യ));
#അല്ലെങ്കിൽ
        തിരികെ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

പൂർണ്ണസംഖ്യ dlopen_zstd(പൂർണ്ണസംഖ്യ log_level) {
#എങ്കിൽ HAVE_ZSTD
        സ്ഥിതം ശൂന്യം *zstd_dl = ശൂന്യ;

        LIBZSTD_NOTE(suggested);

        തിരികെ dlopen_many_sym_or_warn(
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
#അല്ലെങ്കിൽ
        തിരികെ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

പൂർണ്ണസംഖ്യ dlopen_zlib(പൂർണ്ണസംഖ്യ log_level) {
#എങ്കിൽ HAVE_ZLIB
        സ്ഥിതം ശൂന്യം *zlib_dl = ശൂന്യ;

        LIBZ_NOTE(suggested);

        തിരികെ dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#അല്ലെങ്കിൽ
        തിരികെ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

പൂർണ്ണസംഖ്യ dlopen_bzip2(പൂർണ്ണസംഖ്യ log_level) {
#എങ്കിൽ HAVE_BZIP2
        സ്ഥിതം ശൂന്യം *bzip2_dl = ശൂന്യ;

        LIBBZ2_NOTE(suggested);

        തിരികെ dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#അല്ലെങ്കിൽ
        തിരികെ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

പൂർണ്ണസംഖ്യ dlopen_compress_journal(Compression c, പൂർണ്ണസംഖ്യ log_level) {
        തിരഞ്ഞെടുപ്പ് (c) {
        അവസ്ഥ COMPRESSION_NONE:
                തിരികെ 0;
        അവസ്ഥ COMPRESSION_XZ:
                തിരികെ dlopen_xz(log_level);
        അവസ്ഥ COMPRESSION_LZ4:
                തിരികെ dlopen_lz4(log_level);
        അവസ്ഥ COMPRESSION_ZSTD:
                തിരികെ dlopen_zstd(log_level);
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EOPNOTSUPP;
        }
}

പൂർണ്ണസംഖ്യ dlopen_compress(Compression c, പൂർണ്ണസംഖ്യ log_level) {
        തിരഞ്ഞെടുപ്പ് (c) {
        അവസ്ഥ COMPRESSION_GZIP:
                തിരികെ dlopen_zlib(log_level);
        അവസ്ഥ COMPRESSION_BZIP2:
                തിരികെ dlopen_bzip2(log_level);
        സ്ഥിരസ്ഥിതി:
                തിരികെ dlopen_compress_journal(c, log_level);
        }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compress_blob_xz(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                പൂർണ്ണസംഖ്യ level) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_alloc_size > 0);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, ശൂന്യ, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, ശൂന്യ }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_xz(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                എങ്കിൽ (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        തിരികെ -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        എങ്കിൽ (src_size < 80)
                തിരികെ -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ ശൂന്യ,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        എങ്കിൽ (ret != LZMA_OK)
                തിരികെ -ENOBUFS;

        *dst_size = out_pos;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compress_blob_lz4(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                പൂർണ്ണസംഖ്യ level) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_alloc_size > 0);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_LZ4
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_lz4(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        എങ്കിൽ (src_size < 9)
                തിരികെ -ENOBUFS;

        എങ്കിൽ (src_size > INT_MAX)
                തിരികെ -EFBIG;
        എങ്കിൽ (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        എങ്കിൽ (level <= 0)
                r = sym_LZ4_compress_default(src, (അക്ഷരം*)dst + 8, src_size, (പൂർണ്ണസംഖ്യ) dst_alloc_size - 8);
        അല്ലെങ്കിൽ
                r = sym_LZ4_compress_HC(src, (അക്ഷരം*)dst + 8, src_size, (പൂർണ്ണസംഖ്യ) dst_alloc_size - 8, level);
        എങ്കിൽ (r <= 0)
                തിരികെ -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compress_blob_zstd(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                പൂർണ്ണസംഖ്യ level) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_alloc_size > 0);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_ZSTD
        size_t k;
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_zstd(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        എങ്കിൽ (sym_ZSTD_isError(k))
                തിരികെ zstd_ret_to_errno(k);

        *dst_size = k;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compress_blob_gzip(സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                       ശൂന്യം *dst, size_t dst_alloc_size, size_t *dst_size, പൂർണ്ണസംഖ്യ level) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_alloc_size > 0);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_ZLIB
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_zlib(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size > UINT_MAX)
                തിരികെ -EFBIG;
        എങ്കിൽ (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (പൂർണ്ണസംഖ്യ) വലുപ്പം(s));
        എങ്കിൽ (r != Z_OK)
                തിരികെ -ENOMEM;

        s.next_in = (ശൂന്യം*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        എങ്കിൽ (r != Z_STREAM_END)
                തിരികെ -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compress_blob_bzip2(
                സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                ശൂന്യം *dst, size_t dst_alloc_size, size_t *dst_size, പൂർണ്ണസംഖ്യ level) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_alloc_size > 0);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_BZIP2
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_bzip2(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size > UINT_MAX)
                തിരികെ -EFBIG;
        എങ്കിൽ (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        എങ്കിൽ (r != BZ_OK)
                തിരികെ -ENOMEM;

        s.next_in = (അക്ഷരം*) src;
        s.avail_in = src_size;
        s.next_out = (അക്ഷരം*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        എങ്കിൽ (r != BZ_STREAM_END)
                തിരികെ -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

പൂർണ്ണസംഖ്യ compress_blob_journal(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                ശൂന്യം *dst, size_t dst_alloc_size, size_t *dst_size, പൂർണ്ണസംഖ്യ level) {

        തിരഞ്ഞെടുപ്പ് (compression) {
        അവസ്ഥ COMPRESSION_XZ:
                തിരികെ compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        അവസ്ഥ COMPRESSION_LZ4:
                തിരികെ compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        അവസ്ഥ COMPRESSION_ZSTD:
                തിരികെ compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EOPNOTSUPP;
        }
}

പൂർണ്ണസംഖ്യ compress_blob(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                ശൂന്യം *dst, size_t dst_alloc_size, size_t *dst_size, പൂർണ്ണസംഖ്യ level) {

        തിരഞ്ഞെടുപ്പ് (compression) {
        അവസ്ഥ COMPRESSION_GZIP:
                തിരികെ compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        അവസ്ഥ COMPRESSION_BZIP2:
                തിരികെ compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        സ്ഥിരസ്ഥിതി:
                തിരികെ compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_blob_xz(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **dst,
                size_t *dst_size,
                size_t dst_max) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_XZ
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_xz(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        എങ്കിൽ (ret != LZMA_OK)
                തിരികെ -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        എങ്കിൽ (!greedy_realloc(dst, space, 1))
                തിരികെ -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        വേണ്ടി (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                എങ്കിൽ (ret == LZMA_STREAM_END)
                        നിർത്തുക;
                എങ്കിൽ (ret != LZMA_OK)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        നിർത്തുക;
                എങ്കിൽ (dst_max > 0 && space == dst_max)
                        തിരികെ -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                പ്രതിജ്ഞാ(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                എങ്കിൽ (!greedy_realloc(dst, space, 1))
                        തിരികെ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_blob_lz4(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **dst,
                size_t *dst_size,
                size_t dst_max) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_LZ4
        അക്ഷരം* out;
        പൂർണ്ണസംഖ്യ r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size <= 8)
                തിരികെ -EBADMSG;

        എങ്കിൽ (src_size - 8 > INT_MAX)
                തിരികെ -EFBIG;

        size = unaligned_read_le64(src);
        എങ്കിൽ (size < 0 || (unsigned) size != unaligned_read_le64(src))
                തിരികെ -EFBIG;
        എങ്കിൽ (dst_max > 0 && (size_t) size > dst_max)
                തിരികെ -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        എങ്കിൽ (!out)
                തിരികെ -ENOMEM;

        r = sym_LZ4_decompress_safe((അക്ഷരം*)src + 8, out, src_size - 8, size);
        എങ്കിൽ (r < 0 || r != size)
                തിരികെ -EBADMSG;

        *dst_size = size;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(ശൂന്യം) {
#എങ്കിൽ HAVE_ZSTD
        എങ്കിൽ (dlopen_zstd(LOG_DEBUG) < 0)
                തിരികെ 0;

        തിരികെ sym_ZSTD_DStreamOutSize();
#അല്ലെങ്കിൽ
        തിരികെ 0;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_blob_zstd(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **dst,
                size_t *dst_size,
                size_t dst_max) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_ZSTD
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_zstd(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        എങ്കിൽ (size == ZSTD_CONTENTSIZE_ERROR)
                തിരികെ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        എങ്കിൽ (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        അല്ലെങ്കിൽ {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        എങ്കിൽ (!greedy_realloc(dst, space, 1))
                തിരികെ -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        എങ്കിൽ (!dctx)
                തിരികെ -ENOMEM;

        ZSTD_inBuffer നിവേശം = {
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

        വേണ്ടി (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &നിവേശം);
                എങ്കിൽ (sym_ZSTD_isError(k))
                        തിരികെ log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                എങ്കിൽ (output.pos >= cap)
                        നിർത്തുക;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                എങ്കിൽ (k == 0 && നിവേശം.pos >= നിവേശം.size)
                        നിർത്തുക;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                എങ്കിൽ (output.pos == output.size) {
                        എങ്കിൽ (space > SIZE_MAX / 2)
                                തിരികെ -E2BIG;
                        space = MIN(2 * space, cap);
                        എങ്കിൽ (!greedy_realloc(dst, space, 1))
                                തിരികെ -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        തുടരുക;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                എങ്കിൽ (നിവേശം.pos >= നിവേശം.size)
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_blob_gzip(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **dst,
                size_t *dst_size,
                size_t dst_max) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_ZLIB
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_zlib(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size > UINT_MAX)
                തിരികെ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (പൂർണ്ണസംഖ്യ) വലുപ്പം(s));
        എങ്കിൽ (r != Z_OK)
                തിരികെ -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        എങ്കിൽ (!greedy_realloc(dst, space, 1))
                തിരികെ -ENOMEM;

        s.next_in = (ശൂന്യം*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        വേണ്ടി (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                എങ്കിൽ (r == Z_STREAM_END)
                        നിർത്തുക;
                എങ്കിൽ (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        തിരികെ -EBADMSG;

                എങ്കിൽ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        നിർത്തുക;
                എങ്കിൽ (dst_max > 0 && space == dst_max)
                        തിരികെ -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                എങ്കിൽ (!greedy_realloc(dst, space, 1))
                        തിരികെ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_blob_bzip2(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **dst,
                size_t *dst_size,
                size_t dst_max) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(dst);
        പ്രതിജ്ഞാ(dst_size);

#എങ്കിൽ HAVE_BZIP2
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_bzip2(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size > UINT_MAX)
                തിരികെ -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        എങ്കിൽ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                തിരികെ -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        എങ്കിൽ (!greedy_realloc(dst, space, 1))
                തിരികെ -ENOMEM;

        s.next_in = (അക്ഷരം*) src;
        s.avail_in = src_size;
        s.next_out = (അക്ഷരം*) *dst;
        s.avail_out = space;

        വേണ്ടി (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                എങ്കിൽ (r == BZ_STREAM_END)
                        നിർത്തുക;
                എങ്കിൽ (r != BZ_OK)
                        തിരികെ -EBADMSG;

                എങ്കിൽ (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        നിർത്തുക;
                എങ്കിൽ (dst_max > 0 && space == dst_max)
                        തിരികെ -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                എങ്കിൽ (!greedy_realloc(dst, space, 1))
                        തിരികെ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (അക്ഷരം*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

പൂർണ്ണസംഖ്യ decompress_blob_journal(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **dst,
                size_t *dst_size,
                size_t dst_max) {

        തിരഞ്ഞെടുപ്പ് (compression) {
        അവസ്ഥ COMPRESSION_XZ:
                തിരികെ decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        അവസ്ഥ COMPRESSION_LZ4:
                തിരികെ decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        അവസ്ഥ COMPRESSION_ZSTD:
                തിരികെ decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EPROTONOSUPPORT;
        }
}

പൂർണ്ണസംഖ്യ decompress_blob(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **dst,
                size_t *dst_size,
                size_t dst_max) {

        തിരഞ്ഞെടുപ്പ് (compression) {
        അവസ്ഥ COMPRESSION_GZIP:
                തിരികെ decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        അവസ്ഥ COMPRESSION_BZIP2:
                തിരികെ decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        സ്ഥിരസ്ഥിതി:
                തിരികെ decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

പൂർണ്ണസംഖ്യ decompress_zlib_raw(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം *dst,
                size_t dst_size,
                പൂർണ്ണസംഖ്യ wbits) {

#എങ്കിൽ HAVE_ZLIB
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_zlib(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size > UINT_MAX)
                തിരികെ -EFBIG;
        എങ്കിൽ (dst_size > UINT_MAX)
                തിരികെ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (ശൂന്യം*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (പൂർണ്ണസംഖ്യ) വലുപ്പം(s));
        എങ്കിൽ (r != Z_OK)
                തിരികെ -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        എങ്കിൽ (r != Z_STREAM_END || produced != dst_size)
                തിരികെ -EBADMSG;

        തിരികെ 0;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_startswith_xz(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **buffer,
                സ്ഥിരം ശൂന്യം *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(prefix);

#എങ്കിൽ HAVE_XZ
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_xz(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        എങ്കിൽ (ret != LZMA_OK)
                തിരികെ -EBADMSG;

        എങ്കിൽ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                തിരികെ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        വേണ്ടി (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                എങ്കിൽ (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        തിരികെ -EBADMSG;

                എങ്കിൽ (allocated - s.avail_out >= prefix_len + 1)
                        തിരികെ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((സ്ഥിരം uint8_t*) *buffer)[prefix_len] == extra;

                എങ്കിൽ (ret == LZMA_STREAM_END)
                        തിരികെ 0;

                s.avail_out += allocated;

                എങ്കിൽ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        തിരികെ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_startswith_lz4(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **buffer,
                സ്ഥിരം ശൂന്യം *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(prefix);

#എങ്കിൽ HAVE_LZ4
        size_t allocated;
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_lz4(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size <= 8)
                തിരികെ -EBADMSG;

        എങ്കിൽ (src_size - 8 > INT_MAX)
                തിരികെ -EFBIG;

        എങ്കിൽ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                തിരികെ -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (അക്ഷരം*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        എങ്കിൽ (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                എങ്കിൽ (sym_LZ4_version_സംഖ്യ() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        തിരികെ 0;

                എങ്കിൽ (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        എങ്കിൽ (memcmp(*buffer, prefix, r) != 0)
                                തിരികെ 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (size < prefix_len + 1)
                        തിരികെ 0;
        }

        തിരികെ memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((സ്ഥിരം uint8_t*) *buffer)[prefix_len] == extra;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_startswith_zstd(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **buffer,
                സ്ഥിരം ശൂന്യം *prefix,
                size_t prefix_len,
                uint8_t extra) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(prefix);

#എങ്കിൽ HAVE_ZSTD
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_zstd(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        എങ്കിൽ (size == ZSTD_CONTENTSIZE_ERROR)
                തിരികെ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        എങ്കിൽ (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                തിരികെ 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        എങ്കിൽ (!dctx)
                തിരികെ -ENOMEM;

        എങ്കിൽ (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                തിരികെ -ENOMEM;

        ZSTD_inBuffer നിവേശം = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &നിവേശം);
        എങ്കിൽ (sym_ZSTD_isError(k))
                തിരികെ log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        എങ്കിൽ (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                എങ്കിൽ (k != 0)
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                തിരികെ 0; /* Decompressed text too short to match the prefix and extra */
        }

        തിരികെ memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((സ്ഥിരം uint8_t*) *buffer)[prefix_len] == extra;
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_startswith_gzip(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **buffer,
                സ്ഥിരം ശൂന്യം *prefix,
                size_t prefix_len,
                uint8_t extra) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(prefix);

#എങ്കിൽ HAVE_ZLIB
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_zlib(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size > UINT_MAX)
                തിരികെ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (പൂർണ്ണസംഖ്യ) വലുപ്പം(s));
        എങ്കിൽ (r != Z_OK)
                തിരികെ -EBADMSG;

        എങ്കിൽ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                തിരികെ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (ശൂന്യം*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        വേണ്ടി (;;) {
                r = sym_inflate(&s, Z_FINISH);

                എങ്കിൽ (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        തിരികെ -EBADMSG;

                എങ്കിൽ (allocated - s.avail_out >= prefix_len + 1)
                        തിരികെ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((സ്ഥിരം uint8_t*) *buffer)[prefix_len] == extra;

                എങ്കിൽ (r == Z_STREAM_END)
                        തിരികെ 0;

                എങ്കിൽ (s.avail_out > 0)
                        തിരികെ -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                എങ്കിൽ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        തിരികെ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_startswith_bzip2(
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                ശൂന്യം **buffer,
                സ്ഥിരം ശൂന്യം *prefix,
                size_t prefix_len,
                uint8_t extra) {

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(src_size > 0);
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(prefix);

#എങ്കിൽ HAVE_BZIP2
        പൂർണ്ണസംഖ്യ r;

        r = dlopen_bzip2(LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (src_size > UINT_MAX)
                തിരികെ -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        എങ്കിൽ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                തിരികെ -EIO;

        എങ്കിൽ (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                തിരികെ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (അക്ഷരം*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        വേണ്ടി (;;) {
                r = sym_BZ2_bzDecompress(&s);

                എങ്കിൽ (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        തിരികെ -EBADMSG;

                എങ്കിൽ (allocated - s.avail_out >= prefix_len + 1)
                        തിരികെ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((സ്ഥിരം uint8_t*) *buffer)[prefix_len] == extra;

                എങ്കിൽ (r == BZ_STREAM_END)
                        തിരികെ 0;

                എങ്കിൽ (s.avail_out > 0)
                        തിരികെ -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                എങ്കിൽ (!(greedy_realloc(buffer, allocated * 2, 1)))
                        തിരികെ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (അക്ഷരം*) *buffer + used;
        }
#അല്ലെങ്കിൽ
        തിരികെ -EPROTONOSUPPORT;
#endif
}

പൂർണ്ണസംഖ്യ decompress_startswith_journal(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                ശൂന്യം **buffer,
                സ്ഥിരം ശൂന്യം *prefix, size_t prefix_len,
                uint8_t extra) {

        തിരഞ്ഞെടുപ്പ് (compression) {
        അവസ്ഥ COMPRESSION_XZ:
                തിരികെ decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        അവസ്ഥ COMPRESSION_LZ4:
                തിരികെ decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        അവസ്ഥ COMPRESSION_ZSTD:
                തിരികെ decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        സ്ഥിരസ്ഥിതി:
                തിരികെ -EOPNOTSUPP;
        }
}

പൂർണ്ണസംഖ്യ decompress_startswith(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                ശൂന്യം **buffer,
                സ്ഥിരം ശൂന്യം *prefix, size_t prefix_len,
                uint8_t extra) {

        തിരഞ്ഞെടുപ്പ് (compression) {
        അവസ്ഥ COMPRESSION_GZIP:
                തിരികെ decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        അവസ്ഥ COMPRESSION_BZIP2:
                തിരികെ decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        സ്ഥിരസ്ഥിതി:
                തിരികെ decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

പൂർണ്ണസംഖ്യ compress_stream(
                Compression type,
                പൂർണ്ണസംഖ്യ fdf, പൂർണ്ണസംഖ്യ fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = ശൂന്യ;
        _cleanup_free_ ശൂന്യം *buf = ശൂന്യ;
        _cleanup_free_ uint8_t *നിവേശം = ശൂന്യ;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fdf >= 0);
        പ്രതിജ്ഞാ(fdt >= 0);

        r = compressor_new(&c, type);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        നിവേശം = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        എങ്കിൽ (!നിവേശം)
                തിരികെ -ENOMEM;

        വേണ്ടി (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                എങ്കിൽ (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = പഠ(fdf, നിവേശം, m);
                എങ്കിൽ (n < 0)
                        തിരികെ -errno;

                എങ്കിൽ (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        എങ്കിൽ (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                                total_out += buf_size;
                        }
                        നിർത്തുക;
                }

                total_in += n;
                എങ്കിൽ (max_bytes != UINT64_MAX) {
                        പ്രതിജ്ഞാ(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, നിവേശം, n, &buf, &buf_size, &buf_alloc);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        total_out += buf_size;
                }
        }

        എങ്കിൽ (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        എങ്കിൽ (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        അല്ലെങ്കിൽ
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (ഇരട്ട) total_out / total_in * 100);

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ should_sparse(പൂർണ്ണസംഖ്യ fd) {
        ഘടന stat st;

        പ്രതിജ്ഞാ(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        എങ്കിൽ (!S_ISREG(st.st_mode))
                തിരികെ false;

        പൂർണ്ണസംഖ്യ flags = fcntl(fd, F_GETFL);
        എങ്കിൽ (flags < 0)
                തിരികെ -errno;

        എങ്കിൽ (FLAGS_SET(flags, O_APPEND))
                തിരികെ false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        എങ്കിൽ (pos < 0)
                തിരികെ -errno;

        തിരികെ pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
സ്ഥിതം പൂർണ്ണസംഖ്യ finalize_sparse(പൂർണ്ണസംഖ്യ fd) {
        off_t pos;

        പ്രതിജ്ഞാ(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        എങ്കിൽ (pos < 0)
                തിരികെ -errno;

        എങ്കിൽ (ftruncate(fd, pos) < 0)
                തിരികെ -errno;

        തിരികെ 0;
}

/* Common helper for decompress_stream_*() wrappers */

ഘടന decompress_stream_userdata {
        പൂർണ്ണസംഖ്യ fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_stream_write_callback(സ്ഥിരം ശൂന്യം *data, size_t size, ശൂന്യം *userdata) {
        ഘടന decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        എങ്കിൽ (u->max_bytes != UINT64_MAX) {
                എങ്കിൽ (u->max_bytes < size)
                        തിരികെ -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        എങ്കിൽ (u->skip > 0) {
                എങ്കിൽ (u->skip >= size) {
                        u->skip -= size;
                        തിരികെ 0;
                }

                data = (സ്ഥിരം uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        എങ്കിൽ (size == 0)
                തിരികെ 0;

        എങ്കിൽ (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                എങ്കിൽ (k < 0)
                        തിരികെ (പൂർണ്ണസംഖ്യ) k;
                തിരികെ 0;
        }

        തിരികെ loop_write(u->fd, data, size);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        പ്രതിജ്ഞാ(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        തിരഞ്ഞെടുപ്പ് (type) {
        അവസ്ഥ COMPRESSION_NONE:
                നിർത്തുക;

#എങ്കിൽ HAVE_XZ
        അവസ്ഥ COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                എങ്കിൽ (xz_ret != LZMA_OK)
                        തിരികെ -EIO;
                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_LZ4
        അവസ്ഥ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                എങ്കിൽ (sym_LZ4F_isError(rc))
                        തിരികെ -ENOMEM;

                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_ZSTD
        അവസ്ഥ COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                എങ്കിൽ (!c->d_zstd)
                        തിരികെ -ENOMEM;
                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_ZLIB
        അവസ്ഥ COMPRESSION_GZIP:
                എങ്കിൽ (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (പൂർണ്ണസംഖ്യ) വലുപ്പം(c->gzip)) != Z_OK)
                        തിരികെ -EIO;
                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_BZIP2
        അവസ്ഥ COMPRESSION_BZIP2:
                എങ്കിൽ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        തിരികെ -EIO;
                നിർത്തുക;
#endif

        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ decompress_blob_to_fd_impl(
                Compression type,
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                പൂർണ്ണസംഖ്യ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(src);
        പ്രതിജ്ഞാ(fdt >= 0);

        എങ്കിൽ (type == COMPRESSION_NONE)
                തിരികെ -EOPNOTSUPP;

        എങ്കിൽ (src_size == 0)
                തിരികെ -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        എങ്കിൽ (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                എങ്കിൽ (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        എങ്കിൽ (src_size <= വലുപ്പം(uint64_t))
                                തിരികെ -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        എങ്കിൽ (sz < skip)
                                തിരികെ -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                എങ്കിൽ (max_bytes == 0)
                        തിരികെ -ENOBUFS;

                _cleanup_free_ ശൂന്യം *buf = ശൂന്യ;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                /* Check the result size again for safety. */
                എങ്കിൽ (size < skip)
                        തിരികെ -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (ഇരട്ട) size / src_size * 100);

                എങ്കിൽ (ret_size)
                        *ret_size = size - skip;
                തിരികെ 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = ശൂന്യ;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        ഘടന decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_തള്ളുക(c, src, src_size, decompress_stream_write_callback, &userdata);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Finalize */
        r = decompressor_തള്ളുക(c, /* data= */ ശൂന്യ, /* size= */ 0, decompress_stream_write_callback, &userdata);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (userdata.total_out < skip)
                തിരികെ -EBADMSG;

        എങ്കിൽ (userdata.sparse) {
                r = finalize_sparse(fdt);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (ഇരട്ട) userdata.total_out / src_size * 100);

        എങ്കിൽ (ret_size)
                *ret_size = userdata.total_out - skip;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ decompress_blob_to_fd_journal(
                Compression type,
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                പൂർണ്ണസംഖ്യ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        പൂർണ്ണസംഖ്യ r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

പൂർണ്ണസംഖ്യ decompress_blob_to_fd(
                Compression type,
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                പൂർണ്ണസംഖ്യ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        പൂർണ്ണസംഖ്യ r;

        r = dlopen_compress(type, LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

പൂർണ്ണസംഖ്യ decompress_stream_full(
                Compression type,
                പൂർണ്ണസംഖ്യ fdf, പൂർണ്ണസംഖ്യ fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = ശൂന്യ;
        _cleanup_free_ uint8_t *buf = ശൂന്യ;
        uint64_t total_in = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fdf >= 0);
        പ്രതിജ്ഞാ(fdt >= 0);

        എങ്കിൽ (type == COMPRESSION_NONE)
                തിരികെ -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = decompressor_new(&c, type, flags);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        ഘടന decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        വേണ്ടി (;;) {
                ssize_t n;

                n = പഠ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                എങ്കിൽ (n < 0)
                        തിരികെ -errno;

                total_in += n;

                r = decompressor_തള്ളുക(c, buf, n, decompress_stream_write_callback, &userdata);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (n == 0)
                        നിർത്തുക;
        }

        എങ്കിൽ (total_in == 0)
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        എങ്കിൽ (userdata.sparse) {
                r = finalize_sparse(fdt);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (ഇരട്ട) userdata.total_out / total_in * 100);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ decompress_stream_by_filename(സ്ഥിരം അക്ഷരം *filename, പൂർണ്ണസംഖ്യ fdf, പൂർണ്ണസംഖ്യ fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        എങ്കിൽ (c == COMPRESSION_NONE)
                തിരികെ -EPROTONOSUPPORT;

        തിരികെ decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        എങ്കിൽ (!c)
                തിരികെ ശൂന്യ;

        തിരഞ്ഞെടുപ്പ് (c->type) {

#എങ്കിൽ HAVE_XZ
        അവസ്ഥ COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_LZ4
        അവസ്ഥ COMPRESSION_LZ4:
                എങ്കിൽ (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = ശൂന്യ;
                        c->lz4_header = mfree(c->lz4_header);
                } അല്ലെങ്കിൽ {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = ശൂന്യ;
                }
                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_ZSTD
        അവസ്ഥ COMPRESSION_ZSTD:
                എങ്കിൽ (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = ശൂന്യ;
                } അല്ലെങ്കിൽ {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = ശൂന്യ;
                }
                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_ZLIB
        അവസ്ഥ COMPRESSION_GZIP:
                എങ്കിൽ (c->encoding)
                        sym_deflateEnd(&c->gzip);
                അല്ലെങ്കിൽ
                        sym_inflateEnd(&c->gzip);
                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_BZIP2
        അവസ്ഥ COMPRESSION_BZIP2:
                എങ്കിൽ (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                അല്ലെങ്കിൽ
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                നിർത്തുക;
#endif

        സ്ഥിരസ്ഥിതി:
                നിർത്തുക;
        }

        തിരികെ mfree(c);
}

Compression compressor_type(സ്ഥിരം Compressor *c) {
        തിരികെ c ? c->type : _COMPRESSION_INVALID;
}

പൂർണ്ണസംഖ്യ decompressor_detect(Decompressor **decompressor, സ്ഥിരം ശൂന്യം *data, size_t size) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(decompressor);

        എങ്കിൽ (*decompressor)
                തിരികെ 1;

        എങ്കിൽ (size < COMPRESSION_MAGIC_BYTES_MAX)
                തിരികെ 0;

        പ്രതിജ്ഞാ(data);

        Compression type = compression_detect_from_magic(data);
        എങ്കിൽ (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ decompressor_force(Decompressor **decompressor, Compression type) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(decompressor);
        പ്രതിജ്ഞാ(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *decompressor = compressor_free(*decompressor);
        തിരികെ decompressor_new(decompressor, type, /* flags= */ 0);
}

പൂർണ്ണസംഖ്യ decompressor_force_off(Decompressor **decompressor) {
        തിരികെ decompressor_force(decompressor, COMPRESSION_NONE);
}

പൂർണ്ണസംഖ്യ decompressor_തള്ളുക(Decompressor *c, സ്ഥിരം ശൂന്യം *data, size_t size, DecompressorCallback callback, ശൂന്യം *userdata) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(c);
        പ്രതിജ്ഞാ(data || size == 0);
        പ്രതിജ്ഞാ(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        എങ്കിൽ (c->encoding)
                തിരികെ -EINVAL;

        എങ്കിൽ (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                തിരികെ 0;

#എങ്കിൽ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = ശൂന്യ;
        എങ്കിൽ (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                എങ്കിൽ (!buffer)
                        തിരികെ -ENOMEM;
        }
#endif

        തിരഞ്ഞെടുപ്പ് (c->type) {

        അവസ്ഥ COMPRESSION_NONE:
                എങ്കിൽ (c->finished)
                        തിരികെ -EBADMSG;

                എങ്കിൽ (size == 0) {
                        c->finished = true;
                        തിരികെ 0;
                }

                r = callback(data, size, userdata);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                തിരികെ 0;

#എങ്കിൽ HAVE_XZ
        അവസ്ഥ COMPRESSION_XZ:
                എങ്കിൽ (c->finished)
                        തിരികെ -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                വേണ്ടി (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        എങ്കിൽ (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                തിരികെ -EBADMSG;

                        എങ്കിൽ (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                തിരികെ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        എങ്കിൽ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                        }

                        എങ്കിൽ (lzr == LZMA_STREAM_END) {
                                എങ്കിൽ (c->xz.avail_in > 0)
                                        തിരികെ -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                തിരികെ 0;
                        }

                        എങ്കിൽ (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                എങ്കിൽ (size == 0)
                                        തിരികെ -EBADMSG; /* truncated */

                                തിരികെ 0; /* No progress possible with the current input. */
                        }
                }
#endif

#എങ്കിൽ HAVE_LZ4
        അവസ്ഥ COMPRESSION_LZ4: {
                സ്ഥിരം uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                വേണ്ടി (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, ശൂന്യ);
                        എങ്കിൽ (sym_LZ4F_isError(rc))
                                തിരികെ -EBADMSG;

                        എങ്കിൽ (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                തിരികെ -EIO;

                        എങ്കിൽ (consumed > 0) {
                                പ്രതിജ്ഞാ(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        എങ്കിൽ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                        }

                        എങ്കിൽ (src_remaining == 0) {
                                എങ്കിൽ (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        തിരികെ 0;
                                }

                                എങ്കിൽ (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        തുടരുക;

                                എങ്കിൽ (size == 0)
                                        തിരികെ -EBADMSG; /* truncated */

                                തിരികെ 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#എങ്കിൽ HAVE_ZSTD
        അവസ്ഥ COMPRESSION_ZSTD: {
                ZSTD_inBuffer നിവേശം = {
                        .src =  (ശൂന്യം*) data,
                        .size = size,
                };

                c->finished = false;

                വേണ്ടി (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &നിവേശം);
                        എങ്കിൽ (sym_ZSTD_isError(res))
                                തിരികെ -EBADMSG;

                        എങ്കിൽ (നിവേശം.pos > നിവേശം.size ||
                            output.pos > output.size)
                                തിരികെ -EIO;

                        എങ്കിൽ (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                        }

                        എങ്കിൽ (നിവേശം.pos == നിവേശം.size) {
                                എങ്കിൽ (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        തിരികെ 0;
                                }

                                എങ്കിൽ (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        തുടരുക;

                                എങ്കിൽ (size == 0)
                                        തിരികെ -EBADMSG; /* truncated */

                                തിരികെ 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#എങ്കിൽ HAVE_ZLIB
        അവസ്ഥ COMPRESSION_GZIP:
                എങ്കിൽ (size > UINT_MAX)
                        തിരികെ -EFBIG;

                c->gzip.next_in = (ശൂന്യം*) data;
                c->gzip.avail_in = size;

                വേണ്ടി (;;) {
                        എങ്കിൽ (c->finished) {
                                എങ്കിൽ (sym_inflateReset(&c->gzip) != Z_OK)
                                        തിരികെ -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        പൂർണ്ണസംഖ്യ zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        എങ്കിൽ (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                തിരികെ -EBADMSG;

                        എങ്കിൽ (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                തിരികെ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        എങ്കിൽ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                        }

                        എങ്കിൽ (zr == Z_STREAM_END) {
                                c->finished = true;

                                എങ്കിൽ (c->gzip.avail_in > 0)
                                        തുടരുക; /* New stream may be pending. */

                                തിരികെ 0;
                        }

                        എങ്കിൽ (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                എങ്കിൽ (size == 0)
                                        തിരികെ -EBADMSG; /* truncated */

                                തിരികെ 0; /* No progress possible with the current input. */
                        }
                }
#endif

#എങ്കിൽ HAVE_BZIP2
        അവസ്ഥ COMPRESSION_BZIP2:
                എങ്കിൽ (size > UINT_MAX)
                        തിരികെ -EFBIG;

                c->bzip2.next_in = (അക്ഷരം*) data;
                c->bzip2.avail_in = size;

                വേണ്ടി (;;) {
                        എങ്കിൽ (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                എങ്കിൽ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        തിരികെ -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (അക്ഷരം*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        പൂർണ്ണസംഖ്യ bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        എങ്കിൽ (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                തിരികെ -EBADMSG;

                        എങ്കിൽ (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                തിരികെ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        എങ്കിൽ (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                        }

                        എങ്കിൽ (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                എങ്കിൽ (c->bzip2.avail_in > 0)
                                        തുടരുക; /* New stream may be pending. */

                                തിരികെ 0;
                        }

                        എങ്കിൽ (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                എങ്കിൽ (size == 0)
                                        തിരികെ -EBADMSG; /* truncated */

                                തിരികെ 0; /* No progress possible with the current input. */
                        }
                }
#endif

        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }
}

പൂർണ്ണസംഖ്യ compressor_new(Compressor **ret, Compression type) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        തിരഞ്ഞെടുപ്പ് (type) {

#എങ്കിൽ HAVE_XZ
        അവസ്ഥ COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                എങ്കിൽ (xzr != LZMA_OK)
                        തിരികെ -EIO;

                c->type = COMPRESSION_XZ;
                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_LZ4
        അവസ്ഥ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                എങ്കിൽ (sym_LZ4F_isError(rc))
                        തിരികെ -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                എങ്കിൽ (!c->lz4_header)
                        തിരികെ -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                എങ്കിൽ (sym_LZ4F_isError(c->lz4_header_size))
                        തിരികെ -EINVAL;

                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_ZSTD
        അവസ്ഥ COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                എങ്കിൽ (!c->c_zstd)
                        തിരികെ -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                എങ്കിൽ (sym_ZSTD_isError(z))
                        തിരികെ -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                എങ്കിൽ (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_ZLIB
        അവസ്ഥ COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (പൂർണ്ണസംഖ്യ) വലുപ്പം(c->gzip));
                എങ്കിൽ (r != Z_OK)
                        തിരികെ -EIO;

                c->type = COMPRESSION_GZIP;
                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_BZIP2
        അവസ്ഥ COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                എങ്കിൽ (r != BZ_OK)
                        തിരികെ -EIO;

                c->type = COMPRESSION_BZIP2;
                നിർത്തുക;
#endif

        അവസ്ഥ COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                നിർത്തുക;

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        തിരികെ 0;
}

#എങ്കിൽ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
സ്ഥിതം പൂർണ്ണസംഖ്യ enlarge_buffer(ശൂന്യം **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(buffer_size);
        പ്രതിജ്ഞാ(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        എങ്കിൽ (*buffer_allocated >= need)
                തിരികെ 0;

        എങ്കിൽ (!greedy_realloc(buffer, need, 1))
                തിരികെ -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        തിരികെ 1;
}
#endif

പൂർണ്ണസംഖ്യ compressor_start(
                Compressor *c,
                സ്ഥിരം ശൂന്യം *data,
                size_t size,
                ശൂന്യം **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#എങ്കിൽ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        പൂർണ്ണസംഖ്യ r;
#endif

        പ്രതിജ്ഞാ(c);
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(buffer_size);
        പ്രതിജ്ഞാ(buffer_allocated);

        എങ്കിൽ (!c->encoding)
                തിരികെ -EINVAL;

        എങ്കിൽ (size == 0)
                തിരികെ 0;

        പ്രതിജ്ഞാ(data);

        *buffer_size = 0;

        തിരഞ്ഞെടുപ്പ് (c->type) {

#എങ്കിൽ HAVE_XZ
        അവസ്ഥ COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                ആകുമ്പോൾ (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        എങ്കിൽ (lzr != LZMA_OK)
                                തിരികെ -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_LZ4
        അവസ്ഥ COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                എങ്കിൽ (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, ശൂന്യ);
                എങ്കിൽ (sym_LZ4F_isError(n))
                        തിരികെ -EIO;

                *buffer_size += n;
                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_ZSTD
        അവസ്ഥ COMPRESSION_ZSTD: {
                ZSTD_inBuffer നിവേശം = {
                        .src = data,
                        .size = size,
                };

                ആകുമ്പോൾ (നിവേശം.pos < നിവേശം.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &നിവേശം, ZSTD_e_continue);
                        എങ്കിൽ (sym_ZSTD_isError(res))
                                തിരികെ -EIO;

                        *buffer_size += output.pos;
                }

                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_ZLIB
        അവസ്ഥ COMPRESSION_GZIP:
                എങ്കിൽ (size > UINT_MAX)
                        തിരികെ -EFBIG;

                c->gzip.next_in = (ശൂന്യം*) data;
                c->gzip.avail_in = size;

                ആകുമ്പോൾ (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        എങ്കിൽ (r != Z_OK)
                                തിരികെ -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_BZIP2
        അവസ്ഥ COMPRESSION_BZIP2:
                എങ്കിൽ (size > UINT_MAX)
                        തിരികെ -EFBIG;

                c->bzip2.next_in = (ശൂന്യം*) data;
                c->bzip2.avail_in = size;

                ആകുമ്പോൾ (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (ശൂന്യം*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        എങ്കിൽ (r != BZ_RUN_OK)
                                തിരികെ -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                നിർത്തുക;
#endif

        അവസ്ഥ COMPRESSION_NONE:

                എങ്കിൽ (*buffer_allocated < size) {
                        ശൂന്യം *p;

                        p = realloc(*buffer, size);
                        എങ്കിൽ (!p)
                                തിരികെ -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                നിർത്തുക;

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EOPNOTSUPP;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ compressor_finish(Compressor *c, ശൂന്യം **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#എങ്കിൽ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        പൂർണ്ണസംഖ്യ r;
#endif

        പ്രതിജ്ഞാ(c);
        പ്രതിജ്ഞാ(buffer);
        പ്രതിജ്ഞാ(buffer_size);
        പ്രതിജ്ഞാ(buffer_allocated);

        എങ്കിൽ (!c->encoding)
                തിരികെ -EINVAL;

        *buffer_size = 0;

        തിരഞ്ഞെടുപ്പ് (c->type) {

#എങ്കിൽ HAVE_XZ
        അവസ്ഥ COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                ചെയ്യുക {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        എങ്കിൽ (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                തിരികെ -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } ആകുമ്പോൾ (lzr != LZMA_STREAM_END);

                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_LZ4
        അവസ്ഥ COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, ശൂന്യ);
                എങ്കിൽ (sym_LZ4F_isError(n))
                        തിരികെ -EIO;

                *buffer_size = n;
                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_ZSTD
        അവസ്ഥ COMPRESSION_ZSTD: {
                ZSTD_inBuffer നിവേശം = {};
                size_t res;

                ചെയ്യുക {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &നിവേശം, ZSTD_e_end);
                        എങ്കിൽ (sym_ZSTD_isError(res))
                                തിരികെ -EIO;

                        *buffer_size += output.pos;
                } ആകുമ്പോൾ (res != 0);

                നിർത്തുക;
        }
#endif

#എങ്കിൽ HAVE_ZLIB
        അവസ്ഥ COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                ചെയ്യുക {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        എങ്കിൽ (!IN_SET(r, Z_OK, Z_STREAM_END))
                                തിരികെ -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } ആകുമ്പോൾ (r != Z_STREAM_END);

                നിർത്തുക;
#endif

#എങ്കിൽ HAVE_BZIP2
        അവസ്ഥ COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                ചെയ്യുക {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (ശൂന്യം*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        എങ്കിൽ (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                തിരികെ -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } ആകുമ്പോൾ (r != BZ_STREAM_END);

                നിർത്തുക;
#endif

        അവസ്ഥ COMPRESSION_NONE:
                നിർത്തുക;

        സ്ഥിരസ്ഥിതി:
                തിരികെ -EOPNOTSUPP;
        }

        തിരികെ 0;
}
