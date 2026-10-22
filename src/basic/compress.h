/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "dlopen-note.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା Compression {
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
Compression compression_from_string_harder(ଅଚଳ ଅକ୍ଷର *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(ଅଚଳ ଅକ୍ଷର *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#define COMPRESS_PIPE_BUFFER_SIZE (128U*1024U)

#define COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(ଅଚଳ uint8_t data[ସ୍ଥିର COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Compressor Compressor;
ପ୍ରକାରବ୍ୟାଖ୍ୟା Compressor Decompressor;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*DecompressorCallback)(ଅଚଳ ଶୂନ୍ୟ *data, size_t size, ଶୂନ୍ୟ *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressor_new(Compressor **ret, Compression type);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressor_start(Compressor *c, ଅଚଳ ଶୂନ୍ୟ *data, size_t size, ଶୂନ୍ୟ **buffer, size_t *buffer_size, size_t *buffer_allocated);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା compressor_finish(Compressor *c, ଶୂନ୍ୟ **buffer, size_t *buffer_size, size_t *buffer_allocated);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_detect(Decompressor **decompressor, ଅଚଳ ଶୂନ୍ୟ *data, size_t size);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_force(Decompressor **decompressor, Compression type);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_force_off(Decompressor **decompressor);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompressor_ଠେଲ(Decompressor *c, ଅଚଳ ଶୂନ୍ୟ *data, size_t size, DecompressorCallback callback, ଶୂନ୍ୟ *userdata);

Compression compressor_type(ଅଚଳ Compressor *c);

/* Blob compression/decompression */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob_journal(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                ଶୂନ୍ୟ *dst, size_t dst_alloc_size, size_t *dst_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_blob(Compression compression,
                  ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                  ଶୂନ୍ୟ *dst, size_t dst_alloc_size, size_t *dst_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_journal(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                ଶୂନ୍ୟ **dst, size_t *dst_size, size_t dst_max);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob(Compression compression,
                    ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                    ଶୂନ୍ୟ **dst, size_t *dst_size, size_t dst_max);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_to_fd_journal(
                Compression type,
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_blob_to_fd(
                Compression type,
                ଅଚଳ ଶୂନ୍ୟ *src,
                uint64_t src_size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt,
                uint64_t max_bytes,
                uint64_t skip,
                size_t *ret_size);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_zlib_raw(ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                        ଶୂନ୍ୟ *dst, size_t dst_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା wbits);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith_journal(
                Compression compression,
                ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                ଶୂନ୍ୟ **buffer,
                ଅଚଳ ଶୂନ୍ୟ *prefix, size_t prefix_len,
                uint8_t extra);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_startswith(Compression compression,
                          ଅଚଳ ଶୂନ୍ୟ *src, uint64_t src_size,
                          ଶୂନ୍ୟ **buffer,
                          ଅଚଳ ଶୂନ୍ୟ *prefix, size_t prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା compress_stream(Compression type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_size);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_stream_full(Compression type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt, uint64_t max_bytes, DecompressFlags flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_stream(Compression type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt, uint64_t max_bytes) {
        ଫେରନ୍ତୁ decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା decompress_stream_by_filename(ଅଚଳ ଅକ୍ଷର *filename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt, uint64_t max_bytes);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_xz(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) _dlopen_loader_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_lz4(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) _dlopen_loader_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_zstd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) _dlopen_loader_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_zlib(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) _dlopen_loader_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_bzip2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) _dlopen_loader_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_compress(Compression c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_compress_journal(Compression c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_compress_default(ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
        ଫେରନ୍ତୁ dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* default_compression_extension(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
size_t zstd_dstream_out_size(ଶୂନ୍ୟ);
