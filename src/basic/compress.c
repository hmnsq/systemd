/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਜੇ HAVE_XZ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <lzma.h>
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <lz4.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lz4frame.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lz4hc.h>
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <zstd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <zstd_errors.h>
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <zlib.h>
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BZIP2
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <bzlib.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bitfield.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "compress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dlfcn-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unaligned.h"

#ਜੇ HAVE_XZ
ਸਥਿਰ DLSYM_PROTOTYPE(lzma_auto_decoder) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(lzma_ਕੋਡ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(lzma_easy_encoder) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(lzma_ਸਿਰਾ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(lzma_ਧਾਰਾ_ਬਫਰ_encode) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(lzma_ਧਾਰਾ_decoder) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(lzma_lzma_preset) = ਨਿਰਾ;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ lzma_ਸਿਰਾ_wrapper(lzma_ਧਾਰਾ *ls) {
        sym_lzma_ਸਿਰਾ(ls);
}
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_compressBegin) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_compressBound) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4_F_compress_ਸਿਰਾ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_compressUpdate) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_decompress) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4F_isError) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4_compress_HC) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4_compress_default) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4_decompress_ਸੁਰੱਖਿਅਤ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4_decompress_ਸੁਰੱਖਿਅਤ_partial) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(LZ4_ਸੰਸਕਰਣ_ਸੰਖਿਆ) = ਨਿਰਾ;

ਸਥਿਰ ਅਚਲ LZ4F_preferences_t lz4_preferences = {
        .ਚੌਖਟਾ_Info.ਖੰਡ_ਮਾਪ_ID = 5,
};
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_CCtx_ਸੈੱਟ_Parameter) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_compress) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_compressStream2) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_createCCtx) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_createDCtx) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_CStream_ਵਿੱਚ_ਮਾਪ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_CStream_ਬਾਹਰਲਾ_ਮਾਪ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_decompress_ਧਾਰਾ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_DStream_ਵਿੱਚ_ਮਾਪ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_DStream_ਬਾਹਰਲਾ_ਮਾਪ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_freeCCtx) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_freeDCtx) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_get_Error_ਕੋਡ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_getErrorName) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_get_ਚੌਖਟਾ_Content_ਮਾਪ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(ZSTD_isError) = ਨਿਰਾ;

DEFINE_TRIVIAL_CLEANUP_FUNC_ਭਰਿਆ_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, ਨਿਰਾ);

