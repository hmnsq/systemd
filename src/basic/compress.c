/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#যদি HAVE_XZ
#ifndef SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#endif
#অন্তর্ভাব <lzma.h>
#endif

#যদি HAVE_LZ4
#ifndef SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#endif
#অন্তর্ভাব <lz4.h>
#অন্তর্ভাব <lz4frame.h>
#অন্তর্ভাব <lz4hc.h>
#endif

#যদি HAVE_ZSTD
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#endif
#অন্তর্ভাব <zstd.h>
#অন্তর্ভাব <zstd_errors.h>
#endif

#যদি HAVE_ZLIB
#ifndef SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#endif
#অন্তর্ভাব <zlib.h>
#endif

#যদি HAVE_BZIP2
#ifndef SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#endif
#অন্তর্ভাব <bzlib.h>
#endif

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "bitfield.h"
#অন্তর্ভাব "compress.h"
#অন্তর্ভাব "dlfcn-util.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "unaligned.h"

#যদি HAVE_XZ
স্থির DLSYM_PROTOTYPE(lzma_auto_decoder) = রিক্ত;
স্থির DLSYM_PROTOTYPE(lzma_code) = রিক্ত;
স্থির DLSYM_PROTOTYPE(lzma_easy_encoder) = রিক্ত;
স্থির DLSYM_PROTOTYPE(lzma_end) = রিক্ত;
স্থির DLSYM_PROTOTYPE(lzma_stream_buffer_encode) = রিক্ত;
স্থির DLSYM_PROTOTYPE(lzma_stream_decoder) = রিক্ত;
স্থির DLSYM_PROTOTYPE(lzma_lzma_preset) = রিক্ত;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
স্থির অন্তঃস্থ শূন্য lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#যদি HAVE_LZ4
স্থির DLSYM_PROTOTYPE(LZ4F_compressBegin) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_compressBound) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_compressEnd) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_compressUpdate) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_decompress) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4F_isError) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4_compress_HC) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4_compress_default) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4_decompress_safe) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = রিক্ত;
স্থির DLSYM_PROTOTYPE(LZ4_version_সংখ্যা) = রিক্ত;

স্থির ধ্রুব LZ4F_preferences_t lz4_preferences = {
        .frameInfo.blockSizeID = 5,
};
#endif

#যদি HAVE_ZSTD
স্থির DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_compress) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_compressStream2) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_createCCtx) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_createDCtx) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_CStreamInSize) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_CStreamOutSize) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_decompressStream) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_DStreamInSize) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_DStreamOutSize) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_freeCCtx) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_freeDCtx) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_getErrorCode) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_getErrorName) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_getFrameContentSize) = রিক্ত;
স্থির DLSYM_PROTOTYPE(ZSTD_isError) = রিক্ত;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, রিক্ত);

স্থির পূর্ণাঙ্ক zstd_ret_to_errno(size_t ret) {
        বিকল্প (sym_ZSTD_getErrorCode(ret)) {
        পক্ষ ZSTD_error_dstSize_tooSmall:
                প্রত্যাগম -ENOBUFS;
        পক্ষ ZSTD_error_memory_allocation:
                প্রত্যাগম -ENOMEM;
        স্বভাব:
                প্রত্যাগম -EBADMSG;
        }
}
#endif

#যদি HAVE_ZLIB
স্থির DLSYM_PROTOTYPE(deflateInit2_) = রিক্ত;
স্থির DLSYM_PROTOTYPE(deflate) = রিক্ত;
স্থির DLSYM_PROTOTYPE(deflateEnd) = রিক্ত;
স্থির DLSYM_PROTOTYPE(inflateInit2_) = রিক্ত;
স্থির DLSYM_PROTOTYPE(inflate) = রিক্ত;
স্থির DLSYM_PROTOTYPE(inflateEnd) = রিক্ত;
স্থির DLSYM_PROTOTYPE(inflateReset) = রিক্ত;

স্থির অন্তঃস্থ শূন্য deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

স্থির অন্তঃস্থ শূন্য inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#যদি HAVE_BZIP2
স্থির DLSYM_PROTOTYPE(BZ2_bzCompressInit) = রিক্ত;
স্থির DLSYM_PROTOTYPE(BZ2_bzCompress) = রিক্ত;
স্থির DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = রিক্ত;
স্থির DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = রিক্ত;
স্থির DLSYM_PROTOTYPE(BZ2_bzDecompress) = রিক্ত;
স্থির DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = রিক্ত;

স্থির অন্তঃস্থ শূন্য BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

স্থির অন্তঃস্থ শূন্য BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
সংরচনা Compressor {
        Compression type;
        bool encoding;
        bool finished;
        সঙ্ঘ {
#যদি HAVE_XZ
                lzma_stream xz;
#endif
#যদি HAVE_LZ4
                সংরচনা {
                        LZ4F_compressionContext_t c_lz4;
                        শূন্য *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_size;
                };
                LZ4F_decompressionContext_t d_lz4;
#endif
#যদি HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#endif
#যদি HAVE_ZLIB
                z_stream gzip;
#endif
#যদি HAVE_BZIP2
                bz_stream bzip2;
#endif
        };
};

#define ALIGN_8(l) ALIGN_TO(l, পরিমাণ(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

স্থির ধ্রুব বর্ণ* ধ্রুব compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

স্থির ধ্রুব বর্ণ* ধ্রুব compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

স্থির ধ্রুব বর্ণ* ধ্রুব compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(ধ্রুব বর্ণ *s) {
        Compression c;

        প্রতিজ্ঞা(s);

        c = compression_from_string(s);
        যদি (c >= 0)
                প্রত্যাগম c;

        প্রত্যাগম compression_uppercase_from_string(s);
}

Compression compression_from_filename(ধ্রুব বর্ণ *filename) {
        Compression c;
        ধ্রুব বর্ণ *e;

        প্রতিজ্ঞা(filename);

        e = strrchr(filename, '.');
        যদি (!e)
                প্রত্যাগম COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        যদি (c < 0)
                প্রত্যাগম COMPRESSION_NONE;

        প্রত্যাগম c;
}

bool compression_supported_journal(Compression c) {
        স্থির ধ্রুব unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        প্রতিজ্ঞা(c >= 0);
        প্রতিজ্ঞা(c < _COMPRESSION_MAX);

        প্রত্যাগম BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        স্থির ধ্রুব unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        প্রতিজ্ঞা(c >= 0);
        প্রতিজ্ঞা(c < _COMPRESSION_MAX);

        প্রত্যাগম BIT_SET(supported, c);
}

