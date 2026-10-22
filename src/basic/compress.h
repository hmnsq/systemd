/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "dlopen-note.h"

തരനിർവചനം എണ്ണൽ Compression {
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
Compression compression_from_string_harder(സ്ഥിരം അക്ഷരം *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(സ്ഥിരം അക്ഷരം *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_BUFFER_SIZE (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(സ്ഥിരം uint8_t data[സ്ഥിതം COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

തരനിർവചനം ഘടന Compressor Compressor;
തരനിർവചനം Compressor Decompressor;

തരനിർവചനം പൂർണ്ണസംഖ്യ (*DecompressorCallback)(സ്ഥിരം ശൂന്യം *data, size_t size, ശൂന്യം *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

പൂർണ്ണസംഖ്യ compressor_new(Compressor **ret, Compression type);
പൂർണ്ണസംഖ്യ compressor_start(Compressor *c, സ്ഥിരം ശൂന്യം *data, size_t size, ശൂന്യം **buffer, size_t *buffer_size, size_t *buffer_allocated);
പൂർണ്ണസംഖ്യ compressor_finish(Compressor *c, ശൂന്യം **buffer, size_t *buffer_size, size_t *buffer_allocated);

പൂർണ്ണസംഖ്യ decompressor_detect(Decompressor **decompressor, സ്ഥിരം ശൂന്യം *data, size_t size);
പൂർണ്ണസംഖ്യ decompressor_force(Decompressor **decompressor, Compression type);
പൂർണ്ണസംഖ്യ decompressor_force_off(Decompressor **decompressor);
പൂർണ്ണസംഖ്യ decompressor_തള്ളുക(Decompressor *c, സ്ഥിരം ശൂന്യം *data, size_t size, DecompressorCallback callback, ശൂന്യം *userdata);

Compression compressor_type(സ്ഥിരം Compressor *c);

/* Blob compression/decompression */

പൂർണ്ണസംഖ്യ compress_blob_journal(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                ശൂന്യം *dst, size_t dst_alloc_size, size_t *dst_size, പൂർണ്ണസംഖ്യ level);
പൂർണ്ണസംഖ്യ compress_blob(Compression compression,
                  സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                  ശൂന്യം *dst, size_t dst_alloc_size, size_t *dst_size, പൂർണ്ണസംഖ്യ level);
പൂർണ്ണസംഖ്യ decompress_blob_journal(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                ശൂന്യം **dst, size_t *dst_size, size_t dst_max);
പൂർണ്ണസംഖ്യ decompress_blob(Compression compression,
                    സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                    ശൂന്യം **dst, size_t *dst_size, size_t dst_max);

പൂർണ്ണസംഖ്യ decompress_blob_to_fd_journal(
                Compression type,
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                പൂർണ്ണസംഖ്യ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);
പൂർണ്ണസംഖ്യ decompress_blob_to_fd(
                Compression type,
                സ്ഥിരം ശൂന്യം *src,
                uint64_t src_size,
                പൂർണ്ണസംഖ്യ fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);

പൂർണ്ണസംഖ്യ decompress_zlib_raw(സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                        ശൂന്യം *dst, size_t dst_size, പൂർണ്ണസംഖ്യ wbits);

പൂർണ്ണസംഖ്യ decompress_startswith_journal(
                Compression compression,
                സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                ശൂന്യം **buffer,
                സ്ഥിരം ശൂന്യം *prefix, size_t prefix_len,
                uint8_t extra);
പൂർണ്ണസംഖ്യ decompress_startswith(Compression compression,
                          സ്ഥിരം ശൂന്യം *src, uint64_t src_size,
                          ശൂന്യം **buffer,
                          സ്ഥിരം ശൂന്യം *prefix, size_t prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

പൂർണ്ണസംഖ്യ compress_stream(Compression type, പൂർണ്ണസംഖ്യ fdf, പൂർണ്ണസംഖ്യ fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_size);
തരനിർവചനം എണ്ണൽ DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

പൂർണ്ണസംഖ്യ decompress_stream_full(Compression type, പൂർണ്ണസംഖ്യ fdf, പൂർണ്ണസംഖ്യ fdt, uint64_t max_bytes, DecompressFlags flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ decompress_stream(Compression type, പൂർണ്ണസംഖ്യ fdf, പൂർണ്ണസംഖ്യ fdt, uint64_t max_bytes) {
        തിരികെ decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
പൂർണ്ണസംഖ്യ decompress_stream_by_filename(സ്ഥിരം അക്ഷരം *filename, പൂർണ്ണസംഖ്യ fdf, പൂർണ്ണസംഖ്യ fdt, uint64_t max_bytes);

പൂർണ്ണസംഖ്യ dlopen_xz(പൂർണ്ണസംഖ്യ log_level) _dlopen_loader_;
പൂർണ്ണസംഖ്യ dlopen_lz4(പൂർണ്ണസംഖ്യ log_level) _dlopen_loader_;
പൂർണ്ണസംഖ്യ dlopen_zstd(പൂർണ്ണസംഖ്യ log_level) _dlopen_loader_;
പൂർണ്ണസംഖ്യ dlopen_zlib(പൂർണ്ണസംഖ്യ log_level) _dlopen_loader_;
പൂർണ്ണസംഖ്യ dlopen_bzip2(പൂർണ്ണസംഖ്യ log_level) _dlopen_loader_;

പൂർണ്ണസംഖ്യ dlopen_compress(Compression c, പൂർണ്ണസംഖ്യ log_level);
പൂർണ്ണസംഖ്യ dlopen_compress_journal(Compression c, പൂർണ്ണസംഖ്യ log_level);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ dlopen_compress_default(പൂർണ്ണസംഖ്യ log_level) {
        തിരികെ dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* default_compression_extension(ശൂന്യം) {
        തിരികെ compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_out_size(ശൂന്യം);
