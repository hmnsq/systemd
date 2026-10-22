/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#જો HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#અન્તર્ભાવ <lzma.h>
#endif

#જો HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#અન્તર્ભાવ <lz4.h>
#અન્તર્ભાવ <lz4frame.h>
#અન્તર્ભાવ <lz4hc.h>
#endif

#જો HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#અન્તર્ભાવ <zstd.h>
#અન્તર્ભાવ <zstd_errors.h>
#endif

#જો HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#અન્તર્ભાવ <zlib.h>
#endif

#જો HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#અન્તર્ભાવ <bzlib.h>
#endif

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "bitfield.h"
#અન્તર્ભાવ "compress.h"
#અન્તર્ભાવ "dlfcn-util.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "unaligned.h"

#જો HAVE_XZ
સ્થિર DLSYM_PROTOTYPE(lzma_auto_decoder) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(lzma_code) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(lzma_easy_encoder) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(lzma_end) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(lzma_stream_decoder) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(lzma_lzma_preset) = ખાલી;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
સ્થિર અંતઃરેખ શૂન્ય lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#જો HAVE_LZ4
સ્થિર DLSYM_PROTOTYPE(LZ4F_compressBegin) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_compressBound) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_compressEnd) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_compressUpdate) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_decompress) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4F_isError) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4_compress_HC) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4_compress_default) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4_decompress_safe) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(LZ4_version_સંખ્યા) = ખાલી;

સ્થિર અચલ LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#જો HAVE_ZSTD
સ્થિર DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_compress) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_compressStream2) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_createCCtx) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_createDCtx) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_decompressStream) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_freeCCtx) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_freeDCtx) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_getErrorCode) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_getErrorName) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(ZSTD_isError) = ખાલી;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, ખાલી);

સ્થિર પૂર્ણાંક zstd_ret_to_errno(size_t ret) {
        પસંદગી (sym_ZSTD_getErrorCode(ret)) {
        સ્થિતિ ZSTD_error_dstSize_tooSmall:
                પાછા -ENOBUFS;
        સ્થિતિ ZSTD_error_memory_allocation:
                પાછા -ENOMEM;
        મૂળભૂત:
                પાછા -EBADMSG;
        }
}
#endif

#જો HAVE_ZLIB
સ્થિર DLSYM_PROTOTYPE(deflateInit2_) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(deflate) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(deflateEnd) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(inflateInit2_) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(inflate) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(inflateEnd) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(inflateReset) = ખાલી;

સ્થિર અંતઃરેખ શૂન્ય deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

સ્થિર અંતઃરેખ શૂન્ય inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#જો HAVE_BZIP2
સ્થિર DLSYM_PROTOTYPE(BZ2_bzCompressInit) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(BZ2_bzCompress) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(BZ2_bzDecompress) = ખાલી;
સ્થિર DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = ખાલી;

સ્થિર અંતઃરેખ શૂન્ય BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