ਸਥਿਰ ਪੂਰਨ zstd_ret_ਮੰਜ਼ਿਲ_errno(size_t ret) {
        ਚੋਣ (sym_ZSTD_get_Error_ਕੋਡ(ret)) {
        ਹਾਲਤ ZSTD_error_dst_ਮਾਪ_too_Small:
                ਵਾਪਸ -ENOBUFS;
        ਹਾਲਤ ZSTD_error_ਸਮ੍ਰਿਤੀ_ਵੰਡ:
                ਵਾਪਸ -ENOMEM;
        ਮੂਲ:
                ਵਾਪਸ -EBADMSG;
        }
}
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
ਸਥਿਰ DLSYM_PROTOTYPE(deflateInit2_) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(deflate) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(deflate_ਸਿਰਾ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(inflateInit2_) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(inflate) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(inflate_ਸਿਰਾ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(inflateReset) = ਨਿਰਾ;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ deflate_ਸਿਰਾ_wrapper(z_ਧਾਰਾ *s) {
        sym_deflate_ਸਿਰਾ(s);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ inflate_ਸਿਰਾ_wrapper(z_ਧਾਰਾ *s) {
        sym_inflate_ਸਿਰਾ(s);
}
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BZIP2
ਸਥਿਰ DLSYM_PROTOTYPE(BZ2_bzCompressInit) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(BZ2_bzCompress) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(BZ2_bz_Compress_ਸਿਰਾ) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(BZ2_bzDecompress) = ਨਿਰਾ;
ਸਥਿਰ DLSYM_PROTOTYPE(BZ2_bz_Decompress_ਸਿਰਾ) = ਨਿਰਾ;

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ BZ2_bz_Compress_ਸਿਰਾ_wrapper(bz_ਧਾਰਾ *s) {
        sym_BZ2_bz_Compress_ਸਿਰਾ(s);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ BZ2_bz_Decompress_ਸਿਰਾ_wrapper(bz_ਧਾਰਾ *s) {
        sym_BZ2_bz_Decompress_ਸਿਰਾ(s);
}
#ਜੇ_ਅੰਤ

/* Opaque Compressor/Decompressor struct definition */
ਬਣਤਰ Compressor {
        Compression type;
        bool encoding;
        bool ਮੁਕੰਮਲ;
        ਸੰਘ {
#ਜੇ HAVE_XZ
                lzma_ਧਾਰਾ xz;
#ਜੇ_ਅੰਤ
#ਜੇ HAVE_LZ4
                ਬਣਤਰ {
                        LZ4F_compressionContext_t c_lz4;
                        ਖਾਲੀ *lz4_ਸਿਰਲੇਖ;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_ਸਿਰਲੇਖ_ਮਾਪ;
                };
                LZ4F_decompressionContext_t d_lz4;
#ਜੇ_ਅੰਤ
#ਜੇ HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#ਜੇ_ਅੰਤ
#ਜੇ HAVE_ZLIB
                z_ਧਾਰਾ gzip;
#ਜੇ_ਅੰਤ
#ਜੇ HAVE_BZIP2
                bz_ਧਾਰਾ bzip2;
#ਜੇ_ਅੰਤ
        };
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALIGN_8(l) ALIGN_ਮੰਜ਼ਿਲ(l, ਆਕਾਰ(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ZLIB_WBITS_GZIP (15 + 16)

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ compression_ਸਾਰਣੀ[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ compression_uppercase_ਸਾਰਣੀ[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ compression_extension_ਸਾਰਣੀ[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "",
        [COMPRESSION_XZ]    = ".xz",
        [COMPRESSION_LZ4]   = ".lz4",
        [COMPRESSION_ZSTD]  = ".zst",
        [COMPRESSION_GZIP]  = ".gz",
        [COMPRESSION_BZIP2] = ".bz2",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(compression, Compression);
DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(compression_uppercase, Compression);
DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(compression_extension, Compression);

Compression compression_from_string_harder(ਅਚਲ ਅੱਖਰ *s) {
        Compression c;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        c = compression_from_string(s);
        ਜੇ (c >= 0)
                ਵਾਪਸ c;

        ਵਾਪਸ compression_uppercase_from_string(s);
}

Compression compression_from_filename(ਅਚਲ ਅੱਖਰ *filename) {
        Compression c;
        ਅਚਲ ਅੱਖਰ *e;

        ਪ੍ਰਤਿਜ੍ਞਾ(filename);

        e = strrchr(filename, '.');
        ਜੇ (!e)
                ਵਾਪਸ COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        ਜੇ (c < 0)
                ਵਾਪਸ COMPRESSION_NONE;

        ਵਾਪਸ c;
}

bool compression_supported_journal(Compression c) {
        ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        ਪ੍ਰਤਿਜ੍ਞਾ(c >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(c < _COMPRESSION_MAX);

        ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(supported, c);
}

bool compression_supported(Compression c) {
        ਸਥਿਰ ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        ਪ੍ਰਤਿਜ੍ਞਾ(c >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(c < _COMPRESSION_MAX);

        ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੈੱਟ(supported, c);
}

Compression compression_detect_from_magic(ਅਚਲ uint8_t ਅੰਕੜੇ[ਸਥਿਰ COMPRESSION_MAGIC_ਬਾਈਟਾਂ_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        ਜੇ (memcmp(ਅੰਕੜੇ, (ਅਚਲ uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                ਵਾਪਸ COMPRESSION_GZIP;
        ਜੇ (memcmp(ਅੰਕੜੇ, (ਅਚਲ uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                ਵਾਪਸ COMPRESSION_XZ;
        ਜੇ (memcmp(ਅੰਕੜੇ, (ਅਚਲ uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                ਵਾਪਸ COMPRESSION_ZSTD;
        ਜੇ (memcmp(ਅੰਕੜੇ, (ਅਚਲ uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                ਵਾਪਸ COMPRESSION_LZ4;
        ਜੇ (memcmp(ਅੰਕੜੇ, (ਅਚਲ uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                ਵਾਪਸ COMPRESSION_BZIP2;

        ਵਾਪਸ _COMPRESSION_INVALID;
}

ਪੂਰਨ dlopen_xz(ਪੂਰਨ log_level) {
#ਜੇ HAVE_XZ
        ਸਥਿਰ ਖਾਲੀ *lzma_dl = ਨਿਰਾ;

        LIBLZMA_NOTE(suggested);

        ਵਾਪਸ dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_ਕੋਡ),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_ਸਿਰਾ),
                        DLSYM_ARG(lzma_ਧਾਰਾ_ਬਫਰ_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_ਧਾਰਾ_decoder));
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ dlopen_lz4(ਪੂਰਨ log_level) {
#ਜੇ HAVE_LZ4
        ਸਥਿਰ ਖਾਲੀ *lz4_dl = ਨਿਰਾ;

        LIBLZ4_NOTE(suggested);

        ਵਾਪਸ dlopen_many_sym_or_warn(
                        &lz4_dl,
                        "liblz4.so.1", log_level,
                        DLSYM_ARG(LZ4F_compressBegin),
                        DLSYM_ARG(LZ4F_compressBound),
                        DLSYM_ARG(LZ4_F_compress_ਸਿਰਾ),
                        DLSYM_ARG(LZ4F_compressUpdate),
                        DLSYM_ARG(LZ4F_createCompressionContext),
                        DLSYM_ARG(LZ4F_createDecompressionContext),
                        DLSYM_ARG(LZ4F_decompress),
                        DLSYM_ARG(LZ4F_freeCompressionContext),
                        DLSYM_ARG(LZ4F_freeDecompressionContext),
                        DLSYM_ARG(LZ4F_isError),
                        DLSYM_ARG(LZ4_compress_default),
                        DLSYM_ARG(LZ4_compress_HC),
                        DLSYM_ARG(LZ4_decompress_ਸੁਰੱਖਿਅਤ),
                        DLSYM_ARG(LZ4_decompress_ਸੁਰੱਖਿਅਤ_partial),
                        DLSYM_ARG(LZ4_ਸੰਸਕਰਣ_ਸੰਖਿਆ));
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ dlopen_zstd(ਪੂਰਨ log_level) {
#ਜੇ HAVE_ZSTD
        ਸਥਿਰ ਖਾਲੀ *zstd_dl = ਨਿਰਾ;

        LIBZSTD_NOTE(suggested);

        ਵਾਪਸ dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_get_Error_ਕੋਡ),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_ਚੌਖਟਾ_Content_ਮਾਪ),
                        DLSYM_ARG(ZSTD_decompress_ਧਾਰਾ),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_ਬਾਹਰਲਾ_ਮਾਪ),
                        DLSYM_ARG(ZSTD_CStream_ਵਿੱਚ_ਮਾਪ),
                        DLSYM_ARG(ZSTD_CStream_ਬਾਹਰਲਾ_ਮਾਪ),
                        DLSYM_ARG(ZSTD_CCtx_ਸੈੱਟ_Parameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_ਵਿੱਚ_ਮਾਪ),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ dlopen_zlib(ਪੂਰਨ log_level) {
#ਜੇ HAVE_ZLIB
        ਸਥਿਰ ਖਾਲੀ *zlib_dl = ਨਿਰਾ;

        LIBZ_NOTE(suggested);

        ਵਾਪਸ dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflate_ਸਿਰਾ),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflate_ਸਿਰਾ),
                        DLSYM_ARG(inflateReset));
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ dlopen_bzip2(ਪੂਰਨ log_level) {
#ਜੇ HAVE_BZIP2
        ਸਥਿਰ ਖਾਲੀ *bzip2_dl = ਨਿਰਾ;

        LIBBZ2_NOTE(suggested);

        ਵਾਪਸ dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bz_Compress_ਸਿਰਾ),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bz_Decompress_ਸਿਰਾ));
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ dlopen_compress_journal(Compression c, ਪੂਰਨ log_level) {
        ਚੋਣ (c) {
        ਹਾਲਤ COMPRESSION_NONE:
                ਵਾਪਸ 0;
        ਹਾਲਤ COMPRESSION_XZ:
                ਵਾਪਸ dlopen_xz(log_level);
        ਹਾਲਤ COMPRESSION_LZ4:
                ਵਾਪਸ dlopen_lz4(log_level);
        ਹਾਲਤ COMPRESSION_ZSTD:
                ਵਾਪਸ dlopen_zstd(log_level);
        ਮੂਲ:
                ਵਾਪਸ -EOPNOTSUPP;
        }
}

ਪੂਰਨ dlopen_compress(Compression c, ਪੂਰਨ log_level) {
        ਚੋਣ (c) {
        ਹਾਲਤ COMPRESSION_GZIP:
                ਵਾਪਸ dlopen_zlib(log_level);
        ਹਾਲਤ COMPRESSION_BZIP2:
                ਵਾਪਸ dlopen_bzip2(log_level);
        ਮੂਲ:
                ਵਾਪਸ dlopen_compress_journal(c, log_level);
        }
}

ਸਥਿਰ ਪੂਰਨ compress_blob_xz(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ *dst,
                size_t dst_alloc_ਮਾਪ,
                size_t *dst_ਮਾਪ,
                ਪੂਰਨ level) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_alloc_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, ਨਿਰਾ, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_ਬਹੁਲਕ_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, ਨਿਰਾ }
        };
        lzma_ret ret;
        size_t ਬਾਹਰਲਾ_pos = 0;
        ਪੂਰਨ r;

        r = dlopen_xz(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                ਜੇ (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        ਵਾਪਸ -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ਜੇ (src_ਮਾਪ < 80)
                ਵਾਪਸ -ENOBUFS;

        ret = sym_lzma_ਧਾਰਾ_ਬਫਰ_encode(filters, LZMA_CHECK_NONE, /* allocator= */ ਨਿਰਾ,
                                            src, src_ਮਾਪ, dst, &ਬਾਹਰਲਾ_pos, dst_alloc_ਮਾਪ);
        ਜੇ (ret != LZMA_OK)
                ਵਾਪਸ -ENOBUFS;

        *dst_ਮਾਪ = ਬਾਹਰਲਾ_pos;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ compress_blob_lz4(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ *dst,
                size_t dst_alloc_ਮਾਪ,
                size_t *dst_ਮਾਪ,
                ਪੂਰਨ level) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_alloc_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_LZ4
        ਪੂਰਨ r;

        r = dlopen_lz4(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ਜੇ (src_ਮਾਪ < 9)
                ਵਾਪਸ -ENOBUFS;

        ਜੇ (src_ਮਾਪ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -EFBIG;
        ਜੇ (dst_alloc_ਮਾਪ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                dst_alloc_ਮਾਪ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;

        ਜੇ (level <= 0)
                r = sym_LZ4_compress_default(src, (ਅੱਖਰ*)dst + 8, src_ਮਾਪ, (ਪੂਰਨ) dst_alloc_ਮਾਪ - 8);
        ਨਹੀਂਤਾਂ
                r = sym_LZ4_compress_HC(src, (ਅੱਖਰ*)dst + 8, src_ਮਾਪ, (ਪੂਰਨ) dst_alloc_ਮਾਪ - 8, level);
        ਜੇ (r <= 0)
                ਵਾਪਸ -ENOBUFS;

        unaligned_write_le64(dst, src_ਮਾਪ);
        *dst_ਮਾਪ = r + 8;

        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ compress_blob_zstd(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ *dst,
                size_t dst_alloc_ਮਾਪ,
                size_t *dst_ਮਾਪ,
                ਪੂਰਨ level) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_alloc_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_ZSTD
        size_t k;
        ਪੂਰਨ r;

        r = dlopen_zstd(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        k = sym_ZSTD_compress(dst, dst_alloc_ਮਾਪ, src, src_ਮਾਪ, level < 0 ? 0 : level);
        ਜੇ (sym_ZSTD_isError(k))
                ਵਾਪਸ zstd_ret_ਮੰਜ਼ਿਲ_errno(k);

        *dst_ਮਾਪ = k;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ compress_blob_gzip(ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                       ਖਾਲੀ *dst, size_t dst_alloc_ਮਾਪ, size_t *dst_ਮਾਪ, ਪੂਰਨ level) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_alloc_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_ZLIB
        ਪੂਰਨ r;

        r = dlopen_zlib(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ > UINT_MAX)
                ਵਾਪਸ -EFBIG;
        ਜੇ (dst_alloc_ਮਾਪ > UINT_MAX)
                dst_alloc_ਮਾਪ = UINT_MAX;

        _cleanup_(deflate_ਸਿਰਾ_wrapper) z_ਧਾਰਾ s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_ਸੰਸਕਰਣ, (ਪੂਰਨ) ਆਕਾਰ(s));
        ਜੇ (r != Z_OK)
                ਵਾਪਸ -ENOMEM;

        s.ਅਗਲਾ_ਵਿੱਚ = (ਖਾਲੀ*) src;
        s.avail_ਵਿੱਚ = src_ਮਾਪ;
        s.ਅਗਲਾ_ਬਾਹਰਲਾ = dst;
        s.avail_ਬਾਹਰਲਾ = dst_alloc_ਮਾਪ;

        r = sym_deflate(&s, Z_ਅੰਤ);
        ਜੇ (r != Z_ਧਾਰਾ_ਸਿਰਾ)
                ਵਾਪਸ -ENOBUFS;

        *dst_ਮਾਪ = dst_alloc_ਮਾਪ - s.avail_ਬਾਹਰਲਾ;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ compress_blob_bzip2(
                ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                ਖਾਲੀ *dst, size_t dst_alloc_ਮਾਪ, size_t *dst_ਮਾਪ, ਪੂਰਨ level) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_alloc_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_BZIP2
        ਪੂਰਨ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ > UINT_MAX)
                ਵਾਪਸ -EFBIG;
        ਜੇ (dst_alloc_ਮਾਪ > UINT_MAX)
                dst_alloc_ਮਾਪ = UINT_MAX;

        _cleanup_(BZ2_bz_Compress_ਸਿਰਾ_wrapper) bz_ਧਾਰਾ s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        ਜੇ (r != BZ_OK)
                ਵਾਪਸ -ENOMEM;

        s.ਅਗਲਾ_ਵਿੱਚ = (ਅੱਖਰ*) src;
        s.avail_ਵਿੱਚ = src_ਮਾਪ;
        s.ਅਗਲਾ_ਬਾਹਰਲਾ = (ਅੱਖਰ*) dst;
        s.avail_ਬਾਹਰਲਾ = dst_alloc_ਮਾਪ;

        r = sym_BZ2_bzCompress(&s, BZ_ਅੰਤ);

        ਜੇ (r != BZ_ਧਾਰਾ_ਸਿਰਾ)
                ਵਾਪਸ -ENOBUFS;

        *dst_ਮਾਪ = dst_alloc_ਮਾਪ - s.avail_ਬਾਹਰਲਾ;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ compress_blob_journal(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                ਖਾਲੀ *dst, size_t dst_alloc_ਮਾਪ, size_t *dst_ਮਾਪ, ਪੂਰਨ level) {

        ਚੋਣ (compression) {
        ਹਾਲਤ COMPRESSION_XZ:
                ਵਾਪਸ compress_blob_xz(src, src_ਮਾਪ, dst, dst_alloc_ਮਾਪ, dst_ਮਾਪ, level);
        ਹਾਲਤ COMPRESSION_LZ4:
                ਵਾਪਸ compress_blob_lz4(src, src_ਮਾਪ, dst, dst_alloc_ਮਾਪ, dst_ਮਾਪ, level);
        ਹਾਲਤ COMPRESSION_ZSTD:
                ਵਾਪਸ compress_blob_zstd(src, src_ਮਾਪ, dst, dst_alloc_ਮਾਪ, dst_ਮਾਪ, level);
        ਮੂਲ:
                ਵਾਪਸ -EOPNOTSUPP;
        }
}

ਪੂਰਨ compress_blob(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                ਖਾਲੀ *dst, size_t dst_alloc_ਮਾਪ, size_t *dst_ਮਾਪ, ਪੂਰਨ level) {

        ਚੋਣ (compression) {
        ਹਾਲਤ COMPRESSION_GZIP:
                ਵਾਪਸ compress_blob_gzip(src, src_ਮਾਪ, dst, dst_alloc_ਮਾਪ, dst_ਮਾਪ, level);
        ਹਾਲਤ COMPRESSION_BZIP2:
                ਵਾਪਸ compress_blob_bzip2(src, src_ਮਾਪ, dst, dst_alloc_ਮਾਪ, dst_ਮਾਪ, level);
        ਮੂਲ:
                ਵਾਪਸ compress_blob_journal(compression, src, src_ਮਾਪ, dst, dst_alloc_ਮਾਪ, dst_ਮਾਪ, level);
        }
}

ਸਥਿਰ ਪੂਰਨ decompress_blob_xz(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **dst,
                size_t *dst_ਮਾਪ,
                size_t dst_max) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_XZ
        ਪੂਰਨ r;

        r = dlopen_xz(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_(lzma_ਸਿਰਾ_wrapper) lzma_ਧਾਰਾ s = LZMA_ਧਾਰਾ_INIT;
        lzma_ret ret = sym_lzma_ਧਾਰਾ_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ਜੇ (ret != LZMA_OK)
                ਵਾਪਸ -ENOMEM;

        size_t space = MIN(src_ਮਾਪ * 2, dst_max ?: SIZE_MAX);
        ਜੇ (!greedy_realloc(dst, space, 1))
                ਵਾਪਸ -ENOMEM;

        s.ਅਗਲਾ_ਵਿੱਚ = src;
        s.avail_ਵਿੱਚ = src_ਮਾਪ;

        s.ਅਗਲਾ_ਬਾਹਰਲਾ = *dst;
        s.avail_ਬਾਹਰਲਾ = space;

        ਲਈ (;;) {
                size_t ਵਰਤਿਆ;

                ret = sym_lzma_ਕੋਡ(&s, LZMA_ਅੰਤ);
                ਜੇ (ret == LZMA_ਧਾਰਾ_ਸਿਰਾ)
                        ਰੋਕੋ;
                ਜੇ (ret != LZMA_OK)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (dst_max > 0 && (space - s.avail_ਬਾਹਰਲਾ) >= dst_max)
                        ਰੋਕੋ;
                ਜੇ (dst_max > 0 && space == dst_max)
                        ਵਾਪਸ -ENOBUFS;

                ਵਰਤਿਆ = space - s.avail_ਬਾਹਰਲਾ;
                /* Silence static analyzers, space is bounded by allocation size */
                ਪ੍ਰਤਿਜ੍ਞਾ(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                ਜੇ (!greedy_realloc(dst, space, 1))
                        ਵਾਪਸ -ENOMEM;

                s.avail_ਬਾਹਰਲਾ = space - ਵਰਤਿਆ;
                s.ਅਗਲਾ_ਬਾਹਰਲਾ = *(uint8_t**)dst + ਵਰਤਿਆ;
        }

        *dst_ਮਾਪ = space - s.avail_ਬਾਹਰਲਾ;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_blob_lz4(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **dst,
                size_t *dst_ਮਾਪ,
                size_t dst_max) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_LZ4
        ਅੱਖਰ* ਬਾਹਰਲਾ;
        ਪੂਰਨ r, ਮਾਪ; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ <= 8)
                ਵਾਪਸ -EBADMSG;

        ਜੇ (src_ਮਾਪ - 8 > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -EFBIG;

        ਮਾਪ = unaligned_read_le64(src);
        ਜੇ (ਮਾਪ < 0 || (ਬਿਨਾਨਿਸ਼ਾਨੀ) ਮਾਪ != unaligned_read_le64(src))
                ਵਾਪਸ -EFBIG;
        ਜੇ (dst_max > 0 && (size_t) ਮਾਪ > dst_max)
                ਵਾਪਸ -ENOBUFS;
        ਬਾਹਰਲਾ = greedy_realloc(dst, ਮਾਪ, 1);
        ਜੇ (!ਬਾਹਰਲਾ)
                ਵਾਪਸ -ENOMEM;

        r = sym_LZ4_decompress_ਸੁਰੱਖਿਅਤ((ਅੱਖਰ*)src + 8, ਬਾਹਰਲਾ, src_ਮਾਪ - 8, ਮਾਪ);
        ਜੇ (r < 0 || r != ਮਾਪ)
                ਵਾਪਸ -EBADMSG;

        *dst_ਮਾਪ = ਮਾਪ;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

size_t zstd_dstream_ਬਾਹਰਲਾ_ਮਾਪ(ਖਾਲੀ) {
#ਜੇ HAVE_ZSTD
        ਜੇ (dlopen_zstd(LOG_DEBUG) < 0)
                ਵਾਪਸ 0;

        ਵਾਪਸ sym_ZSTD_DStream_ਬਾਹਰਲਾ_ਮਾਪ();
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_blob_zstd(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **dst,
                size_t *dst_ਮਾਪ,
                size_t dst_max) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_ZSTD
        ਪੂਰਨ r;

        r = dlopen_zstd(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ ਮਾਪ = sym_ZSTD_get_ਚੌਖਟਾ_Content_ਮਾਪ(src, src_ਮਾਪ);
        ਜੇ (ਮਾਪ == ZSTD_CONTENTSIZE_ERROR)
                ਵਾਪਸ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        ਜੇ (ਮਾਪ != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_ਬਾਹਰਲਾ_ਮਾਪ(), (size_t) MIN(ਮਾਪ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ) cap));
        ਨਹੀਂਤਾਂ {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_ਮਾਪ <= UINT64_MAX / 2 ? src_ਮਾਪ * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_ਬਾਹਰਲਾ_ਮਾਪ(), (size_t) MIN(twice, (uint64_t) cap));
        }

        ਜੇ (!greedy_realloc(dst, space, 1))
                ਵਾਪਸ -ENOMEM;
        space = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ਜੇ (!dctx)
                ਵਾਪਸ -ENOMEM;

        ZSTD_ਵਿੱਚ_ਬਫਰ ਨਿਵੇਸ਼ = {
                .src = src,
                .ਮਾਪ = src_ਮਾਪ,
        };
        ZSTD_ਬਾਹਰਲਾ_ਬਫਰ output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .ਮਾਪ = MIN(space, cap),
        };

        ਲਈ (;;) {
                size_t k = sym_ZSTD_decompress_ਧਾਰਾ(dctx, &output, &ਨਿਵੇਸ਼);
                ਜੇ (sym_ZSTD_isError(k))
                        ਵਾਪਸ log_debug_errno(zstd_ret_ਮੰਜ਼ਿਲ_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                ਜੇ (output.pos >= cap)
                        ਰੋਕੋ;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                ਜੇ (k == 0 && ਨਿਵੇਸ਼.pos >= ਨਿਵੇਸ਼.ਮਾਪ)
                        ਰੋਕੋ;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                ਜੇ (output.pos == output.ਮਾਪ) {
                        ਜੇ (space > SIZE_MAX / 2)
                                ਵਾਪਸ -E2BIG;
                        space = MIN(2 * space, cap);
                        ਜੇ (!greedy_realloc(dst, space, 1))
                                ਵਾਪਸ -ENOMEM;
                        space = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*dst);
                        output.dst = *dst;
                        output.ਮਾਪ = MIN(space, cap);
                        ਜਾਰੀ;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                ਜੇ (ਨਿਵੇਸ਼.pos >= ਨਿਵੇਸ਼.ਮਾਪ)
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_ਮਾਪ = output.pos;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_blob_gzip(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **dst,
                size_t *dst_ਮਾਪ,
                size_t dst_max) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_ZLIB
        ਪੂਰਨ r;

        r = dlopen_zlib(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ > UINT_MAX)
                ਵਾਪਸ -EFBIG;

        _cleanup_(inflate_ਸਿਰਾ_wrapper) z_ਧਾਰਾ s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_ਸੰਸਕਰਣ, (ਪੂਰਨ) ਆਕਾਰ(s));
        ਜੇ (r != Z_OK)
                ਵਾਪਸ -ENOMEM;

        size_t space = MIN3(src_ਮਾਪ * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ਜੇ (!greedy_realloc(dst, space, 1))
                ਵਾਪਸ -ENOMEM;

        s.ਅਗਲਾ_ਵਿੱਚ = (ਖਾਲੀ*) src;
        s.avail_ਵਿੱਚ = src_ਮਾਪ;
        s.ਅਗਲਾ_ਬਾਹਰਲਾ = *dst;
        s.avail_ਬਾਹਰਲਾ = space;

        ਲਈ (;;) {
                size_t ਵਰਤਿਆ;

                r = sym_inflate(&s, Z_NO_FLUSH);
                ਜੇ (r == Z_ਧਾਰਾ_ਸਿਰਾ)
                        ਰੋਕੋ;
                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, Z_OK, Z_BUF_ERROR))
                        ਵਾਪਸ -EBADMSG;

                ਜੇ (dst_max > 0 && (space - s.avail_ਬਾਹਰਲਾ) >= dst_max)
                        ਰੋਕੋ;
                ਜੇ (dst_max > 0 && space == dst_max)
                        ਵਾਪਸ -ENOBUFS;

                ਵਰਤਿਆ = space - s.avail_ਬਾਹਰਲਾ;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                ਜੇ (!greedy_realloc(dst, space, 1))
                        ਵਾਪਸ -ENOMEM;

                s.avail_ਬਾਹਰਲਾ = space - ਵਰਤਿਆ;
                s.ਅਗਲਾ_ਬਾਹਰਲਾ = *(uint8_t**)dst + ਵਰਤਿਆ;
        }

        *dst_ਮਾਪ = space - s.avail_ਬਾਹਰਲਾ;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_blob_bzip2(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **dst,
                size_t *dst_ਮਾਪ,
                size_t dst_max) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst);
        ਪ੍ਰਤਿਜ੍ਞਾ(dst_ਮਾਪ);

#ਜੇ HAVE_BZIP2
        ਪੂਰਨ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ > UINT_MAX)
                ਵਾਪਸ -EFBIG;

        _cleanup_(BZ2_bz_Decompress_ਸਿਰਾ_wrapper) bz_ਧਾਰਾ s = {};

        ਜੇ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ਵਾਪਸ -EIO;

        size_t space = MIN3(src_ਮਾਪ * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ਜੇ (!greedy_realloc(dst, space, 1))
                ਵਾਪਸ -ENOMEM;

        s.ਅਗਲਾ_ਵਿੱਚ = (ਅੱਖਰ*) src;
        s.avail_ਵਿੱਚ = src_ਮਾਪ;
        s.ਅਗਲਾ_ਬਾਹਰਲਾ = (ਅੱਖਰ*) *dst;
        s.avail_ਬਾਹਰਲਾ = space;

        ਲਈ (;;) {
                size_t ਵਰਤਿਆ;

                r = sym_BZ2_bzDecompress(&s);
                ਜੇ (r == BZ_ਧਾਰਾ_ਸਿਰਾ)
                        ਰੋਕੋ;
                ਜੇ (r != BZ_OK)
                        ਵਾਪਸ -EBADMSG;

                ਜੇ (dst_max > 0 && (space - s.avail_ਬਾਹਰਲਾ) >= dst_max)
                        ਰੋਕੋ;
                ਜੇ (dst_max > 0 && space == dst_max)
                        ਵਾਪਸ -ENOBUFS;

                ਵਰਤਿਆ = space - s.avail_ਬਾਹਰਲਾ;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                ਜੇ (!greedy_realloc(dst, space, 1))
                        ਵਾਪਸ -ENOMEM;

                s.avail_ਬਾਹਰਲਾ = space - ਵਰਤਿਆ;
                s.ਅਗਲਾ_ਬਾਹਰਲਾ = (ਅੱਖਰ*) *dst + ਵਰਤਿਆ;
        }

        *dst_ਮਾਪ = space - s.avail_ਬਾਹਰਲਾ;
        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ decompress_blob_journal(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **dst,
                size_t *dst_ਮਾਪ,
                size_t dst_max) {

        ਚੋਣ (compression) {
        ਹਾਲਤ COMPRESSION_XZ:
                ਵਾਪਸ decompress_blob_xz(
                                src, src_ਮਾਪ,
                                dst, dst_ਮਾਪ, dst_max);
        ਹਾਲਤ COMPRESSION_LZ4:
                ਵਾਪਸ decompress_blob_lz4(
                                src, src_ਮਾਪ,
                                dst, dst_ਮਾਪ, dst_max);
        ਹਾਲਤ COMPRESSION_ZSTD:
                ਵਾਪਸ decompress_blob_zstd(
                                src, src_ਮਾਪ,
                                dst, dst_ਮਾਪ, dst_max);
        ਮੂਲ:
                ਵਾਪਸ -EPROTONOSUPPORT;
        }
}

ਪੂਰਨ decompress_blob(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **dst,
                size_t *dst_ਮਾਪ,
                size_t dst_max) {

        ਚੋਣ (compression) {
        ਹਾਲਤ COMPRESSION_GZIP:
                ਵਾਪਸ decompress_blob_gzip(
                                src, src_ਮਾਪ,
                                dst, dst_ਮਾਪ, dst_max);
        ਹਾਲਤ COMPRESSION_BZIP2:
                ਵਾਪਸ decompress_blob_bzip2(
                                src, src_ਮਾਪ,
                                dst, dst_ਮਾਪ, dst_max);
        ਮੂਲ:
                ਵਾਪਸ decompress_blob_journal(
                                compression,
                                src, src_ਮਾਪ,
                                dst, dst_ਮਾਪ, dst_max);
        }
}

ਪੂਰਨ decompress_zlib_raw(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ *dst,
                size_t dst_ਮਾਪ,
                ਪੂਰਨ wbits) {

#ਜੇ HAVE_ZLIB
        ਪੂਰਨ r;

        r = dlopen_zlib(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ > UINT_MAX)
                ਵਾਪਸ -EFBIG;
        ਜੇ (dst_ਮਾਪ > UINT_MAX)
                ਵਾਪਸ -EFBIG;

        _cleanup_(inflate_ਸਿਰਾ_wrapper) z_ਧਾਰਾ s = {
                .ਅਗਲਾ_ਵਿੱਚ = (ਖਾਲੀ*) src,
                .avail_ਵਿੱਚ = src_ਮਾਪ,
                .ਅਗਲਾ_ਬਾਹਰਲਾ = dst,
                .avail_ਬਾਹਰਲਾ = dst_ਮਾਪ,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_ਸੰਸਕਰਣ, (ਪੂਰਨ) ਆਕਾਰ(s));
        ਜੇ (r != Z_OK)
                ਵਾਪਸ -EIO;

        r = sym_inflate(&s, Z_ਅੰਤ);
        size_t produced = (uint8_t*) s.ਅਗਲਾ_ਬਾਹਰਲਾ - (uint8_t*) dst;

        ਜੇ (r != Z_ਧਾਰਾ_ਸਿਰਾ || produced != dst_ਮਾਪ)
                ਵਾਪਸ -EBADMSG;

        ਵਾਪਸ 0;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_startswith_xz(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ,
                size_t ਅਗੇਤਰ_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

#ਜੇ HAVE_XZ
        ਪੂਰਨ r;

        r = dlopen_xz(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_(lzma_ਸਿਰਾ_wrapper) lzma_ਧਾਰਾ s = LZMA_ਧਾਰਾ_INIT;
        lzma_ret ret = sym_lzma_ਧਾਰਾ_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ਜੇ (ret != LZMA_OK)
                ਵਾਪਸ -EBADMSG;

        ਜੇ (!(greedy_realloc(ਬਫਰ, ALIGN_8(ਅਗੇਤਰ_len + 1), 1)))
                ਵਾਪਸ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ);

        s.ਅਗਲਾ_ਵਿੱਚ = src;
        s.avail_ਵਿੱਚ = src_ਮਾਪ;

        s.ਅਗਲਾ_ਬਾਹਰਲਾ = *ਬਫਰ;
        s.avail_ਬਾਹਰਲਾ = allocated;

        ਲਈ (;;) {
                ret = sym_lzma_ਕੋਡ(&s, LZMA_ਅੰਤ);

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(ret, LZMA_OK, LZMA_ਧਾਰਾ_ਸਿਰਾ))
                        ਵਾਪਸ -EBADMSG;

                ਜੇ (allocated - s.avail_ਬਾਹਰਲਾ >= ਅਗੇਤਰ_len + 1)
                        ਵਾਪਸ memcmp(*ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len) == 0 &&
                                ((ਅਚਲ uint8_t*) *ਬਫਰ)[ਅਗੇਤਰ_len] == extra;

                ਜੇ (ret == LZMA_ਧਾਰਾ_ਸਿਰਾ)
                        ਵਾਪਸ 0;

                s.avail_ਬਾਹਰਲਾ += allocated;

                ਜੇ (!(greedy_realloc(ਬਫਰ, allocated * 2, 1)))
                        ਵਾਪਸ -ENOMEM;

                allocated = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ);
                s.ਅਗਲਾ_ਬਾਹਰਲਾ = *(uint8_t**)ਬਫਰ + allocated - s.avail_ਬਾਹਰਲਾ;
        }

#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_startswith_lz4(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ,
                size_t ਅਗੇਤਰ_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

#ਜੇ HAVE_LZ4
        size_t allocated;
        ਪੂਰਨ r;

        r = dlopen_lz4(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ <= 8)
                ਵਾਪਸ -EBADMSG;

        ਜੇ (src_ਮਾਪ - 8 > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -EFBIG;

        ਜੇ (!(greedy_realloc(ਬਫਰ, ALIGN_8(ਅਗੇਤਰ_len + 1), 1)))
                ਵਾਪਸ -ENOMEM;
        allocated = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ);

        r = sym_LZ4_decompress_ਸੁਰੱਖਿਅਤ_partial(
                        (ਅੱਖਰ*)src + 8,
                        *ਬਫਰ,
                        src_ਮਾਪ - 8,
                        ਅਗੇਤਰ_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        ਜੇ (r < 0 || (size_t) r < ਅਗੇਤਰ_len + 1) {
                size_t ਮਾਪ;

                ਜੇ (sym_LZ4_ਸੰਸਕਰਣ_ਸੰਖਿਆ() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        ਵਾਪਸ 0;

                ਜੇ (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        ਜੇ (memcmp(*ਬਫਰ, ਅਗੇਤਰ, r) != 0)
                                ਵਾਪਸ 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_ਮਾਪ, ਬਫਰ, &ਮਾਪ, 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (ਮਾਪ < ਅਗੇਤਰ_len + 1)
                        ਵਾਪਸ 0;
        }

        ਵਾਪਸ memcmp(*ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len) == 0 &&
                ((ਅਚਲ uint8_t*) *ਬਫਰ)[ਅਗੇਤਰ_len] == extra;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_startswith_zstd(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ,
                size_t ਅਗੇਤਰ_len,
                uint8_t extra) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

#ਜੇ HAVE_ZSTD
        ਪੂਰਨ r;

        r = dlopen_zstd(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ ਮਾਪ = sym_ZSTD_get_ਚੌਖਟਾ_Content_ਮਾਪ(src, src_ਮਾਪ);
        ਜੇ (ਮਾਪ == ZSTD_CONTENTSIZE_ERROR)
                ਵਾਪਸ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        ਜੇ (ਮਾਪ != ZSTD_CONTENTSIZE_UNKNOWN && ਮਾਪ < ਅਗੇਤਰ_len + 1)
                ਵਾਪਸ 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ਜੇ (!dctx)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (!(greedy_realloc(ਬਫਰ, MAX(sym_ZSTD_DStream_ਬਾਹਰਲਾ_ਮਾਪ(), ਅਗੇਤਰ_len + 1), 1)))
                ਵਾਪਸ -ENOMEM;

        ZSTD_ਵਿੱਚ_ਬਫਰ ਨਿਵੇਸ਼ = {
                .src = src,
                .ਮਾਪ = src_ਮਾਪ,
        };
        ZSTD_ਬਾਹਰਲਾ_ਬਫਰ output = {
                .dst = *ਬਫਰ,
                .ਮਾਪ = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ),
        };
        size_t k;

        k = sym_ZSTD_decompress_ਧਾਰਾ(dctx, &output, &ਨਿਵੇਸ਼);
        ਜੇ (sym_ZSTD_isError(k))
                ਵਾਪਸ log_debug_errno(zstd_ret_ਮੰਜ਼ਿਲ_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        ਜੇ (output.pos < ਅਗੇਤਰ_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                ਜੇ (k != 0)
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                ਵਾਪਸ 0; /* Decompressed text too short to match the prefix and extra */
        }

        ਵਾਪਸ memcmp(*ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len) == 0 &&
                ((ਅਚਲ uint8_t*) *ਬਫਰ)[ਅਗੇਤਰ_len] == extra;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_startswith_gzip(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ,
                size_t ਅਗੇਤਰ_len,
                uint8_t extra) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

#ਜੇ HAVE_ZLIB
        ਪੂਰਨ r;

        r = dlopen_zlib(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ > UINT_MAX)
                ਵਾਪਸ -EFBIG;

        _cleanup_(inflate_ਸਿਰਾ_wrapper) z_ਧਾਰਾ s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_ਸੰਸਕਰਣ, (ਪੂਰਨ) ਆਕਾਰ(s));
        ਜੇ (r != Z_OK)
                ਵਾਪਸ -EBADMSG;

        ਜੇ (!(greedy_realloc(ਬਫਰ, ALIGN_8(ਅਗੇਤਰ_len + 1), 1)))
                ਵਾਪਸ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ);

        s.ਅਗਲਾ_ਵਿੱਚ = (ਖਾਲੀ*) src;
        s.avail_ਵਿੱਚ = src_ਮਾਪ;

        s.ਅਗਲਾ_ਬਾਹਰਲਾ = *ਬਫਰ;
        s.avail_ਬਾਹਰਲਾ = MIN(allocated, (size_t) UINT_MAX);

        ਲਈ (;;) {
                r = sym_inflate(&s, Z_ਅੰਤ);

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, Z_OK, Z_ਧਾਰਾ_ਸਿਰਾ, Z_BUF_ERROR))
                        ਵਾਪਸ -EBADMSG;

                ਜੇ (allocated - s.avail_ਬਾਹਰਲਾ >= ਅਗੇਤਰ_len + 1)
                        ਵਾਪਸ memcmp(*ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len) == 0 &&
                                ((ਅਚਲ uint8_t*) *ਬਫਰ)[ਅਗੇਤਰ_len] == extra;

                ਜੇ (r == Z_ਧਾਰਾ_ਸਿਰਾ)
                        ਵਾਪਸ 0;

                ਜੇ (s.avail_ਬਾਹਰਲਾ > 0)
                        ਵਾਪਸ -EBADMSG; /* truncated */

                size_t ਵਰਤਿਆ = allocated - s.avail_ਬਾਹਰਲਾ;

                ਜੇ (!(greedy_realloc(ਬਫਰ, allocated * 2, 1)))
                        ਵਾਪਸ -ENOMEM;

                allocated = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ);
                s.avail_ਬਾਹਰਲਾ = MIN(allocated - ਵਰਤਿਆ, (size_t) UINT_MAX);
                s.ਅਗਲਾ_ਬਾਹਰਲਾ = *(uint8_t**)ਬਫਰ + ਵਰਤਿਆ;
        }
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ decompress_startswith_bzip2(
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ,
                size_t ਅਗੇਤਰ_len,
                uint8_t extra) {

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(src_ਮਾਪ > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

#ਜੇ HAVE_BZIP2
        ਪੂਰਨ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (src_ਮਾਪ > UINT_MAX)
                ਵਾਪਸ -EFBIG;

        _cleanup_(BZ2_bz_Decompress_ਸਿਰਾ_wrapper) bz_ਧਾਰਾ s = {};

        ਜੇ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ਵਾਪਸ -EIO;

        ਜੇ (!(greedy_realloc(ਬਫਰ, ALIGN_8(ਅਗੇਤਰ_len + 1), 1)))
                ਵਾਪਸ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ);

        s.ਅਗਲਾ_ਵਿੱਚ = (ਅੱਖਰ*) src;
        s.avail_ਵਿੱਚ = src_ਮਾਪ;

        s.ਅਗਲਾ_ਬਾਹਰਲਾ = *ਬਫਰ;
        s.avail_ਬਾਹਰਲਾ = MIN(allocated, (size_t) UINT_MAX);

        ਲਈ (;;) {
                r = sym_BZ2_bzDecompress(&s);

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, BZ_OK, BZ_ਧਾਰਾ_ਸਿਰਾ))
                        ਵਾਪਸ -EBADMSG;

                ਜੇ (allocated - s.avail_ਬਾਹਰਲਾ >= ਅਗੇਤਰ_len + 1)
                        ਵਾਪਸ memcmp(*ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len) == 0 &&
                                ((ਅਚਲ uint8_t*) *ਬਫਰ)[ਅਗੇਤਰ_len] == extra;

                ਜੇ (r == BZ_ਧਾਰਾ_ਸਿਰਾ)
                        ਵਾਪਸ 0;

                ਜੇ (s.avail_ਬਾਹਰਲਾ > 0)
                        ਵਾਪਸ -EBADMSG; /* truncated */

                size_t ਵਰਤਿਆ = allocated - s.avail_ਬਾਹਰਲਾ;

                ਜੇ (!(greedy_realloc(ਬਫਰ, allocated * 2, 1)))
                        ਵਾਪਸ -ENOMEM;

                allocated = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ);
                s.avail_ਬਾਹਰਲਾ = MIN(allocated - ਵਰਤਿਆ, (size_t) UINT_MAX);
                s.ਅਗਲਾ_ਬਾਹਰਲਾ = (ਅੱਖਰ*) *ਬਫਰ + ਵਰਤਿਆ;
        }
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ -EPROTONOSUPPORT;
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ decompress_startswith_journal(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ, size_t ਅਗੇਤਰ_len,
                uint8_t extra) {

        ਚੋਣ (compression) {
        ਹਾਲਤ COMPRESSION_XZ:
                ਵਾਪਸ decompress_startswith_xz(src, src_ਮਾਪ, ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len, extra);
        ਹਾਲਤ COMPRESSION_LZ4:
                ਵਾਪਸ decompress_startswith_lz4(src, src_ਮਾਪ, ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len, extra);
        ਹਾਲਤ COMPRESSION_ZSTD:
                ਵਾਪਸ decompress_startswith_zstd(src, src_ਮਾਪ, ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len, extra);
        ਮੂਲ:
                ਵਾਪਸ -EOPNOTSUPP;
        }
}

ਪੂਰਨ decompress_startswith(
                Compression compression,
                ਅਚਲ ਖਾਲੀ *src, uint64_t src_ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                ਅਚਲ ਖਾਲੀ *ਅਗੇਤਰ, size_t ਅਗੇਤਰ_len,
                uint8_t extra) {

        ਚੋਣ (compression) {
        ਹਾਲਤ COMPRESSION_GZIP:
                ਵਾਪਸ decompress_startswith_gzip(src, src_ਮਾਪ, ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len, extra);
        ਹਾਲਤ COMPRESSION_BZIP2:
                ਵਾਪਸ decompress_startswith_bzip2(src, src_ਮਾਪ, ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len, extra);
        ਮੂਲ:
                ਵਾਪਸ decompress_startswith_journal(compression, src, src_ਮਾਪ, ਬਫਰ, ਅਗੇਤਰ, ਅਗੇਤਰ_len, extra);
        }
}

ਪੂਰਨ compress_ਧਾਰਾ(
                Compression type,
                ਪੂਰਨ fdf, ਪੂਰਨ fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_ਮਾਪ) {

        _cleanup_(compressor_freep) Compressor *c = ਨਿਰਾ;
        _cleanup_free_ ਖਾਲੀ *buf = ਨਿਰਾ;
        _cleanup_free_ uint8_t *ਨਿਵੇਸ਼ = ਨਿਰਾ;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t ਕੁੱਲ_ਵਿੱਚ = 0, ਕੁੱਲ_ਬਾਹਰਲਾ = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fdf >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(fdt >= 0);

        r = compressor_new(&c, type);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਨਿਵੇਸ਼ = new(uint8_t, COMPRESS_PIPE_ਬਫਰ_ਮਾਪ);
        ਜੇ (!ਨਿਵੇਸ਼)
                ਵਾਪਸ -ENOMEM;

        ਲਈ (;;) {
                size_t m = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ;
                ssize_t n;

                ਜੇ (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = ਪਠ(fdf, ਨਿਵੇਸ਼, m);
                ਜੇ (n < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (n == 0) {
                        r = compressor_ਅੰਤ(c, &buf, &buf_size, &buf_alloc);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ਜੇ (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                                ਕੁੱਲ_ਬਾਹਰਲਾ += buf_size;
                        }
                        ਰੋਕੋ;
                }

                ਕੁੱਲ_ਵਿੱਚ += n;
                ਜੇ (max_bytes != UINT64_MAX) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_ਸ਼ੁਰੂ(c, ਨਿਵੇਸ਼, n, &buf, &buf_size, &buf_alloc);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਕੁੱਲ_ਬਾਹਰਲਾ += buf_size;
                }
        }

        ਜੇ (ret_uncompressed_ਮਾਪ)
                *ret_uncompressed_ਮਾਪ = ਕੁੱਲ_ਵਿੱਚ;

        ਜੇ (ਕੁੱਲ_ਵਿੱਚ == 0)
                log_debug("%s compression finished (no input data)", compression_ਮੰਜ਼ਿਲ_string(type));
        ਨਹੀਂਤਾਂ
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_ਮੰਜ਼ਿਲ_string(type), ਕੁੱਲ_ਵਿੱਚ, ਕੁੱਲ_ਬਾਹਰਲਾ, (ਦੁੱਗਣਾ) ਕੁੱਲ_ਬਾਹਰਲਾ / ਕੁੱਲ_ਵਿੱਚ * 100);

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ should_sparse(ਪੂਰਨ fd) {
        ਬਣਤਰ stat st;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (!S_ISREG(st.st_mode))
                ਵਾਪਸ false;

        ਪੂਰਨ flags = fcntl(fd, F_GETFL);
        ਜੇ (flags < 0)
                ਵਾਪਸ -errno;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, O_APPEND))
                ਵਾਪਸ false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        ਜੇ (pos < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
ਸਥਿਰ ਪੂਰਨ finalize_sparse(ਪੂਰਨ fd) {
        off_t pos;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        ਜੇ (pos < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ftruncate(fd, pos) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 0;
}

/* Common helper for decompress_stream_*() wrappers */

ਬਣਤਰ decompress_ਧਾਰਾ_userdata {
        ਪੂਰਨ fd;
        uint64_t max_bytes;
        uint64_t ਕੁੱਲ_ਬਾਹਰਲਾ;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ਸਥਿਰ ਪੂਰਨ decompress_ਧਾਰਾ_write_callback(ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t ਮਾਪ, ਖਾਲੀ *userdata) {
        ਬਣਤਰ decompress_ਧਾਰਾ_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        ਜੇ (u->max_bytes != UINT64_MAX) {
                ਜੇ (u->max_bytes < ਮਾਪ)
                        ਵਾਪਸ -EFBIG;
                u->max_bytes -= ਮਾਪ;
        }

        u->ਕੁੱਲ_ਬਾਹਰਲਾ += ਮਾਪ;

        ਜੇ (u->skip > 0) {
                ਜੇ (u->skip >= ਮਾਪ) {
                        u->skip -= ਮਾਪ;
                        ਵਾਪਸ 0;
                }

                ਅੰਕੜੇ = (ਅਚਲ uint8_t*) ਅੰਕੜੇ + u->skip;
                ਮਾਪ -= u->skip;
                u->skip = 0;
        }

        ਜੇ (ਮਾਪ == 0)
                ਵਾਪਸ 0;

        ਜੇ (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, ਅੰਕੜੇ, ਮਾਪ, 64);
                ਜੇ (k < 0)
                        ਵਾਪਸ (ਪੂਰਨ) k;
                ਵਾਪਸ 0;
        }

        ਵਾਪਸ loop_write(u->fd, ਅੰਕੜੇ, ਮਾਪ);
}

ਸਥਿਰ ਪੂਰਨ decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        ਚੋਣ (type) {
        ਹਾਲਤ COMPRESSION_NONE:
                ਰੋਕੋ;

#ਜੇ HAVE_XZ
        ਹਾਲਤ COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_ਸੈੱਟ(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_ਸੈੱਟ(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_ਧਾਰਾ_decoder(&c->xz, UINT64_MAX, xz_flags);
                ਜੇ (xz_ret != LZMA_OK)
                        ਵਾਪਸ -EIO;
                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
        ਹਾਲਤ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4_F_ਸੰਸਕਰਣ);
                ਜੇ (sym_LZ4F_isError(rc))
                        ਵਾਪਸ -ENOMEM;

                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
        ਹਾਲਤ COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                ਜੇ (!c->d_zstd)
                        ਵਾਪਸ -ENOMEM;
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
        ਹਾਲਤ COMPRESSION_GZIP:
                ਜੇ (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_ਸੰਸਕਰਣ, (ਪੂਰਨ) ਆਕਾਰ(c->gzip)) != Z_OK)
                        ਵਾਪਸ -EIO;
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BZIP2
        ਹਾਲਤ COMPRESSION_BZIP2:
                ਜੇ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        ਵਾਪਸ -EIO;
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

        ਮੂਲ:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ decompress_blob_ਮੰਜ਼ਿਲ_fd_impl(
                Compression type,
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਪੂਰਨ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ਮਾਪ) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(src);
        ਪ੍ਰਤਿਜ੍ਞਾ(fdt >= 0);

        ਜੇ (type == COMPRESSION_NONE)
                ਵਾਪਸ -EOPNOTSUPP;

        ਜੇ (src_ਮਾਪ == 0)
                ਵਾਪਸ -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        ਜੇ (type == COMPRESSION_LZ4 &&
            src_ਮਾਪ >= COMPRESSION_MAGIC_ਬਾਈਟਾਂ_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                ਜੇ (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        ਜੇ (src_ਮਾਪ <= ਆਕਾਰ(uint64_t))
                                ਵਾਪਸ -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        ਜੇ (sz < skip)
                                ਵਾਪਸ -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                ਜੇ (max_bytes == 0)
                        ਵਾਪਸ -ENOBUFS;

                _cleanup_free_ ਖਾਲੀ *buf = ਨਿਰਾ;
                size_t ਮਾਪ = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_ਮਾਪ, &buf, &ਮਾਪ,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                /* Check the result size again for safety. */
                ਜੇ (ਮਾਪ < skip)
                        ਵਾਪਸ -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, ਮਾਪ - skip);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_ਮਾਪ, ਮਾਪ, (ਦੁੱਗਣਾ) ਮਾਪ / src_ਮਾਪ * 100);

                ਜੇ (ret_ਮਾਪ)
                        *ret_ਮਾਪ = ਮਾਪ - skip;
                ਵਾਪਸ 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = ਨਿਰਾ;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਬਣਤਰ decompress_ਧਾਰਾ_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ਧੱਕੋ(c, src, src_ਮਾਪ, decompress_ਧਾਰਾ_write_callback, &userdata);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Finalize */
        r = decompressor_ਧੱਕੋ(c, /* data= */ ਨਿਰਾ, /* size= */ 0, decompress_ਧਾਰਾ_write_callback, &userdata);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (userdata.ਕੁੱਲ_ਬਾਹਰਲਾ < skip)
                ਵਾਪਸ -EBADMSG;

        ਜੇ (userdata.sparse) {
                r = finalize_sparse(fdt);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_ਮੰਜ਼ਿਲ_string(type), src_ਮਾਪ, userdata.ਕੁੱਲ_ਬਾਹਰਲਾ,
                  (ਦੁੱਗਣਾ) userdata.ਕੁੱਲ_ਬਾਹਰਲਾ / src_ਮਾਪ * 100);

        ਜੇ (ret_ਮਾਪ)
                *ret_ਮਾਪ = userdata.ਕੁੱਲ_ਬਾਹਰਲਾ - skip;

        ਵਾਪਸ 0;
}

ਪੂਰਨ decompress_blob_ਮੰਜ਼ਿਲ_fd_journal(
                Compression type,
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਪੂਰਨ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ਮਾਪ) {

        ਪੂਰਨ r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ decompress_blob_ਮੰਜ਼ਿਲ_fd_impl(type, src, src_ਮਾਪ, fdt, max_bytes, skip, ret_ਮਾਪ);
}

ਪੂਰਨ decompress_blob_ਮੰਜ਼ਿਲ_fd(
                Compression type,
                ਅਚਲ ਖਾਲੀ *src,
                uint64_t src_ਮਾਪ,
                ਪੂਰਨ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ਮਾਪ) {

        ਪੂਰਨ r;

        r = dlopen_compress(type, LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ decompress_blob_ਮੰਜ਼ਿਲ_fd_impl(type, src, src_ਮਾਪ, fdt, max_bytes, skip, ret_ਮਾਪ);
}

ਪੂਰਨ decompress_ਧਾਰਾ_ਭਰਿਆ(
                Compression type,
                ਪੂਰਨ fdf, ਪੂਰਨ fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = ਨਿਰਾ;
        _cleanup_free_ uint8_t *buf = ਨਿਰਾ;
        uint64_t ਕੁੱਲ_ਵਿੱਚ = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fdf >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(fdt >= 0);

        ਜੇ (type == COMPRESSION_NONE)
                ਵਾਪਸ -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = decompressor_new(&c, type, flags);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਬਣਤਰ decompress_ਧਾਰਾ_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_ਬਫਰ_ਮਾਪ);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        ਲਈ (;;) {
                ssize_t n;

                n = ਪਠ(fdf, buf, COMPRESS_PIPE_ਬਫਰ_ਮਾਪ);
                ਜੇ (n < 0)
                        ਵਾਪਸ -errno;

                ਕੁੱਲ_ਵਿੱਚ += n;

                r = decompressor_ਧੱਕੋ(c, buf, n, decompress_ਧਾਰਾ_write_callback, &userdata);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (n == 0)
                        ਰੋਕੋ;
        }

        ਜੇ (ਕੁੱਲ_ਵਿੱਚ == 0)
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_ਮੰਜ਼ਿਲ_string(type));

        ਜੇ (userdata.sparse) {
                r = finalize_sparse(fdt);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_ਮੰਜ਼ਿਲ_string(type), ਕੁੱਲ_ਵਿੱਚ, userdata.ਕੁੱਲ_ਬਾਹਰਲਾ,
                  (ਦੁੱਗਣਾ) userdata.ਕੁੱਲ_ਬਾਹਰਲਾ / ਕੁੱਲ_ਵਿੱਚ * 100);

        ਵਾਪਸ 0;
}

ਪੂਰਨ decompress_ਧਾਰਾ_by_filename(ਅਚਲ ਅੱਖਰ *filename, ਪੂਰਨ fdf, ਪੂਰਨ fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        ਜੇ (c == COMPRESSION_NONE)
                ਵਾਪਸ -EPROTONOSUPPORT;

        ਵਾਪਸ decompress_ਧਾਰਾ(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        ਜੇ (!c)
                ਵਾਪਸ ਨਿਰਾ;

        ਚੋਣ (c->type) {

#ਜੇ HAVE_XZ
        ਹਾਲਤ COMPRESSION_XZ:
                sym_lzma_ਸਿਰਾ(&c->xz);
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
        ਹਾਲਤ COMPRESSION_LZ4:
                ਜੇ (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = ਨਿਰਾ;
                        c->lz4_ਸਿਰਲੇਖ = mfree(c->lz4_ਸਿਰਲੇਖ);
                } ਨਹੀਂਤਾਂ {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = ਨਿਰਾ;
                }
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
        ਹਾਲਤ COMPRESSION_ZSTD:
                ਜੇ (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = ਨਿਰਾ;
                } ਨਹੀਂਤਾਂ {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = ਨਿਰਾ;
                }
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
        ਹਾਲਤ COMPRESSION_GZIP:
                ਜੇ (c->encoding)
                        sym_deflate_ਸਿਰਾ(&c->gzip);
                ਨਹੀਂਤਾਂ
                        sym_inflate_ਸਿਰਾ(&c->gzip);
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BZIP2
        ਹਾਲਤ COMPRESSION_BZIP2:
                ਜੇ (c->encoding)
                        sym_BZ2_bz_Compress_ਸਿਰਾ(&c->bzip2);
                ਨਹੀਂਤਾਂ
                        sym_BZ2_bz_Decompress_ਸਿਰਾ(&c->bzip2);
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

        ਮੂਲ:
                ਰੋਕੋ;
        }

        ਵਾਪਸ mfree(c);
}

Compression compressor_type(ਅਚਲ Compressor *c) {
        ਵਾਪਸ c ? c->type : _COMPRESSION_INVALID;
}

ਪੂਰਨ decompressor_detect(Decompressor **decompressor, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t ਮਾਪ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(decompressor);

        ਜੇ (*decompressor)
                ਵਾਪਸ 1;

        ਜੇ (ਮਾਪ < COMPRESSION_MAGIC_ਬਾਈਟਾਂ_MAX)
                ਵਾਪਸ 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ);

        Compression type = compression_detect_from_magic(ਅੰਕੜੇ);
        ਜੇ (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_ਮੰਜ਼ਿਲ_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 1;
}

ਪੂਰਨ decompressor_force(Decompressor **decompressor, Compression type) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(decompressor);
        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *decompressor = compressor_free(*decompressor);
        ਵਾਪਸ decompressor_new(decompressor, type, /* flags= */ 0);
}

ਪੂਰਨ decompressor_force_off(Decompressor **decompressor) {
        ਵਾਪਸ decompressor_force(decompressor, COMPRESSION_NONE);
}

ਪੂਰਨ decompressor_ਧੱਕੋ(Decompressor *c, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, size_t ਮਾਪ, DecompressorCallback callback, ਖਾਲੀ *userdata) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(c);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ || ਮਾਪ == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        ਜੇ (c->encoding)
                ਵਾਪਸ -EINVAL;

        ਜੇ (c->ਮੁਕੰਮਲ && ਮਾਪ == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                ਵਾਪਸ 0;

#ਜੇ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *ਬਫਰ = ਨਿਰਾ;
        ਜੇ (c->type != COMPRESSION_NONE) {
                ਬਫਰ = new(uint8_t, COMPRESS_PIPE_ਬਫਰ_ਮਾਪ);
                ਜੇ (!ਬਫਰ)
                        ਵਾਪਸ -ENOMEM;
        }
#ਜੇ_ਅੰਤ

        ਚੋਣ (c->type) {

        ਹਾਲਤ COMPRESSION_NONE:
                ਜੇ (c->ਮੁਕੰਮਲ)
                        ਵਾਪਸ -EBADMSG;

                ਜੇ (ਮਾਪ == 0) {
                        c->ਮੁਕੰਮਲ = true;
                        ਵਾਪਸ 0;
                }

                r = callback(ਅੰਕੜੇ, ਮਾਪ, userdata);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਵਾਪਸ 0;

#ਜੇ HAVE_XZ
        ਹਾਲਤ COMPRESSION_XZ:
                ਜੇ (c->ਮੁਕੰਮਲ)
                        ਵਾਪਸ -EBADMSG;

                c->xz.ਅਗਲਾ_ਵਿੱਚ = ਅੰਕੜੇ;
                c->xz.avail_ਵਿੱਚ = ਮਾਪ;

                ਲਈ (;;) {
                        c->xz.ਅਗਲਾ_ਬਾਹਰਲਾ = ਬਫਰ;
                        c->xz.avail_ਬਾਹਰਲਾ = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ;

                        lzma_ret lzr = sym_lzma_ਕੋਡ(&c->xz, ਮਾਪ == 0 ? LZMA_ਅੰਤ : LZMA_ਲੜੀਵਾਰ);
                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(lzr, LZMA_OK, LZMA_ਧਾਰਾ_ਸਿਰਾ))
                                ਵਾਪਸ -EBADMSG;

                        ਜੇ (c->xz.avail_ਬਾਹਰਲਾ > COMPRESS_PIPE_ਬਫਰ_ਮਾਪ)
                                ਵਾਪਸ -EIO;

                        size_t produced = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ - c->xz.avail_ਬਾਹਰਲਾ;
                        ਜੇ (produced > 0) {
                                r = callback(ਬਫਰ, produced, userdata);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }

                        ਜੇ (lzr == LZMA_ਧਾਰਾ_ਸਿਰਾ) {
                                ਜੇ (c->xz.avail_ਵਿੱਚ > 0)
                                        ਵਾਪਸ -EBADMSG; /* garbage data at the end */

                                c->ਮੁਕੰਮਲ = true;
                                ਵਾਪਸ 0;
                        }

                        ਜੇ (c->xz.avail_ਵਿੱਚ == 0 && c->xz.avail_ਬਾਹਰਲਾ > 0) {
                                ਜੇ (ਮਾਪ == 0)
                                        ਵਾਪਸ -EBADMSG; /* truncated */

                                ਵਾਪਸ 0; /* No progress possible with the current input. */
                        }
                }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
        ਹਾਲਤ COMPRESSION_LZ4: {
                ਅਚਲ uint8_t *src = ਅੰਕੜੇ;
                size_t src_ਬਚਿਆ = ਮਾਪ;

                c->ਮੁਕੰਮਲ = false;

                ਲਈ (;;) {
                        size_t produced = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ;
                        size_t consumed = src_ਬਚਿਆ;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, ਬਫਰ, &produced, src, &consumed, ਨਿਰਾ);
                        ਜੇ (sym_LZ4F_isError(rc))
                                ਵਾਪਸ -EBADMSG;

                        ਜੇ (consumed > src_ਬਚਿਆ ||
                            produced > COMPRESS_PIPE_ਬਫਰ_ਮਾਪ)
                                ਵਾਪਸ -EIO;

                        ਜੇ (consumed > 0) {
                                ਪ੍ਰਤਿਜ੍ਞਾ(src);
                                src += consumed;
                                src_ਬਚਿਆ -= consumed;
                        }

                        ਜੇ (produced > 0) {
                                r = callback(ਬਫਰ, produced, userdata);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }

                        ਜੇ (src_ਬਚਿਆ == 0) {
                                ਜੇ (rc == 0) {
                                        /* The current stream has finished. */
                                        c->ਮੁਕੰਮਲ = true;
                                        ਵਾਪਸ 0;
                                }

                                ਜੇ (produced == COMPRESS_PIPE_ਬਫਰ_ਮਾਪ)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ਜਾਰੀ;

                                ਜੇ (ਮਾਪ == 0)
                                        ਵਾਪਸ -EBADMSG; /* truncated */

                                ਵਾਪਸ 0; /* No progress possible with the current input. */
                        }
                }
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
        ਹਾਲਤ COMPRESSION_ZSTD: {
                ZSTD_ਵਿੱਚ_ਬਫਰ ਨਿਵੇਸ਼ = {
                        .src =  (ਖਾਲੀ*) ਅੰਕੜੇ,
                        .ਮਾਪ = ਮਾਪ,
                };

                c->ਮੁਕੰਮਲ = false;

                ਲਈ (;;) {
                        ZSTD_ਬਾਹਰਲਾ_ਬਫਰ output = {
                                .dst = ਬਫਰ,
                                .ਮਾਪ = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ,
                        };

                        size_t res = sym_ZSTD_decompress_ਧਾਰਾ(c->d_zstd, &output, &ਨਿਵੇਸ਼);
                        ਜੇ (sym_ZSTD_isError(res))
                                ਵਾਪਸ -EBADMSG;

                        ਜੇ (ਨਿਵੇਸ਼.pos > ਨਿਵੇਸ਼.ਮਾਪ ||
                            output.pos > output.ਮਾਪ)
                                ਵਾਪਸ -EIO;

                        ਜੇ (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }

                        ਜੇ (ਨਿਵੇਸ਼.pos == ਨਿਵੇਸ਼.ਮਾਪ) {
                                ਜੇ (res == 0) {
                                        /* The current stream has finished. */
                                        c->ਮੁਕੰਮਲ = true;
                                        ਵਾਪਸ 0;
                                }

                                ਜੇ (output.pos == COMPRESS_PIPE_ਬਫਰ_ਮਾਪ)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ਜਾਰੀ;

                                ਜੇ (ਮਾਪ == 0)
                                        ਵਾਪਸ -EBADMSG; /* truncated */

                                ਵਾਪਸ 0; /* No progress possible with the current input. */
                        }
                }
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
        ਹਾਲਤ COMPRESSION_GZIP:
                ਜੇ (ਮਾਪ > UINT_MAX)
                        ਵਾਪਸ -EFBIG;

                c->gzip.ਅਗਲਾ_ਵਿੱਚ = (ਖਾਲੀ*) ਅੰਕੜੇ;
                c->gzip.avail_ਵਿੱਚ = ਮਾਪ;

                ਲਈ (;;) {
                        ਜੇ (c->ਮੁਕੰਮਲ) {
                                ਜੇ (sym_inflateReset(&c->gzip) != Z_OK)
                                        ਵਾਪਸ -EIO;

                                c->ਮੁਕੰਮਲ = false;
                        }

                        c->gzip.ਅਗਲਾ_ਬਾਹਰਲਾ = ਬਫਰ;
                        c->gzip.avail_ਬਾਹਰਲਾ = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        ਪੂਰਨ zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(zr, Z_OK, Z_ਧਾਰਾ_ਸਿਰਾ) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_ਵਿੱਚ == 0))
                                ਵਾਪਸ -EBADMSG;

                        ਜੇ (c->gzip.avail_ਬਾਹਰਲਾ > COMPRESS_PIPE_ਬਫਰ_ਮਾਪ)
                                ਵਾਪਸ -EIO;

                        size_t produced = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ - c->gzip.avail_ਬਾਹਰਲਾ;
                        ਜੇ (produced > 0) {
                                r = callback(ਬਫਰ, produced, userdata);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }

                        ਜੇ (zr == Z_ਧਾਰਾ_ਸਿਰਾ) {
                                c->ਮੁਕੰਮਲ = true;

                                ਜੇ (c->gzip.avail_ਵਿੱਚ > 0)
                                        ਜਾਰੀ; /* New stream may be pending. */

                                ਵਾਪਸ 0;
                        }

                        ਜੇ (c->gzip.avail_ਵਿੱਚ == 0 && c->gzip.avail_ਬਾਹਰਲਾ > 0) {
                                ਜੇ (ਮਾਪ == 0)
                                        ਵਾਪਸ -EBADMSG; /* truncated */

                                ਵਾਪਸ 0; /* No progress possible with the current input. */
                        }
                }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BZIP2
        ਹਾਲਤ COMPRESSION_BZIP2:
                ਜੇ (ਮਾਪ > UINT_MAX)
                        ਵਾਪਸ -EFBIG;

                c->bzip2.ਅਗਲਾ_ਵਿੱਚ = (ਅੱਖਰ*) ਅੰਕੜੇ;
                c->bzip2.avail_ਵਿੱਚ = ਮਾਪ;

                ਲਈ (;;) {
                        ਜੇ (c->ਮੁਕੰਮਲ) {
                                sym_BZ2_bz_Decompress_ਸਿਰਾ(&c->bzip2);
                                ਜੇ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        ਵਾਪਸ -EIO;

                                c->ਮੁਕੰਮਲ = false;
                        }

                        c->bzip2.ਅਗਲਾ_ਬਾਹਰਲਾ = (ਅੱਖਰ*) ਬਫਰ;
                        c->bzip2.avail_ਬਾਹਰਲਾ = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ;

                        ਪੂਰਨ bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(bzr, BZ_OK, BZ_ਧਾਰਾ_ਸਿਰਾ))
                                ਵਾਪਸ -EBADMSG;

                        ਜੇ (c->bzip2.avail_ਬਾਹਰਲਾ > COMPRESS_PIPE_ਬਫਰ_ਮਾਪ)
                                ਵਾਪਸ -EIO;

                        size_t produced = COMPRESS_PIPE_ਬਫਰ_ਮਾਪ - c->bzip2.avail_ਬਾਹਰਲਾ;
                        ਜੇ (produced > 0) {
                                r = callback(ਬਫਰ, produced, userdata);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }

                        ਜੇ (bzr == BZ_ਧਾਰਾ_ਸਿਰਾ) {
                                c->ਮੁਕੰਮਲ = true;

                                ਜੇ (c->bzip2.avail_ਵਿੱਚ > 0)
                                        ਜਾਰੀ; /* New stream may be pending. */

                                ਵਾਪਸ 0;
                        }

                        ਜੇ (c->bzip2.avail_ਵਿੱਚ == 0 && c->bzip2.avail_ਬਾਹਰਲਾ > 0) {
                                ਜੇ (ਮਾਪ == 0)
                                        ਵਾਪਸ -EBADMSG; /* truncated */

                                ਵਾਪਸ 0; /* No progress possible with the current input. */
                        }
                }
#ਜੇ_ਅੰਤ

        ਮੂਲ:
                assert_not_reached();
        }
}

ਪੂਰਨ compressor_new(Compressor **ret, Compression type) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        ਚੋਣ (type) {

#ਜੇ HAVE_XZ
        ਹਾਲਤ COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                ਜੇ (xzr != LZMA_OK)
                        ਵਾਪਸ -EIO;

                c->type = COMPRESSION_XZ;
                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
        ਹਾਲਤ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4_F_ਸੰਸਕਰਣ);
                ਜੇ (sym_LZ4F_isError(rc))
                        ਵਾਪਸ -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t ਸਿਰਲੇਖ_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_ਸਿਰਲੇਖ = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਸਿਰਲੇਖ_bound);
                ਜੇ (!c->lz4_ਸਿਰਲੇਖ)
                        ਵਾਪਸ -ENOMEM;

                c->lz4_ਸਿਰਲੇਖ_ਮਾਪ = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_ਸਿਰਲੇਖ, ਸਿਰਲੇਖ_bound, &lz4_preferences);
                ਜੇ (sym_LZ4F_isError(c->lz4_ਸਿਰਲੇਖ_ਮਾਪ))
                        ਵਾਪਸ -EINVAL;

                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
        ਹਾਲਤ COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                ਜੇ (!c->c_zstd)
                        ਵਾਪਸ -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_ਸੈੱਟ_Parameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                ਜੇ (sym_ZSTD_isError(z))
                        ਵਾਪਸ -EIO;

                z = sym_ZSTD_CCtx_ਸੈੱਟ_Parameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                ਜੇ (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
        ਹਾਲਤ COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_ਸੰਸਕਰਣ, (ਪੂਰਨ) ਆਕਾਰ(c->gzip));
                ਜੇ (r != Z_OK)
                        ਵਾਪਸ -EIO;

                c->type = COMPRESSION_GZIP;
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BZIP2
        ਹਾਲਤ COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                ਜੇ (r != BZ_OK)
                        ਵਾਪਸ -EIO;

                c->type = COMPRESSION_BZIP2;
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

        ਹਾਲਤ COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                ਰੋਕੋ;

        ਮੂਲ:
                ਵਾਪਸ -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        ਵਾਪਸ 0;
}

#ਜੇ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
ਸਥਿਰ ਪੂਰਨ enlarge_ਬਫਰ(ਖਾਲੀ **ਬਫਰ, size_t *ਬਫਰ_ਮਾਪ, size_t *ਬਫਰ_allocated, size_t need) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ_ਮਾਪ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ_allocated);

        need = MAX3(need, *ਬਫਰ_ਮਾਪ + 1, (size_t) COMPRESS_PIPE_ਬਫਰ_ਮਾਪ);
        ਜੇ (*ਬਫਰ_allocated >= need)
                ਵਾਪਸ 0;

        ਜੇ (!greedy_realloc(ਬਫਰ, need, 1))
                ਵਾਪਸ -ENOMEM;

        *ਬਫਰ_allocated = MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(*ਬਫਰ);
        ਵਾਪਸ 1;
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ compressor_ਸ਼ੁਰੂ(
                Compressor *c,
                ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ,
                size_t ਮਾਪ,
                ਖਾਲੀ **ਬਫਰ,
                size_t *ਬਫਰ_ਮਾਪ,
                size_t *ਬਫਰ_allocated) {

#ਜੇ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ਪੂਰਨ r;
#ਜੇ_ਅੰਤ

        ਪ੍ਰਤਿਜ੍ਞਾ(c);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ_ਮਾਪ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ_allocated);

        ਜੇ (!c->encoding)
                ਵਾਪਸ -EINVAL;

        ਜੇ (ਮਾਪ == 0)
                ਵਾਪਸ 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕੜੇ);

        *ਬਫਰ_ਮਾਪ = 0;

        ਚੋਣ (c->type) {

#ਜੇ HAVE_XZ
        ਹਾਲਤ COMPRESSION_XZ:

                c->xz.ਅਗਲਾ_ਵਿੱਚ = ਅੰਕੜੇ;
                c->xz.avail_ਵਿੱਚ = ਮਾਪ;

                ਜਦੋਂ (c->xz.avail_ਵਿੱਚ > 0) {
                        lzma_ret lzr;

                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, /* need= */ 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        c->xz.ਅਗਲਾ_ਬਾਹਰਲਾ = (uint8_t*) *ਬਫਰ + *ਬਫਰ_ਮਾਪ;
                        c->xz.avail_ਬਾਹਰਲਾ = *ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ;

                        lzr = sym_lzma_ਕੋਡ(&c->xz, LZMA_ਲੜੀਵਾਰ);
                        ਜੇ (lzr != LZMA_OK)
                                ਵਾਪਸ -EIO;

                        *ਬਫਰ_ਮਾਪ += (*ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ) - c->xz.avail_ਬਾਹਰਲਾ;
                }

                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
        ਹਾਲਤ COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                ਜੇ (c->lz4_ਸਿਰਲੇਖ_ਮਾਪ > 0) {
                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, c->lz4_ਸਿਰਲੇਖ_ਮਾਪ);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ਸਮ੍ਰਿਤੀ_ਨਕਲ(*ਬਫਰ, c->lz4_ਸਿਰਲੇਖ, c->lz4_ਸਿਰਲੇਖ_ਮਾਪ);
                        *ਬਫਰ_ਮਾਪ = c->lz4_ਸਿਰਲੇਖ_ਮਾਪ;
                        c->lz4_ਸਿਰਲੇਖ = mfree(c->lz4_ਸਿਰਲੇਖ);
                        c->lz4_ਸਿਰਲੇਖ_ਮਾਪ = 0;
                }

                size_t bound = sym_LZ4F_compressBound(ਮਾਪ, &lz4_preferences);
                r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, *ਬਫਰ_ਮਾਪ + bound);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *ਬਫਰ + *ਬਫਰ_ਮਾਪ,
                                                   *ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ,
                                                   ਅੰਕੜੇ, ਮਾਪ, ਨਿਰਾ);
                ਜੇ (sym_LZ4F_isError(n))
                        ਵਾਪਸ -EIO;

                *ਬਫਰ_ਮਾਪ += n;
                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
        ਹਾਲਤ COMPRESSION_ZSTD: {
                ZSTD_ਵਿੱਚ_ਬਫਰ ਨਿਵੇਸ਼ = {
                        .src = ਅੰਕੜੇ,
                        .ਮਾਪ = ਮਾਪ,
                };

                ਜਦੋਂ (ਨਿਵੇਸ਼.pos < ਨਿਵੇਸ਼.ਮਾਪ) {
                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, /* need= */ 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ZSTD_ਬਾਹਰਲਾ_ਬਫਰ output = {
                                .dst = ((uint8_t *) *ਬਫਰ + *ਬਫਰ_ਮਾਪ),
                                .ਮਾਪ = *ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ਨਿਵੇਸ਼, ZSTD_e_continue);
                        ਜੇ (sym_ZSTD_isError(res))
                                ਵਾਪਸ -EIO;

                        *ਬਫਰ_ਮਾਪ += output.pos;
                }

                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
        ਹਾਲਤ COMPRESSION_GZIP:
                ਜੇ (ਮਾਪ > UINT_MAX)
                        ਵਾਪਸ -EFBIG;

                c->gzip.ਅਗਲਾ_ਵਿੱਚ = (ਖਾਲੀ*) ਅੰਕੜੇ;
                c->gzip.avail_ਵਿੱਚ = ਮਾਪ;

                ਜਦੋਂ (c->gzip.avail_ਵਿੱਚ > 0) {
                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, /* need= */ 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        size_t avail = MIN(*ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ, (size_t) UINT_MAX);
                        c->gzip.ਅਗਲਾ_ਬਾਹਰਲਾ = (uint8_t*) *ਬਫਰ + *ਬਫਰ_ਮਾਪ;
                        c->gzip.avail_ਬਾਹਰਲਾ = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        ਜੇ (r != Z_OK)
                                ਵਾਪਸ -EIO;

                        *ਬਫਰ_ਮਾਪ += avail - c->gzip.avail_ਬਾਹਰਲਾ;
                }

                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BZIP2
        ਹਾਲਤ COMPRESSION_BZIP2:
                ਜੇ (ਮਾਪ > UINT_MAX)
                        ਵਾਪਸ -EFBIG;

                c->bzip2.ਅਗਲਾ_ਵਿੱਚ = (ਖਾਲੀ*) ਅੰਕੜੇ;
                c->bzip2.avail_ਵਿੱਚ = ਮਾਪ;

                ਜਦੋਂ (c->bzip2.avail_ਵਿੱਚ > 0) {
                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, /* need= */ 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        size_t avail = MIN(*ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ, (size_t) UINT_MAX);
                        c->bzip2.ਅਗਲਾ_ਬਾਹਰਲਾ = (ਖਾਲੀ*) ((uint8_t*) *ਬਫਰ + *ਬਫਰ_ਮਾਪ);
                        c->bzip2.avail_ਬਾਹਰਲਾ = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_ਲੜੀਵਾਰ);
                        ਜੇ (r != BZ_ਲੜੀਵਾਰ_OK)
                                ਵਾਪਸ -EIO;

                        *ਬਫਰ_ਮਾਪ += avail - c->bzip2.avail_ਬਾਹਰਲਾ;
                }

                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

        ਹਾਲਤ COMPRESSION_NONE:

                ਜੇ (*ਬਫਰ_allocated < ਮਾਪ) {
                        ਖਾਲੀ *p;

                        p = realloc(*ਬਫਰ, ਮਾਪ);
                        ਜੇ (!p)
                                ਵਾਪਸ -ENOMEM;

                        *ਬਫਰ = p;
                        *ਬਫਰ_allocated = ਮਾਪ;
                }

                ਸਮ੍ਰਿਤੀ_ਨਕਲ(*ਬਫਰ, ਅੰਕੜੇ, ਮਾਪ);
                *ਬਫਰ_ਮਾਪ = ਮਾਪ;
                ਰੋਕੋ;

        ਮੂਲ:
                ਵਾਪਸ -EOPNOTSUPP;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ compressor_ਅੰਤ(Compressor *c, ਖਾਲੀ **ਬਫਰ, size_t *ਬਫਰ_ਮਾਪ, size_t *ਬਫਰ_allocated) {
#ਜੇ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ਪੂਰਨ r;
#ਜੇ_ਅੰਤ

        ਪ੍ਰਤਿਜ੍ਞਾ(c);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ_ਮਾਪ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਫਰ_allocated);

        ਜੇ (!c->encoding)
                ਵਾਪਸ -EINVAL;

        *ਬਫਰ_ਮਾਪ = 0;

        ਚੋਣ (c->type) {

#ਜੇ HAVE_XZ
        ਹਾਲਤ COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_ਵਿੱਚ = 0;

                ਕਰੋ {
                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, /* need= */ 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        c->xz.ਅਗਲਾ_ਬਾਹਰਲਾ = (uint8_t*) *ਬਫਰ + *ਬਫਰ_ਮਾਪ;
                        c->xz.avail_ਬਾਹਰਲਾ = *ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ;

                        lzr = sym_lzma_ਕੋਡ(&c->xz, LZMA_ਅੰਤ);
                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(lzr, LZMA_OK, LZMA_ਧਾਰਾ_ਸਿਰਾ))
                                ਵਾਪਸ -EIO;

                        *ਬਫਰ_ਮਾਪ += (*ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ) - c->xz.avail_ਬਾਹਰਲਾ;
                } ਜਦੋਂ (lzr != LZMA_ਧਾਰਾ_ਸਿਰਾ);

                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_LZ4
        ਹਾਲਤ COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, bound);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                size_t n = sym_LZ4_F_compress_ਸਿਰਾ(c->c_lz4, *ਬਫਰ, *ਬਫਰ_allocated, ਨਿਰਾ);
                ਜੇ (sym_LZ4F_isError(n))
                        ਵਾਪਸ -EIO;

                *ਬਫਰ_ਮਾਪ = n;
                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZSTD
        ਹਾਲਤ COMPRESSION_ZSTD: {
                ZSTD_ਵਿੱਚ_ਬਫਰ ਨਿਵੇਸ਼ = {};
                size_t res;

                ਕਰੋ {
                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, /* need= */ 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ZSTD_ਬਾਹਰਲਾ_ਬਫਰ output = {
                                .dst = ((uint8_t *) *ਬਫਰ + *ਬਫਰ_ਮਾਪ),
                                .ਮਾਪ = *ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ਨਿਵੇਸ਼, ZSTD_e_ਸਿਰਾ);
                        ਜੇ (sym_ZSTD_isError(res))
                                ਵਾਪਸ -EIO;

                        *ਬਫਰ_ਮਾਪ += output.pos;
                } ਜਦੋਂ (res != 0);

                ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_ZLIB
        ਹਾਲਤ COMPRESSION_GZIP:
                c->gzip.avail_ਵਿੱਚ = 0;

                ਕਰੋ {
                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, /* need= */ 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        size_t avail = MIN(*ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ, (size_t) UINT_MAX);
                        c->gzip.ਅਗਲਾ_ਬਾਹਰਲਾ = (uint8_t*) *ਬਫਰ + *ਬਫਰ_ਮਾਪ;
                        c->gzip.avail_ਬਾਹਰਲਾ = avail;

                        r = sym_deflate(&c->gzip, Z_ਅੰਤ);
                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, Z_OK, Z_ਧਾਰਾ_ਸਿਰਾ))
                                ਵਾਪਸ -EIO;

                        *ਬਫਰ_ਮਾਪ += avail - c->gzip.avail_ਬਾਹਰਲਾ;
                } ਜਦੋਂ (r != Z_ਧਾਰਾ_ਸਿਰਾ);

                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_BZIP2
        ਹਾਲਤ COMPRESSION_BZIP2:
                c->bzip2.avail_ਵਿੱਚ = 0;

                ਕਰੋ {
                        r = enlarge_ਬਫਰ(ਬਫਰ, ਬਫਰ_ਮਾਪ, ਬਫਰ_allocated, /* need= */ 0);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        size_t avail = MIN(*ਬਫਰ_allocated - *ਬਫਰ_ਮਾਪ, (size_t) UINT_MAX);
                        c->bzip2.ਅਗਲਾ_ਬਾਹਰਲਾ = (ਖਾਲੀ*) ((uint8_t*) *ਬਫਰ + *ਬਫਰ_ਮਾਪ);
                        c->bzip2.avail_ਬਾਹਰਲਾ = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_ਅੰਤ);
                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, BZ_ਅੰਤ_OK, BZ_ਧਾਰਾ_ਸਿਰਾ))
                                ਵਾਪਸ -EIO;

                        *ਬਫਰ_ਮਾਪ += avail - c->bzip2.avail_ਬਾਹਰਲਾ;
                } ਜਦੋਂ (r != BZ_ਧਾਰਾ_ਸਿਰਾ);

                ਰੋਕੋ;
#ਜੇ_ਅੰਤ

        ਹਾਲਤ COMPRESSION_NONE:
                ਰੋਕੋ;

        ਮੂਲ:
                ਵਾਪਸ -EOPNOTSUPP;
        }

        ਵਾਪਸ 0;
}
