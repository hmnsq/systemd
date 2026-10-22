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
স্থির DLSYM_PROTOTYPE(lzma_auto_decoder) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(lzma_code) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(lzma_easy_encoder) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(lzma_end) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(lzma_stream_বাফার_encode) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(lzma_stream_decoder) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(lzma_lzma_preset) = ফাঁকা;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
স্থির অন্তঃরেখ শূন্য lzma_end_wrapper(lzma_stream *ls) {
        sym_lzma_end(ls);
}
#endif

#যদি HAVE_LZ4
স্থির DLSYM_PROTOTYPE(LZ4F_compressBegin) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_compressBound) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_compressEnd) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_compressUpdate) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_decompress) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4F_isError) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4_compress_HC) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4_compress_default) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4_decompress_safe) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4_decompress_safe_partial) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(LZ4_version_সংখ্যা) = ফাঁকা;

স্থির ধ্রুবক LZ4F_preferences_t lz4_preferences = {
        .frameInfo.block_আকার_ID = 5,
};
#endif

#যদি HAVE_ZSTD
স্থির DLSYM_PROTOTYPE(ZSTD_CCtx_setParameter) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_compress) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_compressStream2) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_createCCtx) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_createDCtx) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_CStream_In_আকার) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_CStream_Out_আকার) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_decompressStream) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_DStream_In_আকার) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_DStream_Out_আকার) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_freeCCtx) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_freeDCtx) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_getErrorCode) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_getErrorName) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_get_Frame_Content_আকার) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(ZSTD_isError) = ফাঁকা;

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, ফাঁকা);

স্থির পূর্ণসংখ্যা zstd_ret_to_errno(size_t ret) {
        নির্বাচন (sym_ZSTD_getErrorCode(ret)) {
        ক্ষেত্র ZSTD_error_dst_আকার_too_Small:
                ফেরত -ENOBUFS;
        ক্ষেত্র ZSTD_error_memory_allocation:
                ফেরত -ENOMEM;
        পূর্বনির্ধারিত:
                ফেরত -EBADMSG;
        }
}
#endif

#যদি HAVE_ZLIB
স্থির DLSYM_PROTOTYPE(deflateInit2_) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(deflate) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(deflateEnd) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(inflateInit2_) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(inflate) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(inflateEnd) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(inflateReset) = ফাঁকা;

স্থির অন্তঃরেখ শূন্য deflateEnd_wrapper(z_stream *s) {
        sym_deflateEnd(s);
}

স্থির অন্তঃরেখ শূন্য inflateEnd_wrapper(z_stream *s) {
        sym_inflateEnd(s);
}
#endif

#যদি HAVE_BZIP2
স্থির DLSYM_PROTOTYPE(BZ2_bzCompressInit) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(BZ2_bzCompress) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(BZ2_bzCompressEnd) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(BZ2_bzDecompress) = ফাঁকা;
স্থির DLSYM_PROTOTYPE(BZ2_bzDecompressEnd) = ফাঁকা;

স্থির অন্তঃরেখ শূন্য BZ2_bzCompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzCompressEnd(s);
}

স্থির অন্তঃরেখ শূন্য BZ2_bzDecompressEnd_wrapper(bz_stream *s) {
        sym_BZ2_bzDecompressEnd(s);
}
#endif

