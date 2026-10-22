/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"
#شامل "dlopen-note.h"

تعریفنوع شمارش Compression {
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
Compression compression_from_string_harder(ثابت نویسه *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ثابت نویسه *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_BUFFER_SIZE (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(ثابت uint8_t data[ایستا COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

تعریفنوع ساختار Compressor Compressor;
تعریفنوع Compressor Decompressor;

تعریفنوع صحیح (*DecompressorCallback)(ثابت تهی *data, size_t size, تهی *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

صحیح compressor_new(Compressor **ret, Compression type);
صحیح compressor_start(Compressor *c, ثابت تهی *data, size_t size, تهی **buffer, size_t *buffer_size, size_t *buffer_allocated);
صحیح compressor_finish(Compressor *c, تهی **buffer, size_t *buffer_size, size_t *buffer_allocated);

صحیح decompressor_detect(Decompressor **decompressor, ثابت تهی *data, size_t size);
صحیح decompressor_force(Decompressor **decompressor, Compression type);
صحیح decompressor_force_off(Decompressor **decompressor);
صحیح decompressor_افزودن(Decompressor *c, ثابت تهی *data, size_t size, DecompressorCallback callback, تهی *userdata);

Compression compressor_type(ثابت Compressor *c);

/* Blob compression/decompression */

صحیح compress_blob_journal(
                Compression compression,
                ثابت تهی *src, uint64_t src_size,
                تهی *dst, size_t dst_alloc_size, size_t *dst_size, صحیح level);
صحیح compress_blob(Compression compression,
                  ثابت تهی *src, uint64_t src_size,
                  تهی *dst, size_t dst_alloc_size, size_t *dst_size, صحیح level);
صحیح decompress_blob_journal(
                Compression compression,
                ثابت تهی *src, uint64_t src_size,
                تهی **dst, size_t *dst_size, size_t dst_max);
صحیح decompress_blob(Compression compression,
                    ثابت تهی *src, uint64_t src_size,
                    تهی **dst, size_t *dst_size, size_t dst_max);

صحیح decompress_blob_to_fd_journal(
                Compression type,
                ثابت تهی *src,
                uint64_t src_size,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);
صحیح decompress_blob_to_fd(
                Compression type,
                ثابت تهی *src,
                uint64_t src_size,
                صحیح fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);

صحیح decompress_zlib_raw(ثابت تهی *src, uint64_t src_size,
                        تهی *dst, size_t dst_size, صحیح wbits);

صحیح decompress_startswith_journal(
                Compression compression,
                ثابت تهی *src, uint64_t src_size,
                تهی **buffer,
                ثابت تهی *prefix, size_t prefix_len,
                uint8_t extra);
صحیح decompress_startswith(Compression compression,
                          ثابت تهی *src, uint64_t src_size,
                          تهی **buffer,
                          ثابت تهی *prefix, size_t prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

صحیح compress_stream(Compression type, صحیح fdf, صحیح fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_size);
تعریفنوع شمارش DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

صحیح decompress_stream_full(Compression type, صحیح fdf, صحیح fdt, uint64_t max_bytes, DecompressFlags flags);
ایستا درونخطی صحیح decompress_stream(Compression type, صحیح fdf, صحیح fdt, uint64_t max_bytes) {
        بازگشت decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
صحیح decompress_stream_by_filename(ثابت نویسه *filename, صحیح fdf, صحیح fdt, uint64_t max_bytes);

صحیح dlopen_xz(صحیح log_level) _dlopen_loader_;
صحیح dlopen_lz4(صحیح log_level) _dlopen_loader_;
صحیح dlopen_zstd(صحیح log_level) _dlopen_loader_;
صحیح dlopen_zlib(صحیح log_level) _dlopen_loader_;
صحیح dlopen_bzip2(صحیح log_level) _dlopen_loader_;

صحیح dlopen_compress(Compression c, صحیح log_level);
صحیح dlopen_compress_journal(Compression c, صحیح log_level);
ایستا درونخطی صحیح dlopen_compress_default(صحیح log_level) {
        بازگشت dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

ایستا درونخطی ثابت نویسه* default_compression_extension(تهی) {
        بازگشت compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_out_size(تهی);
