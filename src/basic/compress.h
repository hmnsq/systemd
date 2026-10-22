/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "dlopen-note.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ Compression {
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
Compression compression_from_string_harder(ಧ್ರುವ ವರ್ಣ *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ಧ್ರುವ ವರ್ಣ *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_BUFFER_SIZE (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(ಧ್ರುವ uint8_t data[ಸ್ಥಿರ COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Compressor Compressor;
ಪ್ರಕಾರನಿರ್ದೇಶ Compressor Decompressor;

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*DecompressorCallback)(ಧ್ರುವ ಶೂನ್ಯ *data, size_t size, ಶೂನ್ಯ *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

ಪೂರ್ಣಾಙ್ಕ compressor_new(Compressor **ret, Compression type);
ಪೂರ್ಣಾಙ್ಕ compressor_start(Compressor *c, ಧ್ರುವ ಶೂನ್ಯ *data, size_t size, ಶೂನ್ಯ **buffer, size_t *buffer_size, size_t *buffer_allocated);
ಪೂರ್ಣಾಙ್ಕ compressor_finish(Compressor *c, ಶೂನ್ಯ **buffer, size_t *buffer_size, size_t *buffer_allocated);

ಪೂರ್ಣಾಙ್ಕ decompressor_detect(Decompressor **decompressor, ಧ್ರುವ ಶೂನ್ಯ *data, size_t size);
ಪೂರ್ಣಾಙ್ಕ decompressor_force(Decompressor **decompressor, Compression type);
ಪೂರ್ಣಾಙ್ಕ decompressor_force_off(Decompressor **decompressor);
ಪೂರ್ಣಾಙ್ಕ decompressor_ತಳ್ಳು(Decompressor *c, ಧ್ರುವ ಶೂನ್ಯ *data, size_t size, DecompressorCallback callback, ಶೂನ್ಯ *userdata);

Compression compressor_type(ಧ್ರುವ Compressor *c);

/* Blob compression/decompression */

ಪೂರ್ಣಾಙ್ಕ compress_blob_journal(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                ಶೂನ್ಯ *dst, size_t dst_alloc_size, size_t *dst_size, ಪೂರ್ಣಾಙ್ಕ level);
ಪೂರ್ಣಾಙ್ಕ compress_blob(Compression compression,
                  ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                  ಶೂನ್ಯ *dst, size_t dst_alloc_size, size_t *dst_size, ಪೂರ್ಣಾಙ್ಕ level);
ಪೂರ್ಣಾಙ್ಕ decompress_blob_journal(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                ಶೂನ್ಯ **dst, size_t *dst_size, size_t dst_max);
ಪೂರ್ಣಾಙ್ಕ decompress_blob(Compression compression,
                    ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                    ಶೂನ್ಯ **dst, size_t *dst_size, size_t dst_max);

ಪೂರ್ಣಾಙ್ಕ decompress_blob_to_fd_journal(
                Compression type,
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಪೂರ್ಣಾಙ್ಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);
ಪೂರ್ಣಾಙ್ಕ decompress_blob_to_fd(
                Compression type,
                ಧ್ರುವ ಶೂನ್ಯ *src,
                uint64_t src_size,
                ಪೂರ್ಣಾಙ್ಕ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);

ಪೂರ್ಣಾಙ್ಕ decompress_zlib_raw(ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                        ಶೂನ್ಯ *dst, size_t dst_size, ಪೂರ್ಣಾಙ್ಕ wbits);

ಪೂರ್ಣಾಙ್ಕ decompress_startswith_journal(
                Compression compression,
                ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                ಶೂನ್ಯ **buffer,
                ಧ್ರುವ ಶೂನ್ಯ *prefix, size_t prefix_len,
                uint8_t extra);
ಪೂರ್ಣಾಙ್ಕ decompress_startswith(Compression compression,
                          ಧ್ರುವ ಶೂನ್ಯ *src, uint64_t src_size,
                          ಶೂನ್ಯ **buffer,
                          ಧ್ರುವ ಶೂನ್ಯ *prefix, size_t prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

ಪೂರ್ಣಾಙ್ಕ compress_stream(Compression type, ಪೂರ್ಣಾಙ್ಕ fdf, ಪೂರ್ಣಾಙ್ಕ fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_size);
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

ಪೂರ್ಣಾಙ್ಕ decompress_stream_full(Compression type, ಪೂರ್ಣಾಙ್ಕ fdf, ಪೂರ್ಣಾಙ್ಕ fdt, uint64_t max_bytes, DecompressFlags flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ decompress_stream(Compression type, ಪೂರ್ಣಾಙ್ಕ fdf, ಪೂರ್ಣಾಙ್ಕ fdt, uint64_t max_bytes) {
        ಪ್ರತ್ಯಾಗಮ decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
ಪೂರ್ಣಾಙ್ಕ decompress_stream_by_filename(ಧ್ರುವ ವರ್ಣ *filename, ಪೂರ್ಣಾಙ್ಕ fdf, ಪೂರ್ಣಾಙ್ಕ fdt, uint64_t max_bytes);

ಪೂರ್ಣಾಙ್ಕ dlopen_xz(ಪೂರ್ಣಾಙ್ಕ log_level) _dlopen_loader_;
ಪೂರ್ಣಾಙ್ಕ dlopen_lz4(ಪೂರ್ಣಾಙ್ಕ log_level) _dlopen_loader_;
ಪೂರ್ಣಾಙ್ಕ dlopen_zstd(ಪೂರ್ಣಾಙ್ಕ log_level) _dlopen_loader_;
ಪೂರ್ಣಾಙ್ಕ dlopen_zlib(ಪೂರ್ಣಾಙ್ಕ log_level) _dlopen_loader_;
ಪೂರ್ಣಾಙ್ಕ dlopen_bzip2(ಪೂರ್ಣಾಙ್ಕ log_level) _dlopen_loader_;

ಪೂರ್ಣಾಙ್ಕ dlopen_compress(Compression c, ಪೂರ್ಣಾಙ್ಕ log_level);
ಪೂರ್ಣಾಙ್ಕ dlopen_compress_journal(Compression c, ಪೂರ್ಣಾಙ್ಕ log_level);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ dlopen_compress_default(ಪೂರ್ಣಾಙ್ಕ log_level) {
        ಪ್ರತ್ಯಾಗಮ dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* default_compression_extension(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_out_size(ಶೂನ್ಯ);
