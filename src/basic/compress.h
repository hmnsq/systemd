/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "dlopen-note.h"

प्रकार क्रमागत Compression {
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
Compression compression_from_string_harder(स्थिर अक्षर *s);

/* Derives the compression type from a filename's extension, defaulting to COMPRESSION_NONE if the
 * filename does not carry a recognized compression suffix. */
Compression compression_from_filename(स्थिर अक्षर *filename);

bool compression_supported_journal(Compression c);
bool compression_supported(Compression c);

/* Buffer size used by streaming compression APIs and pipeline stages that feed into them. Sized to
 * match the typical Linux pipe buffer so that pipeline stages don't lose throughput due to small
 * intermediate buffers. */
#घोषणा COMPRESS_PIPE_बफर_आकार (128U*1024U)

#घोषणा COMPRESSION_MAGIC_BYTES_MAX 6U
Compression compression_detect_from_magic(स्थिर uint8_t तथ्य[अटल COMPRESSION_MAGIC_BYTES_MAX]);

/* Compressor / Decompressor — opaque push-based streaming compression context */

प्रकार काष्ठा Compressor Compressor;
प्रकार Compressor Decompressor;

प्रकार पूर्णांक (*DecompressorCallback)(स्थिर व्योम *तथ्य, माप_प्रकार आकार, व्योम *userdata);

Compressor* compressor_free(Compressor *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(Compressor*, compressor_free);

पूर्णांक compressor_new(Compressor **ret, Compression type);
पूर्णांक compressor_start(Compressor *c, स्थिर व्योम *तथ्य, माप_प्रकार आकार, व्योम **बफर, माप_प्रकार *बफर_आकार, माप_प्रकार *बफर_allocated);
पूर्णांक compressor_finish(Compressor *c, व्योम **बफर, माप_प्रकार *बफर_आकार, माप_प्रकार *बफर_allocated);

पूर्णांक decompressor_detect(Decompressor **decompressor, स्थिर व्योम *तथ्य, माप_प्रकार आकार);
पूर्णांक decompressor_force(Decompressor **decompressor, Compression type);
पूर्णांक decompressor_force_off(Decompressor **decompressor);
पूर्णांक decompressor_धकेलो(Decompressor *c, स्थिर व्योम *तथ्य, माप_प्रकार आकार, DecompressorCallback callback, व्योम *userdata);

Compression compressor_type(स्थिर Compressor *c);

/* Blob compression/decompression */

पूर्णांक compress_blob_journal(
                Compression compression,
                स्थिर व्योम *src, uint64_t src_आकार,
                व्योम *dst, माप_प्रकार dst_alloc_आकार, माप_प्रकार *dst_आकार, पूर्णांक level);
पूर्णांक compress_blob(Compression compression,
                  स्थिर व्योम *src, uint64_t src_आकार,
                  व्योम *dst, माप_प्रकार dst_alloc_आकार, माप_प्रकार *dst_आकार, पूर्णांक level);
पूर्णांक decompress_blob_journal(
                Compression compression,
                स्थिर व्योम *src, uint64_t src_आकार,
                व्योम **dst, माप_प्रकार *dst_आकार, माप_प्रकार dst_max);
पूर्णांक decompress_blob(Compression compression,
                    स्थिर व्योम *src, uint64_t src_आकार,
                    व्योम **dst, माप_प्रकार *dst_आकार, माप_प्रकार dst_max);

पूर्णांक decompress_blob_to_fd_journal(
                Compression type,
                स्थिर व्योम *src,
                uint64_t src_आकार,
                पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_आकार);
पूर्णांक decompress_blob_to_fd(
                Compression type,
                स्थिर व्योम *src,
                uint64_t src_आकार,
                पूर्णांक fdt,
                uint64_t max_bytes,
                uint64_t skip,
                माप_प्रकार *ret_आकार);

पूर्णांक decompress_zlib_raw(स्थिर व्योम *src, uint64_t src_आकार,
                        व्योम *dst, माप_प्रकार dst_आकार, पूर्णांक wbits);

पूर्णांक decompress_startswith_journal(
                Compression compression,
                स्थिर व्योम *src, uint64_t src_आकार,
                व्योम **बफर,
                स्थिर व्योम *prefix, माप_प्रकार prefix_len,
                uint8_t extra);
पूर्णांक decompress_startswith(Compression compression,
                          स्थिर व्योम *src, uint64_t src_आकार,
                          व्योम **बफर,
                          स्थिर व्योम *prefix, माप_प्रकार prefix_len,
                          uint8_t extra);

/* Stream compression/decompression (fd-to-fd) */

पूर्णांक compress_stream(Compression type, पूर्णांक fdf, पूर्णांक fdt, uint64_t max_bytes, uint64_t *ret_uncompressed_आकार);
प्रकार क्रमागत DecompressFlags {
        DECOMPRESS_LEGACY_LZMA         = 1 << 0, /* With COMPRESSION_XZ, also accept the legacy .lzma format */
        DECOMPRESS_REFUSE_CONCATENATED = 1 << 1, /* With COMPRESSION_XZ, refuse the second and later stream */
} DecompressFlags;

पूर्णांक decompress_stream_full(Compression type, पूर्णांक fdf, पूर्णांक fdt, uint64_t max_bytes, DecompressFlags flags);
अटल अंतरभूत पूर्णांक decompress_stream(Compression type, पूर्णांक fdf, पूर्णांक fdt, uint64_t max_bytes) {
        वापस decompress_stream_full(type, fdf, fdt, max_bytes, /* flags= */ 0);
}
पूर्णांक decompress_stream_by_filename(स्थिर अक्षर *filename, पूर्णांक fdf, पूर्णांक fdt, uint64_t max_bytes);

पूर्णांक dlopen_xz(पूर्णांक log_level) _dlopen_loader_;
पूर्णांक dlopen_lz4(पूर्णांक log_level) _dlopen_loader_;
पूर्णांक dlopen_zstd(पूर्णांक log_level) _dlopen_loader_;
पूर्णांक dlopen_zlib(पूर्णांक log_level) _dlopen_loader_;
पूर्णांक dlopen_bzip2(पूर्णांक log_level) _dlopen_loader_;

पूर्णांक dlopen_compress(Compression c, पूर्णांक log_level);
पूर्णांक dlopen_compress_journal(Compression c, पूर्णांक log_level);
अटल अंतरभूत पूर्णांक dlopen_compress_default(पूर्णांक log_level) {
        वापस dlopen_compress_journal(DEFAULT_COMPRESSION, log_level);
}

अटल अंतरभूत स्थिर अक्षर* default_compression_extension(व्योम) {
        वापस compression_extension_to_string(DEFAULT_COMPRESSION) ?: "";
}

/* Returns ZSTD_DStreamOutSize() — the streaming decompressor's recommended output buffer size, which
 * decompress_blob() uses to size its initial buffer — or 0 if libzstd is unavailable. Exposed so tests
 * can line content up with the decoder's buffer boundary instead of hardcoding the value. */
माप_प्रकार zstd_dstream_out_आकार(व्योम);