સ્થિર અંતઃરેખ શૂન્ય BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
સંરચના Compressor {
        Compression type;
        bool encoding;
        bool finished;
        સંઘ {
#જો HAVE_XZ
                lzma_stream xz;
#endif
#જો HAVE_LZ4
                સંરચના {
                        LZ4F_compressionContext_t c_lz4;
                        શૂન્ય *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#જો HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#જો HAVE_ZLIB
                z_stream gzip;
#endif
#જો HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, કદ(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

સ્થિર અચલ અક્ષર* અચલ compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

સ્થિર અચલ અક્ષર* અચલ compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

સ્થિર અચલ અક્ષર* અચલ compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(અચલ અક્ષર *s) {
        Compression c;

        પ્રતિજ્ઞા(s);

        c = compression_from_string(s);
        જો (c >= 0)
                પાછા c;

        પાછા compression_uppercase_from_string(s);
}

Compression compression_from_filename(અચલ અક્ષર *filename) {
        Compression c;
        અચલ અક્ષર *e;

        પ્રતિજ્ઞા(filename);

        e = strrchr(filename, '.');
        જો (!e)
                પાછા COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        જો (c < 0)
                પાછા COMPRESSION_NONE;

        પાછા c;
}

bool compression_supported_journal(Compression c) {
        સ્થિર અચલ unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        પ્રતિજ્ઞા(c >= 0);
        પ્રતિજ્ઞા(c < _COMPRESSION_MAX);

        પાછા BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        સ્થિર અચલ unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        પ્રતિજ્ઞા(c >= 0);
        પ્રતિજ્ઞા(c < _COMPRESSION_MAX);

        પાછા BIT_SET(supported, c);
}

Compression compression_detect_from_magic(અચલ uint8_t data[સ્થિર COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        જો (memcmp(data, (અચલ uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                પાછા COMPRESSION_GZIP;
        જો (memcmp(data, (અચલ uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                પાછા COMPRESSION_XZ;
        જો (memcmp(data, (અચલ uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                પાછા COMPRESSION_ZSTD;
        જો (memcmp(data, (અચલ uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                પાછા COMPRESSION_LZ4;
        જો (memcmp(data, (અચલ uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                પાછા COMPRESSION_BZIP2;

        પાછા _COMPRESSION_INVALID;
}

પૂર્ણાંક dlopen_xz(પૂર્ણાંક log_level) {
#જો HAVE_XZ
        સ્થિર શૂન્ય *lzma_dl = ખાલી;

        LIBLZMA_NOTE(suggested);

        પાછા dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#નહીંતર
        પાછા log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

પૂર્ણાંક dlopen_lz4(પૂર્ણાંક log_level) {
#જો HAVE_LZ4
        સ્થિર શૂન્ય *lz4_dl = ખાલી;

        LIBLZ4_NOTE(suggested);

        પાછા dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_સંખ્યા));
#નહીંતર
        પાછા log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

પૂર્ણાંક dlopen_zstd(પૂર્ણાંક log_level) {
#જો HAVE_ZSTD
        સ્થિર શૂન્ય *zstd_dl = ખાલી;

        LIBZSTD_NOTE(suggested);

        પાછા dlopen_many_sym_or_warn(
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
#નહીંતર
        પાછા log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

પૂર્ણાંક dlopen_zlib(પૂર્ણાંક log_level) {
#જો HAVE_ZLIB
        સ્થિર શૂન્ય *zlib_dl = ખાલી;

        LIBZ_NOTE(suggested);

        પાછા dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#નહીંતર
        પાછા log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

પૂર્ણાંક dlopen_bzip2(પૂર્ણાંક log_level) {
#જો HAVE_BZIP2
        સ્થિર શૂન્ય *bzip2_dl = ખાલી;

        LIBBZ2_NOTE(suggested);

        પાછા dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#નહીંતર
        પાછા log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

પૂર્ણાંક dlopen_compress_journal(Compression c, પૂર્ણાંક log_level) {
        પસંદગી (c) {
        સ્થિતિ COMPRESSION_NONE:
                પાછા 0;
        સ્થિતિ COMPRESSION_XZ:
                પાછા dlopen_xz(log_level);
        સ્થિતિ COMPRESSION_LZ4:
                પાછા dlopen_lz4(log_level);
        સ્થિતિ COMPRESSION_ZSTD:
                પાછા dlopen_zstd(log_level);
        મૂળભૂત:
                પાછા -EOPNOTSUPP;
        }
}

પૂર્ણાંક dlopen_compress(Compression c, પૂર્ણાંક log_level) {
        પસંદગી (c) {
        સ્થિતિ COMPRESSION_GZIP:
                પાછા dlopen_zlib(log_level);
        સ્થિતિ COMPRESSION_BZIP2:
                પાછા dlopen_bzip2(log_level);
        મૂળભૂત:
                પાછા dlopen_compress_journal(c, log_level);
        }
}

સ્થિર પૂર્ણાંક compress_blob_xz(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                પૂર્ણાંક level) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_alloc_size > 0);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, ખાલી, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, ખાલી }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        પૂર્ણાંક r;

        r = dlopen_xz(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                જો (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        પાછા -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        જો (src_size < 80)
                પાછા -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ ખાલી,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        જો (ret != LZMA_OK)
                પાછા -ENOBUFS;

        *dst_size = out_pos;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક compress_blob_lz4(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                પૂર્ણાંક level) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_alloc_size > 0);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_LZ4
        પૂર્ણાંક r;

        r = dlopen_lz4(LOG_DEBUG);
        જો (r < 0)
                પાછા r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        જો (src_size < 9)
                પાછા -ENOBUFS;

        જો (src_size > INT_MAX)
                પાછા -EFBIG;
        જો (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        જો (level <= 0)
                r = sym_LZ4_compress_default(src, (અક્ષર*)dst + 8, src_size, (પૂર્ણાંક) dst_alloc_size - 8);
        નહીંતર
                r = sym_LZ4_compress_HC(src, (અક્ષર*)dst + 8, src_size, (પૂર્ણાંક) dst_alloc_size - 8, level);
        જો (r <= 0)
                પાછા -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક compress_blob_zstd(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                પૂર્ણાંક level) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_alloc_size > 0);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_ZSTD
        size_t k;
        પૂર્ણાંક r;

        r = dlopen_zstd(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        જો (sym_ZSTD_isError(k))
                પાછા zstd_ret_to_errno(k);

        *dst_size = k;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક compress_blob_gzip(અચલ શૂન્ય *src, uint64_t src_size,
                       શૂન્ય *dst, size_t dst_alloc_size, size_t *dst_size, પૂર્ણાંક level) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_alloc_size > 0);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_ZLIB
        પૂર્ણાંક r;

        r = dlopen_zlib(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size > UINT_MAX)
                પાછા -EFBIG;
        જો (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (પૂર્ણાંક) કદ(s));
        જો (r != Z_OK)
                પાછા -ENOMEM;

        s.next_in = (શૂન્ય*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        જો (r != Z_STREAM_END)
                પાછા -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક compress_blob_bzip2(
                અચલ શૂન્ય *src, uint64_t src_size,
                શૂન્ય *dst, size_t dst_alloc_size, size_t *dst_size, પૂર્ણાંક level) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_alloc_size > 0);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_BZIP2
        પૂર્ણાંક r;

        r = dlopen_bzip2(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size > UINT_MAX)
                પાછા -EFBIG;
        જો (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        જો (r != BZ_OK)
                પાછા -ENOMEM;

        s.next_in = (અક્ષર*) src;
        s.avail_in = src_size;
        s.next_out = (અક્ષર*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        જો (r != BZ_STREAM_END)
                પાછા -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

પૂર્ણાંક compress_blob_journal(
                Compression compression,
                અચલ શૂન્ય *src, uint64_t src_size,
                શૂન્ય *dst, size_t dst_alloc_size, size_t *dst_size, પૂર્ણાંક level) {

        પસંદગી (compression) {
        સ્થિતિ COMPRESSION_XZ:
                પાછા compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        સ્થિતિ COMPRESSION_LZ4:
                પાછા compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        સ્થિતિ COMPRESSION_ZSTD:
                પાછા compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        મૂળભૂત:
                પાછા -EOPNOTSUPP;
        }
}

પૂર્ણાંક compress_blob(
                Compression compression,
                અચલ શૂન્ય *src, uint64_t src_size,
                શૂન્ય *dst, size_t dst_alloc_size, size_t *dst_size, પૂર્ણાંક level) {

        પસંદગી (compression) {
        સ્થિતિ COMPRESSION_GZIP:
                પાછા compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        સ્થિતિ COMPRESSION_BZIP2:
                પાછા compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        મૂળભૂત:
                પાછા compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

સ્થિર પૂર્ણાંક decompress_blob_xz(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **dst,
                size_t *dst_size,
                size_t dst_max) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_XZ
        પૂર્ણાંક r;

        r = dlopen_xz(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        જો (ret != LZMA_OK)
                પાછા -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        જો (!greedy_realloc(dst, space, 1))
                પાછા -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        માટે (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                જો (ret == LZMA_STREAM_END)
                        તોડો;
                જો (ret != LZMA_OK)
                        પાછા -ENOMEM;

                જો (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        તોડો;
                જો (dst_max > 0 && space == dst_max)
                        પાછા -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                પ્રતિજ્ઞા(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                જો (!greedy_realloc(dst, space, 1))
                        પાછા -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક decompress_blob_lz4(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **dst,
                size_t *dst_size,
                size_t dst_max) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_LZ4
        અક્ષર* out;
        પૂર્ણાંક r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size <= 8)
                પાછા -EBADMSG;

        જો (src_size - 8 > INT_MAX)
                પાછા -EFBIG;

        size = unaligned_read_le64(src);
        જો (size < 0 || (unsigned) size != unaligned_read_le64(src))
                પાછા -EFBIG;
        જો (dst_max > 0 && (size_t) size > dst_max)
                પાછા -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        જો (!out)
                પાછા -ENOMEM;

        r = sym_LZ4_decompress_safe((અક્ષર*)src + 8, out, src_size - 8, size);
        જો (r < 0 || r != size)
                પાછા -EBADMSG;

        *dst_size = size;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(શૂન્ય) {
#જો HAVE_ZSTD
        જો (dlopen_zstd(LOG_DEBUG) < 0)
                પાછા 0;

        પાછા sym_ZSTD_DStreamOutSize();
#નહીંતર
        પાછા 0;
#endif
}

સ્થિર પૂર્ણાંક decompress_blob_zstd(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **dst,
                size_t *dst_size,
                size_t dst_max) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_ZSTD
        પૂર્ણાંક r;

        r = dlopen_zstd(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        જો (size == ZSTD_CONTENTSIZE_ERROR)
                પાછા -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        જો (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        નહીંતર {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        જો (!greedy_realloc(dst, space, 1))
                પાછા -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        જો (!dctx)
                પાછા -ENOMEM;

        ZSTD_inBuffer આગત = {
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

        માટે (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &આગત);
                જો (sym_ZSTD_isError(k))
                        પાછા log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                જો (output.pos >= cap)
                        તોડો;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                જો (k == 0 && આગત.pos >= આગત.size)
                        તોડો;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                જો (output.pos == output.size) {
                        જો (space > SIZE_MAX / 2)
                                પાછા -E2BIG;
                        space = MIN(2 * space, cap);
                        જો (!greedy_realloc(dst, space, 1))
                                પાછા -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        ચાલુ;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                જો (આગત.pos >= આગત.size)
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક decompress_blob_gzip(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **dst,
                size_t *dst_size,
                size_t dst_max) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_ZLIB
        પૂર્ણાંક r;

        r = dlopen_zlib(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size > UINT_MAX)
                પાછા -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (પૂર્ણાંક) કદ(s));
        જો (r != Z_OK)
                પાછા -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        જો (!greedy_realloc(dst, space, 1))
                પાછા -ENOMEM;

        s.next_in = (શૂન્ય*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        માટે (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                જો (r == Z_STREAM_END)
                        તોડો;
                જો (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        પાછા -EBADMSG;

                જો (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        તોડો;
                જો (dst_max > 0 && space == dst_max)
                        પાછા -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                જો (!greedy_realloc(dst, space, 1))
                        પાછા -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક decompress_blob_bzip2(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **dst,
                size_t *dst_size,
                size_t dst_max) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(dst);
        પ્રતિજ્ઞા(dst_size);

#જો HAVE_BZIP2
        પૂર્ણાંક r;

        r = dlopen_bzip2(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size > UINT_MAX)
                પાછા -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        જો (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                પાછા -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        જો (!greedy_realloc(dst, space, 1))
                પાછા -ENOMEM;

        s.next_in = (અક્ષર*) src;
        s.avail_in = src_size;
        s.next_out = (અક્ષર*) *dst;
        s.avail_out = space;

        માટે (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                જો (r == BZ_STREAM_END)
                        તોડો;
                જો (r != BZ_OK)
                        પાછા -EBADMSG;

                જો (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        તોડો;
                જો (dst_max > 0 && space == dst_max)
                        પાછા -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                જો (!greedy_realloc(dst, space, 1))
                        પાછા -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (અક્ષર*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

પૂર્ણાંક decompress_blob_journal(
                Compression compression,
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **dst,
                size_t *dst_size,
                size_t dst_max) {

        પસંદગી (compression) {
        સ્થિતિ COMPRESSION_XZ:
                પાછા decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        સ્થિતિ COMPRESSION_LZ4:
                પાછા decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        સ્થિતિ COMPRESSION_ZSTD:
                પાછા decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        મૂળભૂત:
                પાછા -EPROTONOSUPPORT;
        }
}

પૂર્ણાંક decompress_blob(
                Compression compression,
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **dst,
                size_t *dst_size,
                size_t dst_max) {

        પસંદગી (compression) {
        સ્થિતિ COMPRESSION_GZIP:
                પાછા decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        સ્થિતિ COMPRESSION_BZIP2:
                પાછા decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        મૂળભૂત:
                પાછા decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

પૂર્ણાંક decompress_zlib_raw(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય *dst,
                size_t dst_size,
                પૂર્ણાંક wbits) {

#જો HAVE_ZLIB
        પૂર્ણાંક r;

        r = dlopen_zlib(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size > UINT_MAX)
                પાછા -EFBIG;
        જો (dst_size > UINT_MAX)
                પાછા -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (શૂન્ય*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (પૂર્ણાંક) કદ(s));
        જો (r != Z_OK)
                પાછા -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        જો (r != Z_STREAM_END || produced != dst_size)
                પાછા -EBADMSG;

        પાછા 0;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક decompress_startswith_xz(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **buffer,
                અચલ શૂન્ય *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(prefix);

#જો HAVE_XZ
        પૂર્ણાંક r;

        r = dlopen_xz(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        જો (ret != LZMA_OK)
                પાછા -EBADMSG;

        જો (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                પાછા -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        માટે (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                જો (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        પાછા -EBADMSG;

                જો (allocated - s.avail_out >= prefix_len + 1)
                        પાછા memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((અચલ uint8_t*) *buffer)[prefix_len] == extra;

                જો (ret == LZMA_STREAM_END)
                        પાછા 0;

                s.avail_out += allocated;

                જો (!(greedy_realloc(buffer, allocated * 2, 1)))
                        પાછા -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક decompress_startswith_lz4(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **buffer,
                અચલ શૂન્ય *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(prefix);

#જો HAVE_LZ4
        size_t allocated;
        પૂર્ણાંક r;

        r = dlopen_lz4(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size <= 8)
                પાછા -EBADMSG;

        જો (src_size - 8 > INT_MAX)
                પાછા -EFBIG;

        જો (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                પાછા -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (અક્ષર*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        જો (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                જો (sym_LZ4_version_સંખ્યા() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        પાછા 0;

                જો (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        જો (memcmp(*buffer, prefix, r) != 0)
                                પાછા 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                જો (r < 0)
                        પાછા r;

                જો (size < prefix_len + 1)
                        પાછા 0;
        }

        પાછા memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((અચલ uint8_t*) *buffer)[prefix_len] == extra;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક decompress_startswith_zstd(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **buffer,
                અચલ શૂન્ય *prefix,
                size_t prefix_len,
                uint8_t extra) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(prefix);

#જો HAVE_ZSTD
        પૂર્ણાંક r;

        r = dlopen_zstd(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        જો (size == ZSTD_CONTENTSIZE_ERROR)
                પાછા -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        જો (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                પાછા 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        જો (!dctx)
                પાછા -ENOMEM;

        જો (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                પાછા -ENOMEM;

        ZSTD_inBuffer આગત = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &આગત);
        જો (sym_ZSTD_isError(k))
                પાછા log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        જો (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                જો (k != 0)
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                પાછા 0; /* Decompressed text too short to match the prefix and extra */
        }

        પાછા memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((અચલ uint8_t*) *buffer)[prefix_len] == extra;
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક decompress_startswith_gzip(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **buffer,
                અચલ શૂન્ય *prefix,
                size_t prefix_len,
                uint8_t extra) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(prefix);

#જો HAVE_ZLIB
        પૂર્ણાંક r;

        r = dlopen_zlib(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size > UINT_MAX)
                પાછા -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (પૂર્ણાંક) કદ(s));
        જો (r != Z_OK)
                પાછા -EBADMSG;

        જો (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                પાછા -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (શૂન્ય*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        માટે (;;) {
                r = sym_inflate(&s, Z_FINISH);

                જો (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        પાછા -EBADMSG;

                જો (allocated - s.avail_out >= prefix_len + 1)
                        પાછા memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((અચલ uint8_t*) *buffer)[prefix_len] == extra;

                જો (r == Z_STREAM_END)
                        પાછા 0;

                જો (s.avail_out > 0)
                        પાછા -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                જો (!(greedy_realloc(buffer, allocated * 2, 1)))
                        પાછા -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

સ્થિર પૂર્ણાંક decompress_startswith_bzip2(
                અચલ શૂન્ય *src,
                uint64_t src_size,
                શૂન્ય **buffer,
                અચલ શૂન્ય *prefix,
                size_t prefix_len,
                uint8_t extra) {

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(src_size > 0);
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(prefix);

#જો HAVE_BZIP2
        પૂર્ણાંક r;

        r = dlopen_bzip2(LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        જો (src_size > UINT_MAX)
                પાછા -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        જો (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                પાછા -EIO;

        જો (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                પાછા -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (અક્ષર*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        માટે (;;) {
                r = sym_BZ2_bzDecompress(&s);

                જો (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        પાછા -EBADMSG;

                જો (allocated - s.avail_out >= prefix_len + 1)
                        પાછા memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((અચલ uint8_t*) *buffer)[prefix_len] == extra;

                જો (r == BZ_STREAM_END)
                        પાછા 0;

                જો (s.avail_out > 0)
                        પાછા -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                જો (!(greedy_realloc(buffer, allocated * 2, 1)))
                        પાછા -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (અક્ષર*) *buffer + used;
        }
#નહીંતર
        પાછા -EPROTONOSUPPORT;
#endif
}

પૂર્ણાંક decompress_startswith_journal(
                Compression compression,
                અચલ શૂન્ય *src, uint64_t src_size,
                શૂન્ય **buffer,
                અચલ શૂન્ય *prefix, size_t prefix_len,
                uint8_t extra) {

        પસંદગી (compression) {
        સ્થિતિ COMPRESSION_XZ:
                પાછા decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        સ્થિતિ COMPRESSION_LZ4:
                પાછા decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        સ્થિતિ COMPRESSION_ZSTD:
                પાછા decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        મૂળભૂત:
                પાછા -EOPNOTSUPP;
        }
}

પૂર્ણાંક decompress_startswith(
                Compression compression,
                અચલ શૂન્ય *src, uint64_t src_size,
                શૂન્ય **buffer,
                અચલ શૂન્ય *prefix, size_t prefix_len,
                uint8_t extra) {

        પસંદગી (compression) {
        સ્થિતિ COMPRESSION_GZIP:
                પાછા decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        સ્થિતિ COMPRESSION_BZIP2:
                પાછા decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        મૂળભૂત:
                પાછા decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

પૂર્ણાંક compress_stream(
                Compression type,
                પૂર્ણાંક fdf, પૂર્ણાંક fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = ખાલી;
        _cleanup_free_ શૂન્ય *buf = ખાલી;
        _cleanup_free_ uint8_t *આગત = ખાલી;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fdf >= 0);
        પ્રતિજ્ઞા(fdt >= 0);

        r = compressor_new(&c, type);
        જો (r < 0)
                પાછા r;

        આગત = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        જો (!આગત)
                પાછા -ENOMEM;

        માટે (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                જો (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = પઠ(fdf, આગત, m);
                જો (n < 0)
                        પાછા -errno;

                જો (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        જો (r < 0)
                                પાછા r;

                        જો (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                જો (r < 0)
                                        પાછા r;
                                total_out += buf_size;
                        }
                        તોડો;
                }

                total_in += n;
                જો (max_bytes != UINT64_MAX) {
                        પ્રતિજ્ઞા(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, આગત, n, &buf, &buf_size, &buf_alloc);
                જો (r < 0)
                        પાછા r;

                જો (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        જો (r < 0)
                                પાછા r;
                        total_out += buf_size;
                }
        }

        જો (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        જો (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        નહીંતર
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (દ્વિગુણ) total_out / total_in * 100);

        પાછા 0;
}

સ્થિર પૂર્ણાંક should_sparse(પૂર્ણાંક fd) {
        સંરચના stat st;

        પ્રતિજ્ઞા(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        જો (fstat(fd, &st) < 0)
                પાછા -errno;

        જો (!S_ISREG(st.st_mode))
                પાછા false;

        પૂર્ણાંક flags = fcntl(fd, F_GETFL);
        જો (flags < 0)
                પાછા -errno;

        જો (FLAGS_SET(flags, O_APPEND))
                પાછા false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        જો (pos < 0)
                પાછા -errno;

        પાછા pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
સ્થિર પૂર્ણાંક finalize_sparse(પૂર્ણાંક fd) {
        off_t pos;

        પ્રતિજ્ઞા(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        જો (pos < 0)
                પાછા -errno;

        જો (ftruncate(fd, pos) < 0)
                પાછા -errno;

        પાછા 0;
}

/* Common helper for decompress_stream_*() wrappers */

સંરચના decompress_stream_userdata {
        પૂર્ણાંક fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

સ્થિર પૂર્ણાંક decompress_stream_write_callback(અચલ શૂન્ય *data, size_t size, શૂન્ય *userdata) {
        સંરચના decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        જો (u->max_bytes != UINT64_MAX) {
                જો (u->max_bytes < size)
                        પાછા -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        જો (u->skip > 0) {
                જો (u->skip >= size) {
                        u->skip -= size;
                        પાછા 0;
                }

                data = (અચલ uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        જો (size == 0)
                પાછા 0;

        જો (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                જો (k < 0)
                        પાછા (પૂર્ણાંક) k;
                પાછા 0;
        }

        પાછા loop_write(u->fd, data, size);
}

સ્થિર પૂર્ણાંક decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        પ્રતિજ્ઞા(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        જો (!c)
                પાછા -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        પસંદગી (type) {
        સ્થિતિ COMPRESSION_NONE:
                તોડો;

#જો HAVE_XZ
        સ્થિતિ COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                જો (xz_ret != LZMA_OK)
                        પાછા -EIO;
                તોડો;
        }
#endif

#જો HAVE_LZ4
        સ્થિતિ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                જો (sym_LZ4F_isError(rc))
                        પાછા -ENOMEM;

                તોડો;
        }
#endif

#જો HAVE_ZSTD
        સ્થિતિ COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                જો (!c->d_zstd)
                        પાછા -ENOMEM;
                તોડો;
#endif

#જો HAVE_ZLIB
        સ્થિતિ COMPRESSION_GZIP:
                જો (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (પૂર્ણાંક) કદ(c->gzip)) != Z_OK)
                        પાછા -EIO;
                તોડો;
#endif

#જો HAVE_BZIP2
        સ્થિતિ COMPRESSION_BZIP2:
                જો (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        પાછા -EIO;
                તોડો;
#endif

        મૂળભૂત:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        પાછા 0;
}

સ્થિર પૂર્ણાંક decompress_blob_to_fd_impl(
                Compression type,
                અચલ શૂન્ય *src,
                uint64_t src_size,
                પૂર્ણાંક fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(src);
        પ્રતિજ્ઞા(fdt >= 0);

        જો (type == COMPRESSION_NONE)
                પાછા -EOPNOTSUPP;

        જો (src_size == 0)
                પાછા -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        જો (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                જો (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        જો (src_size <= કદ(uint64_t))
                                પાછા -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        જો (sz < skip)
                                પાછા -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                જો (max_bytes == 0)
                        પાછા -ENOBUFS;

                _cleanup_free_ શૂન્ય *buf = ખાલી;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                જો (r < 0)
                        પાછા r;

                /* Check the result size again for safety. */
                જો (size < skip)
                        પાછા -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                જો (r < 0)
                        પાછા r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (દ્વિગુણ) size / src_size * 100);

                જો (ret_size)
                        *ret_size = size - skip;
                પાછા 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = ખાલી;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        જો (r < 0)
                પાછા r;

        સંરચના decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ધકેલો(c, src, src_size, decompress_stream_write_callback, &userdata);
        જો (r < 0)
                પાછા r;

        /* Finalize */
        r = decompressor_ધકેલો(c, /* data= */ ખાલી, /* size= */ 0, decompress_stream_write_callback, &userdata);
        જો (r < 0)
                પાછા r;

        જો (userdata.total_out < skip)
                પાછા -EBADMSG;

        જો (userdata.sparse) {
                r = finalize_sparse(fdt);
                જો (r < 0)
                        પાછા r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (દ્વિગુણ) userdata.total_out / src_size * 100);

        જો (ret_size)
                *ret_size = userdata.total_out - skip;

        પાછા 0;
}

પૂર્ણાંક decompress_blob_to_fd_journal(
                Compression type,
                અચલ શૂન્ય *src,
                uint64_t src_size,
                પૂર્ણાંક fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        પૂર્ણાંક r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        પાછા decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

પૂર્ણાંક decompress_blob_to_fd(
                Compression type,
                અચલ શૂન્ય *src,
                uint64_t src_size,
                પૂર્ણાંક fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        પૂર્ણાંક r;

        r = dlopen_compress(type, LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        પાછા decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

પૂર્ણાંક decompress_stream_full(
                Compression type,
                પૂર્ણાંક fdf, પૂર્ણાંક fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = ખાલી;
        _cleanup_free_ uint8_t *buf = ખાલી;
        uint64_t total_in = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fdf >= 0);
        પ્રતિજ્ઞા(fdt >= 0);

        જો (type == COMPRESSION_NONE)
                પાછા -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        r = decompressor_new(&c, type, flags);
        જો (r < 0)
                પાછા r;

        સંરચના decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        જો (!buf)
                પાછા -ENOMEM;

        માટે (;;) {
                ssize_t n;

                n = પઠ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                જો (n < 0)
                        પાછા -errno;

                total_in += n;

                r = decompressor_ધકેલો(c, buf, n, decompress_stream_write_callback, &userdata);
                જો (r < 0)
                        પાછા r;

                જો (n == 0)
                        તોડો;
        }

        જો (total_in == 0)
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        જો (userdata.sparse) {
                r = finalize_sparse(fdt);
                જો (r < 0)
                        પાછા r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (દ્વિગુણ) userdata.total_out / total_in * 100);

        પાછા 0;
}

પૂર્ણાંક decompress_stream_by_filename(અચલ અક્ષર *filename, પૂર્ણાંક fdf, પૂર્ણાંક fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        જો (c == COMPRESSION_NONE)
                પાછા -EPROTONOSUPPORT;

        પાછા decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        જો (!c)
                પાછા ખાલી;

        પસંદગી (c->type) {

#જો HAVE_XZ
        સ્થિતિ COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                તોડો;
#endif

#જો HAVE_LZ4
        સ્થિતિ COMPRESSION_LZ4:
                જો (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = ખાલી;
                        c->lz4_header = mfree(c->lz4_header);
                } નહીંતર {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = ખાલી;
                }
                તોડો;
#endif

#જો HAVE_ZSTD
        સ્થિતિ COMPRESSION_ZSTD:
                જો (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = ખાલી;
                } નહીંતર {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = ખાલી;
                }
                તોડો;
#endif

#જો HAVE_ZLIB
        સ્થિતિ COMPRESSION_GZIP:
                જો (c->encoding)
                        sym_deflateEnd(&c->gzip);
                નહીંતર
                        sym_inflateEnd(&c->gzip);
                તોડો;
#endif

#જો HAVE_BZIP2
        સ્થિતિ COMPRESSION_BZIP2:
                જો (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                નહીંતર
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                તોડો;
#endif

        મૂળભૂત:
                તોડો;
        }

        પાછા mfree(c);
}

Compression compressor_type(અચલ Compressor *c) {
        પાછા c ? c->type : _COMPRESSION_INVALID;
}

પૂર્ણાંક decompressor_detect(Decompressor **decompressor, અચલ શૂન્ય *data, size_t size) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(decompressor);

        જો (*decompressor)
                પાછા 1;

        જો (size < COMPRESSION_MAGIC_BYTES_MAX)
                પાછા 0;

        પ્રતિજ્ઞા(data);

        Compression type = compression_detect_from_magic(data);
        જો (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        જો (r < 0)
                પાછા r;

        પાછા 1;
}

પૂર્ણાંક decompressor_force(Decompressor **decompressor, Compression type) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(decompressor);
        પ્રતિજ્ઞા(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        *decompressor = compressor_free(*decompressor);
        પાછા decompressor_new(decompressor, type, /* flags= */ 0);
}

પૂર્ણાંક decompressor_force_off(Decompressor **decompressor) {
        પાછા decompressor_force(decompressor, COMPRESSION_NONE);
}

પૂર્ણાંક decompressor_ધકેલો(Decompressor *c, અચલ શૂન્ય *data, size_t size, DecompressorCallback callback, શૂન્ય *userdata) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(c);
        પ્રતિજ્ઞા(data || size == 0);
        પ્રતિજ્ઞા(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        જો (c->encoding)
                પાછા -EINVAL;

        જો (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                પાછા 0;

#જો HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = ખાલી;
        જો (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                જો (!buffer)
                        પાછા -ENOMEM;
        }
#endif

        પસંદગી (c->type) {

        સ્થિતિ COMPRESSION_NONE:
                જો (c->finished)
                        પાછા -EBADMSG;

                જો (size == 0) {
                        c->finished = true;
                        પાછા 0;
                }

                r = callback(data, size, userdata);
                જો (r < 0)
                        પાછા r;

                પાછા 0;

#જો HAVE_XZ
        સ્થિતિ COMPRESSION_XZ:
                જો (c->finished)
                        પાછા -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                માટે (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        જો (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                પાછા -EBADMSG;

                        જો (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                પાછા -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        જો (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                જો (r < 0)
                                        પાછા r;
                        }

                        જો (lzr == LZMA_STREAM_END) {
                                જો (c->xz.avail_in > 0)
                                        પાછા -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                પાછા 0;
                        }

                        જો (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                જો (size == 0)
                                        પાછા -EBADMSG; /* truncated */

                                પાછા 0; /* No progress possible with the current input. */
                        }
                }
#endif

#જો HAVE_LZ4
        સ્થિતિ COMPRESSION_LZ4: {
                અચલ uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                માટે (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, ખાલી);
                        જો (sym_LZ4F_isError(rc))
                                પાછા -EBADMSG;

                        જો (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                પાછા -EIO;

                        જો (consumed > 0) {
                                પ્રતિજ્ઞા(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        જો (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                જો (r < 0)
                                        પાછા r;
                        }

                        જો (src_remaining == 0) {
                                જો (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        પાછા 0;
                                }

                                જો (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ચાલુ;

                                જો (size == 0)
                                        પાછા -EBADMSG; /* truncated */

                                પાછા 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#જો HAVE_ZSTD
        સ્થિતિ COMPRESSION_ZSTD: {
                ZSTD_inBuffer આગત = {
                        .src =  (શૂન્ય*) data,
                        .size = size,
                };

                c->finished = false;

                માટે (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &આગત);
                        જો (sym_ZSTD_isError(res))
                                પાછા -EBADMSG;

                        જો (આગત.pos > આગત.size ||
                            output.pos > output.size)
                                પાછા -EIO;

                        જો (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                જો (r < 0)
                                        પાછા r;
                        }

                        જો (આગત.pos == આગત.size) {
                                જો (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        પાછા 0;
                                }

                                જો (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        ચાલુ;

                                જો (size == 0)
                                        પાછા -EBADMSG; /* truncated */

                                પાછા 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#જો HAVE_ZLIB
        સ્થિતિ COMPRESSION_GZIP:
                જો (size > UINT_MAX)
                        પાછા -EFBIG;

                c->gzip.next_in = (શૂન્ય*) data;
                c->gzip.avail_in = size;

                માટે (;;) {
                        જો (c->finished) {
                                જો (sym_inflateReset(&c->gzip) != Z_OK)
                                        પાછા -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        પૂર્ણાંક zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        જો (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                પાછા -EBADMSG;

                        જો (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                પાછા -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        જો (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                જો (r < 0)
                                        પાછા r;
                        }

                        જો (zr == Z_STREAM_END) {
                                c->finished = true;

                                જો (c->gzip.avail_in > 0)
                                        ચાલુ; /* New stream may be pending. */

                                પાછા 0;
                        }

                        જો (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                જો (size == 0)
                                        પાછા -EBADMSG; /* truncated */

                                પાછા 0; /* No progress possible with the current input. */
                        }
                }
#endif

#જો HAVE_BZIP2
        સ્થિતિ COMPRESSION_BZIP2:
                જો (size > UINT_MAX)
                        પાછા -EFBIG;

                c->bzip2.next_in = (અક્ષર*) data;
                c->bzip2.avail_in = size;

                માટે (;;) {
                        જો (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                જો (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        પાછા -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (અક્ષર*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        પૂર્ણાંક bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        જો (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                પાછા -EBADMSG;

                        જો (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                પાછા -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        જો (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                જો (r < 0)
                                        પાછા r;
                        }

                        જો (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                જો (c->bzip2.avail_in > 0)
                                        ચાલુ; /* New stream may be pending. */

                                પાછા 0;
                        }

                        જો (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                જો (size == 0)
                                        પાછા -EBADMSG; /* truncated */

                                પાછા 0; /* No progress possible with the current input. */
                        }
                }
#endif

        મૂળભૂત:
                assert_not_reached();
        }
}

પૂર્ણાંક compressor_new(Compressor **ret, Compression type) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        જો (r < 0)
                પાછા r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        જો (!c)
                પાછા -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        પસંદગી (type) {

#જો HAVE_XZ
        સ્થિતિ COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                જો (xzr != LZMA_OK)
                        પાછા -EIO;

                c->type = COMPRESSION_XZ;
                તોડો;
        }
#endif

#જો HAVE_LZ4
        સ્થિતિ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                જો (sym_LZ4F_isError(rc))
                        પાછા -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                જો (!c->lz4_header)
                        પાછા -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                જો (sym_LZ4F_isError(c->lz4_header_size))
                        પાછા -EINVAL;

                તોડો;
        }
#endif

#જો HAVE_ZSTD
        સ્થિતિ COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                જો (!c->c_zstd)
                        પાછા -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                જો (sym_ZSTD_isError(z))
                        પાછા -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                જો (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                તોડો;
#endif

#જો HAVE_ZLIB
        સ્થિતિ COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (પૂર્ણાંક) કદ(c->gzip));
                જો (r != Z_OK)
                        પાછા -EIO;

                c->type = COMPRESSION_GZIP;
                તોડો;
#endif

#જો HAVE_BZIP2
        સ્થિતિ COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                જો (r != BZ_OK)
                        પાછા -EIO;

                c->type = COMPRESSION_BZIP2;
                તોડો;
#endif

        સ્થિતિ COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                તોડો;

        મૂળભૂત:
                પાછા -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        પાછા 0;
}

#જો HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
સ્થિર પૂર્ણાંક enlarge_buffer(શૂન્ય **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(buffer_size);
        પ્રતિજ્ઞા(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        જો (*buffer_allocated >= need)
                પાછા 0;

        જો (!greedy_realloc(buffer, need, 1))
                પાછા -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        પાછા 1;
}
#endif

પૂર્ણાંક compressor_start(
                Compressor *c,
                અચલ શૂન્ય *data,
                size_t size,
                શૂન્ય **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#જો HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        પૂર્ણાંક r;
#endif

        પ્રતિજ્ઞા(c);
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(buffer_size);
        પ્રતિજ્ઞા(buffer_allocated);

        જો (!c->encoding)
                પાછા -EINVAL;

        જો (size == 0)
                પાછા 0;

        પ્રતિજ્ઞા(data);

        *buffer_size = 0;

        પસંદગી (c->type) {

#જો HAVE_XZ
        સ્થિતિ COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                જ્યારે (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        જો (r < 0)
                                પાછા r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        જો (lzr != LZMA_OK)
                                પાછા -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                તોડો;
#endif

#જો HAVE_LZ4
        સ્થિતિ COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                જો (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        જો (r < 0)
                                પાછા r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                જો (r < 0)
                        પાછા r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, ખાલી);
                જો (sym_LZ4F_isError(n))
                        પાછા -EIO;

                *buffer_size += n;
                તોડો;
        }
#endif

#જો HAVE_ZSTD
        સ્થિતિ COMPRESSION_ZSTD: {
                ZSTD_inBuffer આગત = {
                        .src = data,
                        .size = size,
                };

                જ્યારે (આગત.pos < આગત.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        જો (r < 0)
                                પાછા r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &આગત, ZSTD_e_continue);
                        જો (sym_ZSTD_isError(res))
                                પાછા -EIO;

                        *buffer_size += output.pos;
                }

                તોડો;
        }
#endif

#જો HAVE_ZLIB
        સ્થિતિ COMPRESSION_GZIP:
                જો (size > UINT_MAX)
                        પાછા -EFBIG;

                c->gzip.next_in = (શૂન્ય*) data;
                c->gzip.avail_in = size;

                જ્યારે (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        જો (r < 0)
                                પાછા r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        જો (r != Z_OK)
                                પાછા -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                તોડો;
#endif

#જો HAVE_BZIP2
        સ્થિતિ COMPRESSION_BZIP2:
                જો (size > UINT_MAX)
                        પાછા -EFBIG;

                c->bzip2.next_in = (શૂન્ય*) data;
                c->bzip2.avail_in = size;

                જ્યારે (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        જો (r < 0)
                                પાછા r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (શૂન્ય*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        જો (r != BZ_RUN_OK)
                                પાછા -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                તોડો;
#endif

        સ્થિતિ COMPRESSION_NONE:

                જો (*buffer_allocated < size) {
                        શૂન્ય *p;

                        p = realloc(*buffer, size);
                        જો (!p)
                                પાછા -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                તોડો;

        મૂળભૂત:
                પાછા -EOPNOTSUPP;
        }

        પાછા 0;
}

પૂર્ણાંક compressor_finish(Compressor *c, શૂન્ય **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#જો HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        પૂર્ણાંક r;
#endif

        પ્રતિજ્ઞા(c);
        પ્રતિજ્ઞા(buffer);
        પ્રતિજ્ઞા(buffer_size);
        પ્રતિજ્ઞા(buffer_allocated);

        જો (!c->encoding)
                પાછા -EINVAL;

        *buffer_size = 0;

        પસંદગી (c->type) {

#જો HAVE_XZ
        સ્થિતિ COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                કરો {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        જો (r < 0)
                                પાછા r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        જો (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                પાછા -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } જ્યારે (lzr != LZMA_STREAM_END);

                તોડો;
        }
#endif

#જો HAVE_LZ4
        સ્થિતિ COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                જો (r < 0)
                        પાછા r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, ખાલી);
                જો (sym_LZ4F_isError(n))
                        પાછા -EIO;

                *buffer_size = n;
                તોડો;
        }
#endif

#જો HAVE_ZSTD
        સ્થિતિ COMPRESSION_ZSTD: {
                ZSTD_inBuffer આગત = {};
                size_t res;

                કરો {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        જો (r < 0)
                                પાછા r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &આગત, ZSTD_e_end);
                        જો (sym_ZSTD_isError(res))
                                પાછા -EIO;

                        *buffer_size += output.pos;
                } જ્યારે (res != 0);

                તોડો;
        }
#endif

#જો HAVE_ZLIB
        સ્થિતિ COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                કરો {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        જો (r < 0)
                                પાછા r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        જો (!IN_SET(r, Z_OK, Z_STREAM_END))
                                પાછા -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } જ્યારે (r != Z_STREAM_END);

                તોડો;
#endif

#જો HAVE_BZIP2
        સ્થિતિ COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                કરો {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        જો (r < 0)
                                પાછા r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (શૂન્ય*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        જો (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                પાછા -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } જ્યારે (r != BZ_STREAM_END);

                તોડો;
#endif

        સ્થિતિ COMPRESSION_NONE:
                તોડો;

        મૂળભૂત:
                પાછા -EOPNOTSUPP;
        }

        પાછા 0;
}
