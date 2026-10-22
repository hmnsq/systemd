/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಆದರೆ HAVE_XZ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <lzma.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <lz4.h>
#ಅನ್ತರ್ಭಾವ <lz4frame.h>
#ಅನ್ತರ್ಭಾವ <lz4hc.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <zstd.h>
#ಅನ್ತರ್ಭಾವ <zstd_errors.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <zlib.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BZIP2
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <bzlib.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "bitfield.h"
#ಅನ್ತರ್ಭಾವ "compress.h"
#ಅನ್ತರ್ಭಾವ "dlfcn-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"

#ಆದರೆ HAVE_XZ
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_auto_decoder) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_ಸಂಕೇತ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_easy_encoder) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_ಕೊನೆ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_ಹರಿವು_ಬಫರ್_encode) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_ಹರಿವು_decoder) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(lzma_lzma_preset) = ಖಾಲಿ;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ lzma_ಕೊನೆ_wrapper(lzma_ಹರಿವು *ls) {
        sym_lzma_ಕೊನೆ(ls);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_compressBegin) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_compressBound) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_F_compress_ಕೊನೆ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_compressUpdate) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_decompress) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4F_isError) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_compress_HC) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_compress_default) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_decompress_ಸುರಕ್ಷಿತ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_decompress_ಸುರಕ್ಷಿತ_partial) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(LZ4_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ) = ಖಾಲಿ;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ LZ4F_preferences_t lz4_preferences = {
        .ಚೌಕಟ್ಟು_Info.ಖಂಡ_ಗಾತ್ರ_ID = 5,
};
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_CCtx_ಗಣ_Parameter) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_compress) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_compressStream2) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_createCCtx) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_createDCtx) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_CStream_ಒಳ_ಗಾತ್ರ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_CStream_ಹೊರಹೋಗು_ಗಾತ್ರ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_decompress_ಹರಿವು) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_DStream_ಒಳ_ಗಾತ್ರ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_DStream_ಹೊರಹೋಗು_ಗಾತ್ರ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_freeCCtx) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_freeDCtx) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_get_Error_ಸಂಕೇತ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_getErrorName) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_get_ಚೌಕಟ್ಟು_Content_ಗಾತ್ರ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(ZSTD_isError) = ಖಾಲಿ;

