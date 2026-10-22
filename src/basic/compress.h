/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "dlopen-note.h"

రకనిర్వచనం గణన Compression {
        COMPRESSION_NONE,
        COMPRESSION_XZ,
        COMPRESSION_LZ4,
        COMPRESSION_ZSTD,
        COMPRESSION_GZIP,
        COMPRESSION_BZIP2,
        _COMPRESSION_MAX,
        _COMPRESSION_INVALID = -EINVAL,
} Compression;

DECLARE_STRING_పట్టిక_LOOKUP(compression, Compression);
DECLARE_STRING_పట్టిక_LOOKUP(compression_uppercase, Compression);
DECLARE_STRING_పట్టిక_LOOKUP(compression_extension, Compression);

/* Try the lowercase string table first, fall back to the uppercase one. Useful for parsing user input
 * where both forms (e.g. "xz" and "XZ") have historically been accepted. */
Compression compression_from_string_harder(స్థిరరాశి అక్షరం *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(స్థిరరాశి అక్షరం *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#నిర్వచించు COMPRESS_PIPE_బఫరు_కొలత (128U*1024U)

#నిర్వచించు COMPRESSION_MAGIC_బైట్లు_MAX 6U
Compression compression_detect_from_magic(స్థిరరాశి uint8_t దత్తాంశం[స్థిర COMPRESSION_MAGIC_బైట్లు_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

రకనిర్వచనం నిర్మాణం Compressor Compressor;
రకనిర్వచనం Compressor Decompressor;

రకనిర్వచనం పూర్ణాంకం (*DecompressorCallback)(స్థిరరాశి శూన్య *దత్తాంశం, size_t కొలత, శూన్య *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

పూర్ణాంకం compressor_new(Compressor **ret, Compression type);
పూర్ణాంకం compressor_ప్రారంభం(Compressor *c, స్థిరరాశి శూన్య *దత్తాంశం, size_t కొలత, శూన్య **బఫరు, size_t *బఫరు_కొలత, size_t *బఫరు_allocated);
పూర్ణాంకం compressor_ముగింపు(Compressor *c, శూన్య **బఫరు, size_t *బఫరు_కొలత, size_t *బఫరు_allocated);

పూర్ణాంకం decompressor_detect(Decompressor **decompressor, స్థిరరాశి శూన్య *దత్తాంశం, size_t కొలత);
పూర్ణాంకం decompressor_force(Decompressor **decompressor, Compression type);
పూర్ణాంకం decompressor_force_off(Decompressor **decompressor);
పూర్ణాంకం decompressor_నెట్టు(Decompressor *c, స్థిరరాశి శూన్య *దత్తాంశం, size_t కొలత, DecompressorCallback callback, శూన్య *userdata);

Compression compressor_type(స్థిరరాశి Compressor *c);

/* Blob compression/decompression */

పూర్ణాంకం compress_blob_journal(
                Compression compression,
                స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                శూన్య *dst, size_t dst_alloc_కొలత, size_t *dst_కొలత, పూర్ణాంకం level);
పూర్ణాంకం compress_blob(Compression compression,
                  స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                  శూన్య *dst, size_t dst_alloc_కొలత, size_t *dst_కొలత, పూర్ణాంకం level);
పూర్ణాంకం decompress_blob_journal(
                Compression compression,
                స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                శూన్య **dst, size_t *dst_కొలత, size_t dst_max);
పూర్ణాంకం decompress_blob(Compression compression,
                    స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                    శూన్య **dst, size_t *dst_కొలత, size_t dst_max);

పూర్ణాంకం decompress_blob_గమ్యం_fd_journal(
                Compression type,
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                పూర్ణాంకం fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_కొలత);
పూర్ణాంకం decompress_blob_గమ్యం_fd(
                Compression type,
                స్థిరరాశి శూన్య *src,
                uint64_t src_కొలత,
                పూర్ణాంకం fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_కొలత);

పూర్ణాంకం decompress_zlib_raw(స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                        శూన్య *dst, size_t dst_కొలత, పూర్ణాంకం wbits);

పూర్ణాంకం decompress_startswith_journal(
                Compression compression,
                స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                శూన్య **బఫరు,
                స్థిరరాశి శూన్య *ఉపసర్గ, size_t ఉపసర్గ_len,
                uint8_t extra);
పూర్ణాంకం decompress_startswith(Compression compression,
                          స్థిరరాశి శూన్య *src, uint64_t src_కొలత,
                          శూన్య **బఫరు,
                          స్థిరరాశి శూన్య *ఉపసర్గ, size_t ఉపసర్గ_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

పూర్ణాంకం compress_ప్రవాహం(Compression type, పూర్ణాంకం fdf, పూర్ణాంకం fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_కొలత);
రకనిర్వచనం గణన DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

పూర్ణాంకం decompress_ప్రవాహం_నిండిన(Compression type, పూర్ణాంకం fdf, పూర్ణాంకం fdt, uint64_t max_bytes, DecompressFlags flags);
స్థిర అంతఃవరుస పూర్ణాంకం decompress_ప్రవాహం(Compression type, పూర్ణాంకం fdf, పూర్ణాంకం fdt, uint64_t max_bytes) {
        తిరిగి decompress_ప్రవాహం_నిండిన(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
పూర్ణాంకం decompress_ప్రవాహం_by_filename(స్థిరరాశి అక్షరం *filename, పూర్ణాంకం fdf, పూర్ణాంకం fdt, uint64_t max_bytes);

పూర్ణాంకం dlopen_xz(పూర్ణాంకం log_level) _dlopen_loader_;
పూర్ణాంకం dlopen_lz4(పూర్ణాంకం log_level) _dlopen_loader_;
పూర్ణాంకం dlopen_zstd(పూర్ణాంకం log_level) _dlopen_loader_;
పూర్ణాంకం dlopen_zlib(పూర్ణాంకం log_level) _dlopen_loader_;
పూర్ణాంకం dlopen_bzip2(పూర్ణాంకం log_level) _dlopen_loader_;

పూర్ణాంకం dlopen_compress(Compression c, పూర్ణాంకం log_level);
పూర్ణాంకం dlopen_compress_journal(Compression c, పూర్ణాంకం log_level);
స్థిర అంతఃవరుస పూర్ణాంకం dlopen_compress_default(పూర్ణాంకం log_level) {
        తిరిగి dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* default_compression_extension(శూన్య) {
        తిరిగి compression_extension_గమ్యం_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_బహిర్గతం_కొలత(శూన్య);
