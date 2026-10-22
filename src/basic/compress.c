/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଯଦି HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#ଅନ୍ତର୍ଭାଵ <lzma.h>
#endif

#ଯଦି HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#ଅନ୍ତର୍ଭାଵ <lz4.h>
#ଅନ୍ତର୍ଭାଵ <lz4frame.h>
#ଅନ୍ତର୍ଭାଵ <lz4hc.h>
#endif

#ଯଦି HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#ଅନ୍ତର୍ଭାଵ <zstd.h>
#ଅନ୍ତର୍ଭାଵ <zstd_errors.h>
#endif

#ଯଦି HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#ଅନ୍ତର୍ଭାଵ <zlib.h>
#endif

#ଯଦି HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#ଅନ୍ତର୍ଭାଵ <bzlib.h>
#endif

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "bitfield.h"
#ଅନ୍ତର୍ଭାଵ "compress.h"
#ଅନ୍ତର୍ଭାଵ "dlfcn-util.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "unaligned.h"

#ଯଦି HAVE_XZ
ସ୍ଥିର DLSYM_PROTOTYPE(lzma_auto_decoder) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(lzma_code) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(lzma_easy_encoder) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(lzma_end) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(lzma_stream_decoder) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(lzma_lzma_preset) = ଖାଲି;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#ଯଦି HAVE_LZ4
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_compressBegin) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_compressBound) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_compressEnd) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_compressUpdate) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_decompress) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4F_isError) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4_compress_HC) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4_compress_default) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4_decompress_safe) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(LZ4_version_ସଂଖ୍ୟା) = ଖାଲି;

ସ୍ଥିର ଅଚଳ LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#ଯଦି HAVE_ZSTD
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_compress) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_compressStream2) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_createCCtx) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_createDCtx) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_decompressStream) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_freeCCtx) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_freeDCtx) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_getErrorCode) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_getErrorName) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(ZSTD_isError) = ଖାଲି;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, ଖାଲି);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା zstd_ret_to_errno(size_t ret) {
        ବାଛ (sym_ZSTD_getErrorCode(ret)) {
        ସ୍ଥିତି ZSTD_error_dstSize_tooSmall:
                ଫେରନ୍ତୁ -ENOBUFS;
        ସ୍ଥିତି ZSTD_error_memory_allocation:
                ଫେରନ୍ତୁ -ENOMEM;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EBADMSG;
        }
}
#endif

