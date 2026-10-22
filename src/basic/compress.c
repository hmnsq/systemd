/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#எனில் HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#அந்தர்பாவ <lzma.h>
#endif

#எனில் HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#அந்தர்பாவ <lz4.h>
#அந்தர்பாவ <lz4frame.h>
#அந்தர்பாவ <lz4hc.h>
#endif

#எனில் HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#அந்தர்பாவ <zstd.h>
#அந்தர்பாவ <zstd_errors.h>
#endif

#எனில் HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#அந்தர்பாவ <zlib.h>
#endif

#எனில் HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#அந்தர்பாவ <bzlib.h>
#endif

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "bitfield.h"
#அந்தர்பாவ "compress.h"
#அந்தர்பாவ "dlfcn-util.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "unaligned.h"

#எனில் HAVE_XZ
நிலையான DLSYM_PROTOTYPE(lzma_auto_decoder) = வெற்று;
நிலையான DLSYM_PROTOTYPE(lzma_code) = வெற்று;
நிலையான DLSYM_PROTOTYPE(lzma_easy_encoder) = வெற்று;
நிலையான DLSYM_PROTOTYPE(lzma_end) = வெற்று;
நிலையான DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = வெற்று;
நிலையான DLSYM_PROTOTYPE(lzma_stream_decoder) = வெற்று;
நிலையான DLSYM_PROTOTYPE(lzma_lzma_preset) = வெற்று;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
நிலையான உள்வரி வெறுமை lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#எனில் HAVE_LZ4
நிலையான DLSYM_PROTOTYPE(LZ4F_compressBegin) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_compressBound) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_compressEnd) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_compressUpdate) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_decompress) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4F_isError) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4_compress_HC) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4_compress_default) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4_decompress_safe) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = வெற்று;
நிலையான DLSYM_PROTOTYPE(LZ4_version_எண்) = வெற்று;

நிலையான மாறிலி LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#எனில் HAVE_ZSTD
நிலையான DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_compress) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_compressStream2) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_createCCtx) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_createDCtx) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_decompressStream) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_freeCCtx) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_freeDCtx) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_getErrorCode) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_getErrorName) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = வெற்று;
நிலையான DLSYM_PROTOTYPE(ZSTD_isError) = வெற்று;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, வெற்று);

நிலையான முழு zstd_ret_to_errno(size_t ret) {
        தேர்வு (sym_ZSTD_getErrorCode(ret)) {
        நிலை ZSTD_error_dstSize_tooSmall:
                திரும்பு -ENOBUFS;
        நிலை ZSTD_error_memory_allocation:
                திரும்பு -ENOMEM;
        இயல்பு:
                திரும்பு -EBADMSG;
        }
}
#endif

#எனில் HAVE_ZLIB
நிலையான DLSYM_PROTOTYPE(deflateInit2_) = வெற்று;
நிலையான DLSYM_PROTOTYPE(deflate) = வெற்று;
நிலையான DLSYM_PROTOTYPE(deflateEnd) = வெற்று;
நிலையான DLSYM_PROTOTYPE(inflateInit2_) = வெற்று;
நிலையான DLSYM_PROTOTYPE(inflate) = வெற்று;
நிலையான DLSYM_PROTOTYPE(inflateEnd) = வெற்று;
நிலையான DLSYM_PROTOTYPE(inflateReset) = வெற்று;

நிலையான உள்வரி வெறுமை deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

நிலையான உள்வரி வெறுமை inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#எனில் HAVE_BZIP2
நிலையான DLSYM_PROTOTYPE(BZ2_bzCompressInit) = வெற்று;
நிலையான DLSYM_PROTOTYPE(BZ2_bzCompress) = வெற்று;
நிலையான DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = வெற்று;
நிலையான DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = வெற்று;
நிலையான DLSYM_PROTOTYPE(BZ2_bzDecompress) = வெற்று;
நிலையான DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = வெற்று;

நிலையான உள்வரி வெறுமை BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

