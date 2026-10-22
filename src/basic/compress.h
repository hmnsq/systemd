/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "dlopen-note.h"

வகைவரை எண்ணிக்கை Compression {
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
Compression compression_from_string_harder(மாறிலி எழுத்து *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(மாறிலி எழுத்து *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_BUFFER_SIZE (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(மாறிலி uint8_t data[நிலையான COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

வகைவரை கட்டமைப்பு Compressor Compressor;
வகைவரை Compressor Decompressor;

வகைவரை முழு (*DecompressorCallback)(மாறிலி வெறுமை *data, size_t size, வெறுமை *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

முழு compressor_new(Compressor **ret, Compression type);
முழு compressor_start(Compressor *c, மாறிலி வெறுமை *data, size_t size, வெறுமை **buffer, size_t *buffer_size, size_t *buffer_allocated);
முழு compressor_finish(Compressor *c, வெறுமை **buffer, size_t *buffer_size, size_t *buffer_allocated);

முழு decompressor_detect(Decompressor **decompressor, மாறிலி வெறுமை *data, size_t size);
முழு decompressor_force(Decompressor **decompressor, Compression type);
முழு decompressor_force_off(Decompressor **decompressor);
முழு decompressor_தள்ளு(Decompressor *c, மாறிலி வெறுமை *data, size_t size, DecompressorCallback callback, வெறுமை *userdata);

Compression compressor_type(மாறிலி Compressor *c);

/* Blob compression/decompression */

முழு compress_blob_journal(
                Compression compression,
                மாறிலி வெறுமை *src, uint64_t src_size,
                வெறுமை *dst, size_t dst_alloc_size, size_t *dst_size, முழு level);
முழு compress_blob(Compression compression,
                  மாறிலி வெறுமை *src, uint64_t src_size,
                  வெறுமை *dst, size_t dst_alloc_size, size_t *dst_size, முழு level);
முழு decompress_blob_journal(
                Compression compression,
                மாறிலி வெறுமை *src, uint64_t src_size,
                வெறுமை **dst, size_t *dst_size, size_t dst_max);
முழு decompress_blob(Compression compression,
                    மாறிலி வெறுமை *src, uint64_t src_size,
                    வெறுமை **dst, size_t *dst_size, size_t dst_max);

முழு decompress_blob_to_fd_journal(
                Compression type,
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                முழு fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);
முழு decompress_blob_to_fd(
                Compression type,
                மாறிலி வெறுமை *src,
                uint64_t src_size,
                முழு fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);

முழு decompress_zlib_raw(மாறிலி வெறுமை *src, uint64_t src_size,
                        வெறுமை *dst, size_t dst_size, முழு wbits);

முழு decompress_startswith_journal(
                Compression compression,
                மாறிலி வெறுமை *src, uint64_t src_size,
                வெறுமை **buffer,
                மாறிலி வெறுமை *prefix, size_t prefix_len,
                uint8_t extra);
முழு decompress_startswith(Compression compression,
                          மாறிலி வெறுமை *src, uint64_t src_size,
                          வெறுமை **buffer,
                          மாறிலி வெறுமை *prefix, size_t prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

முழு compress_stream(Compression type, முழு fdf, முழு fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_size);
வகைவரை எண்ணிக்கை DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

முழு decompress_stream_full(Compression type, முழு fdf, முழு fdt, uint64_t max_bytes, DecompressFlags flags);
நிலையான உள்வரி முழு decompress_stream(Compression type, முழு fdf, முழு fdt, uint64_t max_bytes) {
        திரும்பு decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
முழு decompress_stream_by_filename(மாறிலி எழுத்து *filename, முழு fdf, முழு fdt, uint64_t max_bytes);

முழு dlopen_xz(முழு log_level) _dlopen_loader_;
முழு dlopen_lz4(முழு log_level) _dlopen_loader_;
முழு dlopen_zstd(முழு log_level) _dlopen_loader_;
முழு dlopen_zlib(முழு log_level) _dlopen_loader_;
முழு dlopen_bzip2(முழு log_level) _dlopen_loader_;

முழு dlopen_compress(Compression c, முழு log_level);
முழு dlopen_compress_journal(Compression c, முழு log_level);
நிலையான உள்வரி முழு dlopen_compress_default(முழு log_level) {
        திரும்பு dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

நிலையான உள்வரி மாறிலி எழுத்து* default_compression_extension(வெறுமை) {
        திரும்பு compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_out_size(வெறுமை);
