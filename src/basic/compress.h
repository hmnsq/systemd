/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "dlopen-note.h"

प्रकारनिर्देश गणना Compression {
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
Compression compression_from_string_harder(ध्रुव वर्ण *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ध्रुव वर्ण *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_BUFFER_SIZE (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(ध्रुव uint8_t data[स्थिर COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

प्रकारनिर्देश संरचना Compressor Compressor;
प्रकारनिर्देश Compressor Decompressor;

प्रकारनिर्देश पूर्णाङ्क (*DecompressorCallback)(ध्रुव शून्य *data, माप_प्रकार size, शून्य *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

पूर्णाङ्क compressor_new(Compressor **ret, Compression type);
पूर्णाङ्क compressor_start(Compressor *c, ध्रुव शून्य *data, माप_प्रकार size, शून्य **buffer, माप_प्रकार *buffer_size, माप_प्रकार *buffer_allocated);
पूर्णाङ्क compressor_finish(Compressor *c, शून्य **buffer, माप_प्रकार *buffer_size, माप_प्रकार *buffer_allocated);

पूर्णाङ्क decompressor_detect(Decompressor **decompressor, ध्रुव शून्य *data, माप_प्रकार size);
पूर्णाङ्क decompressor_force(Decompressor **decompressor, Compression type);
पूर्णाङ्क decompressor_force_off(Decompressor **decompressor);
पूर्णाङ्क decompressor_धकेलो(Decompressor *c, ध्रुव शून्य *data, माप_प्रकार size, DecompressorCallback callback, शून्य *userdata);

Compression compressor_type(ध्रुव Compressor *c);

/* Blob compression/decompression */

पूर्णाङ्क compress_blob_journal(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_size,
                शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णाङ्क level);
पूर्णाङ्क compress_blob(Compression compression,
                  ध्रुव शून्य *src, uint64_t src_size,
                  शून्य *dst, माप_प्रकार dst_alloc_size, माप_प्रकार *dst_size, पूर्णाङ्क level);
पूर्णाङ्क decompress_blob_journal(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_size,
                शून्य **dst, माप_प्रकार *dst_size, माप_प्रकार dst_max);
पूर्णाङ्क decompress_blob(Compression compression,
                    ध्रुव शून्य *src, uint64_t src_size,
                    शून्य **dst, माप_प्रकार *dst_size, माप_प्रकार dst_max);

पूर्णाङ्क decompress_blob_to_fd_journal(
                Compression type,
                ध्रुव शून्य *src,
                uint64_t src_size,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_size);
पूर्णाङ्क decompress_blob_to_fd(
                Compression type,
                ध्रुव शून्य *src,
                uint64_t src_size,
                पूर्णाङ्क fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_size);

पूर्णाङ्क decompress_zlib_raw(ध्रुव शून्य *src, uint64_t src_size,
                        शून्य *dst, माप_प्रकार dst_size, पूर्णाङ्क wbits);

पूर्णाङ्क decompress_startswith_journal(
                Compression compression,
                ध्रुव शून्य *src, uint64_t src_size,
                शून्य **buffer,
                ध्रुव शून्य *prefix, माप_प्रकार prefix_len,
                uint8_t extra);
पूर्णाङ्क decompress_startswith(Compression compression,
                          ध्रुव शून्य *src, uint64_t src_size,
                          शून्य **buffer,
                          ध्रुव शून्य *prefix, माप_प्रकार prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

पूर्णाङ्क compress_stream(Compression type, पूर्णाङ्क fdf, पूर्णाङ्क fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_size);
प्रकारनिर्देश गणना DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

पूर्णाङ्क decompress_stream_full(Compression type, पूर्णाङ्क fdf, पूर्णाङ्क fdt, uint64_t max_bytes, DecompressFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क decompress_stream(Compression type, पूर्णाङ्क fdf, पूर्णाङ्क fdt, uint64_t max_bytes) {
        प्रत्यागम decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
पूर्णाङ्क decompress_stream_by_filename(ध्रुव वर्ण *filename, पूर्णाङ्क fdf, पूर्णाङ्क fdt, uint64_t max_bytes);

पूर्णाङ्क dlopen_xz(पूर्णाङ्क log_level) _dlopen_loader_;
पूर्णाङ्क dlopen_lz4(पूर्णाङ्क log_level) _dlopen_loader_;
पूर्णाङ्क dlopen_zstd(पूर्णाङ्क log_level) _dlopen_loader_;
पूर्णाङ्क dlopen_zlib(पूर्णाङ्क log_level) _dlopen_loader_;
पूर्णाङ्क dlopen_bzip2(पूर्णाङ्क log_level) _dlopen_loader_;

पूर्णाङ्क dlopen_compress(Compression c, पूर्णाङ्क log_level);
पूर्णाङ्क dlopen_compress_journal(Compression c, पूर्णाङ्क log_level);
स्थिर अन्तःस्थ पूर्णाङ्क dlopen_compress_default(पूर्णाङ्क log_level) {
        प्रत्यागम dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

स्थिर अन्तःस्थ ध्रुव वर्ण* default_compression_extension(शून्य) {
        प्रत्यागम compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
माप_प्रकार zstd_dstream_out_size(शून्य);