DEFINE_TRIVIAL_CLEANUP_FUNC_ತುಂಬಿದ_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, ಖಾಲಿ);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ zstd_ret_ಗಮ್ಯ_errno(size_t ret) {
        ಆಯ್ಕೆ (sym_ZSTD_get_Error_ಸಂಕೇತ(ret)) {
        ಸ್ಥಿತಿ ZSTD_error_dst_ಗಾತ್ರ_too_Small:
                ಹಿಂತಿರುಗಿ -ENOBUFS;
        ಸ್ಥಿತಿ ZSTD_error_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ:
                ಹಿಂತಿರುಗಿ -ENOMEM;
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EBADMSG;
        }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
ಸ್ಥಿರ DLSYM_PROTOTYPE(deflateInit2_) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(deflate) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(deflate_ಕೊನೆ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(inflateInit2_) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(inflate) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(inflate_ಕೊನೆ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(inflateReset) = ಖಾಲಿ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ deflate_ಕೊನೆ_wrapper(z_ಹರಿವು *s) {
        sym_deflate_ಕೊನೆ(s);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ inflate_ಕೊನೆ_wrapper(z_ಹರಿವು *s) {
        sym_inflate_ಕೊನೆ(s);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BZIP2
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzCompressInit) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzCompress) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bz_Compress_ಕೊನೆ) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bzDecompress) = ಖಾಲಿ;
ಸ್ಥಿರ DLSYM_PROTOTYPE(BZ2_bz_Decompress_ಕೊನೆ) = ಖಾಲಿ;

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ BZ2_bz_Compress_ಕೊನೆ_wrapper(bz_ಹರಿವು *s) {
        sym_BZ2_bz_Compress_ಕೊನೆ(s);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ BZ2_bz_Decompress_ಕೊನೆ_wrapper(bz_ಹರಿವು *s) {
        sym_BZ2_bz_Decompress_ಕೊನೆ(s);
}
#ಆದರೆ_ಅಂತ್ಯ

/* Opaque Compressor/Decompressor struct definition */
ರಚನೆ Compressor {
        Compression type;
        bool encoding;
        bool ಪೂರ್ಣಗೊಂಡ;
        ಒಕ್ಕೂಟ {
#ಆದರೆ HAVE_XZ
                lzma_ಹರಿವು xz;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ HAVE_LZ4
                ರಚನೆ {
                        LZ4F_compressionContext_t c_lz4;
                        ಶೂನ್ಯ *lz4_ಶೀರ್ಷಿಕೆ;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
                };
                LZ4F_decompressionContext_t d_lz4;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ HAVE_ZLIB
                z_ಹರಿವು gzip;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ HAVE_BZIP2
                bz_ಹರಿವು bzip2;
#ಆದರೆ_ಅಂತ್ಯ
        };
};

#ವ್ಯಾಖ್ಯಾನಿಸು ALIGN_8(l) ALIGN_ಗಮ್ಯ(l, ಗಾತ್ರ(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#ವ್ಯಾಖ್ಯಾನಿಸು ZLIB_WBITS_GZIP (15 + 16)

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ compression_ಕೋಷ್ಟಕ[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ compression_uppercase_ಕೋಷ್ಟಕ[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ compression_extension_ಕೋಷ್ಟಕ[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "",
        [COMPRESSION_XZ]    = ".xz",
        [COMPRESSION_LZ4]   = ".lz4",
        [COMPRESSION_ZSTD]  = ".zst",
        [COMPRESSION_GZIP]  = ".gz",
        [COMPRESSION_BZIP2] = ".bz2",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(compression, Compression);
DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(compression_uppercase, Compression);
DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(compression_extension, Compression);

Compression compression_from_string_harder(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        Compression c;

        ಪ್ರತಿಜ್ಞಾ(s);

        c = compression_from_string(s);
        ಆದರೆ (c >= 0)
                ಹಿಂತಿರುಗಿ c;

        ಹಿಂತಿರುಗಿ compression_uppercase_from_string(s);
}

Compression compression_from_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename) {
        Compression c;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        ಪ್ರತಿಜ್ಞಾ(filename);

        e = strrchr(filename, '.');
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        ಆದರೆ (c < 0)
                ಹಿಂತಿರುಗಿ COMPRESSION_NONE;

        ಹಿಂತಿರುಗಿ c;
}

bool compression_supported_journal(Compression c) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        ಪ್ರತಿಜ್ಞಾ(c >= 0);
        ಪ್ರತಿಜ್ಞಾ(c < _COMPRESSION_MAX);

        ಹಿಂತಿರುಗಿ ಬಿಟ್_ಗಣ(supported, c);
}

bool compression_supported(Compression c) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        ಪ್ರತಿಜ್ಞಾ(c >= 0);
        ಪ್ರತಿಜ್ಞಾ(c < _COMPRESSION_MAX);

        ಹಿಂತಿರುಗಿ ಬಿಟ್_ಗಣ(supported, c);
}

Compression compression_detect_from_magic(ಸ್ಥಿರಾಂಕ uint8_t ದತ್ತಾಂಶ[ಸ್ಥಿರ COMPRESSION_MAGIC_ಬೈಟುಗಳು_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        ಆದರೆ (memcmp(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                ಹಿಂತಿರುಗಿ COMPRESSION_GZIP;
        ಆದರೆ (memcmp(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                ಹಿಂತಿರುಗಿ COMPRESSION_XZ;
        ಆದರೆ (memcmp(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                ಹಿಂತಿರುಗಿ COMPRESSION_ZSTD;
        ಆದರೆ (memcmp(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                ಹಿಂತಿರುಗಿ COMPRESSION_LZ4;
        ಆದರೆ (memcmp(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                ಹಿಂತಿರುಗಿ COMPRESSION_BZIP2;

        ಹಿಂತಿರುಗಿ _COMPRESSION_INVALID;
}

ಪೂರ್ಣಾಂಕ dlopen_xz(ಪೂರ್ಣಾಂಕ log_level) {
#ಆದರೆ HAVE_XZ
        ಸ್ಥಿರ ಶೂನ್ಯ *lzma_dl = ಖಾಲಿ;

        LIBLZMA_NOTE(suggested);

        ಹಿಂತಿರುಗಿ dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_ಸಂಕೇತ),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_ಕೊನೆ),
                        DLSYM_ARG(lzma_ಹರಿವು_ಬಫರ್_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_ಹರಿವು_decoder));
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ dlopen_lz4(ಪೂರ್ಣಾಂಕ log_level) {
#ಆದರೆ HAVE_LZ4
        ಸ್ಥಿರ ಶೂನ್ಯ *lz4_dl = ಖಾಲಿ;

        LIBLZ4_NOTE(suggested);

        ಹಿಂತಿರುಗಿ dlopen_many_sym_or_warn(
                        &lz4_dl,
                        "liblz4.so.1", log_level,
                        DLSYM_ARG(LZ4F_compressBegin),
                        DLSYM_ARG(LZ4F_compressBound),
                        DLSYM_ARG(LZ4_F_compress_ಕೊನೆ),
                        DLSYM_ARG(LZ4F_compressUpdate),
                        DLSYM_ARG(LZ4F_createCompressionContext),
                        DLSYM_ARG(LZ4F_createDecompressionContext),
                        DLSYM_ARG(LZ4F_decompress),
                        DLSYM_ARG(LZ4F_freeCompressionContext),
                        DLSYM_ARG(LZ4F_freeDecompressionContext),
                        DLSYM_ARG(LZ4F_isError),
                        DLSYM_ARG(LZ4_compress_default),
                        DLSYM_ARG(LZ4_compress_HC),
                        DLSYM_ARG(LZ4_decompress_ಸುರಕ್ಷಿತ),
                        DLSYM_ARG(LZ4_decompress_ಸುರಕ್ಷಿತ_partial),
                        DLSYM_ARG(LZ4_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ));
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ dlopen_zstd(ಪೂರ್ಣಾಂಕ log_level) {
#ಆದರೆ HAVE_ZSTD
        ಸ್ಥಿರ ಶೂನ್ಯ *zstd_dl = ಖಾಲಿ;

        LIBZSTD_NOTE(suggested);

        ಹಿಂತಿರುಗಿ dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_get_Error_ಸಂಕೇತ),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_ಚೌಕಟ್ಟು_Content_ಗಾತ್ರ),
                        DLSYM_ARG(ZSTD_decompress_ಹರಿವು),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_ಹೊರಹೋಗು_ಗಾತ್ರ),
                        DLSYM_ARG(ZSTD_CStream_ಒಳ_ಗಾತ್ರ),
                        DLSYM_ARG(ZSTD_CStream_ಹೊರಹೋಗು_ಗಾತ್ರ),
                        DLSYM_ARG(ZSTD_CCtx_ಗಣ_Parameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_ಒಳ_ಗಾತ್ರ),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ dlopen_zlib(ಪೂರ್ಣಾಂಕ log_level) {
#ಆದರೆ HAVE_ZLIB
        ಸ್ಥಿರ ಶೂನ್ಯ *zlib_dl = ಖಾಲಿ;

        LIBZ_NOTE(suggested);

        ಹಿಂತಿರುಗಿ dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflate_ಕೊನೆ),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflate_ಕೊನೆ),
                        DLSYM_ARG(inflateReset));
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ dlopen_bzip2(ಪೂರ್ಣಾಂಕ log_level) {
#ಆದರೆ HAVE_BZIP2
        ಸ್ಥಿರ ಶೂನ್ಯ *bzip2_dl = ಖಾಲಿ;

        LIBBZ2_NOTE(suggested);

        ಹಿಂತಿರುಗಿ dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bz_Compress_ಕೊನೆ),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bz_Decompress_ಕೊನೆ));
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ dlopen_compress_journal(Compression c, ಪೂರ್ಣಾಂಕ log_level) {
        ಆಯ್ಕೆ (c) {
        ಸ್ಥಿತಿ COMPRESSION_NONE:
                ಹಿಂತಿರುಗಿ 0;
        ಸ್ಥಿತಿ COMPRESSION_XZ:
                ಹಿಂತಿರುಗಿ dlopen_xz(log_level);
        ಸ್ಥಿತಿ COMPRESSION_LZ4:
                ಹಿಂತಿರುಗಿ dlopen_lz4(log_level);
        ಸ್ಥಿತಿ COMPRESSION_ZSTD:
                ಹಿಂತಿರುಗಿ dlopen_zstd(log_level);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        }
}

ಪೂರ್ಣಾಂಕ dlopen_compress(Compression c, ಪೂರ್ಣಾಂಕ log_level) {
        ಆಯ್ಕೆ (c) {
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                ಹಿಂತಿರುಗಿ dlopen_zlib(log_level);
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                ಹಿಂತಿರುಗಿ dlopen_bzip2(log_level);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ dlopen_compress_journal(c, log_level);
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compress_blob_xz(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ *dst,
                size_t dst_alloc_ಗಾತ್ರ,
                size_t *dst_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, ಖಾಲಿ, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_ಬಹುಲಕ_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, ಖಾಲಿ }
        };
        lzma_ret ret;
        size_t ಹೊರಹೋಗು_pos = 0;
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_xz(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                ಆದರೆ (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ಆದರೆ (src_ಗಾತ್ರ < 80)
                ಹಿಂತಿರುಗಿ -ENOBUFS;

        ret = sym_lzma_ಹರಿವು_ಬಫರ್_encode(filters, LZMA_CHECK_NONE, /* allocator= */ ಖಾಲಿ,
                                            src, src_ಗಾತ್ರ, dst, &ಹೊರಹೋಗು_pos, dst_alloc_ಗಾತ್ರ);
        ಆದರೆ (ret != LZMA_OK)
                ಹಿಂತಿರುಗಿ -ENOBUFS;

        *dst_ಗಾತ್ರ = ಹೊರಹೋಗು_pos;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compress_blob_lz4(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ *dst,
                size_t dst_alloc_ಗಾತ್ರ,
                size_t *dst_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_LZ4
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_lz4(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ಆದರೆ (src_ಗಾತ್ರ < 9)
                ಹಿಂತಿರುಗಿ -ENOBUFS;

        ಆದರೆ (src_ಗಾತ್ರ > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -EFBIG;
        ಆದರೆ (dst_alloc_ಗಾತ್ರ > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                dst_alloc_ಗಾತ್ರ = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;

        ಆದರೆ (level <= 0)
                r = sym_LZ4_compress_default(src, (ಅಕ್ಷರ*)dst + 8, src_ಗಾತ್ರ, (ಪೂರ್ಣಾಂಕ) dst_alloc_ಗಾತ್ರ - 8);
        ಇಲ್ಲದಿದ್ದರೆ
                r = sym_LZ4_compress_HC(src, (ಅಕ್ಷರ*)dst + 8, src_ಗಾತ್ರ, (ಪೂರ್ಣಾಂಕ) dst_alloc_ಗಾತ್ರ - 8, level);
        ಆದರೆ (r <= 0)
                ಹಿಂತಿರುಗಿ -ENOBUFS;

        unaligned_write_le64(dst, src_ಗಾತ್ರ);
        *dst_ಗಾತ್ರ = r + 8;

        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compress_blob_zstd(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ *dst,
                size_t dst_alloc_ಗಾತ್ರ,
                size_t *dst_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_ZSTD
        size_t k;
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_zstd(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        k = sym_ZSTD_compress(dst, dst_alloc_ಗಾತ್ರ, src, src_ಗಾತ್ರ, level < 0 ? 0 : level);
        ಆದರೆ (sym_ZSTD_isError(k))
                ಹಿಂತಿರುಗಿ zstd_ret_ಗಮ್ಯ_errno(k);

        *dst_ಗಾತ್ರ = k;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compress_blob_gzip(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                       ಶೂನ್ಯ *dst, size_t dst_alloc_ಗಾತ್ರ, size_t *dst_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_ZLIB
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_zlib(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ > UINT_MAX)
                ಹಿಂತಿರುಗಿ -EFBIG;
        ಆದರೆ (dst_alloc_ಗಾತ್ರ > UINT_MAX)
                dst_alloc_ಗಾತ್ರ = UINT_MAX;

        _cleanup_(deflate_ಕೊನೆ_wrapper) z_ಹರಿವು s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_ಆವೃತ್ತಿ, (ಪೂರ್ಣಾಂಕ) ಗಾತ್ರ(s));
        ಆದರೆ (r != Z_OK)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        s.ಮುಂದಿನ_ಒಳ = (ಶೂನ್ಯ*) src;
        s.avail_ಒಳ = src_ಗಾತ್ರ;
        s.ಮುಂದಿನ_ಹೊರಹೋಗು = dst;
        s.avail_ಹೊರಹೋಗು = dst_alloc_ಗಾತ್ರ;

        r = sym_deflate(&s, Z_ಮುಕ್ತಾಯ);
        ಆದರೆ (r != Z_ಹರಿವು_ಕೊನೆ)
                ಹಿಂತಿರುಗಿ -ENOBUFS;

        *dst_ಗಾತ್ರ = dst_alloc_ಗಾತ್ರ - s.avail_ಹೊರಹೋಗು;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compress_blob_bzip2(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ *dst, size_t dst_alloc_ಗಾತ್ರ, size_t *dst_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ level) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_alloc_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_BZIP2
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ > UINT_MAX)
                ಹಿಂತಿರುಗಿ -EFBIG;
        ಆದರೆ (dst_alloc_ಗಾತ್ರ > UINT_MAX)
                dst_alloc_ಗಾತ್ರ = UINT_MAX;

        _cleanup_(BZ2_bz_Compress_ಕೊನೆ_wrapper) bz_ಹರಿವು s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        ಆದರೆ (r != BZ_OK)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        s.ಮುಂದಿನ_ಒಳ = (ಅಕ್ಷರ*) src;
        s.avail_ಒಳ = src_ಗಾತ್ರ;
        s.ಮುಂದಿನ_ಹೊರಹೋಗು = (ಅಕ್ಷರ*) dst;
        s.avail_ಹೊರಹೋಗು = dst_alloc_ಗಾತ್ರ;

        r = sym_BZ2_bzCompress(&s, BZ_ಮುಕ್ತಾಯ);

        ಆದರೆ (r != BZ_ಹರಿವು_ಕೊನೆ)
                ಹಿಂತಿರುಗಿ -ENOBUFS;

        *dst_ಗಾತ್ರ = dst_alloc_ಗಾತ್ರ - s.avail_ಹೊರಹೋಗು;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ compress_blob_journal(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ *dst, size_t dst_alloc_ಗಾತ್ರ, size_t *dst_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ level) {

        ಆಯ್ಕೆ (compression) {
        ಸ್ಥಿತಿ COMPRESSION_XZ:
                ಹಿಂತಿರುಗಿ compress_blob_xz(src, src_ಗಾತ್ರ, dst, dst_alloc_ಗಾತ್ರ, dst_ಗಾತ್ರ, level);
        ಸ್ಥಿತಿ COMPRESSION_LZ4:
                ಹಿಂತಿರುಗಿ compress_blob_lz4(src, src_ಗಾತ್ರ, dst, dst_alloc_ಗಾತ್ರ, dst_ಗಾತ್ರ, level);
        ಸ್ಥಿತಿ COMPRESSION_ZSTD:
                ಹಿಂತಿರುಗಿ compress_blob_zstd(src, src_ಗಾತ್ರ, dst, dst_alloc_ಗಾತ್ರ, dst_ಗಾತ್ರ, level);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        }
}

ಪೂರ್ಣಾಂಕ compress_blob(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ *dst, size_t dst_alloc_ಗಾತ್ರ, size_t *dst_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ level) {

        ಆಯ್ಕೆ (compression) {
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                ಹಿಂತಿರುಗಿ compress_blob_gzip(src, src_ಗಾತ್ರ, dst, dst_alloc_ಗಾತ್ರ, dst_ಗಾತ್ರ, level);
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                ಹಿಂತಿರುಗಿ compress_blob_bzip2(src, src_ಗಾತ್ರ, dst, dst_alloc_ಗಾತ್ರ, dst_ಗಾತ್ರ, level);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ compress_blob_journal(compression, src, src_ಗಾತ್ರ, dst, dst_alloc_ಗಾತ್ರ, dst_ಗಾತ್ರ, level);
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_blob_xz(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **dst,
                size_t *dst_ಗಾತ್ರ,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_XZ
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_xz(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_(lzma_ಕೊನೆ_wrapper) lzma_ಹರಿವು s = LZMA_ಹರಿವು_INIT;
        lzma_ret ret = sym_lzma_ಹರಿವು_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ಆದರೆ (ret != LZMA_OK)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        size_t space = MIN(src_ಗಾತ್ರ * 2, dst_max ?: SIZE_MAX);
        ಆದರೆ (!greedy_realloc(dst, space, 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        s.ಮುಂದಿನ_ಒಳ = src;
        s.avail_ಒಳ = src_ಗಾತ್ರ;

        s.ಮುಂದಿನ_ಹೊರಹೋಗು = *dst;
        s.avail_ಹೊರಹೋಗು = space;

        ಗಾಗಿ (;;) {
                size_t ಬಳಸಿದ;

                ret = sym_lzma_ಸಂಕೇತ(&s, LZMA_ಮುಕ್ತಾಯ);
                ಆದರೆ (ret == LZMA_ಹರಿವು_ಕೊನೆ)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (ret != LZMA_OK)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (dst_max > 0 && (space - s.avail_ಹೊರಹೋಗು) >= dst_max)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (dst_max > 0 && space == dst_max)
                        ಹಿಂತಿರುಗಿ -ENOBUFS;

                ಬಳಸಿದ = space - s.avail_ಹೊರಹೋಗು;
                /* Silence static analyzers, space is bounded by allocation size */
                ಪ್ರತಿಜ್ಞಾ(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                ಆದರೆ (!greedy_realloc(dst, space, 1))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                s.avail_ಹೊರಹೋಗು = space - ಬಳಸಿದ;
                s.ಮುಂದಿನ_ಹೊರಹೋಗು = *(uint8_t**)dst + ಬಳಸಿದ;
        }

        *dst_ಗಾತ್ರ = space - s.avail_ಹೊರಹೋಗು;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_blob_lz4(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **dst,
                size_t *dst_ಗಾತ್ರ,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_LZ4
        ಅಕ್ಷರ* ಹೊರಹೋಗು;
        ಪೂರ್ಣಾಂಕ r, ಗಾತ್ರ_2; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ <= 8)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (src_ಗಾತ್ರ - 8 > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -EFBIG;

        ಗಾತ್ರ_2 = unaligned_read_le64(src);
        ಆದರೆ (ಗಾತ್ರ_2 < 0 || (ಚಿಹ್ನೆರಹಿತ) ಗಾತ್ರ_2 != unaligned_read_le64(src))
                ಹಿಂತಿರುಗಿ -EFBIG;
        ಆದರೆ (dst_max > 0 && (size_t) ಗಾತ್ರ_2 > dst_max)
                ಹಿಂತಿರುಗಿ -ENOBUFS;
        ಹೊರಹೋಗು = greedy_realloc(dst, ಗಾತ್ರ_2, 1);
        ಆದರೆ (!ಹೊರಹೋಗು)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        r = sym_LZ4_decompress_ಸುರಕ್ಷಿತ((ಅಕ್ಷರ*)src + 8, ಹೊರಹೋಗು, src_ಗಾತ್ರ - 8, ಗಾತ್ರ_2);
        ಆದರೆ (r < 0 || r != ಗಾತ್ರ_2)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        *dst_ಗಾತ್ರ = ಗಾತ್ರ_2;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

size_t zstd_dstream_ಹೊರಹೋಗು_ಗಾತ್ರ(ಶೂನ್ಯ) {
#ಆದರೆ HAVE_ZSTD
        ಆದರೆ (dlopen_zstd(LOG_DEBUG) < 0)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ sym_ZSTD_DStream_ಹೊರಹೋಗು_ಗಾತ್ರ();
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_blob_zstd(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **dst,
                size_t *dst_ಗಾತ್ರ,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_ZSTD
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_zstd(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ ಗಾತ್ರ_2 = sym_ZSTD_get_ಚೌಕಟ್ಟು_Content_ಗಾತ್ರ(src, src_ಗಾತ್ರ);
        ಆದರೆ (ಗಾತ್ರ_2 == ZSTD_CONTENTSIZE_ERROR)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        ಆದರೆ (ಗಾತ್ರ_2 != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_ಹೊರಹೋಗು_ಗಾತ್ರ(), (size_t) MIN(ಗಾತ್ರ_2, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ) cap));
        ಇಲ್ಲದಿದ್ದರೆ {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_ಗಾತ್ರ <= UINT64_MAX / 2 ? src_ಗಾತ್ರ * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_ಹೊರಹೋಗು_ಗಾತ್ರ(), (size_t) MIN(twice, (uint64_t) cap));
        }

        ಆದರೆ (!greedy_realloc(dst, space, 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;
        space = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ಆದರೆ (!dctx)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ZSTD_ಒಳ_ಬಫರ್ ಒಳಹರಿವು = {
                .src = src,
                .ಗಾತ್ರ_2 = src_ಗಾತ್ರ,
        };
        ZSTD_ಹೊರಹೋಗು_ಬಫರ್ output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .ಗಾತ್ರ_2 = MIN(space, cap),
        };

        ಗಾಗಿ (;;) {
                size_t k = sym_ZSTD_decompress_ಹರಿವು(dctx, &output, &ಒಳಹರಿವು);
                ಆದರೆ (sym_ZSTD_isError(k))
                        ಹಿಂತಿರುಗಿ log_debug_errno(zstd_ret_ಗಮ್ಯ_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                ಆದರೆ (output.pos >= cap)
                        ನಿಲ್ಲಿಸು;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                ಆದರೆ (k == 0 && ಒಳಹರಿವು.pos >= ಒಳಹರಿವು.ಗಾತ್ರ_2)
                        ನಿಲ್ಲಿಸು;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                ಆದರೆ (output.pos == output.ಗಾತ್ರ_2) {
                        ಆದರೆ (space > SIZE_MAX / 2)
                                ಹಿಂತಿರುಗಿ -E2BIG;
                        space = MIN(2 * space, cap);
                        ಆದರೆ (!greedy_realloc(dst, space, 1))
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                        space = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*dst);
                        output.dst = *dst;
                        output.ಗಾತ್ರ_2 = MIN(space, cap);
                        ಮುಂದುವರಿ;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                ಆದರೆ (ಒಳಹರಿವು.pos >= ಒಳಹರಿವು.ಗಾತ್ರ_2)
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_ಗಾತ್ರ = output.pos;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_blob_gzip(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **dst,
                size_t *dst_ಗಾತ್ರ,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_ZLIB
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_zlib(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ > UINT_MAX)
                ಹಿಂತಿರುಗಿ -EFBIG;

        _cleanup_(inflate_ಕೊನೆ_wrapper) z_ಹರಿವು s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_ಆವೃತ್ತಿ, (ಪೂರ್ಣಾಂಕ) ಗಾತ್ರ(s));
        ಆದರೆ (r != Z_OK)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        size_t space = MIN3(src_ಗಾತ್ರ * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ಆದರೆ (!greedy_realloc(dst, space, 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        s.ಮುಂದಿನ_ಒಳ = (ಶೂನ್ಯ*) src;
        s.avail_ಒಳ = src_ಗಾತ್ರ;
        s.ಮುಂದಿನ_ಹೊರಹೋಗು = *dst;
        s.avail_ಹೊರಹೋಗು = space;

        ಗಾಗಿ (;;) {
                size_t ಬಳಸಿದ;

                r = sym_inflate(&s, Z_NO_FLUSH);
                ಆದರೆ (r == Z_ಹರಿವು_ಕೊನೆ)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (!ಒಳ_ಗಣ(r, Z_OK, Z_BUF_ERROR))
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ಆದರೆ (dst_max > 0 && (space - s.avail_ಹೊರಹೋಗು) >= dst_max)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (dst_max > 0 && space == dst_max)
                        ಹಿಂತಿರುಗಿ -ENOBUFS;

                ಬಳಸಿದ = space - s.avail_ಹೊರಹೋಗು;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                ಆದರೆ (!greedy_realloc(dst, space, 1))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                s.avail_ಹೊರಹೋಗು = space - ಬಳಸಿದ;
                s.ಮುಂದಿನ_ಹೊರಹೋಗು = *(uint8_t**)dst + ಬಳಸಿದ;
        }

        *dst_ಗಾತ್ರ = space - s.avail_ಹೊರಹೋಗು;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_blob_bzip2(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **dst,
                size_t *dst_ಗಾತ್ರ,
                size_t dst_max) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(dst);
        ಪ್ರತಿಜ್ಞಾ(dst_ಗಾತ್ರ);

#ಆದರೆ HAVE_BZIP2
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ > UINT_MAX)
                ಹಿಂತಿರುಗಿ -EFBIG;

        _cleanup_(BZ2_bz_Decompress_ಕೊನೆ_wrapper) bz_ಹರಿವು s = {};

        ಆದರೆ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ಹಿಂತಿರುಗಿ -EIO;

        size_t space = MIN3(src_ಗಾತ್ರ * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ಆದರೆ (!greedy_realloc(dst, space, 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        s.ಮುಂದಿನ_ಒಳ = (ಅಕ್ಷರ*) src;
        s.avail_ಒಳ = src_ಗಾತ್ರ;
        s.ಮುಂದಿನ_ಹೊರಹೋಗು = (ಅಕ್ಷರ*) *dst;
        s.avail_ಹೊರಹೋಗು = space;

        ಗಾಗಿ (;;) {
                size_t ಬಳಸಿದ;

                r = sym_BZ2_bzDecompress(&s);
                ಆದರೆ (r == BZ_ಹರಿವು_ಕೊನೆ)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (r != BZ_OK)
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ಆದರೆ (dst_max > 0 && (space - s.avail_ಹೊರಹೋಗು) >= dst_max)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (dst_max > 0 && space == dst_max)
                        ಹಿಂತಿರುಗಿ -ENOBUFS;

                ಬಳಸಿದ = space - s.avail_ಹೊರಹೋಗು;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                ಆದರೆ (!greedy_realloc(dst, space, 1))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                s.avail_ಹೊರಹೋಗು = space - ಬಳಸಿದ;
                s.ಮುಂದಿನ_ಹೊರಹೋಗು = (ಅಕ್ಷರ*) *dst + ಬಳಸಿದ;
        }

        *dst_ಗಾತ್ರ = space - s.avail_ಹೊರಹೋಗು;
        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ decompress_blob_journal(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **dst,
                size_t *dst_ಗಾತ್ರ,
                size_t dst_max) {

        ಆಯ್ಕೆ (compression) {
        ಸ್ಥಿತಿ COMPRESSION_XZ:
                ಹಿಂತಿರುಗಿ decompress_blob_xz(
                                src, src_ಗಾತ್ರ,
                                dst, dst_ಗಾತ್ರ, dst_max);
        ಸ್ಥಿತಿ COMPRESSION_LZ4:
                ಹಿಂತಿರುಗಿ decompress_blob_lz4(
                                src, src_ಗಾತ್ರ,
                                dst, dst_ಗಾತ್ರ, dst_max);
        ಸ್ಥಿತಿ COMPRESSION_ZSTD:
                ಹಿಂತಿರುಗಿ decompress_blob_zstd(
                                src, src_ಗಾತ್ರ,
                                dst, dst_ಗಾತ್ರ, dst_max);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
        }
}

ಪೂರ್ಣಾಂಕ decompress_blob(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **dst,
                size_t *dst_ಗಾತ್ರ,
                size_t dst_max) {

        ಆಯ್ಕೆ (compression) {
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                ಹಿಂತಿರುಗಿ decompress_blob_gzip(
                                src, src_ಗಾತ್ರ,
                                dst, dst_ಗಾತ್ರ, dst_max);
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                ಹಿಂತಿರುಗಿ decompress_blob_bzip2(
                                src, src_ಗಾತ್ರ,
                                dst, dst_ಗಾತ್ರ, dst_max);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ decompress_blob_journal(
                                compression,
                                src, src_ಗಾತ್ರ,
                                dst, dst_ಗಾತ್ರ, dst_max);
        }
}

ಪೂರ್ಣಾಂಕ decompress_zlib_raw(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ *dst,
                size_t dst_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ wbits) {

#ಆದರೆ HAVE_ZLIB
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_zlib(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ > UINT_MAX)
                ಹಿಂತಿರುಗಿ -EFBIG;
        ಆದರೆ (dst_ಗಾತ್ರ > UINT_MAX)
                ಹಿಂತಿರುಗಿ -EFBIG;

        _cleanup_(inflate_ಕೊನೆ_wrapper) z_ಹರಿವು s = {
                .ಮುಂದಿನ_ಒಳ = (ಶೂನ್ಯ*) src,
                .avail_ಒಳ = src_ಗಾತ್ರ,
                .ಮುಂದಿನ_ಹೊರಹೋಗು = dst,
                .avail_ಹೊರಹೋಗು = dst_ಗಾತ್ರ,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_ಆವೃತ್ತಿ, (ಪೂರ್ಣಾಂಕ) ಗಾತ್ರ(s));
        ಆದರೆ (r != Z_OK)
                ಹಿಂತಿರುಗಿ -EIO;

        r = sym_inflate(&s, Z_ಮುಕ್ತಾಯ);
        size_t produced = (uint8_t*) s.ಮುಂದಿನ_ಹೊರಹೋಗು - (uint8_t*) dst;

        ಆದರೆ (r != Z_ಹರಿವು_ಕೊನೆ || produced != dst_ಗಾತ್ರ)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಹಿಂತಿರುಗಿ 0;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_startswith_xz(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **ಬಫರ್,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ,
                size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

#ಆದರೆ HAVE_XZ
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_xz(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_(lzma_ಕೊನೆ_wrapper) lzma_ಹರಿವು s = LZMA_ಹರಿವು_INIT;
        lzma_ret ret = sym_lzma_ಹರಿವು_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ಆದರೆ (ret != LZMA_OK)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (!(greedy_realloc(ಬಫರ್, ALIGN_8(ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1), 1)))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್);

        s.ಮುಂದಿನ_ಒಳ = src;
        s.avail_ಒಳ = src_ಗಾತ್ರ;

        s.ಮುಂದಿನ_ಹೊರಹೋಗು = *ಬಫರ್;
        s.avail_ಹೊರಹೋಗು = allocated;

        ಗಾಗಿ (;;) {
                ret = sym_lzma_ಸಂಕೇತ(&s, LZMA_ಮುಕ್ತಾಯ);

                ಆದರೆ (!ಒಳ_ಗಣ(ret, LZMA_OK, LZMA_ಹರಿವು_ಕೊನೆ))
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ಆದರೆ (allocated - s.avail_ಹೊರಹೋಗು >= ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1)
                        ಹಿಂತಿರುಗಿ memcmp(*ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len) == 0 &&
                                ((ಸ್ಥಿರಾಂಕ uint8_t*) *ಬಫರ್)[ಪೂರ್ವಪ್ರತ್ಯಯ_len] == extra;

                ಆದರೆ (ret == LZMA_ಹರಿವು_ಕೊನೆ)
                        ಹಿಂತಿರುಗಿ 0;

                s.avail_ಹೊರಹೋಗು += allocated;

                ಆದರೆ (!(greedy_realloc(ಬಫರ್, allocated * 2, 1)))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                allocated = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್);
                s.ಮುಂದಿನ_ಹೊರಹೋಗು = *(uint8_t**)ಬಫರ್ + allocated - s.avail_ಹೊರಹೋಗು;
        }

#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_startswith_lz4(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **ಬಫರ್,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ,
                size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

#ಆದರೆ HAVE_LZ4
        size_t allocated;
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_lz4(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ <= 8)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (src_ಗಾತ್ರ - 8 > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -EFBIG;

        ಆದರೆ (!(greedy_realloc(ಬಫರ್, ALIGN_8(ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1), 1)))
                ಹಿಂತಿರುಗಿ -ENOMEM;
        allocated = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್);

        r = sym_LZ4_decompress_ಸುರಕ್ಷಿತ_partial(
                        (ಅಕ್ಷರ*)src + 8,
                        *ಬಫರ್,
                        src_ಗಾತ್ರ - 8,
                        ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        ಆದರೆ (r < 0 || (size_t) r < ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1) {
                size_t ಗಾತ್ರ_2;

                ಆದರೆ (sym_LZ4_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        ಹಿಂತಿರುಗಿ 0;

                ಆದರೆ (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        ಆದರೆ (memcmp(*ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, r) != 0)
                                ಹಿಂತಿರುಗಿ 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_ಗಾತ್ರ, ಬಫರ್, &ಗಾತ್ರ_2, 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (ಗಾತ್ರ_2 < ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1)
                        ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ memcmp(*ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len) == 0 &&
                ((ಸ್ಥಿರಾಂಕ uint8_t*) *ಬಫರ್)[ಪೂರ್ವಪ್ರತ್ಯಯ_len] == extra;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_startswith_zstd(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **ಬಫರ್,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ,
                size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                uint8_t extra) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

#ಆದರೆ HAVE_ZSTD
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_zstd(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ ಗಾತ್ರ_2 = sym_ZSTD_get_ಚೌಕಟ್ಟು_Content_ಗಾತ್ರ(src, src_ಗಾತ್ರ);
        ಆದರೆ (ಗಾತ್ರ_2 == ZSTD_CONTENTSIZE_ERROR)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        ಆದರೆ (ಗಾತ್ರ_2 != ZSTD_CONTENTSIZE_UNKNOWN && ಗಾತ್ರ_2 < ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1)
                ಹಿಂತಿರುಗಿ 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ಆದರೆ (!dctx)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (!(greedy_realloc(ಬಫರ್, MAX(sym_ZSTD_DStream_ಹೊರಹೋಗು_ಗಾತ್ರ(), ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1), 1)))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ZSTD_ಒಳ_ಬಫರ್ ಒಳಹರಿವು = {
                .src = src,
                .ಗಾತ್ರ_2 = src_ಗಾತ್ರ,
        };
        ZSTD_ಹೊರಹೋಗು_ಬಫರ್ output = {
                .dst = *ಬಫರ್,
                .ಗಾತ್ರ_2 = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್),
        };
        size_t k;

        k = sym_ZSTD_decompress_ಹರಿವು(dctx, &output, &ಒಳಹರಿವು);
        ಆದರೆ (sym_ZSTD_isError(k))
                ಹಿಂತಿರುಗಿ log_debug_errno(zstd_ret_ಗಮ್ಯ_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        ಆದರೆ (output.pos < ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                ಆದರೆ (k != 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                ಹಿಂತಿರುಗಿ 0; /* Decompressed text too short to match the prefix and extra */
        }

        ಹಿಂತಿರುಗಿ memcmp(*ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len) == 0 &&
                ((ಸ್ಥಿರಾಂಕ uint8_t*) *ಬಫರ್)[ಪೂರ್ವಪ್ರತ್ಯಯ_len] == extra;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_startswith_gzip(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **ಬಫರ್,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ,
                size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                uint8_t extra) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

#ಆದರೆ HAVE_ZLIB
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_zlib(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ > UINT_MAX)
                ಹಿಂತಿರುಗಿ -EFBIG;

        _cleanup_(inflate_ಕೊನೆ_wrapper) z_ಹರಿವು s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_ಆವೃತ್ತಿ, (ಪೂರ್ಣಾಂಕ) ಗಾತ್ರ(s));
        ಆದರೆ (r != Z_OK)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (!(greedy_realloc(ಬಫರ್, ALIGN_8(ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1), 1)))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್);

        s.ಮುಂದಿನ_ಒಳ = (ಶೂನ್ಯ*) src;
        s.avail_ಒಳ = src_ಗಾತ್ರ;

        s.ಮುಂದಿನ_ಹೊರಹೋಗು = *ಬಫರ್;
        s.avail_ಹೊರಹೋಗು = MIN(allocated, (size_t) UINT_MAX);

        ಗಾಗಿ (;;) {
                r = sym_inflate(&s, Z_ಮುಕ್ತಾಯ);

                ಆದರೆ (!ಒಳ_ಗಣ(r, Z_OK, Z_ಹರಿವು_ಕೊನೆ, Z_BUF_ERROR))
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ಆದರೆ (allocated - s.avail_ಹೊರಹೋಗು >= ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1)
                        ಹಿಂತಿರುಗಿ memcmp(*ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len) == 0 &&
                                ((ಸ್ಥಿರಾಂಕ uint8_t*) *ಬಫರ್)[ಪೂರ್ವಪ್ರತ್ಯಯ_len] == extra;

                ಆದರೆ (r == Z_ಹರಿವು_ಕೊನೆ)
                        ಹಿಂತಿರುಗಿ 0;

                ಆದರೆ (s.avail_ಹೊರಹೋಗು > 0)
                        ಹಿಂತಿರುಗಿ -EBADMSG; /* truncated */

                size_t ಬಳಸಿದ = allocated - s.avail_ಹೊರಹೋಗು;

                ಆದರೆ (!(greedy_realloc(ಬಫರ್, allocated * 2, 1)))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                allocated = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್);
                s.avail_ಹೊರಹೋಗು = MIN(allocated - ಬಳಸಿದ, (size_t) UINT_MAX);
                s.ಮುಂದಿನ_ಹೊರಹೋಗು = *(uint8_t**)ಬಫರ್ + ಬಳಸಿದ;
        }
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_startswith_bzip2(
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **ಬಫರ್,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ,
                size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                uint8_t extra) {

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(src_ಗಾತ್ರ > 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(ಪೂರ್ವಪ್ರತ್ಯಯ);

#ಆದರೆ HAVE_BZIP2
        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_bzip2(LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (src_ಗಾತ್ರ > UINT_MAX)
                ಹಿಂತಿರುಗಿ -EFBIG;

        _cleanup_(BZ2_bz_Decompress_ಕೊನೆ_wrapper) bz_ಹರಿವು s = {};

        ಆದರೆ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ಹಿಂತಿರುಗಿ -EIO;

        ಆದರೆ (!(greedy_realloc(ಬಫರ್, ALIGN_8(ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1), 1)))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್);

        s.ಮುಂದಿನ_ಒಳ = (ಅಕ್ಷರ*) src;
        s.avail_ಒಳ = src_ಗಾತ್ರ;

        s.ಮುಂದಿನ_ಹೊರಹೋಗು = *ಬಫರ್;
        s.avail_ಹೊರಹೋಗು = MIN(allocated, (size_t) UINT_MAX);

        ಗಾಗಿ (;;) {
                r = sym_BZ2_bzDecompress(&s);

                ಆದರೆ (!ಒಳ_ಗಣ(r, BZ_OK, BZ_ಹರಿವು_ಕೊನೆ))
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ಆದರೆ (allocated - s.avail_ಹೊರಹೋಗು >= ಪೂರ್ವಪ್ರತ್ಯಯ_len + 1)
                        ಹಿಂತಿರುಗಿ memcmp(*ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len) == 0 &&
                                ((ಸ್ಥಿರಾಂಕ uint8_t*) *ಬಫರ್)[ಪೂರ್ವಪ್ರತ್ಯಯ_len] == extra;

                ಆದರೆ (r == BZ_ಹರಿವು_ಕೊನೆ)
                        ಹಿಂತಿರುಗಿ 0;

                ಆದರೆ (s.avail_ಹೊರಹೋಗು > 0)
                        ಹಿಂತಿರುಗಿ -EBADMSG; /* truncated */

                size_t ಬಳಸಿದ = allocated - s.avail_ಹೊರಹೋಗು;

                ಆದರೆ (!(greedy_realloc(ಬಫರ್, allocated * 2, 1)))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                allocated = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್);
                s.avail_ಹೊರಹೋಗು = MIN(allocated - ಬಳಸಿದ, (size_t) UINT_MAX);
                s.ಮುಂದಿನ_ಹೊರಹೋಗು = (ಅಕ್ಷರ*) *ಬಫರ್ + ಬಳಸಿದ;
        }
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ decompress_startswith_journal(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **ಬಫರ್,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ, size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                uint8_t extra) {

        ಆಯ್ಕೆ (compression) {
        ಸ್ಥಿತಿ COMPRESSION_XZ:
                ಹಿಂತಿರುಗಿ decompress_startswith_xz(src, src_ಗಾತ್ರ, ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len, extra);
        ಸ್ಥಿತಿ COMPRESSION_LZ4:
                ಹಿಂತಿರುಗಿ decompress_startswith_lz4(src, src_ಗಾತ್ರ, ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len, extra);
        ಸ್ಥಿತಿ COMPRESSION_ZSTD:
                ಹಿಂತಿರುಗಿ decompress_startswith_zstd(src, src_ಗಾತ್ರ, ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len, extra);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        }
}

ಪೂರ್ಣಾಂಕ decompress_startswith(
                Compression compression,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, uint64_t src_ಗಾತ್ರ,
                ಶೂನ್ಯ **ಬಫರ್,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಪೂರ್ವಪ್ರತ್ಯಯ, size_t ಪೂರ್ವಪ್ರತ್ಯಯ_len,
                uint8_t extra) {

        ಆಯ್ಕೆ (compression) {
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                ಹಿಂತಿರುಗಿ decompress_startswith_gzip(src, src_ಗಾತ್ರ, ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len, extra);
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                ಹಿಂತಿರುಗಿ decompress_startswith_bzip2(src, src_ಗಾತ್ರ, ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len, extra);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ decompress_startswith_journal(compression, src, src_ಗಾತ್ರ, ಬಫರ್, ಪೂರ್ವಪ್ರತ್ಯಯ, ಪೂರ್ವಪ್ರತ್ಯಯ_len, extra);
        }
}

ಪೂರ್ಣಾಂಕ compress_ಹರಿವು(
                Compression type,
                ಪೂರ್ಣಾಂಕ fdf, ಪೂರ್ಣಾಂಕ fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_ಗಾತ್ರ) {

        _cleanup_(compressor_freep) Compressor *c = ಖಾಲಿ;
        _cleanup_free_ ಶೂನ್ಯ *buf = ಖಾಲಿ;
        _cleanup_free_ uint8_t *ಒಳಹರಿವು = ಖಾಲಿ;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t ಒಟ್ಟು_ಒಳ = 0, ಒಟ್ಟು_ಹೊರಹೋಗು = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fdf >= 0);
        ಪ್ರತಿಜ್ಞಾ(fdt >= 0);

        r = compressor_new(&c, type);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಒಳಹರಿವು = new(uint8_t, COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ);
        ಆದರೆ (!ಒಳಹರಿವು)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (;;) {
                size_t m = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ;
                ssize_t n;

                ಆದರೆ (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = ಪಠ(fdf, ಒಳಹರಿವು, m);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (n == 0) {
                        r = compressor_ಮುಕ್ತಾಯ(c, &buf, &buf_size, &buf_alloc);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ಆದರೆ (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                                ಒಟ್ಟು_ಹೊರಹೋಗು += buf_size;
                        }
                        ನಿಲ್ಲಿಸು;
                }

                ಒಟ್ಟು_ಒಳ += n;
                ಆದರೆ (max_bytes != UINT64_MAX) {
                        ಪ್ರತಿಜ್ಞಾ(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_ಆರಂಭ(c, ಒಳಹರಿವು, n, &buf, &buf_size, &buf_alloc);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಒಟ್ಟು_ಹೊರಹೋಗು += buf_size;
                }
        }

        ಆದರೆ (ret_uncompressed_ಗಾತ್ರ)
                *ret_uncompressed_ಗಾತ್ರ = ಒಟ್ಟು_ಒಳ;

        ಆದರೆ (ಒಟ್ಟು_ಒಳ == 0)
                log_debug("%s compression finished (no input data)", compression_ಗಮ್ಯ_string(type));
        ಇಲ್ಲದಿದ್ದರೆ
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_ಗಮ್ಯ_string(type), ಒಟ್ಟು_ಒಳ, ಒಟ್ಟು_ಹೊರಹೋಗು, (ದ್ವಿಗುಣ) ಒಟ್ಟು_ಹೊರಹೋಗು / ಒಟ್ಟು_ಒಳ * 100);

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ should_sparse(ಪೂರ್ಣಾಂಕ fd) {
        ರಚನೆ stat st;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (!S_ISREG(st.st_mode))
                ಹಿಂತಿರುಗಿ false;

        ಪೂರ್ಣಾಂಕ flags = fcntl(fd, F_GETFL);
        ಆದರೆ (flags < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (FLAGS_ಗಣ(flags, O_APPEND))
                ಹಿಂತಿರುಗಿ false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        ಆದರೆ (pos < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ finalize_sparse(ಪೂರ್ಣಾಂಕ fd) {
        off_t pos;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        ಆದರೆ (pos < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ftruncate(fd, pos) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 0;
}

/* Common helper for decompress_stream_*() wrappers */

ರಚನೆ decompress_ಹರಿವು_userdata {
        ಪೂರ್ಣಾಂಕ fd;
        uint64_t max_bytes;
        uint64_t ಒಟ್ಟು_ಹೊರಹೋಗು;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_ಹರಿವು_write_callback(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *userdata) {
        ರಚನೆ decompress_ಹರಿವು_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        ಆದರೆ (u->max_bytes != UINT64_MAX) {
                ಆದರೆ (u->max_bytes < ಗಾತ್ರ_2)
                        ಹಿಂತಿರುಗಿ -EFBIG;
                u->max_bytes -= ಗಾತ್ರ_2;
        }

        u->ಒಟ್ಟು_ಹೊರಹೋಗು += ಗಾತ್ರ_2;

        ಆದರೆ (u->skip > 0) {
                ಆದರೆ (u->skip >= ಗಾತ್ರ_2) {
                        u->skip -= ಗಾತ್ರ_2;
                        ಹಿಂತಿರುಗಿ 0;
                }

                ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ uint8_t*) ದತ್ತಾಂಶ + u->skip;
                ಗಾತ್ರ_2 -= u->skip;
                u->skip = 0;
        }

        ಆದರೆ (ಗಾತ್ರ_2 == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, ದತ್ತಾಂಶ, ಗಾತ್ರ_2, 64);
                ಆದರೆ (k < 0)
                        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) k;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ loop_write(u->fd, ದತ್ತಾಂಶ, ಗಾತ್ರ_2);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        ಆಯ್ಕೆ (type) {
        ಸ್ಥಿತಿ COMPRESSION_NONE:
                ನಿಲ್ಲಿಸು;

#ಆದರೆ HAVE_XZ
        ಸ್ಥಿತಿ COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_ಗಣ(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_ಗಣ(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_ಹರಿವು_decoder(&c->xz, UINT64_MAX, xz_flags);
                ಆದರೆ (xz_ret != LZMA_OK)
                        ಹಿಂತಿರುಗಿ -EIO;
                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
        ಸ್ಥಿತಿ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4_F_ಆವೃತ್ತಿ);
                ಆದರೆ (sym_LZ4F_isError(rc))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
        ಸ್ಥಿತಿ COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                ಆದರೆ (!c->d_zstd)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                ಆದರೆ (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_ಆವೃತ್ತಿ, (ಪೂರ್ಣಾಂಕ) ಗಾತ್ರ(c->gzip)) != Z_OK)
                        ಹಿಂತಿರುಗಿ -EIO;
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BZIP2
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                ಆದರೆ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        ಹಿಂತಿರುಗಿ -EIO;
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ decompress_blob_ಗಮ್ಯ_fd_impl(
                Compression type,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ಗಾತ್ರ) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(src);
        ಪ್ರತಿಜ್ಞಾ(fdt >= 0);

        ಆದರೆ (type == COMPRESSION_NONE)
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        ಆದರೆ (src_ಗಾತ್ರ == 0)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        ಆದರೆ (type == COMPRESSION_LZ4 &&
            src_ಗಾತ್ರ >= COMPRESSION_MAGIC_ಬೈಟುಗಳು_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                ಆದರೆ (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        ಆದರೆ (src_ಗಾತ್ರ <= ಗಾತ್ರ(uint64_t))
                                ಹಿಂತಿರುಗಿ -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        ಆದರೆ (sz < skip)
                                ಹಿಂತಿರುಗಿ -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                ಆದರೆ (max_bytes == 0)
                        ಹಿಂತಿರುಗಿ -ENOBUFS;

                _cleanup_free_ ಶೂನ್ಯ *buf = ಖಾಲಿ;
                size_t ಗಾತ್ರ_2 = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_ಗಾತ್ರ, &buf, &ಗಾತ್ರ_2,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                /* Check the result size again for safety. */
                ಆದರೆ (ಗಾತ್ರ_2 < skip)
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, ಗಾತ್ರ_2 - skip);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_ಗಾತ್ರ, ಗಾತ್ರ_2, (ದ್ವಿಗುಣ) ಗಾತ್ರ_2 / src_ಗಾತ್ರ * 100);

                ಆದರೆ (ret_ಗಾತ್ರ)
                        *ret_ಗಾತ್ರ = ಗಾತ್ರ_2 - skip;
                ಹಿಂತಿರುಗಿ 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = ಖಾಲಿ;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ರಚನೆ decompress_ಹರಿವು_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ತಳ್ಳು(c, src, src_ಗಾತ್ರ, decompress_ಹರಿವು_write_callback, &userdata);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Finalize */
        r = decompressor_ತಳ್ಳು(c, /* data= */ ಖಾಲಿ, /* size= */ 0, decompress_ಹರಿವು_write_callback, &userdata);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (userdata.ಒಟ್ಟು_ಹೊರಹೋಗು < skip)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (userdata.sparse) {
                r = finalize_sparse(fdt);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_ಗಮ್ಯ_string(type), src_ಗಾತ್ರ, userdata.ಒಟ್ಟು_ಹೊರಹೋಗು,
                  (ದ್ವಿಗುಣ) userdata.ಒಟ್ಟು_ಹೊರಹೋಗು / src_ಗಾತ್ರ * 100);

        ಆದರೆ (ret_ಗಾತ್ರ)
                *ret_ಗಾತ್ರ = userdata.ಒಟ್ಟು_ಹೊರಹೋಗು - skip;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ decompress_blob_ಗಮ್ಯ_fd_journal(
                Compression type,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ಗಾತ್ರ) {

        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ decompress_blob_ಗಮ್ಯ_fd_impl(type, src, src_ಗಾತ್ರ, fdt, max_bytes, skip, ret_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ decompress_blob_ಗಮ್ಯ_fd(
                Compression type,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src,
                uint64_t src_ಗಾತ್ರ,
                ಪೂರ್ಣಾಂಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_ಗಾತ್ರ) {

        ಪೂರ್ಣಾಂಕ r;

        r = dlopen_compress(type, LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ decompress_blob_ಗಮ್ಯ_fd_impl(type, src, src_ಗಾತ್ರ, fdt, max_bytes, skip, ret_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ decompress_ಹರಿವು_ತುಂಬಿದ(
                Compression type,
                ಪೂರ್ಣಾಂಕ fdf, ಪೂರ್ಣಾಂಕ fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = ಖಾಲಿ;
        _cleanup_free_ uint8_t *buf = ಖಾಲಿ;
        uint64_t ಒಟ್ಟು_ಒಳ = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fdf >= 0);
        ಪ್ರತಿಜ್ಞಾ(fdt >= 0);

        ಆದರೆ (type == COMPRESSION_NONE)
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = decompressor_new(&c, type, flags);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ರಚನೆ decompress_ಹರಿವು_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (;;) {
                ssize_t n;

                n = ಪಠ(fdf, buf, COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಒಟ್ಟು_ಒಳ += n;

                r = decompressor_ತಳ್ಳು(c, buf, n, decompress_ಹರಿವು_write_callback, &userdata);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (n == 0)
                        ನಿಲ್ಲಿಸು;
        }

        ಆದರೆ (ಒಟ್ಟು_ಒಳ == 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_ಗಮ್ಯ_string(type));

        ಆದರೆ (userdata.sparse) {
                r = finalize_sparse(fdt);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_ಗಮ್ಯ_string(type), ಒಟ್ಟು_ಒಳ, userdata.ಒಟ್ಟು_ಹೊರಹೋಗು,
                  (ದ್ವಿಗುಣ) userdata.ಒಟ್ಟು_ಹೊರಹೋಗು / ಒಟ್ಟು_ಒಳ * 100);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ decompress_ಹರಿವು_by_filename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಪೂರ್ಣಾಂಕ fdf, ಪೂರ್ಣಾಂಕ fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        ಆದರೆ (c == COMPRESSION_NONE)
                ಹಿಂತಿರುಗಿ -EPROTONOSUPPORT;

        ಹಿಂತಿರುಗಿ decompress_ಹರಿವು(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆಯ್ಕೆ (c->type) {

#ಆದರೆ HAVE_XZ
        ಸ್ಥಿತಿ COMPRESSION_XZ:
                sym_lzma_ಕೊನೆ(&c->xz);
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
        ಸ್ಥಿತಿ COMPRESSION_LZ4:
                ಆದರೆ (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = ಖಾಲಿ;
                        c->lz4_ಶೀರ್ಷಿಕೆ = mfree(c->lz4_ಶೀರ್ಷಿಕೆ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = ಖಾಲಿ;
                }
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
        ಸ್ಥಿತಿ COMPRESSION_ZSTD:
                ಆದರೆ (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = ಖಾಲಿ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = ಖಾಲಿ;
                }
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                ಆದರೆ (c->encoding)
                        sym_deflate_ಕೊನೆ(&c->gzip);
                ಇಲ್ಲದಿದ್ದರೆ
                        sym_inflate_ಕೊನೆ(&c->gzip);
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BZIP2
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                ಆದರೆ (c->encoding)
                        sym_BZ2_bz_Compress_ಕೊನೆ(&c->bzip2);
                ಇಲ್ಲದಿದ್ದರೆ
                        sym_BZ2_bz_Decompress_ಕೊನೆ(&c->bzip2);
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

        ಪೂರ್ವನಿಯೋಜಿತ:
                ನಿಲ್ಲಿಸು;
        }

        ಹಿಂತಿರುಗಿ mfree(c);
}

Compression compressor_type(ಸ್ಥಿರಾಂಕ Compressor *c) {
        ಹಿಂತಿರುಗಿ c ? c->type : _COMPRESSION_INVALID;
}

ಪೂರ್ಣಾಂಕ decompressor_detect(Decompressor **decompressor, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(decompressor);

        ಆದರೆ (*decompressor)
                ಹಿಂತಿರುಗಿ 1;

        ಆದರೆ (ಗಾತ್ರ_2 < COMPRESSION_MAGIC_ಬೈಟುಗಳು_MAX)
                ಹಿಂತಿರುಗಿ 0;

        ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ);

        Compression type = compression_detect_from_magic(ದತ್ತಾಂಶ);
        ಆದರೆ (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_ಗಮ್ಯ_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ decompressor_force(Decompressor **decompressor, Compression type) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(decompressor);
        ಪ್ರತಿಜ್ಞಾ(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *decompressor = compressor_free(*decompressor);
        ಹಿಂತಿರುಗಿ decompressor_new(decompressor, type, /* flags= */ 0);
}

ಪೂರ್ಣಾಂಕ decompressor_force_off(Decompressor **decompressor) {
        ಹಿಂತಿರುಗಿ decompressor_force(decompressor, COMPRESSION_NONE);
}

ಪೂರ್ಣಾಂಕ decompressor_ತಳ್ಳು(Decompressor *c, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t ಗಾತ್ರ_2, DecompressorCallback callback, ಶೂನ್ಯ *userdata) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(c);
        ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ || ಗಾತ್ರ_2 == 0);
        ಪ್ರತಿಜ್ಞಾ(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        ಆದರೆ (c->encoding)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (c->ಪೂರ್ಣಗೊಂಡ && ಗಾತ್ರ_2 == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *ಬಫರ್ = ಖಾಲಿ;
        ಆದರೆ (c->type != COMPRESSION_NONE) {
                ಬಫರ್ = new(uint8_t, COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ);
                ಆದರೆ (!ಬಫರ್)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಆಯ್ಕೆ (c->type) {

        ಸ್ಥಿತಿ COMPRESSION_NONE:
                ಆದರೆ (c->ಪೂರ್ಣಗೊಂಡ)
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ಆದರೆ (ಗಾತ್ರ_2 == 0) {
                        c->ಪೂರ್ಣಗೊಂಡ = true;
                        ಹಿಂತಿರುಗಿ 0;
                }

                r = callback(ದತ್ತಾಂಶ, ಗಾತ್ರ_2, userdata);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ HAVE_XZ
        ಸ್ಥಿತಿ COMPRESSION_XZ:
                ಆದರೆ (c->ಪೂರ್ಣಗೊಂಡ)
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                c->xz.ಮುಂದಿನ_ಒಳ = ದತ್ತಾಂಶ;
                c->xz.avail_ಒಳ = ಗಾತ್ರ_2;

                ಗಾಗಿ (;;) {
                        c->xz.ಮುಂದಿನ_ಹೊರಹೋಗು = ಬಫರ್;
                        c->xz.avail_ಹೊರಹೋಗು = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ;

                        lzma_ret lzr = sym_lzma_ಸಂಕೇತ(&c->xz, ಗಾತ್ರ_2 == 0 ? LZMA_ಮುಕ್ತಾಯ : LZMA_ಸತತ);
                        ಆದರೆ (!ಒಳ_ಗಣ(lzr, LZMA_OK, LZMA_ಹರಿವು_ಕೊನೆ))
                                ಹಿಂತಿರುಗಿ -EBADMSG;

                        ಆದರೆ (c->xz.avail_ಹೊರಹೋಗು > COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ)
                                ಹಿಂತಿರುಗಿ -EIO;

                        size_t produced = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ - c->xz.avail_ಹೊರಹೋಗು;
                        ಆದರೆ (produced > 0) {
                                r = callback(ಬಫರ್, produced, userdata);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }

                        ಆದರೆ (lzr == LZMA_ಹರಿವು_ಕೊನೆ) {
                                ಆದರೆ (c->xz.avail_ಒಳ > 0)
                                        ಹಿಂತಿರುಗಿ -EBADMSG; /* garbage data at the end */

                                c->ಪೂರ್ಣಗೊಂಡ = true;
                                ಹಿಂತಿರುಗಿ 0;
                        }

                        ಆದರೆ (c->xz.avail_ಒಳ == 0 && c->xz.avail_ಹೊರಹೋಗು > 0) {
                                ಆದರೆ (ಗಾತ್ರ_2 == 0)
                                        ಹಿಂತಿರುಗಿ -EBADMSG; /* truncated */

                                ಹಿಂತಿರುಗಿ 0; /* No progress possible with the current input. */
                        }
                }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
        ಸ್ಥಿತಿ COMPRESSION_LZ4: {
                ಸ್ಥಿರಾಂಕ uint8_t *src = ದತ್ತಾಂಶ;
                size_t src_ಉಳಿದ = ಗಾತ್ರ_2;

                c->ಪೂರ್ಣಗೊಂಡ = false;

                ಗಾಗಿ (;;) {
                        size_t produced = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ;
                        size_t consumed = src_ಉಳಿದ;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, ಬಫರ್, &produced, src, &consumed, ಖಾಲಿ);
                        ಆದರೆ (sym_LZ4F_isError(rc))
                                ಹಿಂತಿರುಗಿ -EBADMSG;

                        ಆದರೆ (consumed > src_ಉಳಿದ ||
                            produced > COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ)
                                ಹಿಂತಿರುಗಿ -EIO;

                        ಆದರೆ (consumed > 0) {
                                ಪ್ರತಿಜ್ಞಾ(src);
                                src += consumed;
                                src_ಉಳಿದ -= consumed;
                        }

                        ಆದರೆ (produced > 0) {
                                r = callback(ಬಫರ್, produced, userdata);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }

                        ಆದರೆ (src_ಉಳಿದ == 0) {
                                ಆದರೆ (rc == 0) {
                                        /* The current stream has finished. */
                                        c->ಪೂರ್ಣಗೊಂಡ = true;
                                        ಹಿಂತಿರುಗಿ 0;
                                }

                                ಆದರೆ (produced == COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ಮುಂದುವರಿ;

                                ಆದರೆ (ಗಾತ್ರ_2 == 0)
                                        ಹಿಂತಿರುಗಿ -EBADMSG; /* truncated */

                                ಹಿಂತಿರುಗಿ 0; /* No progress possible with the current input. */
                        }
                }
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
        ಸ್ಥಿತಿ COMPRESSION_ZSTD: {
                ZSTD_ಒಳ_ಬಫರ್ ಒಳಹರಿವು = {
                        .src =  (ಶೂನ್ಯ*) ದತ್ತಾಂಶ,
                        .ಗಾತ್ರ_2 = ಗಾತ್ರ_2,
                };

                c->ಪೂರ್ಣಗೊಂಡ = false;

                ಗಾಗಿ (;;) {
                        ZSTD_ಹೊರಹೋಗು_ಬಫರ್ output = {
                                .dst = ಬಫರ್,
                                .ಗಾತ್ರ_2 = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ,
                        };

                        size_t res = sym_ZSTD_decompress_ಹರಿವು(c->d_zstd, &output, &ಒಳಹರಿವು);
                        ಆದರೆ (sym_ZSTD_isError(res))
                                ಹಿಂತಿರುಗಿ -EBADMSG;

                        ಆದರೆ (ಒಳಹರಿವು.pos > ಒಳಹರಿವು.ಗಾತ್ರ_2 ||
                            output.pos > output.ಗಾತ್ರ_2)
                                ಹಿಂತಿರುಗಿ -EIO;

                        ಆದರೆ (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }

                        ಆದರೆ (ಒಳಹರಿವು.pos == ಒಳಹರಿವು.ಗಾತ್ರ_2) {
                                ಆದರೆ (res == 0) {
                                        /* The current stream has finished. */
                                        c->ಪೂರ್ಣಗೊಂಡ = true;
                                        ಹಿಂತಿರುಗಿ 0;
                                }

                                ಆದರೆ (output.pos == COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ಮುಂದುವರಿ;

                                ಆದರೆ (ಗಾತ್ರ_2 == 0)
                                        ಹಿಂತಿರುಗಿ -EBADMSG; /* truncated */

                                ಹಿಂತಿರುಗಿ 0; /* No progress possible with the current input. */
                        }
                }
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                ಆದರೆ (ಗಾತ್ರ_2 > UINT_MAX)
                        ಹಿಂತಿರುಗಿ -EFBIG;

                c->gzip.ಮುಂದಿನ_ಒಳ = (ಶೂನ್ಯ*) ದತ್ತಾಂಶ;
                c->gzip.avail_ಒಳ = ಗಾತ್ರ_2;

                ಗಾಗಿ (;;) {
                        ಆದರೆ (c->ಪೂರ್ಣಗೊಂಡ) {
                                ಆದರೆ (sym_inflateReset(&c->gzip) != Z_OK)
                                        ಹಿಂತಿರುಗಿ -EIO;

                                c->ಪೂರ್ಣಗೊಂಡ = false;
                        }

                        c->gzip.ಮುಂದಿನ_ಹೊರಹೋಗು = ಬಫರ್;
                        c->gzip.avail_ಹೊರಹೋಗು = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        ಪೂರ್ಣಾಂಕ zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        ಆದರೆ (!ಒಳ_ಗಣ(zr, Z_OK, Z_ಹರಿವು_ಕೊನೆ) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_ಒಳ == 0))
                                ಹಿಂತಿರುಗಿ -EBADMSG;

                        ಆದರೆ (c->gzip.avail_ಹೊರಹೋಗು > COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ)
                                ಹಿಂತಿರುಗಿ -EIO;

                        size_t produced = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ - c->gzip.avail_ಹೊರಹೋಗು;
                        ಆದರೆ (produced > 0) {
                                r = callback(ಬಫರ್, produced, userdata);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }

                        ಆದರೆ (zr == Z_ಹರಿವು_ಕೊನೆ) {
                                c->ಪೂರ್ಣಗೊಂಡ = true;

                                ಆದರೆ (c->gzip.avail_ಒಳ > 0)
                                        ಮುಂದುವರಿ; /* New stream may be pending. */

                                ಹಿಂತಿರುಗಿ 0;
                        }

                        ಆದರೆ (c->gzip.avail_ಒಳ == 0 && c->gzip.avail_ಹೊರಹೋಗು > 0) {
                                ಆದರೆ (ಗಾತ್ರ_2 == 0)
                                        ಹಿಂತಿರುಗಿ -EBADMSG; /* truncated */

                                ಹಿಂತಿರುಗಿ 0; /* No progress possible with the current input. */
                        }
                }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BZIP2
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                ಆದರೆ (ಗಾತ್ರ_2 > UINT_MAX)
                        ಹಿಂತಿರುಗಿ -EFBIG;

                c->bzip2.ಮುಂದಿನ_ಒಳ = (ಅಕ್ಷರ*) ದತ್ತಾಂಶ;
                c->bzip2.avail_ಒಳ = ಗಾತ್ರ_2;

                ಗಾಗಿ (;;) {
                        ಆದರೆ (c->ಪೂರ್ಣಗೊಂಡ) {
                                sym_BZ2_bz_Decompress_ಕೊನೆ(&c->bzip2);
                                ಆದರೆ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        ಹಿಂತಿರುಗಿ -EIO;

                                c->ಪೂರ್ಣಗೊಂಡ = false;
                        }

                        c->bzip2.ಮುಂದಿನ_ಹೊರಹೋಗು = (ಅಕ್ಷರ*) ಬಫರ್;
                        c->bzip2.avail_ಹೊರಹೋಗು = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ;

                        ಪೂರ್ಣಾಂಕ bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        ಆದರೆ (!ಒಳ_ಗಣ(bzr, BZ_OK, BZ_ಹರಿವು_ಕೊನೆ))
                                ಹಿಂತಿರುಗಿ -EBADMSG;

                        ಆದರೆ (c->bzip2.avail_ಹೊರಹೋಗು > COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ)
                                ಹಿಂತಿರುಗಿ -EIO;

                        size_t produced = COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ - c->bzip2.avail_ಹೊರಹೋಗು;
                        ಆದರೆ (produced > 0) {
                                r = callback(ಬಫರ್, produced, userdata);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }

                        ಆದರೆ (bzr == BZ_ಹರಿವು_ಕೊನೆ) {
                                c->ಪೂರ್ಣಗೊಂಡ = true;

                                ಆದರೆ (c->bzip2.avail_ಒಳ > 0)
                                        ಮುಂದುವರಿ; /* New stream may be pending. */

                                ಹಿಂತಿರುಗಿ 0;
                        }

                        ಆದರೆ (c->bzip2.avail_ಒಳ == 0 && c->bzip2.avail_ಹೊರಹೋಗು > 0) {
                                ಆದರೆ (ಗಾತ್ರ_2 == 0)
                                        ಹಿಂತಿರುಗಿ -EBADMSG; /* truncated */

                                ಹಿಂತಿರುಗಿ 0; /* No progress possible with the current input. */
                        }
                }
#ಆದರೆ_ಅಂತ್ಯ

        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }
}

ಪೂರ್ಣಾಂಕ compressor_new(Compressor **ret, Compression type) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        ಆಯ್ಕೆ (type) {

#ಆದರೆ HAVE_XZ
        ಸ್ಥಿತಿ COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                ಆದರೆ (xzr != LZMA_OK)
                        ಹಿಂತಿರುಗಿ -EIO;

                c->type = COMPRESSION_XZ;
                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
        ಸ್ಥಿತಿ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4_F_ಆವೃತ್ತಿ);
                ಆದರೆ (sym_LZ4F_isError(rc))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t ಶೀರ್ಷಿಕೆ_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_ಶೀರ್ಷಿಕೆ = ಸ್ಮರಣೆ_ಕೊಡು(ಶೀರ್ಷಿಕೆ_bound);
                ಆದರೆ (!c->lz4_ಶೀರ್ಷಿಕೆ)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                c->lz4_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_ಶೀರ್ಷಿಕೆ, ಶೀರ್ಷಿಕೆ_bound, &lz4_preferences);
                ಆದರೆ (sym_LZ4F_isError(c->lz4_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
        ಸ್ಥಿತಿ COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                ಆದರೆ (!c->c_zstd)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_ಗಣ_Parameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                ಆದರೆ (sym_ZSTD_isError(z))
                        ಹಿಂತಿರುಗಿ -EIO;

                z = sym_ZSTD_CCtx_ಗಣ_Parameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                ಆದರೆ (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_ಆವೃತ್ತಿ, (ಪೂರ್ಣಾಂಕ) ಗಾತ್ರ(c->gzip));
                ಆದರೆ (r != Z_OK)
                        ಹಿಂತಿರುಗಿ -EIO;

                c->type = COMPRESSION_GZIP;
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BZIP2
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                ಆದರೆ (r != BZ_OK)
                        ಹಿಂತಿರುಗಿ -EIO;

                c->type = COMPRESSION_BZIP2;
                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

        ಸ್ಥಿತಿ COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ enlarge_ಬಫರ್(ಶೂನ್ಯ **ಬಫರ್, size_t *ಬಫರ್_ಗಾತ್ರ, size_t *ಬಫರ್_allocated, size_t need) {
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್_ಗಾತ್ರ);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್_allocated);

        need = MAX3(need, *ಬಫರ್_ಗಾತ್ರ + 1, (size_t) COMPRESS_PIPE_ಬಫರ್_ಗಾತ್ರ);
        ಆದರೆ (*ಬಫರ್_allocated >= need)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (!greedy_realloc(ಬಫರ್, need, 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ಬಫರ್_allocated = MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(*ಬಫರ್);
        ಹಿಂತಿರುಗಿ 1;
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ compressor_ಆರಂಭ(
                Compressor *c,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ,
                size_t ಗಾತ್ರ_2,
                ಶೂನ್ಯ **ಬಫರ್,
                size_t *ಬಫರ್_ಗಾತ್ರ,
                size_t *ಬಫರ್_allocated) {

#ಆದರೆ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ಪೂರ್ಣಾಂಕ r;
#ಆದರೆ_ಅಂತ್ಯ

        ಪ್ರತಿಜ್ಞಾ(c);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್_ಗಾತ್ರ);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್_allocated);

        ಆದರೆ (!c->encoding)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ಗಾತ್ರ_2 == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ);

        *ಬಫರ್_ಗಾತ್ರ = 0;

        ಆಯ್ಕೆ (c->type) {

#ಆದರೆ HAVE_XZ
        ಸ್ಥಿತಿ COMPRESSION_XZ:

                c->xz.ಮುಂದಿನ_ಒಳ = ದತ್ತಾಂಶ;
                c->xz.avail_ಒಳ = ಗಾತ್ರ_2;

                ಆಗ (c->xz.avail_ಒಳ > 0) {
                        lzma_ret lzr;

                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, /* need= */ 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        c->xz.ಮುಂದಿನ_ಹೊರಹೋಗು = (uint8_t*) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ;
                        c->xz.avail_ಹೊರಹೋಗು = *ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ;

                        lzr = sym_lzma_ಸಂಕೇತ(&c->xz, LZMA_ಸತತ);
                        ಆದರೆ (lzr != LZMA_OK)
                                ಹಿಂತಿರುಗಿ -EIO;

                        *ಬಫರ್_ಗಾತ್ರ += (*ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ) - c->xz.avail_ಹೊರಹೋಗು;
                }

                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
        ಸ್ಥಿತಿ COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                ಆದರೆ (c->lz4_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ > 0) {
                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, c->lz4_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ಸ್ಮರಣೆ_ನಕಲು(*ಬಫರ್, c->lz4_ಶೀರ್ಷಿಕೆ, c->lz4_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
                        *ಬಫರ್_ಗಾತ್ರ = c->lz4_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
                        c->lz4_ಶೀರ್ಷಿಕೆ = mfree(c->lz4_ಶೀರ್ಷಿಕೆ);
                        c->lz4_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ = 0;
                }

                size_t bound = sym_LZ4F_compressBound(ಗಾತ್ರ_2, &lz4_preferences);
                r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, *ಬಫರ್_ಗಾತ್ರ + bound);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ,
                                                   *ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ,
                                                   ದತ್ತಾಂಶ, ಗಾತ್ರ_2, ಖಾಲಿ);
                ಆದರೆ (sym_LZ4F_isError(n))
                        ಹಿಂತಿರುಗಿ -EIO;

                *ಬಫರ್_ಗಾತ್ರ += n;
                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
        ಸ್ಥಿತಿ COMPRESSION_ZSTD: {
                ZSTD_ಒಳ_ಬಫರ್ ಒಳಹರಿವು = {
                        .src = ದತ್ತಾಂಶ,
                        .ಗಾತ್ರ_2 = ಗಾತ್ರ_2,
                };

                ಆಗ (ಒಳಹರಿವು.pos < ಒಳಹರಿವು.ಗಾತ್ರ_2) {
                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, /* need= */ 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ZSTD_ಹೊರಹೋಗು_ಬಫರ್ output = {
                                .dst = ((uint8_t *) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ),
                                .ಗಾತ್ರ_2 = *ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ಒಳಹರಿವು, ZSTD_e_continue);
                        ಆದರೆ (sym_ZSTD_isError(res))
                                ಹಿಂತಿರುಗಿ -EIO;

                        *ಬಫರ್_ಗಾತ್ರ += output.pos;
                }

                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                ಆದರೆ (ಗಾತ್ರ_2 > UINT_MAX)
                        ಹಿಂತಿರುಗಿ -EFBIG;

                c->gzip.ಮುಂದಿನ_ಒಳ = (ಶೂನ್ಯ*) ದತ್ತಾಂಶ;
                c->gzip.avail_ಒಳ = ಗಾತ್ರ_2;

                ಆಗ (c->gzip.avail_ಒಳ > 0) {
                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, /* need= */ 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        size_t avail = MIN(*ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ, (size_t) UINT_MAX);
                        c->gzip.ಮುಂದಿನ_ಹೊರಹೋಗು = (uint8_t*) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ;
                        c->gzip.avail_ಹೊರಹೋಗು = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        ಆದರೆ (r != Z_OK)
                                ಹಿಂತಿರುಗಿ -EIO;

                        *ಬಫರ್_ಗಾತ್ರ += avail - c->gzip.avail_ಹೊರಹೋಗು;
                }

                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BZIP2
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                ಆದರೆ (ಗಾತ್ರ_2 > UINT_MAX)
                        ಹಿಂತಿರುಗಿ -EFBIG;

                c->bzip2.ಮುಂದಿನ_ಒಳ = (ಶೂನ್ಯ*) ದತ್ತಾಂಶ;
                c->bzip2.avail_ಒಳ = ಗಾತ್ರ_2;

                ಆಗ (c->bzip2.avail_ಒಳ > 0) {
                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, /* need= */ 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        size_t avail = MIN(*ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ, (size_t) UINT_MAX);
                        c->bzip2.ಮುಂದಿನ_ಹೊರಹೋಗು = (ಶೂನ್ಯ*) ((uint8_t*) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ);
                        c->bzip2.avail_ಹೊರಹೋಗು = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_ಸತತ);
                        ಆದರೆ (r != BZ_ಸತತ_OK)
                                ಹಿಂತಿರುಗಿ -EIO;

                        *ಬಫರ್_ಗಾತ್ರ += avail - c->bzip2.avail_ಹೊರಹೋಗು;
                }

                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

        ಸ್ಥಿತಿ COMPRESSION_NONE:

                ಆದರೆ (*ಬಫರ್_allocated < ಗಾತ್ರ_2) {
                        ಶೂನ್ಯ *p;

                        p = realloc(*ಬಫರ್, ಗಾತ್ರ_2);
                        ಆದರೆ (!p)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        *ಬಫರ್ = p;
                        *ಬಫರ್_allocated = ಗಾತ್ರ_2;
                }

                ಸ್ಮರಣೆ_ನಕಲು(*ಬಫರ್, ದತ್ತಾಂಶ, ಗಾತ್ರ_2);
                *ಬಫರ್_ಗಾತ್ರ = ಗಾತ್ರ_2;
                ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ compressor_ಮುಕ್ತಾಯ(Compressor *c, ಶೂನ್ಯ **ಬಫರ್, size_t *ಬಫರ್_ಗಾತ್ರ, size_t *ಬಫರ್_allocated) {
#ಆದರೆ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        ಪೂರ್ಣಾಂಕ r;
#ಆದರೆ_ಅಂತ್ಯ

        ಪ್ರತಿಜ್ಞಾ(c);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್_ಗಾತ್ರ);
        ಪ್ರತಿಜ್ಞಾ(ಬಫರ್_allocated);

        ಆದರೆ (!c->encoding)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ಬಫರ್_ಗಾತ್ರ = 0;

        ಆಯ್ಕೆ (c->type) {

#ಆದರೆ HAVE_XZ
        ಸ್ಥಿತಿ COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_ಒಳ = 0;

                ಮಾಡು {
                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, /* need= */ 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        c->xz.ಮುಂದಿನ_ಹೊರಹೋಗು = (uint8_t*) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ;
                        c->xz.avail_ಹೊರಹೋಗು = *ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ;

                        lzr = sym_lzma_ಸಂಕೇತ(&c->xz, LZMA_ಮುಕ್ತಾಯ);
                        ಆದರೆ (!ಒಳ_ಗಣ(lzr, LZMA_OK, LZMA_ಹರಿವು_ಕೊನೆ))
                                ಹಿಂತಿರುಗಿ -EIO;

                        *ಬಫರ್_ಗಾತ್ರ += (*ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ) - c->xz.avail_ಹೊರಹೋಗು;
                } ಆಗ (lzr != LZMA_ಹರಿವು_ಕೊನೆ);

                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_LZ4
        ಸ್ಥಿತಿ COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, bound);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                size_t n = sym_LZ4_F_compress_ಕೊನೆ(c->c_lz4, *ಬಫರ್, *ಬಫರ್_allocated, ಖಾಲಿ);
                ಆದರೆ (sym_LZ4F_isError(n))
                        ಹಿಂತಿರುಗಿ -EIO;

                *ಬಫರ್_ಗಾತ್ರ = n;
                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZSTD
        ಸ್ಥಿತಿ COMPRESSION_ZSTD: {
                ZSTD_ಒಳ_ಬಫರ್ ಒಳಹರಿವು = {};
                size_t res;

                ಮಾಡು {
                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, /* need= */ 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        ZSTD_ಹೊರಹೋಗು_ಬಫರ್ output = {
                                .dst = ((uint8_t *) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ),
                                .ಗಾತ್ರ_2 = *ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ಒಳಹರಿವು, ZSTD_e_ಕೊನೆ);
                        ಆದರೆ (sym_ZSTD_isError(res))
                                ಹಿಂತಿರುಗಿ -EIO;

                        *ಬಫರ್_ಗಾತ್ರ += output.pos;
                } ಆಗ (res != 0);

                ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_ZLIB
        ಸ್ಥಿತಿ COMPRESSION_GZIP:
                c->gzip.avail_ಒಳ = 0;

                ಮಾಡು {
                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, /* need= */ 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        size_t avail = MIN(*ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ, (size_t) UINT_MAX);
                        c->gzip.ಮುಂದಿನ_ಹೊರಹೋಗು = (uint8_t*) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ;
                        c->gzip.avail_ಹೊರಹೋಗು = avail;

                        r = sym_deflate(&c->gzip, Z_ಮುಕ್ತಾಯ);
                        ಆದರೆ (!ಒಳ_ಗಣ(r, Z_OK, Z_ಹರಿವು_ಕೊನೆ))
                                ಹಿಂತಿರುಗಿ -EIO;

                        *ಬಫರ್_ಗಾತ್ರ += avail - c->gzip.avail_ಹೊರಹೋಗು;
                } ಆಗ (r != Z_ಹರಿವು_ಕೊನೆ);

                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_BZIP2
        ಸ್ಥಿತಿ COMPRESSION_BZIP2:
                c->bzip2.avail_ಒಳ = 0;

                ಮಾಡು {
                        r = enlarge_ಬಫರ್(ಬಫರ್, ಬಫರ್_ಗಾತ್ರ, ಬಫರ್_allocated, /* need= */ 0);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        size_t avail = MIN(*ಬಫರ್_allocated - *ಬಫರ್_ಗಾತ್ರ, (size_t) UINT_MAX);
                        c->bzip2.ಮುಂದಿನ_ಹೊರಹೋಗು = (ಶೂನ್ಯ*) ((uint8_t*) *ಬಫರ್ + *ಬಫರ್_ಗಾತ್ರ);
                        c->bzip2.avail_ಹೊರಹೋಗು = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_ಮುಕ್ತಾಯ);
                        ಆದರೆ (!ಒಳ_ಗಣ(r, BZ_ಮುಕ್ತಾಯ_OK, BZ_ಹರಿವು_ಕೊನೆ))
                                ಹಿಂತಿರುಗಿ -EIO;

                        *ಬಫರ್_ಗಾತ್ರ += avail - c->bzip2.avail_ಹೊರಹೋಗು;
                } ಆಗ (r != BZ_ಹರಿವು_ಕೊನೆ);

                ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

        ಸ್ಥಿತಿ COMPRESSION_NONE:
                ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
        }

        ಹಿಂತಿರುಗಿ 0;
}
