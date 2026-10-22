/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#ఒకవేళ HAVE_XZ
#నిర్వచితం_కాకపోతే SYSTEMD_CFLAGS_MARKER_LIBXZ
#  error "missing libxz_cflags in meson dependency."
#అయితే_ముగింపు
#అన్తర్భావ <lzma.h>
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
#నిర్వచితం_కాకపోతే SYSTEMD_CFLAGS_MARKER_LIBLZ4
#  error "missing liblz4_cflags in meson dependency."
#అయితే_ముగింపు
#అన్తర్భావ <lz4.h>
#అన్తర్భావ <lz4frame.h>
#అన్తర్భావ <lz4hc.h>
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
#నిర్వచితం_కాకపోతే SYSTEMD_CFLAGS_MARKER_LIBZSTD
#  error "missing libzstd_cflags in meson dependency."
#అయితే_ముగింపు
#అన్తర్భావ <zstd.h>
#అన్తర్భావ <zstd_errors.h>
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
#నిర్వచితం_కాకపోతే SYSTEMD_CFLAGS_MARKER_LIBZ
#  error "missing libz_cflags in meson dependency."
#అయితే_ముగింపు
#అన్తర్భావ <zlib.h>
#అయితే_ముగింపు

#ఒకవేళ HAVE_BZIP2
#నిర్వచితం_కాకపోతే SYSTEMD_CFLAGS_MARKER_LIBBZIP2
#  error "missing libbzip2_cflags in meson dependency."
#అయితే_ముగింపు
#అన్తర్భావ <bzlib.h>
#అయితే_ముగింపు

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "bitfield.h"
#అన్తర్భావ "compress.h"
#అన్తర్భావ "dlfcn-util.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "unaligned.h"

#ఒకవేళ HAVE_XZ
స్థిర DLSYM_PROTOTYPE(lzma_auto_decoder) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(lzma_సంకేతం) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(lzma_easy_encoder) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(lzma_చివర) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(lzma_ప్రవాహం_బఫరు_encode) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(lzma_ప్రవాహం_decoder) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(lzma_lzma_preset) = ఖాళీ;

/* We can’t just do _cleanup_(sym_lzma_end) because a compiler bug makes
 * this fail with:
 * ../src/basic/compress.c: In function ‘decompress_blob_xz’:
 * ../src/basic/compress.c:304:9: error: cleanup argument not a function
 *   304 |         _cleanup_(sym_lzma_end) lzma_stream s = LZMA_STREAM_INIT;
 *       |         ^~~~~~~~~
 */
స్థిర అంతఃవరుస శూన్య lzma_చివర_wrapper(lzma_ప్రవాహం *ls) {
        sym_lzma_చివర(ls);
}
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
స్థిర DLSYM_PROTOTYPE(LZ4F_compressBegin) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4F_compressBound) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4_F_compress_చివర) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4F_compressUpdate) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4F_createCompressionContext) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4F_createDecompressionContext) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4F_decompress) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4F_freeCompressionContext) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4F_freeDecompressionContext) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4F_isError) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4_compress_HC) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4_compress_default) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4_decompress_సురక్షిత) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4_decompress_సురక్షిత_partial) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(LZ4_సంస్కరణ_సంఖ్య) = ఖాళీ;

స్థిర స్థిరరాశి LZ4F_preferences_t lz4_preferences = {
        .చట్రం_Info.ఖండం_కొలత_ID = 5,
};
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
స్థిర DLSYM_PROTOTYPE(ZSTD_CCtx_సమితి_Parameter) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_compress) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_compressStream2) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_createCCtx) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_createDCtx) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_CStream_లోపల_కొలత) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_CStream_బహిర్గతం_కొలత) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_decompress_ప్రవాహం) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_DStream_లోపల_కొలత) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_DStream_బహిర్గతం_కొలత) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_freeCCtx) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_freeDCtx) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_get_Error_సంకేతం) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_getErrorName) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_get_చట్రం_Content_కొలత) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(ZSTD_isError) = ఖాళీ;

DEFINE_TRIVIAL_CLEANUP_FUNC_నిండిన_RENAME(ZSTD_DCtx*, sym_ZSTD_freeDCtx, ZSTD_freeDCtxp, ఖాళీ);

స్థిర పూర్ణాంకం zstd_ret_గమ్యం_errno(size_t ret) {
        ఎంపిక (sym_ZSTD_get_Error_సంకేతం(ret)) {
        స్థితి ZSTD_error_dst_కొలత_too_Small:
                తిరిగి -ENOBUFS;
        స్థితి ZSTD_error_జ్ఞాపకం_కేటాయింపు:
                తిరిగి -ENOMEM;
        అప్రమేయ:
                తిరిగి -EBADMSG;
        }
}
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
స్థిర DLSYM_PROTOTYPE(deflateInit2_) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(deflate) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(deflate_చివర) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(inflateInit2_) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(inflate) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(inflate_చివర) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(inflateReset) = ఖాళీ;

స్థిర అంతఃవరుస శూన్య deflate_చివర_wrapper(z_ప్రవాహం *s) {
        sym_deflate_చివర(s);
}

స్థిర అంతఃవరుస శూన్య inflate_చివర_wrapper(z_ప్రవాహం *s) {
        sym_inflate_చివర(s);
}
#అయితే_ముగింపు

#ఒకవేళ HAVE_BZIP2
స్థిర DLSYM_PROTOTYPE(BZ2_bzCompressInit) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(BZ2_bzCompress) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(BZ2_bz_Compress_చివర) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(BZ2_bzDecompressInit) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(BZ2_bzDecompress) = ఖాళీ;
స్థిర DLSYM_PROTOTYPE(BZ2_bz_Decompress_చివర) = ఖాళీ;

స్థిర అంతఃవరుస శూన్య BZ2_bz_Compress_చివర_wrapper(bz_ప్రవాహం *s) {
        sym_BZ2_bz_Compress_చివర(s);
}

స్థిర అంతఃవరుస శూన్య BZ2_bz_Decompress_చివర_wrapper(bz_ప్రవాహం *s) {
        sym_BZ2_bz_Decompress_చివర(s);
}
#అయితే_ముగింపు

/* Opaque Compressor/Decompressor struct definition */
నిర్మాణం Compressor {
        Compression type;
        bool encoding;
        bool ముగిసిన;
        సంఘం {
#ఒకవేళ HAVE_XZ
                lzma_ప్రవాహం xz;
#అయితే_ముగింపు
#ఒకవేళ HAVE_LZ4
                నిర్మాణం {
                        LZ4F_compressionContext_t c_lz4;
                        శూన్య *lz4_శీర్షిక;        /* stashed frame header from LZ4F_compressBegin */
                        size_t lz4_శీర్షిక_కొలత;
                };
                LZ4F_decompressionContext_t d_lz4;
#అయితే_ముగింపు
#ఒకవేళ HAVE_ZSTD
                ZSTD_CCtx *c_zstd;
                ZSTD_DCtx *d_zstd;
#అయితే_ముగింపు
#ఒకవేళ HAVE_ZLIB
                z_ప్రవాహం gzip;
#అయితే_ముగింపు
#ఒకవేళ HAVE_BZIP2
                bz_ప్రవాహం bzip2;
#అయితే_ముగింపు
        };
};

#నిర్వచించు ALIGN_8(l) ALIGN_గమ్యం(l, పరిమాణం(size_t))

/* zlib windowBits value for gzip format: MAX_WBITS (15) + 16 to enable gzip header detection/generation */
#నిర్వచించు ZLIB_WBITS_GZIP (15 + 16)

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి compression_పట్టిక[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "uncompressed", /* backwards compatibility with importd */
        [COMPRESSION_XZ]    = "xz",
        [COMPRESSION_LZ4]   = "lz4",
        [COMPRESSION_ZSTD]  = "zstd",
        [COMPRESSION_GZIP]  = "gzip",
        [COMPRESSION_BZIP2] = "bzip2",
};

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి compression_uppercase_పట్టిక[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "NONE", /* backwards compatibility with SYSTEMD_JOURNAL_COMPRESS=NONE */
        [COMPRESSION_XZ]    = "XZ",
        [COMPRESSION_LZ4]   = "LZ4",
        [COMPRESSION_ZSTD]  = "ZSTD",
        [COMPRESSION_GZIP]  = "GZIP",
        [COMPRESSION_BZIP2] = "BZIP2",
};

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి compression_extension_పట్టిక[_COMPRESSION_MAX] = {
        [COMPRESSION_NONE]  = "",
        [COMPRESSION_XZ]    = ".xz",
        [COMPRESSION_LZ4]   = ".lz4",
        [COMPRESSION_ZSTD]  = ".zst",
        [COMPRESSION_GZIP]  = ".gz",
        [COMPRESSION_BZIP2] = ".bz2",
};

DEFINE_STRING_పట్టిక_LOOKUP(compression, Compression);
DEFINE_STRING_పట్టిక_LOOKUP(compression_uppercase, Compression);
DEFINE_STRING_పట్టిక_LOOKUP(compression_extension, Compression);

Compression compression_from_string_harder(స్థిరరాశి అక్షరం *s) {
        Compression c;

        ప్రతిజ్ఞా(s);

        c = compression_from_string(s);
        ఒకవేళ (c >= 0)
                తిరిగి c;

        తిరిగి compression_uppercase_from_string(s);
}

Compression compression_from_filename(స్థిరరాశి అక్షరం *filename) {
        Compression c;
        స్థిరరాశి అక్షరం *e;

        ప్రతిజ్ఞా(filename);

        e = strrchr(filename, '.');
        ఒకవేళ (!e)
                తిరిగి COMPRESSION_NONE;

        c = compression_extension_from_string(e);
        ఒకవేళ (c < 0)
                తిరిగి COMPRESSION_NONE;

        తిరిగి c;
}

bool compression_supported_journal(Compression c) {
        స్థిర స్థిరరాశి గుర్తులేని supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD;

        ప్రతిజ్ఞా(c >= 0);
        ప్రతిజ్ఞా(c < _COMPRESSION_MAX);

        తిరిగి బిట్టు_సమితి(supported, c);
}

bool compression_supported(Compression c) {
        స్థిర స్థిరరాశి గుర్తులేని supported =
                (1U << COMPRESSION_NONE) |
                (1U << COMPRESSION_XZ) * HAVE_XZ |
                (1U << COMPRESSION_LZ4) * HAVE_LZ4 |
                (1U << COMPRESSION_ZSTD) * HAVE_ZSTD |
                (1U << COMPRESSION_GZIP) * HAVE_ZLIB |
                (1U << COMPRESSION_BZIP2) * HAVE_BZIP2;

        ప్రతిజ్ఞా(c >= 0);
        ప్రతిజ్ఞా(c < _COMPRESSION_MAX);

        తిరిగి బిట్టు_సమితి(supported, c);
}

Compression compression_detect_from_magic(స్థిరరాశి uint8_t దత్తాంశం[స్థిర COMPRESSION_MAGIC_బైట్లు_MAX]) {
        /* Magic signatures per RFC 1952 (gzip), tukaani.org/xz/xz-file-format.txt (xz),
         * RFC 8878 (zstd), lz4/doc/lz4_Frame_format.md (lz4), and the bzip2 file format.
         * Make sure to update COMPRESSION_MAGIC_BYTES_MAX if needed when adding a new magic. */
        ఒకవేళ (memcmp(దత్తాంశం, (స్థిరరాశి uint8_t[]) { 0x1f, 0x8b }, 2) == 0)
                తిరిగి COMPRESSION_GZIP;
        ఒకవేళ (memcmp(దత్తాంశం, (స్థిరరాశి uint8_t[]) { 0xfd, '7', 'z', 'X', 'Z', 0x00 }, 6) == 0)
                తిరిగి COMPRESSION_XZ;
        ఒకవేళ (memcmp(దత్తాంశం, (స్థిరరాశి uint8_t[]) { 0x28, 0xb5, 0x2f, 0xfd }, 4) == 0)
                తిరిగి COMPRESSION_ZSTD;
        ఒకవేళ (memcmp(దత్తాంశం, (స్థిరరాశి uint8_t[]) { 0x04, 0x22, 0x4d, 0x18 }, 4) == 0)
                తిరిగి COMPRESSION_LZ4;
        ఒకవేళ (memcmp(దత్తాంశం, (స్థిరరాశి uint8_t[]) { 'B', 'Z', 'h' }, 3) == 0)
                తిరిగి COMPRESSION_BZIP2;

        తిరిగి _COMPRESSION_INVALID;
}

పూర్ణాంకం dlopen_xz(పూర్ణాంకం log_level) {
#ఒకవేళ HAVE_XZ
        స్థిర శూన్య *lzma_dl = ఖాళీ;

        LIBLZMA_NOTE(suggested);

        తిరిగి dlopen_many_sym_or_warn(
                        &lzma_dl,
                        "liblzma.so.5", log_level,
                        DLSYM_ARG(lzma_auto_decoder),
                        DLSYM_ARG(lzma_సంకేతం),
                        DLSYM_ARG(lzma_easy_encoder),
                        DLSYM_ARG(lzma_చివర),
                        DLSYM_ARG(lzma_ప్రవాహం_బఫరు_encode),
                        DLSYM_ARG(lzma_lzma_preset),
                        DLSYM_ARG(lzma_ప్రవాహం_decoder));
#లేకపోతే
        తిరిగి log_నిండిన_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lzma support is not compiled in.");
#అయితే_ముగింపు
}

