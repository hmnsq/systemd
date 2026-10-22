/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "dlopen-note.h"

પ્રકારવ્યાખ્યા ગણના Compression {
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
Compression compression_from_string_harder(અચલ અક્ષર *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(અચલ અક્ષર *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_BUFFER_SIZE (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(અચલ uint8_t data[સ્થિર COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

પ્રકારવ્યાખ્યા સંરચના Compressor Compressor;
પ્રકારવ્યાખ્યા Compressor Decompressor;

પ્રકારવ્યાખ્યા પૂર્ણાંક (*DecompressorCallback)(અચલ શૂન્ય *data, size_t size, શૂન્ય *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

પૂર્ણાંક compressor_new(Compressor **ret, Compression type);
પૂર્ણાંક compressor_start(Compressor *c, અચલ શૂન્ય *data, size_t size, શૂન્ય **buffer, size_t *buffer_size, size_t *buffer_allocated);
પૂર્ણાંક compressor_finish(Compressor *c, શૂન્ય **buffer, size_t *buffer_size, size_t *buffer_allocated);

પૂર્ણાંક decompressor_detect(Decompressor **decompressor, અચલ શૂન્ય *data, size_t size);
પૂર્ણાંક decompressor_force(Decompressor **decompressor, Compression type);
પૂર્ણાંક decompressor_force_off(Decompressor **decompressor);
પૂર્ણાંક decompressor_ધકેલો(Decompressor *c, અચલ શૂન્ય *data, size_t size, DecompressorCallback callback, શૂન્ય *userdata);

Compression compressor_type(અચલ Compressor *c);

/* Blob compression/decompression */

પૂર્ણાંક compress_blob_journal(
                Compression compression,
                અચલ શૂન્ય *src, uint64_t src_size,
                શૂન્ય *dst, size_t dst_alloc_size, size_t *dst_size, પૂર્ણાંક level);
પૂર્ણાંક compress_blob(Compression compression,
                  અચલ શૂન્ય *src, uint64_t src_size,
                  શૂન્ય *dst, size_t dst_alloc_size, size_t *dst_size, પૂર્ણાંક level);
પૂર્ણાંક decompress_blob_journal(
                Compression compression,
                અચલ શૂન્ય *src, uint64_t src_size,
                શૂન્ય **dst, size_t *dst_size, size_t dst_max);
પૂર્ણાંક decompress_blob(Compression compression,
                    અચલ શૂન્ય *src, uint64_t src_size,
                    શૂન્ય **dst, size_t *dst_size, size_t dst_max);

પૂર્ણાંક decompress_blob_to_fd_journal(
                Compression type,
                અચલ શૂન્ય *src,
                uint64_t src_size,
                પૂર્ણાંક fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);
પૂર્ણાંક decompress_blob_to_fd(
                Compression type,
                અચલ શૂન્ય *src,
                uint64_t src_size,
                પૂર્ણાંક fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);

પૂર્ણાંક decompress_zlib_raw(અચલ શૂન્ય *src, uint64_t src_size,
                        શૂન્ય *dst, size_t dst_size, પૂર્ણાંક wbits);

પૂર્ણાંક decompress_startswith_journal(
                Compression compression,
                અચલ શૂન્ય *src, uint64_t src_size,
                શૂન્ય **buffer,
                અચલ શૂન્ય *prefix, size_t prefix_len,
                uint8_t extra);
પૂર્ણાંક decompress_startswith(Compression compression,
                          અચલ શૂન્ય *src, uint64_t src_size,
                          શૂન્ય **buffer,
                          અચલ શૂન્ય *prefix, size_t prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

પૂર્ણાંક compress_stream(Compression type, પૂર્ણાંક fdf, પૂર્ણાંક fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_size);
પ્રકારવ્યાખ્યા ગણના DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

પૂર્ણાંક decompress_stream_full(Compression type, પૂર્ણાંક fdf, પૂર્ણાંક fdt, uint64_t max_bytes, DecompressFlags flags);
સ્થિર અંતઃરેખ પૂર્ણાંક decompress_stream(Compression type, પૂર્ણાંક fdf, પૂર્ણાંક fdt, uint64_t max_bytes) {
        પાછા decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
પૂર્ણાંક decompress_stream_by_filename(અચલ અક્ષર *filename, પૂર્ણાંક fdf, પૂર્ણાંક fdt, uint64_t max_bytes);

પૂર્ણાંક dlopen_xz(પૂર્ણાંક log_level) _dlopen_loader_;
પૂર્ણાંક dlopen_lz4(પૂર્ણાંક log_level) _dlopen_loader_;
પૂર્ણાંક dlopen_zstd(પૂર્ણાંક log_level) _dlopen_loader_;
પૂર્ણાંક dlopen_zlib(પૂર્ણાંક log_level) _dlopen_loader_;
પૂર્ણાંક dlopen_bzip2(પૂર્ણાંક log_level) _dlopen_loader_;

પૂર્ણાંક dlopen_compress(Compression c, પૂર્ણાંક log_level);
પૂર્ણાંક dlopen_compress_journal(Compression c, પૂર્ણાંક log_level);
સ્થિર અંતઃરેખ પૂર્ણાંક dlopen_compress_default(પૂર્ણાંક log_level) {
        પાછા dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

સ્થિર અંતઃરેખ અચલ અક્ષર* default_compression_extension(શૂન્ય) {
        પાછા compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_out_size(શૂન્ય);
