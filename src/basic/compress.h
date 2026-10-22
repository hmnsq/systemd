/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "dlopen-note.h"

প্রকারনির্দেশ গণনা Compression {
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
Compression compression_from_string_harder(ধ্রুব বর্ণ *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ধ্রুব বর্ণ *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_BUFFER_SIZE (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(ধ্রুব uint8_t data[স্থির COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

প্রকারনির্দেশ সংরচনা Compressor Compressor;
প্রকারনির্দেশ Compressor Decompressor;

প্রকারনির্দেশ পূর্ণাঙ্ক (*DecompressorCallback)(ধ্রুব শূন্য *data, size_t size, শূন্য *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

পূর্ণাঙ্ক compressor_new(Compressor **ret, Compression type);
পূর্ণাঙ্ক compressor_start(Compressor *c, ধ্রুব শূন্য *data, size_t size, শূন্য **buffer, size_t *buffer_size, size_t *buffer_allocated);
পূর্ণাঙ্ক compressor_finish(Compressor *c, শূন্য **buffer, size_t *buffer_size, size_t *buffer_allocated);

পূর্ণাঙ্ক decompressor_detect(Decompressor **decompressor, ধ্রুব শূন্য *data, size_t size);
পূর্ণাঙ্ক decompressor_force(Decompressor **decompressor, Compression type);
পূর্ণাঙ্ক decompressor_force_off(Decompressor **decompressor);
পূর্ণাঙ্ক decompressor_ঠেলো(Decompressor *c, ধ্রুব শূন্য *data, size_t size, DecompressorCallback callback, শূন্য *userdata);

Compression compressor_type(ধ্রুব Compressor *c);

/* Blob compression/decompression */

পূর্ণাঙ্ক compress_blob_journal(
                Compression compression,
                ধ্রুব শূন্য *src, uint64_t src_size,
                শূন্য *dst, size_t dst_alloc_size, size_t *dst_size, পূর্ণাঙ্ক level);
পূর্ণাঙ্ক compress_blob(Compression compression,
                  ধ্রুব শূন্য *src, uint64_t src_size,
                  শূন্য *dst, size_t dst_alloc_size, size_t *dst_size, পূর্ণাঙ্ক level);
পূর্ণাঙ্ক decompress_blob_journal(
                Compression compression,
                ধ্রুব শূন্য *src, uint64_t src_size,
                শূন্য **dst, size_t *dst_size, size_t dst_max);
পূর্ণাঙ্ক decompress_blob(Compression compression,
                    ধ্রুব শূন্য *src, uint64_t src_size,
                    শূন্য **dst, size_t *dst_size, size_t dst_max);

পূর্ণাঙ্ক decompress_blob_to_fd_journal(
                Compression type,
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                পূর্ণাঙ্ক fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);
পূর্ণাঙ্ক decompress_blob_to_fd(
                Compression type,
                ধ্রুব শূন্য *src,
                uint64_t src_size,
                পূর্ণাঙ্ক fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);

পূর্ণাঙ্ক decompress_zlib_raw(ধ্রুব শূন্য *src, uint64_t src_size,
                        শূন্য *dst, size_t dst_size, পূর্ণাঙ্ক wbits);

পূর্ণাঙ্ক decompress_startswith_journal(
                Compression compression,
                ধ্রুব শূন্য *src, uint64_t src_size,
                শূন্য **buffer,
                ধ্রুব শূন্য *prefix, size_t prefix_len,
                uint8_t extra);
পূর্ণাঙ্ক decompress_startswith(Compression compression,
                          ধ্রুব শূন্য *src, uint64_t src_size,
                          শূন্য **buffer,
                          ধ্রুব শূন্য *prefix, size_t prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

পূর্ণাঙ্ক compress_stream(Compression type, পূর্ণাঙ্ক fdf, পূর্ণাঙ্ক fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_size);
প্রকারনির্দেশ গণনা DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

পূর্ণাঙ্ক decompress_stream_full(Compression type, পূর্ণাঙ্ক fdf, পূর্ণাঙ্ক fdt, uint64_t max_bytes, DecompressFlags flags);
স্থির অন্তঃস্থ পূর্ণাঙ্ক decompress_stream(Compression type, পূর্ণাঙ্ক fdf, পূর্ণাঙ্ক fdt, uint64_t max_bytes) {
        প্রত্যাগম decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
পূর্ণাঙ্ক decompress_stream_by_filename(ধ্রুব বর্ণ *filename, পূর্ণাঙ্ক fdf, পূর্ণাঙ্ক fdt, uint64_t max_bytes);

পূর্ণাঙ্ক dlopen_xz(পূর্ণাঙ্ক log_level) _dlopen_loader_;
পূর্ণাঙ্ক dlopen_lz4(পূর্ণাঙ্ক log_level) _dlopen_loader_;
পূর্ণাঙ্ক dlopen_zstd(পূর্ণাঙ্ক log_level) _dlopen_loader_;
পূর্ণাঙ্ক dlopen_zlib(পূর্ণাঙ্ক log_level) _dlopen_loader_;
পূর্ণাঙ্ক dlopen_bzip2(পূর্ণাঙ্ক log_level) _dlopen_loader_;

পূর্ণাঙ্ক dlopen_compress(Compression c, পূর্ণাঙ্ক log_level);
পূর্ণাঙ্ক dlopen_compress_journal(Compression c, পূর্ণাঙ্ক log_level);
স্থির অন্তঃস্থ পূর্ণাঙ্ক dlopen_compress_default(পূর্ণাঙ্ক log_level) {
        প্রত্যাগম dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

স্থির অন্তঃস্থ ধ্রুব বর্ণ* default_compression_extension(শূন্য) {
        প্রত্যাগম compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_out_size(শূন্য);