పూర్ణాంకం dlopen_lz4(పూర్ణాంకం log_level) {
#ఒకవేళ HAVE_LZ4
        స్థిర శూన్య *lz4_dl = ఖాళీ;

        LIBLZ4_NOTE(suggested);

        తిరిగి dlopen_many_sym_or_warn(
                        &lz4_dl,
                        "liblz4.so.1", log_level,
                        DLSYM_ARG(LZ4F_compressBegin),
                        DLSYM_ARG(LZ4F_compressBound),
                        DLSYM_ARG(LZ4_F_compress_చివర),
                        DLSYM_ARG(LZ4F_compressUpdate),
                        DLSYM_ARG(LZ4F_createCompressionContext),
                        DLSYM_ARG(LZ4F_createDecompressionContext),
                        DLSYM_ARG(LZ4F_decompress),
                        DLSYM_ARG(LZ4F_freeCompressionContext),
                        DLSYM_ARG(LZ4F_freeDecompressionContext),
                        DLSYM_ARG(LZ4F_isError),
                        DLSYM_ARG(LZ4_compress_default),
                        DLSYM_ARG(LZ4_compress_HC),
                        DLSYM_ARG(LZ4_decompress_సురక్షిత),
                        DLSYM_ARG(LZ4_decompress_సురక్షిత_partial),
                        DLSYM_ARG(LZ4_సంస్కరణ_సంఖ్య));
#లేకపోతే
        తిరిగి log_నిండిన_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "lz4 support is not compiled in.");
#అయితే_ముగింపు
}

పూర్ణాంకం dlopen_zstd(పూర్ణాంకం log_level) {
#ఒకవేళ HAVE_ZSTD
        స్థిర శూన్య *zstd_dl = ఖాళీ;

        LIBZSTD_NOTE(suggested);

        తిరిగి dlopen_many_sym_or_warn(
                        &zstd_dl,
                        "libzstd.so.1", log_level,
                        DLSYM_ARG(ZSTD_get_Error_సంకేతం),
                        DLSYM_ARG(ZSTD_compress),
                        DLSYM_ARG(ZSTD_get_చట్రం_Content_కొలత),
                        DLSYM_ARG(ZSTD_decompress_ప్రవాహం),
                        DLSYM_ARG(ZSTD_getErrorName),
                        DLSYM_ARG(ZSTD_DStream_బహిర్గతం_కొలత),
                        DLSYM_ARG(ZSTD_CStream_లోపల_కొలత),
                        DLSYM_ARG(ZSTD_CStream_బహిర్గతం_కొలత),
                        DLSYM_ARG(ZSTD_CCtx_సమితి_Parameter),
                        DLSYM_ARG(ZSTD_compressStream2),
                        DLSYM_ARG(ZSTD_DStream_లోపల_కొలత),
                        DLSYM_ARG(ZSTD_freeCCtx),
                        DLSYM_ARG(ZSTD_freeDCtx),
                        DLSYM_ARG(ZSTD_isError),
                        DLSYM_ARG(ZSTD_createDCtx),
                        DLSYM_ARG(ZSTD_createCCtx));
#లేకపోతే
        తిరిగి log_నిండిన_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zstd support is not compiled in.");
#అయితే_ముగింపు
}

పూర్ణాంకం dlopen_zlib(పూర్ణాంకం log_level) {
#ఒకవేళ HAVE_ZLIB
        స్థిర శూన్య *zlib_dl = ఖాళీ;

        LIBZ_NOTE(suggested);

        తిరిగి dlopen_many_sym_or_warn(
                        &zlib_dl,
                        "libz.so.1", log_level,
                        DLSYM_ARG(deflateInit2_),
                        DLSYM_ARG(deflate),
                        DLSYM_ARG(deflate_చివర),
                        DLSYM_ARG(inflateInit2_),
                        DLSYM_ARG(inflate),
                        DLSYM_ARG(inflate_చివర),
                        DLSYM_ARG(inflateReset));
#లేకపోతే
        తిరిగి log_నిండిన_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "zlib support is not compiled in.");
#అయితే_ముగింపు
}

పూర్ణాంకం dlopen_bzip2(పూర్ణాంకం log_level) {
#ఒకవేళ HAVE_BZIP2
        స్థిర శూన్య *bzip2_dl = ఖాళీ;

        LIBBZ2_NOTE(suggested);

        తిరిగి dlopen_many_sym_or_warn(
                        &bzip2_dl,
                        "libbz2.so.1", log_level,
                        DLSYM_ARG(BZ2_bzCompressInit),
                        DLSYM_ARG(BZ2_bzCompress),
                        DLSYM_ARG(BZ2_bz_Compress_చివర),
                        DLSYM_ARG(BZ2_bzDecompressInit),
                        DLSYM_ARG(BZ2_bzDecompress),
                        DLSYM_ARG(BZ2_bz_Decompress_చివర));
#లేకపోతే
        తిరిగి log_నిండిన_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                              "bzip2 support is not compiled in.");
#అయితే_ముగింపు
}

పూర్ణాంకం dlopen_compress_journal(Compression c, పూర్ణాంకం log_level) {
        ఎంపిక (c) {
        స్థితి COMPRESSION_NONE:
                తిరిగి 0;
        స్థితి COMPRESSION_XZ:
                తిరిగి dlopen_xz(log_level);
        స్థితి COMPRESSION_LZ4:
                తిరిగి dlopen_lz4(log_level);
        స్థితి COMPRESSION_ZSTD:
                తిరిగి dlopen_zstd(log_level);
        అప్రమేయ:
                తిరిగి -EOPNOTSUPP;
        }
}

పూర్ణాంకం dlopen_compress(Compression c, పూర్ణాంకం log_level) {
        ఎంపిక (c) {
        స్థితి COMPRESSION_GZIP:
                తిరిగి dlopen_zlib(log_level);
        స్థితి COMPRESSION_BZIP2:
                తిరిగి dlopen_bzip2(log_level);
        అప్రమేయ:
                తిరిగి dlopen_compress_journal(c, log_level);
        }
}

స్థిర పూర్ణాంకం compress_blob_xz(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య *dst,
                size_t dst_alloc_కొలత,
                size_t *dst_కొలత,
                పూర్ణాంకం level) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_alloc_కొలత > 0);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_XZ
        lzma_options_lzma opt = {
                1u << 20u, ఖాళీ, 0, LZMA_LC_DEFAULT, LZMA_LP_DEFAULT,
                LZMA_PB_DEFAULT, LZMA_బహుళకం_FAST, 128, LZMA_MF_HC3, 4
        };
        lzma_filter filters[] = {
                { LZMA_FILTER_LZMA2, &opt },
                { LZMA_VLI_UNKNOWN, ఖాళీ }
        };
        lzma_ret ret;
        size_t బహిర్గతం_pos = 0;
        పూర్ణాంకం r;

        r = dlopen_xz(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (level >= 0) {
                /* This returns lzma_bool, and true means failure, i.e. the specified level is not supported. */
                ఒకవేళ (sym_lzma_lzma_preset(&opt, (uint32_t) level))
                        తిరిగి -EINVAL;
        }

        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ఒకవేళ (src_కొలత < 80)
                తిరిగి -ENOBUFS;

        ret = sym_lzma_ప్రవాహం_బఫరు_encode(filters, LZMA_CHECK_NONE, /* allocator= */ ఖాళీ,
                                            src, src_కొలత, dst, &బహిర్గతం_pos, dst_alloc_కొలత);
        ఒకవేళ (ret != LZMA_OK)
                తిరిగి -ENOBUFS;

        *dst_కొలత = బహిర్గతం_pos;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం compress_blob_lz4(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య *dst,
                size_t dst_alloc_కొలత,
                size_t *dst_కొలత,
                పూర్ణాంకం level) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_alloc_కొలత > 0);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_LZ4
        పూర్ణాంకం r;

        r = dlopen_lz4(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;
        /* Returns < 0 if we couldn't compress the data or the
         * compressed result is longer than the original */

        ఒకవేళ (src_కొలత < 9)
                తిరిగి -ENOBUFS;

        ఒకవేళ (src_కొలత > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -EFBIG;
        ఒకవేళ (dst_alloc_కొలత > గరిష్ఠ_పూర్ణాంకం)
                dst_alloc_కొలత = గరిష్ఠ_పూర్ణాంకం;

        ఒకవేళ (level <= 0)
                r = sym_LZ4_compress_default(src, (అక్షరం*)dst + 8, src_కొలత, (పూర్ణాంకం) dst_alloc_కొలత - 8);
        లేకపోతే
                r = sym_LZ4_compress_HC(src, (అక్షరం*)dst + 8, src_కొలత, (పూర్ణాంకం) dst_alloc_కొలత - 8, level);
        ఒకవేళ (r <= 0)
                తిరిగి -ENOBUFS;

        unaligned_write_le64(dst, src_కొలత);
        *dst_కొలత = r + 8;

        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం compress_blob_zstd(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య *dst,
                size_t dst_alloc_కొలత,
                size_t *dst_కొలత,
                పూర్ణాంకం level) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_alloc_కొలత > 0);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_ZSTD
        size_t k;
        పూర్ణాంకం r;

        r = dlopen_zstd(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        k = sym_ZSTD_compress(dst, dst_alloc_కొలత, src, src_కొలత, level < 0 ? 0 : level);
        ఒకవేళ (sym_ZSTD_isError(k))
                తిరిగి zstd_ret_గమ్యం_errno(k);

        *dst_కొలత = k;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం compress_blob_gzip(స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                       శూన్య *dst, size_t dst_alloc_కొలత, size_t *dst_కొలత, పూర్ణాంకం level) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_alloc_కొలత > 0);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_ZLIB
        పూర్ణాంకం r;

        r = dlopen_zlib(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత > UINT_MAX)
                తిరిగి -EFBIG;
        ఒకవేళ (dst_alloc_కొలత > UINT_MAX)
                dst_alloc_కొలత = UINT_MAX;

        _cleanup_(deflate_చివర_wrapper) z_ప్రవాహం s = {};

        r = sym_deflateInit2_(&s, level < 0 ? Z_DEFAULT_COMPRESSION : level,
                              /* method= */ Z_DEFLATED,
                              /* windowBits= */ ZLIB_WBITS_GZIP,
                              /* memLevel= */ 8,
                              /* strategy= */ Z_DEFAULT_STRATEGY,
                              ZLIB_సంస్కరణ, (పూర్ణాంకం) పరిమాణం(s));
        ఒకవేళ (r != Z_OK)
                తిరిగి -ENOMEM;

        s.తదుపరి_లోపల = (శూన్య*) src;
        s.avail_లోపల = src_కొలత;
        s.తదుపరి_బహిర్గతం = dst;
        s.avail_బహిర్గతం = dst_alloc_కొలత;

        r = sym_deflate(&s, Z_ముగింపు);
        ఒకవేళ (r != Z_ప్రవాహం_చివర)
                తిరిగి -ENOBUFS;

        *dst_కొలత = dst_alloc_కొలత - s.avail_బహిర్గతం;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం compress_blob_bzip2(
                స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                శూన్య *dst, size_t dst_alloc_కొలత, size_t *dst_కొలత, పూర్ణాంకం level) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_alloc_కొలత > 0);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_BZIP2
        పూర్ణాంకం r;

        r = dlopen_bzip2(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత > UINT_MAX)
                తిరిగి -EFBIG;
        ఒకవేళ (dst_alloc_కొలత > UINT_MAX)
                dst_alloc_కొలత = UINT_MAX;

        _cleanup_(BZ2_bz_Compress_చివర_wrapper) bz_ప్రవాహం s = {};

        r = sym_BZ2_bzCompressInit(&s, level < 0 ? 9 : level, /* verbosity= */ 0, /* workFactor= */ 0);
        ఒకవేళ (r != BZ_OK)
                తిరిగి -ENOMEM;

        s.తదుపరి_లోపల = (అక్షరం*) src;
        s.avail_లోపల = src_కొలత;
        s.తదుపరి_బహిర్గతం = (అక్షరం*) dst;
        s.avail_బహిర్గతం = dst_alloc_కొలత;

        r = sym_BZ2_bzCompress(&s, BZ_ముగింపు);

        ఒకవేళ (r != BZ_ప్రవాహం_చివర)
                తిరిగి -ENOBUFS;

        *dst_కొలత = dst_alloc_కొలత - s.avail_బహిర్గతం;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