/* Opaque Compressor/Decompressor struct definition */
কাঠামো Compressor {
        Compression type;
        bool encoding;
        bool finished;
        সংঘ {
#যদি HAVE_XZ
                lzma_stream xz;
#endif
#যদি HAVE_LZ4
                কাঠামো {
                        LZ4F_compressionContext_t c_lz4;
                        শূন্য *lz4_header;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_header_আকার;
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

#define ALIGN_8(l) ALIGN_TO(l, আকার(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#define ZLIB_WBITS_GZIP (15 + 16)

স্থির ধ্রুবক অক্ষর* ধ্রুবক compression_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

স্থির ধ্রুবক অক্ষর* ধ্রুবক compression_uppercase_table[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

স্থির ধ্রুবক অক্ষর* ধ্রুবক compression_extension_table[_COMPRESSION_MAX] = {
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

Compression compression_from_string_harder(ধ্রুবক অক্ষর *s) {
        Compression c;

        প্রতিজ্ঞা(s);

        c = compression_from_string(s);
        যদি (c >= 0)
                ফেরত c;

        ফেরত compression_uppercase_from_string(s);
}

Compression compression_from_filename(ধ্রুবক অক্ষর *filename) {
        Compression c;
        ধ্রুবক অক্ষর *e;

        প্রতিজ্ঞা(filename);

        e = strrchr(filename, '.');
        যদি (!e)
                ফেরত COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        যদি (c < 0)
                ফেরত COMPRESSION_NONE;

        ফেরত c;
}

bool compression_supported_journal(Compression c) {
        স্থির ধ্রুবক unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        প্রতিজ্ঞা(c >= 0);
        প্রতিজ্ঞা(c < _COMPRESSION_MAX);

        ফেরত BIT_SET(supported, c);
}

bool compression_supported(Compression c) {
        স্থির ধ্রুবক unsigned supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        প্রতিজ্ঞা(c >= 0);
        প্রতিজ্ঞা(c < _COMPRESSION_MAX);

        ফেরত BIT_SET(supported, c);
}

Compression compression_detect_from_magic(ধ্রুবক uint8_t তথ্য[স্থির COMPRESSION_MAGIC_BYTES_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        যদি (memcmp(তথ্য, (ধ্রুবক uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                ফেরত COMPRESSION_GZIP;
        যদি (memcmp(তথ্য, (ধ্রুবক uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                ফেরত COMPRESSION_XZ;
        যদি (memcmp(তথ্য, (ধ্রুবক uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                ফেরত COMPRESSION_ZSTD;
        যদি (memcmp(তথ্য, (ধ্রুবক uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                ফেরত COMPRESSION_LZ4;
        যদি (memcmp(তথ্য, (ধ্রুবক uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                ফেরত COMPRESSION_BZIP2;

        ফেরত _COMPRESSION_INVALID;
}

পূর্ণসংখ্যা dlopen_xz(পূর্ণসংখ্যা log_level) {
#যদি HAVE_XZ
        স্থির শূন্য *lzma_dl = ফাঁকা;

        LIBLZMA_NOTE(suggested);

        ফেরত dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_code),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_end),
                        DLSYM_ARG(lzma_stream_বাফার_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_stream_decoder));
#অন্যথা
        ফেরত log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#endif
}

পূর্ণসংখ্যা dlopen_lz4(পূর্ণসংখ্যা log_level) {
#যদি HAVE_LZ4
        স্থির শূন্য *lz4_dl = ফাঁকা;

        LIBLZ4_NOTE(suggested);

        ফেরত dlopen_many_sym_or_warn(
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
        ফেরত log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#endif
}

পূর্ণসংখ্যা dlopen_zstd(পূর্ণসংখ্যা log_level) {
#যদি HAVE_ZSTD
        স্থির শূন্য *zstd_dl = ফাঁকা;

        LIBZSTD_NOTE(suggested);

        ফেরত dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_getErrorCode),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_Frame_Content_আকার),
                        DLSYM_ARG(ZSTD_decompressStream),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_Out_আকার),
                        DLSYM_ARG(ZSTD_CStream_In_আকার),
                        DLSYM_ARG(ZSTD_CStream_Out_আকার),
                        DLSYM_ARG(ZSTD_CCtx_setParameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_In_আকার),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#অন্যথা
        ফেরত log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#endif
}

পূর্ণসংখ্যা dlopen_zlib(পূর্ণসংখ্যা log_level) {
#যদি HAVE_ZLIB
        স্থির শূন্য *zlib_dl = ফাঁকা;

        LIBZ_NOTE(suggested);

        ফেরত dlopen_many_sym_or_warn(
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
        ফেরত log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#endif
}

পূর্ণসংখ্যা dlopen_bzip2(পূর্ণসংখ্যা log_level) {
#যদি HAVE_BZIP2
        স্থির শূন্য *bzip2_dl = ফাঁকা;

        LIBBZ2_NOTE(suggested);

        ফেরত dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bzCompressEnd),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bzDecompressEnd));
#অন্যথা
        ফেরত log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#endif
}

পূর্ণসংখ্যা dlopen_compress_journal(Compression c, পূর্ণসংখ্যা log_level) {
        নির্বাচন (c) {
        ক্ষেত্র COMPRESSION_NONE:
                ফেরত 0;
        ক্ষেত্র COMPRESSION_XZ:
                ফেরত dlopen_xz(log_level);
        ক্ষেত্র COMPRESSION_LZ4:
                ফেরত dlopen_lz4(log_level);
        ক্ষেত্র COMPRESSION_ZSTD:
                ফেরত dlopen_zstd(log_level);
        পূর্বনির্ধারিত:
                ফেরত -EOPNOTSUPP;
        }
}

পূর্ণসংখ্যা dlopen_compress(Compression c, পূর্ণসংখ্যা log_level) {
        নির্বাচন (c) {
        ক্ষেত্র COMPRESSION_GZIP:
                ফেরত dlopen_zlib(log_level);
        ক্ষেত্র COMPRESSION_BZIP2:
                ফেরত dlopen_bzip2(log_level);
        পূর্বনির্ধারিত:
                ফেরত dlopen_compress_journal(c, log_level);
        }
}

স্থির পূর্ণসংখ্যা compress_blob_xz(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য *dst,
                size_t dst_alloc_আকার,
                size_t *dst_আকার,
                পূর্ণসংখ্যা level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_আকার > 0);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, ফাঁকা, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_MODE_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, ফাঁকা }
        };
        lzma_ret ret;
        size_t out_pos = 0;
        পূর্ণসংখ্যা r;

        r = dlopen_xz(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                যদি (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        ফেরত -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        যদি (src_আকার < 80)
                ফেরত -ENOBUFS;

        ret = sym_lzma_stream_বাফার_encode(filters, LZMA_CHECK_NONE, /* allocator= */ ফাঁকা,
                                            src, src_আকার, dst, &out_pos, dst_alloc_আকার);
        যদি (ret != LZMA_OK)
                ফেরত -ENOBUFS;

        *dst_আকার = out_pos;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা compress_blob_lz4(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য *dst,
                size_t dst_alloc_আকার,
                size_t *dst_আকার,
                পূর্ণসংখ্যা level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_আকার > 0);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_LZ4
        পূর্ণসংখ্যা r;

        r = dlopen_lz4(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        যদি (src_আকার < 9)
                ফেরত -ENOBUFS;

        যদি (src_আকার > INT_MAX)
                ফেরত -EFBIG;
        যদি (dst_alloc_আকার > INT_MAX)
                dst_alloc_আকার = INT_MAX;

        যদি (level <= 0)
                r = sym_LZ4_compress_default(src, (অক্ষর*)dst + 8, src_আকার, (পূর্ণসংখ্যা) dst_alloc_আকার - 8);
        অন্যথা
                r = sym_LZ4_compress_HC(src, (অক্ষর*)dst + 8, src_আকার, (পূর্ণসংখ্যা) dst_alloc_আকার - 8, level);
        যদি (r <= 0)
                ফেরত -ENOBUFS;

        unaligned_write_le64(dst, src_আকার);
        *dst_আকার = r + 8;

        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা compress_blob_zstd(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য *dst,
                size_t dst_alloc_আকার,
                size_t *dst_আকার,
                পূর্ণসংখ্যা level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_আকার > 0);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_ZSTD
        size_t k;
        পূর্ণসংখ্যা r;

        r = dlopen_zstd(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        k = sym_ZSTD_compress(dst, dst_alloc_আকার, src, src_আকার, level < 0 ? 0 : level);
        যদি (sym_ZSTD_isError(k))
                ফেরত zstd_ret_to_errno(k);

        *dst_আকার = k;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা compress_blob_gzip(ধ্রুবক শূন্য *src, uint64_t src_আকার,
                       শূন্য *dst, size_t dst_alloc_আকার, size_t *dst_আকার, পূর্ণসংখ্যা level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_আকার > 0);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_ZLIB
        পূর্ণসংখ্যা r;

        r = dlopen_zlib(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার > UINT_MAX)
                ফেরত -EFBIG;
        যদি (dst_alloc_আকার > UINT_MAX)
                dst_alloc_আকার = UINT_MAX;

        _cleanup_(deflateEnd_wrapper) z_stream s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_VERSION, (পূর্ণসংখ্যা) আকার(s));
        যদি (r != Z_OK)
                ফেরত -ENOMEM;

        s.পরবর্তী_in = (শূন্য*) src;
        s.avail_in = src_আকার;
        s.পরবর্তী_out = dst;
        s.avail_out = dst_alloc_আকার;

        r = sym_deflate(&s, Z_FINISH);
        যদি (r != Z_STREAM_END)
                ফেরত -ENOBUFS;

        *dst_আকার = dst_alloc_আকার - s.avail_out;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা compress_blob_bzip2(
                ধ্রুবক শূন্য *src, uint64_t src_আকার,
                শূন্য *dst, size_t dst_alloc_আকার, size_t *dst_আকার, পূর্ণসংখ্যা level) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_alloc_আকার > 0);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_BZIP2
        পূর্ণসংখ্যা r;

        r = dlopen_bzip2(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার > UINT_MAX)
                ফেরত -EFBIG;
        যদি (dst_alloc_আকার > UINT_MAX)
                dst_alloc_আকার = UINT_MAX;

        _cleanup_(BZ2_bzCompressEnd_wrapper) bz_stream s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        যদি (r != BZ_OK)
                ফেরত -ENOMEM;

        s.পরবর্তী_in = (অক্ষর*) src;
        s.avail_in = src_আকার;
        s.পরবর্তী_out = (অক্ষর*) dst;
        s.avail_out = dst_alloc_আকার;

        r = sym_BZ2_bzCompress(&s, BZ_FINISH);

        যদি (r != BZ_STREAM_END)
                ফেরত -ENOBUFS;

        *dst_আকার = dst_alloc_আকার - s.avail_out;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

পূর্ণসংখ্যা compress_blob_journal(
                Compression compression,
                ধ্রুবক শূন্য *src, uint64_t src_আকার,
                শূন্য *dst, size_t dst_alloc_আকার, size_t *dst_আকার, পূর্ণসংখ্যা level) {

        নির্বাচন (compression) {
        ক্ষেত্র COMPRESSION_XZ:
                ফেরত compress_blob_xz(src, src_আকার, dst, dst_alloc_আকার, dst_আকার, level);
        ক্ষেত্র COMPRESSION_LZ4:
                ফেরত compress_blob_lz4(src, src_আকার, dst, dst_alloc_আকার, dst_আকার, level);
        ক্ষেত্র COMPRESSION_ZSTD:
                ফেরত compress_blob_zstd(src, src_আকার, dst, dst_alloc_আকার, dst_আকার, level);
        পূর্বনির্ধারিত:
                ফেরত -EOPNOTSUPP;
        }
}

পূর্ণসংখ্যা compress_blob(
                Compression compression,
                ধ্রুবক শূন্য *src, uint64_t src_আকার,
                শূন্য *dst, size_t dst_alloc_আকার, size_t *dst_আকার, পূর্ণসংখ্যা level) {

        নির্বাচন (compression) {
        ক্ষেত্র COMPRESSION_GZIP:
                ফেরত compress_blob_gzip(src, src_আকার, dst, dst_alloc_আকার, dst_আকার, level);
        ক্ষেত্র COMPRESSION_BZIP2:
                ফেরত compress_blob_bzip2(src, src_আকার, dst, dst_alloc_আকার, dst_আকার, level);
        পূর্বনির্ধারিত:
                ফেরত compress_blob_journal(compression, src, src_আকার, dst, dst_alloc_আকার, dst_আকার, level);
        }
}

স্থির পূর্ণসংখ্যা decompress_blob_xz(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **dst,
                size_t *dst_আকার,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_XZ
        পূর্ণসংখ্যা r;

        r = dlopen_xz(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        যদি (ret != LZMA_OK)
                ফেরত -ENOMEM;

        size_t space = MIN(src_আকার * 2, dst_max ?: SIZE_MAX);
        যদি (!greedy_realloc(dst, space, 1))
                ফেরত -ENOMEM;

        s.পরবর্তী_in = src;
        s.avail_in = src_আকার;

        s.পরবর্তী_out = *dst;
        s.avail_out = space;

        জন্য (;;) {
                size_t used;

                ret = sym_lzma_code(&s, LZMA_FINISH);
                যদি (ret == LZMA_STREAM_END)
                        বিরতি;
                যদি (ret != LZMA_OK)
                        ফেরত -ENOMEM;

                যদি (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        বিরতি;
                যদি (dst_max > 0 && space == dst_max)
                        ফেরত -ENOBUFS;

                used = space - s.avail_out;
                /* Silence static analyzers, space is bounded by allocation size */
                প্রতিজ্ঞা(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                যদি (!greedy_realloc(dst, space, 1))
                        ফেরত -ENOMEM;

                s.avail_out = space - used;
                s.পরবর্তী_out = *(uint8_t**)dst + used;
        }

        *dst_আকার = space - s.avail_out;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা decompress_blob_lz4(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **dst,
                size_t *dst_আকার,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_LZ4
        অক্ষর* out;
        পূর্ণসংখ্যা r, আকার_2; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার <= 8)
                ফেরত -EBADMSG;

        যদি (src_আকার - 8 > INT_MAX)
                ফেরত -EFBIG;

        আকার_2 = unaligned_read_le64(src);
        যদি (আকার_2 < 0 || (unsigned) আকার_2 != unaligned_read_le64(src))
                ফেরত -EFBIG;
        যদি (dst_max > 0 && (size_t) আকার_2 > dst_max)
                ফেরত -ENOBUFS;
        out = greedy_realloc(dst, আকার_2, 1);
        যদি (!out)
                ফেরত -ENOMEM;

        r = sym_LZ4_decompress_safe((অক্ষর*)src + 8, out, src_আকার - 8, আকার_2);
        যদি (r < 0 || r != আকার_2)
                ফেরত -EBADMSG;

        *dst_আকার = আকার_2;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

size_t zstd_dstream_out_আকার(শূন্য) {
#যদি HAVE_ZSTD
        যদি (dlopen_zstd(LOG_DEBUG) < 0)
                ফেরত 0;

        ফেরত sym_ZSTD_DStream_Out_আকার();
#অন্যথা
        ফেরত 0;
#endif
}

স্থির পূর্ণসংখ্যা decompress_blob_zstd(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **dst,
                size_t *dst_আকার,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_ZSTD
        পূর্ণসংখ্যা r;

        r = dlopen_zstd(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        unsigned long long আকার_2 = sym_ZSTD_get_Frame_Content_আকার(src, src_আকার);
        যদি (আকার_2 == ZSTD_CONTENTSIZE_ERROR)
                ফেরত -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        যদি (আকার_2 != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_Out_আকার(), (size_t) MIN(আকার_2, (unsigned long long) cap));
        অন্যথা {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_আকার <= UINT64_MAX / 2 ? src_আকার * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_Out_আকার(), (size_t) MIN(twice, (uint64_t) cap));
        }

        যদি (!greedy_realloc(dst, space, 1))
                ফেরত -ENOMEM;
        space = MALLOC_SIZEOF_SAFE(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        যদি (!dctx)
                ফেরত -ENOMEM;

        ZSTD_in_বাফার প্রবেশ = {
                .src = src,
                .আকার_2 = src_আকার,
        };
        ZSTD_out_বাফার output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .আকার_2 = MIN(space, cap),
        };

        জন্য (;;) {
                size_t k = sym_ZSTD_decompressStream(dctx, &output, &প্রবেশ);
                যদি (sym_ZSTD_isError(k))
                        ফেরত log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                যদি (output.pos >= cap)
                        বিরতি;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                যদি (k == 0 && প্রবেশ.pos >= প্রবেশ.আকার_2)
                        বিরতি;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                যদি (output.pos == output.আকার_2) {
                        যদি (space > SIZE_MAX / 2)
                                ফেরত -E2BIG;
                        space = MIN(2 * space, cap);
                        যদি (!greedy_realloc(dst, space, 1))
                                ফেরত -ENOMEM;
                        space = MALLOC_SIZEOF_SAFE(*dst);
                        output.dst = *dst;
                        output.আকার_2 = MIN(space, cap);
                        চালু;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                যদি (প্রবেশ.pos >= প্রবেশ.আকার_2)
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_আকার = output.pos;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা decompress_blob_gzip(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **dst,
                size_t *dst_আকার,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_ZLIB
        পূর্ণসংখ্যা r;

        r = dlopen_zlib(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার > UINT_MAX)
                ফেরত -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (পূর্ণসংখ্যা) আকার(s));
        যদি (r != Z_OK)
                ফেরত -ENOMEM;

        size_t space = MIN3(src_আকার * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        যদি (!greedy_realloc(dst, space, 1))
                ফেরত -ENOMEM;

        s.পরবর্তী_in = (শূন্য*) src;
        s.avail_in = src_আকার;
        s.পরবর্তী_out = *dst;
        s.avail_out = space;

        জন্য (;;) {
                size_t used;

                r = sym_inflate(&s, Z_NO_FLUSH);
                যদি (r == Z_STREAM_END)
                        বিরতি;
                যদি (!IN_SET(r, Z_OK, Z_BUF_ERROR))
                        ফেরত -EBADMSG;

                যদি (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        বিরতি;
                যদি (dst_max > 0 && space == dst_max)
                        ফেরত -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                যদি (!greedy_realloc(dst, space, 1))
                        ফেরত -ENOMEM;

                s.avail_out = space - used;
                s.পরবর্তী_out = *(uint8_t**)dst + used;
        }

        *dst_আকার = space - s.avail_out;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা decompress_blob_bzip2(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **dst,
                size_t *dst_আকার,
                size_t dst_max) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(dst);
        প্রতিজ্ঞা(dst_আকার);

#যদি HAVE_BZIP2
        পূর্ণসংখ্যা r;

        r = dlopen_bzip2(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার > UINT_MAX)
                ফেরত -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        যদি (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ফেরত -EIO;

        size_t space = MIN3(src_আকার * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        যদি (!greedy_realloc(dst, space, 1))
                ফেরত -ENOMEM;

        s.পরবর্তী_in = (অক্ষর*) src;
        s.avail_in = src_আকার;
        s.পরবর্তী_out = (অক্ষর*) *dst;
        s.avail_out = space;

        জন্য (;;) {
                size_t used;

                r = sym_BZ2_bzDecompress(&s);
                যদি (r == BZ_STREAM_END)
                        বিরতি;
                যদি (r != BZ_OK)
                        ফেরত -EBADMSG;

                যদি (dst_max > 0 && (space - s.avail_out) >= dst_max)
                        বিরতি;
                যদি (dst_max > 0 && space == dst_max)
                        ফেরত -ENOBUFS;

                used = space - s.avail_out;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                যদি (!greedy_realloc(dst, space, 1))
                        ফেরত -ENOMEM;

                s.avail_out = space - used;
                s.পরবর্তী_out = (অক্ষর*) *dst + used;
        }

        *dst_আকার = space - s.avail_out;
        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

পূর্ণসংখ্যা decompress_blob_journal(
                Compression compression,
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **dst,
                size_t *dst_আকার,
                size_t dst_max) {

        নির্বাচন (compression) {
        ক্ষেত্র COMPRESSION_XZ:
                ফেরত decompress_blob_xz(
                                src, src_আকার,
                                dst, dst_আকার, dst_max);
        ক্ষেত্র COMPRESSION_LZ4:
                ফেরত decompress_blob_lz4(
                                src, src_আকার,
                                dst, dst_আকার, dst_max);
        ক্ষেত্র COMPRESSION_ZSTD:
                ফেরত decompress_blob_zstd(
                                src, src_আকার,
                                dst, dst_আকার, dst_max);
        পূর্বনির্ধারিত:
                ফেরত -EPROTONOSUPPORT;
        }
}

পূর্ণসংখ্যা decompress_blob(
                Compression compression,
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **dst,
                size_t *dst_আকার,
                size_t dst_max) {

        নির্বাচন (compression) {
        ক্ষেত্র COMPRESSION_GZIP:
                ফেরত decompress_blob_gzip(
                                src, src_আকার,
                                dst, dst_আকার, dst_max);
        ক্ষেত্র COMPRESSION_BZIP2:
                ফেরত decompress_blob_bzip2(
                                src, src_আকার,
                                dst, dst_আকার, dst_max);
        পূর্বনির্ধারিত:
                ফেরত decompress_blob_journal(
                                compression,
                                src, src_আকার,
                                dst, dst_আকার, dst_max);
        }
}

পূর্ণসংখ্যা decompress_zlib_raw(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য *dst,
                size_t dst_আকার,
                পূর্ণসংখ্যা wbits) {

#যদি HAVE_ZLIB
        পূর্ণসংখ্যা r;

        r = dlopen_zlib(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার > UINT_MAX)
                ফেরত -EFBIG;
        যদি (dst_আকার > UINT_MAX)
                ফেরত -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {
                .পরবর্তী_in = (শূন্য*) src,
                .avail_in = src_আকার,
                .পরবর্তী_out = dst,
                .avail_out = dst_আকার,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_VERSION, (পূর্ণসংখ্যা) আকার(s));
        যদি (r != Z_OK)
                ফেরত -EIO;

        r = sym_inflate(&s, Z_FINISH);
        size_t produced = (uint8_t*) s.পরবর্তী_out - (uint8_t*) dst;

        যদি (r != Z_STREAM_END || produced != dst_আকার)
                ফেরত -EBADMSG;

        ফেরত 0;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা decompress_startswith_xz(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **বাফার,
                ধ্রুবক শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_XZ
        পূর্ণসংখ্যা r;

        r = dlopen_xz(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        _cleanup_(lzma_end_wrapper) lzma_stream s = LZMA_STREAM_INIT;
        lzma_ret ret = sym_lzma_stream_decoder(&s, UINT64_MAX, /* flags= */ 0);
        যদি (ret != LZMA_OK)
                ফেরত -EBADMSG;

        যদি (!(greedy_realloc(বাফার, ALIGN_8(prefix_len + 1), 1)))
                ফেরত -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*বাফার);

        s.পরবর্তী_in = src;
        s.avail_in = src_আকার;

        s.পরবর্তী_out = *বাফার;
        s.avail_out = allocated;

        জন্য (;;) {
                ret = sym_lzma_code(&s, LZMA_FINISH);

                যদি (!IN_SET(ret, LZMA_OK, LZMA_STREAM_END))
                        ফেরত -EBADMSG;

                যদি (allocated - s.avail_out >= prefix_len + 1)
                        ফেরত memcmp(*বাফার, prefix, prefix_len) == 0 &&
                                ((ধ্রুবক uint8_t*) *বাফার)[prefix_len] == extra;

                যদি (ret == LZMA_STREAM_END)
                        ফেরত 0;

                s.avail_out += allocated;

                যদি (!(greedy_realloc(বাফার, allocated * 2, 1)))
                        ফেরত -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*বাফার);
                s.পরবর্তী_out = *(uint8_t**)বাফার + allocated - s.avail_out;
        }

#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা decompress_startswith_lz4(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **বাফার,
                ধ্রুবক শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_LZ4
        size_t allocated;
        পূর্ণসংখ্যা r;

        r = dlopen_lz4(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার <= 8)
                ফেরত -EBADMSG;

        যদি (src_আকার - 8 > INT_MAX)
                ফেরত -EFBIG;

        যদি (!(greedy_realloc(বাফার, ALIGN_8(prefix_len + 1), 1)))
                ফেরত -ENOMEM;
        allocated = MALLOC_SIZEOF_SAFE(*বাফার);

        r = sym_LZ4_decompress_safe_partial(
                        (অক্ষর*)src + 8,
                        *বাফার,
                        src_আকার - 8,
                        prefix_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        যদি (r < 0 || (size_t) r < prefix_len + 1) {
                size_t আকার_2;

                যদি (sym_LZ4_version_সংখ্যা() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        ফেরত 0;

                যদি (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        যদি (memcmp(*বাফার, prefix, r) != 0)
                                ফেরত 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_আকার, বাফার, &আকার_2, 0);
                যদি (r < 0)
                        ফেরত r;

                যদি (আকার_2 < prefix_len + 1)
                        ফেরত 0;
        }

        ফেরত memcmp(*বাফার, prefix, prefix_len) == 0 &&
                ((ধ্রুবক uint8_t*) *বাফার)[prefix_len] == extra;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা decompress_startswith_zstd(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **বাফার,
                ধ্রুবক শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_ZSTD
        পূর্ণসংখ্যা r;

        r = dlopen_zstd(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        unsigned long long আকার_2 = sym_ZSTD_get_Frame_Content_আকার(src, src_আকার);
        যদি (আকার_2 == ZSTD_CONTENTSIZE_ERROR)
                ফেরত -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        যদি (আকার_2 != ZSTD_CONTENTSIZE_UNKNOWN && আকার_2 < prefix_len + 1)
                ফেরত 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        যদি (!dctx)
                ফেরত -ENOMEM;

        যদি (!(greedy_realloc(বাফার, MAX(sym_ZSTD_DStream_Out_আকার(), prefix_len + 1), 1)))
                ফেরত -ENOMEM;

        ZSTD_in_বাফার প্রবেশ = {
                .src = src,
                .আকার_2 = src_আকার,
        };
        ZSTD_out_বাফার output = {
                .dst = *বাফার,
                .আকার_2 = MALLOC_SIZEOF_SAFE(*বাফার),
        };
        size_t k;

        k = sym_ZSTD_decompressStream(dctx, &output, &প্রবেশ);
        যদি (sym_ZSTD_isError(k))
                ফেরত log_debug_errno(zstd_ret_to_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        যদি (output.pos < prefix_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                যদি (k != 0)
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                ফেরত 0; /* Decompressed text too short to match the prefix and extra */
        }

        ফেরত memcmp(*বাফার, prefix, prefix_len) == 0 &&
                ((ধ্রুবক uint8_t*) *বাফার)[prefix_len] == extra;
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা decompress_startswith_gzip(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **বাফার,
                ধ্রুবক শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_ZLIB
        পূর্ণসংখ্যা r;

        r = dlopen_zlib(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার > UINT_MAX)
                ফেরত -EFBIG;

        _cleanup_(inflateEnd_wrapper) z_stream s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (পূর্ণসংখ্যা) আকার(s));
        যদি (r != Z_OK)
                ফেরত -EBADMSG;

        যদি (!(greedy_realloc(বাফার, ALIGN_8(prefix_len + 1), 1)))
                ফেরত -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*বাফার);

        s.পরবর্তী_in = (শূন্য*) src;
        s.avail_in = src_আকার;

        s.পরবর্তী_out = *বাফার;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        জন্য (;;) {
                r = sym_inflate(&s, Z_FINISH);

                যদি (!IN_SET(r, Z_OK, Z_STREAM_END, Z_BUF_ERROR))
                        ফেরত -EBADMSG;

                যদি (allocated - s.avail_out >= prefix_len + 1)
                        ফেরত memcmp(*বাফার, prefix, prefix_len) == 0 &&
                                ((ধ্রুবক uint8_t*) *বাফার)[prefix_len] == extra;

                যদি (r == Z_STREAM_END)
                        ফেরত 0;

                যদি (s.avail_out > 0)
                        ফেরত -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                যদি (!(greedy_realloc(বাফার, allocated * 2, 1)))
                        ফেরত -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*বাফার);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.পরবর্তী_out = *(uint8_t**)বাফার + used;
        }
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

স্থির পূর্ণসংখ্যা decompress_startswith_bzip2(
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                শূন্য **বাফার,
                ধ্রুবক শূন্য *prefix,
                size_t prefix_len,
                uint8_t extra) {

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(src_আকার > 0);
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(prefix);

#যদি HAVE_BZIP2
        পূর্ণসংখ্যা r;

        r = dlopen_bzip2(LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        যদি (src_আকার > UINT_MAX)
                ফেরত -EFBIG;

        _cleanup_(BZ2_bzDecompressEnd_wrapper) bz_stream s = {};

        যদি (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                ফেরত -EIO;

        যদি (!(greedy_realloc(বাফার, ALIGN_8(prefix_len + 1), 1)))
                ফেরত -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_SAFE(*বাফার);

        s.পরবর্তী_in = (অক্ষর*) src;
        s.avail_in = src_আকার;

        s.পরবর্তী_out = *বাফার;
        s.avail_out = MIN(allocated, (size_t) UINT_MAX);

        জন্য (;;) {
                r = sym_BZ2_bzDecompress(&s);

                যদি (!IN_SET(r, BZ_OK, BZ_STREAM_END))
                        ফেরত -EBADMSG;

                যদি (allocated - s.avail_out >= prefix_len + 1)
                        ফেরত memcmp(*বাফার, prefix, prefix_len) == 0 &&
                                ((ধ্রুবক uint8_t*) *বাফার)[prefix_len] == extra;

                যদি (r == BZ_STREAM_END)
                        ফেরত 0;

                যদি (s.avail_out > 0)
                        ফেরত -EBADMSG; /* truncated */

                size_t used = allocated - s.avail_out;

                যদি (!(greedy_realloc(বাফার, allocated * 2, 1)))
                        ফেরত -ENOMEM;

                allocated = MALLOC_SIZEOF_SAFE(*বাফার);
                s.avail_out = MIN(allocated - used, (size_t) UINT_MAX);
                s.পরবর্তী_out = (অক্ষর*) *বাফার + used;
        }
#অন্যথা
        ফেরত -EPROTONOSUPPORT;
#endif
}

পূর্ণসংখ্যা decompress_startswith_journal(
                Compression compression,
                ধ্রুবক শূন্য *src, uint64_t src_আকার,
                শূন্য **বাফার,
                ধ্রুবক শূন্য *prefix, size_t prefix_len,
                uint8_t extra) {

        নির্বাচন (compression) {
        ক্ষেত্র COMPRESSION_XZ:
                ফেরত decompress_startswith_xz(src, src_আকার, বাফার, prefix, prefix_len, extra);
        ক্ষেত্র COMPRESSION_LZ4:
                ফেরত decompress_startswith_lz4(src, src_আকার, বাফার, prefix, prefix_len, extra);
        ক্ষেত্র COMPRESSION_ZSTD:
                ফেরত decompress_startswith_zstd(src, src_আকার, বাফার, prefix, prefix_len, extra);
        পূর্বনির্ধারিত:
                ফেরত -EOPNOTSUPP;
        }
}

পূর্ণসংখ্যা decompress_startswith(
                Compression compression,
                ধ্রুবক শূন্য *src, uint64_t src_আকার,
                শূন্য **বাফার,
                ধ্রুবক শূন্য *prefix, size_t prefix_len,
                uint8_t extra) {

        নির্বাচন (compression) {
        ক্ষেত্র COMPRESSION_GZIP:
                ফেরত decompress_startswith_gzip(src, src_আকার, বাফার, prefix, prefix_len, extra);
        ক্ষেত্র COMPRESSION_BZIP2:
                ফেরত decompress_startswith_bzip2(src, src_আকার, বাফার, prefix, prefix_len, extra);
        পূর্বনির্ধারিত:
                ফেরত decompress_startswith_journal(compression, src, src_আকার, বাফার, prefix, prefix_len, extra);
        }
}

পূর্ণসংখ্যা compress_stream(
                Compression type,
                পূর্ণসংখ্যা fdf, পূর্ণসংখ্যা fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_আকার) {

        _cleanup_(compressor_freep) Compressor *c = ফাঁকা;
        _cleanup_free_ শূন্য *buf = ফাঁকা;
        _cleanup_free_ uint8_t *প্রবেশ = ফাঁকা;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t যোগ_in = 0, যোগ_out = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fdf >= 0);
        প্রতিজ্ঞা(fdt >= 0);

        r = compressor_new(&c, type);
        যদি (r < 0)
                ফেরত r;

        প্রবেশ = new(uint8_t, COMPRESS_PIPE_বাফার_আকার);
        যদি (!প্রবেশ)
                ফেরত -ENOMEM;

        জন্য (;;) {
                size_t m = COMPRESS_PIPE_বাফার_আকার;
                ssize_t n;

                যদি (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = পঠ(fdf, প্রবেশ, m);
                যদি (n < 0)
                        ফেরত -errno;

                যদি (n == 0) {
                        r = compressor_finish(c, &buf, &buf_size, &buf_alloc);
                        যদি (r < 0)
                                ফেরত r;

                        যদি (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                যদি (r < 0)
                                        ফেরত r;
                                যোগ_out += buf_size;
                        }
                        বিরতি;
                }

                যোগ_in += n;
                যদি (max_bytes != UINT64_MAX) {
                        প্রতিজ্ঞা(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_start(c, প্রবেশ, n, &buf, &buf_size, &buf_alloc);
                যদি (r < 0)
                        ফেরত r;

                যদি (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        যদি (r < 0)
                                ফেরত r;
                        যোগ_out += buf_size;
                }
        }

        যদি (ret_uncompressed_আকার)
                *ret_uncompressed_আকার = যোগ_in;

        যদি (যোগ_in == 0)
                log_debug("%s compression finished (no input data)", compression_to_string(type));
        অন্যথা
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_to_string(type), যোগ_in, যোগ_out, (দ্বিগুণ) যোগ_out / যোগ_in * 100);

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা should_sparse(পূর্ণসংখ্যা fd) {
        কাঠামো stat st;

        প্রতিজ্ঞা(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        যদি (fstat(fd, &st) < 0)
                ফেরত -errno;

        যদি (!S_ISREG(st.st_mode))
                ফেরত false;

        পূর্ণসংখ্যা flags = fcntl(fd, F_GETFL);
        যদি (flags < 0)
                ফেরত -errno;

        যদি (FLAGS_SET(flags, O_APPEND))
                ফেরত false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        যদি (pos < 0)
                ফেরত -errno;

        ফেরত pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
স্থির পূর্ণসংখ্যা finalize_sparse(পূর্ণসংখ্যা fd) {
        off_t pos;

        প্রতিজ্ঞা(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        যদি (pos < 0)
                ফেরত -errno;

        যদি (ftruncate(fd, pos) < 0)
                ফেরত -errno;

        ফেরত 0;
}

/* Common helper for decompress_stream_*() wrappers */

কাঠামো decompress_stream_userdata {
        পূর্ণসংখ্যা fd;
        uint64_t max_bytes;
        uint64_t যোগ_out;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

স্থির পূর্ণসংখ্যা decompress_stream_write_callback(ধ্রুবক শূন্য *তথ্য, size_t আকার_2, শূন্য *userdata) {
        কাঠামো decompress_stream_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        যদি (u->max_bytes != UINT64_MAX) {
                যদি (u->max_bytes < আকার_2)
                        ফেরত -EFBIG;
                u->max_bytes -= আকার_2;
        }

        u->যোগ_out += আকার_2;

        যদি (u->skip > 0) {
                যদি (u->skip >= আকার_2) {
                        u->skip -= আকার_2;
                        ফেরত 0;
                }

                তথ্য = (ধ্রুবক uint8_t*) তথ্য + u->skip;
                আকার_2 -= u->skip;
                u->skip = 0;
        }

        যদি (আকার_2 == 0)
                ফেরত 0;

        যদি (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, তথ্য, আকার_2, 64);
                যদি (k < 0)
                        ফেরত (পূর্ণসংখ্যা) k;
                ফেরত 0;
        }

        ফেরত loop_write(u->fd, তথ্য, আকার_2);
}

স্থির পূর্ণসংখ্যা decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        প্রতিজ্ঞা(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        যদি (!c)
                ফেরত -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        নির্বাচন (type) {
        ক্ষেত্র COMPRESSION_NONE:
                বিরতি;

#যদি HAVE_XZ
        ক্ষেত্র COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_SET(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_SET(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_stream_decoder(&c->xz, UINT64_MAX, xz_flags);
                যদি (xz_ret != LZMA_OK)
                        ফেরত -EIO;
                বিরতি;
        }
#endif

#যদি HAVE_LZ4
        ক্ষেত্র COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4F_VERSION);
                যদি (sym_LZ4F_isError(rc))
                        ফেরত -ENOMEM;

                বিরতি;
        }
#endif

#যদি HAVE_ZSTD
        ক্ষেত্র COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                যদি (!c->d_zstd)
                        ফেরত -ENOMEM;
                বিরতি;
#endif

#যদি HAVE_ZLIB
        ক্ষেত্র COMPRESSION_GZIP:
                যদি (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_VERSION, (পূর্ণসংখ্যা) আকার(c->gzip)) != Z_OK)
                        ফেরত -EIO;
                বিরতি;
#endif

#যদি HAVE_BZIP2
        ক্ষেত্র COMPRESSION_BZIP2:
                যদি (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        ফেরত -EIO;
                বিরতি;
#endif

        পূর্বনির্ধারিত:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা decompress_blob_to_fd_impl(
                Compression type,
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                পূর্ণসংখ্যা fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_আকার) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(src);
        প্রতিজ্ঞা(fdt >= 0);

        যদি (type == COMPRESSION_NONE)
                ফেরত -EOPNOTSUPP;

        যদি (src_আকার == 0)
                ফেরত -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        যদি (type == COMPRESSION_LZ4 &&
            src_আকার >= COMPRESSION_MAGIC_BYTES_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                যদি (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        যদি (src_আকার <= আকার(uint64_t))
                                ফেরত -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        যদি (sz < skip)
                                ফেরত -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                যদি (max_bytes == 0)
                        ফেরত -ENOBUFS;

                _cleanup_free_ শূন্য *buf = ফাঁকা;
                size_t আকার_2 = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_আকার, &buf, &আকার_2,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                যদি (r < 0)
                        ফেরত r;

                /* Check the result size again for safety. */
                যদি (আকার_2 < skip)
                        ফেরত -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, আকার_2 - skip);
                যদি (r < 0)
                        ফেরত r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_আকার, আকার_2, (দ্বিগুণ) আকার_2 / src_আকার * 100);

                যদি (ret_আকার)
                        *ret_আকার = আকার_2 - skip;
                ফেরত 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = ফাঁকা;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        যদি (r < 0)
                ফেরত r;

        কাঠামো decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_ঠেলো(c, src, src_আকার, decompress_stream_write_callback, &userdata);
        যদি (r < 0)
                ফেরত r;

        /* Finalize */
        r = decompressor_ঠেলো(c, /* data= */ ফাঁকা, /* size= */ 0, decompress_stream_write_callback, &userdata);
        যদি (r < 0)
                ফেরত r;

        যদি (userdata.যোগ_out < skip)
                ফেরত -EBADMSG;

        যদি (userdata.sparse) {
                r = finalize_sparse(fdt);
                যদি (r < 0)
                        ফেরত r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), src_আকার, userdata.যোগ_out,
                  (দ্বিগুণ) userdata.যোগ_out / src_আকার * 100);

        যদি (ret_আকার)
                *ret_আকার = userdata.যোগ_out - skip;

        ফেরত 0;
}

পূর্ণসংখ্যা decompress_blob_to_fd_journal(
                Compression type,
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                পূর্ণসংখ্যা fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_আকার) {

        পূর্ণসংখ্যা r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        ফেরত decompress_blob_to_fd_impl(type, src, src_আকার, fdt, max_bytes, skip, ret_আকার);
}

পূর্ণসংখ্যা decompress_blob_to_fd(
                Compression type,
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                পূর্ণসংখ্যা fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_আকার) {

        পূর্ণসংখ্যা r;

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        ফেরত decompress_blob_to_fd_impl(type, src, src_আকার, fdt, max_bytes, skip, ret_আকার);
}

পূর্ণসংখ্যা decompress_stream_full(
                Compression type,
                পূর্ণসংখ্যা fdf, পূর্ণসংখ্যা fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = ফাঁকা;
        _cleanup_free_ uint8_t *buf = ফাঁকা;
        uint64_t যোগ_in = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fdf >= 0);
        প্রতিজ্ঞা(fdt >= 0);

        যদি (type == COMPRESSION_NONE)
                ফেরত -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        r = decompressor_new(&c, type, flags);
        যদি (r < 0)
                ফেরত r;

        কাঠামো decompress_stream_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_বাফার_আকার);
        যদি (!buf)
                ফেরত -ENOMEM;

        জন্য (;;) {
                ssize_t n;

                n = পঠ(fdf, buf, COMPRESS_PIPE_বাফার_আকার);
                যদি (n < 0)
                        ফেরত -errno;

                যোগ_in += n;

                r = decompressor_ঠেলো(c, buf, n, decompress_stream_write_callback, &userdata);
                যদি (r < 0)
                        ফেরত r;

                যদি (n == 0)
                        বিরতি;
        }

        যদি (যোগ_in == 0)
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_to_string(type));

        যদি (userdata.sparse) {
                r = finalize_sparse(fdt);
                যদি (r < 0)
                        ফেরত r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_to_string(type), যোগ_in, userdata.যোগ_out,
                  (দ্বিগুণ) userdata.যোগ_out / যোগ_in * 100);

        ফেরত 0;
}

পূর্ণসংখ্যা decompress_stream_by_filename(ধ্রুবক অক্ষর *filename, পূর্ণসংখ্যা fdf, পূর্ণসংখ্যা fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        যদি (c == COMPRESSION_NONE)
                ফেরত -EPROTONOSUPPORT;

        ফেরত decompress_stream(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        যদি (!c)
                ফেরত ফাঁকা;

        নির্বাচন (c->type) {

#যদি HAVE_XZ
        ক্ষেত্র COMPRESSION_XZ:
                sym_lzma_end(&c->xz);
                বিরতি;
#endif

#যদি HAVE_LZ4
        ক্ষেত্র COMPRESSION_LZ4:
                যদি (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = ফাঁকা;
                        c->lz4_header = mfree(c->lz4_header);
                } অন্যথা {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = ফাঁকা;
                }
                বিরতি;
#endif

#যদি HAVE_ZSTD
        ক্ষেত্র COMPRESSION_ZSTD:
                যদি (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = ফাঁকা;
                } অন্যথা {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = ফাঁকা;
                }
                বিরতি;
#endif

#যদি HAVE_ZLIB
        ক্ষেত্র COMPRESSION_GZIP:
                যদি (c->encoding)
                        sym_deflateEnd(&c->gzip);
                অন্যথা
                        sym_inflateEnd(&c->gzip);
                বিরতি;
#endif

#যদি HAVE_BZIP2
        ক্ষেত্র COMPRESSION_BZIP2:
                যদি (c->encoding)
                        sym_BZ2_bzCompressEnd(&c->bzip2);
                অন্যথা
                        sym_BZ2_bzDecompressEnd(&c->bzip2);
                বিরতি;
#endif

        পূর্বনির্ধারিত:
                বিরতি;
        }

        ফেরত mfree(c);
}

Compression compressor_type(ধ্রুবক Compressor *c) {
        ফেরত c ? c->type : _COMPRESSION_INVALID;
}

পূর্ণসংখ্যা decompressor_detect(Decompressor **decompressor, ধ্রুবক শূন্য *তথ্য, size_t আকার_2) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(decompressor);

        যদি (*decompressor)
                ফেরত 1;

        যদি (আকার_2 < COMPRESSION_MAGIC_BYTES_MAX)
                ফেরত 0;

        প্রতিজ্ঞা(তথ্য);

        Compression type = compression_detect_from_magic(তথ্য);
        যদি (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_to_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        যদি (r < 0)
                ফেরত r;

        ফেরত 1;
}

পূর্ণসংখ্যা decompressor_force(Decompressor **decompressor, Compression type) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(decompressor);
        প্রতিজ্ঞা(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        *decompressor = compressor_free(*decompressor);
        ফেরত decompressor_new(decompressor, type, /* flags= */ 0);
}

পূর্ণসংখ্যা decompressor_force_off(Decompressor **decompressor) {
        ফেরত decompressor_force(decompressor, COMPRESSION_NONE);
}

পূর্ণসংখ্যা decompressor_ঠেলো(Decompressor *c, ধ্রুবক শূন্য *তথ্য, size_t আকার_2, DecompressorCallback callback, শূন্য *userdata) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(c);
        প্রতিজ্ঞা(তথ্য || আকার_2 == 0);
        প্রতিজ্ঞা(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        যদি (c->encoding)
                ফেরত -EINVAL;

        যদি (c->finished && আকার_2 == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                ফেরত 0;

#যদি HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *বাফার = ফাঁকা;
        যদি (c->type != COMPRESSION_NONE) {
                বাফার = new(uint8_t, COMPRESS_PIPE_বাফার_আকার);
                যদি (!বাফার)
                        ফেরত -ENOMEM;
        }
#endif

        নির্বাচন (c->type) {

        ক্ষেত্র COMPRESSION_NONE:
                যদি (c->finished)
                        ফেরত -EBADMSG;

                যদি (আকার_2 == 0) {
                        c->finished = true;
                        ফেরত 0;
                }

                r = callback(তথ্য, আকার_2, userdata);
                যদি (r < 0)
                        ফেরত r;

                ফেরত 0;

#যদি HAVE_XZ
        ক্ষেত্র COMPRESSION_XZ:
                যদি (c->finished)
                        ফেরত -EBADMSG;

                c->xz.পরবর্তী_in = তথ্য;
                c->xz.avail_in = আকার_2;

                জন্য (;;) {
                        c->xz.পরবর্তী_out = বাফার;
                        c->xz.avail_out = COMPRESS_PIPE_বাফার_আকার;

                        lzma_ret lzr = sym_lzma_code(&c->xz, আকার_2 == 0 ? LZMA_FINISH : LZMA_RUN);
                        যদি (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                ফেরত -EBADMSG;

                        যদি (c->xz.avail_out > COMPRESS_PIPE_বাফার_আকার)
                                ফেরত -EIO;

                        size_t produced = COMPRESS_PIPE_বাফার_আকার - c->xz.avail_out;
                        যদি (produced > 0) {
                                r = callback(বাফার, produced, userdata);
                                যদি (r < 0)
                                        ফেরত r;
                        }

                        যদি (lzr == LZMA_STREAM_END) {
                                যদি (c->xz.avail_in > 0)
                                        ফেরত -EBADMSG; /* garbage data at the end */

                                c->finished = true;
                                ফেরত 0;
                        }

                        যদি (c->xz.avail_in == 0 && c->xz.avail_out > 0) {
                                যদি (আকার_2 == 0)
                                        ফেরত -EBADMSG; /* truncated */

                                ফেরত 0; /* No progress possible with the current input. */
                        }
                }
#endif

#যদি HAVE_LZ4
        ক্ষেত্র COMPRESSION_LZ4: {
                ধ্রুবক uint8_t *src = তথ্য;
                size_t src_remaining = আকার_2;

                c->finished = false;

                জন্য (;;) {
                        size_t produced = COMPRESS_PIPE_বাফার_আকার;
                        size_t consumed = src_remaining;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, বাফার, &produced, src, &consumed, ফাঁকা);
                        যদি (sym_LZ4F_isError(rc))
                                ফেরত -EBADMSG;

                        যদি (consumed > src_remaining ||
                            produced > COMPRESS_PIPE_বাফার_আকার)
                                ফেরত -EIO;

                        যদি (consumed > 0) {
                                প্রতিজ্ঞা(src);
                                src += consumed;
                                src_remaining -= consumed;
                        }

                        যদি (produced > 0) {
                                r = callback(বাফার, produced, userdata);
                                যদি (r < 0)
                                        ফেরত r;
                        }

                        যদি (src_remaining == 0) {
                                যদি (rc == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        ফেরত 0;
                                }

                                যদি (produced == COMPRESS_PIPE_বাফার_আকার)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        চালু;

                                যদি (আকার_2 == 0)
                                        ফেরত -EBADMSG; /* truncated */

                                ফেরত 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#যদি HAVE_ZSTD
        ক্ষেত্র COMPRESSION_ZSTD: {
                ZSTD_in_বাফার প্রবেশ = {
                        .src =  (শূন্য*) তথ্য,
                        .আকার_2 = আকার_2,
                };

                c->finished = false;

                জন্য (;;) {
                        ZSTD_out_বাফার output = {
                                .dst = বাফার,
                                .আকার_2 = COMPRESS_PIPE_বাফার_আকার,
                        };

                        size_t res = sym_ZSTD_decompressStream(c->d_zstd, &output, &প্রবেশ);
                        যদি (sym_ZSTD_isError(res))
                                ফেরত -EBADMSG;

                        যদি (প্রবেশ.pos > প্রবেশ.আকার_2 ||
                            output.pos > output.আকার_2)
                                ফেরত -EIO;

                        যদি (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                যদি (r < 0)
                                        ফেরত r;
                        }

                        যদি (প্রবেশ.pos == প্রবেশ.আকার_2) {
                                যদি (res == 0) {
                                        /* The current stream has finished. */
                                        c->finished = true;
                                        ফেরত 0;
                                }

                                যদি (output.pos == COMPRESS_PIPE_বাফার_আকার)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        চালু;

                                যদি (আকার_2 == 0)
                                        ফেরত -EBADMSG; /* truncated */

                                ফেরত 0; /* No progress possible with the current input. */
                        }
                }
        }
#endif

#যদি HAVE_ZLIB
        ক্ষেত্র COMPRESSION_GZIP:
                যদি (আকার_2 > UINT_MAX)
                        ফেরত -EFBIG;

                c->gzip.পরবর্তী_in = (শূন্য*) তথ্য;
                c->gzip.avail_in = আকার_2;

                জন্য (;;) {
                        যদি (c->finished) {
                                যদি (sym_inflateReset(&c->gzip) != Z_OK)
                                        ফেরত -EIO;

                                c->finished = false;
                        }

                        c->gzip.পরবর্তী_out = বাফার;
                        c->gzip.avail_out = COMPRESS_PIPE_বাফার_আকার;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        পূর্ণসংখ্যা zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        যদি (!IN_SET(zr, Z_OK, Z_STREAM_END) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_in == 0))
                                ফেরত -EBADMSG;

                        যদি (c->gzip.avail_out > COMPRESS_PIPE_বাফার_আকার)
                                ফেরত -EIO;

                        size_t produced = COMPRESS_PIPE_বাফার_আকার - c->gzip.avail_out;
                        যদি (produced > 0) {
                                r = callback(বাফার, produced, userdata);
                                যদি (r < 0)
                                        ফেরত r;
                        }

                        যদি (zr == Z_STREAM_END) {
                                c->finished = true;

                                যদি (c->gzip.avail_in > 0)
                                        চালু; /* New stream may be pending. */

                                ফেরত 0;
                        }

                        যদি (c->gzip.avail_in == 0 && c->gzip.avail_out > 0) {
                                যদি (আকার_2 == 0)
                                        ফেরত -EBADMSG; /* truncated */

                                ফেরত 0; /* No progress possible with the current input. */
                        }
                }
#endif

#যদি HAVE_BZIP2
        ক্ষেত্র COMPRESSION_BZIP2:
                যদি (আকার_2 > UINT_MAX)
                        ফেরত -EFBIG;

                c->bzip2.পরবর্তী_in = (অক্ষর*) তথ্য;
                c->bzip2.avail_in = আকার_2;

                জন্য (;;) {
                        যদি (c->finished) {
                                sym_BZ2_bzDecompressEnd(&c->bzip2);
                                যদি (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        ফেরত -EIO;

                                c->finished = false;
                        }

                        c->bzip2.পরবর্তী_out = (অক্ষর*) বাফার;
                        c->bzip2.avail_out = COMPRESS_PIPE_বাফার_আকার;

                        পূর্ণসংখ্যা bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        যদি (!IN_SET(bzr, BZ_OK, BZ_STREAM_END))
                                ফেরত -EBADMSG;

                        যদি (c->bzip2.avail_out > COMPRESS_PIPE_বাফার_আকার)
                                ফেরত -EIO;

                        size_t produced = COMPRESS_PIPE_বাফার_আকার - c->bzip2.avail_out;
                        যদি (produced > 0) {
                                r = callback(বাফার, produced, userdata);
                                যদি (r < 0)
                                        ফেরত r;
                        }

                        যদি (bzr == BZ_STREAM_END) {
                                c->finished = true;

                                যদি (c->bzip2.avail_in > 0)
                                        চালু; /* New stream may be pending. */

                                ফেরত 0;
                        }

                        যদি (c->bzip2.avail_in == 0 && c->bzip2.avail_out > 0) {
                                যদি (আকার_2 == 0)
                                        ফেরত -EBADMSG; /* truncated */

                                ফেরত 0; /* No progress possible with the current input. */
                        }
                }
#endif

        পূর্বনির্ধারিত:
                assert_not_reached();
        }
}

পূর্ণসংখ্যা compressor_new(Compressor **ret, Compression type) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        যদি (r < 0)
                ফেরত r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        যদি (!c)
                ফেরত -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        নির্বাচন (type) {

#যদি HAVE_XZ
        ক্ষেত্র COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                যদি (xzr != LZMA_OK)
                        ফেরত -EIO;

                c->type = COMPRESSION_XZ;
                বিরতি;
        }
#endif

#যদি HAVE_LZ4
        ক্ষেত্র COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4F_VERSION);
                যদি (sym_LZ4F_isError(rc))
                        ফেরত -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t header_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_header = malloc(header_bound);
                যদি (!c->lz4_header)
                        ফেরত -ENOMEM;

                c->lz4_header_আকার = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_header, header_bound, &lz4_preferences);
                যদি (sym_LZ4F_isError(c->lz4_header_আকার))
                        ফেরত -EINVAL;

                বিরতি;
        }
#endif

#যদি HAVE_ZSTD
        ক্ষেত্র COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                যদি (!c->c_zstd)
                        ফেরত -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                যদি (sym_ZSTD_isError(z))
                        ফেরত -EIO;

                z = sym_ZSTD_CCtx_setParameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                যদি (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                বিরতি;
#endif

#যদি HAVE_ZLIB
        ক্ষেত্র COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_VERSION, (পূর্ণসংখ্যা) আকার(c->gzip));
                যদি (r != Z_OK)
                        ফেরত -EIO;

                c->type = COMPRESSION_GZIP;
                বিরতি;
#endif

#যদি HAVE_BZIP2
        ক্ষেত্র COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                যদি (r != BZ_OK)
                        ফেরত -EIO;

                c->type = COMPRESSION_BZIP2;
                বিরতি;
#endif

        ক্ষেত্র COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                বিরতি;

        পূর্বনির্ধারিত:
                ফেরত -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        ফেরত 0;
}

#যদি HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
স্থির পূর্ণসংখ্যা enlarge_বাফার(শূন্য **বাফার, size_t *বাফার_আকার, size_t *বাফার_allocated, size_t need) {
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(বাফার_আকার);
        প্রতিজ্ঞা(বাফার_allocated);

        need = MAX3(need, *বাফার_আকার + 1, (size_t) COMPRESS_PIPE_বাফার_আকার);
        যদি (*বাফার_allocated >= need)
                ফেরত 0;

        যদি (!greedy_realloc(বাফার, need, 1))
                ফেরত -ENOMEM;

        *বাফার_allocated = MALLOC_SIZEOF_SAFE(*বাফার);
        ফেরত 1;
}
#endif

পূর্ণসংখ্যা compressor_start(
                Compressor *c,
                ধ্রুবক শূন্য *তথ্য,
                size_t আকার_2,
                শূন্য **বাফার,
                size_t *বাফার_আকার,
                size_t *বাফার_allocated) {

#যদি HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        পূর্ণসংখ্যা r;
#endif

        প্রতিজ্ঞা(c);
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(বাফার_আকার);
        প্রতিজ্ঞা(বাফার_allocated);

        যদি (!c->encoding)
                ফেরত -EINVAL;

        যদি (আকার_2 == 0)
                ফেরত 0;

        প্রতিজ্ঞা(তথ্য);

        *বাফার_আকার = 0;

        নির্বাচন (c->type) {

#যদি HAVE_XZ
        ক্ষেত্র COMPRESSION_XZ:

                c->xz.পরবর্তী_in = তথ্য;
                c->xz.avail_in = আকার_2;

                যখন (c->xz.avail_in > 0) {
                        lzma_ret lzr;

                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, /* need= */ 0);
                        যদি (r < 0)
                                ফেরত r;

                        c->xz.পরবর্তী_out = (uint8_t*) *বাফার + *বাফার_আকার;
                        c->xz.avail_out = *বাফার_allocated - *বাফার_আকার;

                        lzr = sym_lzma_code(&c->xz, LZMA_RUN);
                        যদি (lzr != LZMA_OK)
                                ফেরত -EIO;

                        *বাফার_আকার += (*বাফার_allocated - *বাফার_আকার) - c->xz.avail_out;
                }

                বিরতি;
#endif

#যদি HAVE_LZ4
        ক্ষেত্র COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                যদি (c->lz4_header_আকার > 0) {
                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, c->lz4_header_আকার);
                        যদি (r < 0)
                                ফেরত r;

                        memcpy(*বাফার, c->lz4_header, c->lz4_header_আকার);
                        *বাফার_আকার = c->lz4_header_আকার;
                        c->lz4_header = mfree(c->lz4_header);
                        c->lz4_header_আকার = 0;
                }

                size_t bound = sym_LZ4F_compressBound(আকার_2, &lz4_preferences);
                r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, *বাফার_আকার + bound);
                যদি (r < 0)
                        ফেরত r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *বাফার + *বাফার_আকার,
                                                   *বাফার_allocated - *বাফার_আকার,
                                                   তথ্য, আকার_2, ফাঁকা);
                যদি (sym_LZ4F_isError(n))
                        ফেরত -EIO;

                *বাফার_আকার += n;
                বিরতি;
        }
#endif

#যদি HAVE_ZSTD
        ক্ষেত্র COMPRESSION_ZSTD: {
                ZSTD_in_বাফার প্রবেশ = {
                        .src = তথ্য,
                        .আকার_2 = আকার_2,
                };

                যখন (প্রবেশ.pos < প্রবেশ.আকার_2) {
                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, /* need= */ 0);
                        যদি (r < 0)
                                ফেরত r;

                        ZSTD_out_বাফার output = {
                                .dst = ((uint8_t *) *বাফার + *বাফার_আকার),
                                .আকার_2 = *বাফার_allocated - *বাফার_আকার,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &প্রবেশ, ZSTD_e_continue);
                        যদি (sym_ZSTD_isError(res))
                                ফেরত -EIO;

                        *বাফার_আকার += output.pos;
                }

                বিরতি;
        }
#endif

#যদি HAVE_ZLIB
        ক্ষেত্র COMPRESSION_GZIP:
                যদি (আকার_2 > UINT_MAX)
                        ফেরত -EFBIG;

                c->gzip.পরবর্তী_in = (শূন্য*) তথ্য;
                c->gzip.avail_in = আকার_2;

                যখন (c->gzip.avail_in > 0) {
                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, /* need= */ 0);
                        যদি (r < 0)
                                ফেরত r;

                        size_t avail = MIN(*বাফার_allocated - *বাফার_আকার, (size_t) UINT_MAX);
                        c->gzip.পরবর্তী_out = (uint8_t*) *বাফার + *বাফার_আকার;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        যদি (r != Z_OK)
                                ফেরত -EIO;

                        *বাফার_আকার += avail - c->gzip.avail_out;
                }

                বিরতি;
#endif

#যদি HAVE_BZIP2
        ক্ষেত্র COMPRESSION_BZIP2:
                যদি (আকার_2 > UINT_MAX)
                        ফেরত -EFBIG;

                c->bzip2.পরবর্তী_in = (শূন্য*) তথ্য;
                c->bzip2.avail_in = আকার_2;

                যখন (c->bzip2.avail_in > 0) {
                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, /* need= */ 0);
                        যদি (r < 0)
                                ফেরত r;

                        size_t avail = MIN(*বাফার_allocated - *বাফার_আকার, (size_t) UINT_MAX);
                        c->bzip2.পরবর্তী_out = (শূন্য*) ((uint8_t*) *বাফার + *বাফার_আকার);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_RUN);
                        যদি (r != BZ_RUN_OK)
                                ফেরত -EIO;

                        *বাফার_আকার += avail - c->bzip2.avail_out;
                }

                বিরতি;
#endif

        ক্ষেত্র COMPRESSION_NONE:

                যদি (*বাফার_allocated < আকার_2) {
                        শূন্য *p;

                        p = realloc(*বাফার, আকার_2);
                        যদি (!p)
                                ফেরত -ENOMEM;

                        *বাফার = p;
                        *বাফার_allocated = আকার_2;
                }

                memcpy(*বাফার, তথ্য, আকার_2);
                *বাফার_আকার = আকার_2;
                বিরতি;

        পূর্বনির্ধারিত:
                ফেরত -EOPNOTSUPP;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা compressor_finish(Compressor *c, শূন্য **বাফার, size_t *বাফার_আকার, size_t *বাফার_allocated) {
#যদি HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        পূর্ণসংখ্যা r;
#endif

        প্রতিজ্ঞা(c);
        প্রতিজ্ঞা(বাফার);
        প্রতিজ্ঞা(বাফার_আকার);
        প্রতিজ্ঞা(বাফার_allocated);

        যদি (!c->encoding)
                ফেরত -EINVAL;

        *বাফার_আকার = 0;

        নির্বাচন (c->type) {

#যদি HAVE_XZ
        ক্ষেত্র COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_in = 0;

                করো {
                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, /* need= */ 0);
                        যদি (r < 0)
                                ফেরত r;

                        c->xz.পরবর্তী_out = (uint8_t*) *বাফার + *বাফার_আকার;
                        c->xz.avail_out = *বাফার_allocated - *বাফার_আকার;

                        lzr = sym_lzma_code(&c->xz, LZMA_FINISH);
                        যদি (!IN_SET(lzr, LZMA_OK, LZMA_STREAM_END))
                                ফেরত -EIO;

                        *বাফার_আকার += (*বাফার_allocated - *বাফার_আকার) - c->xz.avail_out;
                } যখন (lzr != LZMA_STREAM_END);

                বিরতি;
        }
#endif

#যদি HAVE_LZ4
        ক্ষেত্র COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, bound);
                যদি (r < 0)
                        ফেরত r;

                size_t n = sym_LZ4F_compressEnd(c->c_lz4, *বাফার, *বাফার_allocated, ফাঁকা);
                যদি (sym_LZ4F_isError(n))
                        ফেরত -EIO;

                *বাফার_আকার = n;
                বিরতি;
        }
#endif

#যদি HAVE_ZSTD
        ক্ষেত্র COMPRESSION_ZSTD: {
                ZSTD_in_বাফার প্রবেশ = {};
                size_t res;

                করো {
                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, /* need= */ 0);
                        যদি (r < 0)
                                ফেরত r;

                        ZSTD_out_বাফার output = {
                                .dst = ((uint8_t *) *বাফার + *বাফার_আকার),
                                .আকার_2 = *বাফার_allocated - *বাফার_আকার,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &প্রবেশ, ZSTD_e_end);
                        যদি (sym_ZSTD_isError(res))
                                ফেরত -EIO;

                        *বাফার_আকার += output.pos;
                } যখন (res != 0);

                বিরতি;
        }
#endif

#যদি HAVE_ZLIB
        ক্ষেত্র COMPRESSION_GZIP:
                c->gzip.avail_in = 0;

                করো {
                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, /* need= */ 0);
                        যদি (r < 0)
                                ফেরত r;

                        size_t avail = MIN(*বাফার_allocated - *বাফার_আকার, (size_t) UINT_MAX);
                        c->gzip.পরবর্তী_out = (uint8_t*) *বাফার + *বাফার_আকার;
                        c->gzip.avail_out = avail;

                        r = sym_deflate(&c->gzip, Z_FINISH);
                        যদি (!IN_SET(r, Z_OK, Z_STREAM_END))
                                ফেরত -EIO;

                        *বাফার_আকার += avail - c->gzip.avail_out;
                } যখন (r != Z_STREAM_END);

                বিরতি;
#endif

#যদি HAVE_BZIP2
        ক্ষেত্র COMPRESSION_BZIP2:
                c->bzip2.avail_in = 0;

                করো {
                        r = enlarge_বাফার(বাফার, বাফার_আকার, বাফার_allocated, /* need= */ 0);
                        যদি (r < 0)
                                ফেরত r;

                        size_t avail = MIN(*বাফার_allocated - *বাফার_আকার, (size_t) UINT_MAX);
                        c->bzip2.পরবর্তী_out = (শূন্য*) ((uint8_t*) *বাফার + *বাফার_আকার);
                        c->bzip2.avail_out = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_FINISH);
                        যদি (!IN_SET(r, BZ_FINISH_OK, BZ_STREAM_END))
                                ফেরত -EIO;

                        *বাফার_আকার += avail - c->bzip2.avail_out;
                } যখন (r != BZ_STREAM_END);

                বিরতি;
#endif

        ক্ষেত্র COMPRESSION_NONE:
                বিরতি;

        পূর্বনির্ধারিত:
                ফেরত -EOPNOTSUPP;
        }

        ফেরত 0;
}