நிலையான உள்வரி வெறுமை BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
கட்டமைப்பு Compressor {
        Compression type;
        bool encoding;
        bool finished;
        ஒன்றிப்பு {
#எனில் HAVE_XZ
                lzma_stream xz;
#endif
#எனில் HAVE_LZ4
                கட்டமைப்பு {
                        LZ4F_compressionContext_t c_lz4;
                        வெறுமை *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#எனில் HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#எனில் HAVE_ZLIB
                z_stream gzip;
#endif
#எனில் HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, அளவு(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

நிலையான மாறிலி எழுத்து* மாறிலி compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

நிலையான மாறிலி எழுத்து* மாறிலி compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

நிலையான மாறிலி எழுத்து* மாறிலி compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(மாறிலி எழுத்து *s) {
        Compression c;

        ப்ரதிஜ்ஞா(s);

        c = compression_from_string(s);
        எனில் (c >= 0)
                திரும்பு c;

        திரும்பு compression_uppercase_from_string(s);
}

Compression compression_from_filename(மாறிலி எழுத்து *filename) {
        Compression c;
        மாறிலி எழுத்து *e;

        ப்ரதிஜ்ஞா(filename);

        e = strrchr(filename, '.');
        எனில் (!e)
                திரும்பு COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        எனில் (c < 0)
                திரும்பு COMPRESSION_NONE;

        திரும்பு c;
}

bool compression_supported_journal(Compression c) {
        நிலையான மாறிலி unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        ப்ரதிஜ்ஞா(c >= 0);
        ப்ரதிஜ்ஞா(c < _COMPRESSION_MAX);

        திரும்பு BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        நிலையான மாறிலி unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        ப்ரதிஜ்ஞா(c >= 0);
        ப்ரதிஜ்ஞா(c < _COMPRESSION_MAX);

        திரும்பு BIT_SET(supported, c);
}

Compression compression_detect_from_magic(மாறிலி uint8_t data[நிலையான COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        எனில் (memcmp(data, (மாறிலி uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                திரும்பு COMPRESSION_GZIP;
        எனில் (memcmp(data, (மாறிலி uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                திரும்பு COMPRESSION_XZ;
        எனில் (memcmp(data, (மாறிலி uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                திரும்பு COMPRESSION_ZSTD;
        எனில் (memcmp(data, (மாறிலி uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                திரும்பு COMPRESSION_LZ4;
        எனில் (memcmp(data, (மாறிலி uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                திரும்பு COMPRESSION_BZIP2;

        திரும்பு _COMPRESSION_INVALID;
}

முழு dlopen_xz(முழு log_level) {
#எனில் HAVE_XZ
        நிலையான வெறுமை *lzma_dl = வெற்று;

        LIBLZMA_NOTE(suggested);

        திரும்பு dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#இல்லையெனில்
        திரும்பு log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

முழு dlopen_lz4(முழு log_level) {
#எனில் HAVE_LZ4
        நிலையான வெறுமை *lz4_dl = வெற்று;

        LIBLZ4_NOTE(suggested);

        திரும்பு dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_எண்));
#இல்லையெனில்
        திரும்பு log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

முழு dlopen_zstd(முழு log_level) {
#எனில் HAVE_ZSTD
        நிலையான வெறுமை *zstd_dl = வெற்று;

        LIBZSTD_NOTE(suggested);

        திரும்பு dlopen_many_sym_or_warn(
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
#இல்லையெனில்
        திரும்பு log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

முழு dlopen_zlib(முழு log_level) {
#எனில் HAVE_ZLIB
        நிலையான வெறுமை *zlib_dl = வெற்று;

        LIBZ_NOTE(suggested);

        திரும்பு dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#இல்லையெனில்
        திரும்பு log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

முழு dlopen_bzip2(முழு log_level) {
#எனில் HAVE_BZIP2
        நிலையான வெறுமை *bzip2_dl = வெற்று;

        LIBBZ2_NOTE(suggested);

        திரும்பு dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#இல்லையெனில்
        திரும்பு log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

முழு dlopen_compress_journal(Compression c, முழு log_level) {
        தேர்வு (c) {
        நிலை COMPRESSION_NONE:
                திரும்பு 0;
        நிலை COMPRESSION_XZ:
                திரும்பு dlopen_xz(log_level);
        நிலை COMPRESSION_LZ4:
                திரும்பு dlopen_lz4(log_level);
        நிலை COMPRESSION_ZSTD:
                திரும்பு dlopen_zstd(log_level);
        இயல்பு:
                திரும்பு -EOPNOTSUPP;
        }
}

முழு dlopen_compress(Compression c, முழு log_level) {
        தேர்வு (c) {
        நிலை COMPRESSION_GZIP:
                திரும்பு dlopen_zlib(log_level);
        நிலை COMPRESSION_BZIP2:
                திரும்பு dlopen_bzip2(log_level);
        இயல்பு:
                திரும்பு dlopen_compress_journal(c, log_level);
        }
}

நிலையான முழு compress_blob_xz(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                முழு level) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_alloc_size > 0);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, வெற்று, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, வெற்று }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        முழு r;

        r = dlopen_xz(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                எனில் (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        திரும்பு -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        எனில் (src_size < 80)
                திரும்பு -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ வெற்று,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        எனில் (ret != LZMA_OK)
                திரும்பு -ENOBUFS;

        *dst_size = out_pos;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு compress_blob_lz4(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                முழு level) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_alloc_size > 0);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_LZ4
        முழு r;

        r = dlopen_lz4(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        எனில் (src_size < 9)
                திரும்பு -ENOBUFS;

        எனில் (src_size > INT_MAX)
                திரும்பு -EFBIG;
        எனில் (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        எனில் (level <= 0)
                r = sym_LZ4_compress_default(src, (எழுத்து*)dst + 8, src_size, (முழு) dst_alloc_size - 8);
        இல்லையெனில்
                r = sym_LZ4_compress_HC(src, (எழுத்து*)dst + 8, src_size, (முழு) dst_alloc_size - 8, level);
        எனில் (r <= 0)
                திரும்பு -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு compress_blob_zstd(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                முழு level) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_alloc_size > 0);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_ZSTD
        size_t k;
        முழு r;

        r = dlopen_zstd(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        எனில் (sym_ZSTD_isError(k))
                திரும்பு zstd_ret_to_errno(k);

        *dst_size = k;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு compress_blob_gzip(மாறிலி வெறுமை *src, uint64_t src_size,
                       வெறுமை *dst, size_t dst_alloc_size, size_t *dst_size, முழு level) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_alloc_size > 0);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_ZLIB
        முழு r;

        r = dlopen_zlib(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size > UINT_MAX)
                திரும்பு -EFBIG;
        எனில் (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (முழு) அளவு(s));
        எனில் (r != Z_OK)
                திரும்பு -ENOMEM;

        s.next_in = (வெறுமை*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        எனில் (r != Z_STREAM_END)
                திரும்பு -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு compress_blob_bzip2(
                மாறிலி வெறுமை *src, uint64_t src_size,
                வெறுமை *dst, size_t dst_alloc_size, size_t *dst_size, முழு level) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_alloc_size > 0);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_BZIP2
        முழு r;

        r = dlopen_bzip2(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size > UINT_MAX)
                திரும்பு -EFBIG;
        எனில் (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        எனில் (r != BZ_OK)
                திரும்பு -ENOMEM;

        s.next_in = (எழுத்து*) src;
        s.avail_in = src_size;
        s.next_out = (எழுத்து*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        எனில் (r != BZ_STREAM_END)
                திரும்பு -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

முழு compress_blob_journal(
                Compression compression,
                மாறிலி வெறுமை *src, uint64_t src_size,
                வெறுமை *dst, size_t dst_alloc_size, size_t *dst_size, முழு level) {

        தேர்வு (compression) {
        நிலை COMPRESSION_XZ:
                திரும்பு compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        நிலை COMPRESSION_LZ4:
                திரும்பு compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        நிலை COMPRESSION_ZSTD:
                திரும்பு compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        இயல்பு:
                திரும்பு -EOPNOTSUPP;
        }
}

முழு compress_blob(
                Compression compression,
                மாறிலி வெறுமை *src, uint64_t src_size,
                வெறுமை *dst, size_t dst_alloc_size, size_t *dst_size, முழு level) {

        தேர்வு (compression) {
        நிலை COMPRESSION_GZIP:
                திரும்பு compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        நிலை COMPRESSION_BZIP2:
                திரும்பு compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        இயல்பு:
                திரும்பு compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

நிலையான முழு decompress_blob_xz(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **dst,
                size_t *dst_size,
                size_t dst_max) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_XZ
        முழு r;

        r = dlopen_xz(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        எனில் (ret != LZMA_OK)
                திரும்பு -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        எனில் (!greedy_realloc(dst, space, 1))
                திரும்பு -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        க்கு (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                எனில் (ret == LZMA_STREAM_END)
                        நிறுத்து;
                எனில் (ret != LZMA_OK)
                        திரும்பு -ENOMEM;

                எனில் (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        நிறுத்து;
                எனில் (dst_max > 0 && space == dst_max)
                        திரும்பு -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                ப்ரதிஜ்ஞா(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                எனில் (!greedy_realloc(dst, space, 1))
                        திரும்பு -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு decompress_blob_lz4(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **dst,
                size_t *dst_size,
                size_t dst_max) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_LZ4
        எழுத்து* out;
        முழு r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size <= 8)
                திரும்பு -EBADMSG;

        எனில் (src_size - 8 > INT_MAX)
                திரும்பு -EFBIG;

        size = unaligned_read_le64(src);
        எனில் (size < 0 || (unsigned) size != unaligned_read_le64(src))
                திரும்பு -EFBIG;
        எனில் (dst_max > 0 && (size_t) size > dst_max)
                திரும்பு -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        எனில் (!out)
                திரும்பு -ENOMEM;

        r = sym_LZ4_decompress_safe((எழுத்து*)src + 8, out, src_size - 8, size);
        எனில் (r < 0 || r != size)
                திரும்பு -EBADMSG;

        *dst_size = size;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(வெறுமை) {
#எனில் HAVE_ZSTD
        எனில் (dlopen_zstd(LOG_DEBUG) < 0)
                திரும்பு 0;

        திரும்பு sym_ZSTD_DStreamOutSize();
#இல்லையெனில்
        திரும்பு 0;
#endif
}

நிலையான முழு decompress_blob_zstd(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **dst,
                size_t *dst_size,
                size_t dst_max) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_ZSTD
        முழு r;

        r = dlopen_zstd(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        எனில் (size == ZSTD_CONTENTSIZE_ERROR)
                திரும்பு -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        எனில் (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        இல்லையெனில் {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        எனில் (!greedy_realloc(dst, space, 1))
                திரும்பு -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        எனில் (!dctx)
                திரும்பு -ENOMEM;

        ZSTD_inBuffer உள்ளீடு = {
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

        க்கு (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &உள்ளீடு);
                எனில் (sym_ZSTD_isError(k))
                        திரும்பு log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                எனில் (output.pos >= cap)
                        நிறுத்து;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                எனில் (k == 0 && உள்ளீடு.pos >= உள்ளீடு.size)
                        நிறுத்து;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                எனில் (output.pos == output.size) {
                        எனில் (space > SIZE_MAX / 2)
                                திரும்பு -E2BIG;
                        space = MIN(2 * space, cap);
                        எனில் (!greedy_realloc(dst, space, 1))
                                திரும்பு -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        தொடர்;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                எனில் (உள்ளீடு.pos >= உள்ளீடு.size)
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு decompress_blob_gzip(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **dst,
                size_t *dst_size,
                size_t dst_max) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_ZLIB
        முழு r;

        r = dlopen_zlib(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size > UINT_MAX)
                திரும்பு -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (முழு) அளவு(s));
        எனில் (r != Z_OK)
                திரும்பு -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        எனில் (!greedy_realloc(dst, space, 1))
                திரும்பு -ENOMEM;

        s.next_in = (வெறுமை*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        க்கு (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                எனில் (r == Z_STREAM_END)
                        நிறுத்து;
                எனில் (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        திரும்பு -EBADMSG;

                எனில் (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        நிறுத்து;
                எனில் (dst_max > 0 && space == dst_max)
                        திரும்பு -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                எனில் (!greedy_realloc(dst, space, 1))
                        திரும்பு -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு decompress_blob_bzip2(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **dst,
                size_t *dst_size,
                size_t dst_max) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(dst);
        ப்ரதிஜ்ஞா(dst_size);

#எனில் HAVE_BZIP2
        முழு r;

        r = dlopen_bzip2(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size > UINT_MAX)
                திரும்பு -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        எனில் (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                திரும்பு -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        எனில் (!greedy_realloc(dst, space, 1))
                திரும்பு -ENOMEM;

        s.next_in = (எழுத்து*) src;
        s.avail_in = src_size;
        s.next_out = (எழுத்து*) *dst;
        s.avail_out = space;

        க்கு (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                எனில் (r == BZ_STREAM_END)
                        நிறுத்து;
                எனில் (r != BZ_OK)
                        திரும்பு -EBADMSG;

                எனில் (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        நிறுத்து;
                எனில் (dst_max > 0 && space == dst_max)
                        திரும்பு -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                எனில் (!greedy_realloc(dst, space, 1))
                        திரும்பு -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (எழுத்து*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

முழு decompress_blob_journal(
                Compression compression,
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **dst,
                size_t *dst_size,
                size_t dst_max) {

        தேர்வு (compression) {
        நிலை COMPRESSION_XZ:
                திரும்பு decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        நிலை COMPRESSION_LZ4:
                திரும்பு decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        நிலை COMPRESSION_ZSTD:
                திரும்பு decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        இயல்பு:
                திரும்பு -EPROTONOSUPPORT;
        }
}

முழு decompress_blob(
                Compression compression,
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **dst,
                size_t *dst_size,
                size_t dst_max) {

        தேர்வு (compression) {
        நிலை COMPRESSION_GZIP:
                திரும்பு decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        நிலை COMPRESSION_BZIP2:
                திரும்பு decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        இயல்பு:
                திரும்பு decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

முழு decompress_zlib_raw(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை *dst,
                size_t dst_size,
                முழு wbits) {

#எனில் HAVE_ZLIB
        முழு r;

        r = dlopen_zlib(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size > UINT_MAX)
                திரும்பு -EFBIG;
        எனில் (dst_size > UINT_MAX)
                திரும்பு -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (வெறுமை*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (முழு) அளவு(s));
        எனில் (r != Z_OK)
                திரும்பு -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        எனில் (r != Z_STREAM_END || produced != dst_size)
                திரும்பு -EBADMSG;

        திரும்பு 0;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு decompress_startswith_xz(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **buffer,
                மாறிலி வெறுமை *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(prefix);

#எனில் HAVE_XZ
        முழு r;

        r = dlopen_xz(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        எனில் (ret != LZMA_OK)
                திரும்பு -EBADMSG;

        எனில் (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                திரும்பு -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        க்கு (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                எனில் (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        திரும்பு -EBADMSG;

                எனில் (allocated - s.avail_out >= prefix_len + 1)
                        திரும்பு memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((மாறிலி uint8_t*) *buffer)[prefix_len] == extra;

                எனில் (ret == LZMA_STREAM_END)
                        திரும்பு 0;

                s.avail_out += allocated;

                எனில் (!(greedy_realloc(buffer, allocated * 2, 1)))
                        திரும்பு -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு decompress_startswith_lz4(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **buffer,
                மாறிலி வெறுமை *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(prefix);

#எனில் HAVE_LZ4
        size_t allocated;
        முழு r;

        r = dlopen_lz4(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size <= 8)
                திரும்பு -EBADMSG;

        எனில் (src_size - 8 > INT_MAX)
                திரும்பு -EFBIG;

        எனில் (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                திரும்பு -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (எழுத்து*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        எனில் (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                எனில் (sym_LZ4_version_எண்() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        திரும்பு 0;

                எனில் (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        எனில் (memcmp(*buffer, prefix, r) != 0)
                                திரும்பு 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (size < prefix_len + 1)
                        திரும்பு 0;
        }

        திரும்பு memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((மாறிலி uint8_t*) *buffer)[prefix_len] == extra;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு decompress_startswith_zstd(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **buffer,
                மாறிலி வெறுமை *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(prefix);

#எனில் HAVE_ZSTD
        முழு r;

        r = dlopen_zstd(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        எனில் (size == ZSTD_CONTENTSIZE_ERROR)
                திரும்பு -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        எனில் (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                திரும்பு 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        எனில் (!dctx)
                திரும்பு -ENOMEM;

        எனில் (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                திரும்பு -ENOMEM;

        ZSTD_inBuffer உள்ளீடு = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &உள்ளீடு);
        எனில் (sym_ZSTD_isError(k))
                திரும்பு log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        எனில் (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                எனில் (k != 0)
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                திரும்பு 0; /* Decompressed text too short to match the prefix and extra */
        }

        திரும்பு memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((மாறிலி uint8_t*) *buffer)[prefix_len] == extra;
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு decompress_startswith_gzip(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **buffer,
                மாறிலி வெறுமை *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(prefix);

#எனில் HAVE_ZLIB
        முழு r;

        r = dlopen_zlib(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size > UINT_MAX)
                திரும்பு -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (முழு) அளவு(s));
        எனில் (r != Z_OK)
                திரும்பு -EBADMSG;

        எனில் (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                திரும்பு -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (வெறுமை*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        க்கு (;;) {
                r = sym_inflate(&s, Z_FINISH);

                எனில் (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        திரும்பு -EBADMSG;

                எனில் (allocated - s.avail_out >= prefix_len + 1)
                        திரும்பு memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((மாறிலி uint8_t*) *buffer)[prefix_len] == extra;

                எனில் (r == Z_STREAM_END)
                        திரும்பு 0;

                எனில் (s.avail_out > 0)
                        திரும்பு -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                எனில் (!(greedy_realloc(buffer, allocated * 2, 1)))
                        திரும்பு -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

நிலையான முழு decompress_startswith_bzip2(
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                வெறுமை **buffer,
                மாறிலி வெறுமை *prefix,
                size_t prefix_len,
                uint8_t extra) {

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(src_size > 0);
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(prefix);

#எனில் HAVE_BZIP2
        முழு r;

        r = dlopen_bzip2(LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (src_size > UINT_MAX)
                திரும்பு -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        எனில் (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                திரும்பு -EIO;

        எனில் (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                திரும்பு -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (எழுத்து*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        க்கு (;;) {
                r = sym_BZ2_bzDecompress(&s);

                எனில் (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        திரும்பு -EBADMSG;

                எனில் (allocated - s.avail_out >= prefix_len + 1)
                        திரும்பு memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((மாறிலி uint8_t*) *buffer)[prefix_len] == extra;

                எனில் (r == BZ_STREAM_END)
                        திரும்பு 0;

                எனில் (s.avail_out > 0)
                        திரும்பு -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                எனில் (!(greedy_realloc(buffer, allocated * 2, 1)))
                        திரும்பு -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (எழுத்து*) *buffer + used;
        }
#இல்லையெனில்
        திரும்பு -EPROTONOSUPPORT;
#endif
}

முழு decompress_startswith_journal(
                Compression compression,
                மாறிலி வெறுமை *src, uint64_t src_size,
                வெறுமை **buffer,
                மாறிலி வெறுமை *prefix, size_t prefix_len,
                uint8_t extra) {

        தேர்வு (compression) {
        நிலை COMPRESSION_XZ:
                திரும்பு decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        நிலை COMPRESSION_LZ4:
                திரும்பு decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        நிலை COMPRESSION_ZSTD:
                திரும்பு decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        இயல்பு:
                திரும்பு -EOPNOTSUPP;
        }
}

முழு decompress_startswith(
                Compression compression,
                மாறிலி வெறுமை *src, uint64_t src_size,
                வெறுமை **buffer,
                மாறிலி வெறுமை *prefix, size_t prefix_len,
                uint8_t extra) {

        தேர்வு (compression) {
        நிலை COMPRESSION_GZIP:
                திரும்பு decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        நிலை COMPRESSION_BZIP2:
                திரும்பு decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        இயல்பு:
                திரும்பு decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

முழு compress_stream(
                Compression type,
                முழு fdf, முழு fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = வெற்று;
        _cleanup_free_ வெறுமை *buf = வெற்று;
        _cleanup_free_ uint8_t *உள்ளீடு = வெற்று;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        முழு r;

        ப்ரதிஜ்ஞா(fdf >= 0);
        ப்ரதிஜ்ஞா(fdt >= 0);

        r = compressor_new(&c, type);
        எனில் (r < 0)
                திரும்பு r;

        உள்ளீடு = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        எனில் (!உள்ளீடு)
                திரும்பு -ENOMEM;

        க்கு (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                எனில் (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = பட(fdf, உள்ளீடு, m);
                எனில் (n < 0)
                        திரும்பு -errno;

                எனில் (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        எனில் (r < 0)
                                திரும்பு r;

                        எனில் (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                எனில் (r < 0)
                                        திரும்பு r;
                                total_out += buf_size;
                        }
                        நிறுத்து;
                }

                total_in += n;
                எனில் (max_bytes != UINT64_MAX) {
                        ப்ரதிஜ்ஞா(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, உள்ளீடு, n, &buf, &buf_size, &buf_alloc);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        எனில் (r < 0)
                                திரும்பு r;
                        total_out += buf_size;
                }
        }

        எனில் (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        எனில் (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        இல்லையெனில்
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (இரட்டை) total_out / total_in * 100);

        திரும்பு 0;
}

நிலையான முழு should_sparse(முழு fd) {
        கட்டமைப்பு stat st;

        ப்ரதிஜ்ஞா(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        எனில் (!S_ISREG(st.st_mode))
                திரும்பு false;

        முழு flags = fcntl(fd, F_GETFL);
        எனில் (flags < 0)
                திரும்பு -errno;

        எனில் (FLAGS_SET(flags, O_APPEND))
                திரும்பு false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        எனில் (pos < 0)
                திரும்பு -errno;

        திரும்பு pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
நிலையான முழு finalize_sparse(முழு fd) {
        off_t pos;

        ப்ரதிஜ்ஞா(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        எனில் (pos < 0)
                திரும்பு -errno;

        எனில் (ftruncate(fd, pos) < 0)
                திரும்பு -errno;

        திரும்பு 0;
}

/* Common helper for decompress_stream_*() wrappers */

கட்டமைப்பு decompress_stream_userdata {
        முழு fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

நிலையான முழு decompress_stream_write_callback(மாறிலி வெறுமை *data, size_t size, வெறுமை *userdata) {
        கட்டமைப்பு decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        எனில் (u->max_bytes != UINT64_MAX) {
                எனில் (u->max_bytes < size)
                        திரும்பு -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        எனில் (u->skip > 0) {
                எனில் (u->skip >= size) {
                        u->skip -= size;
                        திரும்பு 0;
                }

                data = (மாறிலி uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        எனில் (size == 0)
                திரும்பு 0;

        எனில் (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                எனில் (k < 0)
                        திரும்பு (முழு) k;
                திரும்பு 0;
        }

        திரும்பு loop_write(u->fd, data, size);
}

நிலையான முழு decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        ப்ரதிஜ்ஞா(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        எனில் (!c)
                திரும்பு -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        தேர்வு (type) {
        நிலை COMPRESSION_NONE:
                நிறுத்து;

#எனில் HAVE_XZ
        நிலை COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                எனில் (xz_ret != LZMA_OK)
                        திரும்பு -EIO;
                நிறுத்து;
        }
#endif

#எனில் HAVE_LZ4
        நிலை COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                எனில் (sym_LZ4F_isError(rc))
                        திரும்பு -ENOMEM;

                நிறுத்து;
        }
#endif

#எனில் HAVE_ZSTD
        நிலை COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                எனில் (!c->d_zstd)
                        திரும்பு -ENOMEM;
                நிறுத்து;
#endif

#எனில் HAVE_ZLIB
        நிலை COMPRESSION_GZIP:
                எனில் (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (முழு) அளவு(c->gzip)) != Z_OK)
                        திரும்பு -EIO;
                நிறுத்து;
#endif

#எனில் HAVE_BZIP2
        நிலை COMPRESSION_BZIP2:
                எனில் (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        திரும்பு -EIO;
                நிறுத்து;
#endif

        இயல்பு:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        திரும்பு 0;
}

நிலையான முழு decompress_blob_to_fd_impl(
                Compression type,
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                முழு fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        முழு r;

        ப்ரதிஜ்ஞா(src);
        ப்ரதிஜ்ஞா(fdt >= 0);

        எனில் (type == COMPRESSION_NONE)
                திரும்பு -EOPNOTSUPP;

        எனில் (src_size == 0)
                திரும்பு -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        எனில் (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                எனில் (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        எனில் (src_size <= அளவு(uint64_t))
                                திரும்பு -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        எனில் (sz < skip)
                                திரும்பு -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                எனில் (max_bytes == 0)
                        திரும்பு -ENOBUFS;

                _cleanup_free_ வெறுமை *buf = வெற்று;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                எனில் (r < 0)
                        திரும்பு r;

                /* Check the result size again for safety. */
                எனில் (size < skip)
                        திரும்பு -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                எனில் (r < 0)
                        திரும்பு r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (இரட்டை) size / src_size * 100);

                எனில் (ret_size)
                        *ret_size = size - skip;
                திரும்பு 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = வெற்று;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        எனில் (r < 0)
                திரும்பு r;

        கட்டமைப்பு decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_தள்ளு(c, src, src_size, decompress_stream_write_callback, &userdata);
        எனில் (r < 0)
                திரும்பு r;

        /* Finalize */
        r = decompressor_தள்ளு(c, /* data= */ வெற்று, /* size= */ 0, decompress_stream_write_callback, &userdata);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (userdata.total_out < skip)
                திரும்பு -EBADMSG;

        எனில் (userdata.sparse) {
                r = finalize_sparse(fdt);
                எனில் (r < 0)
                        திரும்பு r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (இரட்டை) userdata.total_out / src_size * 100);

        எனில் (ret_size)
                *ret_size = userdata.total_out - skip;

        திரும்பு 0;
}

முழு decompress_blob_to_fd_journal(
                Compression type,
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                முழு fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        முழு r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

முழு decompress_blob_to_fd(
                Compression type,
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                முழு fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        முழு r;

        r = dlopen_compress(type, LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

முழு decompress_stream_full(
                Compression type,
                முழு fdf, முழு fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = வெற்று;
        _cleanup_free_ uint8_t *buf = வெற்று;
        uint64_t total_in = 0;
        முழு r;

        ப்ரதிஜ்ஞா(fdf >= 0);
        ப்ரதிஜ்ஞா(fdt >= 0);

        எனில் (type == COMPRESSION_NONE)
                திரும்பு -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        r = decompressor_new(&c, type, flags);
        எனில் (r < 0)
                திரும்பு r;

        கட்டமைப்பு decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        எனில் (!buf)
                திரும்பு -ENOMEM;

        க்கு (;;) {
                ssize_t n;

                n = பட(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                எனில் (n < 0)
                        திரும்பு -errno;

                total_in += n;

                r = decompressor_தள்ளு(c, buf, n, decompress_stream_write_callback, &userdata);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (n == 0)
                        நிறுத்து;
        }

        எனில் (total_in == 0)
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        எனில் (userdata.sparse) {
                r = finalize_sparse(fdt);
                எனில் (r < 0)
                        திரும்பு r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (இரட்டை) userdata.total_out / total_in * 100);

        திரும்பு 0;
}

முழு decompress_stream_by_filename(மாறிலி எழுத்து *filename, முழு fdf, முழு fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        எனில் (c == COMPRESSION_NONE)
                திரும்பு -EPROTONOSUPPORT;

        திரும்பு decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        எனில் (!c)
                திரும்பு வெற்று;

        தேர்வு (c->type) {

#எனில் HAVE_XZ
        நிலை COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                நிறுத்து;
#endif

#எனில் HAVE_LZ4
        நிலை COMPRESSION_LZ4:
                எனில் (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = வெற்று;
                        c->lz4_header = mfree(c->lz4_header);
                } இல்லையெனில் {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = வெற்று;
                }
                நிறுத்து;
#endif

#எனில் HAVE_ZSTD
        நிலை COMPRESSION_ZSTD:
                எனில் (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = வெற்று;
                } இல்லையெனில் {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = வெற்று;
                }
                நிறுத்து;
#endif

#எனில் HAVE_ZLIB
        நிலை COMPRESSION_GZIP:
                எனில் (c->encoding)
                        sym_deflateEnd(&c->gzip);
                இல்லையெனில்
                        sym_inflateEnd(&c->gzip);
                நிறுத்து;
#endif

#எனில் HAVE_BZIP2
        நிலை COMPRESSION_BZIP2:
                எனில் (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                இல்லையெனில்
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                நிறுத்து;
#endif

        இயல்பு:
                நிறுத்து;
        }

        திரும்பு mfree(c);
}

Compression compressor_type(மாறிலி Compressor *c) {
        திரும்பு c ? c->type : _COMPRESSION_INVALID;
}

முழு decompressor_detect(Decompressor **decompressor, மாறிலி வெறுமை *data, size_t size) {
        முழு r;

        ப்ரதிஜ்ஞா(decompressor);

        எனில் (*decompressor)
                திரும்பு 1;

        எனில் (size < COMPRESSION_MAGIC_BYTES_MAX)
                திரும்பு 0;

        ப்ரதிஜ்ஞா(data);

        Compression type = compression_detect_from_magic(data);
        எனில் (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு 1;
}

முழு decompressor_force(Decompressor **decompressor, Compression type) {
        முழு r;

        ப்ரதிஜ்ஞா(decompressor);
        ப்ரதிஜ்ஞா(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        *decompressor = compressor_free(*decompressor);
        திரும்பு decompressor_new(decompressor, type, /* flags= */ 0);
}

முழு decompressor_force_off(Decompressor **decompressor) {
        திரும்பு decompressor_force(decompressor, COMPRESSION_NONE);
}

முழு decompressor_தள்ளு(Decompressor *c, மாறிலி வெறுமை *data, size_t size, DecompressorCallback callback, வெறுமை *userdata) {
        முழு r;

        ப்ரதிஜ்ஞா(c);
        ப்ரதிஜ்ஞா(data || size == 0);
        ப்ரதிஜ்ஞா(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        எனில் (c->encoding)
                திரும்பு -EINVAL;

        எனில் (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                திரும்பு 0;

#எனில் HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = வெற்று;
        எனில் (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                எனில் (!buffer)
                        திரும்பு -ENOMEM;
        }
#endif

        தேர்வு (c->type) {

        நிலை COMPRESSION_NONE:
                எனில் (c->finished)
                        திரும்பு -EBADMSG;

                எனில் (size == 0) {
                        c->finished = true;
                        திரும்பு 0;
                }

                r = callback(data, size, userdata);
                எனில் (r < 0)
                        திரும்பு r;

                திரும்பு 0;

#எனில் HAVE_XZ
        நிலை COMPRESSION_XZ:
                எனில் (c->finished)
                        திரும்பு -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                க்கு (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        எனில் (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                திரும்பு -EBADMSG;

                        எனில் (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                திரும்பு -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        எனில் (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }

                        எனில் (lzr == LZMA_STREAM_END) {
                                எனில் (c->xz.avail_in > 0)
                                        திரும்பு -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                திரும்பு 0;
                        }

                        எனில் (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                எனில் (size == 0)
                                        திரும்பு -EBADMSG; /* truncated */

                                திரும்பு 0; /* No progress possible with the current input. */
                        }
                }
#endif

#எனில் HAVE_LZ4
        நிலை COMPRESSION_LZ4: {
                மாறிலி uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                க்கு (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, வெற்று);
                        எனில் (sym_LZ4F_isError(rc))
                                திரும்பு -EBADMSG;

                        எனில் (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                திரும்பு -EIO;

                        எனில் (consumed > 0) {
                                ப்ரதிஜ்ஞா(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        எனில் (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }

                        எனில் (src_remaining == 0) {
                                எனில் (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        திரும்பு 0;
                                }

                                எனில் (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        தொடர்;

                                எனில் (size == 0)
                                        திரும்பு -EBADMSG; /* truncated */

                                திரும்பு 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#எனில் HAVE_ZSTD
        நிலை COMPRESSION_ZSTD: {
                ZSTD_inBuffer உள்ளீடு = {
                        .src =  (வெறுமை*) data,
                        .size = size,
                };

                c->finished = false;

                க்கு (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &உள்ளீடு);
                        எனில் (sym_ZSTD_isError(res))
                                திரும்பு -EBADMSG;

                        எனில் (உள்ளீடு.pos > உள்ளீடு.size ||
                            output.pos > output.size)
                                திரும்பு -EIO;

                        எனில் (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }

                        எனில் (உள்ளீடு.pos == உள்ளீடு.size) {
                                எனில் (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        திரும்பு 0;
                                }

                                எனில் (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        தொடர்;

                                எனில் (size == 0)
                                        திரும்பு -EBADMSG; /* truncated */

                                திரும்பு 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#எனில் HAVE_ZLIB
        நிலை COMPRESSION_GZIP:
                எனில் (size > UINT_MAX)
                        திரும்பு -EFBIG;

                c->gzip.next_in = (வெறுமை*) data;
                c->gzip.avail_in = size;

                க்கு (;;) {
                        எனில் (c->finished) {
                                எனில் (sym_inflateReset(&c->gzip) != Z_OK)
                                        திரும்பு -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        முழு zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        எனில் (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                திரும்பு -EBADMSG;

                        எனில் (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                திரும்பு -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        எனில் (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }

                        எனில் (zr == Z_STREAM_END) {
                                c->finished = true;

                                எனில் (c->gzip.avail_in > 0)
                                        தொடர்; /* New stream may be pending. */

                                திரும்பு 0;
                        }

                        எனில் (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                எனில் (size == 0)
                                        திரும்பு -EBADMSG; /* truncated */

                                திரும்பு 0; /* No progress possible with the current input. */
                        }
                }
#endif

#எனில் HAVE_BZIP2
        நிலை COMPRESSION_BZIP2:
                எனில் (size > UINT_MAX)
                        திரும்பு -EFBIG;

                c->bzip2.next_in = (எழுத்து*) data;
                c->bzip2.avail_in = size;

                க்கு (;;) {
                        எனில் (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                எனில் (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        திரும்பு -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (எழுத்து*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        முழு bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        எனில் (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                திரும்பு -EBADMSG;

                        எனில் (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                திரும்பு -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        எனில் (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }

                        எனில் (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                எனில் (c->bzip2.avail_in > 0)
                                        தொடர்; /* New stream may be pending. */

                                திரும்பு 0;
                        }

                        எனில் (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                எனில் (size == 0)
                                        திரும்பு -EBADMSG; /* truncated */

                                திரும்பு 0; /* No progress possible with the current input. */
                        }
                }
#endif

        இயல்பு:
                assert_not_reached();
        }
}

முழு compressor_new(Compressor **ret, Compression type) {
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        எனில் (r < 0)
                திரும்பு r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        எனில் (!c)
                திரும்பு -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        தேர்வு (type) {

#எனில் HAVE_XZ
        நிலை COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                எனில் (xzr != LZMA_OK)
                        திரும்பு -EIO;

                c->type = COMPRESSION_XZ;
                நிறுத்து;
        }
#endif

#எனில் HAVE_LZ4
        நிலை COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                எனில் (sym_LZ4F_isError(rc))
                        திரும்பு -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                எனில் (!c->lz4_header)
                        திரும்பு -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                எனில் (sym_LZ4F_isError(c->lz4_header_size))
                        திரும்பு -EINVAL;

                நிறுத்து;
        }
#endif

#எனில் HAVE_ZSTD
        நிலை COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                எனில் (!c->c_zstd)
                        திரும்பு -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                எனில் (sym_ZSTD_isError(z))
                        திரும்பு -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                எனில் (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                நிறுத்து;
#endif

#எனில் HAVE_ZLIB
        நிலை COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (முழு) அளவு(c->gzip));
                எனில் (r != Z_OK)
                        திரும்பு -EIO;

                c->type = COMPRESSION_GZIP;
                நிறுத்து;
#endif

#எனில் HAVE_BZIP2
        நிலை COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                எனில் (r != BZ_OK)
                        திரும்பு -EIO;

                c->type = COMPRESSION_BZIP2;
                நிறுத்து;
#endif

        நிலை COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                நிறுத்து;

        இயல்பு:
                திரும்பு -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        திரும்பு 0;
}

#எனில் HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
நிலையான முழு enlarge_buffer(வெறுமை **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(buffer_size);
        ப்ரதிஜ்ஞா(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        எனில் (*buffer_allocated >= need)
                திரும்பு 0;

        எனில் (!greedy_realloc(buffer, need, 1))
                திரும்பு -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        திரும்பு 1;
}
#endif

முழு compressor_start(
                Compressor *c,
                மாறிலி வெறுமை *data,
                size_t size,
                வெறுமை **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#எனில் HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        முழு r;
#endif

        ப்ரதிஜ்ஞா(c);
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(buffer_size);
        ப்ரதிஜ்ஞா(buffer_allocated);

        எனில் (!c->encoding)
                திரும்பு -EINVAL;

        எனில் (size == 0)
                திரும்பு 0;

        ப்ரதிஜ்ஞா(data);

        *buffer_size = 0;

        தேர்வு (c->type) {

#எனில் HAVE_XZ
        நிலை COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                போது (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        எனில் (r < 0)
                                திரும்பு r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        எனில் (lzr != LZMA_OK)
                                திரும்பு -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                நிறுத்து;
#endif

#எனில் HAVE_LZ4
        நிலை COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                எனில் (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        எனில் (r < 0)
                                திரும்பு r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                எனில் (r < 0)
                        திரும்பு r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, வெற்று);
                எனில் (sym_LZ4F_isError(n))
                        திரும்பு -EIO;

                *buffer_size += n;
                நிறுத்து;
        }
#endif

#எனில் HAVE_ZSTD
        நிலை COMPRESSION_ZSTD: {
                ZSTD_inBuffer உள்ளீடு = {
                        .src = data,
                        .size = size,
                };

                போது (உள்ளீடு.pos < உள்ளீடு.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        எனில் (r < 0)
                                திரும்பு r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &உள்ளீடு, ZSTD_e_continue);
                        எனில் (sym_ZSTD_isError(res))
                                திரும்பு -EIO;

                        *buffer_size += output.pos;
                }

                நிறுத்து;
        }
#endif

#எனில் HAVE_ZLIB
        நிலை COMPRESSION_GZIP:
                எனில் (size > UINT_MAX)
                        திரும்பு -EFBIG;

                c->gzip.next_in = (வெறுமை*) data;
                c->gzip.avail_in = size;

                போது (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        எனில் (r < 0)
                                திரும்பு r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        எனில் (r != Z_OK)
                                திரும்பு -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                நிறுத்து;
#endif

#எனில் HAVE_BZIP2
        நிலை COMPRESSION_BZIP2:
                எனில் (size > UINT_MAX)
                        திரும்பு -EFBIG;

                c->bzip2.next_in = (வெறுமை*) data;
                c->bzip2.avail_in = size;

                போது (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        எனில் (r < 0)
                                திரும்பு r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (வெறுமை*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        எனில் (r != BZ_RUN_OK)
                                திரும்பு -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                நிறுத்து;
#endif

        நிலை COMPRESSION_NONE:

                எனில் (*buffer_allocated < size) {
                        வெறுமை *p;

                        p = realloc(*buffer, size);
                        எனில் (!p)
                                திரும்பு -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                நிறுத்து;

        இயல்பு:
                திரும்பு -EOPNOTSUPP;
        }

        திரும்பு 0;
}

முழு compressor_finish(Compressor *c, வெறுமை **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#எனில் HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        முழு r;
#endif

        ப்ரதிஜ்ஞா(c);
        ப்ரதிஜ்ஞா(buffer);
        ப்ரதிஜ்ஞா(buffer_size);
        ப்ரதிஜ்ஞா(buffer_allocated);

        எனில் (!c->encoding)
                திரும்பு -EINVAL;

        *buffer_size = 0;

        தேர்வு (c->type) {

#எனில் HAVE_XZ
        நிலை COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                செய் {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        எனில் (r < 0)
                                திரும்பு r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        எனில் (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                திரும்பு -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } போது (lzr != LZMA_STREAM_END);

                நிறுத்து;
        }
#endif

#எனில் HAVE_LZ4
        நிலை COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                எனில் (r < 0)
                        திரும்பு r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, வெற்று);
                எனில் (sym_LZ4F_isError(n))
                        திரும்பு -EIO;

                *buffer_size = n;
                நிறுத்து;
        }
#endif

#எனில் HAVE_ZSTD
        நிலை COMPRESSION_ZSTD: {
                ZSTD_inBuffer உள்ளீடு = {};
                size_t res;

                செய் {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        எனில் (r < 0)
                                திரும்பு r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &உள்ளீடு, ZSTD_e_end);
                        எனில் (sym_ZSTD_isError(res))
                                திரும்பு -EIO;

                        *buffer_size += output.pos;
                } போது (res != 0);

                நிறுத்து;
        }
#endif

#எனில் HAVE_ZLIB
        நிலை COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                செய் {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        எனில் (r < 0)
                                திரும்பு r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        எனில் (!IN_SET(r, Z_OK, Z_STREAM_END))
                                திரும்பு -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } போது (r != Z_STREAM_END);

                நிறுத்து;
#endif

#எனில் HAVE_BZIP2
        நிலை COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                செய் {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        எனில் (r < 0)
                                திரும்பு r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (வெறுமை*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        எனில் (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                திரும்பு -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } போது (r != BZ_STREAM_END);

                நிறுத்து;
#endif

        நிலை COMPRESSION_NONE:
                நிறுத்து;

        இயல்பு:
                திரும்பு -EOPNOTSUPP;
        }

        திரும்பு 0;
}