పూర్ణాంకం compress_blob_journal(
                Compression compression,
                స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                శూన్య *dst, size_t dst_alloc_కొలత, size_t *dst_కొలత, పూర్ణాంకం level) {

        ఎంపిక (compression) {
        స్థితి COMPRESSION_XZ:
                తిరిగి compress_blob_xz(src, src_కొలత, dst, dst_alloc_కొలత, dst_కొలత, level);
        స్థితి COMPRESSION_LZ4:
                తిరిగి compress_blob_lz4(src, src_కొలత, dst, dst_alloc_కొలత, dst_కొలత, level);
        స్థితి COMPRESSION_ZSTD:
                తిరిగి compress_blob_zstd(src, src_కొలత, dst, dst_alloc_కొలత, dst_కొలత, level);
        అప్రమేయ:
                తిరిగి -EOPNOTSUPP;
        }
}

పూర్ణాంకం compress_blob(
                Compression compression,
                స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                శూన్య *dst, size_t dst_alloc_కొలత, size_t *dst_కొలత, పూర్ణాంకం level) {

        ఎంపిక (compression) {
        స్థితి COMPRESSION_GZIP:
                తిరిగి compress_blob_gzip(src, src_కొలత, dst, dst_alloc_కొలత, dst_కొలత, level);
        స్థితి COMPRESSION_BZIP2:
                తిరిగి compress_blob_bzip2(src, src_కొలత, dst, dst_alloc_కొలత, dst_కొలత, level);
        అప్రమేయ:
                తిరిగి compress_blob_journal(compression, src, src_కొలత, dst, dst_alloc_కొలత, dst_కొలత, level);
        }
}