#ଯଦି HAVE_ZLIB
ସ୍ଥିର DLSYM_PROTOTYPE(deflateInit2_) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(deflate) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(deflateEnd) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(inflateInit2_) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(inflate) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(inflateEnd) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(inflateReset) = ଖାଲି;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#ଯଦି HAVE_BZIP2
ସ୍ଥିର DLSYM_PROTOTYPE(BZ2_bzCompressInit) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(BZ2_bzCompress) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(BZ2_bzDecompress) = ଖାଲି;
ସ୍ଥିର DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = ଖାଲି;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
ସଂରଚନା Compressor {
        Compression type;
        bool encoding;
        bool finished;
        ସଂଘ {
#ଯଦି HAVE_XZ
                lzma_stream xz;
#endif
#ଯଦି HAVE_LZ4
                ସଂରଚନା {
                        LZ4F_compressionContext_t c_lz4;
                        ଶୂନ୍ୟ *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#ଯଦି HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#ଯଦି HAVE_ZLIB
                z_stream gzip;
#endif
#ଯଦି HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, ଆକାର(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(ଅଚଳ ଅକ୍ଷର *s) {
        Compression c;

        ପ୍ରତିଜ୍ଞା(s);

        c = compression_from_string(s);
        ଯଦି (c >= 0)
                ଫେରନ୍ତୁ c;

        ଫେରନ୍ତୁ compression_uppercase_from_string(s);
}

Compression compression_from_filename(ଅଚଳ ଅକ୍ଷର *filename) {
        Compression c;
        ଅଚଳ ଅକ୍ଷର *e;

        ପ୍ରତିଜ୍ଞା(filename);

        e = strrchr(filename, '.');
        ଯଦି (!e)
                ଫେରନ୍ତୁ COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        ଯଦି (c < 0)
                ଫେରନ୍ତୁ COMPRESSION_NONE;

        ଫେରନ୍ତୁ c;
}

bool compression_supported_journal(Compression c) {
        ସ୍ଥିର ଅଚଳ unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        ପ୍ରତିଜ୍ଞା(c >= 0);
        ପ୍ରତିଜ୍ଞା(c < _COMPRESSION_MAX);

        ଫେରନ୍ତୁ BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        ସ୍ଥିର ଅଚଳ unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        ପ୍ରତିଜ୍ଞା(c >= 0);
        ପ୍ରତିଜ୍ଞା(c < _COMPRESSION_MAX);

        ଫେରନ୍ତୁ BIT_SET(supported, c);
}

Compression compression_detect_from_magic(ଅଚଳ uint8_t data[ସ୍ଥିର COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        ଯଦି (memcmp(data, (ଅଚଳ uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                ଫେରନ୍ତୁ COMPRESSION_GZIP;
        ଯଦି (memcmp(data, (ଅଚଳ uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                ଫେରନ୍ତୁ COMPRESSION_XZ;
        ଯଦି (memcmp(data, (ଅଚଳ uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                ଫେରନ୍ତୁ COMPRESSION_ZSTD;
        ଯଦି (memcmp(data, (ଅଚଳ uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                ଫେରନ୍ତୁ COMPRESSION_LZ4;
        ଯଦି (memcmp(data, (ଅଚଳ uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                ଫେରନ୍ତୁ COMPRESSION_BZIP2;

        ଫେରନ୍ତୁ _COMPRESSION_INVALID;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_xz(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
#ଯଦି HAVE_XZ
        ସ୍ଥିର ଶୂନ୍ୟ *lzma_dl = ଖାଲି;

        LIBLZMA_NOTE(suggested);

        ଫେରନ୍ତୁ dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_lz4(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
#ଯଦି HAVE_LZ4
        ସ୍ଥିର ଶୂନ୍ୟ *lz4_dl = ଖାଲି;

        LIBLZ4_NOTE(suggested);

        ଫେରନ୍ତୁ dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_ସଂଖ୍ୟା));
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_zstd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
#ଯଦି HAVE_ZSTD
        ସ୍ଥିର ଶୂନ୍ୟ *zstd_dl = ଖାଲି;

        LIBZSTD_NOTE(suggested);

        ଫେରନ୍ତୁ dlopen_many_sym_or_warn(
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
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_zlib(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
#ଯଦି HAVE_ZLIB
        ସ୍ଥିର ଶୂନ୍ୟ *zlib_dl = ଖାଲି;

        LIBZ_NOTE(suggested);

        ଫେରନ୍ତୁ dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_bzip2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
#ଯଦି HAVE_BZIP2
        ସ୍ଥିର ଶୂନ୍ୟ *bzip2_dl = ଖାଲି;

        LIBBZ2_NOTE(suggested);

        ଫେରନ୍ତୁ dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_compress_journal(Compression c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
        ବାଛ (c) {
        ସ୍ଥିତି COMPRESSION_NONE:
                ଫେରନ୍ତୁ 0;
        ସ୍ଥିତି COMPRESSION_XZ:
                ଫେରନ୍ତୁ dlopen_xz(log_level);
        ସ୍ଥିତି COMPRESSION_LZ4:
                ଫେରନ୍ତୁ dlopen_lz4(log_level);
        ସ୍ଥିତି COMPRESSION_ZSTD:
                ଫେରନ୍ତୁ dlopen_zstd(log_level);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_compress(Compression c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
        ବାଛ (c) {
        ସ୍ଥିତି COMPRESSION_GZIP:
                ଫେରନ୍ତୁ dlopen_zlib(log_level);
        ସ୍ଥିତି COMPRESSION_BZIP2:
                ଫେରନ୍ତୁ dlopen_bzip2(log_level);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ dlopen_compress_journal(c, log_level);
        }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob_xz(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_alloc_size > 0);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, ଖାଲି, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, ଖାଲି }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_xz(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                ଯଦି (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        ଫେରନ୍ତୁ -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ଯଦି (src_size < 80)
                ଫେରନ୍ତୁ -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ ଖାଲି,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        ଯଦି (ret != LZMA_OK)
                ଫେରନ୍ତୁ -ENOBUFS;

        *dst_size = out_pos;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob_lz4(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_alloc_size > 0);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_LZ4
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_lz4(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ଯଦି (src_size < 9)
                ଫେରନ୍ତୁ -ENOBUFS;

        ଯଦି (src_size > INT_MAX)
                ଫେରନ୍ତୁ -EFBIG;
        ଯଦି (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        ଯଦି (level <= 0)
                r = sym_LZ4_compress_default(src, (ଅକ୍ଷର*)dst + 8, src_size, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) dst_alloc_size - 8);
        ଅନ୍ୟଥା
                r = sym_LZ4_compress_HC(src, (ଅକ୍ଷର*)dst + 8, src_size, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) dst_alloc_size - 8, level);
        ଯଦି (r <= 0)
                ଫେରନ୍ତୁ -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob_zstd(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_alloc_size > 0);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_ZSTD
        size_t k;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_zstd(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        ଯଦି (sym_ZSTD_isError(k))
                ଫେରନ୍ତୁ zstd_ret_to_errno(k);

        *dst_size = k;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob_gzip(ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                       ଶୂନ୍ୟ *dst, size_t dst_alloc_size, size_t *dst_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_alloc_size > 0);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_ZLIB
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_zlib(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size > UINT_MAX)
                ଫେରନ୍ତୁ -EFBIG;
        ଯଦି (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ଆକାର(s));
        ଯଦି (r != Z_OK)
                ଫେରନ୍ତୁ -ENOMEM;

        s.next_in = (ଶୂନ୍ୟ*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        ଯଦି (r != Z_STREAM_END)
                ଫେରନ୍ତୁ -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob_bzip2(
                ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                ଶୂନ୍ୟ *dst, size_t dst_alloc_size, size_t *dst_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_alloc_size > 0);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_BZIP2
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_bzip2(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size > UINT_MAX)
                ଫେରନ୍ତୁ -EFBIG;
        ଯଦି (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        ଯଦି (r != BZ_OK)
                ଫେରନ୍ତୁ -ENOMEM;

        s.next_in = (ଅକ୍ଷର*) src;
        s.avail_in = src_size;
        s.next_out = (ଅକ୍ଷର*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        ଯଦି (r != BZ_STREAM_END)
                ଫେରନ୍ତୁ -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob_journal(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                ଶୂନ୍ୟ *dst, size_t dst_alloc_size, size_t *dst_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) {

        ବାଛ (compression) {
        ସ୍ଥିତି COMPRESSION_XZ:
                ଫେରନ୍ତୁ compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        ସ୍ଥିତି COMPRESSION_LZ4:
                ଫେରନ୍ତୁ compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        ସ୍ଥିତି COMPRESSION_ZSTD:
                ଫେରନ୍ତୁ compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                ଶୂନ୍ୟ *dst, size_t dst_alloc_size, size_t *dst_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level) {

        ବାଛ (compression) {
        ସ୍ଥିତି COMPRESSION_GZIP:
                ଫେରନ୍ତୁ compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        ସ୍ଥିତି COMPRESSION_BZIP2:
                ଫେରନ୍ତୁ compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_xz(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_XZ
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_xz(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ଯଦି (ret != LZMA_OK)
                ଫେରନ୍ତୁ -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        ଯଦି (!greedy_realloc(dst, space, 1))
                ଫେରନ୍ତୁ -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        ପାଇଁ (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                ଯଦି (ret == LZMA_STREAM_END)
                        ଭାଙ୍ଗ;
                ଯଦି (ret != LZMA_OK)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ଭାଙ୍ଗ;
                ଯଦି (dst_max > 0 && space == dst_max)
                        ଫେରନ୍ତୁ -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                ପ୍ରତିଜ୍ଞା(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                ଯଦି (!greedy_realloc(dst, space, 1))
                        ଫେରନ୍ତୁ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_lz4(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_LZ4
        ଅକ୍ଷର* out;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size <= 8)
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (src_size - 8 > INT_MAX)
                ଫେରନ୍ତୁ -EFBIG;

        size = unaligned_read_le64(src);
        ଯଦି (size < 0 || (unsigned) size != unaligned_read_le64(src))
                ଫେରନ୍ତୁ -EFBIG;
        ଯଦି (dst_max > 0 && (size_t) size > dst_max)
                ଫେରନ୍ତୁ -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        ଯଦି (!out)
                ଫେରନ୍ତୁ -ENOMEM;

        r = sym_LZ4_decompress_safe((ଅକ୍ଷର*)src + 8, out, src_size - 8, size);
        ଯଦି (r < 0 || r != size)
                ଫେରନ୍ତୁ -EBADMSG;

        *dst_size = size;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(ଶୂନ୍ୟ) {
#ଯଦି HAVE_ZSTD
        ଯଦି (dlopen_zstd(LOG_DEBUG) < 0)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ sym_ZSTD_DStreamOutSize();
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ 0;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_zstd(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_ZSTD
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_zstd(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        ଯଦି (size == ZSTD_CONTENTSIZE_ERROR)
                ଫେରନ୍ତୁ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        ଯଦି (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        ଅନ୍ୟଥା {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        ଯଦି (!greedy_realloc(dst, space, 1))
                ଫେରନ୍ତୁ -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ଯଦି (!dctx)
                ଫେରନ୍ତୁ -ENOMEM;

        ZSTD_inBuffer ନିବେଶ = {
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

        ପାଇଁ (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &ନିବେଶ);
                ଯଦି (sym_ZSTD_isError(k))
                        ଫେରନ୍ତୁ log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                ଯଦି (output.pos >= cap)
                        ଭାଙ୍ଗ;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                ଯଦି (k == 0 && ନିବେଶ.pos >= ନିବେଶ.size)
                        ଭାଙ୍ଗ;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                ଯଦି (output.pos == output.size) {
                        ଯଦି (space > SIZE_MAX / 2)
                                ଫେରନ୍ତୁ -E2BIG;
                        space = MIN(2 * space, cap);
                        ଯଦି (!greedy_realloc(dst, space, 1))
                                ଫେରନ୍ତୁ -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        ଜାରି;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                ଯଦି (ନିବେଶ.pos >= ନିବେଶ.size)
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_gzip(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_ZLIB
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_zlib(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size > UINT_MAX)
                ଫେରନ୍ତୁ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ଆକାର(s));
        ଯଦି (r != Z_OK)
                ଫେରନ୍ତୁ -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ଯଦି (!greedy_realloc(dst, space, 1))
                ଫେରନ୍ତୁ -ENOMEM;

        s.next_in = (ଶୂନ୍ୟ*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        ପାଇଁ (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                ଯଦି (r == Z_STREAM_END)
                        ଭାଙ୍ଗ;
                ଯଦି (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        ଫେରନ୍ତୁ -EBADMSG;

                ଯଦି (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ଭାଙ୍ଗ;
                ଯଦି (dst_max > 0 && space == dst_max)
                        ଫେରନ୍ତୁ -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                ଯଦି (!greedy_realloc(dst, space, 1))
                        ଫେରନ୍ତୁ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_bzip2(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(dst);
        ପ୍ରତିଜ୍ଞା(dst_size);

#ଯଦି HAVE_BZIP2
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_bzip2(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size > UINT_MAX)
                ଫେରନ୍ତୁ -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        ଯଦି (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ଫେରନ୍ତୁ -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ଯଦି (!greedy_realloc(dst, space, 1))
                ଫେରନ୍ତୁ -ENOMEM;

        s.next_in = (ଅକ୍ଷର*) src;
        s.avail_in = src_size;
        s.next_out = (ଅକ୍ଷର*) *dst;
        s.avail_out = space;

        ପାଇଁ (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                ଯଦି (r == BZ_STREAM_END)
                        ଭାଙ୍ଗ;
                ଯଦି (r != BZ_OK)
                        ଫେରନ୍ତୁ -EBADMSG;

                ଯଦି (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ଭାଙ୍ଗ;
                ଯଦି (dst_max > 0 && space == dst_max)
                        ଫେରନ୍ତୁ -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                ଯଦି (!greedy_realloc(dst, space, 1))
                        ଫେରନ୍ତୁ -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (ଅକ୍ଷର*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_journal(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ବାଛ (compression) {
        ସ୍ଥିତି COMPRESSION_XZ:
                ଫେରନ୍ତୁ decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        ସ୍ଥିତି COMPRESSION_LZ4:
                ଫେରନ୍ତୁ decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        ସ୍ଥିତି COMPRESSION_ZSTD:
                ଫେରନ୍ତୁ decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EPROTONOSUPPORT;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **dst,
                size_t *dst_size,
                size_t dst_max) {

        ବାଛ (compression) {
        ସ୍ଥିତି COMPRESSION_GZIP:
                ଫେରନ୍ତୁ decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        ସ୍ଥିତି COMPRESSION_BZIP2:
                ଫେରନ୍ତୁ decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_zlib_raw(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ *dst,
                size_t dst_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା wbits) {

#ଯଦି HAVE_ZLIB
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_zlib(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size > UINT_MAX)
                ଫେରନ୍ତୁ -EFBIG;
        ଯଦି (dst_size > UINT_MAX)
                ଫେରନ୍ତୁ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (ଶୂନ୍ୟ*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ଆକାର(s));
        ଯଦି (r != Z_OK)
                ଫେରନ୍ତୁ -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        ଯଦି (r != Z_STREAM_END || produced != dst_size)
                ଫେରନ୍ତୁ -EBADMSG;

        ଫେରନ୍ତୁ 0;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith_xz(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **buffer,
                ଅଚଳ ଶୂନ୍ୟ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(prefix);

#ଯଦି HAVE_XZ
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_xz(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ଯଦି (ret != LZMA_OK)
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ଫେରନ୍ତୁ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        ପାଇଁ (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                ଯଦି (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        ଫେରନ୍ତୁ -EBADMSG;

                ଯଦି (allocated - s.avail_out >= prefix_len + 1)
                        ଫେରନ୍ତୁ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ଅଚଳ uint8_t*) *buffer)[prefix_len] == extra;

                ଯଦି (ret == LZMA_STREAM_END)
                        ଫେରନ୍ତୁ 0;

                s.avail_out += allocated;

                ଯଦି (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ଫେରନ୍ତୁ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith_lz4(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **buffer,
                ଅଚଳ ଶୂନ୍ୟ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(prefix);

#ଯଦି HAVE_LZ4
        size_t allocated;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_lz4(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size <= 8)
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (src_size - 8 > INT_MAX)
                ଫେରନ୍ତୁ -EFBIG;

        ଯଦି (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ଫେରନ୍ତୁ -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (ଅକ୍ଷର*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        ଯଦି (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                ଯଦି (sym_LZ4_version_ସଂଖ୍ୟା() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        ଫେରନ୍ତୁ 0;

                ଯଦି (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        ଯଦି (memcmp(*buffer, prefix, r) != 0)
                                ଫେରନ୍ତୁ 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (size < prefix_len + 1)
                        ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ଅଚଳ uint8_t*) *buffer)[prefix_len] == extra;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith_zstd(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **buffer,
                ଅଚଳ ଶୂନ୍ୟ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(prefix);

#ଯଦି HAVE_ZSTD
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_zstd(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        ଯଦି (size == ZSTD_CONTENTSIZE_ERROR)
                ଫେରନ୍ତୁ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        ଯଦି (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                ଫେରନ୍ତୁ 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ଯଦି (!dctx)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                ଫେରନ୍ତୁ -ENOMEM;

        ZSTD_inBuffer ନିବେଶ = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &ନିବେଶ);
        ଯଦି (sym_ZSTD_isError(k))
                ଫେରନ୍ତୁ log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        ଯଦି (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                ଯଦି (k != 0)
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                ଫେରନ୍ତୁ 0; /* Decompressed text too short to match the prefix and extra */
        }

        ଫେରନ୍ତୁ memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ଅଚଳ uint8_t*) *buffer)[prefix_len] == extra;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith_gzip(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **buffer,
                ଅଚଳ ଶୂନ୍ୟ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(prefix);

#ଯଦି HAVE_ZLIB
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_zlib(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size > UINT_MAX)
                ଫେରନ୍ତୁ -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ଆକାର(s));
        ଯଦି (r != Z_OK)
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ଫେରନ୍ତୁ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (ଶୂନ୍ୟ*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        ପାଇଁ (;;) {
                r = sym_inflate(&s, Z_FINISH);

                ଯଦି (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        ଫେରନ୍ତୁ -EBADMSG;

                ଯଦି (allocated - s.avail_out >= prefix_len + 1)
                        ଫେରନ୍ତୁ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ଅଚଳ uint8_t*) *buffer)[prefix_len] == extra;

                ଯଦି (r == Z_STREAM_END)
                        ଫେରନ୍ତୁ 0;

                ଯଦି (s.avail_out > 0)
                        ଫେରନ୍ତୁ -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                ଯଦି (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ଫେରନ୍ତୁ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith_bzip2(
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ଶୂନ୍ୟ **buffer,
                ଅଚଳ ଶୂନ୍ୟ *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(src_size > 0);
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(prefix);

#ଯଦି HAVE_BZIP2
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_bzip2(LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (src_size > UINT_MAX)
                ଫେରନ୍ତୁ -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        ଯଦି (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ଫେରନ୍ତୁ -EIO;

        ଯଦି (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                ଫେରନ୍ତୁ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (ଅକ୍ଷର*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        ପାଇଁ (;;) {
                r = sym_BZ2_bzDecompress(&s);

                ଯଦି (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        ଫେରନ୍ତୁ -EBADMSG;

                ଯଦି (allocated - s.avail_out >= prefix_len + 1)
                        ଫେରନ୍ତୁ memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ଅଚଳ uint8_t*) *buffer)[prefix_len] == extra;

                ଯଦି (r == BZ_STREAM_END)
                        ଫେରନ୍ତୁ 0;

                ଯଦି (s.avail_out > 0)
                        ଫେରନ୍ତୁ -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                ଯଦି (!(greedy_realloc(buffer, allocated * 2, 1)))
                        ଫେରନ୍ତୁ -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (ଅକ୍ଷର*) *buffer + used;
        }
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -EPROTONOSUPPORT;
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith_journal(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                ଶୂନ୍ୟ **buffer,
                ଅଚଳ ଶୂନ୍ୟ *prefix, size_t prefix_len,
                uint8_t extra) {

        ବାଛ (compression) {
        ସ୍ଥିତି COMPRESSION_XZ:
                ଫେରନ୍ତୁ decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        ସ୍ଥିତି COMPRESSION_LZ4:
                ଫେରନ୍ତୁ decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        ସ୍ଥିତି COMPRESSION_ZSTD:
                ଫେରନ୍ତୁ decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                ଶୂନ୍ୟ **buffer,
                ଅଚଳ ଶୂନ୍ୟ *prefix, size_t prefix_len,
                uint8_t extra) {

        ବାଛ (compression) {
        ସ୍ଥିତି COMPRESSION_GZIP:
                ଫେରନ୍ତୁ decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        ସ୍ଥିତି COMPRESSION_BZIP2:
                ଫେରନ୍ତୁ decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_stream(
                Compression type,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = ଖାଲି;
        _cleanup_free_ ଶୂନ୍ୟ *buf = ଖାଲି;
        _cleanup_free_ uint8_t *ନିବେଶ = ଖାଲି;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fdf >= 0);
        ପ୍ରତିଜ୍ଞା(fdt >= 0);

        r = compressor_new(&c, type);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ନିବେଶ = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        ଯଦି (!ନିବେଶ)
                ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                ଯଦି (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = ପଠ(fdf, ନିବେଶ, m);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        ଯଦି (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                                total_out += buf_size;
                        }
                        ଭାଙ୍ଗ;
                }

                total_in += n;
                ଯଦି (max_bytes != UINT64_MAX) {
                        ପ୍ରତିଜ୍ଞା(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, ନିବେଶ, n, &buf, &buf_size, &buf_alloc);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        total_out += buf_size;
                }
        }

        ଯଦି (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        ଯଦି (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        ଅନ୍ୟଥା
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (ଦ୍ୱିଗୁଣ) total_out / total_in * 100);

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା should_sparse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ସଂରଚନା stat st;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (!S_ISREG(st.st_mode))
                ଫେରନ୍ତୁ false;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = fcntl(fd, F_GETFL);
        ଯଦି (flags < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (FLAGS_SET(flags, O_APPEND))
                ଫେରନ୍ତୁ false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        ଯଦି (pos < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା finalize_sparse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        off_t pos;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        ଯଦି (pos < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ftruncate(fd, pos) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 0;
}

/* Common helper for decompress_stream_*() wrappers */

ସଂରଚନା decompress_stream_userdata {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_stream_write_callback(ଅଚଳ ଶୂନ୍ୟ *data, size_t size, ଶୂନ୍ୟ *userdata) {
        ସଂରଚନା decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        ଯଦି (u->max_bytes != UINT64_MAX) {
                ଯଦି (u->max_bytes < size)
                        ଫେରନ୍ତୁ -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        ଯଦି (u->skip > 0) {
                ଯଦି (u->skip >= size) {
                        u->skip -= size;
                        ଫେରନ୍ତୁ 0;
                }

                data = (ଅଚଳ uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        ଯଦି (size == 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                ଯଦି (k < 0)
                        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) k;
                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ loop_write(u->fd, data, size);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        ପ୍ରତିଜ୍ଞା(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        ବାଛ (type) {
        ସ୍ଥିତି COMPRESSION_NONE:
                ଭାଙ୍ଗ;

#ଯଦି HAVE_XZ
        ସ୍ଥିତି COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                ଯଦି (xz_ret != LZMA_OK)
                        ଫେରନ୍ତୁ -EIO;
                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_LZ4
        ସ୍ଥିତି COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                ଯଦି (sym_LZ4F_isError(rc))
                        ଫେରନ୍ତୁ -ENOMEM;

                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_ZSTD
        ସ୍ଥିତି COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                ଯଦି (!c->d_zstd)
                        ଫେରନ୍ତୁ -ENOMEM;
                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_ZLIB
        ସ୍ଥିତି COMPRESSION_GZIP:
                ଯଦି (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ଆକାର(c->gzip)) != Z_OK)
                        ଫେରନ୍ତୁ -EIO;
                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_BZIP2
        ସ୍ଥିତି COMPRESSION_BZIP2:
                ଯଦି (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        ଫେରନ୍ତୁ -EIO;
                ଭାଙ୍ଗ;
#endif

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_to_fd_impl(
                Compression type,
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(src);
        ପ୍ରତିଜ୍ଞା(fdt >= 0);

        ଯଦି (type == COMPRESSION_NONE)
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        ଯଦି (src_size == 0)
                ଫେରନ୍ତୁ -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        ଯଦି (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                ଯଦି (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        ଯଦି (src_size <= ଆକାର(uint64_t))
                                ଫେରନ୍ତୁ -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        ଯଦି (sz < skip)
                                ଫେରନ୍ତୁ -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                ଯଦି (max_bytes == 0)
                        ଫେରନ୍ତୁ -ENOBUFS;

                _cleanup_free_ ଶୂନ୍ୟ *buf = ଖାଲି;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                /* Check the result size again for safety. */
                ଯଦି (size < skip)
                        ଫେରନ୍ତୁ -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (ଦ୍ୱିଗୁଣ) size / src_size * 100);

                ଯଦି (ret_size)
                        *ret_size = size - skip;
                ଫେରନ୍ତୁ 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = ଖାଲି;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ସଂରଚନା decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ଠେଲ(c, src, src_size, decompress_stream_write_callback, &userdata);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Finalize */
        r = decompressor_ଠେଲ(c, /* data= */ ଖାଲି, /* size= */ 0, decompress_stream_write_callback, &userdata);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (userdata.total_out < skip)
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (userdata.sparse) {
                r = finalize_sparse(fdt);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (ଦ୍ୱିଗୁଣ) userdata.total_out / src_size * 100);

        ଯଦି (ret_size)
                *ret_size = userdata.total_out - skip;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_to_fd_journal(
                Compression type,
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_to_fd(
                Compression type,
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = dlopen_compress(type, LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_stream_full(
                Compression type,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = ଖାଲି;
        _cleanup_free_ uint8_t *buf = ଖାଲି;
        uint64_t total_in = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fdf >= 0);
        ପ୍ରତିଜ୍ଞା(fdt >= 0);

        ଯଦି (type == COMPRESSION_NONE)
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = decompressor_new(&c, type, flags);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ସଂରଚନା decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (;;) {
                ssize_t n;

                n = ପଠ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ -errno;

                total_in += n;

                r = decompressor_ଠେଲ(c, buf, n, decompress_stream_write_callback, &userdata);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (n == 0)
                        ଭାଙ୍ଗ;
        }

        ଯଦି (total_in == 0)
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        ଯଦି (userdata.sparse) {
                r = finalize_sparse(fdt);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (ଦ୍ୱିଗୁଣ) userdata.total_out / total_in * 100);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_stream_by_filename(ଅଚଳ ଅକ୍ଷର *filename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        ଯଦି (c == COMPRESSION_NONE)
                ଫେରନ୍ତୁ -EPROTONOSUPPORT;

        ଫେରନ୍ତୁ decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        ଯଦି (!c)
                ଫେରନ୍ତୁ ଖାଲି;

        ବାଛ (c->type) {

#ଯଦି HAVE_XZ
        ସ୍ଥିତି COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_LZ4
        ସ୍ଥିତି COMPRESSION_LZ4:
                ଯଦି (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = ଖାଲି;
                        c->lz4_header = mfree(c->lz4_header);
                } ଅନ୍ୟଥା {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = ଖାଲି;
                }
                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_ZSTD
        ସ୍ଥିତି COMPRESSION_ZSTD:
                ଯଦି (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = ଖାଲି;
                } ଅନ୍ୟଥା {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = ଖାଲି;
                }
                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_ZLIB
        ସ୍ଥିତି COMPRESSION_GZIP:
                ଯଦି (c->encoding)
                        sym_deflateEnd(&c->gzip);
                ଅନ୍ୟଥା
                        sym_inflateEnd(&c->gzip);
                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_BZIP2
        ସ୍ଥିତି COMPRESSION_BZIP2:
                ଯଦି (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                ଅନ୍ୟଥା
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                ଭାଙ୍ଗ;
#endif

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଭାଙ୍ଗ;
        }

        ଫେରନ୍ତୁ mfree(c);
}

Compression compressor_type(ଅଚଳ Compressor *c) {
        ଫେରନ୍ତୁ c ? c->type : _COMPRESSION_INVALID;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_detect(Decompressor **decompressor, ଅଚଳ ଶୂନ୍ୟ *data, size_t size) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(decompressor);

        ଯଦି (*decompressor)
                ଫେରନ୍ତୁ 1;

        ଯଦି (size < COMPRESSION_MAGIC_BYTES_MAX)
                ଫେରନ୍ତୁ 0;

        ପ୍ରତିଜ୍ଞା(data);

        Compression type = compression_detect_from_magic(data);
        ଯଦି (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_force(Decompressor **decompressor, Compression type) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(decompressor);
        ପ୍ରତିଜ୍ଞା(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *decompressor = compressor_free(*decompressor);
        ଫେରନ୍ତୁ decompressor_new(decompressor, type, /* flags= */ 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_force_off(Decompressor **decompressor) {
        ଫେରନ୍ତୁ decompressor_force(decompressor, COMPRESSION_NONE);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_ଠେଲ(Decompressor *c, ଅଚଳ ଶୂନ୍ୟ *data, size_t size, DecompressorCallback callback, ଶୂନ୍ୟ *userdata) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(c);
        ପ୍ରତିଜ୍ଞା(data || size == 0);
        ପ୍ରତିଜ୍ଞା(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        ଯଦି (c->encoding)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                ଫେରନ୍ତୁ 0;

#ଯଦି HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = ଖାଲି;
        ଯଦି (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                ଯଦି (!buffer)
                        ଫେରନ୍ତୁ -ENOMEM;
        }
#endif

        ବାଛ (c->type) {

        ସ୍ଥିତି COMPRESSION_NONE:
                ଯଦି (c->finished)
                        ଫେରନ୍ତୁ -EBADMSG;

                ଯଦି (size == 0) {
                        c->finished = true;
                        ଫେରନ୍ତୁ 0;
                }

                r = callback(data, size, userdata);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଫେରନ୍ତୁ 0;

#ଯଦି HAVE_XZ
        ସ୍ଥିତି COMPRESSION_XZ:
                ଯଦି (c->finished)
                        ଫେରନ୍ତୁ -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                ପାଇଁ (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        ଯଦି (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                ଫେରନ୍ତୁ -EBADMSG;

                        ଯଦି (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ଫେରନ୍ତୁ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        ଯଦି (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }

                        ଯଦି (lzr == LZMA_STREAM_END) {
                                ଯଦି (c->xz.avail_in > 0)
                                        ଫେରନ୍ତୁ -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                ଫେରନ୍ତୁ 0;
                        }

                        ଯଦି (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                ଯଦି (size == 0)
                                        ଫେରନ୍ତୁ -EBADMSG; /* truncated */

                                ଫେରନ୍ତୁ 0; /* No progress possible with the current input. */
                        }
                }
#endif

#ଯଦି HAVE_LZ4
        ସ୍ଥିତି COMPRESSION_LZ4: {
                ଅଚଳ uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                ପାଇଁ (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, ଖାଲି);
                        ଯଦି (sym_LZ4F_isError(rc))
                                ଫେରନ୍ତୁ -EBADMSG;

                        ଯଦି (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                ଫେରନ୍ତୁ -EIO;

                        ଯଦି (consumed > 0) {
                                ପ୍ରତିଜ୍ଞା(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        ଯଦି (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }

                        ଯଦି (src_remaining == 0) {
                                ଯଦି (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        ଫେରନ୍ତୁ 0;
                                }

                                ଯଦି (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ଜାରି;

                                ଯଦି (size == 0)
                                        ଫେରନ୍ତୁ -EBADMSG; /* truncated */

                                ଫେରନ୍ତୁ 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#ଯଦି HAVE_ZSTD
        ସ୍ଥିତି COMPRESSION_ZSTD: {
                ZSTD_inBuffer ନିବେଶ = {
                        .src =  (ଶୂନ୍ୟ*) data,
                        .size = size,
                };

                c->finished = false;

                ପାଇଁ (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &ନିବେଶ);
                        ଯଦି (sym_ZSTD_isError(res))
                                ଫେରନ୍ତୁ -EBADMSG;

                        ଯଦି (ନିବେଶ.pos > ନିବେଶ.size ||
                            output.pos > output.size)
                                ଫେରନ୍ତୁ -EIO;

                        ଯଦି (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }

                        ଯଦି (ନିବେଶ.pos == ନିବେଶ.size) {
                                ଯଦି (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        ଫେରନ୍ତୁ 0;
                                }

                                ଯଦି (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ଜାରି;

                                ଯଦି (size == 0)
                                        ଫେରନ୍ତୁ -EBADMSG; /* truncated */

                                ଫେରନ୍ତୁ 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#ଯଦି HAVE_ZLIB
        ସ୍ଥିତି COMPRESSION_GZIP:
                ଯଦି (size > UINT_MAX)
                        ଫେରନ୍ତୁ -EFBIG;

                c->gzip.next_in = (ଶୂନ୍ୟ*) data;
                c->gzip.avail_in = size;

                ପାଇଁ (;;) {
                        ଯଦି (c->finished) {
                                ଯଦି (sym_inflateReset(&c->gzip) != Z_OK)
                                        ଫେରନ୍ତୁ -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        ଯଦି (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                ଫେରନ୍ତୁ -EBADMSG;

                        ଯଦି (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ଫେରନ୍ତୁ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        ଯଦି (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }

                        ଯଦି (zr == Z_STREAM_END) {
                                c->finished = true;

                                ଯଦି (c->gzip.avail_in > 0)
                                        ଜାରି; /* New stream may be pending. */

                                ଫେରନ୍ତୁ 0;
                        }

                        ଯଦି (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                ଯଦି (size == 0)
                                        ଫେରନ୍ତୁ -EBADMSG; /* truncated */

                                ଫେରନ୍ତୁ 0; /* No progress possible with the current input. */
                        }
                }
#endif

#ଯଦି HAVE_BZIP2
        ସ୍ଥିତି COMPRESSION_BZIP2:
                ଯଦି (size > UINT_MAX)
                        ଫେରନ୍ତୁ -EFBIG;

                c->bzip2.next_in = (ଅକ୍ଷର*) data;
                c->bzip2.avail_in = size;

                ପାଇଁ (;;) {
                        ଯଦି (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                ଯଦି (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        ଫେରନ୍ତୁ -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (ଅକ୍ଷର*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        ଯଦି (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                ଫେରନ୍ତୁ -EBADMSG;

                        ଯଦି (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                ଫେରନ୍ତୁ -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        ଯଦି (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }

                        ଯଦି (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                ଯଦି (c->bzip2.avail_in > 0)
                                        ଜାରି; /* New stream may be pending. */

                                ଫେରନ୍ତୁ 0;
                        }

                        ଯଦି (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                ଯଦି (size == 0)
                                        ଫେରନ୍ତୁ -EBADMSG; /* truncated */

                                ଫେରନ୍ତୁ 0; /* No progress possible with the current input. */
                        }
                }
#endif

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressor_new(Compressor **ret, Compression type) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        ବାଛ (type) {

#ଯଦି HAVE_XZ
        ସ୍ଥିତି COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                ଯଦି (xzr != LZMA_OK)
                        ଫେରନ୍ତୁ -EIO;

                c->type = COMPRESSION_XZ;
                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_LZ4
        ସ୍ଥିତି COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                ଯଦି (sym_LZ4F_isError(rc))
                        ଫେରନ୍ତୁ -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                ଯଦି (!c->lz4_header)
                        ଫେରନ୍ତୁ -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                ଯଦି (sym_LZ4F_isError(c->lz4_header_size))
                        ଫେରନ୍ତୁ -EINVAL;

                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_ZSTD
        ସ୍ଥିତି COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                ଯଦି (!c->c_zstd)
                        ଫେରନ୍ତୁ -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                ଯଦି (sym_ZSTD_isError(z))
                        ଫେରନ୍ତୁ -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                ଯଦି (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_ZLIB
        ସ୍ଥିତି COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ଆକାର(c->gzip));
                ଯଦି (r != Z_OK)
                        ଫେରନ୍ତୁ -EIO;

                c->type = COMPRESSION_GZIP;
                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_BZIP2
        ସ୍ଥିତି COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                ଯଦି (r != BZ_OK)
                        ଫେରନ୍ତୁ -EIO;

                c->type = COMPRESSION_BZIP2;
                ଭାଙ୍ଗ;
#endif

        ସ୍ଥିତି COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                ଭାଙ୍ଗ;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        ଫେରନ୍ତୁ 0;
}

#ଯଦି HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା enlarge_buffer(ଶୂନ୍ୟ **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(buffer_size);
        ପ୍ରତିଜ୍ଞା(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        ଯଦି (*buffer_allocated >= need)
                ଫେରନ୍ତୁ 0;

        ଯଦି (!greedy_realloc(buffer, need, 1))
                ଫେରନ୍ତୁ -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        ଫେରନ୍ତୁ 1;
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressor_start(
                Compressor *c,
                ଅଚଳ ଶୂନ୍ୟ *data,
                size_t size,
                ଶୂନ୍ୟ **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#ଯଦି HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
#endif

        ପ୍ରତିଜ୍ଞା(c);
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(buffer_size);
        ପ୍ରତିଜ୍ଞା(buffer_allocated);

        ଯଦି (!c->encoding)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (size == 0)
                ଫେରନ୍ତୁ 0;

        ପ୍ରତିଜ୍ଞା(data);

        *buffer_size = 0;

        ବାଛ (c->type) {

#ଯଦି HAVE_XZ
        ସ୍ଥିତି COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                ଯେତେବେଳେ (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        ଯଦି (lzr != LZMA_OK)
                                ଫେରନ୍ତୁ -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_LZ4
        ସ୍ଥିତି COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                ଯଦି (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, ଖାଲି);
                ଯଦି (sym_LZ4F_isError(n))
                        ଫେରନ୍ତୁ -EIO;

                *buffer_size += n;
                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_ZSTD
        ସ୍ଥିତି COMPRESSION_ZSTD: {
                ZSTD_inBuffer ନିବେଶ = {
                        .src = data,
                        .size = size,
                };

                ଯେତେବେଳେ (ନିବେଶ.pos < ନିବେଶ.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ନିବେଶ, ZSTD_e_continue);
                        ଯଦି (sym_ZSTD_isError(res))
                                ଫେରନ୍ତୁ -EIO;

                        *buffer_size += output.pos;
                }

                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_ZLIB
        ସ୍ଥିତି COMPRESSION_GZIP:
                ଯଦି (size > UINT_MAX)
                        ଫେରନ୍ତୁ -EFBIG;

                c->gzip.next_in = (ଶୂନ୍ୟ*) data;
                c->gzip.avail_in = size;

                ଯେତେବେଳେ (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        ଯଦି (r != Z_OK)
                                ଫେରନ୍ତୁ -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_BZIP2
        ସ୍ଥିତି COMPRESSION_BZIP2:
                ଯଦି (size > UINT_MAX)
                        ଫେରନ୍ତୁ -EFBIG;

                c->bzip2.next_in = (ଶୂନ୍ୟ*) data;
                c->bzip2.avail_in = size;

                ଯେତେବେଳେ (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (ଶୂନ୍ୟ*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        ଯଦି (r != BZ_RUN_OK)
                                ଫେରନ୍ତୁ -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                ଭାଙ୍ଗ;
#endif

        ସ୍ଥିତି COMPRESSION_NONE:

                ଯଦି (*buffer_allocated < size) {
                        ଶୂନ୍ୟ *p;

                        p = realloc(*buffer, size);
                        ଯଦି (!p)
                                ଫେରନ୍ତୁ -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                ଭାଙ୍ଗ;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressor_finish(Compressor *c, ଶୂନ୍ୟ **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#ଯଦି HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
#endif

        ପ୍ରତିଜ୍ଞା(c);
        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(buffer_size);
        ପ୍ରତିଜ୍ଞା(buffer_allocated);

        ଯଦି (!c->encoding)
                ଫେରନ୍ତୁ -EINVAL;

        *buffer_size = 0;

        ବାଛ (c->type) {

#ଯଦି HAVE_XZ
        ସ୍ଥିତି COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                କର {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        ଯଦି (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                ଫେରନ୍ତୁ -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } ଯେତେବେଳେ (lzr != LZMA_STREAM_END);

                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_LZ4
        ସ୍ଥିତି COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, ଖାଲି);
                ଯଦି (sym_LZ4F_isError(n))
                        ଫେରନ୍ତୁ -EIO;

                *buffer_size = n;
                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_ZSTD
        ସ୍ଥିତି COMPRESSION_ZSTD: {
                ZSTD_inBuffer ନିବେଶ = {};
                size_t res;

                କର {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ନିବେଶ, ZSTD_e_end);
                        ଯଦି (sym_ZSTD_isError(res))
                                ଫେରନ୍ତୁ -EIO;

                        *buffer_size += output.pos;
                } ଯେତେବେଳେ (res != 0);

                ଭାଙ୍ଗ;
        }
#endif

#ଯଦି HAVE_ZLIB
        ସ୍ଥିତି COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                କର {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        ଯଦି (!IN_SET(r, Z_OK, Z_STREAM_END))
                                ଫେରନ୍ତୁ -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } ଯେତେବେଳେ (r != Z_STREAM_END);

                ଭାଙ୍ଗ;
#endif

#ଯଦି HAVE_BZIP2
        ସ୍ଥିତି COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                କର {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (ଶୂନ୍ୟ*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        ଯଦି (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                ଫେରନ୍ତୁ -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } ଯେତେବେଳେ (r != BZ_STREAM_END);

                ଭାଙ୍ଗ;
#endif

        ସ୍ଥିତି COMPRESSION_NONE:
                ଭାଙ୍ଗ;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ -EOPNOTSUPP;
        }

        ଫେରନ୍ତୁ 0;
}
