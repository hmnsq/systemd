/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "dlopen-note.h"

প্রকারলেখ গণনা Compression {
        COMPRESSION_NONE,
        COMPRESSION_XZ,
        COMPRESSION_LZ4,
        COMPRESSION_ZSTD,
        COMPRESSION_GZIP,
        COMPRESSION_BZIP2,
        _COMPRESSION_MAX,
        _COMPRESSION_INVALID = -EINVAL,
} Compression;

DECLARE_STRING_TABLE_LOOKUP(compression, Compression);
DECLARE_STRING_TABLE_LOOKUP(compression_uppercase, Compression);
DECLARE_STRING_TABLE_LOOKUP(compression_extension, Compression);

/* Try the lowercase string table first, fall back to the uppercase one. Useful for parsing user input
 * where both forms (e.g. "xz" and "XZ") have historically been accepted. */
Compression compression_from_string_harder(ধ্রুবক অক্ষর *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ধ্রুবক অক্ষর *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_বাফার_আকার (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(ধ্রুবক uint8_t তথ্য[স্থির COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

প্রকারলেখ কাঠামো Compressor Compressor;
প্রকারলেখ Compressor Decompressor;

প্রকারলেখ পূর্ণসংখ্যা (*DecompressorCallback)(ধ্রুবক শূন্য *তথ্য, size_t আকার_2, শূন্য *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

পূর্ণসংখ্যা compressor_new(Compressor **ret, Compression type);
পূর্ণসংখ্যা compressor_start(Compressor *c, ধ্রুবক শূন্য *তথ্য, size_t আকার_2, শূন্য **বাফার, size_t *বাফার_আকার, size_t *বাফার_allocated);
পূর্ণসংখ্যা compressor_finish(Compressor *c, শূন্য **বাফার, size_t *বাফার_আকার, size_t *বাফার_allocated);

পূর্ণসংখ্যা decompressor_detect(Decompressor **decompressor, ধ্রুবক শূন্য *তথ্য, size_t আকার_2);
পূর্ণসংখ্যা decompressor_force(Decompressor **decompressor, Compression type);
পূর্ণসংখ্যা decompressor_force_off(Decompressor **decompressor);
পূর্ণসংখ্যা decompressor_ঠেলো(Decompressor *c, ধ্রুবক শূন্য *তথ্য, size_t আকার_2, DecompressorCallback callback, শূন্য *userdata);

Compression compressor_type(ধ্রুবক Compressor *c);

/* Blob compression/decompression */

পূর্ণসংখ্যা compress_blob_journal(
                Compression compression,
                ধ্রুবক শূন্য *src, uint64_t src_আকার,
                শূন্য *dst, size_t dst_alloc_আকার, size_t *dst_আকার, পূর্ণসংখ্যা level);
পূর্ণসংখ্যা compress_blob(Compression compression,
                  ধ্রুবক শূন্য *src, uint64_t src_আকার,
                  শূন্য *dst, size_t dst_alloc_আকার, size_t *dst_আকার, পূর্ণসংখ্যা level);
পূর্ণসংখ্যা decompress_blob_journal(
                Compression compression,
                ধ্রুবক শূন্য *src, uint64_t src_আকার,
                শূন্য **dst, size_t *dst_আকার, size_t dst_max);
পূর্ণসংখ্যা decompress_blob(Compression compression,
                    ধ্রুবক শূন্য *src, uint64_t src_আকার,
                    শূন্য **dst, size_t *dst_আকার, size_t dst_max);

পূর্ণসংখ্যা decompress_blob_to_fd_journal(
                Compression type,
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                পূর্ণসংখ্যা fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_আকার);
পূর্ণসংখ্যা decompress_blob_to_fd(
                Compression type,
                ধ্রুবক শূন্য *src,
                uint64_t src_আকার,
                পূর্ণসংখ্যা fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_আকার);

পূর্ণসংখ্যা decompress_zlib_raw(ধ্রুবক শূন্য *src, uint64_t src_আকার,
                        শূন্য *dst, size_t dst_আকার, পূর্ণসংখ্যা wbits);

পূর্ণসংখ্যা decompress_startswith_journal(
                Compression compression,
                ধ্রুবক শূন্য *src, uint64_t src_আকার,
                শূন্য **বাফার,
                ধ্রুবক শূন্য *prefix, size_t prefix_len,
                uint8_t extra);
পূর্ণসংখ্যা decompress_startswith(Compression compression,
                          ধ্রুবক শূন্য *src, uint64_t src_আকার,
                          শূন্য **বাফার,
                          ধ্রুবক শূন্য *prefix, size_t prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

পূর্ণসংখ্যা compress_stream(Compression type, পূর্ণসংখ্যা fdf, পূর্ণসংখ্যা fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_আকার);
প্রকারলেখ গণনা DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

পূর্ণসংখ্যা decompress_stream_full(Compression type, পূর্ণসংখ্যা fdf, পূর্ণসংখ্যা fdt, uint64_t max_bytes, DecompressFlags flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা decompress_stream(Compression type, পূর্ণসংখ্যা fdf, পূর্ণসংখ্যা fdt, uint64_t max_bytes) {
        ফেরত decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
পূর্ণসংখ্যা decompress_stream_by_filename(ধ্রুবক অক্ষর *filename, পূর্ণসংখ্যা fdf, পূর্ণসংখ্যা fdt, uint64_t max_bytes);

পূর্ণসংখ্যা dlopen_xz(পূর্ণসংখ্যা log_level) _dlopen_loader_;
পূর্ণসংখ্যা dlopen_lz4(পূর্ণসংখ্যা log_level) _dlopen_loader_;
পূর্ণসংখ্যা dlopen_zstd(পূর্ণসংখ্যা log_level) _dlopen_loader_;
পূর্ণসংখ্যা dlopen_zlib(পূর্ণসংখ্যা log_level) _dlopen_loader_;
পূর্ণসংখ্যা dlopen_bzip2(পূর্ণসংখ্যা log_level) _dlopen_loader_;

পূর্ণসংখ্যা dlopen_compress(Compression c, পূর্ণসংখ্যা log_level);
পূর্ণসংখ্যা dlopen_compress_journal(Compression c, পূর্ণসংখ্যা log_level);
স্থির অন্তঃরেখ পূর্ণসংখ্যা dlopen_compress_default(পূর্ণসংখ্যা log_level) {
        ফেরত dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* default_compression_extension(শূন্য) {
        ফেরত compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_out_আকার(শূন্য);