Compression compression_detect_from_magic(ধ্রুব uint8_t data[স্থির COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        যদি (memcmp(data, (ধ্রুব uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                প্রত্যাগম COMPRESSION_GZIP;
        যদি (memcmp(data, (ধ্রুব uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                প্রত্যাগম COMPRESSION_XZ;
        যদি (memcmp(data, (ধ্রুব uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                প্রত্যাগম COMPRESSION_ZSTD;
        যদি (memcmp(data, (ধ্রুব uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                প্রত্যাগম COMPRESSION_LZ4;
        যদি (memcmp(data, (ধ্রুব uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                প্রত্যাগম COMPRESSION_BZIP2;

        প্রত্যাগম _COMPRESSION_INVALID;
}

পূর্ণাঙ্ক dlopen_xz(পূর্ণাঙ্ক log_level) {
#যদি HAVE_XZ
        স্থির শূন্য *lzma_dl = রিক্ত;

        LIBLZMA_NOTE(suggested);

        প্রত্যাগম dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_buffer_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#অন্যথা
        প্রত্যাগম log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

পূর্ণাঙ্ক dlopen_lz4(পূর্ণাঙ্ক log_level) {
#যদি HAVE_LZ4
        স্থির শূন্য *lz4_dl = রিক্ত;

        LIBLZ4_NOTE(suggested);

        প্রত্যাগম dlopen_many_sym_or_warn(
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
                        DLSYM_ARG(LZ4_version_সংখ্যা));
#অন্যথা
        প্রত্যাগম log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

পূর্ণাঙ্ক dlopen_zstd(পূর্ণাঙ্ক log_level) {
#যদি HAVE_ZSTD
        স্থির শূন্য *zstd_dl = রিক্ত;

        LIBZSTD_NOTE(suggested);

        প্রত্যাগম dlopen_many_sym_or_warn(
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
#অন্যথা
        প্রত্যাগম log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

পূর্ণাঙ্ক dlopen_zlib(পূর্ণাঙ্ক log_level) {
#যদি HAVE_ZLIB
        স্থির শূন্য *zlib_dl = রিক্ত;

        LIBZ_NOTE(suggested);

        প্রত্যাগম dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflateEnd),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflateEnd),
                        DLSYM_ARG(inflateReset));
#অন্যথা
        প্রত্যাগম log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

পূর্ণাঙ্ক dlopen_bzip2(পূর্ণাঙ্ক log_level) {
#যদি HAVE_BZIP2
        স্থির শূন্য *bzip2_dl = রিক্ত;

        LIBBZ2_NOTE(suggested);

        প্রত্যাগম dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#অন্যথা
        প্রত্যাগম log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

পূর্ণাঙ্ক dlopen_compress_journal(Compression c, পূর্ণাঙ্ক log_level) {
        বিকল্প (c) {
        পক্ষ COMPRESSION_NONE:
                প্রত্যাগম 0;
        পক্ষ COMPRESSION_XZ:
                প্রত্যাগম dlopen_xz(log_level);
        পক্ষ COMPRESSION_LZ4:
                প্রত্যাগম dlopen_lz4(log_level);
        পক্ষ COMPRESSION_ZSTD:
                প্রত্যাগম dlopen_zstd(log_level);
        স্বভাব:
                প্রত্যাগম -EOPNOTSUPP;
        }
}

পূর্ণাঙ্ক dlopen_compress(Compression c, পূর্ণাঙ্ক log_level) {
        বিকল্প (c) {
        পক্ষ COMPRESSION_GZIP:
                প্রত্যাগম dlopen_zlib(log_level);
        পক্ষ COMPRESSION_BZIP2:
                প্রত্যাগম dlopen_bzip2(log_level);
        স্বভাব:
                প্রত্যাগম dlopen_compress_journal(c, log_level);
        }
}

স্থির পূর্ণাঙ্ক compress_blob_xz(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                পূর্ণাঙ্ক level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_size > 0);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, রিক্ত, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, রিক্ত }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        পূর্ণাঙ্ক r;

        r = dlopen_xz(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                যদি (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        প্রত্যাগম -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        যদি (src_size < 80)
                প্রত্যাগম -ENOBUFS;

        ret = sym_lzma_stream_buffer_encode(filters, LZMA_CHECK_NONE, /* allocator= */ রিক্ত,
                                            src, src_size, dst, &out_pos, dst_alloc_size);
        যদি (ret != LZMA_OK)
                প্রত্যাগম -ENOBUFS;

        *dst_size = out_pos;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক compress_blob_lz4(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                পূর্ণাঙ্ক level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_size > 0);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_LZ4
        পূর্ণাঙ্ক r;

        r = dlopen_lz4(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        যদি (src_size < 9)
                প্রত্যাগম -ENOBUFS;

        যদি (src_size > INT_MAX)
                প্রত্যাগম -EFBIG;
        যদি (dst_alloc_size > INT_MAX)
                dst_alloc_size = INT_MAX;

        যদি (level <= 0)
                r = sym_LZ4_compress_default(src, (বর্ণ*)dst + 8, src_size, (পূর্ণাঙ্ক) dst_alloc_size - 8);
        অন্যথা
                r = sym_LZ4_compress_HC(src, (বর্ণ*)dst + 8, src_size, (পূর্ণাঙ্ক) dst_alloc_size - 8, level);
        যদি (r <= 0)
                প্রত্যাগম -ENOBUFS;

        unaligned_write_le64(dst, src_size);
        *dst_size = r + 8;

        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক compress_blob_zstd(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য *dst,
                size_t dst_alloc_size,
                size_t *dst_size,
                পূর্ণাঙ্ক level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_size > 0);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_ZSTD
        size_t k;
        পূর্ণাঙ্ক r;

        r = dlopen_zstd(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        k = sym_ZSTD_compress(dst, dst_alloc_size, src, src_size, level < 0 ? 0 : level);
        যদি (sym_ZSTD_isError(k))
                প্রত্যাগম zstd_ret_to_errno(k);

        *dst_size = k;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক compress_blob_gzip(ধ্রুব শূন্য *src, uint64_t src_size,
                       শূন্য *dst, size_t dst_alloc_size, size_t *dst_size, পূর্ণাঙ্ক level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_size > 0);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_ZLIB
        পূর্ণাঙ্ক r;

        r = dlopen_zlib(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size > UINT_MAX)
                প্রত্যাগম -EFBIG;
        যদি (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (পূর্ণাঙ্ক) পরিমাণ(s));
        যদি (r != Z_OK)
                প্রত্যাগম -ENOMEM;

        s.next_in = (শূন্য*) src;
        s.avail_in = src_size;
        s.next_out = dst;
        s.avail_out = dst_alloc_size;

        r = sym_deflate(&s, Z_FINISH);
        যদি (r != Z_STREAM_END)
                প্রত্যাগম -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক compress_blob_bzip2(
                ধ্রুব শূন্য *src, uint64_t src_size,
                শূন্য *dst, size_t dst_alloc_size, size_t *dst_size, পূর্ণাঙ্ক level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_size > 0);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_BZIP2
        পূর্ণাঙ্ক r;

        r = dlopen_bzip2(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size > UINT_MAX)
                প্রত্যাগম -EFBIG;
        যদি (dst_alloc_size > UINT_MAX)
                dst_alloc_size = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        যদি (r != BZ_OK)
                প্রত্যাগম -ENOMEM;

        s.next_in = (বর্ণ*) src;
        s.avail_in = src_size;
        s.next_out = (বর্ণ*) dst;
        s.avail_out = dst_alloc_size;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        যদি (r != BZ_STREAM_END)
                প্রত্যাগম -ENOBUFS;

        *dst_size = dst_alloc_size - s.avail_out;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

পূর্ণাঙ্ক compress_blob_journal(
                Compression compression,
                ধ্রুব শূন্য *src, uint64_t src_size,
                শূন্য *dst, size_t dst_alloc_size, size_t *dst_size, পূর্ণাঙ্ক level) {

        বিকল্প (compression) {
        পক্ষ COMPRESSION_XZ:
                প্রত্যাগম compress_blob_xz(src, src_size, dst, dst_alloc_size, dst_size, level);
        পক্ষ COMPRESSION_LZ4:
                প্রত্যাগম compress_blob_lz4(src, src_size, dst, dst_alloc_size, dst_size, level);
        পক্ষ COMPRESSION_ZSTD:
                প্রত্যাগম compress_blob_zstd(src, src_size, dst, dst_alloc_size, dst_size, level);
        স্বভাব:
                প্রত্যাগম -EOPNOTSUPP;
        }
}

পূর্ণাঙ্ক compress_blob(
                Compression compression,
                ধ্রুব শূন্য *src, uint64_t src_size,
                শূন্য *dst, size_t dst_alloc_size, size_t *dst_size, পূর্ণাঙ্ক level) {

        বিকল্প (compression) {
        পক্ষ COMPRESSION_GZIP:
                প্রত্যাগম compress_blob_gzip(src, src_size, dst, dst_alloc_size, dst_size, level);
        পক্ষ COMPRESSION_BZIP2:
                প্রত্যাগম compress_blob_bzip2(src, src_size, dst, dst_alloc_size, dst_size, level);
        স্বভাব:
                প্রত্যাগম compress_blob_journal(compression, src, src_size, dst, dst_alloc_size, dst_size, level);
        }
}

স্থির পূর্ণাঙ্ক decompress_blob_xz(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **dst,
                size_t *dst_size,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_XZ
        পূর্ণাঙ্ক r;

        r = dlopen_xz(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        যদি (ret != LZMA_OK)
                প্রত্যাগম -ENOMEM;

        size_t space = MIN(src_size * 2, dst_max ?: SIZE_MAX);
        যদি (!greedy_realloc(dst, space, 1))
                প্রত্যাগম -ENOMEM;

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *dst;
        s.avail_out = space;

        প্রতি (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                যদি (ret == LZMA_STREAM_END)
                        ভঙ্গ;
                যদি (ret != LZMA_OK)
                        প্রত্যাগম -ENOMEM;

                যদি (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ভঙ্গ;
                যদি (dst_max > 0 && space == dst_max)
                        প্রত্যাগম -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                প্রতিজ্ঞা(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                যদি (!greedy_realloc(dst, space, 1))
                        প্রত্যাগম -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক decompress_blob_lz4(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **dst,
                size_t *dst_size,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_LZ4
        বর্ণ* out;
        পূর্ণাঙ্ক r, size; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size <= 8)
                প্রত্যাগম -EBADMSG;

        যদি (src_size - 8 > INT_MAX)
                প্রত্যাগম -EFBIG;

        size = unaligned_read_le64(src);
        যদি (size < 0 || (unsigned) size != unaligned_read_le64(src))
                প্রত্যাগম -EFBIG;
        যদি (dst_max > 0 && (size_t) size > dst_max)
                প্রত্যাগম -ENOBUFS;
        out = greedy_realloc(dst, size, 1);
        যদি (!out)
                প্রত্যাগম -ENOMEM;

        r = sym_LZ4_decompress_safe((বর্ণ*)src + 8, out, src_size - 8, size);
        যদি (r < 0 || r != size)
                প্রত্যাগম -EBADMSG;

        *dst_size = size;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_size(শূন্য) {
#যদি HAVE_ZSTD
        যদি (dlopen_zstd(LOG_DEBUG) < 0)
                প্রত্যাগম 0;

        প্রত্যাগম sym_ZSTD_DStreamOutSize();
#অন্যথা
        প্রত্যাগম 0;
#endif
}

স্থির পূর্ণাঙ্ক decompress_blob_zstd(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **dst,
                size_t *dst_size,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_ZSTD
        পূর্ণাঙ্ক r;

        r = dlopen_zstd(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        যদি (size == ZSTD_CONTENTSIZE_ERROR)
                প্রত্যাগম -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        যদি (size != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(size, (unsigned long long) cap));
        অন্যথা {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_size <= UINT64_MAX / 2 ? src_size * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStreamOutSize(), (size_t) MIN(twice, (uint64_t) cap));
        }

        যদি (!greedy_realloc(dst, space, 1))
                প্রত্যাগম -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        যদি (!dctx)
                প্রত্যাগম -ENOMEM;

        ZSTD_inBuffer প্রবেশ = {
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

        প্রতি (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &প্রবেশ);
                যদি (sym_ZSTD_isError(k))
                        প্রত্যাগম log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                যদি (output.pos >= cap)
                        ভঙ্গ;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                যদি (k == 0 && প্রবেশ.pos >= প্রবেশ.size)
                        ভঙ্গ;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                যদি (output.pos == output.size) {
                        যদি (space > SIZE_MAX / 2)
                                প্রত্যাগম -E2BIG;
                        space = MIN(2 * space, cap);
                        যদি (!greedy_realloc(dst, space, 1))
                                প্রত্যাগম -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.size = MIN(space, cap);
                        অনুবর্ত;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                যদি (প্রবেশ.pos >= প্রবেশ.size)
                        প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_size = output.pos;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক decompress_blob_gzip(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **dst,
                size_t *dst_size,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_ZLIB
        পূর্ণাঙ্ক r;

        r = dlopen_zlib(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size > UINT_MAX)
                প্রত্যাগম -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (পূর্ণাঙ্ক) পরিমাণ(s));
        যদি (r != Z_OK)
                প্রত্যাগম -ENOMEM;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        যদি (!greedy_realloc(dst, space, 1))
                প্রত্যাগম -ENOMEM;

        s.next_in = (শূন্য*) src;
        s.avail_in = src_size;
        s.next_out = *dst;
        s.avail_out = space;

        প্রতি (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                যদি (r == Z_STREAM_END)
                        ভঙ্গ;
                যদি (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        প্রত্যাগম -EBADMSG;

                যদি (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ভঙ্গ;
                যদি (dst_max > 0 && space == dst_max)
                        প্রত্যাগম -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                যদি (!greedy_realloc(dst, space, 1))
                        প্রত্যাগম -ENOMEM;

                s.avail_out = space - used;
                s.next_out = *(uint8_t**)dst + used;
        }

        *dst_size = space - s.avail_out;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক decompress_blob_bzip2(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **dst,
                size_t *dst_size,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_size);

#যদি HAVE_BZIP2
        পূর্ণাঙ্ক r;

        r = dlopen_bzip2(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size > UINT_MAX)
                প্রত্যাগম -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        যদি (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                প্রত্যাগম -EIO;

        size_t space = MIN3(src_size * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        যদি (!greedy_realloc(dst, space, 1))
                প্রত্যাগম -ENOMEM;

        s.next_in = (বর্ণ*) src;
        s.avail_in = src_size;
        s.next_out = (বর্ণ*) *dst;
        s.avail_out = space;

        প্রতি (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                যদি (r == BZ_STREAM_END)
                        ভঙ্গ;
                যদি (r != BZ_OK)
                        প্রত্যাগম -EBADMSG;

                যদি (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        ভঙ্গ;
                যদি (dst_max > 0 && space == dst_max)
                        প্রত্যাগম -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                যদি (!greedy_realloc(dst, space, 1))
                        প্রত্যাগম -ENOMEM;

                s.avail_out = space - used;
                s.next_out = (বর্ণ*) *dst + used;
        }

        *dst_size = space - s.avail_out;
        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

পূর্ণাঙ্ক decompress_blob_journal(
                Compression compression,
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **dst,
                size_t *dst_size,
                size_t dst_max) {

        বিকল্প (compression) {
        পক্ষ COMPRESSION_XZ:
                প্রত্যাগম decompress_blob_xz(
                                src, src_size,
                                dst, dst_size, dst_max);
        পক্ষ COMPRESSION_LZ4:
                প্রত্যাগম decompress_blob_lz4(
                                src, src_size,
                                dst, dst_size, dst_max);
        পক্ষ COMPRESSION_ZSTD:
                প্রত্যাগম decompress_blob_zstd(
                                src, src_size,
                                dst, dst_size, dst_max);
        স্বভাব:
                প্রত্যাগম -EPROTONOSUPPORT;
        }
}

পূর্ণাঙ্ক decompress_blob(
                Compression compression,
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **dst,
                size_t *dst_size,
                size_t dst_max) {

        বিকল্প (compression) {
        পক্ষ COMPRESSION_GZIP:
                প্রত্যাগম decompress_blob_gzip(
                                src, src_size,
                                dst, dst_size, dst_max);
        পক্ষ COMPRESSION_BZIP2:
                প্রত্যাগম decompress_blob_bzip2(
                                src, src_size,
                                dst, dst_size, dst_max);
        স্বভাব:
                প্রত্যাগম decompress_blob_journal(
                                compression,
                                src, src_size,
                                dst, dst_size, dst_max);
        }
}

পূর্ণাঙ্ক decompress_zlib_raw(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য *dst,
                size_t dst_size,
                পূর্ণাঙ্ক wbits) {

#যদি HAVE_ZLIB
        পূর্ণাঙ্ক r;

        r = dlopen_zlib(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size > UINT_MAX)
                প্রত্যাগম -EFBIG;
        যদি (dst_size > UINT_MAX)
                প্রত্যাগম -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .next_in = (শূন্য*) src,
                .avail_in = src_size,
                .next_out = dst,
                .avail_out = dst_size,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (পূর্ণাঙ্ক) পরিমাণ(s));
        যদি (r != Z_OK)
                প্রত্যাগম -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.next_out - (uint8_t*) dst;

        যদি (r != Z_STREAM_END || produced != dst_size)
                প্রত্যাগম -EBADMSG;

        প্রত্যাগম 0;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক decompress_startswith_xz(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **buffer,
                ধ্রুব শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_XZ
        পূর্ণাঙ্ক r;

        r = dlopen_xz(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        যদি (ret != LZMA_OK)
                প্রত্যাগম -EBADMSG;

        যদি (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                প্রত্যাগম -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = allocated;

        প্রতি (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                যদি (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        প্রত্যাগম -EBADMSG;

                যদি (allocated - s.avail_out >= prefix_len + 1)
                        প্রত্যাগম memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ধ্রুব uint8_t*) *buffer)[prefix_len] == extra;

                যদি (ret == LZMA_STREAM_END)
                        প্রত্যাগম 0;

                s.avail_out += allocated;

                যদি (!(greedy_realloc(buffer, allocated * 2, 1)))
                        প্রত্যাগম -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.next_out = *(uint8_t**)buffer + allocated - s.avail_out;
        }

#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক decompress_startswith_lz4(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **buffer,
                ধ্রুব শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_LZ4
        size_t allocated;
        পূর্ণাঙ্ক r;

        r = dlopen_lz4(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size <= 8)
                প্রত্যাগম -EBADMSG;

        যদি (src_size - 8 > INT_MAX)
                প্রত্যাগম -EFBIG;

        যদি (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                প্রত্যাগম -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*buffer);

        r = sym_LZ4_decompress_safe_partial(
                        (বর্ণ*)src + 8,
                        *buffer,
                        src_size - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        যদি (r < 0 || (size_t) r < prefix_len + 1) {
                size_t size;

                যদি (sym_LZ4_version_সংখ্যা() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        প্রত্যাগম 0;

                যদি (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        যদি (memcmp(*buffer, prefix, r) != 0)
                                প্রত্যাগম 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_size, buffer, &size, 0);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (size < prefix_len + 1)
                        প্রত্যাগম 0;
        }

        প্রত্যাগম memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ধ্রুব uint8_t*) *buffer)[prefix_len] == extra;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক decompress_startswith_zstd(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **buffer,
                ধ্রুব শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_ZSTD
        পূর্ণাঙ্ক r;

        r = dlopen_zstd(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long size = sym_ZSTD_getFrameContentSize(src, src_size);
        যদি (size == ZSTD_CONTENTSIZE_ERROR)
                প্রত্যাগম -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        যদি (size != ZSTD_CONTENTSIZE_UNKNOWN && size < prefix_len + 1)
                প্রত্যাগম 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        যদি (!dctx)
                প্রত্যাগম -ENOMEM;

        যদি (!(greedy_realloc(buffer, MAX(sym_ZSTD_DStreamOutSize(), prefix_len + 1), 1)))
                প্রত্যাগম -ENOMEM;

        ZSTD_inBuffer প্রবেশ = {
                .src = src,
                .size = src_size,
        };
        ZSTD_outBuffer output = {
                .dst = *buffer,
                .size = MALLOC_SIZEOF_SAFE(*buffer),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &প্রবেশ);
        যদি (sym_ZSTD_isError(k))
                প্রত্যাগম log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        যদি (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                যদি (k != 0)
                        প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                প্রত্যাগম 0; /* Decompressed text too short to match the prefix and extra */
        }

        প্রত্যাগম memcmp(*buffer, prefix, prefix_len) == 0 &&
                ((ধ্রুব uint8_t*) *buffer)[prefix_len] == extra;
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক decompress_startswith_gzip(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **buffer,
                ধ্রুব শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_ZLIB
        পূর্ণাঙ্ক r;

        r = dlopen_zlib(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size > UINT_MAX)
                প্রত্যাগম -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (পূর্ণাঙ্ক) পরিমাণ(s));
        যদি (r != Z_OK)
                প্রত্যাগম -EBADMSG;

        যদি (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                প্রত্যাগম -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (শূন্য*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        প্রতি (;;) {
                r = sym_inflate(&s, Z_FINISH);

                যদি (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        প্রত্যাগম -EBADMSG;

                যদি (allocated - s.avail_out >= prefix_len + 1)
                        প্রত্যাগম memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ধ্রুব uint8_t*) *buffer)[prefix_len] == extra;

                যদি (r == Z_STREAM_END)
                        প্রত্যাগম 0;

                যদি (s.avail_out > 0)
                        প্রত্যাগম -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                যদি (!(greedy_realloc(buffer, allocated * 2, 1)))
                        প্রত্যাগম -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = *(uint8_t**)buffer + used;
        }
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণাঙ্ক decompress_startswith_bzip2(
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                শূন্য **buffer,
                ধ্রুব শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_size > 0);
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_BZIP2
        পূর্ণাঙ্ক r;

        r = dlopen_bzip2(LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (src_size > UINT_MAX)
                প্রত্যাগম -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        যদি (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                প্রত্যাগম -EIO;

        যদি (!(greedy_realloc(buffer, ALIGN_8(prefix_len + 1), 1)))
                প্রত্যাগম -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*buffer);

        s.next_in = (বর্ণ*) src;
        s.avail_in = src_size;

        s.next_out = *buffer;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        প্রতি (;;) {
                r = sym_BZ2_bzDecompress(&s);

                যদি (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        প্রত্যাগম -EBADMSG;

                যদি (allocated - s.avail_out >= prefix_len + 1)
                        প্রত্যাগম memcmp(*buffer, prefix, prefix_len) == 0 &&
                                ((ধ্রুব uint8_t*) *buffer)[prefix_len] == extra;

                যদি (r == BZ_STREAM_END)
                        প্রত্যাগম 0;

                যদি (s.avail_out > 0)
                        প্রত্যাগম -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                যদি (!(greedy_realloc(buffer, allocated * 2, 1)))
                        প্রত্যাগম -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*buffer);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.next_out = (বর্ণ*) *buffer + used;
        }
#অন্যথা
        প্রত্যাগম -EPROTONOSUPPORT;
#endif
}

পূর্ণাঙ্ক decompress_startswith_journal(
                Compression compression,
                ধ্রুব শূন্য *src, uint64_t src_size,
                শূন্য **buffer,
                ধ্রুব শূন্য *prefix, size_t prefix_len,
                uint8_t extra) {

        বিকল্প (compression) {
        পক্ষ COMPRESSION_XZ:
                প্রত্যাগম decompress_startswith_xz(src, src_size, buffer, prefix, prefix_len, extra);
        পক্ষ COMPRESSION_LZ4:
                প্রত্যাগম decompress_startswith_lz4(src, src_size, buffer, prefix, prefix_len, extra);
        পক্ষ COMPRESSION_ZSTD:
                প্রত্যাগম decompress_startswith_zstd(src, src_size, buffer, prefix, prefix_len, extra);
        স্বভাব:
                প্রত্যাগম -EOPNOTSUPP;
        }
}

পূর্ণাঙ্ক decompress_startswith(
                Compression compression,
                ধ্রুব শূন্য *src, uint64_t src_size,
                শূন্য **buffer,
                ধ্রুব শূন্য *prefix, size_t prefix_len,
                uint8_t extra) {

        বিকল্প (compression) {
        পক্ষ COMPRESSION_GZIP:
                প্রত্যাগম decompress_startswith_gzip(src, src_size, buffer, prefix, prefix_len, extra);
        পক্ষ COMPRESSION_BZIP2:
                প্রত্যাগম decompress_startswith_bzip2(src, src_size, buffer, prefix, prefix_len, extra);
        স্বভাব:
                প্রত্যাগম decompress_startswith_journal(compression, src, src_size, buffer, prefix, prefix_len, extra);
        }
}

পূর্ণাঙ্ক compress_stream(
                Compression type,
                পূর্ণাঙ্ক fdf, পূর্ণাঙ্ক fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_size) {

        _cleanup_(compressor_freep) Compressor *c = রিক্ত;
        _cleanup_free_ শূন্য *buf = রিক্ত;
        _cleanup_free_ uint8_t *প্রবেশ = রিক্ত;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t total_in = 0, total_out = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fdf >= 0);
        প্রতিজ্ঞা(fdt >= 0);

        r = compressor_new(&c, type);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রবেশ = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        যদি (!প্রবেশ)
                প্রত্যাগম -ENOMEM;

        প্রতি (;;) {
                size_t m = COMPRESS_PIPE_BUFFER_SIZE;
                ssize_t n;

                যদি (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = পঠ(fdf, প্রবেশ, m);
                যদি (n < 0)
                        প্রত্যাগম -errno;

                যদি (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        যদি (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                যদি (r < 0)
                                        প্রত্যাগম r;
                                total_out += buf_size;
                        }
                        ভঙ্গ;
                }

                total_in += n;
                যদি (max_bytes != UINT64_MAX) {
                        প্রতিজ্ঞা(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, প্রবেশ, n, &buf, &buf_size, &buf_alloc);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        যদি (r < 0)
                                প্রত্যাগম r;
                        total_out += buf_size;
                }
        }

        যদি (ret_uncompressed_size)
                *ret_uncompressed_size = total_in;

        যদি (total_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        অন্যথা
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), total_in, total_out, (দ্বিগুণ) total_out / total_in * 100);

        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক should_sparse(পূর্ণাঙ্ক fd) {
        সংরচনা stat st;

        প্রতিজ্ঞা(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        যদি (fstat(fd, &st) < 0)
                প্রত্যাগম -errno;

        যদি (!S_ISREG(st.st_mode))
                প্রত্যাগম false;

        পূর্ণাঙ্ক flags = fcntl(fd, F_GETFL);
        যদি (flags < 0)
                প্রত্যাগম -errno;

        যদি (FLAGS_SET(flags, O_APPEND))
                প্রত্যাগম false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        যদি (pos < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
স্থির পূর্ণাঙ্ক finalize_sparse(পূর্ণাঙ্ক fd) {
        off_t pos;

        প্রতিজ্ঞা(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        যদি (pos < 0)
                প্রত্যাগম -errno;

        যদি (ftruncate(fd, pos) < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম 0;
}

/* Common helper for decompress_stream_*() wrappers */

সংরচনা decompress_stream_userdata {
        পূর্ণাঙ্ক fd;
        uint64_t max_bytes;
        uint64_t total_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

স্থির পূর্ণাঙ্ক decompress_stream_write_callback(ধ্রুব শূন্য *data, size_t size, শূন্য *userdata) {
        সংরচনা decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        যদি (u->max_bytes != UINT64_MAX) {
                যদি (u->max_bytes < size)
                        প্রত্যাগম -EFBIG;
                u->max_bytes -= size;
        }

        u->total_out += size;

        যদি (u->skip > 0) {
                যদি (u->skip >= size) {
                        u->skip -= size;
                        প্রত্যাগম 0;
                }

                data = (ধ্রুব uint8_t*) data + u->skip;
                size -= u->skip;
                u->skip = 0;
        }

        যদি (size == 0)
                প্রত্যাগম 0;

        যদি (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, data, size, 64);
                যদি (k < 0)
                        প্রত্যাগম (পূর্ণাঙ্ক) k;
                প্রত্যাগম 0;
        }

        প্রত্যাগম loop_write(u->fd, data, size);
}

স্থির পূর্ণাঙ্ক decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        প্রতিজ্ঞা(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        যদি (!c)
                প্রত্যাগম -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        বিকল্প (type) {
        পক্ষ COMPRESSION_NONE:
                ভঙ্গ;

#যদি HAVE_XZ
        পক্ষ COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                যদি (xz_ret != LZMA_OK)
                        প্রত্যাগম -EIO;
                ভঙ্গ;
        }
#endif

#যদি HAVE_LZ4
        পক্ষ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                যদি (sym_LZ4F_isError(rc))
                        প্রত্যাগম -ENOMEM;

                ভঙ্গ;
        }
#endif

#যদি HAVE_ZSTD
        পক্ষ COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                যদি (!c->d_zstd)
                        প্রত্যাগম -ENOMEM;
                ভঙ্গ;
#endif

#যদি HAVE_ZLIB
        পক্ষ COMPRESSION_GZIP:
                যদি (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (পূর্ণাঙ্ক) পরিমাণ(c->gzip)) != Z_OK)
                        প্রত্যাগম -EIO;
                ভঙ্গ;
#endif

#যদি HAVE_BZIP2
        পক্ষ COMPRESSION_BZIP2:
                যদি (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        প্রত্যাগম -EIO;
                ভঙ্গ;
#endif

        স্বভাব:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক decompress_blob_to_fd_impl(
                Compression type,
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                পূর্ণাঙ্ক fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(fdt >= 0);

        যদি (type == COMPRESSION_NONE)
                প্রত্যাগম -EOPNOTSUPP;

        যদি (src_size == 0)
                প্রত্যাগম -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        যদি (type == COMPRESSION_LZ4 &&
            src_size >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                যদি (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        যদি (src_size <= পরিমাণ(uint64_t))
                                প্রত্যাগম -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        যদি (sz < skip)
                                প্রত্যাগম -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                যদি (max_bytes == 0)
                        প্রত্যাগম -ENOBUFS;

                _cleanup_free_ শূন্য *buf = রিক্ত;
                size_t size = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_size, &buf, &size,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                যদি (r < 0)
                        প্রত্যাগম r;

                /* Check the result size again for safety. */
                যদি (size < skip)
                        প্রত্যাগম -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, size - skip);
                যদি (r < 0)
                        প্রত্যাগম r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_size, size, (দ্বিগুণ) size / src_size * 100);

                যদি (ret_size)
                        *ret_size = size - skip;
                প্রত্যাগম 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = রিক্ত;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        যদি (r < 0)
                প্রত্যাগম r;

        সংরচনা decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ঠেলো(c, src, src_size, decompress_stream_write_callback, &userdata);
        যদি (r < 0)
                প্রত্যাগম r;

        /* Finalize */
        r = decompressor_ঠেলো(c, /* data= */ রিক্ত, /* size= */ 0, decompress_stream_write_callback, &userdata);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (userdata.total_out < skip)
                প্রত্যাগম -EBADMSG;

        যদি (userdata.sparse) {
                r = finalize_sparse(fdt);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_size, userdata.total_out,
                  (দ্বিগুণ) userdata.total_out / src_size * 100);

        যদি (ret_size)
                *ret_size = userdata.total_out - skip;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক decompress_blob_to_fd_journal(
                Compression type,
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                পূর্ণাঙ্ক fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        পূর্ণাঙ্ক r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

পূর্ণাঙ্ক decompress_blob_to_fd(
                Compression type,
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                পূর্ণাঙ্ক fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size) {

        পূর্ণাঙ্ক r;

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম decompress_blob_to_fd_impl(type, src, src_size, fdt, max_bytes, skip, ret_size);
}

পূর্ণাঙ্ক decompress_stream_full(
                Compression type,
                পূর্ণাঙ্ক fdf, পূর্ণাঙ্ক fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = রিক্ত;
        _cleanup_free_ uint8_t *buf = রিক্ত;
        uint64_t total_in = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fdf >= 0);
        প্রতিজ্ঞা(fdt >= 0);

        যদি (type == COMPRESSION_NONE)
                প্রত্যাগম -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        r = decompressor_new(&c, type, flags);
        যদি (r < 0)
                প্রত্যাগম r;

        সংরচনা decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
        যদি (!buf)
                প্রত্যাগম -ENOMEM;

        প্রতি (;;) {
                ssize_t n;

                n = পঠ(fdf, buf, COMPRESS_PIPE_BUFFER_SIZE);
                যদি (n < 0)
                        প্রত্যাগম -errno;

                total_in += n;

                r = decompressor_ঠেলো(c, buf, n, decompress_stream_write_callback, &userdata);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (n == 0)
                        ভঙ্গ;
        }

        যদি (total_in == 0)
                প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        যদি (userdata.sparse) {
                r = finalize_sparse(fdt);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), total_in, userdata.total_out,
                  (দ্বিগুণ) userdata.total_out / total_in * 100);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক decompress_stream_by_filename(ধ্রুব বর্ণ *filename, পূর্ণাঙ্ক fdf, পূর্ণাঙ্ক fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        যদি (c == COMPRESSION_NONE)
                প্রত্যাগম -EPROTONOSUPPORT;

        প্রত্যাগম decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        যদি (!c)
                প্রত্যাগম রিক্ত;

        বিকল্প (c->type) {

#যদি HAVE_XZ
        পক্ষ COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                ভঙ্গ;
#endif

#যদি HAVE_LZ4
        পক্ষ COMPRESSION_LZ4:
                যদি (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = রিক্ত;
                        c->lz4_header = mfree(c->lz4_header);
                } অন্যথা {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = রিক্ত;
                }
                ভঙ্গ;
#endif

#যদি HAVE_ZSTD
        পক্ষ COMPRESSION_ZSTD:
                যদি (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = রিক্ত;
                } অন্যথা {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = রিক্ত;
                }
                ভঙ্গ;
#endif

#যদি HAVE_ZLIB
        পক্ষ COMPRESSION_GZIP:
                যদি (c->encoding)
                        sym_deflateEnd(&c->gzip);
                অন্যথা
                        sym_inflateEnd(&c->gzip);
                ভঙ্গ;
#endif

#যদি HAVE_BZIP2
        পক্ষ COMPRESSION_BZIP2:
                যদি (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                অন্যথা
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                ভঙ্গ;
#endif

        স্বভাব:
                ভঙ্গ;
        }

        প্রত্যাগম mfree(c);
}

Compression compressor_type(ধ্রুব Compressor *c) {
        প্রত্যাগম c ? c->type : _COMPRESSION_INVALID;
}

পূর্ণাঙ্ক decompressor_detect(Decompressor **decompressor, ধ্রুব শূন্য *data, size_t size) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(decompressor);

        যদি (*decompressor)
                প্রত্যাগম 1;

        যদি (size < COMPRESSION_MAGIC_BYTES_MAX)
                প্রত্যাগম 0;

        প্রতিজ্ঞা(data);

        Compression type = compression_detect_from_magic(data);
        যদি (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক decompressor_force(Decompressor **decompressor, Compression type) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(decompressor);
        প্রতিজ্ঞা(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        *decompressor = compressor_free(*decompressor);
        প্রত্যাগম decompressor_new(decompressor, type, /* flags= */ 0);
}

পূর্ণাঙ্ক decompressor_force_off(Decompressor **decompressor) {
        প্রত্যাগম decompressor_force(decompressor, COMPRESSION_NONE);
}

পূর্ণাঙ্ক decompressor_ঠেলো(Decompressor *c, ধ্রুব শূন্য *data, size_t size, DecompressorCallback callback, শূন্য *userdata) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(c);
        প্রতিজ্ঞা(data || size == 0);
        প্রতিজ্ঞা(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        যদি (c->encoding)
                প্রত্যাগম -EINVAL;

        যদি (c->finished && size == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                প্রত্যাগম 0;

#যদি HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *buffer = রিক্ত;
        যদি (c->type != COMPRESSION_NONE) {
                buffer = new(uint8_t, COMPRESS_PIPE_BUFFER_SIZE);
                যদি (!buffer)
                        প্রত্যাগম -ENOMEM;
        }
#endif

        বিকল্প (c->type) {

        পক্ষ COMPRESSION_NONE:
                যদি (c->finished)
                        প্রত্যাগম -EBADMSG;

                যদি (size == 0) {
                        c->finished = true;
                        প্রত্যাগম 0;
                }

                r = callback(data, size, userdata);
                যদি (r < 0)
                        প্রত্যাগম r;

                প্রত্যাগম 0;

#যদি HAVE_XZ
        পক্ষ COMPRESSION_XZ:
                যদি (c->finished)
                        প্রত্যাগম -EBADMSG;

                c->xz.next_in = data;
                c->xz.avail_in = size;

                প্রতি (;;) {
                        c->xz.next_out = buffer;
                        c->xz.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        lzma_ret lzr = sym_lzma_code(&c->xz, size == 0 ? LZMA_FINISH : LZMA_RUN);
                        যদি (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                প্রত্যাগম -EBADMSG;

                        যদি (c->xz.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                প্রত্যাগম -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->xz.avail_out;
                        যদি (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                যদি (r < 0)
                                        প্রত্যাগম r;
                        }

                        যদি (lzr == LZMA_STREAM_END) {
                                যদি (c->xz.avail_in > 0)
                                        প্রত্যাগম -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                প্রত্যাগম 0;
                        }

                        যদি (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                যদি (size == 0)
                                        প্রত্যাগম -EBADMSG; /* truncated */

                                প্রত্যাগম 0; /* No progress possible with the current input. */
                        }
                }
#endif

#যদি HAVE_LZ4
        পক্ষ COMPRESSION_LZ4: {
                ধ্রুব uint8_t *src = data;
                size_t src_remaining = size;

                c->finished = false;

                প্রতি (;;) {
                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, buffer, &produced, src, &consumed, রিক্ত);
                        যদি (sym_LZ4F_isError(rc))
                                প্রত্যাগম -EBADMSG;

                        যদি (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_BUFFER_SIZE)
                                প্রত্যাগম -EIO;

                        যদি (consumed > 0) {
                                প্রতিজ্ঞা(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        যদি (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                যদি (r < 0)
                                        প্রত্যাগম r;
                        }

                        যদি (src_remaining == 0) {
                                যদি (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        প্রত্যাগম 0;
                                }

                                যদি (produced == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        অনুবর্ত;

                                যদি (size == 0)
                                        প্রত্যাগম -EBADMSG; /* truncated */

                                প্রত্যাগম 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#যদি HAVE_ZSTD
        পক্ষ COMPRESSION_ZSTD: {
                ZSTD_inBuffer প্রবেশ = {
                        .src =  (শূন্য*) data,
                        .size = size,
                };

                c->finished = false;

                প্রতি (;;) {
                        ZSTD_outBuffer output = {
                                .dst = buffer,
                                .size = COMPRESS_PIPE_BUFFER_SIZE,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &প্রবেশ);
                        যদি (sym_ZSTD_isError(res))
                                প্রত্যাগম -EBADMSG;

                        যদি (প্রবেশ.pos > প্রবেশ.size ||
                            output.pos > output.size)
                                প্রত্যাগম -EIO;

                        যদি (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                যদি (r < 0)
                                        প্রত্যাগম r;
                        }

                        যদি (প্রবেশ.pos == প্রবেশ.size) {
                                যদি (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        প্রত্যাগম 0;
                                }

                                যদি (output.pos == COMPRESS_PIPE_BUFFER_SIZE)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        অনুবর্ত;

                                যদি (size == 0)
                                        প্রত্যাগম -EBADMSG; /* truncated */

                                প্রত্যাগম 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#যদি HAVE_ZLIB
        পক্ষ COMPRESSION_GZIP:
                যদি (size > UINT_MAX)
                        প্রত্যাগম -EFBIG;

                c->gzip.next_in = (শূন্য*) data;
                c->gzip.avail_in = size;

                প্রতি (;;) {
                        যদি (c->finished) {
                                যদি (sym_inflateReset(&c->gzip) != Z_OK)
                                        প্রত্যাগম -EIO;

                                c->finished = false;
                        }

                        c->gzip.next_out = buffer;
                        c->gzip.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        পূর্ণাঙ্ক zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        যদি (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                প্রত্যাগম -EBADMSG;

                        যদি (c->gzip.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                প্রত্যাগম -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->gzip.avail_out;
                        যদি (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                যদি (r < 0)
                                        প্রত্যাগম r;
                        }

                        যদি (zr == Z_STREAM_END) {
                                c->finished = true;

                                যদি (c->gzip.avail_in > 0)
                                        অনুবর্ত; /* New stream may be pending. */

                                প্রত্যাগম 0;
                        }

                        যদি (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                যদি (size == 0)
                                        প্রত্যাগম -EBADMSG; /* truncated */

                                প্রত্যাগম 0; /* No progress possible with the current input. */
                        }
                }
#endif

#যদি HAVE_BZIP2
        পক্ষ COMPRESSION_BZIP2:
                যদি (size > UINT_MAX)
                        প্রত্যাগম -EFBIG;

                c->bzip2.next_in = (বর্ণ*) data;
                c->bzip2.avail_in = size;

                প্রতি (;;) {
                        যদি (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                যদি (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        প্রত্যাগম -EIO;

                                c->finished = false;
                        }

                        c->bzip2.next_out = (বর্ণ*) buffer;
                        c->bzip2.avail_out = COMPRESS_PIPE_BUFFER_SIZE;

                        পূর্ণাঙ্ক bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        যদি (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                প্রত্যাগম -EBADMSG;

                        যদি (c->bzip2.avail_out > COMPRESS_PIPE_BUFFER_SIZE)
                                প্রত্যাগম -EIO;

                        size_t produced = COMPRESS_PIPE_BUFFER_SIZE - c->bzip2.avail_out;
                        যদি (produced > 0) {
                                r = callback(buffer, produced, userdata);
                                যদি (r < 0)
                                        প্রত্যাগম r;
                        }

                        যদি (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                যদি (c->bzip2.avail_in > 0)
                                        অনুবর্ত; /* New stream may be pending. */

                                প্রত্যাগম 0;
                        }

                        যদি (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                যদি (size == 0)
                                        প্রত্যাগম -EBADMSG; /* truncated */

                                প্রত্যাগম 0; /* No progress possible with the current input. */
                        }
                }
#endif

        স্বভাব:
                assert_not_reached();
        }
}

পূর্ণাঙ্ক compressor_new(Compressor **ret, Compression type) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                প্রত্যাগম r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        যদি (!c)
                প্রত্যাগম -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        বিকল্প (type) {

#যদি HAVE_XZ
        পক্ষ COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                যদি (xzr != LZMA_OK)
                        প্রত্যাগম -EIO;

                c->type = COMPRESSION_XZ;
                ভঙ্গ;
        }
#endif

#যদি HAVE_LZ4
        পক্ষ COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                যদি (sym_LZ4F_isError(rc))
                        প্রত্যাগম -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                যদি (!c->lz4_header)
                        প্রত্যাগম -ENOMEM;

                c->lz4_header_size = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                যদি (sym_LZ4F_isError(c->lz4_header_size))
                        প্রত্যাগম -EINVAL;

                ভঙ্গ;
        }
#endif

#যদি HAVE_ZSTD
        পক্ষ COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                যদি (!c->c_zstd)
                        প্রত্যাগম -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                যদি (sym_ZSTD_isError(z))
                        প্রত্যাগম -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                যদি (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                ভঙ্গ;
#endif

#যদি HAVE_ZLIB
        পক্ষ COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (পূর্ণাঙ্ক) পরিমাণ(c->gzip));
                যদি (r != Z_OK)
                        প্রত্যাগম -EIO;

                c->type = COMPRESSION_GZIP;
                ভঙ্গ;
#endif

#যদি HAVE_BZIP2
        পক্ষ COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                যদি (r != BZ_OK)
                        প্রত্যাগম -EIO;

                c->type = COMPRESSION_BZIP2;
                ভঙ্গ;
#endif

        পক্ষ COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                ভঙ্গ;

        স্বভাব:
                প্রত্যাগম -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        প্রত্যাগম 0;
}

#যদি HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
স্থির পূর্ণাঙ্ক enlarge_buffer(শূন্য **buffer, size_t *buffer_size, size_t *buffer_allocated, size_t need) {
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(buffer_size);
        প্রতিজ্ঞা(buffer_allocated);

        need = MAX3(need, *buffer_size + 1, (size_t) COMPRESS_PIPE_BUFFER_SIZE);
        যদি (*buffer_allocated >= need)
                প্রত্যাগম 0;

        যদি (!greedy_realloc(buffer, need, 1))
                প্রত্যাগম -ENOMEM;

        *buffer_allocated = MALLOC_SIZEOF_SAFE(*buffer);
        প্রত্যাগম 1;
}
#endif

পূর্ণাঙ্ক compressor_start(
                Compressor *c,
                ধ্রুব শূন্য *data,
                size_t size,
                শূন্য **buffer,
                size_t *buffer_size,
                size_t *buffer_allocated) {

#যদি HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        পূর্ণাঙ্ক r;
#endif

        প্রতিজ্ঞা(c);
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(buffer_size);
        প্রতিজ্ঞা(buffer_allocated);

        যদি (!c->encoding)
                প্রত্যাগম -EINVAL;

        যদি (size == 0)
                প্রত্যাগম 0;

        প্রতিজ্ঞা(data);

        *buffer_size = 0;

        বিকল্প (c->type) {

#যদি HAVE_XZ
        পক্ষ COMPRESSION_XZ:

                c->xz.next_in = data;
                c->xz.avail_in = size;

                যাবত্ (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        যদি (lzr != LZMA_OK)
                                প্রত্যাগম -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                }

                ভঙ্গ;
#endif

#যদি HAVE_LZ4
        পক্ষ COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                যদি (c->lz4_header_size > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, c->lz4_header_size);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        memcpy(*buffer, c->lz4_header, c->lz4_header_size);
                        *buffer_size = c->lz4_header_size;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_size = 0;
                }

                size_t bound = sym_LZ4F_compressBound(size, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, *buffer_size + bound);
                যদি (r < 0)
                        প্রত্যাগম r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *buffer + *buffer_size,
                                                   *buffer_allocated - *buffer_size,
                                                   data, size, রিক্ত);
                যদি (sym_LZ4F_isError(n))
                        প্রত্যাগম -EIO;

                *buffer_size += n;
                ভঙ্গ;
        }
#endif

#যদি HAVE_ZSTD
        পক্ষ COMPRESSION_ZSTD: {
                ZSTD_inBuffer প্রবেশ = {
                        .src = data,
                        .size = size,
                };

                যাবত্ (প্রবেশ.pos < প্রবেশ.size) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &প্রবেশ, ZSTD_e_continue);
                        যদি (sym_ZSTD_isError(res))
                                প্রত্যাগম -EIO;

                        *buffer_size += output.pos;
                }

                ভঙ্গ;
        }
#endif

#যদি HAVE_ZLIB
        পক্ষ COMPRESSION_GZIP:
                যদি (size > UINT_MAX)
                        প্রত্যাগম -EFBIG;

                c->gzip.next_in = (শূন্য*) data;
                c->gzip.avail_in = size;

                যাবত্ (c->gzip.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        যদি (r != Z_OK)
                                প্রত্যাগম -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                }

                ভঙ্গ;
#endif

#যদি HAVE_BZIP2
        পক্ষ COMPRESSION_BZIP2:
                যদি (size > UINT_MAX)
                        প্রত্যাগম -EFBIG;

                c->bzip2.next_in = (শূন্য*) data;
                c->bzip2.avail_in = size;

                যাবত্ (c->bzip2.avail_in > 0) {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (শূন্য*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        যদি (r != BZ_RUN_OK)
                                প্রত্যাগম -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                }

                ভঙ্গ;
#endif

        পক্ষ COMPRESSION_NONE:

                যদি (*buffer_allocated < size) {
                        শূন্য *p;

                        p = realloc(*buffer, size);
                        যদি (!p)
                                প্রত্যাগম -ENOMEM;

                        *buffer = p;
                        *buffer_allocated = size;
                }

                memcpy(*buffer, data, size);
                *buffer_size = size;
                ভঙ্গ;

        স্বভাব:
                প্রত্যাগম -EOPNOTSUPP;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক compressor_finish(Compressor *c, শূন্য **buffer, size_t *buffer_size, size_t *buffer_allocated) {
#যদি HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        পূর্ণাঙ্ক r;
#endif

        প্রতিজ্ঞা(c);
        প্রতিজ্ঞা(buffer);
        প্রতিজ্ঞা(buffer_size);
        প্রতিজ্ঞা(buffer_allocated);

        যদি (!c->encoding)
                প্রত্যাগম -EINVAL;

        *buffer_size = 0;

        বিকল্প (c->type) {

#যদি HAVE_XZ
        পক্ষ COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                কুরু {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        c->xz.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->xz.avail_out = *buffer_allocated - *buffer_size;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        যদি (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                প্রত্যাগম -EIO;

                        *buffer_size += (*buffer_allocated - *buffer_size) - c->xz.avail_out;
                } যাবত্ (lzr != LZMA_STREAM_END);

                ভঙ্গ;
        }
#endif

#যদি HAVE_LZ4
        পক্ষ COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_buffer(buffer, buffer_size, buffer_allocated, bound);
                যদি (r < 0)
                        প্রত্যাগম r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *buffer, *buffer_allocated, রিক্ত);
                যদি (sym_LZ4F_isError(n))
                        প্রত্যাগম -EIO;

                *buffer_size = n;
                ভঙ্গ;
        }
#endif

#যদি HAVE_ZSTD
        পক্ষ COMPRESSION_ZSTD: {
                ZSTD_inBuffer প্রবেশ = {};
                size_t res;

                কুরু {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        ZSTD_outBuffer output = {
                                .dst = ((uint8_t *) *buffer + *buffer_size),
                                .size = *buffer_allocated - *buffer_size,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &প্রবেশ, ZSTD_e_end);
                        যদি (sym_ZSTD_isError(res))
                                প্রত্যাগম -EIO;

                        *buffer_size += output.pos;
                } যাবত্ (res != 0);

                ভঙ্গ;
        }
#endif

#যদি HAVE_ZLIB
        পক্ষ COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                কুরু {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->gzip.next_out = (uint8_t*) *buffer + *buffer_size;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        যদি (!IN_SET(r, Z_OK, Z_STREAM_END))
                                প্রত্যাগম -EIO;

                        *buffer_size += avail - c->gzip.avail_out;
                } যাবত্ (r != Z_STREAM_END);

                ভঙ্গ;
#endif

#যদি HAVE_BZIP2
        পক্ষ COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                কুরু {
                        r = enlarge_buffer(buffer, buffer_size, buffer_allocated, /* need= */ 0);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        size_t avail = MIN(*buffer_allocated - *buffer_size, (size_t) UINT_MAX);
                        c->bzip2.next_out = (শূন্য*) ((uint8_t*) *buffer + *buffer_size);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        যদি (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                প্রত্যাগম -EIO;

                        *buffer_size += avail - c->bzip2.avail_out;
                } যাবত্ (r != BZ_STREAM_END);

                ভঙ্গ;
#endif

        পক্ষ COMPRESSION_NONE:
                ভঙ্গ;

        স্বভাব:
                প্রত্যাগম -EOPNOTSUPP;
        }

        প্রত্যাগম 0;
}