స్థిర పూర్ణాంకం decompress_blob_xz(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **dst,
                size_t *dst_కొలత,
                size_t dst_max) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_XZ
        పూర్ణాంకం r;

        r = dlopen_xz(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_(lzma_చివర_wrapper) lzma_ప్రవాహం s = LZMA_ప్రవాహం_INIT;
        lzma_ret ret = sym_lzma_ప్రవాహం_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ఒకవేళ (ret != LZMA_OK)
                తిరిగి -ENOMEM;

        size_t space = MIN(src_కొలత * 2, dst_max ?: SIZE_MAX);
        ఒకవేళ (!greedy_realloc(dst, space, 1))
                తిరిగి -ENOMEM;

        s.తదుపరి_లోపల = src;
        s.avail_లోపల = src_కొలత;

        s.తదుపరి_బహిర్గతం = *dst;
        s.avail_బహిర్గతం = space;

        కొరకు (;;) {
                size_t వాడిన;

                ret = sym_lzma_సంకేతం(&s, LZMA_ముగింపు);
                ఒకవేళ (ret == LZMA_ప్రవాహం_చివర)
                        ఆపు;
                ఒకవేళ (ret != LZMA_OK)
                        తిరిగి -ENOMEM;

                ఒకవేళ (dst_max > 0 && (space - s.avail_బహిర్గతం) >= dst_max)
                        ఆపు;
                ఒకవేళ (dst_max > 0 && space == dst_max)
                        తిరిగి -ENOBUFS;

                వాడిన = space - s.avail_బహిర్గతం;
                /* Silence static analyzers, space is bounded by allocation size */
                ప్రతిజ్ఞా(space <= SIZE_MAX / 2);
                space = MIN(2 * space, dst_max ?: SIZE_MAX);
                ఒకవేళ (!greedy_realloc(dst, space, 1))
                        తిరిగి -ENOMEM;

                s.avail_బహిర్గతం = space - వాడిన;
                s.తదుపరి_బహిర్గతం = *(uint8_t**)dst + వాడిన;
        }

        *dst_కొలత = space - s.avail_బహిర్గతం;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_blob_lz4(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **dst,
                size_t *dst_కొలత,
                size_t dst_max) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_LZ4
        అక్షరం* బహిర్గతం;
        పూర్ణాంకం r, కొలత; /* LZ4 uses int for size */

        r = dlopen_lz4(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత <= 8)
                తిరిగి -EBADMSG;

        ఒకవేళ (src_కొలత - 8 > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -EFBIG;

        కొలత = unaligned_read_le64(src);
        ఒకవేళ (కొలత < 0 || (గుర్తులేని) కొలత != unaligned_read_le64(src))
                తిరిగి -EFBIG;
        ఒకవేళ (dst_max > 0 && (size_t) కొలత > dst_max)
                తిరిగి -ENOBUFS;
        బహిర్గతం = greedy_realloc(dst, కొలత, 1);
        ఒకవేళ (!బహిర్గతం)
                తిరిగి -ENOMEM;

        r = sym_LZ4_decompress_సురక్షిత((అక్షరం*)src + 8, బహిర్గతం, src_కొలత - 8, కొలత);
        ఒకవేళ (r < 0 || r != కొలత)
                తిరిగి -EBADMSG;

        *dst_కొలత = కొలత;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

size_t zstd_dstream_బహిర్గతం_కొలత(శూన్య) {
#ఒకవేళ HAVE_ZSTD
        ఒకవేళ (dlopen_zstd(LOG_DEBUG) < 0)
                తిరిగి 0;

        తిరిగి sym_ZSTD_DStream_బహిర్గతం_కొలత();
#లేకపోతే
        తిరిగి 0;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_blob_zstd(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **dst,
                size_t *dst_కొలత,
                size_t dst_max) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_ZSTD
        పూర్ణాంకం r;

        r = dlopen_zstd(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t, so
         * keep the value in that type until we've clamped it down to something that fits size_t. */
        గుర్తులేని దీర్ఘ దీర్ఘ కొలత = sym_ZSTD_get_చట్రం_Content_కొలత(src, src_కొలత);
        ఒకవేళ (కొలత == ZSTD_CONTENTSIZE_ERROR)
                తిరిగి -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN is returned when the frame header doesn't record the decompressed
         * size, which is the case e.g. for kernel images compressed with 'zstd'. Per the zstd
         * documentation this is not an error, it just means we don't know the output size upfront and
         * have to grow the output buffer as we decompress. */

        /* dst_max == 0 means "no limit"; fold that into a single ceiling up front so the rest of the
         * function can treat the unlimited case like any other rather than repeating the sentinel. */
        size_t cap = dst_max ?: SIZE_MAX;

        size_t space;
        ఒకవేళ (కొలత != ZSTD_CONTENTSIZE_UNKNOWN)
                /* The MIN clamps the (possibly 64-bit) content size to the ceiling, which is <= SIZE_MAX,
                 * so the result always fits size_t. */
                space = MAX(sym_ZSTD_DStream_బహిర్గతం_కొలత(), (size_t) MIN(కొలత, (గుర్తులేని దీర్ఘ దీర్ఘ) cap));
        లేకపోతే {
                /* Start from twice the compressed size, guarding the doubling against overflow, then
                 * clamp to the ceiling. */
                uint64_t twice = src_కొలత <= UINT64_MAX / 2 ? src_కొలత * 2 : UINT64_MAX;
                space = MAX(sym_ZSTD_DStream_బహిర్గతం_కొలత(), (size_t) MIN(twice, (uint64_t) cap));
        }

        ఒకవేళ (!greedy_realloc(dst, space, 1))
                తిరిగి -ENOMEM;
        space = MALLOC_SIZEOF_సురక్షిత(*dst);

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ఒకవేళ (!dctx)
                తిరిగి -ENOMEM;

        ZSTD_లోపల_బఫరు ఆదానం = {
                .src = src,
                .కొలత = src_కొలత,
        };
        ZSTD_బహిర్గతం_బఫరు output = {
                .dst = *dst,
                /* Never offer the decoder more room than the cap: greedy_realloc() over-allocates, so
                 * without this clamp a single ZSTD_decompressStream() call could write past dst_max
                 * before the output.pos >= cap check below fires. */
                .కొలత = MIN(space, cap),
        };

        కొరకు (;;) {
                size_t k = sym_ZSTD_decompress_ప్రవాహం(dctx, &output, &ఆదానం);
                ఒకవేళ (sym_ZSTD_isError(k))
                        తిరిగి log_debug_errno(zstd_ret_గమ్యం_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));

                /* output.size is clamped to MIN(space, cap) here and after every grow, and zstd keeps
                 * output.pos <= output.size, so output.pos never exceeds cap: this fires exactly when the
                 * caller's limit has been filled. */
                ఒకవేళ (output.pos >= cap)
                        ఆపు;

                /* k == 0 means the current frame was fully decoded and flushed. If no input is left
                 * either, the whole stream is done. This must be checked before the grow branch below:
                 * the frame can finish exactly as the output buffer fills, and growing then would call
                 * the decoder again on already-exhausted input, which it'd misread as a truncated next
                 * frame. */
                ఒకవేళ (k == 0 && ఆదానం.pos >= ఆదానం.కొలత)
                        ఆపు;

                /* Otherwise there is still work to do: either decoded data buffered inside the decoder
                 * that didn't fit (k != 0, e.g. zstd's buffered-flush mode), or another concatenated
                 * frame to decode (k == 0 with input left). If the output buffer is full, grow it and
                 * call again. Guard against the doubling overflowing size_t (only reachable with an
                 * uncapped dst_max once the allocation has already grown past SIZE_MAX/2). */
                ఒకవేళ (output.pos == output.కొలత) {
                        ఒకవేళ (space > SIZE_MAX / 2)
                                తిరిగి -E2BIG;
                        space = MIN(2 * space, cap);
                        ఒకవేళ (!greedy_realloc(dst, space, 1))
                                తిరిగి -ENOMEM;
                        space = MALLOC_SIZEOF_సురక్షిత(*dst);
                        output.dst = *dst;
                        output.కొలత = MIN(space, cap);
                        కొనసాగు;
                }

                /* The output buffer still has room but the decoder stopped. If input is exhausted the
                 * decoder wanted more (k != 0, since k == 0 + no input broke above), i.e. the stream was
                 * truncated mid-frame. Otherwise input remains (a concatenated frame) and we loop. */
                ఒకవేళ (ఆదానం.pos >= ఆదానం.కొలత)
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
        }

        *dst_కొలత = output.pos;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_blob_gzip(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **dst,
                size_t *dst_కొలత,
                size_t dst_max) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_ZLIB
        పూర్ణాంకం r;

        r = dlopen_zlib(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత > UINT_MAX)
                తిరిగి -EFBIG;

        _cleanup_(inflate_చివర_wrapper) z_ప్రవాహం s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_సంస్కరణ, (పూర్ణాంకం) పరిమాణం(s));
        ఒకవేళ (r != Z_OK)
                తిరిగి -ENOMEM;

        size_t space = MIN3(src_కొలత * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ఒకవేళ (!greedy_realloc(dst, space, 1))
                తిరిగి -ENOMEM;

        s.తదుపరి_లోపల = (శూన్య*) src;
        s.avail_లోపల = src_కొలత;
        s.తదుపరి_బహిర్గతం = *dst;
        s.avail_బహిర్గతం = space;

        కొరకు (;;) {
                size_t వాడిన;

                r = sym_inflate(&s, Z_NO_FLUSH);
                ఒకవేళ (r == Z_ప్రవాహం_చివర)
                        ఆపు;
                ఒకవేళ (!లోపల_సమితి(r, Z_OK, Z_BUF_ERROR))
                        తిరిగి -EBADMSG;

                ఒకవేళ (dst_max > 0 && (space - s.avail_బహిర్గతం) >= dst_max)
                        ఆపు;
                ఒకవేళ (dst_max > 0 && space == dst_max)
                        తిరిగి -ENOBUFS;

                వాడిన = space - s.avail_బహిర్గతం;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, UINT_MAX);
                ఒకవేళ (!greedy_realloc(dst, space, 1))
                        తిరిగి -ENOMEM;

                s.avail_బహిర్గతం = space - వాడిన;
                s.తదుపరి_బహిర్గతం = *(uint8_t**)dst + వాడిన;
        }

        *dst_కొలత = space - s.avail_బహిర్గతం;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_blob_bzip2(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **dst,
                size_t *dst_కొలత,
                size_t dst_max) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(dst);
        ప్రతిజ్ఞా(dst_కొలత);

#ఒకవేళ HAVE_BZIP2
        పూర్ణాంకం r;

        r = dlopen_bzip2(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత > UINT_MAX)
                తిరిగి -EFBIG;

        _cleanup_(BZ2_bz_Decompress_చివర_wrapper) bz_ప్రవాహం s = {};

        ఒకవేళ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                తిరిగి -EIO;

        size_t space = MIN3(src_కొలత * 2, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
        ఒకవేళ (!greedy_realloc(dst, space, 1))
                తిరిగి -ENOMEM;

        s.తదుపరి_లోపల = (అక్షరం*) src;
        s.avail_లోపల = src_కొలత;
        s.తదుపరి_బహిర్గతం = (అక్షరం*) *dst;
        s.avail_బహిర్గతం = space;

        కొరకు (;;) {
                size_t వాడిన;

                r = sym_BZ2_bzDecompress(&s);
                ఒకవేళ (r == BZ_ప్రవాహం_చివర)
                        ఆపు;
                ఒకవేళ (r != BZ_OK)
                        తిరిగి -EBADMSG;

                ఒకవేళ (dst_max > 0 && (space - s.avail_బహిర్గతం) >= dst_max)
                        ఆపు;
                ఒకవేళ (dst_max > 0 && space == dst_max)
                        తిరిగి -ENOBUFS;

                వాడిన = space - s.avail_బహిర్గతం;
                space = MIN3(2 * space, dst_max ?: SIZE_MAX, (size_t) UINT_MAX);
                ఒకవేళ (!greedy_realloc(dst, space, 1))
                        తిరిగి -ENOMEM;

                s.avail_బహిర్గతం = space - వాడిన;
                s.తదుపరి_బహిర్గతం = (అక్షరం*) *dst + వాడిన;
        }

        *dst_కొలత = space - s.avail_బహిర్గతం;
        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

పూర్ణాంకం decompress_blob_journal(
                Compression compression,
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **dst,
                size_t *dst_కొలత,
                size_t dst_max) {

        ఎంపిక (compression) {
        స్థితి COMPRESSION_XZ:
                తిరిగి decompress_blob_xz(
                                src, src_కొలత,
                                dst, dst_కొలత, dst_max);
        స్థితి COMPRESSION_LZ4:
                తిరిగి decompress_blob_lz4(
                                src, src_కొలత,
                                dst, dst_కొలత, dst_max);
        స్థితి COMPRESSION_ZSTD:
                తిరిగి decompress_blob_zstd(
                                src, src_కొలత,
                                dst, dst_కొలత, dst_max);
        అప్రమేయ:
                తిరిగి -EPROTONOSUPPORT;
        }
}

పూర్ణాంకం decompress_blob(
                Compression compression,
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **dst,
                size_t *dst_కొలత,
                size_t dst_max) {

        ఎంపిక (compression) {
        స్థితి COMPRESSION_GZIP:
                తిరిగి decompress_blob_gzip(
                                src, src_కొలత,
                                dst, dst_కొలత, dst_max);
        స్థితి COMPRESSION_BZIP2:
                తిరిగి decompress_blob_bzip2(
                                src, src_కొలత,
                                dst, dst_కొలత, dst_max);
        అప్రమేయ:
                తిరిగి decompress_blob_journal(
                                compression,
                                src, src_కొలత,
                                dst, dst_కొలత, dst_max);
        }
}

పూర్ణాంకం decompress_zlib_raw(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య *dst,
                size_t dst_కొలత,
                పూర్ణాంకం wbits) {

#ఒకవేళ HAVE_ZLIB
        పూర్ణాంకం r;

        r = dlopen_zlib(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత > UINT_MAX)
                తిరిగి -EFBIG;
        ఒకవేళ (dst_కొలత > UINT_MAX)
                తిరిగి -EFBIG;

        _cleanup_(inflate_చివర_wrapper) z_ప్రవాహం s = {
                .తదుపరి_లోపల = (శూన్య*) src,
                .avail_లోపల = src_కొలత,
                .తదుపరి_బహిర్గతం = dst,
                .avail_బహిర్గతం = dst_కొలత,
        };

        r = sym_inflateInit2_(&s, /* windowBits= */ wbits, ZLIB_సంస్కరణ, (పూర్ణాంకం) పరిమాణం(s));
        ఒకవేళ (r != Z_OK)
                తిరిగి -EIO;

        r = sym_inflate(&s, Z_ముగింపు);
        size_t produced = (uint8_t*) s.తదుపరి_బహిర్గతం - (uint8_t*) dst;

        ఒకవేళ (r != Z_ప్రవాహం_చివర || produced != dst_కొలత)
                తిరిగి -EBADMSG;

        తిరిగి 0;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_startswith_xz(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **బఫరు,
                స్థిరరాశి శూన్య *ఉపసర్గ,
                size_t ఉపసర్గ_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(ఉపసర్గ);

#ఒకవేళ HAVE_XZ
        పూర్ణాంకం r;

        r = dlopen_xz(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_(lzma_చివర_wrapper) lzma_ప్రవాహం s = LZMA_ప్రవాహం_INIT;
        lzma_ret ret = sym_lzma_ప్రవాహం_decoder(&s, UINT64_MAX, /* flags= */ 0);
        ఒకవేళ (ret != LZMA_OK)
                తిరిగి -EBADMSG;

        ఒకవేళ (!(greedy_realloc(బఫరు, ALIGN_8(ఉపసర్గ_len + 1), 1)))
                తిరిగి -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_సురక్షిత(*బఫరు);

        s.తదుపరి_లోపల = src;
        s.avail_లోపల = src_కొలత;

        s.తదుపరి_బహిర్గతం = *బఫరు;
        s.avail_బహిర్గతం = allocated;

        కొరకు (;;) {
                ret = sym_lzma_సంకేతం(&s, LZMA_ముగింపు);

                ఒకవేళ (!లోపల_సమితి(ret, LZMA_OK, LZMA_ప్రవాహం_చివర))
                        తిరిగి -EBADMSG;

                ఒకవేళ (allocated - s.avail_బహిర్గతం >= ఉపసర్గ_len + 1)
                        తిరిగి memcmp(*బఫరు, ఉపసర్గ, ఉపసర్గ_len) == 0 &&
                                ((స్థిరరాశి uint8_t*) *బఫరు)[ఉపసర్గ_len] == extra;

                ఒకవేళ (ret == LZMA_ప్రవాహం_చివర)
                        తిరిగి 0;

                s.avail_బహిర్గతం += allocated;

                ఒకవేళ (!(greedy_realloc(బఫరు, allocated * 2, 1)))
                        తిరిగి -ENOMEM;

                allocated = MALLOC_SIZEOF_సురక్షిత(*బఫరు);
                s.తదుపరి_బహిర్గతం = *(uint8_t**)బఫరు + allocated - s.avail_బహిర్గతం;
        }

#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_startswith_lz4(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **బఫరు,
                స్థిరరాశి శూన్య *ఉపసర్గ,
                size_t ఉపసర్గ_len,
                uint8_t extra) {

        /* Checks whether the decompressed blob starts with the mentioned prefix. The byte extra needs to
         * follow the prefix */

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(ఉపసర్గ);

#ఒకవేళ HAVE_LZ4
        size_t allocated;
        పూర్ణాంకం r;

        r = dlopen_lz4(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత <= 8)
                తిరిగి -EBADMSG;

        ఒకవేళ (src_కొలత - 8 > గరిష్ఠ_పూర్ణాంకం)
                తిరిగి -EFBIG;

        ఒకవేళ (!(greedy_realloc(బఫరు, ALIGN_8(ఉపసర్గ_len + 1), 1)))
                తిరిగి -ENOMEM;
        allocated = MALLOC_SIZEOF_సురక్షిత(*బఫరు);

        r = sym_LZ4_decompress_సురక్షిత_partial(
                        (అక్షరం*)src + 8,
                        *బఫరు,
                        src_కొలత - 8,
                        ఉపసర్గ_len + 1,
                        allocated);

        /* One lz4 < 1.8.3, we might get "failure" (r < 0), or "success" where just a part of the buffer is
         * decompressed. But if we get a smaller amount of bytes than requested, we don't know whether there
         * isn't enough data to fill the requested size or whether we just got a partial answer.
         */
        ఒకవేళ (r < 0 || (size_t) r < ఉపసర్గ_len + 1) {
                size_t కొలత;

                ఒకవేళ (sym_LZ4_సంస్కరణ_సంఖ్య() >= 10803)
                        /* We trust that the newer lz4 decompresses the number of bytes we
                         * requested if available in the compressed string. */
                        తిరిగి 0;

                ఒకవేళ (r > 0)
                        /* Compare what we have first, in case of mismatch we can
                         * shortcut the full comparison. */
                        ఒకవేళ (memcmp(*బఫరు, ఉపసర్గ, r) != 0)
                                తిరిగి 0;

                /* Before version 1.8.3, lz4 always tries to decode full a "sequence",
                 * so in pathological cases might need to decompress the full field. */
                r = decompress_blob_lz4(src, src_కొలత, బఫరు, &కొలత, 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (కొలత < ఉపసర్గ_len + 1)
                        తిరిగి 0;
        }

        తిరిగి memcmp(*బఫరు, ఉపసర్గ, ఉపసర్గ_len) == 0 &&
                ((స్థిరరాశి uint8_t*) *బఫరు)[ఉపసర్గ_len] == extra;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_startswith_zstd(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **బఫరు,
                స్థిరరాశి శూన్య *ఉపసర్గ,
                size_t ఉపసర్గ_len,
                uint8_t extra) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(ఉపసర్గ);

#ఒకవేళ HAVE_ZSTD
        పూర్ణాంకం r;

        r = dlopen_zstd(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* ZSTD_getFrameContentSize() returns unsigned long long, which is not necessarily size_t. */
        గుర్తులేని దీర్ఘ దీర్ఘ కొలత = sym_ZSTD_get_చట్రం_Content_కొలత(src, src_కొలత);
        ఒకవేళ (కొలత == ZSTD_CONTENTSIZE_ERROR)
                తిరిగి -EBADMSG;

        /* ZSTD_CONTENTSIZE_UNKNOWN means the frame doesn't record the decompressed size, so we can't
         * shortcut here. The single decompression below only needs prefix_len + 1 bytes anyway. */
        ఒకవేళ (కొలత != ZSTD_CONTENTSIZE_UNKNOWN && కొలత < ఉపసర్గ_len + 1)
                తిరిగి 0; /* Decompressed text too short to match the prefix and extra */

        _cleanup_(ZSTD_freeDCtxp) ZSTD_DCtx *dctx = sym_ZSTD_createDCtx();
        ఒకవేళ (!dctx)
                తిరిగి -ENOMEM;

        ఒకవేళ (!(greedy_realloc(బఫరు, MAX(sym_ZSTD_DStream_బహిర్గతం_కొలత(), ఉపసర్గ_len + 1), 1)))
                తిరిగి -ENOMEM;

        ZSTD_లోపల_బఫరు ఆదానం = {
                .src = src,
                .కొలత = src_కొలత,
        };
        ZSTD_బహిర్గతం_బఫరు output = {
                .dst = *బఫరు,
                .కొలత = MALLOC_SIZEOF_సురక్షిత(*బఫరు),
        };
        size_t k;

        k = sym_ZSTD_decompress_ప్రవాహం(dctx, &output, &ఆదానం);
        ఒకవేళ (sym_ZSTD_isError(k))
                తిరిగి log_debug_errno(zstd_ret_గమ్యం_errno(k), "ZSTD decoder failed: %s", sym_ZSTD_getErrorName(k));
        ఒకవేళ (output.pos < ఉపసర్గ_len + 1) {
                /* The output buffer is at least prefix_len + 1 bytes, so if it isn't full the decoder
                 * stopped for another reason: either the frame ended (k == 0), in which case the stream
                 * is simply too short to match the prefix, or it's still expecting input (k != 0), which
                 * means the stream was truncated mid-frame. */
                ఒకవేళ (k != 0)
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "ZSTD stream ended unexpectedly, probably corrupted.");
                తిరిగి 0; /* Decompressed text too short to match the prefix and extra */
        }

        తిరిగి memcmp(*బఫరు, ఉపసర్గ, ఉపసర్గ_len) == 0 &&
                ((స్థిరరాశి uint8_t*) *బఫరు)[ఉపసర్గ_len] == extra;
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_startswith_gzip(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **బఫరు,
                స్థిరరాశి శూన్య *ఉపసర్గ,
                size_t ఉపసర్గ_len,
                uint8_t extra) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(ఉపసర్గ);

#ఒకవేళ HAVE_ZLIB
        పూర్ణాంకం r;

        r = dlopen_zlib(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత > UINT_MAX)
                తిరిగి -EFBIG;

        _cleanup_(inflate_చివర_wrapper) z_ప్రవాహం s = {};

        r = sym_inflateInit2_(&s, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_సంస్కరణ, (పూర్ణాంకం) పరిమాణం(s));
        ఒకవేళ (r != Z_OK)
                తిరిగి -EBADMSG;

        ఒకవేళ (!(greedy_realloc(బఫరు, ALIGN_8(ఉపసర్గ_len + 1), 1)))
                తిరిగి -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_సురక్షిత(*బఫరు);

        s.తదుపరి_లోపల = (శూన్య*) src;
        s.avail_లోపల = src_కొలత;

        s.తదుపరి_బహిర్గతం = *బఫరు;
        s.avail_బహిర్గతం = MIN(allocated, (size_t) UINT_MAX);

        కొరకు (;;) {
                r = sym_inflate(&s, Z_ముగింపు);

                ఒకవేళ (!లోపల_సమితి(r, Z_OK, Z_ప్రవాహం_చివర, Z_BUF_ERROR))
                        తిరిగి -EBADMSG;

                ఒకవేళ (allocated - s.avail_బహిర్గతం >= ఉపసర్గ_len + 1)
                        తిరిగి memcmp(*బఫరు, ఉపసర్గ, ఉపసర్గ_len) == 0 &&
                                ((స్థిరరాశి uint8_t*) *బఫరు)[ఉపసర్గ_len] == extra;

                ఒకవేళ (r == Z_ప్రవాహం_చివర)
                        తిరిగి 0;

                ఒకవేళ (s.avail_బహిర్గతం > 0)
                        తిరిగి -EBADMSG; /* truncated */

                size_t వాడిన = allocated - s.avail_బహిర్గతం;

                ఒకవేళ (!(greedy_realloc(బఫరు, allocated * 2, 1)))
                        తిరిగి -ENOMEM;

                allocated = MALLOC_SIZEOF_సురక్షిత(*బఫరు);
                s.avail_బహిర్గతం = MIN(allocated - వాడిన, (size_t) UINT_MAX);
                s.తదుపరి_బహిర్గతం = *(uint8_t**)బఫరు + వాడిన;
        }
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం decompress_startswith_bzip2(
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                శూన్య **బఫరు,
                స్థిరరాశి శూన్య *ఉపసర్గ,
                size_t ఉపసర్గ_len,
                uint8_t extra) {

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(src_కొలత > 0);
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(ఉపసర్గ);

#ఒకవేళ HAVE_BZIP2
        పూర్ణాంకం r;

        r = dlopen_bzip2(LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (src_కొలత > UINT_MAX)
                తిరిగి -EFBIG;

        _cleanup_(BZ2_bz_Decompress_చివర_wrapper) bz_ప్రవాహం s = {};

        ఒకవేళ (sym_BZ2_bzDecompressInit(&s, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                తిరిగి -EIO;

        ఒకవేళ (!(greedy_realloc(బఫరు, ALIGN_8(ఉపసర్గ_len + 1), 1)))
                తిరిగి -ENOMEM;

        size_t allocated = MALLOC_SIZEOF_సురక్షిత(*బఫరు);

        s.తదుపరి_లోపల = (అక్షరం*) src;
        s.avail_లోపల = src_కొలత;

        s.తదుపరి_బహిర్గతం = *బఫరు;
        s.avail_బహిర్గతం = MIN(allocated, (size_t) UINT_MAX);

        కొరకు (;;) {
                r = sym_BZ2_bzDecompress(&s);

                ఒకవేళ (!లోపల_సమితి(r, BZ_OK, BZ_ప్రవాహం_చివర))
                        తిరిగి -EBADMSG;

                ఒకవేళ (allocated - s.avail_బహిర్గతం >= ఉపసర్గ_len + 1)
                        తిరిగి memcmp(*బఫరు, ఉపసర్గ, ఉపసర్గ_len) == 0 &&
                                ((స్థిరరాశి uint8_t*) *బఫరు)[ఉపసర్గ_len] == extra;

                ఒకవేళ (r == BZ_ప్రవాహం_చివర)
                        తిరిగి 0;

                ఒకవేళ (s.avail_బహిర్గతం > 0)
                        తిరిగి -EBADMSG; /* truncated */

                size_t వాడిన = allocated - s.avail_బహిర్గతం;

                ఒకవేళ (!(greedy_realloc(బఫరు, allocated * 2, 1)))
                        తిరిగి -ENOMEM;

                allocated = MALLOC_SIZEOF_సురక్షిత(*బఫరు);
                s.avail_బహిర్గతం = MIN(allocated - వాడిన, (size_t) UINT_MAX);
                s.తదుపరి_బహిర్గతం = (అక్షరం*) *బఫరు + వాడిన;
        }
#లేకపోతే
        తిరిగి -EPROTONOSUPPORT;
#అయితే_ముగింపు
}

పూర్ణాంకం decompress_startswith_journal(
                Compression compression,
                స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                శూన్య **బఫరు,
                స్థిరరాశి శూన్య *ఉపసర్గ, size_t ఉపసర్గ_len,
                uint8_t extra) {

        ఎంపిక (compression) {
        స్థితి COMPRESSION_XZ:
                తిరిగి decompress_startswith_xz(src, src_కొలత, బఫరు, ఉపసర్గ, ఉపసర్గ_len, extra);
        స్థితి COMPRESSION_LZ4:
                తిరిగి decompress_startswith_lz4(src, src_కొలత, బఫరు, ఉపసర్గ, ఉపసర్గ_len, extra);
        స్థితి COMPRESSION_ZSTD:
                తిరిగి decompress_startswith_zstd(src, src_కొలత, బఫరు, ఉపసర్గ, ఉపసర్గ_len, extra);
        అప్రమేయ:
                తిరిగి -EOPNOTSUPP;
        }
}

పూర్ణాంకం decompress_startswith(
                Compression compression,
                స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                శూన్య **బఫరు,
                స్థిరరాశి శూన్య *ఉపసర్గ, size_t ఉపసర్గ_len,
                uint8_t extra) {

        ఎంపిక (compression) {
        స్థితి COMPRESSION_GZIP:
                తిరిగి decompress_startswith_gzip(src, src_కొలత, బఫరు, ఉపసర్గ, ఉపసర్గ_len, extra);
        స్థితి COMPRESSION_BZIP2:
                తిరిగి decompress_startswith_bzip2(src, src_కొలత, బఫరు, ఉపసర్గ, ఉపసర్గ_len, extra);
        అప్రమేయ:
                తిరిగి decompress_startswith_journal(compression, src, src_కొలత, బఫరు, ఉపసర్గ, ఉపసర్గ_len, extra);
        }
}

పూర్ణాంకం compress_ప్రవాహం(
                Compression type,
                పూర్ణాంకం fdf, పూర్ణాంకం fdt,
                uint64_t max_bytes,
                uint64_t *ret_uncompressed_కొలత) {

        _cleanup_(compressor_freep) Compressor *c = ఖాళీ;
        _cleanup_free_ శూన్య *buf = ఖాళీ;
        _cleanup_free_ uint8_t *ఆదానం = ఖాళీ;
        size_t buf_size = 0, buf_alloc = 0;
        uint64_t మొత్తం_లోపల = 0, మొత్తం_బహిర్గతం = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fdf >= 0);
        ప్రతిజ్ఞా(fdt >= 0);

        r = compressor_new(&c, type);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఆదానం = new(uint8_t, COMPRESS_PIPE_బఫరు_కొలత);
        ఒకవేళ (!ఆదానం)
                తిరిగి -ENOMEM;

        కొరకు (;;) {
                size_t m = COMPRESS_PIPE_బఫరు_కొలత;
                ssize_t n;

                ఒకవేళ (max_bytes != UINT64_MAX && (uint64_t) m > max_bytes)
                        m = (size_t) max_bytes;

                n = పఠ(fdf, ఆదానం, m);
                ఒకవేళ (n < 0)
                        తిరిగి -errno;

                ఒకవేళ (n == 0) {
                        r = compressor_ముగింపు(c, &buf, &buf_size, &buf_alloc);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        ఒకవేళ (buf_size > 0) {
                                r = loop_write(fdt, buf, buf_size);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                                మొత్తం_బహిర్గతం += buf_size;
                        }
                        ఆపు;
                }

                మొత్తం_లోపల += n;
                ఒకవేళ (max_bytes != UINT64_MAX) {
                        ప్రతిజ్ఞా(max_bytes >= (uint64_t) n);
                        max_bytes -= n;
                }

                r = compressor_ప్రారంభం(c, ఆదానం, n, &buf, &buf_size, &buf_alloc);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (buf_size > 0) {
                        r = loop_write(fdt, buf, buf_size);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        మొత్తం_బహిర్గతం += buf_size;
                }
        }

        ఒకవేళ (ret_uncompressed_కొలత)
                *ret_uncompressed_కొలత = మొత్తం_లోపల;

        ఒకవేళ (మొత్తం_లోపల == 0)
                log_debug("%s compression finished (no input data)", compression_గమ్యం_string(type));
        లేకపోతే
                log_debug("%s compression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                          compression_గమ్యం_string(type), మొత్తం_లోపల, మొత్తం_బహిర్గతం, (ద్విగుణ) మొత్తం_బహిర్గతం / మొత్తం_లోపల * 100);

        తిరిగి 0;
}

స్థిర పూర్ణాంకం should_sparse(పూర్ణాంకం fd) {
        నిర్మాణం stat st;

        ప్రతిజ్ఞా(fd >= 0);

        /* Determine whether sparse writes should be used for this fd. Sparse writes are only safe on regular
         * files, without O_APPEND (which ignores the lseek position, collapsing any holes we try to create),
         * and only when starting from a position at or beyond the current end of file (otherwise the skipped
         * ranges would overlap pre-existing data instead of being genuinely unwritten). */

        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        ఒకవేళ (!S_ISREG(st.st_mode))
                తిరిగి false;

        పూర్ణాంకం flags = fcntl(fd, F_GETFL);
        ఒకవేళ (flags < 0)
                తిరిగి -errno;

        ఒకవేళ (FLAGS_సమితి(flags, O_APPEND))
                తిరిగి false;

        off_t pos = lseek(fd, 0, SEEK_CUR);
        ఒకవేళ (pos < 0)
                తిరిగి -errno;

        తిరిగి pos >= st.st_size;
}

/* After sparse decompression, set the file size to the current position to account for
 * trailing holes that sparse_write() created via lseek but never extended the file size for. */
స్థిర పూర్ణాంకం finalize_sparse(పూర్ణాంకం fd) {
        off_t pos;

        ప్రతిజ్ఞా(fd >= 0);

        pos = lseek(fd, 0, SEEK_CUR);
        ఒకవేళ (pos < 0)
                తిరిగి -errno;

        ఒకవేళ (ftruncate(fd, pos) < 0)
                తిరిగి -errno;

        తిరిగి 0;
}

/* Common helper for decompress_stream_*() wrappers */

నిర్మాణం decompress_ప్రవాహం_userdata {
        పూర్ణాంకం fd;
        uint64_t max_bytes;
        uint64_t మొత్తం_బహిర్గతం;
        uint64_t skip; /* bytes to discard from the very start of the decompressed stream before anything is
                        * actually written */
        bool sparse;
};

స్థిర పూర్ణాంకం decompress_ప్రవాహం_write_callback(స్థిరరాశి శూన్య *దత్తాంశం, size_t కొలత, శూన్య *userdata) {
        నిర్మాణం decompress_ప్రవాహం_userdata *u = ASSERT_PTR(userdata);

        /* Accounting (max_bytes/total_out) happens against the *full* decompressed stream, including the
         * part we're about to skip. */
        ఒకవేళ (u->max_bytes != UINT64_MAX) {
                ఒకవేళ (u->max_bytes < కొలత)
                        తిరిగి -EFBIG;
                u->max_bytes -= కొలత;
        }

        u->మొత్తం_బహిర్గతం += కొలత;

        ఒకవేళ (u->skip > 0) {
                ఒకవేళ (u->skip >= కొలత) {
                        u->skip -= కొలత;
                        తిరిగి 0;
                }

                దత్తాంశం = (స్థిరరాశి uint8_t*) దత్తాంశం + u->skip;
                కొలత -= u->skip;
                u->skip = 0;
        }

        ఒకవేళ (కొలత == 0)
                తిరిగి 0;

        ఒకవేళ (u->sparse) {
                /* Note: sparse_write() does not retry on EINTR and converts short writes to -EIO.
                 * This is fine here since sparse mode is only used on regular files, where short
                 * writes and EINTR are not expected in practice. */
                ssize_t k = sparse_write(u->fd, దత్తాంశం, కొలత, 64);
                ఒకవేళ (k < 0)
                        తిరిగి (పూర్ణాంకం) k;
                తిరిగి 0;
        }

        తిరిగి loop_write(u->fd, దత్తాంశం, కొలత);
}

స్థిర పూర్ణాంకం decompressor_new(Decompressor **ret, Compression type, DecompressFlags flags) {
        ప్రతిజ్ఞా(ret);

        _cleanup_(compressor_freep) Decompressor *c = new0(Decompressor, 1);
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        c->type = _COMPRESSION_INVALID;

        ఎంపిక (type) {
        స్థితి COMPRESSION_NONE:
                ఆపు;

#ఒకవేళ HAVE_XZ
        స్థితి COMPRESSION_XZ: {
                uint32_t xz_flags = LZMA_TELL_UNSUPPORTED_CHECK |
                        (FLAGS_సమితి(flags, DECOMPRESS_REFUSE_CONCATENATED) ? 0 : LZMA_CONCATENATED);
                lzma_ret xz_ret = FLAGS_సమితి(flags, DECOMPRESS_LEGACY_LZMA) ?
                        sym_lzma_auto_decoder(&c->xz, UINT64_MAX, xz_flags) :
                        sym_lzma_ప్రవాహం_decoder(&c->xz, UINT64_MAX, xz_flags);
                ఒకవేళ (xz_ret != LZMA_OK)
                        తిరిగి -EIO;
                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
        స్థితి COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createDecompressionContext(&c->d_lz4, LZ4_F_సంస్కరణ);
                ఒకవేళ (sym_LZ4F_isError(rc))
                        తిరిగి -ENOMEM;

                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
        స్థితి COMPRESSION_ZSTD:
                c->d_zstd = sym_ZSTD_createDCtx();
                ఒకవేళ (!c->d_zstd)
                        తిరిగి -ENOMEM;
                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
        స్థితి COMPRESSION_GZIP:
                ఒకవేళ (sym_inflateInit2_(&c->gzip, /* windowBits= */ ZLIB_WBITS_GZIP, ZLIB_సంస్కరణ, (పూర్ణాంకం) పరిమాణం(c->gzip)) != Z_OK)
                        తిరిగి -EIO;
                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_BZIP2
        స్థితి COMPRESSION_BZIP2:
                ఒకవేళ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                        తిరిగి -EIO;
                ఆపు;
#అయితే_ముగింపు

        అప్రమేయ:
                assert_not_reached();
        }

        c->type = type;
        c->encoding = false;
        *ret = TAKE_PTR(c);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం decompress_blob_గమ్యం_fd_impl(
                Compression type,
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                పూర్ణాంకం fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_కొలత) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(src);
        ప్రతిజ్ఞా(fdt >= 0);

        ఒకవేళ (type == COMPRESSION_NONE)
                తిరిగి -EOPNOTSUPP;

        ఒకవేళ (src_కొలత == 0)
                తిరిగి -EBADMSG;

        /* LZ4 is special: compress_blob() produces the raw LZ4 block format (with a custom 8-byte size
         * header), while the streaming decompressor infra (decompressor_push()) expects LZ4 Frame format, as
         * produced by compress_stream(). The two are not interchangeable, so route LZ4 blobs through the raw
         * block decoder instead. */
        ఒకవేళ (type == COMPRESSION_LZ4 &&
            src_కొలత >= COMPRESSION_MAGIC_బైట్లు_MAX &&
            compression_detect_from_magic(src) != COMPRESSION_LZ4) {

                /* Hopefully, the data is produced by compress_blob(). In that case, let's use
                 * decompress_blob_lz4() + loop_write(). */

                ఒకవేళ (skip > 0) {
                        /* Check if the result size is larger than skip. */

                        ఒకవేళ (src_కొలత <= పరిమాణం(uint64_t))
                                తిరిగి -EBADMSG;

                        uint64_t sz = unaligned_read_le64(src);
                        ఒకవేళ (sz < skip)
                                తిరిగి -EBADMSG;
                }

                /* max_bytes == 0 means do not write anything, but decompress_blob_lz4() with
                 * dst_max=0 means no size limit. Let's return earlier when max_bytes == 0. */
                ఒకవేళ (max_bytes == 0)
                        తిరిగి -ENOBUFS;

                _cleanup_free_ శూన్య *buf = ఖాళీ;
                size_t కొలత = 0;  /* avoid false maybe-uninitialized warning */
                r = decompress_blob_lz4(src, src_కొలత, &buf, &కొలత,
                                        max_bytes >= SIZE_MAX ? 0 : (size_t) max_bytes);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                /* Check the result size again for safety. */
                ఒకవేళ (కొలత < skip)
                        తిరిగి -EBADMSG;

                r = loop_write(fdt, (uint8_t*) buf + skip, కొలత - skip);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                log_debug("lz4 decompression finished (%" PRIu64 " -> %zu bytes, %.1f%%)",
                          src_కొలత, కొలత, (ద్విగుణ) కొలత / src_కొలత * 100);

                ఒకవేళ (ret_కొలత)
                        *ret_కొలత = కొలత - skip;
                తిరిగి 0;
        }

        _cleanup_(compressor_freep) Decompressor *c = ఖాళీ;
        r = decompressor_new(&c, type, DECOMPRESS_REFUSE_CONCATENATED);
        ఒకవేళ (r < 0)
                తిరిగి r;

        నిర్మాణం decompress_ప్రవాహం_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .skip = skip,
                .sparse = should_sparse(fdt) > 0,
        };

        r = decompressor_నెట్టు(c, src, src_కొలత, decompress_ప్రవాహం_write_callback, &userdata);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Finalize */
        r = decompressor_నెట్టు(c, /* data= */ ఖాళీ, /* size= */ 0, decompress_ప్రవాహం_write_callback, &userdata);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (userdata.మొత్తం_బహిర్గతం < skip)
                తిరిగి -EBADMSG;

        ఒకవేళ (userdata.sparse) {
                r = finalize_sparse(fdt);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_గమ్యం_string(type), src_కొలత, userdata.మొత్తం_బహిర్గతం,
                  (ద్విగుణ) userdata.మొత్తం_బహిర్గతం / src_కొలత * 100);

        ఒకవేళ (ret_కొలత)
                *ret_కొలత = userdata.మొత్తం_బహిర్గతం - skip;

        తిరిగి 0;
}

పూర్ణాంకం decompress_blob_గమ్యం_fd_journal(
                Compression type,
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                పూర్ణాంకం fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_కొలత) {

        పూర్ణాంకం r;

        r = dlopen_compress_journal(type, LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి decompress_blob_గమ్యం_fd_impl(type, src, src_కొలత, fdt, max_bytes, skip, ret_కొలత);
}

పూర్ణాంకం decompress_blob_గమ్యం_fd(
                Compression type,
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                పూర్ణాంకం fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_కొలత) {

        పూర్ణాంకం r;

        r = dlopen_compress(type, LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి decompress_blob_గమ్యం_fd_impl(type, src, src_కొలత, fdt, max_bytes, skip, ret_కొలత);
}

పూర్ణాంకం decompress_ప్రవాహం_నిండిన(
                Compression type,
                పూర్ణాంకం fdf, పూర్ణాంకం fdt,
                uint64_t max_bytes,
                DecompressFlags flags) {

        _cleanup_(compressor_freep) Decompressor *c = ఖాళీ;
        _cleanup_free_ uint8_t *buf = ఖాళీ;
        uint64_t మొత్తం_లోపల = 0;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fdf >= 0);
        ప్రతిజ్ఞా(fdt >= 0);

        ఒకవేళ (type == COMPRESSION_NONE)
                తిరిగి -EOPNOTSUPP;

        r = dlopen_compress(type, LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = decompressor_new(&c, type, flags);
        ఒకవేళ (r < 0)
                తిరిగి r;

        నిర్మాణం decompress_ప్రవాహం_userdata userdata = {
                .fd = fdt,
                .max_bytes = max_bytes,
                .sparse = should_sparse(fdt) > 0,
        };

        buf = new(uint8_t, COMPRESS_PIPE_బఫరు_కొలత);
        ఒకవేళ (!buf)
                తిరిగి -ENOMEM;

        కొరకు (;;) {
                ssize_t n;

                n = పఠ(fdf, buf, COMPRESS_PIPE_బఫరు_కొలత);
                ఒకవేళ (n < 0)
                        తిరిగి -errno;

                మొత్తం_లోపల += n;

                r = decompressor_నెట్టు(c, buf, n, decompress_ప్రవాహం_write_callback, &userdata);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (n == 0)
                        ఆపు;
        }

        ఒకవేళ (మొత్తం_లోపల == 0)
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EBADMSG), "%s decompression failed: no data read",
                                       compression_గమ్యం_string(type));

        ఒకవేళ (userdata.sparse) {
                r = finalize_sparse(fdt);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        log_debug("%s decompression finished (%" PRIu64 " -> %" PRIu64 " bytes, %.1f%%)",
                  compression_గమ్యం_string(type), మొత్తం_లోపల, userdata.మొత్తం_బహిర్గతం,
                  (ద్విగుణ) userdata.మొత్తం_బహిర్గతం / మొత్తం_లోపల * 100);

        తిరిగి 0;
}

పూర్ణాంకం decompress_ప్రవాహం_by_filename(స్థిరరాశి అక్షరం *filename, పూర్ణాంకం fdf, పూర్ణాంకం fdt, uint64_t max_bytes) {
        Compression c = compression_from_filename(filename);
        ఒకవేళ (c == COMPRESSION_NONE)
                తిరిగి -EPROTONOSUPPORT;

        తిరిగి decompress_ప్రవాహం(c, fdf, fdt, max_bytes);
}

/* Push-based streaming compression/decompression context API */

Compressor* compressor_free(Compressor *c) {
        ఒకవేళ (!c)
                తిరిగి ఖాళీ;

        ఎంపిక (c->type) {

#ఒకవేళ HAVE_XZ
        స్థితి COMPRESSION_XZ:
                sym_lzma_చివర(&c->xz);
                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
        స్థితి COMPRESSION_LZ4:
                ఒకవేళ (c->encoding) {
                        sym_LZ4F_freeCompressionContext(c->c_lz4);
                        c->c_lz4 = ఖాళీ;
                        c->lz4_శీర్షిక = mfree(c->lz4_శీర్షిక);
                } లేకపోతే {
                        sym_LZ4F_freeDecompressionContext(c->d_lz4);
                        c->d_lz4 = ఖాళీ;
                }
                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
        స్థితి COMPRESSION_ZSTD:
                ఒకవేళ (c->encoding) {
                        sym_ZSTD_freeCCtx(c->c_zstd);
                        c->c_zstd = ఖాళీ;
                } లేకపోతే {
                        sym_ZSTD_freeDCtx(c->d_zstd);
                        c->d_zstd = ఖాళీ;
                }
                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
        స్థితి COMPRESSION_GZIP:
                ఒకవేళ (c->encoding)
                        sym_deflate_చివర(&c->gzip);
                లేకపోతే
                        sym_inflate_చివర(&c->gzip);
                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_BZIP2
        స్థితి COMPRESSION_BZIP2:
                ఒకవేళ (c->encoding)
                        sym_BZ2_bz_Compress_చివర(&c->bzip2);
                లేకపోతే
                        sym_BZ2_bz_Decompress_చివర(&c->bzip2);
                ఆపు;
#అయితే_ముగింపు

        అప్రమేయ:
                ఆపు;
        }

        తిరిగి mfree(c);
}

Compression compressor_type(స్థిరరాశి Compressor *c) {
        తిరిగి c ? c->type : _COMPRESSION_INVALID;
}

పూర్ణాంకం decompressor_detect(Decompressor **decompressor, స్థిరరాశి శూన్య *దత్తాంశం, size_t కొలత) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(decompressor);

        ఒకవేళ (*decompressor)
                తిరిగి 1;

        ఒకవేళ (కొలత < COMPRESSION_MAGIC_బైట్లు_MAX)
                తిరిగి 0;

        ప్రతిజ్ఞా(దత్తాంశం);

        Compression type = compression_detect_from_magic(దత్తాంశం);
        ఒకవేళ (type == _COMPRESSION_INVALID)
                type = COMPRESSION_NONE;

        log_debug("Detected compression type: %s", compression_గమ్యం_string(type));

        r = dlopen_compress(type, LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = decompressor_new(decompressor, type, /* flags= */ 0);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 1;
}

పూర్ణాంకం decompressor_force(Decompressor **decompressor, Compression type) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(decompressor);
        ప్రతిజ్ఞా(type >= 0 && type < _COMPRESSION_MAX);

        /* Like decompressor_detect(), but uses the specified compression algorithm instead of guessing
         * from the magic bytes. Replaces any decompressor already set up. */

        r = dlopen_compress(type, LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *decompressor = compressor_free(*decompressor);
        తిరిగి decompressor_new(decompressor, type, /* flags= */ 0);
}

పూర్ణాంకం decompressor_force_off(Decompressor **decompressor) {
        తిరిగి decompressor_force(decompressor, COMPRESSION_NONE);
}

పూర్ణాంకం decompressor_నెట్టు(Decompressor *c, స్థిరరాశి శూన్య *దత్తాంశం, size_t కొలత, DecompressorCallback callback, శూన్య *userdata) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(c);
        ప్రతిజ్ఞా(దత్తాంశం || కొలత == 0);
        ప్రతిజ్ఞా(callback);

        /* This feeds `size` bytes of compressed data into `c`. Passing size == 0 has special meaning: it
         * signals that no further input will be provided, and the function will attempt to finalize the
         * stream, failing with -EBADMSG if the compressed data was truncated or has trailing garbage after
         * the logical end of stream. */

        ఒకవేళ (c->encoding)
                తిరిగి -EINVAL;

        ఒకవేళ (c->ముగిసిన && కొలత == 0)
                /* The previous stream was correctly finished, and it was the last one. */
                తిరిగి 0;

#ఒకవేళ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        _cleanup_free_ uint8_t *బఫరు = ఖాళీ;
        ఒకవేళ (c->type != COMPRESSION_NONE) {
                బఫరు = new(uint8_t, COMPRESS_PIPE_బఫరు_కొలత);
                ఒకవేళ (!బఫరు)
                        తిరిగి -ENOMEM;
        }
#అయితే_ముగింపు

        ఎంపిక (c->type) {

        స్థితి COMPRESSION_NONE:
                ఒకవేళ (c->ముగిసిన)
                        తిరిగి -EBADMSG;

                ఒకవేళ (కొలత == 0) {
                        c->ముగిసిన = true;
                        తిరిగి 0;
                }

                r = callback(దత్తాంశం, కొలత, userdata);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                తిరిగి 0;

#ఒకవేళ HAVE_XZ
        స్థితి COMPRESSION_XZ:
                ఒకవేళ (c->ముగిసిన)
                        తిరిగి -EBADMSG;

                c->xz.తదుపరి_లోపల = దత్తాంశం;
                c->xz.avail_లోపల = కొలత;

                కొరకు (;;) {
                        c->xz.తదుపరి_బహిర్గతం = బఫరు;
                        c->xz.avail_బహిర్గతం = COMPRESS_PIPE_బఫరు_కొలత;

                        lzma_ret lzr = sym_lzma_సంకేతం(&c->xz, కొలత == 0 ? LZMA_ముగింపు : LZMA_నిరంతరం);
                        ఒకవేళ (!లోపల_సమితి(lzr, LZMA_OK, LZMA_ప్రవాహం_చివర))
                                తిరిగి -EBADMSG;

                        ఒకవేళ (c->xz.avail_బహిర్గతం > COMPRESS_PIPE_బఫరు_కొలత)
                                తిరిగి -EIO;

                        size_t produced = COMPRESS_PIPE_బఫరు_కొలత - c->xz.avail_బహిర్గతం;
                        ఒకవేళ (produced > 0) {
                                r = callback(బఫరు, produced, userdata);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }

                        ఒకవేళ (lzr == LZMA_ప్రవాహం_చివర) {
                                ఒకవేళ (c->xz.avail_లోపల > 0)
                                        తిరిగి -EBADMSG; /* garbage data at the end */

                                c->ముగిసిన = true;
                                తిరిగి 0;
                        }

                        ఒకవేళ (c->xz.avail_లోపల == 0 && c->xz.avail_బహిర్గతం > 0) {
                                ఒకవేళ (కొలత == 0)
                                        తిరిగి -EBADMSG; /* truncated */

                                తిరిగి 0; /* No progress possible with the current input. */
                        }
                }
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
        స్థితి COMPRESSION_LZ4: {
                స్థిరరాశి uint8_t *src = దత్తాంశం;
                size_t src_మిగిలిన = కొలత;

                c->ముగిసిన = false;

                కొరకు (;;) {
                        size_t produced = COMPRESS_PIPE_బఫరు_కొలత;
                        size_t consumed = src_మిగిలిన;

                        size_t rc = sym_LZ4F_decompress(c->d_lz4, బఫరు, &produced, src, &consumed, ఖాళీ);
                        ఒకవేళ (sym_LZ4F_isError(rc))
                                తిరిగి -EBADMSG;

                        ఒకవేళ (consumed > src_మిగిలిన ||
                            produced > COMPRESS_PIPE_బఫరు_కొలత)
                                తిరిగి -EIO;

                        ఒకవేళ (consumed > 0) {
                                ప్రతిజ్ఞా(src);
                                src += consumed;
                                src_మిగిలిన -= consumed;
                        }

                        ఒకవేళ (produced > 0) {
                                r = callback(బఫరు, produced, userdata);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }

                        ఒకవేళ (src_మిగిలిన == 0) {
                                ఒకవేళ (rc == 0) {
                                        /* The current stream has finished. */
                                        c->ముగిసిన = true;
                                        తిరిగి 0;
                                }

                                ఒకవేళ (produced == COMPRESS_PIPE_బఫరు_కొలత)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        కొనసాగు;

                                ఒకవేళ (కొలత == 0)
                                        తిరిగి -EBADMSG; /* truncated */

                                తిరిగి 0; /* No progress possible with the current input. */
                        }
                }
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
        స్థితి COMPRESSION_ZSTD: {
                ZSTD_లోపల_బఫరు ఆదానం = {
                        .src =  (శూన్య*) దత్తాంశం,
                        .కొలత = కొలత,
                };

                c->ముగిసిన = false;

                కొరకు (;;) {
                        ZSTD_బహిర్గతం_బఫరు output = {
                                .dst = బఫరు,
                                .కొలత = COMPRESS_PIPE_బఫరు_కొలత,
                        };

                        size_t res = sym_ZSTD_decompress_ప్రవాహం(c->d_zstd, &output, &ఆదానం);
                        ఒకవేళ (sym_ZSTD_isError(res))
                                తిరిగి -EBADMSG;

                        ఒకవేళ (ఆదానం.pos > ఆదానం.కొలత ||
                            output.pos > output.కొలత)
                                తిరిగి -EIO;

                        ఒకవేళ (output.pos > 0) {
                                r = callback(output.dst, output.pos, userdata);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }

                        ఒకవేళ (ఆదానం.pos == ఆదానం.కొలత) {
                                ఒకవేళ (res == 0) {
                                        /* The current stream has finished. */
                                        c->ముగిసిన = true;
                                        తిరిగి 0;
                                }

                                ఒకవేళ (output.pos == COMPRESS_PIPE_బఫరు_కొలత)
                                        /* Remaining data exist in the library buffer. We need to read them. */
                                        కొనసాగు;

                                ఒకవేళ (కొలత == 0)
                                        తిరిగి -EBADMSG; /* truncated */

                                తిరిగి 0; /* No progress possible with the current input. */
                        }
                }
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
        స్థితి COMPRESSION_GZIP:
                ఒకవేళ (కొలత > UINT_MAX)
                        తిరిగి -EFBIG;

                c->gzip.తదుపరి_లోపల = (శూన్య*) దత్తాంశం;
                c->gzip.avail_లోపల = కొలత;

                కొరకు (;;) {
                        ఒకవేళ (c->ముగిసిన) {
                                ఒకవేళ (sym_inflateReset(&c->gzip) != Z_OK)
                                        తిరిగి -EIO;

                                c->ముగిసిన = false;
                        }

                        c->gzip.తదుపరి_బహిర్గతం = బఫరు;
                        c->gzip.avail_బహిర్గతం = COMPRESS_PIPE_బఫరు_కొలత;

                        /* Z_BUF_ERROR is not fatal if all input is used up: it just means no progress was
                         * possible, which happens if the previous iteration filled the output buffer exactly
                         * while also consuming all input. That case is handled by the avail_in check
                         * below. If there's still input left however, Z_BUF_ERROR means we are stuck, hence
                         * refuse it, so that we don't loop forever. */
                        పూర్ణాంకం zr = sym_inflate(&c->gzip, Z_NO_FLUSH);
                        ఒకవేళ (!లోపల_సమితి(zr, Z_OK, Z_ప్రవాహం_చివర) &&
                            !(zr == Z_BUF_ERROR && c->gzip.avail_లోపల == 0))
                                తిరిగి -EBADMSG;

                        ఒకవేళ (c->gzip.avail_బహిర్గతం > COMPRESS_PIPE_బఫరు_కొలత)
                                తిరిగి -EIO;

                        size_t produced = COMPRESS_PIPE_బఫరు_కొలత - c->gzip.avail_బహిర్గతం;
                        ఒకవేళ (produced > 0) {
                                r = callback(బఫరు, produced, userdata);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }

                        ఒకవేళ (zr == Z_ప్రవాహం_చివర) {
                                c->ముగిసిన = true;

                                ఒకవేళ (c->gzip.avail_లోపల > 0)
                                        కొనసాగు; /* New stream may be pending. */

                                తిరిగి 0;
                        }

                        ఒకవేళ (c->gzip.avail_లోపల == 0 && c->gzip.avail_బహిర్గతం > 0) {
                                ఒకవేళ (కొలత == 0)
                                        తిరిగి -EBADMSG; /* truncated */

                                తిరిగి 0; /* No progress possible with the current input. */
                        }
                }
#అయితే_ముగింపు

#ఒకవేళ HAVE_BZIP2
        స్థితి COMPRESSION_BZIP2:
                ఒకవేళ (కొలత > UINT_MAX)
                        తిరిగి -EFBIG;

                c->bzip2.తదుపరి_లోపల = (అక్షరం*) దత్తాంశం;
                c->bzip2.avail_లోపల = కొలత;

                కొరకు (;;) {
                        ఒకవేళ (c->ముగిసిన) {
                                sym_BZ2_bz_Decompress_చివర(&c->bzip2);
                                ఒకవేళ (sym_BZ2_bzDecompressInit(&c->bzip2, /* verbosity= */ 0, /* small= */ 0) != BZ_OK)
                                        తిరిగి -EIO;

                                c->ముగిసిన = false;
                        }

                        c->bzip2.తదుపరి_బహిర్గతం = (అక్షరం*) బఫరు;
                        c->bzip2.avail_బహిర్గతం = COMPRESS_PIPE_బఫరు_కొలత;

                        పూర్ణాంకం bzr = sym_BZ2_bzDecompress(&c->bzip2);
                        ఒకవేళ (!లోపల_సమితి(bzr, BZ_OK, BZ_ప్రవాహం_చివర))
                                తిరిగి -EBADMSG;

                        ఒకవేళ (c->bzip2.avail_బహిర్గతం > COMPRESS_PIPE_బఫరు_కొలత)
                                తిరిగి -EIO;

                        size_t produced = COMPRESS_PIPE_బఫరు_కొలత - c->bzip2.avail_బహిర్గతం;
                        ఒకవేళ (produced > 0) {
                                r = callback(బఫరు, produced, userdata);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }

                        ఒకవేళ (bzr == BZ_ప్రవాహం_చివర) {
                                c->ముగిసిన = true;

                                ఒకవేళ (c->bzip2.avail_లోపల > 0)
                                        కొనసాగు; /* New stream may be pending. */

                                తిరిగి 0;
                        }

                        ఒకవేళ (c->bzip2.avail_లోపల == 0 && c->bzip2.avail_బహిర్గతం > 0) {
                                ఒకవేళ (కొలత == 0)
                                        తిరిగి -EBADMSG; /* truncated */

                                తిరిగి 0; /* No progress possible with the current input. */
                        }
                }
#అయితే_ముగింపు

        అప్రమేయ:
                assert_not_reached();
        }
}

పూర్ణాంకం compressor_new(Compressor **ret, Compression type) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = dlopen_compress(type, LOG_DEBUG);
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_(compressor_freep) Compressor *c = new0(Compressor, 1);
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        c->type = _COMPRESSION_INVALID;
        /* Set encoding early so that compressor_freep calls the correct cleanup (compression vs
         * decompression) if any operation in the switch fails after setting c->type. This is safe
         * because _COMPRESSION_INVALID hits the default: break case regardless of the encoding flag. */
        c->encoding = true;

        ఎంపిక (type) {

#ఒకవేళ HAVE_XZ
        స్థితి COMPRESSION_XZ: {
                lzma_ret xzr = sym_lzma_easy_encoder(&c->xz, LZMA_PRESET_DEFAULT, LZMA_CHECK_CRC64);
                ఒకవేళ (xzr != LZMA_OK)
                        తిరిగి -EIO;

                c->type = COMPRESSION_XZ;
                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
        స్థితి COMPRESSION_LZ4: {
                size_t rc = sym_LZ4F_createCompressionContext(&c->c_lz4, LZ4_F_సంస్కరణ);
                ఒకవేళ (sym_LZ4F_isError(rc))
                        తిరిగి -ENOMEM;

                c->type = COMPRESSION_LZ4;

                /* Generate the frame header and stash it for the first compressor_start call */
                size_t శీర్షిక_bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                c->lz4_శీర్షిక = జ్ఞాపకం_ఇవ్వు(శీర్షిక_bound);
                ఒకవేళ (!c->lz4_శీర్షిక)
                        తిరిగి -ENOMEM;

                c->lz4_శీర్షిక_కొలత = sym_LZ4F_compressBegin(c->c_lz4, c->lz4_శీర్షిక, శీర్షిక_bound, &lz4_preferences);
                ఒకవేళ (sym_LZ4F_isError(c->lz4_శీర్షిక_కొలత))
                        తిరిగి -EINVAL;

                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
        స్థితి COMPRESSION_ZSTD:
                c->c_zstd = sym_ZSTD_createCCtx();
                ఒకవేళ (!c->c_zstd)
                        తిరిగి -ENOMEM;

                c->type = COMPRESSION_ZSTD;

                size_t z = sym_ZSTD_CCtx_సమితి_Parameter(c->c_zstd, ZSTD_c_compressionLevel, ZSTD_CLEVEL_DEFAULT);
                ఒకవేళ (sym_ZSTD_isError(z))
                        తిరిగి -EIO;

                z = sym_ZSTD_CCtx_సమితి_Parameter(c->c_zstd, ZSTD_c_checksumFlag, /* enable= */ 1);
                ఒకవేళ (sym_ZSTD_isError(z))
                        log_debug("Failed to enable ZSTD checksum, ignoring: %s", sym_ZSTD_getErrorName(z));

                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
        స్థితి COMPRESSION_GZIP:
                r = sym_deflateInit2_(&c->gzip,
                                      Z_DEFAULT_COMPRESSION,
                                      /* method= */ Z_DEFLATED,
                                      /* windowBits= */ ZLIB_WBITS_GZIP,
                                      /* memLevel= */ 8,
                                      /* strategy= */ Z_DEFAULT_STRATEGY,
                                      ZLIB_సంస్కరణ, (పూర్ణాంకం) పరిమాణం(c->gzip));
                ఒకవేళ (r != Z_OK)
                        తిరిగి -EIO;

                c->type = COMPRESSION_GZIP;
                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_BZIP2
        స్థితి COMPRESSION_BZIP2:
                r = sym_BZ2_bzCompressInit(&c->bzip2, /* blockSize100k= */ 9, /* verbosity= */ 0, /* workFactor= */ 0);
                ఒకవేళ (r != BZ_OK)
                        తిరిగి -EIO;

                c->type = COMPRESSION_BZIP2;
                ఆపు;
#అయితే_ముగింపు

        స్థితి COMPRESSION_NONE:
                c->type = COMPRESSION_NONE;
                ఆపు;

        అప్రమేయ:
                తిరిగి -EOPNOTSUPP;
        }

        *ret = TAKE_PTR(c);
        తిరిగి 0;
}

#ఒకవేళ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
స్థిర పూర్ణాంకం enlarge_బఫరు(శూన్య **బఫరు, size_t *బఫరు_కొలత, size_t *బఫరు_allocated, size_t need) {
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(బఫరు_కొలత);
        ప్రతిజ్ఞా(బఫరు_allocated);

        need = MAX3(need, *బఫరు_కొలత + 1, (size_t) COMPRESS_PIPE_బఫరు_కొలత);
        ఒకవేళ (*బఫరు_allocated >= need)
                తిరిగి 0;

        ఒకవేళ (!greedy_realloc(బఫరు, need, 1))
                తిరిగి -ENOMEM;

        *బఫరు_allocated = MALLOC_SIZEOF_సురక్షిత(*బఫరు);
        తిరిగి 1;
}
#అయితే_ముగింపు

పూర్ణాంకం compressor_ప్రారంభం(
                Compressor *c,
                స్థిరరాశి శూన్య *దత్తాంశం,
                size_t కొలత,
                శూన్య **బఫరు,
                size_t *బఫరు_కొలత,
                size_t *బఫరు_allocated) {

#ఒకవేళ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        పూర్ణాంకం r;
#అయితే_ముగింపు

        ప్రతిజ్ఞా(c);
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(బఫరు_కొలత);
        ప్రతిజ్ఞా(బఫరు_allocated);

        ఒకవేళ (!c->encoding)
                తిరిగి -EINVAL;

        ఒకవేళ (కొలత == 0)
                తిరిగి 0;

        ప్రతిజ్ఞా(దత్తాంశం);

        *బఫరు_కొలత = 0;

        ఎంపిక (c->type) {

#ఒకవేళ HAVE_XZ
        స్థితి COMPRESSION_XZ:

                c->xz.తదుపరి_లోపల = దత్తాంశం;
                c->xz.avail_లోపల = కొలత;

                అయితే (c->xz.avail_లోపల > 0) {
                        lzma_ret lzr;

                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, /* need= */ 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        c->xz.తదుపరి_బహిర్గతం = (uint8_t*) *బఫరు + *బఫరు_కొలత;
                        c->xz.avail_బహిర్గతం = *బఫరు_allocated - *బఫరు_కొలత;

                        lzr = sym_lzma_సంకేతం(&c->xz, LZMA_నిరంతరం);
                        ఒకవేళ (lzr != LZMA_OK)
                                తిరిగి -EIO;

                        *బఫరు_కొలత += (*బఫరు_allocated - *బఫరు_కొలత) - c->xz.avail_బహిర్గతం;
                }

                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
        స్థితి COMPRESSION_LZ4: {
                /* Prepend any stashed frame header from compressor_new */
                ఒకవేళ (c->lz4_శీర్షిక_కొలత > 0) {
                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, c->lz4_శీర్షిక_కొలత);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        జ్ఞాపకం_నకలు(*బఫరు, c->lz4_శీర్షిక, c->lz4_శీర్షిక_కొలత);
                        *బఫరు_కొలత = c->lz4_శీర్షిక_కొలత;
                        c->lz4_శీర్షిక = mfree(c->lz4_శీర్షిక);
                        c->lz4_శీర్షిక_కొలత = 0;
                }

                size_t bound = sym_LZ4F_compressBound(కొలత, &lz4_preferences);
                r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, *బఫరు_కొలత + bound);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                size_t n = sym_LZ4F_compressUpdate(c->c_lz4,
                                                   (uint8_t*) *బఫరు + *బఫరు_కొలత,
                                                   *బఫరు_allocated - *బఫరు_కొలత,
                                                   దత్తాంశం, కొలత, ఖాళీ);
                ఒకవేళ (sym_LZ4F_isError(n))
                        తిరిగి -EIO;

                *బఫరు_కొలత += n;
                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
        స్థితి COMPRESSION_ZSTD: {
                ZSTD_లోపల_బఫరు ఆదానం = {
                        .src = దత్తాంశం,
                        .కొలత = కొలత,
                };

                అయితే (ఆదానం.pos < ఆదానం.కొలత) {
                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, /* need= */ 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        ZSTD_బహిర్గతం_బఫరు output = {
                                .dst = ((uint8_t *) *బఫరు + *బఫరు_కొలత),
                                .కొలత = *బఫరు_allocated - *బఫరు_కొలత,
                        };

                        size_t res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ఆదానం, ZSTD_e_continue);
                        ఒకవేళ (sym_ZSTD_isError(res))
                                తిరిగి -EIO;

                        *బఫరు_కొలత += output.pos;
                }

                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
        స్థితి COMPRESSION_GZIP:
                ఒకవేళ (కొలత > UINT_MAX)
                        తిరిగి -EFBIG;

                c->gzip.తదుపరి_లోపల = (శూన్య*) దత్తాంశం;
                c->gzip.avail_లోపల = కొలత;

                అయితే (c->gzip.avail_లోపల > 0) {
                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, /* need= */ 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        size_t avail = MIN(*బఫరు_allocated - *బఫరు_కొలత, (size_t) UINT_MAX);
                        c->gzip.తదుపరి_బహిర్గతం = (uint8_t*) *బఫరు + *బఫరు_కొలత;
                        c->gzip.avail_బహిర్గతం = avail;

                        r = sym_deflate(&c->gzip, Z_NO_FLUSH);
                        ఒకవేళ (r != Z_OK)
                                తిరిగి -EIO;

                        *బఫరు_కొలత += avail - c->gzip.avail_బహిర్గతం;
                }

                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_BZIP2
        స్థితి COMPRESSION_BZIP2:
                ఒకవేళ (కొలత > UINT_MAX)
                        తిరిగి -EFBIG;

                c->bzip2.తదుపరి_లోపల = (శూన్య*) దత్తాంశం;
                c->bzip2.avail_లోపల = కొలత;

                అయితే (c->bzip2.avail_లోపల > 0) {
                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, /* need= */ 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        size_t avail = MIN(*బఫరు_allocated - *బఫరు_కొలత, (size_t) UINT_MAX);
                        c->bzip2.తదుపరి_బహిర్గతం = (శూన్య*) ((uint8_t*) *బఫరు + *బఫరు_కొలత);
                        c->bzip2.avail_బహిర్గతం = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_నిరంతరం);
                        ఒకవేళ (r != BZ_నిరంతరం_OK)
                                తిరిగి -EIO;

                        *బఫరు_కొలత += avail - c->bzip2.avail_బహిర్గతం;
                }

                ఆపు;
#అయితే_ముగింపు

        స్థితి COMPRESSION_NONE:

                ఒకవేళ (*బఫరు_allocated < కొలత) {
                        శూన్య *p;

                        p = realloc(*బఫరు, కొలత);
                        ఒకవేళ (!p)
                                తిరిగి -ENOMEM;

                        *బఫరు = p;
                        *బఫరు_allocated = కొలత;
                }

                జ్ఞాపకం_నకలు(*బఫరు, దత్తాంశం, కొలత);
                *బఫరు_కొలత = కొలత;
                ఆపు;

        అప్రమేయ:
                తిరిగి -EOPNOTSUPP;
        }

        తిరిగి 0;
}

పూర్ణాంకం compressor_ముగింపు(Compressor *c, శూన్య **బఫరు, size_t *బఫరు_కొలత, size_t *బఫరు_allocated) {
#ఒకవేళ HAVE_XZ || HAVE_LZ4 || HAVE_ZSTD || HAVE_ZLIB || HAVE_BZIP2
        పూర్ణాంకం r;
#అయితే_ముగింపు

        ప్రతిజ్ఞా(c);
        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(బఫరు_కొలత);
        ప్రతిజ్ఞా(బఫరు_allocated);

        ఒకవేళ (!c->encoding)
                తిరిగి -EINVAL;

        *బఫరు_కొలత = 0;

        ఎంపిక (c->type) {

#ఒకవేళ HAVE_XZ
        స్థితి COMPRESSION_XZ: {
                lzma_ret lzr;

                c->xz.avail_లోపల = 0;

                చేయి {
                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, /* need= */ 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        c->xz.తదుపరి_బహిర్గతం = (uint8_t*) *బఫరు + *బఫరు_కొలత;
                        c->xz.avail_బహిర్గతం = *బఫరు_allocated - *బఫరు_కొలత;

                        lzr = sym_lzma_సంకేతం(&c->xz, LZMA_ముగింపు);
                        ఒకవేళ (!లోపల_సమితి(lzr, LZMA_OK, LZMA_ప్రవాహం_చివర))
                                తిరిగి -EIO;

                        *బఫరు_కొలత += (*బఫరు_allocated - *బఫరు_కొలత) - c->xz.avail_బహిర్గతం;
                } అయితే (lzr != LZMA_ప్రవాహం_చివర);

                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_LZ4
        స్థితి COMPRESSION_LZ4: {
                size_t bound = sym_LZ4F_compressBound(0, &lz4_preferences);
                r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, bound);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                size_t n = sym_LZ4_F_compress_చివర(c->c_lz4, *బఫరు, *బఫరు_allocated, ఖాళీ);
                ఒకవేళ (sym_LZ4F_isError(n))
                        తిరిగి -EIO;

                *బఫరు_కొలత = n;
                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZSTD
        స్థితి COMPRESSION_ZSTD: {
                ZSTD_లోపల_బఫరు ఆదానం = {};
                size_t res;

                చేయి {
                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, /* need= */ 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        ZSTD_బహిర్గతం_బఫరు output = {
                                .dst = ((uint8_t *) *బఫరు + *బఫరు_కొలత),
                                .కొలత = *బఫరు_allocated - *బఫరు_కొలత,
                        };

                        res = sym_ZSTD_compressStream2(c->c_zstd, &output, &ఆదానం, ZSTD_e_చివర);
                        ఒకవేళ (sym_ZSTD_isError(res))
                                తిరిగి -EIO;

                        *బఫరు_కొలత += output.pos;
                } అయితే (res != 0);

                ఆపు;
        }
#అయితే_ముగింపు

#ఒకవేళ HAVE_ZLIB
        స్థితి COMPRESSION_GZIP:
                c->gzip.avail_లోపల = 0;

                చేయి {
                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, /* need= */ 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        size_t avail = MIN(*బఫరు_allocated - *బఫరు_కొలత, (size_t) UINT_MAX);
                        c->gzip.తదుపరి_బహిర్గతం = (uint8_t*) *బఫరు + *బఫరు_కొలత;
                        c->gzip.avail_బహిర్గతం = avail;

                        r = sym_deflate(&c->gzip, Z_ముగింపు);
                        ఒకవేళ (!లోపల_సమితి(r, Z_OK, Z_ప్రవాహం_చివర))
                                తిరిగి -EIO;

                        *బఫరు_కొలత += avail - c->gzip.avail_బహిర్గతం;
                } అయితే (r != Z_ప్రవాహం_చివర);

                ఆపు;
#అయితే_ముగింపు

#ఒకవేళ HAVE_BZIP2
        స్థితి COMPRESSION_BZIP2:
                c->bzip2.avail_లోపల = 0;

                చేయి {
                        r = enlarge_బఫరు(బఫరు, బఫరు_కొలత, బఫరు_allocated, /* need= */ 0);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        size_t avail = MIN(*బఫరు_allocated - *బఫరు_కొలత, (size_t) UINT_MAX);
                        c->bzip2.తదుపరి_బహిర్గతం = (శూన్య*) ((uint8_t*) *బఫరు + *బఫరు_కొలత);
                        c->bzip2.avail_బహిర్గతం = avail;

                        r = sym_BZ2_bzCompress(&c->bzip2, BZ_ముగింపు);
                        ఒకవేళ (!లోపల_సమితి(r, BZ_ముగింపు_OK, BZ_ప్రవాహం_చివర))
                                తిరిగి -EIO;

                        *బఫరు_కొలత += avail - c->bzip2.avail_బహిర్గతం;
                } అయితే (r != BZ_ప్రవాహం_చివర);

                ఆపు;
#అయితే_ముగింపు

        స్థితి COMPRESSION_NONE:
                ఆపు;

        అప్రమేయ:
                తిరిగి -EOPNOTSUPP;
        }

        తిరిగి 0;
}
